/*
Yet Another Game Library
Copyright (c) 2001-2007, Virgilio A. Blones, Jr. (vij_blones_jr@yahoo.com)
See https://sourceforge.net/projects/yaglib/ for the latest updates.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.
      
    * Redistributions in binary form must reproduce the above copyright notice, 
      this list of conditions and the following disclaimer in the documentation 
      and/or other materials provided with the distribution.
      
    * Neither the name of this library (YAGLib) nor the names of its contributors 
      may be used to endorse or promote products derived from this software 
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/** 
 * @file  GjResourceManagement.h
 * @brief Manages resources so that no other part of the code need worry about file locations
 *
 */
#ifndef GJ_RESOURCE_MANAGEMENT_HEADER
#define GJ_RESOURCE_MANAGEMENT_HEADER

#include "GjDefs.h"
#include "GjTemplates.h"
#include "GjSettings.h"

namespace yaglib 
{

class ResourceStore;
class ResourceGroup;
class ResourceManager;

class DataPack
{
public:
  DataPack() : mData(NULL), mSize(0) { };
  virtual ~DataPack() { cleanup(); };

  void cleanup()
  {
    SAFE_DELETE(mData);
    mSize = 0;
  };

  void* getData() const { return mData; };
  size_t getSize() const { return mSize; };

  void setData(void* data, const size_t size) 
  { 
    cleanup(); 
    mData = data;
    mSize = size;
  };

  WideString const& getFileName() const { return mFileName; };
  void setFileName(const WideString& fileName) { mFileName = fileName; };

private:
  WideString mFileName;
  void* mData;
  size_t mSize;
};

typedef std::vector<DataPack> DataPacks;

class ResourceStore
{
public:
  ResourceStore(const WideString& pathName);

  WideString const& getPathName() const;
  virtual bool lookup(DataPack& dataPack, const WideString fileName, const bool fileNameOnly = false) = 0;

private:
  WideString mPathName;
};

class FileSystemStore : public ResourceStore
{
public:
  FileSystemStore(const WideString& pathName);

  virtual bool lookup(DataPack& dataPack, const WideString fileName, const bool fileNameOnly = false);

private:
  WideString mQualifiedPath;
};

#define CONFIG_FOLDERS_SECTION            L"resource-folders"
#define GROUP_NAME_ANY                    WideString(L"")

class ResourceManager : public Singleton<ResourceManager>
{
public:
  bool initialize(MultipleSettings& settings, const WideString overridePath = L"");

  int lookup(DataPack& dataPack, const WideString fileName, 
    const WideString groupName = GROUP_NAME_ANY, const bool fileNameOnly = false);
  int lookup(DataPacks& dataPacks, const WideString fileName, const bool fileNameOnly = false);

  WideString operator[](const WideString fileName);

private:
  class ResourceGroup : public ObjectList<ResourceStore>
  {
  public:
    virtual bool lookup(DataPack& dataPack, const WideString fileName, const bool fileNameOnly);
  };

  WideString mRootPath;
  WideString mOverridePath;
  typedef std::map<WideString, ResourceGroup> Groups;
  Groups mGroups;
};


// macros for the singleton
#define g_ResourceManager   (ResourceManager::Instance())

} /* namespace yaglib */

#endif /* GJ_RESOURCE_MANAGEMENT_HEADER */
