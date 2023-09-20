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

#include "GjJoystickDeviceDX.h"
#include <cstdlib>
using namespace yaglib;

DIJoystickDevice::DIJoystickDevice(LPDIRECTINPUT8 lpdi8, NativeApplication& application) :
  DIAxialDevice(lpdi8, application)
{
  mDeviceType = IDT_JOYSTICK;
}

DIJoystickDevice::~DIJoystickDevice()
{
}

HRESULT DIJoystickDevice::init()
{
  mDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, 
    &handleJoystickEnumeration, this, DIEDFL_ATTACHEDONLY);

  if(mDevice == NULL)
    return E_FAIL;

  DIDEVCAPS diCaps;
  memset(&diCaps, 0, sizeof(diCaps));
  diCaps.dwSize = sizeof(diCaps);

  int numberOfButtons = 4, numberOfAxes = 1;
  if(SUCCEEDED(mDevice->GetCapabilities(&diCaps)))
  {
    numberOfButtons = diCaps.dwButtons;
    numberOfAxes = diCaps.dwAxes;
  }

  initMonitors(numberOfButtons, numberOfAxes);

  const int AXIS_LOW_RANGE = -1000;
  const int AXIS_HIGH_RANGE = 1000;

  // set the range for the axes
	DIPROPRANGE diprg; 
	diprg.diph.dwSize = sizeof(DIPROPRANGE); 
	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	diprg.diph.dwHow = DIPH_BYOFFSET; 
	diprg.lMin = AXIS_LOW_RANGE; 
	diprg.lMax = AXIS_HIGH_RANGE; 

  DWORD dwObj[] = {
    DIJOFS_X, DIJOFS_Y, DIJOFS_Z, 
    DIJOFS_RX, DIJOFS_RY, DIJOFS_RZ};
  for(int i = 0; i < sizeof(dwObj); i++)
  {
    diprg.diph.dwObj = dwObj[i];
    mDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
  }

  // calibrate/center joystick
  // catch the HRESULT of failure? maybe not now...
	DIPROPDWORD dipdw;
  memset(&dipdw, 0, sizeof(dipdw));
	dipdw.diph.dwSize = sizeof(DIPROPDWORD); 
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE; 
	dipdw.dwData = DIPROPAUTOCENTER_ON;
	mDevice->SetProperty(DIPROP_AUTOCENTER, &dipdw.diph);

  return S_OK;
}

HRESULT DIJoystickDevice::update()
{
  mDevice->Poll();

  DIJOYSTATE2 js;
  memset(&js, 0, sizeof(js));
  if(FAILED(getData(IDT_JOYSTICK, &js, NULL, sizeof(js))))
    return E_FAIL;

  // the new position, and the movement change
  Point3 previous, current, delta;

  mAxesMonitor->getPosition(previous);
  current = Point3(js.lX, js.lY, 0); 
  if(previous != current)
  {
    delta = previous - current;
    mAxesMonitor->setMovement(delta);
    mAxesMonitor->setPosition(current);
  }

  if(mAxesMonitor->getNumberOfAxes() > 1)
  {
    mAxesMonitor->getPosition(previous, 1);
    current = Point3(js.lZ, js.lRz, 0);
    if(previous != current)
    {
      delta = previous - current;
      mAxesMonitor->setMovement(delta, 1);
      mAxesMonitor->setPosition(current, 1);
    }
  }

  // button states
  DWORD timeStamp = GetTickCount();
  for(int i = 0; i < mButtonMonitor->getNumberOfButtons(); i++)
  {
    mButtonMonitor->updateIfChanged(i, 
      (js.rgbButtons[i] & 0x80) != 0, timeStamp, !isBuffered());
  }

  return S_OK;
}

BOOL DIJoystickDevice::enumJoystickHandler(const DIDEVICEINSTANCE* pInst)
{
  if(SUCCEEDED(prepare(pInst->guidInstance, &c_dfDIJoystick2)))
  {
    strcpy(mName, (char*)pInst->tszProductName);
    return DIENUM_STOP;
  }

  return DIENUM_CONTINUE;
}

BOOL CALLBACK DIJoystickDevice::handleJoystickEnumeration(    
  const DIDEVICEINSTANCE* pInst, void* pUserData)
{
  DIJoystickDevice* joy = reinterpret_cast<DIJoystickDevice*>(pUserData);
  if(joy == NULL)
    return DIENUM_STOP;

  return joy->enumJoystickHandler(pInst);
}

