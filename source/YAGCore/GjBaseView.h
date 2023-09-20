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
 * @file  GJBaseView.h
 * @brief UI Base class
 *
 */

#ifndef GJ_BASE_VIEW_HEADER
#define GJ_BASE_VIEW_HEADER

#include "GjDefs.h"
#include "GjRectangles.h"
#include "GjTemplates.h"
#include "GjScreen.h"
#include "GjSprites.h"
#include "GjBitmapFont.h"
#include "GjEvents.h"

namespace yaglib
{

// view states
typedef unsigned STATE;
typedef unsigned OPTION;

const STATE VS_VISIBLE          = 0x0001;
const STATE VS_FOCUSED          = 0x0002;
const STATE VS_DISABLED         = 0x0004;
const STATE VS_PRESSED          = 0x0008;
const STATE VS_DRAGGING         = 0x0010;
const STATE VS_CAPTURED_MOUSE   = 0x0020;
const STATE VS_MOUSEOVER        = 0x0040;

const OPTION VO_DRAGGABLE       = 0x0001;
const OPTION VO_OWNERBOUNDED    = 0x0002;   // if this is not set, OWNERCLIPPED is used
const OPTION VO_AUTODRAG        = 0x0004;   // plain click will initiate dragging
const OPTION VO_TOPMOST_ONSELECT= 0x0008;   // move to top when selected/clicked
const OPTION VO_AUTOSIZE_WIDTH  = 0x0010;   // change width to sprite width
const OPTION VO_AUTOSIZE_HEIGHT = 0x0020;   // change height to sprite height
const OPTION VO_AUTOSIZE        = 0x0030;   // combination of previous two
const OPTION VO_NOCAPTION       = 0x0040;   // the caption will not be displayed

const __int64 STARTING_Z_LEVEL  = 1L;

class BaseView : public EventConsumer
{
public:
  BaseView(BaseView* owner, const GJRECT& bounds, SpriteFactory* spriteFactory, const WideString spriteName = L"");
  virtual ~BaseView();

  __int64 getZLevel() const { return mZLevel; };
  __int64 getZOrder() const { return mZOrder; };
  GJFLOAT getZValue() const { return mZValue; };
  GJFLOAT getLayerZ() const 
  {
    GJFLOAT zBase = 1.0f;
    for(int i = 0; i < mZLevel; i++) zBase /= 100;
    return zBase * -1.0f;
  };

  GJRECT const& getBounds();
  void setBounds(const GJRECT& bounds);
  BaseView* getOwner();

  virtual void draw();
  virtual void invalidate();
  void addChild(BaseView* view, const bool onTop = true);
  void removeChild(BaseView* view);
  bool isChildOnTop(BaseView* view);

  void moveToTop();
  bool onTop();
  void show();
  void hide();

  BaseView* childAtBottom();
  BaseView* childAtTop();

  bool getState(STATE stateToCheck);
  bool getStates(STATE statesToCheck, bool allOfThem = true);
  void setState(STATE stateToChange, bool activate = true);

  bool getOption(OPTION optionToCheck);
  bool getOptions(OPTION optionsToCheck, bool allOfThem = true);
  void setOption(OPTION optionToChange, bool activate = true);

  void toLocal(GJPOINT& p);
  void toLocal(GJRECT& r);
  void toGlobal(GJPOINT& p);
  void toGlobal(GJRECT& r);

  WideString& getCaption();
  void setCaption(const WideString& caption);
  void changeSprite(const WideString& spriteName);
  void refreshSprite();
  int getSpriteFrameCount() const;
  void setSpriteFrameTo(const int index);

  // EventConsumer overrides
  virtual bool consumeEvent(Event& event);
  virtual void postMessageEvent(void* _origin, const int messageId, const DWORD param, const void* ptrParam);
  virtual void captureMouse(EventConsumer* who);
  virtual void releaseMouse();
  virtual EventConsumer* getCapturedMouseTarget();
  virtual void frameUpdate(const double timeSinceLast);
  virtual void addToUpdateList(EventConsumer* who);
  virtual void removeFromUpdateList(EventConsumer* who);

protected:
  typedef ObjectList<BaseView> ChildViewList;
  ChildViewList mChildViews;

  STATE mStates;
  OPTION mOptions;
  OPTION mRequiredOptions;
  OPTION mUnwantedOptions;

  BaseView* mOwner;
  BaseView* mControlUnderMouse;
  GJRECT mBounds;
  GJRECT mLocalBounds;
  WideString mCaption;

  __int64 mZOrder;
  __int64 mZLevel;
  GJFLOAT mZValue;

  SpriteFactory* mSpriteSource;
  WideString mSpriteName;                                            
  bool mSpriteIsOptional;
  Sprite* mSprite;
  FontSprite* mFont;

  void setOwner(BaseView* owner);
  void setZLevel(const __int64 zLevel, const __int64 zOrder);
  void computeZValue();

  void updateSprites(const bool force = true);
  void commonInit(const GJRECT& bounds);
  void wantFrameUpdates(const bool interested);
  virtual void checkAndSetBounds(const GJRECT& bounds);
  void setRequiredOptions(OPTION options);

  // writing things out
  bool loadFont();
  void writeCaption(const GJRECT& r, int options = 0);
  void textOut(const WideString text, const GJRECT& rScreen, int options = 0);

  // sprite management
  void adjustSizeToSprite();
  virtual bool loadSprite();
  void unloadSprite();

  // for overriding...
  virtual void drawThis();
  virtual void boundsChanged();
  virtual void statesChanged(const STATE oldState, const STATE newState);
  virtual void statesChanged(); // this one is called by the owner

  // child-management
  virtual void clearViews();
  bool hasChildren();
  bool ownsChild(BaseView* view);
  void childToTop(BaseView* view);
  void childToBottom(BaseView* view);

  // iteration on children
  void drawChildren();
  void reassignZValues();
  void propagateBoundsChange();
  void propagateStatesChange(const STATE oldState, const STATE newState);

private:
  void checkViewUnderMouse(GJPOINT& mouseLocation);
  void handleDragging(Event& event, GJPOINT& mouseLocation);
};

class DesktopManager : public BaseView
{
public:
  DesktopManager(EventConsumer* eventMaster, const GJRECT& bounds, SpriteFactory* spriteFactory, const WideString spriteName = L"");
  DesktopManager(const GJRECT& bounds);
  virtual ~DesktopManager();

  virtual void postMessageEvent(void* _origin, const int messageId, const DWORD param, const void* ptrParam);
  virtual void captureMouse(EventConsumer* who);
  virtual void releaseMouse();
  virtual EventConsumer* getCapturedMouseTarget();
  virtual void addToUpdateList(EventConsumer* who);
  virtual void removeFromUpdateList(EventConsumer* who);
private:
  EventConsumer* mEventMaster;
};

}; /* namespace yaglib */

#endif /* GJ_BASE_VIEW_HEADER */
