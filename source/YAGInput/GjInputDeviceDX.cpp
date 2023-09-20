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

#include "GjInputDeviceDX.h"
using namespace yaglib;

BaseDIDevice::BaseDIDevice(LPDIRECTINPUT8 lpdi8, NativeApplication& application) :
  mDevice(NULL), mDInput(lpdi8), mWindowHandle(application.getWindowHandle()),
  mApplication(application)
{
  setBuffered(false);
}

BaseDIDevice::~BaseDIDevice()
{
  release();
}

void BaseDIDevice::release()
{
  if(mDevice != NULL)
  {
    mDevice->Unacquire();
    mDevice->Release();
    mDevice = NULL;
  }
}

HRESULT BaseDIDevice::prepare(REFGUID rguid, LPCDIDATAFORMAT lpdf)
{
  release();

  if(FAILED(mDInput->CreateDevice(rguid, &mDevice, NULL)))
    return E_FAIL;

  if(FAILED(mDevice->SetDataFormat(lpdf)))
    return E_FAIL;

  DWORD dwFlags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;
  if(FAILED(mDevice->SetCooperativeLevel(mWindowHandle, dwFlags)))
    return E_FAIL;

  return S_OK;
}

HRESULT BaseDIDevice::getData(InputDeviceType idt, void* pData, DWORD* pdwCount, const size_t dataSize)
{
  HRESULT hr = E_FAIL;
  DWORD size = (DWORD) dataSize;

  if(idt == IDT_MOUSE)
    hr = mDevice->GetDeviceData(size, (DIDEVICEOBJECTDATA*)pData, pdwCount, 0);
  else
    hr = mDevice->GetDeviceState(size, pData);

  if(FAILED(hr))
  {
    if((hr == DIERR_NOTACQUIRED) || (hr == DIERR_INPUTLOST))
    {
      do
      {
        hr = mDevice->Acquire();
      } while(hr == DIERR_INPUTLOST);

      // if another app has the device priority, nothing to be done about it
      if(hr == DIERR_OTHERAPPHASPRIO) 
        return E_FAIL;

      if(SUCCEEDED(hr))
      {
        if(idt == IDT_MOUSE)
          hr = mDevice->GetDeviceData(size, (DIDEVICEOBJECTDATA*)pData, pdwCount, 0);
        else
          hr = mDevice->GetDeviceState(size, pData);
      }

      if(FAILED(hr)) 
        return E_FAIL;
    }
    else
      return E_FAIL;
  } // if(FAILED(hr))

  return S_OK;
}

inline LPDIRECTINPUTDEVICE8 BaseDIDevice::device8()
{
  return mDevice;
}

inline LPDIRECTINPUT8 BaseDIDevice::directInput8()
{
  return mDInput;
}

