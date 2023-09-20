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

#include "GjPanels.h"
using namespace yaglib;

SimplePanel::SimplePanel(BaseView* owner, const GJRECT& bounds, SpriteFactory* spriteFactory, 
  const WideString spriteName) : BaseView(owner, bounds, spriteFactory, spriteName)
{
}

void SimplePanel::drawThis()
{
  if(loadSprite())
    mSprite->draw();

  GJRECT r = mLocalBounds;
  toGlobal(r);
  writeCaption(r, HJUST_CENTER|VJUST_CENTER);
}

BorderedPanel::BorderedPanel(BaseView* owner, const GJRECT& bounds, SpriteFactory* spriteFactory, 
  const WideString spriteName) : BaseView(owner, bounds, spriteFactory, spriteName), 
  mFramesInitialized(false), mMBSprite(NULL)
{
}

bool BorderedPanel::loadSprite()
{
  if((mMBSprite == NULL) && (mSpriteName.size() > 0))
  {
    mMBSprite = mSpriteSource->createMultiBlitSprite(mSpriteName);
    mSprite = mMBSprite;
    calculateMinimumSize();
  }
  return mMBSprite != NULL;
}

void BorderedPanel::updateInternalSprite()
{
  if(mMBSprite)
  {
    GJPOINT p(0, 0); toGlobal(p);
    mMBSprite->setPosition(p.x, p.y, mZValue);
    mMBSprite->beginUpdate();
    for(int i = FIRST_PANEL_FRAME_INDEX; i <= LAST_PANEL_FRAME_INDEX; i++)
    {
      GJRECT target = mFrames[i];
      mMBSprite->add(target, 0xffffffff, i);
    }
    mMBSprite->endUpdate();
  }
}

void BorderedPanel::drawThis()
{
  if(loadSprite())
    mSprite->draw();

  GJRECT r = mLocalBounds;
  toGlobal(r);
  writeCaption(r, HJUST_CENTER|VJUST_CENTER);
}

void BorderedPanel::calculateFrameBounds()
{
  for(int i = 0; i < NUMBER_OF_PANEL_FRAMES; i++)
    mFrames[i] = mFrameSize;

  GJFLOAT diffWidth = mLocalBounds.width - mFrameSize.width,
          diffHeight = mLocalBounds.height - mFrameSize.height;

  mFrames[PANEL_INDEX_UPPER_RIGHT].translate_h(diffWidth);
  mFrames[PANEL_INDEX_LOWER_LEFT].translate_v(diffHeight);
  mFrames[PANEL_INDEX_LOWER_RIGHT].translate(diffWidth, diffHeight);

  mFrames[PANEL_INDEX_LEFT_EDGE].translate_v(mFrameSize.height);
  mFrames[PANEL_INDEX_LEFT_EDGE].setHeight(diffHeight - mFrameSize.height);
  mFrames[PANEL_INDEX_RIGHT_EDGE].translate(diffWidth, mFrameSize.height);
  mFrames[PANEL_INDEX_RIGHT_EDGE].setHeight(diffHeight - mFrameSize.height);

  mFrames[PANEL_INDEX_TOP_EDGE].translate_h(mFrameSize.width-2);
  mFrames[PANEL_INDEX_TOP_EDGE].setWidth(diffWidth - mFrameSize.width + 2);
  mFrames[PANEL_INDEX_BOTTOM_EDGE].translate(mFrameSize.width-2, diffHeight);
  mFrames[PANEL_INDEX_BOTTOM_EDGE].setWidth(diffWidth - mFrameSize.width + 2);

  updateInternalSprite();
}

void BorderedPanel::boundsChanged()
{
  BaseView::boundsChanged();
  if(mSprite != NULL)
    calculateFrameBounds();
}

void BorderedPanel::checkAndSetBounds(const GJRECT& bounds)
{
  GJRECT r = bounds;
  if(mSprite != NULL)
  {
    if(r.width < mMinimumSize.width)
      r.setWidth(mMinimumSize.width);
    if(r.height < mMinimumSize.height)
      r.setHeight(mMinimumSize.height);
  }

  BaseView::checkAndSetBounds(r);
}

void BorderedPanel::calculateMinimumSize()
{
  if(mSprite == NULL)
  {
    mMinimumSize = GJRECT(0, 0, 0, 0);
    mFrameSize = GJRECT(0, 0, 0, 0);
  }
  else
  {
    mFrameSize = mSprite->getTextureInfo()->byPixels[PANEL_INDEX_UPPER_LEFT];
    mFrameSize.normalize();
    mMinimumSize = GJRECT(0, 0, mFrameSize.width*3, mFrameSize.height*3);
    if((mBounds.width < mMinimumSize.width) || (mBounds.height < mMinimumSize.height))
      checkAndSetBounds(mBounds);

    calculateFrameBounds();
  }
}


DynamicPanel::DynamicPanel(BaseView* owner, const GJRECT& bounds, SpriteFactory* spriteFactory, 
  const WideString spriteName) : BaseView(owner, bounds, spriteFactory, spriteName)
{
  setRequiredOptions(VO_AUTOSIZE);
}

void DynamicPanel::drawThis()
{
  if(loadSprite())
    mSprite->draw();
}
