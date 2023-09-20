
#ifndef GJ_SOCKET_UTILS_HEADER
#define GJ_SOCKET_UTILS_HEADER

#include "GjDefs.h"
#include <winsock2.h>

namespace yaglib
{
  namespace sockets
  {

/** A singleton that MUST be initialized before Winsock2 can be used
 *
 * In order to properly use this, just call the static member
 * initResult(), and check for whatever it returned.  Note that
 * the Socket class automatically calls this one so if you subclass
 * that, you don't need to call this yourself.
 */
class Winsock2
{
public:
  static Winsock2 const& getSingleton();

  int getInitResult();

private:
  static Winsock2 singleton;
  int mInitResult;

  Winsock2();
  Winsock2(const Winsock2&) {}; // copy constructor disabled
  ~Winsock2();
};

/** The basic socket class
 *
 * Note that in this implementation, it is nothing more than a glorified
 * container for a socket value.  It does have a couple of useful of
 * class functions though.  If all you want is a class to wrap around
 * a socket value, then this is your class.  Otherwise, create a subclass
 * that will implement the functionality you want
 */
class Socket
{
public:
  Socket();
  virtual ~Socket();

  SOCKET getSocket();
  bool valid();

  /** Disconnect the given socket */
  static void dropSocket(SOCKET theSocket);

  /** Create a string representation of the IP address contained in theSocket */
  static WideString peerAddress(SOCKET theSocket);

protected:
  SOCKET m_Socket;
};

  } /* namespace sockets */
} /* namespace yaglib */


#endif /* GJ_SOCKET_UTILS_HEADER */