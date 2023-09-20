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
 * @file  GjKeyboardDeviceDX.h
 * @brief DirectInput keyboard
 *
 */

#ifndef GJ_KEYBOARD_DEVICE_DX_HEADER
#define GJ_KEYBOARD_DEVICE_DX_HEADER

#include "GjInputDeviceDX.h"

namespace yaglib
{

// forward declaration
class DIKeyboardDevice;

class KeyboardButtonMonitor : public ButtonMonitor
{
public:
  static const int NUMBER_OF_BUTTONS = 256;
  static const size_t KEYBOARD_BUFFER_SIZE = NUMBER_OF_BUTTONS * sizeof(char);

  KeyboardButtonMonitor();

  virtual bool isPressed(const int buttonCode) const;
  virtual bool isReleased(const int buttonCode) const;

private:
  friend class DIKeyboardDevice;
  static const int PRESSED_BIT = 0x80;

  char mCurrBuffer[NUMBER_OF_BUTTONS];
  char mPrevBuffer[NUMBER_OF_BUTTONS];

  void clear();
  void updateBuffers(char* newValues);
  void notifyOnChanges();
};

class DIKeyboardDevice : public BaseDIDevice
{
public:
  DIKeyboardDevice(LPDIRECTINPUT8 lpdi8, NativeApplication& application);
  virtual ~DIKeyboardDevice();

  virtual HRESULT init();
  virtual HRESULT update();

  virtual ButtonMonitor* const getButtonMonitor() const;

private:
  KeyboardButtonMonitor* mButtonMonitor;

protected:
  virtual void bufferedModeChanged();
};

} /* namespace yaglib */


#endif /* GJ_KEYBOARD_DEVICE_DX_HEADER */
