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

#include "GjBitmapFont.h"
#include "GjUnicodeUtils.h"
#include "GjResourceManagement.h"
#include "GjIniFiles.h"
#include "GjStringUtils.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <cstdlib>
#include <cmath>
using namespace yaglib;

BMFontMeta::BMFontMeta(const WideString& fileName)
{
  load(fileName);
}

BMFontMeta::~BMFontMeta()
{
}

void BMFontMeta::load(const WideString& fileName)
{
  //
  // keywords are the following:
  // -- info, common, page <fileName>, chars, char, kernings, kerning
  //
  std::ifstream source(UTF8String(fileName).c_str());
  std::string s;
  while(getline(source, s))
  {
    // comment encountered, ignore it
    boost::trim(s);
    if(s.size() == 0)
      continue;
    // determine the keyword
    size_t space = s.find(' ');
    if(space == std::string::npos)
      continue;
    // possible keyword, extract it
    std::string keyword = s.substr(0, space);
    s = s.substr(space+1);
    // 
    if(keyword == "info")
      parseInfo(s);
    else if(keyword == "common")
      parseCommon(s);
    else if(keyword == "page")
      parsePage(s);
    else if(keyword == "chars")
      parseChars(s);
    else if(keyword == "char")
      parseChar(s);
    else if(keyword == "kernings")
      parseKernings(s);
    else if(keyword == "kerning")
      parseKerning(s);
  }
}

void BMFontMeta::parseInfo(std::string raw)
{
  // this is nice, but we don't need this yet
}

void BMFontMeta::parsePage(std::string raw)
{
  std::string fileName = raw.substr(raw.find("file=")+6);
  boost::trim(fileName);
  //
  mTextureFileName = from_char_p(fileName.substr(0, fileName.size()-1).c_str());
  string_utils::chop_after_last(mTextureFileName, WideString(L"."), false);
}

void BMFontMeta::parseCommon(std::string raw)
{
  raw = raw.substr(raw.find('=')+1);
  std::string s = raw.substr(0, raw.find(' '));
  mLineHeight = static_cast<GJFLOAT>(atof(s.c_str()));

  raw = raw.substr(raw.find('=')+1);
  s = raw.substr(0, raw.find(' '));
  mBaseLine = static_cast<GJFLOAT>(atof(s.c_str()));

  raw = raw.substr(raw.find('=')+1);
  s = raw.substr(0, raw.find(' '));
  mScaleFactor.x = static_cast<GJFLOAT>(atof(s.c_str()));

  raw = raw.substr(raw.find('=')+1);
  s = raw.substr(0, raw.find(' '));
  mScaleFactor.y = static_cast<GJFLOAT>(atof(s.c_str()));
}

void BMFontMeta::parseChars(std::string raw)
{
  // it's not quite important to know how many characters there are...
}

void BMFontMeta::parseChar(std::string raw)
{
  BMChar bmc;

  raw = raw.substr(raw.find('=')+1); 
  std::string s = raw.substr(0, raw.find(' '));
  int charId = atoi(s.c_str());

  raw = raw.substr(raw.find('=')+1); 
  s = raw.substr(0, raw.find(' '));
  GJFLOAT left = static_cast<GJFLOAT>(atof(s.c_str()));

  raw = raw.substr(raw.find('=')+1); 
  s = raw.substr(0, raw.find(' '));
  GJFLOAT top = static_cast<GJFLOAT>(atof(s.c_str()));

  raw = raw.substr(raw.find('=')+1); 
  s = raw.substr(0, raw.find(' '));
  GJFLOAT right = left + static_cast<GJFLOAT>(atof(s.c_str()));

  raw = raw.substr(raw.find('=')+1); 
  s = raw.substr(0, raw.find(' '));
  GJFLOAT bottom = top + static_cast<GJFLOAT>(atof(s.c_str()));

  bmc.rTexture = bmc.rScreen = bmc.rBase = GJRECT(left, top, right, bottom);  
  bmc.rScreen.normalize();
  //bmc.rTexture -= 0.5;
  bmc.rTexture.scale(1/mScaleFactor.x, 1/mScaleFactor.y);
  
  raw = raw.substr(raw.find('=')+1); 
  s = raw.substr(0, raw.find(' '));
  bmc.offset.x = static_cast<GJFLOAT>(atof(s.c_str()));

  raw = raw.substr(raw.find('=')+1); 
  s = raw.substr(0, raw.find(' '));
  bmc.offset.y = static_cast<GJFLOAT>(atof(s.c_str()));

  raw = raw.substr(raw.find('=')+1); 
  s = raw.substr(0, raw.find(' '));
  bmc.xAdvance = atoi(s.c_str());

  mCharMap[charId] = bmc;
}

