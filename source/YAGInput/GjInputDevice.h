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
 * @file  GjInputDevice.h
 * @brief Wraps the input device
 *
 * Class to wrap input devices
 *
 */

#ifndef GJ_INPUT_DEVICE_HEADER
#define GJ_INPUT_DEVICE_HEADER

#include "GjDefs.h"
#include "GjNativeApp.h"
#include "GjInputUtils.h"

namespace yaglib
{

typedef enum InputDeviceType 
{
  IDT_KEYBOARD, IDT_MOUSE, IDT_JOYSTICK
};

class DeviceListener
{
public:
  virtual void buttonChanged(InputDeviceType idt, int buttonCode, bool pressed) = 0;
};

class InputDevice
{
public:
  virtual ButtonMonitor* const getButtonMonitor() const;
  virtual AxesMonitor* const getAxesMonitor() const;

  bool isBuffered();
  void setBuffered(bool flag);

  InputDeviceType getType();

protected:
  InputDeviceType mDeviceType;
  virtual void bufferedModeChanged();

private:
  bool mBufferedMode;
};

class InputDeviceManager
{
public:
  virtual bool initialize(NativeApplication& application, WideString joystickName) = 0;
  virtual void shutdown() = 0;
  virtual bool isInitialiazed() = 0;
  virtual void update() = 0;
  virtual InputDevice* getDevice(InputDeviceType devType) = 0;

  const int getShiftStates();

protected:
  int mShiftStates;

  void updateShiftStates(InputDevice* keyboard);
};

} /* namespace yaglib */

#endif /* GJ_INPUT_DEVICE_HEADER */
