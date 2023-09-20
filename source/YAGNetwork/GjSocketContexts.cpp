
#include "GjSocketContexts.h"
#include "GjTrace.h"
#include "GjUnicodeUtils.h"
using namespace yaglib;
using namespace yaglib::sockets;

ContextManager::ContextManager() : 
  m_IOCP(new IOCP()), m_PacketFreeList(new PacketsAllocator())
{
  // must call this!
  //Winsock2::initResult();

  // create the worker threads.  the number depends on the number of 
  // processors in the system
  SYSTEM_INFO si;
	GetSystemInfo(&si);
	int maxThreads = si.dwNumberOfProcessors * 2;

  // delegate thread creation...
  createWorkerThreads(maxThreads);
}

ContextManager::~ContextManager()
{
  deleteWorkerThreads();
  delete m_IOCP;
  delete m_PacketFreeList;
}

void ContextManager::createWorkerThreads(int numThreads)
{
  // this loop create the thread objects.  note that thread objects
  // creates thread functions that are SUSPENDED initially! this is
  // useful now since we can resume them all at the same time later
  while(numThreads)
  {
    WorkerThread* wt = new WorkerThread();
    wt->setFreeOnTerminate(true);
    m_WorkerThreads.push_back(wt);
    --numThreads;
  }

  // all the threads are created, now resume them all...
  for(std::vector<WorkerThread*>::iterator iter = m_WorkerThreads.begin();
    iter != m_WorkerThreads.end(); iter++)
  {
    WorkerThread* wt = *iter;
    wt->resume();
  }
}

void ContextManager::deleteWorkerThreads()
{
  // post a termination notice to each running thread. wait
  // half a second in between notices just to be sure.
  for(std::vector<WorkerThread*>::iterator iter = m_WorkerThreads.begin();
    iter != m_WorkerThreads.end(); iter++)
  {
    m_IOCP->postStatus(NULL, 0, NULL);
    Sleep(500);
  }

  m_WorkerThreads.clear();
}

ContextManager::Context* 
  ContextManager::connect(const WideString& ipAddress, int port)
{
  SOCKET hClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(INVALID_SOCKET == hClient)
  {
    GTRACE(L"connect: cannot create socket. %ld", WSAGetLastError());
    return NULL;
  }

	SOCKADDR_IN saClient;
  memset(&saClient, 0, sizeof(saClient));

  saClient.sin_family = AF_INET;
  saClient.sin_addr.s_addr = inet_addr(UTF8String(ipAddress).c_str());
  saClient.sin_port = htons(port);
  int nRet = ::connect(hClient, (SOCKADDR*)&saClient, sizeof(SOCKADDR));

  if(SOCKET_ERROR == nRet)
  {
    GTRACE(L"connect: connect() function returned error %ld", WSAGetLastError());
    closesocket(hClient);
    return NULL;
  }

  // connection successful, create a context to remember this connection....
  return associateSocketWithContext(hClient);
}

ContextManager::Context* 
  ContextManager::associateSocketWithContext(SOCKET hClient)
{
  Context* context = new Context(this);
  context->init(hClient);

  return context;
}

void ContextManager::WorkerThread::main(PVOID startParam)
{
  // this is the worker thread
  ContextManager* cm = reinterpret_cast<ContextManager*>(startParam);
  IOCP* iocp = cm->getIOCP();

  // infinite loop that handles whatever comes in thru the completion port
  while(true)
  {
    Context* context = NULL;
    Overlapped* ol = NULL;
    DWORD dwIoSize = 0;
    DWORD dwBytesSent;
    //DWORD dwIoResult;

    // getStatus() will call GetQueuedCompletionStatus()
    int status = iocp->getStatus((LPDWORD) &context, &dwIoSize, (LPOVERLAPPED*) &ol);

    // check the returned value
    if((status == 0) && (context == NULL))
      break;

    // getStatus() succeeded, but the socket may have been closed, so detect that
    if((dwIoSize == 0) && (context == NULL) && (ol == NULL))
      break;

    PacketsThreadSafe* pendingList = context->getPendingList();

    switch(ol->m_Operation)
    {
    case soSending:
      // if the send operation was only partially completed, then
      // call WSASend again to resend the remaining data, after adjusting
      // the values of course...
      if(dwIoSize < ol->m_WSABuf.len)
      {
        ol->m_WSABuf.buf += dwIoSize;
        ol->m_WSABuf.len -= dwIoSize;
        WSASend(context->getSocket(), &(ol->m_WSABuf), 1, &dwBytesSent, MSG_PARTIAL, &ol->m_Overlapped, NULL);
      }
      else
      {
      // all data was sent, free the structures we allocated
        pendingList->remove(ol);
        cm->getFreeList()->add(ol);
      }
      break;

    case soNothing:
      // this is the initialization operation, convert it to a receive op
      ol->m_WSABuf.buf = (char*)&ol->m_Packet.m_Header;
      ol->m_WSABuf.len = sizeof(ol->m_Packet.m_Header.m_PacketSize);
	  /*dwIoResult = */WSARecv(context->getSocket(), &(ol->m_WSABuf), 1, &dwBytesSent, 0, &ol->m_Overlapped, NULL);
      break;

    case soReceivingPacketSize:
    case soReceivingPacketData:
      break;
    }

  }
}


