
#include "GjSocketUtils.h"
#include "GjUnicodeUtils.h"

using namespace yaglib;
using namespace yaglib::sockets;

Winsock2::Winsock2()
{
  WSADATA wsaData;
  mInitResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
}

Winsock2::~Winsock2()
{
  if(NO_ERROR == mInitResult)
    WSACleanup();
}

int Winsock2::getInitResult()
{
  return mInitResult;
};

Winsock2 const& Winsock2::getSingleton()
{
  return singleton;
}


void Socket::dropSocket(SOCKET theSocket)
{
  LINGER linger;
  linger.l_onoff = 1;
  linger.l_linger = 0;
  setsockopt(theSocket, SOL_SOCKET, SO_LINGER, (char*) &linger, sizeof(LINGER));
  CancelIo((HANDLE) theSocket);
  closesocket(theSocket);
}

WideString Socket::peerAddress(SOCKET theSocket)
{
  SOCKADDR_IN sa;
  int saLen = 0;
  /*int nRet = */getpeername(theSocket, (SOCKADDR*) &sa, &saLen);
  const int MAX_IP_LENGTH = 64; // too long I know, precautionary
  char remoteIp[MAX_IP_LENGTH];
  
  char* ipString = inet_ntoa(sa.sin_addr);
  if(ipString)
    strcpy(remoteIp, ipString);
  else
    remoteIp[0] = '\0';

  return WideStringUtils::Create(remoteIp);
}

Socket::Socket() : m_Socket(INVALID_SOCKET) 
{
  //Winsock2::initResult();
}

Socket::~Socket() 
{ 
  if (INVALID_SOCKET != m_Socket) closesocket(m_Socket); 
}

SOCKET Socket::getSocket() 
{ 
  return m_Socket; 
}

bool Socket::valid() 
{ 
  return (INVALID_SOCKET != m_Socket); 
}


