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
 * @file  GjInputDeviceDX.h
 * @brief DirectInput Device Base
 *
 * Base class for directinput wrappers
 *
 */

#ifndef GJ_INPUT_DEVICE_DX_HEADER
#define GJ_INPUT_DEVICE_DX_HEADER

#define DIRECTINPUT_VERSION 0x0800

#include "GjInputDevice.h"
#include "GjDirectX.h"
#include "dinput.h"

namespace yaglib
{

class BaseDIDevice : public InputDevice
{
public:
  BaseDIDevice(LPDIRECTINPUT8 lpdi8, NativeApplication& application);
  virtual ~BaseDIDevice();  

  virtual HRESULT prepare(REFGUID rguid, LPCDIDATAFORMAT lpdf);
  virtual HRESULT init() = 0;
  virtual HRESULT update() = 0;

  LPDIRECTINPUTDEVICE8 device8();
  LPDIRECTINPUT8 directInput8();

protected:
  LPDIRECTINPUTDEVICE8  mDevice;
  LPDIRECTINPUT8        mDInput;
  HWND                  mWindowHandle;
  NativeApplication&          mApplication;

  void release();
  HRESULT getData(InputDeviceType idt, void* pData, DWORD* pdwCount, const size_t dataSize);
};

} /* namespace yaglib */


#endif /* GJ_INPUT_DEVICE_DX_HEADER */