void BMFontMeta::parseKernings(std::string raw)
{
}

void BMFontMeta::parseKerning(std::string raw)
{
  raw = raw.substr(raw.find('=')+1); 
  std::string s = raw.substr(0, raw.find(' '));
  TCHAR firstId = static_cast<TCHAR>(atoi(s.c_str()));

  raw = raw.substr(raw.find('=')+1); 
  s = raw.substr(0, raw.find(' '));
  TCHAR secondId = static_cast<TCHAR>(atoi(s.c_str()));

  raw = raw.substr(raw.find('=')+1); 
  GJFLOAT adjustment = static_cast<GJFLOAT>(atof(raw.c_str()));

  KerningMap::iterator km = mKerningMap.empty() ? mKerningMap.end() : mKerningMap.find(firstId);
  if(km == mKerningMap.end())
  {
    IntMap im;
    im[secondId] = adjustment;
    mKerningMap[firstId] = im;
  }
  else
  {
    km->second[secondId] = adjustment;
  }
}

BMChar const& BMFontMeta::getCharInfo(TCHAR ch) const
{
  static BMChar blank = {GJRECT(), GJRECT(),GJRECT(),GJPOINT(), 0};
  CharMap::const_iterator iter = mCharMap.empty() ? mCharMap.end() : mCharMap.find(ch);
  if(iter == mCharMap.end())
    return blank;
  else
    return iter->second;
}

GJFLOAT BMFontMeta::getKerningAdjustment(TCHAR ch1, TCHAR ch2) const
{
  KerningMap::const_iterator km = mKerningMap.empty() ? mKerningMap.end() : mKerningMap.find(ch1);
  if(km != mKerningMap.end())
  {
    IntMap::const_iterator im = km->second.empty() ? km->second.end() : km->second.find(ch2);
    if(im != km->second.end())
      return im->second;
  }

  return 0;
}

BMFont::BMFont(BMFontMeta* bmfMeta, const bool ownsMetaData) : 
  mFontMeta(bmfMeta), mOwnsMeta(ownsMetaData)
{
  mLineHeight = bmfMeta->getLineHeight();
  mBaseLine = bmfMeta->getBaseLine();
}

BMFont::~BMFont()
{
  if(mOwnsMeta)
    SAFE_DELETE(mFontMeta);
}

