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

#include "GjMetaData.h"
#include "GjIniFiles.h"
#include "GjResourceManagement.h"
#include "GjUnicodeUtils.h"
#include "GjStringUtils.h"
#include <boost/algorithm/string.hpp>
using namespace yaglib;


void MetaDataManager::initialize()
{
  loadTextureMeta();
  loadSpriteMeta();
}

void MetaDataManager::shutdown()
{
  mTextures.clear();
  mTextureAliases.clear();
  mSprites.clear();
}

TextureMeta const* MetaDataManager::getTextureMeta(const WideString textureName) const
{
  StringMap::const_iterator iter = mTextureAliases.empty() ? mTextureAliases.end() : mTextureAliases.find(textureName);
  WideString alias = (iter != mTextureAliases.end()) ? iter->second : textureName;

  TextureMetaMap::const_iterator subj = mTextures.empty() ? mTextures.end() : mTextures.find(alias);
  return subj != mTextures.end() ? &(subj->second) : NULL;
}

SpriteMeta const* MetaDataManager::getSpriteMeta(const WideString spriteName) const
{
  SpriteMetaMap::const_iterator subj = mSprites.empty() ? mSprites.end() : mSprites.find(spriteName);
  return subj != mSprites.end() ? &(subj->second) : NULL;
}

void MetaDataManager::loadTextureMeta()
{
  DataPacks allPacks;
  g_ResourceManager.lookup(allPacks, IMAGE_CONFIG_FILENAME, true);
  for(DataPacks::iterator iter = allPacks.begin(); iter != allPacks.end(); iter++)
    processTextureMetaFile(iter->getFileName());
}

#define ENTRY_NAME_TRANSPARENT_COLOR    L"transparentColor"
#define ENTRY_NAME_WIDTH                L"width"
#define ENTRY_NAME_HEIGHT               L"height"

void MetaDataManager::processTextureMetaFile(const WideString fileName)
{
  IniSettings ini(fileName);
  for(int i = 0; i < static_cast<int>(ini.size()); i++)
  {
    // skip frame lists, we'll acquire them concurrent with the actual texture info
    Settings section = ini[i];
    WideString sectionName = section.getName();
    if(sectionName[0] == '#')
      continue;
    // check for the aliases section
    if(sectionName == ALIASES_SECTION)
    {
      for(int j = 0; j < static_cast<int>(section.size()); j++)
        mTextureAliases[section[j].getName()] = section[j].getValue();
      continue;
    }
    // make sure there is a file for this
    DataPack dp;
    int result = g_ResourceManager.lookup(dp, sectionName, GROUP_NAME_ANY, true);
    if(!result)
      continue;

    TextureMeta tm;
    tm.fileName = dp.getFileName();

    // get the transparent color if it's there
    if(section.exists(ENTRY_NAME_TRANSPARENT_COLOR))
      tm.transparentColor = string_utils::parse_hex_int(section[ENTRY_NAME_TRANSPARENT_COLOR].getValue());
    
    // get the width and the height
    tm.size.width = string_utils::parse_int(section[ENTRY_NAME_WIDTH].getValue()),
    tm.size.height = string_utils::parse_int(section[ENTRY_NAME_HEIGHT].getValue());

    // now we need to parse the frames
    section = ini[L"#" + sectionName];
    for(int j = 0; j < static_cast<int>(section.size()); j++)
    {
      typedef std::vector<WideString> split_list;
      split_list items;
      boost::split(items, section[j].getValue(), boost::is_any_of(L","), boost::token_compress_on);
      GJRECT r = GJRECT(
        static_cast<GJFLOAT>(string_utils::parse_int(items[0])),
        static_cast<GJFLOAT>(string_utils::parse_int(items[1])),
        static_cast<GJFLOAT>(string_utils::parse_int(items[2])),
        static_cast<GJFLOAT>(string_utils::parse_int(items[3]))
        );
      GJRECT r2 = r;
      r2.scale(1/static_cast<GJFLOAT>(tm.size.width), 1/static_cast<GJFLOAT>(tm.size.height));
      //
      tm.byPixels.add(r);
      tm.byTexels.add(r2);
    }
    // all done, store it
    mTextures[sectionName] = tm;
  }
}

void MetaDataManager::loadSpriteMeta()
{
  DataPacks allPacks;
  g_ResourceManager.lookup(allPacks, SPRITES_CONFIG_FILENAME, true);
  for(DataPacks::iterator iter = allPacks.begin(); iter != allPacks.end(); iter++)
    processSpriteMetaFile(iter->getFileName());
}

void MetaDataManager::processSpriteMetaFile(const WideString fileName)
{
  IniSettings ini(fileName);
  if(ini.exists(SPRITES_MAIN_SECTION))
  {
    Settings section = ini[SPRITES_MAIN_SECTION];
    for(int i = 0; i < static_cast<int>(section.size()); i++)
    {
      typedef std::vector<WideString> split_list;
      split_list items;
      boost::split(items, section[i].getValue(), boost::is_any_of(L","), boost::token_compress_on);
      if(items.size() < 2)
        items.push_back(L"0");
      if(items.size() < 3)
        items.push_back(items[1]);
      //
      mSprites[section[i].getName()] = SpriteMeta(items[0], 
        string_utils::parse_int(items[1]), string_utils::parse_int(items[2]));
    }
  }
}
