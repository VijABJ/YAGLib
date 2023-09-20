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

#include "GjInputDevice.h"
#include "GjKeyTables.h"
using namespace yaglib;

void InputDeviceManager::updateShiftStates(InputDevice* keyboard)
{
  if(keyboard != NULL)
  {
    ButtonMonitor* bm = keyboard->getButtonMonitor();
    mShiftStates = 0;

    // update the shift key states
    for(size_t i = 0; i < keyShiftsCount; i++)
    {
      KeyShift ks = keyShifts[i];
      if(bm->isPressed(ks.code))
        mShiftStates |= ks.shift;
    }
  }
}

ButtonMonitor* const InputDevice::getButtonMonitor() const
{
  return NULL;
}

AxesMonitor* const InputDevice::getAxesMonitor() const
{
  return NULL;
}

bool InputDevice::isBuffered()
{
  return mBufferedMode;
}

void InputDevice::setBuffered(bool flag)
{
  if(mBufferedMode != flag)
  {
    mBufferedMode = flag;
    bufferedModeChanged();
  }
}

void InputDevice::bufferedModeChanged()
{
}

InputDeviceType InputDevice::getType()
{
  return mDeviceType;
}

const int InputDeviceManager::getShiftStates()
{
  return mShiftStates;
}

