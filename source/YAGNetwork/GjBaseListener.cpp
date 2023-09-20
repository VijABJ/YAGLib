
#include <process.h>
#include "GjBaseListener.h"
#include "GjTrace.h"
using namespace yaglib;
using namespace yaglib::sockets;

ListenerSocket::ListenerSocket(const int port, 
  const int maxConnections, const int maxWorkers) :
  Socket(), m_Port(port), m_ActiveConnections(0),
  m_MaxConnections(maxConnections), m_MaxWorkers(maxWorkers), 
  m_IOCP(new IOCP()), m_hSocketEvent(WSA_INVALID_EVENT), 
  m_hStopEvent(INVALID_HANDLE_VALUE)
{
  if(0 >= m_MaxWorkers)
  {
    SYSTEM_INFO si;
    ZeroMemory(&si, sizeof(SYSTEM_INFO));
	  GetSystemInfo(&si);
    m_MaxWorkers = si.dwNumberOfProcessors * 2;
  }
}

ListenerSocket::~ListenerSocket()
{
  stop();
  cleanupSocket();
  delete m_IOCP;
  if (INVALID_HANDLE_VALUE != m_hStopEvent) CloseHandle(m_hStopEvent);
}

bool ListenerSocket::allocateSocket()
{
  GTRACE(L"Listener: Creating socket");

	m_Socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);	
  if (INVALID_SOCKET == m_Socket)
  {
    GTRACE(L"Listener: Error creating socket (%ld)", WSAGetLastError());
    return false;
  }

  GTRACE(L"Listener: Creating socket event");
	m_hSocketEvent = WSACreateEvent();
	if (WSA_INVALID_EVENT == m_hSocketEvent)
  {
    GTRACE(L"Listener: Error creating socket event (%ld)", WSAGetLastError());
    cleanupSocket();
    return false;
  }

  GTRACE(L"Listener: Configuring socket for listening");
	if (SOCKET_ERROR == WSAEventSelect(m_Socket, m_hSocketEvent, FD_ACCEPT))
	{
    GTRACE(L"Listener: Error configuring socket (%ld)", WSAGetLastError());
    cleanupSocket();
    return false;
	}

  GTRACE(L"Listener: Binding socket to port %d", m_Port);

  SOCKADDR_IN sa;
  sa.sin_port = htons(m_Port);
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = INADDR_ANY;

  if (SOCKET_ERROR == bind(m_Socket, (LPSOCKADDR)&sa, sizeof(sockaddr)))
  {
    GTRACE(L"Listener: Error binding socket (%ld)", WSAGetLastError());
    cleanupSocket();
    return false;
  }

  GTRACE(L"Listener: Starting listen process");
  if (SOCKET_ERROR == listen(m_Socket, SOMAXCONN))
  {
    GTRACE(L"Listener: Error starting the listen process (%ld)", WSAGetLastError());
    cleanupSocket();
    return false;
  }

  return true;
}

void ListenerSocket::cleanupSocket()
{
  if (WSA_INVALID_EVENT != m_hSocketEvent)
  {
    WSACloseEvent(m_hSocketEvent);
    m_hSocketEvent = WSA_INVALID_EVENT;
  }
  if (INVALID_SOCKET != m_Socket)
  {
    closesocket(m_Socket);
    m_Socket = INVALID_SOCKET;
  }
}

void ListenerSocket::handleSocketAccept(SOCKET hClient)
{
  if(hClient == SOCKET_ERROR)
  {
    // ignore WSAEWOULDBLOCK errors, as there is no point in
    // reporting it since we will just loop again and wait anyway
    int err = WSAGetLastError();
    if(WSAEWOULDBLOCK != err)
    {
      GTRACE(L"handleSocketAccept: WSAAccept() failed %ld", err);
    }
    return;
  }

  // we have a connection! check if we're going to break the maximum
  // allowable connections
  if (m_ActiveConnections >= m_MaxConnections)
  {
    dropSocket(hClient);
    GTRACE(L"handleSocketAccept: Will exceed max connections, dropping new client");
    return;
  }

  // it's now ok to REALLY accept the newly connected client...
  acceptIncomingSocket(hClient);

}

bool ListenerSocket::start()
{
  if (m_AlreadyListening) 
    return true;

  if (!allocateSocket()) 
    return false;

  if (INVALID_HANDLE_VALUE == m_hStopEvent)
  {
    GTRACE(L"Listener: Allocating stop event for the listener thread");
    m_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  }

  // should I test this? if this fails, then Windows has just ran out
  // of handles at this point when nothing is happening yet!

  GTRACE(L"Listener: Setting up the listener thread");
  unsigned threadId = 0;
  HANDLE hThread = (HANDLE) _beginthreadex(
    NULL,                     // inherit our security
    0,                        // stack size, use default
    ServerListenerThread,     // the callback
    (void*) this,             // the thread parameter
    0,                        // init flag
    &threadId);               // could be useful

  m_AlreadyListening = (INVALID_HANDLE_VALUE != hThread);
  CloseHandle(hThread);

  if (!m_AlreadyListening)
  {
    GTRACE(L"Listener: failed to start the listening thread");
    cleanupSocket();
    m_hStopEvent = INVALID_HANDLE_VALUE;
  }
  else
  {
    GTRACE(L"Listener: Successfully started");
  }

  return m_AlreadyListening;
}

