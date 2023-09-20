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
 * @file  GjBitmapFont.h
 * @brief Bitmap font support
 *
 * Uses data files from Bitmap FontWriter Generator (www.angelcode.com)
 *
 */
#ifndef GJ_BITMAP_fONT_HEADER
#define GJ_BITMAP_fONT_HEADER

#include "GjDefs.h"
#include "GjTemplates.h"
#include "GjRectangles.h"

namespace yaglib 
{

typedef struct tagBMChar
{
  GJRECT rBase;
  GJRECT rScreen;
  GJRECT rTexture;
  GJPOINT offset;
  int xAdvance;
} BMChar;

class BMFontMeta
{
public:
  BMFontMeta(const WideString& fileName);
  virtual ~BMFontMeta();

  BMChar const& getCharInfo(TCHAR ch) const;
  GJFLOAT getKerningAdjustment(TCHAR ch1, TCHAR ch2) const;
  GJFLOAT getLineHeight() const { return mLineHeight; };
  GJFLOAT getBaseLine() const { return mBaseLine; };
  WideString const& getTextureFileName() const { return mTextureFileName; };

  // character map
  typedef std::map<TCHAR,BMChar> CharMap;
  const CharMap& getCharMap() const { return mCharMap; };

private:
  // general information
  WideString mTextureFileName;
  GJFLOAT mLineHeight;
  GJFLOAT mBaseLine;
  GJPOINT mScaleFactor;

  CharMap mCharMap;

  // kerning map
  typedef std::map<TCHAR, GJFLOAT> IntMap;
  typedef std::map<TCHAR, IntMap> KerningMap;
  KerningMap mKerningMap;

  void load(const WideString& fileName);
  void parseInfo(std::string raw);
  void parsePage(std::string raw);
  void parseCommon(std::string raw);
  void parseChars(std::string raw);
  void parseChar(std::string raw);
  void parseKernings(std::string raw);
  void parseKerning(std::string raw);
};

struct BMDrawItem
{
  GJRECT rScreen;
  GJRECT rTexture;
};

typedef std::vector<BMDrawItem> DrawItems;

const int HJUST_LEFT    = 0x0000;
const int HJUST_RIGHT   = 0x0001;
const int HJUST_CENTER  = 0x0002;
const int VJUST_TOP     = 0x0000;
const int VJUST_BOTTOM  = 0x0010;
const int VJUST_CENTER  = 0x0020;

const int FD_DEFAULTS   = 0x0000;

class BMFont
{
public:
  BMFont(BMFontMeta* bmfMeta, const bool ownsMetaData = true);
  virtual ~BMFont();

  void drawSingle(const WideString text, const GJRECT& rScreen, DrawItems& items, int options = 0);
  BMFontMeta* getFontMeta() { return mFontMeta; };

private:
  BMFontMeta* mFontMeta;
  bool mOwnsMeta;
  // for efficiency
  GJFLOAT mLineHeight;
  GJFLOAT mBaseLine;
};

struct FontItem
{
  BMFont* font;
  int color;
  FontItem() : font(NULL), color(0) {};
  FontItem(BMFont* _font, const int _color) : font(_font), color(_color) {};
};

class FontManager : public Singleton<FontManager>
{
public:
  virtual ~FontManager() { shutdown(); };

  bool initialize();
  void shutdown();

  FontItem* get(const WideString name);

private:
  typedef std::map<WideString, BMFont*> BMFonts;
  BMFonts mFonts;
  typedef std::map<WideString, FontItem> FontList;
  FontList mFontList;
};

#define g_FontManager (FontManager::Instance())

} /* namespace yaglib */

#endif /* GJ_BITMAP_fONT_HEADER */
