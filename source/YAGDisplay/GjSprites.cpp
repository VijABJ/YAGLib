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

#include "GjSprites.h"
#include "GjClipper.h"
#include "GjResourceManagement.h"
#include "GjIniFiles.h"
#include "GjUnicodeUtils.h"
using namespace yaglib;

Sprite::Sprite(const WideString spriteName) : mTexture(NULL), mInfo(NULL),
  mTextureInfo(NULL), mPosition(GJPOINT3()), mSize(GJSIZE()), mBounds(GJRECT()),
  mActiveFrame(0), mTexelRect(GJRECT()), mColor(0xffffffff)
{
  mInfo = g_MetaDataManager.getSpriteMeta(spriteName);
  mTexture = g_TextureManager[mInfo->textureName];
  mTextureInfo = g_MetaDataManager.getTextureMeta(mInfo->textureName);
  frameChanged();
}

Sprite::~Sprite()
{
  // nothing to do, for now
}

void Sprite::setPosition(const GJPOINT3 p)
{
  if(mPosition != p)
  {
    mPosition = p;
    mBounds = GJRECT(mPosition.x, mPosition.y, mPosition.x+mSize.width, mPosition.y+mSize.height);
    positionChanged();
  }
}

void Sprite::move(const GJFLOAT dx, const GJFLOAT dy, const GJFLOAT dz)
{
  mPosition += GJPOINT3(dx, dy, dz);
  mBounds = GJRECT(mPosition.x, mPosition.y, mPosition.x+mSize.width, mPosition.y+mSize.height);
  positionChanged();
}

void Sprite::setSize(const GJSIZE size)
{
  if(mSize != size)
  {
    mSize = size;
    mBounds = GJRECT(mPosition.x, mPosition.y, mPosition.x+mSize.width, mPosition.y+mSize.height);
    sizeChanged();
  }
}

void Sprite::draw(const GJFLOAT x, const GJFLOAT y, const GJFLOAT z)
{
  setPosition(GJPOINT3(x, y, z));
  draw();
}

void Sprite::drawRelative(const GJFLOAT dx, const GJFLOAT dy, const GJFLOAT dz)
{
  move(dx, dy, dz);
  draw();
}

void Sprite::setColor(const ColorQuad newColor)
{
  if(mColor != newColor)
  {
    mColor = newColor;
    colorChanged();
  }
}

void Sprite::frameChanged()
{
  int frameIndex = mActiveFrame + mInfo->firstFrame;
  // if the screen bounds is zero width, or height, use the texture frame bounds
  if((mBounds.width <= 0.0f) || (mBounds.height <= 0.0f))
  {
    mBounds = mTextureInfo->byPixels[frameIndex];
    mBounds.normalize();
    mSize = GJSIZE(mBounds.width, mBounds.height);
  }
  mTexelRect = mTextureInfo->byTexels[frameIndex];
}

void Sprite::changeFrame(const int whichFrame)
{
  switch(whichFrame)
  {
  case CHANGE_TO_LAST_FRAME:
    mActiveFrame = mInfo->lastFrame-1;
    break;
  case CHANGE_TO_NEXT_FRAME:
    if(mInfo->frameCount > 1)
    {
      mActiveFrame++;
      if(mActiveFrame >= mInfo->lastFrame)
        mActiveFrame = 0;
    }
    break;
  case CHANGE_TO_PREV_FRAME:
    if(mInfo->frameCount > 1)
    {
      if(mActiveFrame == 0)
        mActiveFrame = mInfo->lastFrame-1;
      else
        mActiveFrame--;
    }
    break;
  default:
    if((whichFrame >= 0) && (whichFrame <= mInfo->lastFrame) && (whichFrame != mActiveFrame))
      mActiveFrame = whichFrame;
    else
      return;
    break;
  }
  frameChanged();
}

GJRECT const& Sprite::getFrameBounds() const
{
  int frameIndex = mActiveFrame + mInfo->firstFrame;
  return mTextureInfo->byPixels[frameIndex];
}

/*
void Sprite::draw(const GJRECT& rDest, const GJRECT& rSource, DWORD color)
{
  loadTexture();
  GJRECT rd = rDest, rs = rSource;
  int result = g_Clipper.clip(rd);
  if(result)
  {
    #define RATIO_ADJUST(coord, size)   rs.coord += ((rd.coord - rDest.coord) / rDest.size) * rSource.size

    if(result & CLIPPED_VS_LEFT)
      RATIO_ADJUST(left, width);
    if(result & CLIPPED_VS_RIGHT)
      RATIO_ADJUST(right, width);
    if(result & CLIPPED_VS_TOP)
      RATIO_ADJUST(top, height);
    if(result & CLIPPED_VS_BOTTOM)
      RATIO_ADJUST(bottom, height);
  }
  g_TextureManager.getRenderer()->add(mTexture, rd, rs, color); 
}
*/

MultiBlitSprite::MultiBlitSprite(const WideString spriteName) : Sprite(spriteName)
{
}

MultiBlitSprite::~MultiBlitSprite()
{
}

void MultiBlitSprite::beginUpdate()
{
  mBlitList.clear();
}

void MultiBlitSprite::add(const GJRECT bounds, const ColorQuad color, const int frameIndex)
{
  mBlitList.push_back(BlitData(bounds, color, frameIndex));
}

void MultiBlitSprite::endUpdate()
{
  blitListUpdated();
}


FontSprite::FontSprite(FontItem* fItem) : Sprite(fItem->font->getFontMeta()->getTextureFileName()), 
  mText(L""), mFontItem(fItem), mTextDrawOptions(0)
{
  mColor = fItem->color;
}

void FontSprite::setTextDrawOptions(const int newOptions)
{
  if(mTextDrawOptions != newOptions)
  {
    mTextDrawOptions = newOptions;
    textOptionsChanged();
  }
}

void FontSprite::textChanged()
{
  mFontItem->font->drawSingle(mText, 
    GJRECT(0.0f, 0.0f, mSize.width, mSize.height), mTextData, mTextDrawOptions);
}