ContextManager::Context::Context(ContextManager* owner) :
  m_Owner(owner), m_Socket(INVALID_SOCKET), m_MsgIdSequenceTracker(0),
  m_Pending(new PacketsThreadSafe())
{
}

ContextManager::Context::~Context()
{
  cleanup();
  delete m_Pending;
}

void ContextManager::Context::init(SOCKET hRemote)
{
  cleanup();
  m_Socket = hRemote;
  m_RemoteIp = Socket::peerAddress(hRemote);
  // associate this context with the completion port
  // maybe I should add error-checking later? 
  m_Owner->getIOCP()->associateSocket(hRemote, (DWORD) this);

  // create the initial queue posting in order to startup the worker thread
  Overlapped* ol = new Overlapped();
  memset(ol, 0, sizeof(Overlapped));
  ol->m_Operation = soNothing; // well, this is an init...
  m_Owner->getIOCP()->postStatus((DWORD) this, 0, &ol->m_Overlapped);

}

void ContextManager::Context::cleanup()
{
  if(INVALID_SOCKET != m_Socket)
  {
    Socket::dropSocket(m_Socket);
    m_Pending->clear();
    m_Socket = INVALID_SOCKET;
  }
}

Longint ContextManager::Context::getMessageId()
{
  m_CodeLock.enter();
  Longint result = ++m_MsgIdSequenceTracker;
  m_CodeLock.leave();
  return result;
}

void ContextManager::Context::send(void* buffer, Longint bufferSize)
{
  if((NULL == buffer) || (0 == bufferSize))
    return;

  Byte* data = (Byte*) buffer;
  Longint messageId = getMessageId();
  Longint packetIndex = 0;
  Longint packetCount = (bufferSize / MAX_DATA_SIZE) + ((bufferSize % MAX_DATA_SIZE) ? 1 : 0);
  Longint messageSize = bufferSize;
  Longint packetSize;

  while(bufferSize)
  {
    packetSize = (bufferSize <= MAX_DATA_SIZE) ? bufferSize : MAX_DATA_SIZE;

    Overlapped* ol = m_Owner->getFreeList()->allocate();

    Packet* packet = &ol->m_Packet;
    packet->m_Header.m_PacketSize = packetSize + HEADER_SIZE;
    packet->m_Header.m_PacketIndex = ++packetIndex;
    packet->m_Header.m_NumberOfPackets = packetCount;
    packet->m_Header.m_MessageId = messageId;
    packet->m_Header.m_SizeWhenComplete = messageSize;
    memcpy((void*) &packet->m_Data, (void*) data, packetSize);

    ol->m_WSABuf.buf = (char*) packet;
    ol->m_WSABuf.len = packet->m_Header.m_PacketSize;
    ol->m_Operation = soSending;

    DWORD dwBytesSent;
    m_Pending->add(ol);
    WSASend(m_Socket, &(ol->m_WSABuf), 1, &dwBytesSent, MSG_PARTIAL, &ol->m_Overlapped, NULL);

    data += packetSize;
    bufferSize -= packetSize;
  }
}

SOCKET ContextManager::Context::getSocket() 
{ 
  return m_Socket; 
}

const WideString& ContextManager::Context::getRemoteIp() 
{ 
  return m_RemoteIp; 
}

ContextManager* ContextManager::Context::getOwner() 
{ 
  return m_Owner; 
}

PacketsThreadSafe* ContextManager::Context::getPendingList() 
{ 
  return m_Pending; 
}

IOCP* ContextManager::getIOCP() 
{ 
  return m_IOCP; 
}

PacketsAllocator* ContextManager::getFreeList() 
{ 
  return m_PacketFreeList; 
}
