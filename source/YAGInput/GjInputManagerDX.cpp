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

#include "GjInputManagerDX.h"
#include <cstdlib>
using namespace yaglib;

InputDeviceManagerDX::InputDeviceManagerDX() : 
  mDirectInput(NULL), mKeyboard(NULL), mMouse(NULL), mJoystick(NULL)
{
  CoInitialize(NULL);
}

InputDeviceManagerDX::~InputDeviceManagerDX()
{
  shutdown();
  CoUninitialize();
}

bool InputDeviceManagerDX::initialize(NativeApplication& application, WideString joystickName)
{
  shutdown();

  mInstance = application.getInstance();
  mWindowHandle = application.getWindowHandle();
  mIsFullScreen = application.isFullScreen();

  if(FAILED(DirectInput8Create(mInstance, DIRECTINPUT_VERSION,
    IID_IDirectInput8, (void**)&mDirectInput, NULL)))
    return false;

  // create and initialize the keyboard. if this fails,
  // there is no point in continuing. no keyboard means
  // *really* limited user input, so forget it.
  mKeyboard = new DIKeyboardDevice(mDirectInput, application);
  if(FAILED(mKeyboard->init()))
  {
    delete mKeyboard;
    mKeyboard = NULL;
    return false;
  }

  // unlike the keyboard, the mouse and the joystick are
  // not very critical. so we won't fail the initialization
  // just because either or both failed to get up and 
  // running. the caller will be able to detect the
  // absence of these devices anyway so they can decide
  // if lack of either should result in a shutdown
  mMouse = new DIMouseDevice(mDirectInput, application);
  if(FAILED(mMouse->init()))
  {
    delete mMouse;
    mMouse = NULL;
  }

  mJoystick = new DIJoystickDevice(mDirectInput, application);
  if(FAILED(mJoystick->init()))
  {
    delete mJoystick;
    mJoystick = NULL;
  }

  return true;
}

void InputDeviceManagerDX::shutdown()
{
  SAFE_DELETE(mJoystick);
  SAFE_DELETE(mMouse);
  SAFE_DELETE(mKeyboard);
  SAFE_RELEASE(mDirectInput);
}

void InputDeviceManagerDX::update()
{
  // no need to check the keyboard. it MUST exist!
  mKeyboard->update();
  updateShiftStates(mKeyboard);

  if(mMouse)     mMouse->update();
  if(mJoystick)  mJoystick->update();
}

InputDevice* InputDeviceManagerDX::getDevice(InputDeviceType devType)
{
  switch(devType)
  {
  case IDT_KEYBOARD:
    return mKeyboard;
  case IDT_MOUSE:
    return mMouse;
  case IDT_JOYSTICK:
    return mJoystick;
  default:
    return NULL;
  }
}
