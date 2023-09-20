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
 * @file  GjNativeDefs.h
 * @brief Definitions required by NativeXXXXX modules
 *
 */
#ifndef GJ_NATIVE_DEFS_HEADER
#define GJ_NATIVE_DEFS_HEADER

#include "GjDefs.h"
#include <boost/function.hpp>

namespace yaglib
{

#ifdef GJPLATFORM_WINDOWS

//
// windows implementation
/////////////////////////////

#define NATIVE_INSTANCE       HINSTANCE
#define NATIVE_WINDOW_HANDLE  HWND
#define BAD_WINDOW_HANDLE     NULL

typedef struct Win32Params
{
  UINT msg;
  WPARAM wParam;
  LPARAM lParam;
  //
  Win32Params(UINT _msg, WPARAM _wParam, LPARAM _lParam) :
    msg(_msg), wParam(_wParam), lParam(_lParam)
  { };
  //
} Win32Params;

#else
#error Unsupported platform
#endif

typedef enum NativeApplicationEvent 
{ 
  Undefined, Forwarded,
  ApplicationStartup, ApplicationShutdown, ApplicationIdle,
  WindowCreated, WindowDestroyed, WindowActivated, WindowDeactivated,
  WindowMoved, WindowFullScreenToggled, WindowResized
};

typedef boost::function<void (NativeApplicationEvent, NATIVE_WINDOW_HANDLE, void*)> InterfaceIntercept;

class NativeWindowInterface
{
public:
  virtual bool create(InterfaceIntercept intercept) = 0;
  virtual void destroy() = 0;

  virtual NATIVE_WINDOW_HANDLE getWindowHandle() const = 0;
  virtual bool isFullScreen() const = 0;
  virtual void setFullScreen(const bool newState) = 0;

  virtual int getWidth() const = 0;
  virtual int getHeight() const = 0;
};

} /* namespace yaglib */

#endif /* GJ_NATIVE_DEFS_HEADER */
