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

#include "GjNativeApp.h"
#include "GjStringUtils.h"
#include "_GjIntercepts.h"
#include <boost/bind.hpp>
using namespace yaglib;

//* NativeApplication

NativeApplication::NativeApplication(const NATIVE_INSTANCE instance) :
  mInstance(instance), mIsRunning(false), mProxies(new InterceptProxies())
{
}

NativeApplication::~NativeApplication()
{
  delete mProxies;
}

NATIVE_INSTANCE NativeApplication::getInstance() const
{
  return mInstance;
}

bool NativeApplication::isRunning() const
{
  return mIsRunning;
}

static WideString s_ApplicationPath(L"");
static WideString s_ApplicationFile(L"");

void checkStaticPaths()
{
  if(s_ApplicationFile.size() == 0)
  {
    const int MAX_PATH_LENGTH = 256;
    TCHAR buf[MAX_PATH_LENGTH];
    GetModuleFileName(0, buf, MAX_PATH_LENGTH);

    s_ApplicationFile = WideString(buf);
    s_ApplicationPath = string_utils::chop_after_last_copy(s_ApplicationFile, WideString(L"\\"));
  }
}

WideString const& NativeApplication::ApplicationPath()
{
  checkStaticPaths();
  return s_ApplicationPath;
}

WideString const& NativeApplication::ApplicationFile()
{
  checkStaticPaths();
  return s_ApplicationFile;
}

void NativeApplication::windowCallback(NativeApplicationEvent event, NATIVE_WINDOW_HANDLE handle, void* params)
{
  (*mProxies)(event, handle, params);
  if(event == WindowDestroyed) --mWindowCount;
}

void NativeApplication::subscribe(InterfaceIntercept handler, const WideString& name, 
  const bool acceptsFoward)
{
  mProxies->add(handler, name, acceptsFoward);
}

void NativeApplication::unsubscribe(const WideString& name)
{
  mProxies->remove(name);
}

NativeWindowPtr& NativeApplication::addWindow(NativeWindowInterface* window)
{
  mWindows.push_back(NativeWindowPtr(window));
  return mWindows[mWindows.size()-1];
}

NativeWindowPtr const& NativeApplication::getWindow(const int index) const
{
  assert((index >= 0) && (index < (int)mWindows.size()));
  return mWindows[index];
}

bool NativeApplication::initialize()
{
  CoInitialize(NULL);
  for(WindowList::iterator iter = mWindows.begin(); iter != mWindows.end(); ++iter)
  {
    (*iter)->create(InterfaceIntercept(
      boost::bind(&NativeApplication::windowCallback, this, _1, _2, _3))); 
  }

  windowCallback(ApplicationStartup, BAD_WINDOW_HANDLE, NULL);
  mIsRunning = true;
  mWindowCount = (int)mWindows.size();

  return mIsRunning;
}

void NativeApplication::shutdown()
{
  mIsRunning = false;
  windowCallback(ApplicationShutdown, BAD_WINDOW_HANDLE, NULL);
  CoUninitialize();
}

int NativeApplication::run()
{
  if(!initialize()) 
    return -1;

  MSG msg;
  memset(&msg, 0, sizeof(MSG));
  while(msg.message != WM_QUIT) {
    // check if queue has something in it
    if(PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) 
    {
      // yep, it has something. empty the message queue
      while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
      {
        if (msg.message == WM_QUIT) break;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    } else
      // no message in the queue, do our thing
      windowCallback(ApplicationIdle, BAD_WINDOW_HANDLE, NULL);
    // check if we MUST terminate now. this happens when ALL the 
    // windows have been closed
    if(mWindowCount <= 0)
      terminate();
  }

  shutdown();
  return 0;
}

void NativeApplication::terminate()
{
  if(!mIsRunning) return;
  PostQuitMessage(0);
}

NATIVE_WINDOW_HANDLE NativeApplication::getWindowHandle(const int index) const
{
  assert((index >= 0) && (index < (int)mWindows.size()));
  return mWindows[index]->getWindowHandle();
}


