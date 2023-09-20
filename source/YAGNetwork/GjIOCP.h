
#ifndef GJ_IOCP_HEADER
#define GJ_IOCP_HEADER

#include "GjDefs.h"
#include <winsock2.h>

namespace yaglib
{
  namespace sockets
  {

class IOCP
{
public:
  IOCP();
  ~IOCP();

  HANDLE getPort();

  bool postStatus(
    ULONG_PTR completionKey, 
    DWORD dwNumBytes = 0, 
    OVERLAPPED *pOverlapped = 0);

  int getStatus(
    ULONG_PTR *pCompletionKey, 
    PDWORD pdwNumBytes,
    OVERLAPPED **ppOverlapped);

  int getStatus(
    ULONG_PTR *pCompletionKey, 
    PDWORD pdwNumBytes,
    OVERLAPPED **ppOverlapped, 
    DWORD dwMilliseconds);

  void associateSocket(SOCKET hSocket, DWORD dwCompletionKey);

private:
  HANDLE m_Handle;
};

  } /* namespace sockets */
} /* namespace yaglib */

#endif /* GJ_IOCP_HEADER */