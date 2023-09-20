
#ifndef GJ_VFS_VOLUME_HEADER
#define GJ_VFS_VOLUME_HEADER

#include "GjDefs.h"
#include "GjVFSDefs.h"
#include "GjVFSEntities.h"
#include "GjVFSFile.h"
#include "GjVFSFolder.h"

#include <fstream>

namespace yaglib
{
  namespace vfs
  {

class Volume : public Folder
{
public:
  Volume();
  virtual ~Volume();

  bool folderExists(WideString& path);
  bool fileExists(WideString& path);

  Entity* add(WideString& path, WideString& pathToFile);
  Entity* add(WideString& path, Byte* buf, unsigned int bufSize);

  Folder* getFolder(WideString& path, bool forceCreate = true);
  File* getFile(WideString& path, bool forceCreate = true);

  WideString& getOverridePath();
  void setOverridePath(WideString& path);

protected:
  WideString m_OverridePath;

  Folder* getFolder(std::vector<WideString>& names, bool forceCreate = true);
  Entity* getEntity(WideString& path) { return NULL; };
};

class ReadWriteVolume : public Volume
{
public:
  ReadWriteVolume();
  virtual ~ReadWriteVolume();

  void setFileName(WideString& fileName);
  const WideString& getFileName() const;

  bool saveToFile(WideString& fileName);
  bool loadFromFile(WideString& fileName);

protected:
  void loadFileData(Folder* folder, std::ifstream& source);
  void loadFileData(File* file, std::ifstream& source);

private:
  WideString m_FileName;
};

  } /* namespace vfs */
} /* namespace yaglib */

#endif /* GJ_VFS_VOLUME_HEADER */
