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
 * @file  GjAxialDeviceDX.h
 * @brief DirectX-specific axial devices
 *
 * Axial devices are those that have moving axes.  Mostly everything apart from the keyboard
 *
 */

#ifndef GJ_AXIAL_DEVICE_DX_HEADER
#define GJ_AXIAL_DEVICE_DX_HEADER

#include "GjInputDeviceDX.h"

namespace yaglib
{

class DIAxialDevice;

class ButtonMonitorEx : public ButtonMonitor
{
public:
  ButtonMonitorEx(const int numberOfButtons);
  virtual ~ButtonMonitorEx();

  virtual bool isPressed(const int buttonCode) const;
  virtual bool isReleased(const int buttonCode) const;

  void updateIfChanged(const int buttonCode, const bool expectedState, 
    DWORD timeStamp, const bool triggerNotifications = true);

private:
  typedef struct ButtonState
  {
    bool pressed;
    DWORD lastPressed;
    DWORD lastReleased;
    DWORD lastChanged;
  } ButtonState;

  ButtonState* mButtonStates;
  ButtonState* getButtonState(const int buttonCode);
};

class DIAxialDevice : public BaseDIDevice
{
public:
  DIAxialDevice(LPDIRECTINPUT8 lpdi8, NativeApplication& application);
  virtual ~DIAxialDevice();

  virtual ButtonMonitor* const getButtonMonitor() const;
  virtual AxesMonitor* const getAxesMonitor() const;

protected:
  ButtonMonitorEx* mButtonMonitor;
  AxesMonitor* mAxesMonitor;

  void initMonitors(int numberOfButtons, int numberOfAxes);
};

inline ButtonMonitor* const DIAxialDevice::getButtonMonitor() const
{
  return mButtonMonitor;
}

inline AxesMonitor* const DIAxialDevice::getAxesMonitor() const
{
  return mAxesMonitor;
}

} /* namespace yaglib */


#endif /* GJ_AXIAL_DEVICE_DX_HEADER */
