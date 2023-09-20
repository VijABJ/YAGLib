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

#include "GjBaseView.h"
using namespace yaglib;

// NOTE: the front of the view vector list is the view that is under everyone else!

BaseView::BaseView(BaseView* owner, const GJRECT& bounds, SpriteFactory* spriteFactory, 
  const WideString spriteName) : mOwner(owner), mSpriteSource(spriteFactory),
  mFont(NULL), mBounds(GJRECT(0, 0, 0, 0)), mStates(0), mOptions(0), 
  mSpriteName(spriteName), mSprite(NULL), mZLevel(STARTING_Z_LEVEL),
  mZOrder(1), mSpriteIsOptional(mSpriteName.size() == 0) 
{
  commonInit(bounds);
}

void BaseView::commonInit(const GJRECT& bounds)
{
  mRequiredOptions = mUnwantedOptions = 0;
  checkAndSetBounds(bounds);
  if(mOwner != NULL)
    mOwner->addChild(this);
  setState(VS_VISIBLE, true);
  mControlUnderMouse = NULL;
}

BaseView::~BaseView()
{
  if(mOwner)
    mOwner->removeChild(this);
  unloadSprite();
  clearViews();
}

void BaseView::computeZValue()
{
  __int64 level = mZLevel;
  mZValue = mZOrder;
  BaseView* v = mOwner;
  while(v)
  {
    level--;
    mZValue /= 100;
    mZValue += mZOrder;
    v = v->getOwner();
  }

  mZValue *= -1;
}

void BaseView::wantFrameUpdates(const bool interested)
{
  if(interested)
    addToUpdateList(this);
  else
    removeFromUpdateList(this);
}

void BaseView::checkViewUnderMouse(GJPOINT& mouseLocation)
{
  // update which control is under the mouse, if any...
  BaseView* hoverMouseTarget = NULL;
  for(ChildViewList::iterator iter = mChildViews.begin(); iter != mChildViews.end(); iter++)
  {
    BaseView* child = *iter;
    if(!child->getState(VS_VISIBLE))
      continue;

    if(child->mBounds.contains(mouseLocation))
    {
      hoverMouseTarget = child;
      break;
    }
  }

  // now, trigger mouse hover states on the suspect controls...
  if(mControlUnderMouse != hoverMouseTarget)
  {
    if(mControlUnderMouse != NULL)
      mControlUnderMouse->setState(VS_MOUSEOVER, false);

    mControlUnderMouse = hoverMouseTarget;
    if(mControlUnderMouse != NULL)
      mControlUnderMouse->setState(VS_MOUSEOVER, true);
  }
}

void BaseView::handleDragging(Event& event, GJPOINT& mouseLocation)
{
  if(event.mouse.type == MOUSE_MOVE)
  {
    GJRECT r = mBounds;
    r.translate(event.mouse.motion);
    if((mOptions & VO_OWNERBOUNDED) != 0)
      mOwner->mLocalBounds.cage(r);

    setBounds(r);
  }
  else // any other button activity cancels the drag operation
  {
    releaseMouse();
    setState(VS_DRAGGING, false);
  }
}

bool BaseView::consumeEvent(Event& event)
{
  // mouse events are kinda special...
  if(event.event == EVENT_MOUSE)
  {
    GJPOINT mouseLocation = event.mouse.position;
    toLocal(mouseLocation);

    // check if we're in drag mode, and whether we should move, or cancel
    if(getState(VS_DRAGGING))
    {
      handleDragging(event, mouseLocation);
      return true;
    }

    // find out if we have child under the mouse
    checkViewUnderMouse(mouseLocation);

    // distribute mouse events first because a child might actually want it!
    // note that mouse events are distributed only to controls under the mouse cursor.
    for(ChildViewList::iterator iter = mChildViews.begin(); iter != mChildViews.end(); iter++)
    {
      BaseView* child = *iter;
      if(!child->getState(VS_VISIBLE))
        continue;

      if(child->mBounds.contains(mouseLocation))
        if(child->consumeEvent(event))
          return true;
    }

    if((event.mouse.type == MOUSE_LBUTTON_DOWN) && ((mOptions & VO_TOPMOST_ONSELECT) != 0))
      moveToTop();

    // now parse the mouse event ourselves, we might want it
    // check if we're draggable; if not, no point in checking the triggered controls!
    if(getOption(VO_DRAGGABLE) && (event.mouse.type == MOUSE_LBUTTON_DOWN) &&
      (((mOptions & VO_AUTODRAG) != 0) || ((event.mouse.shiftKeyStates & MASK_CONTROL_KEYS) != 0)))
    {
      captureMouse(this);
      setState(VS_DRAGGING, true);
      return true;
    }

    // if we get here, no one wanted the mouse event! so we'll just ignore it
    return false;
  }

  // distribute other events
  for(ChildViewList::iterator iter = mChildViews.begin(); iter != mChildViews.end(); iter++)
  {
    BaseView* child = *iter;
    if(!child->getState(VS_VISIBLE))
      continue;

    if(child->consumeEvent(event))
      return true;
  }

  return false;
}

