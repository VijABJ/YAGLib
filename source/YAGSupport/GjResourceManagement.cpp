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

#include "GjResourceManagement.h"
#include "GjUnicodeUtils.h"
#include "GjNativeApp.h"
#include "GjStringUtils.h"
#include "GjBFS.h"
#include <iostream>
#include <fstream>
using namespace yaglib;

// resource store base
ResourceStore::ResourceStore(const WideString& pathName) : mPathName(pathName)
{
}

WideString const& ResourceStore::getPathName() const
{
  return mPathName;
}

class FindFile : public std::unary_function<ResourceStore*, bool>
{
public:
  explicit FindFile(DataPack& dataPack, const WideString& fileName, const bool fileNameOnly) : 
    mDataPack(dataPack), mFileName(fileName), mFileNameOnly(fileNameOnly)
  { };
  bool operator()(ResourceStore* store) 
  { return store->lookup(mDataPack, mFileName, mFileNameOnly); };
private:
  DataPack& mDataPack;
  WideString mFileName;
  bool mFileNameOnly;
};

FileSystemStore::FileSystemStore(const WideString& pathName) : 
  ResourceStore(pathName), mQualifiedPath(pathName)
{
  WideString::iterator iter = mQualifiedPath.end()-1;
  if((*iter) != '\\') 
    mQualifiedPath += L"\\";
    
  if(!bfs::exists(mQualifiedPath) || !bfs::is_directory(mQualifiedPath))
    throw std::exception("Resource folder does not exists");
}

bool FileSystemStore::lookup(DataPack& dataPack, const WideString fileName, const bool fileNameOnly)
{
  WideString qualifiedFileName = mQualifiedPath + fileName;
  if(!bfs::exists(qualifiedFileName) || bfs::is_directory(qualifiedFileName))
    return false;

  dataPack.setFileName(qualifiedFileName);
  if(!fileNameOnly)
  {
    std::ifstream source(UTF8String(qualifiedFileName).c_str(), std::ios::binary);
    source.seekg(0, std::ios_base::end);
    size_t size = source.tellg();
    source.seekg(0, std::ios_base::beg);

    char* data = new char[size];
    source.read(data, static_cast<std::streamsize>(size));
    dataPack.setData(data, size);
  }

  return true;
}

bool ResourceManager::ResourceGroup::lookup(DataPack& dataPack, const WideString fileName, const bool fileNameOnly)
{
  dataPack.cleanup();
  iterator iter = std::find_if(begin(), end(), FindFile(dataPack, fileName, fileNameOnly));  
  return iter != end();
}

bool ResourceManager::initialize(MultipleSettings& settings, const WideString overridePath)
{
  // set internal paths 
  mOverridePath = overridePath;
  mRootPath = string_utils::conditional_append_copy(NativeApplication::ApplicationPath(), '\\');

  if(!settings.exists(CONFIG_FOLDERS_SECTION))
    return false;

  Settings folderList = settings[CONFIG_FOLDERS_SECTION];
  for(int i = 0; i < static_cast<int>(folderList.size()); i++)
  {
    Setting item = folderList[i];
    WideString folderName = mRootPath + item.getValue();
    if(!bfs::exists(folderName) || !bfs::is_directory(folderName))
      continue;

    Groups::iterator subj = mGroups.empty() ? mGroups.end() : mGroups.find(item.getName());
    if(subj != mGroups.end())
      subj->second.add(new FileSystemStore(folderName));
    else
    {
      mGroups[item.getName()] = ResourceGroup();
      mGroups[item.getName()].add(new FileSystemStore(folderName));
    }
  }

  return true;
}

int ResourceManager::lookup(DataPack& dataPack, const WideString fileName, 
  const WideString groupName, const bool fileNameOnly)
{
  bool found = false;
  if(groupName == GROUP_NAME_ANY)
  {
    for(Groups::iterator iter = mGroups.begin(); iter != mGroups.end(); iter++)
    {
      found = iter->second.lookup(dataPack, fileName, fileNameOnly);
      if(found)
        break;
    }
  }
  else
  {
    Groups::iterator subj = mGroups.empty() ? mGroups.end() : mGroups.find(groupName);
    if(subj != mGroups.end())
      found = subj->second.lookup(dataPack, fileName, fileNameOnly);
  }

  return found ? 1 : 0;
}

int ResourceManager::lookup(DataPacks& dataPacks, const WideString fileName, const bool fileNameOnly)
{
  int foundCount = 0;
  for(Groups::iterator iter = mGroups.begin(); iter != mGroups.end(); iter++)
  {
    DataPack dp; 
    dp.cleanup();
    bool found = iter->second.lookup(dp, fileName, fileNameOnly);
    if(found)
    {
      dataPacks.push_back(dp);
      foundCount++;
    }
  }

  return foundCount;
}

WideString ResourceManager::operator[](const WideString fileName)
{
  DataPack dp;
  int count = lookup(dp, fileName, GROUP_NAME_ANY, true);
  return count ? dp.getFileName() : L"";
}

