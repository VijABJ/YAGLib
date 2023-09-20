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

#include "GjBFS.h"
#include "GjUnicodeUtils.h"
#include <boost/filesystem.hpp>
using namespace yaglib;
using namespace boost;

// just in case someone else managed to redefine this
#ifdef YAGLIB_FORCE_BOOST_FILESYSTEM_TO_STD_STRING
#undef YAGLIB_FORCE_BOOST_FILESYSTEM_TO_STD_STRING
#endif

// if VC is earlier than VC8, then we need to do some
// special (aka, braindead) stuff.
#if _MSC_VER < 1400
#define YAGLIB_FORCE_BOOST_FILESYSTEM_TO_STD_STRING
#endif

bool bfs::exists(const WideString& fileName)
{
#ifdef YAGLIB_FORCE_BOOST_FILESYSTEM_TO_STD_STRING
  return filesystem::exists(UTF8String(fileName).c_str());
#else
  return filesystem::exists(fileName);
#endif
}

bool bfs::is_directory(const WideString& fileName)
{
#ifdef YAGLIB_FORCE_BOOST_FILESYSTEM_TO_STD_STRING
  return filesystem::is_directory(UTF8String(fileName).c_str());
#else
  return filesystem::is_directory(fileName);
#endif
}

bool bfs::is_file(const WideString& fileName)
{
#ifdef YAGLIB_FORCE_BOOST_FILESYSTEM_TO_STD_STRING
  return !filesystem::is_directory(UTF8String(fileName).c_str());
#else
  return !filesystem::is_directory(fileName);
#endif
}

size_t bfs::list_files(WideString path, std::vector<WideString>& list, 
  const bool wantFiles, const bool wantFolders)
{
  if(!bfs::exists(path))
    return 0;

  using namespace boost;

#ifdef YAGLIB_FORCE_BOOST_FILESYSTEM_TO_STD_STRING
  filesystem::directory_iterator end; // default constructor is the value beyond iterator range
  std::string s_path = std::string(UTF8String(path).c_str());
  for(filesystem::directory_iterator iter(s_path); iter != end; ++iter)
  {
    bool isFolder = is_directory(iter->status());
    // skip special folders, we don't want `em at all
    if(isFolder && ((iter->path().leaf() == ".") || (iter->path().leaf() == "..")))
      continue;
    //
    if(isFolder && wantFolders)
      list.push_back(from_char_p(iter->path().leaf().c_str()));
    else if(!isFolder && wantFiles)
      list.push_back(from_char_p(iter->path().leaf().c_str()));
  }
#else
  filesystem::wdirectory_iterator end; // default constructor is the value beyond iterator range
  for(filesystem::wdirectory_iterator iter(path); iter != end; ++iter)
  {
    bool isFolder = is_directory(iter->status());
    // skip special folders, we don't want `em at all
    if(isFolder && ((iter->path().leaf() == L".") || (iter->path().leaf() == L"..")))
      continue;
    //
    if(isFolder && wantFolders)
      list.push_back(iter->path().leaf());
    else if(!isFolder && wantFiles)
      list.push_back(iter->path().leaf());
  }
#endif

  return list.size();
}
