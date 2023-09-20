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

#include "GjMouseDeviceDX.h"
#include <cstdlib>
using namespace yaglib;

DIMouseDevice::DIMouseDevice(LPDIRECTINPUT8 lpdi8, NativeApplication& application) :
  DIAxialDevice(lpdi8, application)
{
  mDeviceType = IDT_MOUSE;
  initMonitors(GetSystemMetrics(SM_CMOUSEBUTTONS), MAX_AXES);
}

DIMouseDevice::~DIMouseDevice()
{
}

HRESULT DIMouseDevice::init()
{
  mIsFullScreen = mApplication.isFullScreen();

  if(FAILED(prepare(GUID_SysMouse, &c_dfDIMouse)))
    return E_FAIL;

  if(!(mEvent = CreateEvent(NULL, FALSE, FALSE, NULL)))
    return E_FAIL;

  if(FAILED(mDevice->SetEventNotification(mEvent)))
    return E_FAIL;

  DIPROPDWORD dipdw;
  dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  dipdw.diph.dwObj = 0;
  dipdw.diph.dwHow = DIPH_DEVICE;
  dipdw.dwData = MOUSE_QUEUE_SIZE;

  if(FAILED(mDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
    return E_FAIL;

  mDevice->Acquire();

#ifndef WINDOWS_MOUSE_MOVEMENTS
  if(mIsFullScreen)
  {
#endif
    POINT p;
    GetCursorPos(&p);
    Point3 winMouse = Point3(p);
    mAxesMonitor->setPosition(winMouse);
#ifndef WINDOWS_MOUSE_MOVEMENTS
  }
  else
    mAxesMonitor->setPosition(Point3(0));
#endif

  return S_OK;
}

HRESULT DIMouseDevice::update()
{
  DIDEVICEOBJECTDATA data[MOUSE_QUEUE_SIZE];
  DWORD dwCount = MOUSE_QUEUE_SIZE;

  memset(&data, 0, sizeof(DIDEVICEOBJECTDATA)*MOUSE_QUEUE_SIZE);
  if(FAILED(getData(IDT_MOUSE, &data[0], &dwCount, sizeof(DIDEVICEOBJECTDATA))))
    return E_FAIL;

  Point3 delta(0, 0, 0);
  Point3 position; 
  mAxesMonitor->getPosition(position);

  // process button changes, but NOT mouse movements.  it is easier
  // if movements are tracked separately using windows facilities so that
  // no more extra settings is required
  for(DWORD i = 0; i < dwCount; i++)
  {
    int buttonCode = -1;

    switch(data[i].dwOfs)
    {
#ifdef WINDOWS_MOUSE_MOVEMENTS
    case DIMOFS_X:
      if(mIsFullScreen)
        delta.x += (int)data[i].dwData;
      break;

    case DIMOFS_Y:
      if(mIsFullScreen)
        delta.y += (int)data[i].dwData;
      break;

    case DIMOFS_Z:
      if(mIsFullScreen)
        delta.z += (int)data[i].dwData;
      break;
#endif
    case DIMOFS_BUTTON0:
      buttonCode = 0;
      break;

    case DIMOFS_BUTTON1:
      buttonCode = 1;
      break;

    case DIMOFS_BUTTON2:
      buttonCode = 2;
      break;

    case DIMOFS_BUTTON3:
      buttonCode = 3;
      break;

    } // switch

#ifndef WINDOWS_MOUSE_MOVEMENTS
    // process movements
    if(delta != 0)
    {
      position += delta;        // update the position
      mAxesMonitor->getBounds().clip(position);  // clip position if necessary
    }
#endif

    // process buttons
    if(buttonCode != -1)
    {
      mButtonMonitor->updateIfChanged(buttonCode,
        (data[i].dwData & 0x80) != 0, data[i].dwTimeStamp, !isBuffered()); 
    }

  } // for

#ifndef WINDOWS_MOUSE_MOVEMENTS
  if(!mIsFullScreen)
  {
#endif
    // get current mouse position via windows api
    POINT p;
    GetCursorPos(&p);
#ifdef WINDOWS_MOUSE_MOVEMENTS
    if(!mIsFullScreen)
#endif
    ScreenToClient(mWindowHandle, &p);
    Point3 winMouse = Point3(p);
    mAxesMonitor->getBounds().clip(winMouse);

    if(winMouse != position)          
    {
      delta = winMouse - position;
      //delta = position - winMouse;
      position = winMouse;
    }
#ifndef WINDOWS_MOUSE_MOVEMENTS
  }
#endif

  if(delta != 0)
  {
    mAxesMonitor->setMovement(delta);
    mAxesMonitor->setPosition(position);
  }

  return S_OK;
}