void BaseView::postMessageEvent(void* _origin, const int messageId, const DWORD param, const void* ptrParam)
{
  if(mOwner)
    mOwner->postMessageEvent(_origin, messageId, param, ptrParam);
}

void BaseView::captureMouse(EventConsumer* who)
{
  if(who == this)
    setState(VS_CAPTURED_MOUSE);

  if(mOwner)
    mOwner->captureMouse(who);
}

void BaseView::releaseMouse()
{
  if(mOwner != NULL)
  {
    if(mOwner->getCapturedMouseTarget() == this)
      setState(VS_CAPTURED_MOUSE, false);

    mOwner->releaseMouse();
  }
}

EventConsumer* BaseView::getCapturedMouseTarget()
{
  return (mOwner != NULL) ? mOwner->getCapturedMouseTarget() : NULL;
}

void BaseView::frameUpdate(const double timeSinceLast)
{
  // nothing to do
}

bool BaseView::loadFont()
{
  if(!mFont)
  {
    if(mSpriteName.size() > 0)
      mFont = mSpriteSource->createFontSprite(mSpriteName);
    if(mFont == NULL)
      mFont = mSpriteSource->createFontSprite(L"default");
  }
  return (mFont != NULL);
}

void BaseView::writeCaption(const GJRECT& r, int options)
{
  if(((mOptions & VO_NOCAPTION) == 0) && (mCaption.size() > 0))
    textOut(mCaption, r, options);
}

void BaseView::textOut(const WideString text, const GJRECT& rScreen, int options)
{
  if(!loadFont())
    return;

  mFont->setTextDrawOptions(options);
  mFont->setPosition(rScreen.left, rScreen.top, mZValue);
  mFont->setSize(rScreen.width, rScreen.height);
  mFont->draw();
}

void BaseView::addToUpdateList(EventConsumer* who)
{
  if(mOwner)
    mOwner->addToUpdateList(who);
}

void BaseView::removeFromUpdateList(EventConsumer* who)
{
  if(mOwner)
    mOwner->removeFromUpdateList(who);
}

GJRECT const& BaseView::getBounds()
{
  return mBounds;
}

void BaseView::setBounds(const GJRECT& bounds)
{
  if((mBounds.left != bounds.left) || (mBounds.top != bounds.top) || 
     (mBounds.right != bounds.right) || (mBounds.bottom != bounds.bottom))
  {
    checkAndSetBounds(bounds);
    boundsChanged();
  }
}

void BaseView::checkAndSetBounds(const GJRECT& bounds)
{
  mBounds = bounds;
  mLocalBounds = mBounds;
  mLocalBounds -= mBounds.getTopLeft();
}

void BaseView::setRequiredOptions(OPTION options)
{
  mRequiredOptions |= options;
  setOption(options);
}

void BaseView::setOwner(BaseView* owner)
{
  if(mOwner != owner)
  {
    if(mOwner != NULL) 
      mOwner->removeChild(this);

    mOwner = owner;
    if(mOwner != NULL)
      mOwner->addChild(this);
  }
}

BaseView* BaseView::getOwner()
{
  return mOwner;
}

void BaseView::moveToTop()
{
  if(mOwner != NULL)
    mOwner->childToTop(this);
}

bool BaseView::onTop()
{
  return (mOwner == NULL) || (mOwner->isChildOnTop(this));
}

void BaseView::show()
{
  setState(VS_VISIBLE, true);
}

void BaseView::hide()
{
  setState(VS_VISIBLE, false);
}

BaseView* BaseView::childAtBottom()
{
  return (mChildViews.size() == 0) ? NULL : *(mChildViews.begin());
}

BaseView* BaseView::childAtTop()
{
  return (mChildViews.size() == 0) ? NULL : *(mChildViews.end()-1);
}

bool BaseView::getState(STATE stateToCheck)
{
  return (mStates & stateToCheck) != 0;
}

bool BaseView::getStates(STATE statesToCheck, bool allOfThem)
{
  return allOfThem ? 
    ((mStates & statesToCheck) == statesToCheck) :
    ((mStates & statesToCheck) != 0);
}

