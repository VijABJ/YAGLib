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
 * @file  GjWin32Window.h
 * @brief Wraps Win32's windowing system with our NativeWindowInterface
 *
 */
#ifndef GJ_WIN32_WINDOWS_HEADER
#define GJ_WIN32_WINDOWS_HEADER

#include "GjNativeDefs.h"
#include <boost/function.hpp>
#include <boost/utility.hpp>

#ifndef GJPLATFORM_WINDOWS
#error This module is for the Windows platform only
#endif

namespace yaglib
{

class Win32WindowWrapper : private boost::noncopyable, public NativeWindowInterface
{
public:
  Win32WindowWrapper(const WideString& className, const WideString& caption, HINSTANCE instance,
    const bool showSystemCursor, const bool fullScreen, 
    const int width, const int height,  const int left = 0, const int top = 0);
  virtual ~Win32WindowWrapper();

  // NOTE: is it VERY IMPORTANT that this member function be the first in
  // the declaration list, aside from te ctor/dtor pair.
  void _callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

  // implements NativeWindowInterface
  virtual bool create(InterfaceIntercept intercept);
  virtual void destroy();

  virtual NATIVE_WINDOW_HANDLE getWindowHandle() const;
  virtual bool isFullScreen() const;
  virtual void setFullScreen(const bool newState) {};

  virtual int getWidth() const { return mWidth; };
  virtual int getHeight() const { return mHeight; };

private:
  HWND mWindowHandle;
  UINT mWinStyle;
  WideString mClassName;
  WideString mCaption;
  bool mIsFullScreen;
  bool mShowCursor;
  //
  int mLeft;
  int mTop;
  int mWidth;
  int mHeight;
  //
  InterfaceIntercept mIntercept;
  WNDCLASSEX mWndClassEx;

  void callIntercept(NativeApplicationEvent event, Win32Params* params = NULL);
};

} /* namespace yaglib */

#endif /* GJ_WIN32_WINDOWS_HEADER */
