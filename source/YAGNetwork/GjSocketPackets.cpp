
#include "GjSocketPackets.h"
using namespace yaglib;
using namespace yaglib::sockets;

// Packets
Packets::Packets()
{
}

Packets::~Packets()
{
  clear();
}

void Packets::clear()
{
  if(m_Items.size() == 0) return;

  for(std::vector<Overlapped*>::iterator iter = m_Items.begin();
    iter != m_Items.end(); iter++)
    delete *iter;

  m_Items.clear();
}

bool Packets::isEmpty()
{
  return m_Items.size() == 0;
}

void Packets::add(Overlapped* ol)
{
  m_Items.push_back(ol);
}

void Packets::remove(Overlapped* ol)
{
  for(std::vector<Overlapped*>::iterator iter = m_Items.begin();
    iter != m_Items.end(); iter++)
  {
    if((*iter) == ol)
    {
      m_Items.erase(iter);
      break;
    }
  }
}

Overlapped* Packets::extract()
{
  if(m_Items.size() == 0)
    return NULL;

  std::vector<Overlapped*>::iterator iter = m_Items.begin();
  Overlapped* ol = *iter;
  m_Items.erase(iter);

  return ol;
}


// PacketsThreadSafe
PacketsThreadSafe::PacketsThreadSafe()
{
}

PacketsThreadSafe::~PacketsThreadSafe()
{
}

void PacketsThreadSafe::clear()
{
  m_Lock.enter();
  Packets::clear();
  m_Lock.leave();
}

bool PacketsThreadSafe::isEmpty()
{
  m_Lock.enter();
  bool isEmpty = Packets::isEmpty();
  m_Lock.leave();
  return isEmpty;
}

void PacketsThreadSafe::add(Overlapped* ol)
{
  m_Lock.enter();
  Packets::add(ol);
  m_Lock.leave();
}

void PacketsThreadSafe::remove(Overlapped* ol)
{
  m_Lock.enter();
  Packets::remove(ol);
  m_Lock.leave();
}

Overlapped* PacketsThreadSafe::extract()
{
  m_Lock.enter();
  Overlapped* ol = Packets::extract();
  m_Lock.leave();
  return ol;
}

// PacketsAllocator
Overlapped* PacketsAllocator::allocate()
{
  Overlapped* ol = extract();

  m_Lock.enter();
  if(!ol)
    Overlapped* ol = new Overlapped();

  memset(ol, 0, sizeof(Overlapped));
  m_Lock.leave();

  return ol;
}

void PacketsAllocator::release(Overlapped* ol)
{
  PacketsThreadSafe::add(ol);
}

