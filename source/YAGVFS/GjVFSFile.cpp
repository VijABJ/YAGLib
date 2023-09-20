
#include "GjVFSFile.h"
#include "GjUnicodeUtils.h"
#include <fstream>
using namespace yaglib;
using namespace yaglib::vfs;


File::File(const WideString& name, Entities* container) :
  Entity(name, container), m_DataHolder(NULL)
{
  m_DataHolder = new DataHolder(dynamic_cast<DemandLoader*>(this));
}

File::~File()
{
  delete m_DataHolder;
}

void File::loadFromFile(WideString& fileName)
{
  int nWhere = (int) fileName.rfind(L"\\");
  WideString shortName = (nWhere > 0) ? fileName.substr(nWhere+1) : fileName;
  setName(shortName);
  m_DataHolder->assign(fileName);
}

void File::saveToFile(WideString& fileName)
{
  UTF8String utf8FileName(fileName);
  std::ofstream dest(utf8FileName.c_str(), std::ios::out|std::ios::binary);
  if(dest.is_open()) 
    dest.write((char*)m_DataHolder->getData(), m_DataHolder->getDataSize());
}

int File::getIdSize()
{
  // we just add two ints which we will use to store the offset and size
  // of the data associated with this file object
  return Entity::getIdSize() + (sizeof(int) * 2); 
}

void File::saveId(std::ofstream& dest, int& offset)
{
  Entity::saveId(dest, offset);

  int bufSize = m_DataHolder->getDataSize();
  int _offset = offset;

  dest.write((char*)&bufSize, sizeof(int));
  dest.write((char*)&_offset, sizeof(int));

  offset += bufSize;
}

Entity* File::loadId(std::ifstream& source)
{
  Entity::loadId(source);

  int bufSize, offset;

  source.read((char*)&bufSize, sizeof(int));
  source.read((char*)&offset, sizeof(int));
  m_DataHolder->assign(offset, bufSize);

  return this;
}

void File::saveData(std::ofstream& dest)
{
  dest.write((char*)m_DataHolder->getData(), m_DataHolder->getDataSize());
}

DataHolder* const File::getDataHolder() const
{
  return m_DataHolder;
}

const PByte File::getPointerToData() const 
{ 
  return m_DataHolder->getData();
}

void File::DropData() 
{
  m_DataHolder->dropData();
}