void ListenerSocket::stop()
{
  if (m_AlreadyListening)
  {
    GTRACE(L"Listener: stopping the listener");

    SetEvent(m_hStopEvent);
    Sleep(1000); // maybe one second is too short? investigate later...

    cleanupSocket();
    m_AlreadyListening = false;
  }
}

unsigned __stdcall ListenerSocket::ServerListenerThread(LPVOID lParam)
{
  ListenerSocket* _this = reinterpret_cast<ListenerSocket*>(lParam);
  if(!_this) return -1;

  // this section duplicate the handles required to make the main 
  // loop work.  previously, I just used assignments of the handle
  // values onto the array, but I walked thru the process in my
  // head and discovered that this could run into bugs if the main
  // thread closes the handle before this loop terminates! of course
  // this might be moot since I could simply alter the event check
  // to catch anything invalid (that is, NOT WAIT_TIMEOUT and the
  // property WAIT_OBJECT_0 values), but this would depend too much
  // on the termination sequence to be deterministic
  HANDLE waitList[2];
  HANDLE curThread = GetCurrentThread();
  //waitList[0] = _this->m_hStopEvent;
  DuplicateHandle(
    curThread, _this->m_hStopEvent, 
    curThread, &waitList[0],
    NULL, FALSE, DUPLICATE_SAME_ACCESS);
  //waitList[1] = _this->m_hSocketEvent;
  DuplicateHandle(
    curThread, _this->m_hSocketEvent, 
    curThread, &waitList[1],
    NULL, FALSE, DUPLICATE_SAME_ACCESS);

  // loop forever and check for accepts, and ONLY accepts!
  // oh, and check the termination event as well in case the
  // main thread asks us to...
  while(1)
  {
    // perform the wait on important events
    DWORD waitResult = WaitForMultipleObjects(2, &waitList[0], FALSE, INFINITE);

    // we're asked to quit, do so
    if(waitResult == WAIT_OBJECT_0) break;

    // ensure this is a valid result
    if(waitResult == (WAIT_OBJECT_0 + 1))
    {
		  // Figure out what happened
      WSANETWORKEVENTS netEvents;
		  int checkResult = WSAEnumNetworkEvents(
        _this->m_Socket, _this->m_hSocketEvent, &netEvents);

		  if (checkResult == SOCKET_ERROR)
		  {
			  GTRACE(L"ServerListenerThread: WSAEnumNetworkEvents() failed. %ld\n", WSAGetLastError());
			  break;
		  }

		  // since we set the socket to only trigger on accept event,
      // the event we can get is mostly accept.  but just check
      // to be sure...
      if(netEvents.lNetworkEvents & FD_ACCEPT)
      {
        if(netEvents.iErrorCode[FD_ACCEPT_BIT])
        {
          GTRACE(L"ServerListenerThread: not an accept event! %ld", WSAGetLastError());
          break;
        }

        // we're good! continue processing here...

        // accept the client connection, filter it though...
        SOCKET hClient = INVALID_SOCKET;
        SOCKADDR_IN saClient;
        int saLen = sizeof(SOCKADDR_IN);
        hClient = WSAAccept(_this->m_Socket, 
          (SOCKADDR*) &saClient, &saLen, 
          ConnectAcceptCondition, (DWORD) _this);

        // call the class handler for socket accepting
        _this->handleSocketAccept(hClient);

      }
    }
    else
    {
      GTRACE(L"ServerListenerThread: encountered a wait error. %ld", waitResult);
    }

	} // while....

  CloseHandle(waitList[0]);
  CloseHandle(waitList[1]);

  return 0;
}

int __stdcall ListenerSocket::ConnectAcceptCondition(
  IN LPWSABUF lpCallerId,
	IN LPWSABUF lpCallerData,
	IN OUT LPQOS lpSQOS,
	IN OUT LPQOS lpGQOS,
	IN LPWSABUF lpCalleeId, 
	OUT LPWSABUF lpCalleeData,
	OUT GROUP FAR *g,
	IN DWORD dwCallbackData)
{
  ListenerSocket* _this = reinterpret_cast<ListenerSocket*>(dwCallbackData);
  if(!_this) return CF_REJECT;


  /*
  some notes:

  > block connections from localhost, in final version of course,
    or maybe make it an option?
  > maintain a banned ip list, and check against it
  > maintain a list of connections and never allow more than N
    connections, where N is configurable.


  */

  return CF_ACCEPT; // accept it for now
}

inline bool ListenerSocket::isStarted()
{
  return m_AlreadyListening;
}

inline int ListenerSocket::getPort() 
{ 
  return m_Port; 
}

inline int ListenerSocket::getActiveConnections() 
{ 
  return m_ActiveConnections; 
}

inline int ListenerSocket::getMaxConnections() 
{ 
  return m_MaxConnections; 
}

inline int ListenerSocket::getMaxWorkers() 
{ 
  return m_MaxWorkers; 
}
