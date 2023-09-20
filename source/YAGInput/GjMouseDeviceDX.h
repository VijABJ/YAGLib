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
 * @file  GjMouseDeviceDX.h
 * @brief DirectInput mouse wrapper
 *
 */

#ifndef GJ_MOUSE_DEVICE_DX_HEADER
#define GJ_MOUSE_DEVICE_DX_HEADER

#include "GjAxialDeviceDX.h"

// undefine if you want to use directinput monitoring of mouse movements
#define WINDOWS_MOUSE_MOVEMENTS

namespace yaglib
{

class DIMouseDevice : public DIAxialDevice
{
public:
  DIMouseDevice(LPDIRECTINPUT8 lpdi8, NativeApplication& application);
  virtual ~DIMouseDevice();

  virtual HRESULT init();
  virtual HRESULT update();

private:
  static const int MAX_MOUSE_BUTTONS = 5;   ///< left, right, middle, and two side-buttons
  static const int MAX_AXES = 2;            ///< the actual mouse axis, and the mouse wheel
  static const DWORD MOUSE_QUEUE_SIZE = 16; ///< mouse queue

  HANDLE mEvent;
  bool mIsFullScreen;
};

} /* namespace yaglib */


#endif /* GJ_MOUSE_DEVICE_DX_HEADER */
