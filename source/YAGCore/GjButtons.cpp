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

#include "GjButtons.h"
using namespace yaglib;

CommandGenerator::CommandGenerator(BaseView* owner, const GJRECT& bounds, SpriteFactory* spriteFactory, 
  const WideString spriteName, const int messageId) : BaseView(owner, bounds, spriteFactory, spriteName), 
  mMessageId(messageId)
{
  mUnwantedOptions |= VO_AUTODRAG;
}

bool CommandGenerator::consumeEvent(Event& event)
{
  if(BaseView::consumeEvent(event))
    return true;

  // if we're disabled, we don't process events
  if(getState(VS_DISABLED))
    return false;

  if(event.event == EVENT_MOUSE)
  {
    // do we own the mouse?
    if(getState(VS_CAPTURED_MOUSE))
    {
      GJRECT r = mLocalBounds;
      toGlobal(r);
      if(event.mouse.type != MOUSE_MOVE)
      {
        releaseMouse();
        setState(VS_PRESSED, false);
        if(r.contains(event.mouse.position))
          postMessageEvent(this, mMessageId, 0, NULL);
      }
      else
        setState(VS_PRESSED, r.contains(event.mouse.position));
      //
      return true;
    }
    else
    // we don't own the mouse, so check if we should!
    {
      if(event.mouse.type == MOUSE_LBUTTON_DOWN)
      {
        captureMouse(this);
        setState(VS_PRESSED);
        return true;
      }
    }

  }

  return false;
}

SimpleButton::SimpleButton(BaseView* owner, const GJRECT& bounds, SpriteFactory* spriteFactory, 
  const WideString spriteName, const int messageId) : 
  CommandGenerator(owner, bounds, spriteFactory, spriteName, messageId) 
{
  setRequiredOptions(VO_AUTOSIZE_HEIGHT);
}

void SimpleButton::drawThis()
{
  // order by states:
  // 0-2 = normal
  // 3-5 = pressed
  // 6-8 = disabled
  // 9-11= mouse over
  int startingFrame = 0; 
  if(getState(VS_DISABLED))
    startingFrame = 6;
  else if(getState(VS_PRESSED))
    startingFrame = 3;
  else if(getState(VS_MOUSEOVER))
    startingFrame = 9;

  GJRECT r = mLocalBounds;
  toGlobal(r);

  mSprite->changeFrame(startingFrame);
  GJRECT frame = mSprite->getFrameBounds();
  mSprite->setPosition(r.left, r.top, mZValue);
  mSprite->setSize(frame.width+3.5f, frame.height);
  GJFLOAT deltaX = frame.width;
  mSprite->draw();

  r.setWidth(r.width - frame.width);
  r.translate_h(frame.width);

  mSprite->changeFrame(startingFrame+2);
  frame = mSprite->getFrameBounds();
  mSprite->setPosition(r.right-frame.width-2.5f, r.top, mZValue);
  mSprite->setSize(frame.width+2.5f, frame.height);
  deltaX += frame.width;
  mSprite->draw();

  r.setWidth(r.width - frame.width);
  mSprite->changeFrame(startingFrame+1);
  mSprite->setPosition(r.left, r.top, mZValue);
  mSprite->setSize(r.width, r.height);
  mSprite->draw();

  // drawing the caption...
  r = mLocalBounds;
  toGlobal(r);
  writeCaption(r, HJUST_CENTER|VJUST_CENTER);
}

ImageButton::ImageButton(BaseView* owner, const GJRECT& bounds, SpriteFactory* spriteFactory, 
  const WideString spriteName, const int messageId) : 
  CommandGenerator(owner, bounds, spriteFactory, spriteName, messageId)
{
  setRequiredOptions(VO_AUTOSIZE);
}

void ImageButton::drawThis()
{
  GJRECT r = mLocalBounds;
  toGlobal(r);

  if(mSprite)
  {
    mSprite->setPosition(r.left, r.top, mZValue);
    mSprite->setSize(r.width, r.height);
    mSprite->draw();
  }
  else
  {
    const static WideString noImageNotice(L"<no image>");
    BaseView::drawThis();
    textOut(noImageNotice, r, HJUST_CENTER|VJUST_CENTER);
  }
}

