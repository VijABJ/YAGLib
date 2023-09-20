
#ifndef GJ_VFS_FILE_HEADER
#define GJ_VFS_FILE_HEADER

#include "GjDefs.h"
#include "GjVFSDefs.h"
#include "GjVFSEntities.h"
#include "GjVFSDataHolder.h"

namespace yaglib
{
  namespace vfs
  {

class File : public Entity
{
public:
  File(const WideString& name, Entities* container);
  virtual ~File();

  DataHolder* const getDataHolder() const;
  const PByte getPointerToData() const;
  void DropData();

  void loadFromFile(WideString& fileName);
  void saveToFile(WideString& fileName);

  // following four functions takes care of data persistence
  virtual int getIdSize();
  virtual void saveId(std::ofstream& dest, int& offset);
  virtual Entity* loadId(std::ifstream& source);
  virtual void saveData(std::ofstream& dest);

private:
  DataHolder* m_DataHolder;
};

  } /* namespace vfs */
} /* namespace yaglib */


#endif /* GJ_VFS_FILE_HEADER */
