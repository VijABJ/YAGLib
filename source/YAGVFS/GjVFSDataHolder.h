
#ifndef GJ_VFS_DATA_HOLDER_HEADER
#define GJ_VFS_DATA_HOLDER_HEADER

#include "GjDefs.h"
#include "GjVFSDefs.h"
#include "GjVFSEntities.h"

namespace yaglib
{
  namespace vfs
  {

class DataHolder
{
public:
  DataHolder(DemandLoader* demandLoader);
  DataHolder(PByte data, size_t size, bool ownsData = false);
  virtual ~DataHolder();

  void assign(PByte data, size_t, bool ownsData);
  void assign(PByte source, int offset, size_t size, bool ownsData);
  void assign(int offset, size_t size);
  void assign(WideString& fileName);

  void dropData();

  const PByte getData();
  const size_t getDataSize() const;
  const int getOffset() const;
  const DemandLoader* getDemandLoader() const;
  const bool canLoadOnDemand() const;
  const bool isDataOwned() const;
  const bool hasData() const;

private:
  DemandLoader* m_DemandLoader;
  
  PByte m_Data;       // this here's our actual data
  size_t m_DataSize;  // and this tells how much we have
  bool m_OwnsData;    // if this is true, we own the data, and should clean it up

  int m_Offset;       // if to be demand-loaded, this is the offset 

};

  } /* namespace vfs */
} /* namespace yaglib */



#endif /* GJ_VFS_DATA_HOLDER_HEADER */
