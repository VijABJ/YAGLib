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

#include "GjEventFrameworkBase.h"
#include "GjGameLibConstants.h"
#include "GjBaseView.h"
#include "GjPanels.h"
#include "GjKeyCodes.h"
using namespace yaglib;

EventDrivenFramework::EventDrivenFramework(DisplayDevice* displayDevice) :
  ExtendedFramework(displayDevice), mEventConsumer(NULL), mCapturedMouseTarget(NULL)
{
  mDeviceOptions = INTERCEPT_KEYBOARD | INTERCEPT_MOUSE | INTERCEPT_GAMEPAD;
}

void EventDrivenFramework::setConsumer(EventConsumer* consumer)
{
  mEventConsumer = consumer;
}

void EventDrivenFramework::clearConsumer()
{
  setConsumer(NULL);
}

bool EventDrivenFramework::consumeEvent(Event& event)
{
  if((event.event == EVENT_MOUSE) && (mCapturedMouseTarget != NULL))
    return mCapturedMouseTarget->consumeEvent(event);

  return (mEventConsumer != NULL) ? mEventConsumer->consumeEvent(event) : false;
}

void EventDrivenFramework::postMessageEvent(void* _origin, const int messageId, const DWORD param, const void* ptrParam)
{
  mEventQueue.push(Event(_origin, messageId, param, ptrParam));
}

void EventDrivenFramework::captureMouse(EventConsumer* who)
{
  assert(mCapturedMouseTarget == NULL);
  mCapturedMouseTarget = who;
}

void EventDrivenFramework::releaseMouse()
{
  mCapturedMouseTarget = NULL;
}

EventConsumer* EventDrivenFramework::getCapturedMouseTarget()
{
  return mCapturedMouseTarget;
}

void EventDrivenFramework::frameUpdate(const double timeSinceLast)
{
  for(UpdateList::iterator iter = mUpdateList.begin(); iter != mUpdateList.end(); iter++)
    (*iter)->frameUpdate(timeSinceLast);
}

void EventDrivenFramework::addToUpdateList(EventConsumer* who)
{
  mUpdateList.push_back(who);
}

void EventDrivenFramework::removeFromUpdateList(EventConsumer* who)
{
  for(UpdateList::iterator iter = mUpdateList.begin(); iter != mUpdateList.end(); iter++)
    if(*iter == who)
    {
      mUpdateList.erase(iter);
      break;
    }
}

bool EventDrivenFramework::startup()
{
  if(!ExtendedFramework::startup())
    return false;

  mMouse->setBuffered(false);
  mKeyboard->getButtonMonitor()->attach(mKeyboardListener, ButtonMonitor::ALL_BUTTONS);
  return true;
}

bool EventDrivenFramework::beforeFrame(double timeSinceLast)
{
  if(!ExtendedFramework::beforeFrame(timeSinceLast))
    return false;

  while(mEventQueue.size() > 0)
  {
    Event event = mEventQueue.front();
    mEventQueue.pop();
    consumeEvent(event);
  }

  frameUpdate(timeSinceLast);
  return true;
}

void EventDrivenFramework::appHandler(
  NativeApplicationEvent event, NATIVE_WINDOW_HANDLE hWindow, void* appMessage)
{
  ExtendedFramework::appHandler(event, hWindow, appMessage);
  if(appMessage)
  {
    Win32Params* wp = (Win32Params*)appMessage;
    mEventQueue.push(Event(this, wp->msg, wp->wParam, wp->lParam));
  }

  return;
}

void EventDrivenFramework::updateShiftStates()
{
  int mask = 0;
  ButtonMonitor* bm = mKeyboard->getButtonMonitor();

  if(bm->isPressed(KEY_LSHIFT) || bm->isPressed(KEY_RSHIFT))
    mask |= MASK_SHIFT_KEYS;
  if(bm->isPressed(KEY_LALT) || bm->isPressed(KEY_RALT))
    mask |= MASK_ALT_KEYS;
  if(bm->isPressed(KEY_LCONTROL) || bm->isPressed(KEY_RCONTROL))
    mask |= MASK_CONTROL_KEYS;
  if(bm->isPressed(KEY_LWIN) || bm->isPressed(KEY_RWIN))
    mask |= MASK_WIN_KEYS;
  if(bm->isPressed(KEY_NUMLOCK))
    mask |= MASK_NUM_LOCK;
  if(bm->isPressed(KEY_CAPSLOCK))
    mask |= MASK_CAPS_LOCK;
  if(bm->isPressed(KEY_SCROLL))
    mask |= MASK_SCROLL_LOCK;

  mShiftKeyStates = mask;
}

char EventDrivenFramework::charFromKeyCode(const int keyCode, const int shiftKeyStates)
{
  return '\0';
}

void EventDrivenFramework::createMouseEvent(const MouseEventType type)
{
  Point3 p, m;
  mMouse->getAxesMonitor()->getPosition(p);
  mMouse->getAxesMonitor()->getMovement(m);

  POINT position = (POINT) p;
  POINT motion = (POINT) m;

  int buttonStates = 0;
  ButtonMonitor* bm = mMouse->getButtonMonitor();
  if(bm->isPressed(0))
    buttonStates |= MOUSE_LEFT_BUTTON;
  if(bm->isPressed(1))
    buttonStates |= MOUSE_RIGHT_BUTTON;
  if(bm->isPressed(2))
    buttonStates |= MOUSE_MIDDLE_BUTTON;

  // optimization. maybe remove this later?
  if(type != MOUSE_MOVE) 
    updateShiftStates();

  mEventQueue.push(Event(this, motion, position, buttonStates, mShiftKeyStates, type));
}