void BMFont::drawSingle(const WideString text, const GJRECT& rScreen, DrawItems& items, int options)
{
  // no sense going thru the hoops for an empty string
  if(text.size() == 0)
    return;

  // preps
  items.clear();
  GJPOINT cursor = rScreen.getTopLeft();

  // prep some vars we'll need in the loop
  TCHAR lastChar = 0;
  BMDrawItem di;
  WideString::const_iterator lastCharacter = text.end()-1;
  for(WideString::const_iterator iter = text.begin(); iter != text.end(); iter++)
  {
    // retrieve, and adjust the rectangles we need
    TCHAR ch = *iter;
    BMChar bmc = mFontMeta->getCharInfo(ch);
    GJRECT rFinal = bmc.rScreen;
    rFinal.translate(cursor);
    rFinal.translate(bmc.offset);
    // store the rectangles to draw
    di.rScreen = rFinal;
    di.rTexture = bmc.rTexture;
    items.push_back(di);
    // advance the cursor
    cursor.x += bmc.xAdvance;
    if(lastChar && (iter != lastCharacter))
    {
      GJFLOAT adjustment = mFontMeta->getKerningAdjustment(lastChar, ch);
      cursor.x += adjustment;
    }

    lastChar = ch;
  }

  // adjust rectangles depending on the justifications
  if(options & HJUST_RIGHT)
  {
    DrawItems::iterator last = items.end()-1;
    GJFLOAT delta = rScreen.right - last->rScreen.right;
    for(DrawItems::iterator iter = items.begin(); iter != items.end(); iter++)
      iter->rScreen.translate_h(delta);
  }
  else if(options & HJUST_CENTER)
  {
    GJFLOAT delta = floor((rScreen.right - cursor.x)/2);
    for(DrawItems::iterator iter = items.begin(); iter != items.end(); iter++)
      iter->rScreen.translate_h(delta);
  }

  // vertical adjustments
  if(options & VJUST_BOTTOM)
  {
    GJFLOAT delta = rScreen.height - mLineHeight;
    for(DrawItems::iterator iter = items.begin(); iter != items.end(); iter++)
      iter->rScreen.translate_v(delta);
  }
  else if(options & VJUST_CENTER)
  {
    GJFLOAT delta = floor((rScreen.height - mLineHeight)/2);
    for(DrawItems::iterator iter = items.begin(); iter != items.end(); iter++)
      iter->rScreen.translate_v(delta);
  }

  // do the draw
  //for(DrawItems::iterator iter = items.begin(); iter != items.end(); iter++)
  //  mSprite->draw(iter->rScreen, iter->rTexture, color);

}

#define SECTION_FONTS               L"fonts"
#define SECTION_ALIASES             L"aliases"
#define SECTION_CONFIGURATION_FILE  L"fonts.cfg"

bool FontManager::initialize()
{
  WideString configFile = g_ResourceManager[SECTION_CONFIGURATION_FILE];
  if(configFile.size() == 0)
    return false;

  IniSettings settings(configFile);
  if(settings.exists(SECTION_FONTS))
  {
    Settings fontItems = settings[SECTION_FONTS];
    for(int i = 0; i < static_cast<int>(fontItems.size()); i++)
    {
      Setting item = fontItems[i];
      WideString qualifiedName = g_ResourceManager[item.getValue()];
      if(qualifiedName.size() != 0)
        mFonts[item.getValue()] = new BMFont(new BMFontMeta(qualifiedName));
    }
  }

  if(settings.exists(SECTION_ALIASES))
  {
    Settings aliases = settings[SECTION_ALIASES];
    for(int i = 0; i < static_cast<int>(aliases.size()); i++)
    {
      Setting item = aliases[i];
      //
      int color = 0xffffffff;
      WideString value = item.getValue();
      size_t comma = value.find(',');
      if(comma != WideString::npos)
      {
        color = string_utils::parse_hex_int(value.substr(comma+1));
        value = value.substr(0, comma);
      }
      //
      BMFonts::iterator bmfi = mFonts.empty() ? mFonts.end() : mFonts.find(value);
      if(bmfi != mFonts.end())
        mFontList[item.getName()] = FontItem(bmfi->second, color);
    }
  }

  return true;
}

void FontManager::shutdown()
{
  mFontList.clear();
  //
  for(BMFonts::iterator iter = mFonts.begin(); iter != mFonts.end(); iter++)
    SAFE_DELETE(iter->second);
  mFonts.clear();
}

FontItem* FontManager::get(const WideString name)
{
  FontList::iterator fi = mFontList.empty() ? mFontList.end() : mFontList.find(name);
  return (fi == mFontList.end()) ? NULL : &(fi->second);
}

