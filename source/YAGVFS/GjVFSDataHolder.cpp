
#include "GjVFSDataHolder.h"
#include "GjUnicodeUtils.h"
#include <fstream>
using namespace yaglib;
using namespace yaglib::vfs;

DataHolder::DataHolder(DemandLoader* demandLoader) :
  m_DemandLoader(demandLoader), m_Data(NULL), m_DataSize(0),
  m_Offset(0)
{
  // basic setup, has demand loader, but no data.
  // there's no data descriptor either (size, offset).
  // in this mode, the demand loader (presumably also
  // the owner) must initialize this object properly
  // at a later time.
}

DataHolder::DataHolder(PByte data, size_t size, bool ownsData) :
  m_DemandLoader(NULL), m_Data(NULL), m_DataSize(0), m_Offset(0)
{
  // standalone setup. demand-loading will not be supported
  // because a demand-loader was not passed in.
  assign(data, size, ownsData);
}

void DataHolder::assign(PByte data, size_t size, bool ownsData)
{
  dropData();
  if((data == NULL) || (size == 0)) 
    return;

  if(!ownsData)
  {
    // we're not supposed to own the passed buffer, so let's make 
    // our buffer and copy it there
    m_Data = new Byte [m_DataSize];
    memcpy(m_Data, data, size);
  }
  else
    m_Data = data;

  // note that regardless of ownsData passed, we *will* end up
  // having responsibility for cleaning up the data.  you see, if
  // false was passed in ownsData, we make our own copy, which we
  // must cleanup!
  m_OwnsData = true;
  m_DataSize = size;
}

void DataHolder::assign(PByte source, int offset, size_t size, bool ownsData)
{
  if(source == NULL)
    return;

  PByte data = (PByte) ((long long)source + offset);
  assign(data, size, ownsData);
}

void DataHolder::assign(int offset, size_t size)
{
  // ignore this call if there is no demand loader to begin with
  if(m_DemandLoader == NULL)
    return;

  dropData();
  m_DataSize = size;
  m_Offset = offset;
}

void DataHolder::assign(WideString& fileName)
{
  UTF8String utf8FileName(fileName);
  std::ifstream source(utf8FileName.c_str(), std::ios::in|std::ios::binary);

  if(source.is_open()) 
  {
    source.seekg(0, std::ios::end);
    int length = source.tellg();
    source.seekg(0, std::ios::beg);

    char* buffer = new char [length];
    source.read(buffer, length);
    source.close();

    // get rid of the old data first, if it's there...
    dropData();

    m_OwnsData = true;
    m_DataSize = length;
    m_Data = (PByte)buffer;
  }
}

void DataHolder::dropData()
{
  if((m_Data != NULL) && (m_OwnsData))
  {
    delete [] m_Data;
    m_Data = NULL;
  }
}

const PByte DataHolder::getData()
{
  // return immediately if we do have the data
  if(m_Data != NULL)
    return m_Data;

  // we don't have any data! check if we can perform demand-loading...
  if(m_DemandLoader == NULL)
    return NULL;

  // perform demand-loading, if this fails, return NULL
  // demand-loading MUST return a unique data buffer that 
  // we WILL own, and cleanup later.
  PByte temp = m_DemandLoader->Load(m_Offset, m_DataSize);
  if(temp == NULL)
    return NULL;

  // success! remember this data pointer for next time...
  m_Data = temp;
  m_OwnsData = true;

  // should now have data to return
  return m_Data;
}

DataHolder::~DataHolder() 
{ 
  dropData(); 
}

const size_t DataHolder::getDataSize() const 
{ 
  return m_DataSize; 
}

const int DataHolder::getOffset() const 
{ 
  return m_Offset; 
}

const DemandLoader* DataHolder::getDemandLoader() const 
{ 
  return m_DemandLoader; 
}

const bool DataHolder::canLoadOnDemand() const 
{ 
  return (m_DemandLoader != NULL); 
}

const bool DataHolder::isDataOwned() const 
{ 
  return m_OwnsData; 
}

const bool DataHolder::hasData() const 
{ 
  return (m_Data != NULL); 
}

