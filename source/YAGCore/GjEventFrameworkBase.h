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
 * @file  GjEventFrameworkBase.h
 * @brief The framework that uses the event system
 *
 */

#ifndef GJ_EVENT_FRAMEWORK_BASE_HEADER
#define GJ_EVENT_FRAMEWORK_BASE_HEADER

#include "GjBaseView.h"
#include "GjEvents.h"
#include "GjFramework.h"

namespace yaglib
{

class EventDrivenFramework : public ExtendedFramework, public EventConsumer
{
public:
  EventDrivenFramework(DisplayDevice* displayDevice);

  virtual void handleKeyboardChange(int buttonCode, bool pressed);
  virtual void handleMouseButtonChange(int buttonCode, bool pressed);
  virtual void handleMouseMovements(int axisNumber);
  virtual void handleGamePadButtonChange(int buttonCode, bool pressed);
  virtual void handleGamePadMovements(int axisNumber);

  void setConsumer(EventConsumer* consumer);
  void clearConsumer();

  virtual bool consumeEvent(Event& event);
  virtual void postMessageEvent(void* _origin, const int messageId, const DWORD param, const void* ptrParam);
  virtual void captureMouse(EventConsumer* who);
  virtual void releaseMouse();
  virtual EventConsumer* getCapturedMouseTarget();
  virtual void frameUpdate(const double timeSinceLast);
  virtual void addToUpdateList(EventConsumer* who);
  virtual void removeFromUpdateList(EventConsumer* who);

protected:
  typedef std::queue<Event> EventQueue;
  EventQueue mEventQueue;
  EventConsumer* mEventConsumer;
  EventConsumer* mCapturedMouseTarget;
  int mShiftKeyStates;
  typedef std::vector<EventConsumer*> UpdateList;
  UpdateList mUpdateList;

  virtual bool startup();
  virtual bool beforeFrame(const double timeSinceLast);
  virtual void appHandler(NativeApplicationEvent event, NATIVE_WINDOW_HANDLE hWindow, void* appMessage);

  void updateShiftStates();
  char charFromKeyCode(const int keyCode, const int shiftKeyStates);
  void createMouseEvent(const MouseEventType type);
};

const int STDMESSAGE_EXIT = 1;

class BaseEventFramework : public EventDrivenFramework
{
public:
  BaseEventFramework(DisplayDevice* displayDevice, const GJFLOAT updatePeriod = 0.0);
  virtual ~BaseEventFramework();

  virtual bool frame(const double timeSinceLast);
  virtual bool startup();
  virtual void shutdown();
protected:
  Sprite* mMouseCursor;
  BaseView* mDesktop;
  BaseView* mRoot;

  void createMouseCursor(const WideString spriteName);
  void initializeMouseCursorPosition();
  void moveMouseCursor();
  virtual bool consumeEvent(Event& event);
};

}; /* namespace yaglib */

#endif /* GJ_EVENT_FRAMEWORK_BASE_HEADER */
