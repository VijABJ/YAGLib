
#if !defined(__GJ_BASE_LISTENER_HEADER__)
#define __GJ_BASE_LISTENER_HEADER__

#include "GjDefs.h"
#include <winsock2.h>
#include "GjSocketUtils.h"
#include "GjIOCP.h"

namespace yaglib
{
  namespace sockets
  {

const int DEFAULT_MAX_CONNECTIONS = 10000;

class ListenerSocket : public Socket
{
public:
  ListenerSocket(const int port, 
      const int maxConnections = DEFAULT_MAX_CONNECTIONS, 
      const int maxWorkers = 0);
  virtual ~ListenerSocket();

  bool isStarted();
  bool start();
  void stop();

  static unsigned __stdcall ServerListenerThread(LPVOID lParam);
	static int __stdcall ConnectAcceptCondition(
    IN LPWSABUF lpCallerId,
		IN LPWSABUF lpCallerData,
		IN OUT LPQOS lpSQOS,
		IN OUT LPQOS lpGQOS,
		IN LPWSABUF lpCalleeId, 
		OUT LPWSABUF lpCalleeData,
		OUT GROUP FAR *g,
		IN DWORD dwCallbackData);

  int getPort();
  int getActiveConnections();
  int getMaxConnections();
  int getMaxWorkers();

protected:
  bool allocateSocket();
  void cleanupSocket();
  void handleSocketAccept(SOCKET hClient);

  // following must be overriden
  virtual void acceptIncomingSocket(SOCKET hClient) = 0;

private:
  bool m_AlreadyListening;

  int m_Port;
  int m_ActiveConnections;
  int m_MaxConnections;
  int m_MaxWorkers;

  IOCP* m_IOCP;

  WSAEVENT m_hSocketEvent;
  HANDLE m_hStopEvent;

};

  } /* namespace sockets */
} /* namespace yaglib */

#endif /* __GJ_BASE_LISTENER_HEADER__ */