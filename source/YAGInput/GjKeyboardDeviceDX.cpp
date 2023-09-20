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

#include "GjKeyboardDeviceDx.h"
#include <cstdlib>
using namespace yaglib;

KeyboardButtonMonitor::KeyboardButtonMonitor() : ButtonMonitor(NUMBER_OF_BUTTONS)
{
  clear();
}

void KeyboardButtonMonitor::clear()
{
  memset(&mCurrBuffer, 0, KEYBOARD_BUFFER_SIZE);
  memset(&mPrevBuffer, 0, KEYBOARD_BUFFER_SIZE);
}

void KeyboardButtonMonitor::updateBuffers(char* newValues)
{
  memcpy(mPrevBuffer, mCurrBuffer, KEYBOARD_BUFFER_SIZE);
  memcpy(mCurrBuffer, newValues, KEYBOARD_BUFFER_SIZE);
}

void KeyboardButtonMonitor::notifyOnChanges()
{
  char* prev = mPrevBuffer;
  char* curr = mCurrBuffer;
  for(int i = 0; i < KeyboardButtonMonitor::NUMBER_OF_BUTTONS; i++, prev++, curr++)
    if(*prev != *curr)
      notify(i);
}

bool KeyboardButtonMonitor::isPressed(const int buttonCode) const
{
  return ((buttonCode >= 0) && (buttonCode < NUMBER_OF_BUTTONS)) ?
    (mCurrBuffer[buttonCode] & PRESSED_BIT) != 0 : false;
}

bool KeyboardButtonMonitor::isReleased(const int buttonCode) const
{
  return ((buttonCode >= 0) && (buttonCode < NUMBER_OF_BUTTONS)) ?
    (mCurrBuffer[buttonCode] & PRESSED_BIT) == 0 : true;
}


/////////////////////////////////////////////////////////

DIKeyboardDevice::DIKeyboardDevice(LPDIRECTINPUT8 lpdi8, NativeApplication& application) :
  BaseDIDevice(lpdi8, application), mButtonMonitor(new KeyboardButtonMonitor())
{
  mDeviceType = IDT_KEYBOARD;
}

DIKeyboardDevice::~DIKeyboardDevice()
{
  delete mButtonMonitor;
}

void DIKeyboardDevice::bufferedModeChanged()
{
}

HRESULT DIKeyboardDevice::init()
{
  if(FAILED(prepare(GUID_SysKeyboard, &c_dfDIKeyboard)))
    return E_FAIL;

  mButtonMonitor->clear();
  mDevice->Acquire ();
  return S_OK;
}

HRESULT DIKeyboardDevice::update()
{
  if(mDevice == NULL)
    return E_FAIL;

  // get the current keyboard buffer
  char buffer[KeyboardButtonMonitor::NUMBER_OF_BUTTONS];
  if(FAILED(getData(IDT_KEYBOARD, &buffer[0], NULL, KeyboardButtonMonitor::KEYBOARD_BUFFER_SIZE)))
    return E_FAIL;

  // if successfully retrieved the keyboard buffer, we copy some more buffers
  mButtonMonitor->updateBuffers(buffer);

  /* loop through the array and check for changes.
   * later we don't need to do that if console mode is in effect.
   */
  if(!isBuffered())
    mButtonMonitor->notifyOnChanges();
  else
  {
    // TODO: buffered input here
  }

  return S_OK;
}

ButtonMonitor* const DIKeyboardDevice::getButtonMonitor() const
{
  return mButtonMonitor;
}

