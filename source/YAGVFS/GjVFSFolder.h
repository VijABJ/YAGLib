
#ifndef GJ_VFS_FOLDER_HEADER
#define GJ_VFS_FOLDER_HEADER

#include "GjDefs.h"
#include "GjVFSDefs.h"
#include "GjVFSEntities.h"
#include "GjVFSFile.h"

namespace yaglib
{
  namespace vfs
  {

class Files : public Entities
{
public:
  Files(Entity* owner);
  File* operator[](int index);
  File* operator[](const WideString& name);
};

class Folder; // forward declaration

class Folders : public Entities
{
public:
  Folders(Entity* owner);
  Folder* operator[](int index);
  Folder* operator[](const WideString& name);
};

class Folder : public Entity
{
public:
  Folder(const WideString& name, Entities* container);
  virtual ~Folder();

  virtual const bool isFolder() const;
  void clear();

  void attach(Entity* entity);
  void detach(Entity* entity, bool free = false);

  Folders* const getFolders() const;
  Files* const getFiles() const;

  Folder* createFolder(const WideString& name);
  File* createFile(const WideString& name);

  // following four functions takes care of data persistence
  virtual int getIdSize();
  virtual void saveId(std::ofstream& dest, int& offset);
  virtual Entity* loadId(std::ifstream& source);
  virtual void saveData(std::ofstream& dest);

protected:
  virtual void containerChanged();
  virtual void nameChanged();
  virtual bool nameAvailable(const WideString& nameToCheck);

private:
  Folders* m_Folders;
  Files* m_Files;

  void propagateIdentityChange();
};

  } /* namespace vfs */
} /* namespace yaglib */


#endif /* GJ_VFS_FOLDER_HEADER */
