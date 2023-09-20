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
 * @file  GjEvents.h
 * @brief Event system
 *
 */

#ifndef GJ_EVENT_SYSTEM_HEADER
#define GJ_EVENT_SYSTEM_HEADER

#include "GjDefs.h"
#include "GjInputDevice.h"

namespace yaglib
{

typedef enum EventType 
{ 
  EVENT_NOTHING,
  EVENT_MOUSE,
  EVENT_GAME_CONTROLLER,
  EVENT_KEYBOARD,
  EVENT_OS_GUI,
  EVENT_MESSAGE
} EventType;

typedef struct tagKbdEvent
{
  bool keyPressed;
  char character;
  int keyCode;
  int shiftKeyStates;
} KbdEvent;

typedef enum MouseEventType
{
  MOUSE_LBUTTON_DOWN,
  MOUSE_LBUTTON_UP,
  MOUSE_LBUTTON_DBLCLICK,
  MOUSE_RBUTTON_DOWN,
  MOUSE_RBUTTON_UP,
  MOUSE_RBUTTON_DBLCLICK,
  MOUSE_MBUTTON_DOWN,
  MOUSE_MBUTTON_UP,
  MOUSE_MBUTTON_DBLCLICK,
  MOUSE_MOVE
} MouseEventType;

typedef struct tagMouseEvent
{
  MouseEventType type;
  POINT motion;
  POINT position;
  int buttonStates;
  int shiftKeyStates;
} MouseEvent;

typedef struct tagControllerEvent
{
  int axis;
  POINT motion;
  POINT position;
  int buttonStates;
  int shiftKeyStates;
} ControllerEvent;

typedef struct tagOSEvent
{
  int osEvent;
  WPARAM wParam;
  LPARAM lParam;
} OSEvent;

typedef struct tagMessageEvent
{
  int messageId;
  DWORD param;
  void* ptrParam;
} MessageEvent;

typedef struct Event
{
  void* origin;
  EventType event;
  // anonymous union of event-specific data
  union
  {
    KbdEvent kbd;
    MouseEvent mouse;
    ControllerEvent controller;
    OSEvent os;
    MessageEvent msg;
  };

  Event()
  {
    origin = NULL;
    event = EVENT_NOTHING;
  };

  Event(void* _origin, const char character, const int keyCode, 
    const int shiftKeyStates, const bool keyPressed)
  {
    origin = _origin;
    event = EVENT_KEYBOARD;
    kbd.keyPressed = keyPressed;
    kbd.character = character;
    kbd.keyCode = keyCode;
    kbd.shiftKeyStates = shiftKeyStates;
  };

  Event(void* _origin, const POINT& motion, const POINT& position, 
    const int buttonStates, const int shiftKeyStates, const MouseEventType type)
  {
    origin = _origin;
    event = EVENT_MOUSE;
    mouse.type = type;
    mouse.motion = motion;
    mouse.position = position;
    mouse.buttonStates = buttonStates;
    mouse.shiftKeyStates = shiftKeyStates;
  };

  Event(void* _origin, const int axis, const POINT& motion, const POINT& position, 
    const int buttonStates, const int shiftKeyStates)
  {
    origin = _origin;
    event = EVENT_GAME_CONTROLLER;
    controller.axis = axis;
    controller.motion = motion;
    controller.position = position;
    controller.buttonStates = buttonStates;
    controller.shiftKeyStates = shiftKeyStates;
  };

  Event(void* _origin, const int osEvent, const WPARAM wParam, const LPARAM lParam)
  {
    origin = _origin;
    event = EVENT_OS_GUI;
    os.osEvent = osEvent;
    os.wParam = wParam;
    os.lParam = lParam;
  };

  Event(void* _origin, const int messageId, const DWORD param, const void* ptrParam)
  {
    origin = _origin;
    event = EVENT_MESSAGE;
    msg.messageId = messageId;
    msg.param = param;
    msg.ptrParam = const_cast<void*>(ptrParam);
  };
} Event;

const int MASK_SHIFT_KEYS     = 0x0001;
const int MASK_ALT_KEYS       = 0x0002;
const int MASK_CONTROL_KEYS   = 0x0004;
const int MASK_WIN_KEYS       = 0x0008;
const int MASK_NUM_LOCK       = 0x0010;
const int MASK_CAPS_LOCK      = 0x0020;
const int MASK_SCROLL_LOCK    = 0x0040;

const int MOUSE_LEFT_BUTTON   = 0x0001;
const int MOUSE_RIGHT_BUTTON  = 0x0002;
const int MOUSE_MIDDLE_BUTTON = 0x0004;

class EventConsumer
{
public:
  virtual bool consumeEvent(Event& event) = 0;
  virtual void postMessageEvent(void* _origin, const int messageId, const DWORD param, const void* ptrParam) = 0;
  virtual void captureMouse(EventConsumer* who) = 0;
  virtual void releaseMouse() = 0;
  virtual EventConsumer* getCapturedMouseTarget() = 0;
  //
  virtual void frameUpdate(const double timeSinceLast) = 0;
  virtual void addToUpdateList(EventConsumer* who) = 0;
  virtual void removeFromUpdateList(EventConsumer* who) = 0;
};

} /* namespace gjlib */

#endif /* GJ_EVENT_SYSTEM_HEADER */
