
#include "GjVFSVolume.h"
#include "GjUtils.h"
#include "GjUnicodeUtils.h"
using namespace yaglib;
using namespace yaglib::vfs;

Volume::Volume() : Folder(PATH_SEPARATOR, NULL)
{
}

Volume::~Volume()
{
}

Folder* Volume::getFolder(std::vector<WideString>& names, bool forceCreate)
{
  Folder* folder = this;

  for(std::vector<WideString>::iterator iter = names.begin(); iter != names.end(); iter++)
  {
    Folders* list = folder->getFolders();

    Folder* subFolder = (*list)[*iter];
    if(subFolder == NULL)
    {
      if(!forceCreate) return NULL;
      subFolder = folder->createFolder(*iter);
    }

    folder = subFolder;
  }

  return folder;
}

bool Volume::folderExists(WideString& path)
{
  std::vector<WideString> names;
  WideStringUtils::splitPath(path, names);

  return getFolder(names, false) != NULL;
}

bool Volume::fileExists(WideString& path)
{
  return getFile(path, false) != NULL;
}

Entity* Volume::add(WideString& path, WideString& pathToFile)
{
  File* file = getFile(path, true);
  file->loadFromFile(pathToFile);
  return file;
}

Entity* Volume::add(WideString& path, Byte* buf, unsigned int bufSize)
{
  File* file = getFile(path, true);
  DataHolder* dh = file->getDataHolder();
  dh->assign(buf, bufSize, false);
  return file;
}

Folder* Volume::getFolder(WideString& path, bool forceCreate)
{
  std::vector<WideString> names;
  WideStringUtils::splitPath(path, names);
  return getFolder(names, forceCreate);
}

File* Volume::getFile(WideString& path, bool forceCreate)
{
  std::vector<WideString> names;
  WideStringUtils::splitPath(path, names);
  WideString fileName = names[names.size()-1];
  names.erase(names.end()-1);

  Folder* folder = getFolder(names, forceCreate);
  if(folder == NULL)
    return NULL;

  Files* list = folder->getFiles();
  File* theFile = dynamic_cast<File*>(list->find(fileName));
  if((theFile == NULL) && forceCreate)
    theFile = folder->createFile(fileName);

  // if there is indeed a file, we should check the override path
  // in case there is an override file for this one
  if((NULL != theFile) && (m_OverridePath.size() > 0))
  {
    WideString qualifiedPath = m_OverridePath + path;
    if(OSFiles::Exists(const_cast<TCHAR*>(qualifiedPath.c_str())))
    {
      theFile->getDataHolder()->assign(qualifiedPath);
    }
  }

  return theFile;
}

WideString& Volume::getOverridePath()
{
  return m_OverridePath;
}

void Volume::setOverridePath(WideString& path)
{
  m_OverridePath = path;
}


bool ReadWriteVolume::saveToFile(WideString& fileName)
{
  // open the destination file, bail out if it's not opened
  UTF8String utf8(fileName);
  std::ofstream dest(utf8.c_str(), std::ios::binary|std::ios::out);
  if(!dest.is_open()) return false;

  // write the header
  VFS_HEADER vfsHeader;
  strcpy(vfsHeader.signature, cDefSignature);
  vfsHeader.headerSize = sizeof(VFS_HEADER);
  vfsHeader.folderOffset = vfsHeader.headerSize;
  vfsHeader.dataOffset = getIdSize() + vfsHeader.headerSize + 1;
  dest.write((char*) &vfsHeader, sizeof(VFS_HEADER));

  // save the structures
  int offset = vfsHeader.dataOffset;
  saveId(dest, offset);

  // save the data itself
  saveData(dest);

  return false;
}

bool ReadWriteVolume::loadFromFile(WideString& fileName)
{
  // open the source file, bail out if it's not opened
  UTF8String utf8(fileName);
  std::ifstream source(utf8.c_str(), std::ios::binary|std::ios::in);
  if(!source.is_open()) return false;

  // load the header
  VFS_HEADER vfsHeader;
  source.read((char*)&vfsHeader, sizeof(VFS_HEADER));

  // make sure to clear whatever we currently have
  clear();

  // load the structures
  loadId(source);

  // load the data, maybe not?
  loadFileData(this, source);

  // trigger name updates across the board
  nameChanged();

  return false;
}

void ReadWriteVolume::loadFileData(Folder* folder, std::ifstream& source)
{
  Folders* folders = folder->getFolders();
  for(Entities::iterator iter = folders->begin(); iter != folders->end(); iter++)
    loadFileData(dynamic_cast<Folder*>(*iter), source);

  Files* files = folder->getFiles();
  for(Entities::iterator iter = files->begin(); iter != files->end(); iter++)
    loadFileData(dynamic_cast<File*>(*iter), source);
}

void ReadWriteVolume::loadFileData(File* file, std::ifstream& source)
{
  DataHolder* holder = const_cast<DataHolder*>(file->getDataHolder());
  int offset = holder->getOffset();
  size_t size = holder->getDataSize();
  char* data = new char[size];

  source.seekg(offset, std::ios::beg);
  source.read(data, size);
  holder->assign((PByte)data, size, true);
}

ReadWriteVolume::ReadWriteVolume() : Volume(), m_FileName(L"")
{
}

ReadWriteVolume::~ReadWriteVolume()
{
}

void ReadWriteVolume::setFileName(WideString& fileName)
{
  m_FileName = fileName;
}

const WideString& ReadWriteVolume::getFileName() const
{
  return m_FileName;
}

