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
 * @file  GjSprites.h
 * @brief Sprite-related stuff
 *
 * note that sprite names MUST be unique for the whole application.
 *
 */
#ifndef GJ_SPRITES_HEADER
#define GJ_SPRITES_HEADER

#include "GjDefs.h"
#include "GjPoints.h"
#include "GjColors.h"
#include "GjTemplates.h"
#include "GjTextures.h"
#include "GjMetaData.h"
#include "GjBitmapFont.h"

namespace yaglib 
{

const int CHANGE_TO_FIRST_FRAME = 0;
const int CHANGE_TO_LAST_FRAME  = -1;
const int CHANGE_TO_NEXT_FRAME  = -2;
const int CHANGE_TO_PREV_FRAME  = -3;


class Sprite 
{
public:
  Sprite(const WideString spriteName);
  virtual ~Sprite();

  Texture const* getTexture() const { return mTexture; };
  SpriteMeta const* getInfo() const { return mInfo; };
  TextureMeta const* getTextureInfo() const { return mTextureInfo; };

  GJPOINT3 const& getPosition() const { return mPosition; };
  GJSIZE const& getSize() const { return mSize; };

  void setPosition(const GJPOINT3 p);
  void setPosition(const GJFLOAT x, const GJFLOAT y, const GJFLOAT z = 0.0f)
    { setPosition(GJPOINT3(x, y, z)); };
  void move(const GJFLOAT dx, const GJFLOAT dy, const GJFLOAT dz = 0.0f);

  void setSize(const GJSIZE size);
  void setSize(const GJFLOAT w, const GJFLOAT h) { setSize(GJSIZE(w, h)); };

  ColorQuad const& getColor() const { return mColor; };
  void setColor(const ColorQuad newColor);

  void changeFrame(const int whichFrame);
  GJRECT const& getFrameBounds() const;
  GJRECT const& getBounds() const { return mBounds; };

  virtual void draw() = 0;
  void draw(const GJFLOAT x, const GJFLOAT y, const GJFLOAT z = 0.0f);
  void drawRelative(const GJFLOAT dx, const GJFLOAT dy, const GJFLOAT dz);

protected:
  Texture* mTexture;
  SpriteMeta const* mInfo;
  TextureMeta const* mTextureInfo;

  GJPOINT3 mPosition;
  GJSIZE mSize;
  GJRECT mBounds;
  int mActiveFrame;
  ColorQuad mColor;
  GJRECT mTexelRect;

  virtual void frameChanged();
  virtual void colorChanged() {};
  virtual void positionChanged() {};
  virtual void sizeChanged() {};
};

class MultiBlitSprite : public Sprite
{
public:
  MultiBlitSprite(const WideString spriteName);
  virtual ~MultiBlitSprite();

  void beginUpdate();
  void add(const GJRECT bounds, const ColorQuad color, const int frameIndex);
  void endUpdate();

protected:
  struct BlitData
  {
    ColorQuad color;
    GJRECT frameBounds;
    int frameIndex;
    //
    BlitData() {};
    BlitData(const GJRECT r, const ColorQuad c, const int idx) :
      frameBounds(r), color(c), frameIndex(idx) {};
  };
  typedef std::vector<BlitData> BlitList;
  BlitList mBlitList;

  virtual void blitListUpdated() = 0;
};


class FontSprite : public Sprite
{
public:
  FontSprite(FontItem* fItem);
  virtual ~FontSprite() {};

  WideString const& getText() const { return mText; };
  void setText(const WideString newText) { mText = newText; textChanged(); };
  int getTextDrawOptions() const { return mTextDrawOptions; };
  void setTextDrawOptions(const int newOptions);

protected:
  WideString mText;
  FontItem* mFontItem;
  DrawItems mTextData;
  int mTextDrawOptions;

  virtual void textChanged();
  virtual void textOptionsChanged() {};
};

class SpriteFactory
{
public:
  virtual Sprite* createSprite(const WideString spriteName) = 0;
  virtual MultiBlitSprite* createMultiBlitSprite(const WideString spriteName) = 0;
  virtual FontSprite* createFontSprite(const WideString fontName) = 0;
};


} /* namespace yaglib */

#endif /* GJ_SPRITES_HEADER */