void BaseView::setState(STATE stateToChange, bool activate)
{
  STATE oldState = mStates;
  if(activate)
  {
    if((mStates & stateToChange) == 0)
      mStates |= stateToChange;
    else
      return;
  }
  else
  {
    if((mStates & stateToChange) != 0)
      mStates &= ~stateToChange;
    else
      return;
  }
  statesChanged(oldState, mStates);
}

bool BaseView::getOption(OPTION optionToCheck)
{
  return (mOptions & optionToCheck) != 0;
}

bool BaseView::getOptions(OPTION optionsToCheck, bool allOfThem)
{
  return allOfThem ? 
    ((mOptions & optionsToCheck) == optionsToCheck) :
    ((mOptions & optionsToCheck) != 0);
}

void BaseView::setOption(OPTION optionToChange, bool activate)
{
  if(activate)
  {
    if((mOptions & optionToChange) == 0)
      mOptions |= optionToChange;
  }
  else
  {
    if((mOptions & optionToChange) != 0)
      mOptions &= ~optionToChange;
  }
  // 
  if(mRequiredOptions)
    mOptions |= mRequiredOptions;
  if(mUnwantedOptions)
    mOptions &= ~mUnwantedOptions;
}

void BaseView::toLocal(GJPOINT& p)
{
  if(mOwner)
    mOwner->toLocal(p);

  p -= mBounds.getTopLeft();
}

void BaseView::toLocal(GJRECT& r)
{
  if(mOwner)
    mOwner->toLocal(r);

  r -= mBounds.getTopLeft();
}

void BaseView::toGlobal(GJPOINT& p)
{
  p += mBounds.getTopLeft();

  if(mOwner)
    mOwner->toGlobal(p);
}

void BaseView::toGlobal(GJRECT& r)
{
  r += mBounds.getTopLeft();

  if(mOwner)
    mOwner->toGlobal(r);
}

WideString& BaseView::getCaption()
{
  return mCaption;
}

void BaseView::setCaption(const WideString& caption)
{
  if(mCaption != caption)
  {
    mCaption = caption;
    if(loadFont())
      mFont->setText(caption);
  }
}

void BaseView::draw()
{
  GJRECT r = mLocalBounds;
  toGlobal(r);
  g_Clipper.set(r);

  if(loadSprite() || mSpriteIsOptional)
    drawThis();

  drawChildren();
  g_Clipper.rewind();
}

void BaseView::invalidate()
{
}

void BaseView::updateSprites(const bool force)
{
  // update the sprites
  GJRECT r = mLocalBounds;
  toGlobal(r);
  //
  if((mSprite) || ((!mSprite && force) && loadSprite()))
  {
    mSprite->setPosition(r.left, r.top, mZValue);
    mSprite->setSize(r.width, r.height);
  }
  if(loadFont())
  {
    mFont->setPosition(r.left, r.top, mZValue);
    mFont->setSize(r.width, r.height);
  }
  //
}

void BaseView::boundsChanged()
{
  updateSprites();
  propagateBoundsChange();
}

void BaseView::statesChanged()
{
  // nothing to do right now. we don't particularly care if the owner
  // is now in a different state
}

void BaseView::statesChanged(const STATE oldState, const STATE newState)
{
  propagateStatesChange(oldState, newState);
}

void BaseView::drawThis()
{
  // default drawing is a white rectangle...
  // but since the redone classes, drawRect() is not available right now...
}

void BaseView::clearViews()
{
  while(mChildViews.size() > 0)
  {
    BaseView* view = mChildViews.remove(0);
    assert(view != NULL);
    view->mOwner = NULL;
    delete view;
  }
}

bool BaseView::hasChildren()
{
  return mChildViews.size() > 0;
}

bool BaseView::ownsChild(BaseView* view)
{
  return mChildViews.exists(view);
}

void BaseView::childToTop(BaseView* view)
{
  if(ownsChild(view) && !mChildViews.isLast(view))
  {
    mChildViews.remove(mChildViews.indexOf(view));
    mChildViews.prepend(view);
    reassignZValues();
  }
}

void BaseView::childToBottom(BaseView* view)
{
  if(ownsChild(view) && !mChildViews.isFirst(view))
  {
    mChildViews.remove(mChildViews.indexOf(view));
    mChildViews.append(view);
    reassignZValues();
  }
}

void BaseView::addChild(BaseView* view, const bool onTop)
{
  if(!ownsChild(view))
  {
    if(onTop)
      mChildViews.prepend(view);
    else
      mChildViews.append(view);
    //
    reassignZValues();
  }
}

