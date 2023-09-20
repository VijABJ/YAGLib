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

#include "GjWin32Window.h"
using namespace yaglib;

LRESULT __stdcall _winProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  // warning C4312 comes up here since, apparently, LONG_PTR simplifies to LONG, which is too
  // small to contain a 64-bit pointer value.  This shouldn't be an issue on 32-bit code.
  Win32WindowWrapper* window = reinterpret_cast<Win32WindowWrapper*> (GetWindowLongPtr(hWnd, GWLP_USERDATA));
  if(NULL != window)
  {
    try
    {
      window->_callback(hWnd, msg, wParam, lParam);
    }
    catch(...)
    {
    }
  }
  
  return (DefWindowProc(hWnd, msg, wParam, lParam));
}

Win32WindowWrapper::Win32WindowWrapper(const WideString& className, const WideString& caption, 
  HINSTANCE instance, const bool showSystemCursor, const bool fullScreen, 
  const int width, const int height,  const int left, const int top) :
  mWindowHandle(NULL), mWinStyle(0), mClassName(className), mCaption(caption), 
  mIsFullScreen(fullScreen), mShowCursor(showSystemCursor), mLeft(left), mTop(top),
  mWidth(width), mHeight(height)
{
  // create a window class structure and initialize it
  memset(&mWndClassEx, 0, sizeof(WNDCLASSEX));
  mWndClassEx.cbSize        = sizeof(WNDCLASSEX);
  mWndClassEx.style         = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
  mWndClassEx.lpfnWndProc   = &_winProc;
  mWndClassEx.cbClsExtra    = 4; // size of pointer to Self
  mWndClassEx.cbWndExtra    = 0;
  mWndClassEx.hInstance     = instance;
  mWndClassEx.hIcon         = LoadIcon(0, IDI_APPLICATION);
  mWndClassEx.hCursor       = 0; // LoadCursor (0, IDC_ARROW);
  mWndClassEx.hbrBackground = 0; // GetStockObject (BLACK_BRUSH);
  mWndClassEx.lpszMenuName  = NULL;
  mWndClassEx.hIconSm       = LoadIcon(0, IDI_APPLICATION);
  mWndClassEx.lpszClassName = mClassName.c_str();
}

Win32WindowWrapper::~Win32WindowWrapper()
{
  destroy();
}

NATIVE_WINDOW_HANDLE Win32WindowWrapper::getWindowHandle() const
{
  return mWindowHandle;
}

bool Win32WindowWrapper::isFullScreen() const
{
  return mIsFullScreen;
}

void Win32WindowWrapper::_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  RECT rBounds;
  switch(msg) {
    case WM_CREATE:
      if(GetWindowRect(mWindowHandle, &rBounds))
        SetWindowPos(mWindowHandle, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE);
      callIntercept(WindowCreated);
      break;

    case WM_DESTROY:
      callIntercept(WindowDestroyed);
      break;

    case WM_ACTIVATE:
      callIntercept(((wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE)) ?
        WindowActivated : WindowDeactivated);
      break;

    case WM_MOVE:
      callIntercept(WindowMoved);
      break;

    //case WM_SETCURSOR:
    //  CursorShown (false);
    //  break;
  }

  Win32Params params(msg, wParam, lParam);
  callIntercept(Forwarded, &params);
}

void Win32WindowWrapper::callIntercept(NativeApplicationEvent event, Win32Params* params)
{
  if(!mIntercept.empty())
    mIntercept(event, mWindowHandle, static_cast<void*>(params));
}

bool Win32WindowWrapper::create(InterfaceIntercept intercept)
{
  // remove the cursor if it's not needed
  mWndClassEx.hCursor = mShowCursor ? LoadCursor(0, IDC_ARROW) : 0;

  // set full screen flags
  mWinStyle = (mIsFullScreen) ? 
    WS_POPUP : 
    (WS_EX_APPWINDOW | WS_EX_TOPMOST | WS_SYSMENU); // or 0

  // Register window class
  if(RegisterClassEx(&mWndClassEx) == 0) return false;

  // Create the main Window
  mWindowHandle = CreateWindow(mClassName.c_str(), mCaption.c_str(), mWinStyle,
    mLeft, mTop, mWidth, mHeight, 0, 0, mWndClassEx.hInstance, NULL);
  if (mWindowHandle == 0) return false;

  // store the pointer for this class
  // warning C4244 comes up to point out that the 64-bit pointer value of this is too
  // large to store in a LONG_PTR (which is a LONG). ignore this for 32-bit builds.
  mIntercept = intercept;
  SetWindowLongPtr(mWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

  // adjust the size so that the client area is the requested dimensions
  if(!mIsFullScreen)
  {
    RECT winRect, clientRect;
    GetWindowRect(mWindowHandle, &winRect);
    GetClientRect(mWindowHandle, &clientRect);

    int winWidth = winRect.right - winRect.left;
    int winHeight = winRect.bottom - winRect.top;
    int hDelta = winWidth - (clientRect.right-clientRect.left);
    int vDelta = winHeight - (clientRect.bottom-clientRect.top);

    MoveWindow(mWindowHandle, winRect.left, winRect.top, 
      winWidth + hDelta, winHeight + vDelta, TRUE);
  }

  // show the window on the screen
  ShowWindow(mWindowHandle, SW_SHOW);
  UpdateWindow(mWindowHandle);

  // set the focus on our newly created window
  SetFocus(mWindowHandle);

  // hide the cursor!
  if(!mShowCursor)
    ShowCursor(FALSE);

  return true;
}

void Win32WindowWrapper::destroy()
{
  if(mWindowHandle != NULL)
  {
    mIntercept.clear();
    DestroyWindow(mWindowHandle);
    mWindowHandle = NULL;
  }
}