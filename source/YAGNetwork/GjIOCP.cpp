
#include "GjIOCP.h"
#include "GjTrace.h"
using namespace yaglib;
using namespace yaglib::sockets;

IOCP::IOCP()
{
  m_Handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  GTRACE(L"IO Completion Port: %ld", m_Handle);
}

IOCP::~IOCP()
{
  CloseHandle(m_Handle);
}

bool IOCP::postStatus(
  ULONG_PTR completionKey, 
  DWORD dwNumBytes /* = 0 */, 
  OVERLAPPED *pOverlapped /* = 0 */) 
{
  return (PostQueuedCompletionStatus(m_Handle, dwNumBytes, completionKey, pOverlapped) != FALSE);
}

int IOCP::getStatus(
  ULONG_PTR *pCompletionKey, PDWORD pdwNumBytes, OVERLAPPED **ppOverlapped)
{
  int nRet = GetQueuedCompletionStatus(m_Handle, pdwNumBytes, pCompletionKey, ppOverlapped, INFINITE);
  return nRet;
}

int IOCP::getStatus(
   ULONG_PTR *pCompletionKey, PDWORD pdwNumBytes,
   OVERLAPPED **ppOverlapped, DWORD dwMilliseconds)
{
  int nRet = GetQueuedCompletionStatus(m_Handle, pdwNumBytes, pCompletionKey, ppOverlapped, dwMilliseconds); 
  return nRet;
}

void IOCP::associateSocket(SOCKET hSocket, DWORD dwCompletionKey)
{
  CreateIoCompletionPort((HANDLE) hSocket, m_Handle, dwCompletionKey, 0);
}

HANDLE IOCP::getPort()
{
  return m_Handle;
}