void BaseView::removeChild(BaseView* view)
{
  if(ownsChild(view))
    mChildViews.remove(mChildViews.indexOf(view));
}

bool BaseView::isChildOnTop(BaseView* view)
{
  return ownsChild(view) && (mChildViews.indexOf(view) == (mChildViews.size()-1));
}

void BaseView::setZLevel(const __int64 zLevel, const __int64 zOrder)
{
  mZLevel = zLevel;
  mZOrder = zOrder;
  computeZValue();
  updateSprites(false);
  reassignZValues();
}

void BaseView::drawChildren()
{
  if(mChildViews.size() > 0)
  {
    ChildViewList::iterator iter = mChildViews.end() - 1;
    while(true)
    {
      BaseView* child = *iter;
      if(child->getState(VS_VISIBLE))
        child->draw();

      if(iter == mChildViews.begin())
        break;
      iter--;
    }
  }
}

void BaseView::reassignZValues()
{
  __int64 nextLevel = mZLevel+1, childOrder = static_cast<__int64>(mChildViews.size());
  for(ChildViewList::iterator iter = mChildViews.begin(); iter != mChildViews.end(); iter++, childOrder--)
    (*iter)->setZLevel(nextLevel, childOrder);
}

void BaseView::propagateBoundsChange()
{
  for(ChildViewList::iterator iter = mChildViews.begin(); iter != mChildViews.end(); iter++)
    (*iter)->boundsChanged();
}

void BaseView::propagateStatesChange(const STATE oldState, const STATE newState)
{
  for(ChildViewList::iterator iter = mChildViews.begin(); iter != mChildViews.end(); iter++)
    (*iter)->statesChanged();
}

void BaseView::changeSprite(const WideString& spriteName)
{
  if(mSpriteName != spriteName)
  {
    unloadSprite();
    mSpriteName = spriteName;
    loadSprite();
    mSpriteIsOptional = mSprite == NULL;
  }
}

void BaseView::refreshSprite()
{
  loadSprite();
}

bool BaseView::loadSprite()
{
  if((mSprite == NULL) && (mSpriteName.size() > 0))
  {
    mSprite = mSpriteSource->createSprite(mSpriteName);
    if(mSprite)
    {
      mSprite->changeFrame(0);
      GJPOINT p(0, 0);
      toGlobal(p);
      mSprite->setPosition(p.x, p.y, mZValue);
      //
      if((mOptions & VO_AUTOSIZE) != 0)
        adjustSizeToSprite();
      else
        mSprite->setSize(mLocalBounds.width, mLocalBounds.height);
    }
  }

  return mSprite != NULL;
}

void BaseView::adjustSizeToSprite()
{
  if(mSprite)
  {
    GJRECT newBounds = mBounds;
    GJRECT frame = mSprite->getFrameBounds();

    if(mOptions & VO_AUTOSIZE_WIDTH)
      newBounds.setWidth(frame.width);
    if(mOptions & VO_AUTOSIZE_HEIGHT)
      newBounds.setHeight(frame.height);

    setBounds(newBounds);
  }
}

void BaseView::unloadSprite()
{
  if(mSprite)
    SAFE_DELETE(mSprite);
}

int BaseView::getSpriteFrameCount() const
{
  return (mSprite != NULL) ? mSprite->getInfo()->frameCount : 0;
}

void BaseView::setSpriteFrameTo(const int index)
{
  if(mSprite != NULL)
  {
    mSprite->changeFrame(index);
    if((mOptions & VO_AUTOSIZE) != 0)
      adjustSizeToSprite();
  }
}

////////
DesktopManager::DesktopManager(EventConsumer* eventMaster, const GJRECT& bounds, 
  SpriteFactory* spriteFactory, const WideString spriteName) :
  BaseView(NULL, bounds, spriteFactory), mEventMaster(eventMaster)
{
}

DesktopManager::~DesktopManager()
{
}

void DesktopManager::postMessageEvent(void* _origin, const int messageId, const DWORD param, const void* ptrParam)
{
  mEventMaster->postMessageEvent(_origin, messageId, param, ptrParam);
}

void DesktopManager::captureMouse(EventConsumer* who)
{
  mEventMaster->captureMouse(who);
}

void DesktopManager::releaseMouse()
{
  mEventMaster->releaseMouse();
}

EventConsumer* DesktopManager::getCapturedMouseTarget()
{
  return mEventMaster->getCapturedMouseTarget();
}

void DesktopManager::addToUpdateList(EventConsumer* who)
{
  mEventMaster->addToUpdateList(who);
}

void DesktopManager::removeFromUpdateList(EventConsumer* who)
{
  mEventMaster->removeFromUpdateList(who);
}


