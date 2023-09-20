
#ifndef GJ_SOCKET_PACKETS_HEADER
#define GJ_SOCKET_PACKETS_HEADER

#include "GjDefs.h"
#include <winsock2.h>
#include "GjThreads.h"

namespace yaglib
{
  namespace sockets
  {

struct PacketHeader
{
  Longint m_PacketSize;        // size of this packet
  Longint m_PacketIndex;       // 1-based
  Longint m_NumberOfPackets;   // # of packets in a message
  Longint m_SizeWhenComplete;  // number of bytes when message is complete
  Longint m_MessageId;         // tag # for message as a whole
};

const Longint MAX_PACKET_SIZE = 1024 * 8; // 8K, maximum packet size, includes header
const Longint HEADER_SIZE = sizeof(PacketHeader);
const Longint MAX_DATA_SIZE = MAX_PACKET_SIZE - HEADER_SIZE;

struct Packet
{
  PacketHeader m_Header;
  Byte m_Data[MAX_DATA_SIZE];

  Packet() { init(); };
  void init() { memset(&m_Header, 0, sizeof(m_Header)); };
};

enum SocketOperation {soNothing, soSending, soReceivingPacketSize, soReceivingPacketData};

struct Overlapped
{
  OVERLAPPED m_Overlapped;
  WSABUF m_WSABuf;
  Packet m_Packet;
  SocketOperation m_Operation;
};

class Packets
{
public:
  Packets();
  ~Packets();

  void clear();
  bool isEmpty();

  void add(Overlapped* ol);
  void remove(Overlapped* ol);
  Overlapped* extract();

private:
  std::vector<Overlapped*> m_Items;
};

class PacketsThreadSafe : private Packets
{
public:
  PacketsThreadSafe();
  ~PacketsThreadSafe();

  void clear();
  bool isEmpty();

  void add(Overlapped* ol);
  void remove(Overlapped* ol);
  Overlapped* extract();

protected:
  CriticalSection m_Lock;
};

class PacketsAllocator : public PacketsThreadSafe
{
public:
  Overlapped* allocate();
  void release(Overlapped* ol);
};


  } /* namespace sockets */
} /* namespace yaglib */


#endif /* GJ_SOCKET_PACKETS_HEADER */
