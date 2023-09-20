
#ifndef GJ_SOCKET_CONTEXTS_HEADER
#define GJ_SOCKET_CONTEXTS_HEADER

#include "GjDefs.h"
#include <winsock2.h>
#include "GjThreads.h"
#include "GjSocketUtils.h"
#include "GjIOCP.h"
#include "GjSocketPackets.h"

namespace yaglib
{
  namespace sockets
  {

class ContextManager
{
public:
  class Context
  {
  public:
    Context(ContextManager* owner);
    ~Context();

    // not part of the constructor so that we can make
    // a connection pool later
    void init(SOCKET hRemote);
    void cleanup();

    SOCKET getSocket();
    const WideString& getRemoteIp();
    ContextManager* getOwner();
    PacketsThreadSafe* getPendingList();

    void send(void* buffer, Longint bufferSize);

  private:
    ContextManager* m_Owner;
    SOCKET m_Socket;
    CriticalSection m_CodeLock;
    Longint m_MsgIdSequenceTracker;
    WideString m_RemoteIp;
    PacketsThreadSafe* m_Pending;

    Longint getMessageId();
  };

  ContextManager();
  ~ContextManager();

  IOCP* getIOCP();
  PacketsAllocator* getFreeList();

  Context* connect(const WideString& ipAddress, int port);
  Context* associateSocketWithContext(SOCKET hClient);

private:
  friend class Context;

  class WorkerThread : public Thread
  {
  public:
    virtual void main(PVOID startParam);
  };

  IOCP* m_IOCP;
  std::vector<WorkerThread*> m_WorkerThreads;
  PacketsAllocator* m_PacketFreeList;

  void createWorkerThreads(int numThreads);
  void deleteWorkerThreads();
};

  } /* namespace sockets */
} /* namespace yaglib */


#endif /* GJ_SOCKET_CONTEXTS_HEADER */
