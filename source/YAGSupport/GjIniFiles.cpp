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

#include "GjIniFiles.h"
#include "GjUnicodeUtils.h"
#include "GjStringUtils.h"
#include "GjBFS.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
using namespace yaglib;

void IniSettingsStore::load(const WideString& objectName, MultipleSettings& settings)
{
  if(!bfs::exists(objectName) || bfs::is_directory(objectName))
    return;

  std::ifstream source(UTF8String(objectName).c_str());
  std::string ss;
  Settings* group = NULL;
  while(getline(source, ss))
  {
    // remove whitespaces, skip blanks and comments
    boost::trim(ss);
    if((ss.size() == 0) || (ss[0] == COMMENT_CHAR))
      continue;
    
    // check for section starts
    if(ss[0] == SECTION_BEGIN_CHAR)
    {
      WideString ws = from_char_p(std::string(ss.substr(1, ss.find(SECTION_END_CHAR) - 1)).c_str());
      group = &(settings[ws]);
      continue;
    }

    // an item! add to the group if there's one
    if(NULL != group)
      group->add(from_char_p(ss.c_str()));
  }
}

void IniSettingsStore::save(const WideString& objectName, MultipleSettings& settings)
{
  if(bfs::exists(objectName))
    _wunlink(objectName.c_str());

  std::ofstream dest(UTF8String(objectName).c_str());
  for(int i = 0; i < static_cast<int>(settings.size()); i++)
  {
    Settings group = settings[i];
    dest << SECTION_BEGIN_CHAR << UTF8String(group.getName()).c_str() << SECTION_END_CHAR << std::endl;
    for(int j = 0; j < static_cast<int>(group.size()); j++)
    {
      Setting item = group[j];
      dest << UTF8String(item.getName()).c_str() << '=' << UTF8String(item.getValue()).c_str() << std::endl;
    }
  }
  dest << std::endl;
}

IniSettings::IniSettings(const WideString& fileName)
{
  load(fileName);
}

void IniSettings::load(const WideString& fileName)
{
  IniSettingsStore iss;
  iss.load(fileName, *this);
}

void IniSettings::save(const WideString& fileName)
{
  IniSettingsStore iss;
  iss.save(fileName, *this);
}
