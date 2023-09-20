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

#include "GjAxialDeviceDx.h"
#include <cstdlib>
using namespace yaglib;

ButtonMonitorEx::ButtonMonitorEx(const int numberOfButtons) :
  ButtonMonitor(numberOfButtons)
{
  mButtonStates = new ButtonState[numberOfButtons];
  memset(mButtonStates, 0, sizeof(ButtonState) * numberOfButtons);
}

ButtonMonitorEx::~ButtonMonitorEx()
{
  delete [] mButtonStates;
}

bool ButtonMonitorEx::isPressed(const int buttonCode) const
{
  return ((buttonCode >= 0) && (buttonCode < getNumberOfButtons())) ?
    (mButtonStates + buttonCode)->pressed : false;
}

bool ButtonMonitorEx::isReleased(const int buttonCode) const
{
  return ((buttonCode >= 0) && (buttonCode < getNumberOfButtons())) ?
    (mButtonStates + buttonCode)->pressed : true;
}

ButtonMonitorEx::ButtonState* ButtonMonitorEx::getButtonState(const int buttonCode)
{
  return ((buttonCode >= 0) && (buttonCode < getNumberOfButtons())) ?
    (mButtonStates + buttonCode) : NULL;
}

void ButtonMonitorEx::updateIfChanged(const int buttonCode, 
  const bool expectedState, DWORD timeStamp, const bool triggerNotifications)
{
  ButtonState* bState = getButtonState(buttonCode);
  if(expectedState == bState->pressed)
    return;

  if(bState->pressed)
    bState->lastPressed = bState->lastChanged;
  else
    bState->lastReleased = bState->lastChanged;

  bState->lastChanged = timeStamp;
  bState->pressed = expectedState;

  if(triggerNotifications)
    notify(buttonCode);
}

DIAxialDevice::DIAxialDevice(LPDIRECTINPUT8 lpdi8, NativeApplication& application) :
  BaseDIDevice(lpdi8, application), mButtonMonitor(NULL), mAxesMonitor(NULL)
{
}

DIAxialDevice::~DIAxialDevice()
{
  delete mButtonMonitor;
  delete mAxesMonitor;
}

void DIAxialDevice::initMonitors(int numberOfButtons, int numberOfAxes)
{
  if(mButtonMonitor) delete mButtonMonitor;
  if(mAxesMonitor) delete mAxesMonitor;

  mButtonMonitor = new ButtonMonitorEx(numberOfButtons);
  mAxesMonitor = new AxesMonitor(numberOfAxes);
}

