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
 * @file  GjMetaData.h
 * @brief MetaData structures/classes
 *
 */
#ifndef GJ_META_DATA_HEADER
#define GJ_META_DATA_HEADER

#include "GjDefs.h"
#include "GjColors.h"
#include "GjPoints.h"
#include "GjRectangles.h"
#include "GjTemplates.h"

namespace yaglib 
{                    

#define IMAGE_CONFIG_FILENAME     L"image.info"
#define ALIASES_SECTION           L"|aliases|"
#define SPRITES_CONFIG_FILENAME   L"sprites.info"
#define SPRITES_MAIN_SECTION      L"main"

struct TextureMeta
{
  Size size;
  ColorQuad transparentColor;
  GJRECTS byPixels;
  GJRECTS byTexels;
  WideString fileName;
  //
  TextureMeta() : transparentColor(0), size(Size(0)) {};
  bool isTransparent() const { return (int)transparentColor != 0; };
};

struct SpriteMeta
{
  WideString textureName;
  int firstFrame;
  int lastFrame;
  int frameCount;
  //
  SpriteMeta() : firstFrame(-1), lastFrame(-1), frameCount(0) {};
  SpriteMeta(const WideString _textureName, int _firstFrame, int _lastFrame) : 
    textureName(_textureName), firstFrame(_firstFrame), 
    lastFrame(_lastFrame), frameCount(_lastFrame - _firstFrame + 1) {};
};

class MetaDataManager : public Singleton<MetaDataManager>
{
public:
  void initialize();
  void shutdown();

  TextureMeta const* getTextureMeta(const WideString textureName) const;
  SpriteMeta const* getSpriteMeta(const WideString spriteName) const;

private:
  typedef std::map<WideString, TextureMeta> TextureMetaMap;
  typedef std::map<WideString, SpriteMeta>  SpriteMetaMap;
  typedef std::map<WideString, WideString>  StringMap;
  //
  TextureMetaMap mTextures;
  StringMap mTextureAliases;
  SpriteMetaMap mSprites;
  //
  void loadTextureMeta();
  void processTextureMetaFile(const WideString fileName);
  void loadSpriteMeta();
  void processSpriteMetaFile(const WideString fileName);
};

#define g_MetaDataManager (MetaDataManager::Instance())


} /* namespace yaglib */

#endif /* GJ_META_DATA_HEADER */