void EventDrivenFramework::handleKeyboardChange(int buttonCode, bool pressed)
{
  updateShiftStates();
  mEventQueue.push(Event(this, charFromKeyCode(buttonCode, mShiftKeyStates), 
    buttonCode, mShiftKeyStates, pressed));
}

void EventDrivenFramework::handleMouseButtonChange(int buttonCode, bool pressed)
{
  static MouseEventType downEvents[] = {MOUSE_LBUTTON_DOWN, MOUSE_RBUTTON_DOWN, MOUSE_MBUTTON_DOWN};
  static MouseEventType upEvents[] = {MOUSE_LBUTTON_UP, MOUSE_RBUTTON_UP, MOUSE_MBUTTON_UP};
  static MouseEventType dblClicks[] = {MOUSE_LBUTTON_DBLCLICK, MOUSE_RBUTTON_DBLCLICK, MOUSE_MBUTTON_DBLCLICK};
  static DWORD lastPressed[] = {0, 0, 0};
  const int DOUBLE_CLICK_INTERVAL = 500; // i millisecond

  if((buttonCode >= 0) && (buttonCode <= 2))
  {
    // check for double-clicks...
    bool dblClickEvent = false;
    if(pressed)
    {
      DWORD tickNow = GetTickCount();
      DWORD ticksElapsed = tickNow - lastPressed[buttonCode];
      lastPressed[buttonCode] = tickNow;
      dblClickEvent = (ticksElapsed <= DOUBLE_CLICK_INTERVAL);
    }

    createMouseEvent(pressed ? 
      (dblClickEvent ? dblClicks[buttonCode] : downEvents[buttonCode]) : upEvents[buttonCode]);
  }
}

void EventDrivenFramework::handleMouseMovements(int axisNumber)
{
  createMouseEvent(MOUSE_MOVE);
}

void EventDrivenFramework::handleGamePadButtonChange(int buttonCode, bool pressed)
{
}

void EventDrivenFramework::handleGamePadMovements(int axisNumber)
{
}

BaseEventFramework::BaseEventFramework(DisplayDevice* displayDevice, const GJFLOAT updatePeriod) : 
  EventDrivenFramework(displayDevice), mDesktop(NULL), mRoot(NULL), mMouseCursor(NULL)
{
}

BaseEventFramework::~BaseEventFramework()
{
}

bool BaseEventFramework::startup()
{
  if(!EventDrivenFramework::startup())
    return false;

  // create the desktop and the root view
  GJRECT r = g_Clipper.getBounds();
  mDesktop = new DesktopManager(this, r, mScreen);
  setConsumer(mDesktop);

  // this should contain all the controls to be used
  mRoot = new SimplePanel(mDesktop, r, mScreen, L"");
  createMouseCursor(SPRITE_MOUSE);

  return true;
}

void BaseEventFramework::createMouseCursor(const WideString spriteName)
{
  //if(getApplication()->isFullScreen())
  //{
    // check if the default mouse sprite is available
    mMouseCursor = mScreen->createSprite(spriteName); 
    if(mMouseCursor == NULL)
      return;

    Rect r = mMouseCursor->getFrameBounds();
    //mMouseCursor->setPositionOffset(4.0f / static_cast<GJFLOAT>(r.width), 4.0f / static_cast<GJFLOAT>(r.height));
    initializeMouseCursorPosition();
  //}
}

void BaseEventFramework::initializeMouseCursorPosition()
{
  GJRECT r = g_Clipper.getBounds();
  Point pc = Point(
    static_cast<int>(r.left + r.width/2),
    static_cast<int>(r.top + r.height/2)
    );

  GJPOINT p = r.centerOf();
  mMouseCursor->setPosition(p.x, p.y, -46.0f);
  mMouse->getAxesMonitor()->setPosition(Point3(pc.x, pc.y, 0));
}

void BaseEventFramework::moveMouseCursor()
{
  if(mMouseCursor != NULL)
  {
    Point3 position;
    mMouse->getAxesMonitor()->getPosition(position);
    mMouseCursor->setPosition(static_cast<GJFLOAT>(position.x), static_cast<GJFLOAT>(position.y), -46.0f);
  }
}

void BaseEventFramework::shutdown()
{
  clearConsumer();
  SAFE_DELETE(mDesktop);
  EventDrivenFramework::shutdown();
}

bool BaseEventFramework::frame(const double timeSinceLast)
{
  mDesktop->draw();
  if(mShowFPS && mFPSFont) mFPSFont->draw();

  if(mMouseCursor)
    mMouseCursor->draw();
  return true;
}

bool BaseEventFramework::consumeEvent(Event& event)
{
  bool result = EventDrivenFramework::consumeEvent(event);
  switch(event.event)
  {
  case EVENT_MOUSE:
    if(event.mouse.type == MOUSE_MOVE)
      moveMouseCursor();
    break;
  case EVENT_KEYBOARD:
    if(event.kbd.keyPressed && (event.kbd.keyCode == KEY_ESCAPE) && !result)
    {
      PostQuitMessage(0);
      return true;
    }
    break;
  case EVENT_MESSAGE:
    if(event.msg.messageId == STDMESSAGE_EXIT)
    {
      PostQuitMessage(0);
      return true;
    }
    break;
  }

  return result;
}

