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
 * @file  GjPanels.h
 * @brief Panels
 *
 */

#ifndef GJ_PANELS_HEADER
#define GJ_PANELS_HEADER

#include "GjBaseView.h"

namespace yaglib
{

class SimplePanel : public BaseView
{
public:
  SimplePanel(BaseView* owner, const GJRECT& bounds, SpriteFactory* spriteFactory, const WideString spriteName = L"");

protected:
  virtual void drawThis();
};

const int PANEL_INDEX_UPPER_LEFT    = 1;
const int PANEL_INDEX_UPPER_RIGHT   = 2;
const int PANEL_INDEX_LOWER_LEFT    = 3;
const int PANEL_INDEX_LOWER_RIGHT   = 4;
const int PANEL_INDEX_LEFT_EDGE     = 5;
const int PANEL_INDEX_RIGHT_EDGE    = 6;
const int PANEL_INDEX_TOP_EDGE      = 7;
const int PANEL_INDEX_BOTTOM_EDGE   = 8;

const int NUMBER_OF_PANEL_FRAMES    = 9;
const int FIRST_PANEL_FRAME_INDEX   = 1;  
const int LAST_PANEL_FRAME_INDEX    = 8;

class BorderedPanel : public BaseView
{
public:
  BorderedPanel(BaseView* owner, const GJRECT& bounds, SpriteFactory* spriteFactory, const WideString spriteName = L"");

protected:
  virtual void drawThis();
  virtual void checkAndSetBounds(const GJRECT& bounds);
  virtual void boundsChanged();

private:
  MultiBlitSprite* mMBSprite;
  GJRECT mMinimumSize;
  GJRECT mFrameSize;
  GJRECT mFrames[NUMBER_OF_PANEL_FRAMES];

  bool mFramesInitialized;
  void calculateMinimumSize();
  void calculateFrameBounds();
  void updateInternalSprite();
  virtual bool loadSprite();
};

class DynamicPanel : public BaseView
{
public: 
  DynamicPanel(BaseView* owner, const GJRECT& bounds, SpriteFactory* spriteFactory, const WideString spriteName);

protected:
  virtual void drawThis();
};

}; /* namespace yaglib */

#endif /* GJ_PANELS_HEADER */
