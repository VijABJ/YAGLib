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
 * @file  GjInputManagerDX.h
 * @brief DirectInput manager class
 *
 */

#ifndef GJ_INPUT_MANAGER_DX_HEADER
#define GJ_INPUT_MANAGER_DX_HEADER

#include "GjInputDeviceDX.h"
#include "GjKeyboardDeviceDX.h"
#include "GjAxialDeviceDX.h"
#include "GjMouseDeviceDX.h"
#include "GjJoystickDeviceDX.h"

namespace yaglib
{

class InputDeviceManagerDX : public InputDeviceManager
{
public:
  InputDeviceManagerDX();
  virtual ~InputDeviceManagerDX();

  virtual bool initialize(NativeApplication& application, WideString joystickName);
  virtual void shutdown();

  virtual bool isInitialiazed();
  virtual void update();

  virtual InputDevice* getDevice(InputDeviceType devType);

  DIKeyboardDevice* getKeyboardDevice();
  DIMouseDevice*    getMouseDevice();
  DIJoystickDevice* getJoystickDevice();

  HINSTANCE getInstance();
  HWND getWindowHandle();
  LPDIRECTINPUT8 directInput8();

private:
  HINSTANCE mInstance;
  HWND mWindowHandle;
  bool mIsFullScreen;
  LPDIRECTINPUT8 mDirectInput;

  DIKeyboardDevice* mKeyboard;
  DIMouseDevice*    mMouse;
  DIJoystickDevice* mJoystick;

};

inline bool InputDeviceManagerDX::isInitialiazed()
{
  return mDirectInput != NULL;
}

inline DIKeyboardDevice* InputDeviceManagerDX::getKeyboardDevice()
{
  return mKeyboard;
}

inline DIMouseDevice* InputDeviceManagerDX::getMouseDevice()
{
  return mMouse;
}

inline DIJoystickDevice* InputDeviceManagerDX::getJoystickDevice()
{
  return mJoystick;
}

inline HINSTANCE InputDeviceManagerDX::getInstance()
{
  return mInstance;
}

inline HWND InputDeviceManagerDX::getWindowHandle()
{
  return mWindowHandle;
}

inline LPDIRECTINPUT8 InputDeviceManagerDX::directInput8()
{
  return mDirectInput;
}

} /* namespace yaglib */


#endif /* GJ_INPUT_MANAGER_DX_HEADER */
