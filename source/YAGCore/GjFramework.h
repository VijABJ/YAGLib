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
 * @file  GjFramework.h
 * @brief Application framework
 *
 */

#ifndef GJ_GAME_FRAMEWORK_HEADER
#define GJ_GAME_FRAMEWORK_HEADER

#include "GjDefs.h"
#include "GjBaseFramework.h"
#include "GjScreen.h"
#include "GjNativeApp.h"

namespace yaglib
{

class ExtendedFramework;

class DisplayDevice
{
  friend class ExtendedFramework;
public:
  DisplayDevice() : mApplication(NULL) {};

protected:
  NativeApplication* mApplication;

  virtual bool _createScreenObject(Screen** screen) = 0;
  virtual bool _startup() = 0;
  virtual int _handleApplicationEvents(NativeApplicationEvent event, NATIVE_WINDOW_HANDLE hWindow, void* appMessage) = 0;

  bool createScreenObject(Screen** screen) { return _createScreenObject(screen); };
  bool startup() { return _startup(); };
  int appHandler(NativeApplicationEvent event, NATIVE_WINDOW_HANDLE hWindow, void* appMessage)
    { return _handleApplicationEvents(event, hWindow, appMessage); };
};

class ExtendedFramework : public BasicFramework
{
public:
  ExtendedFramework(DisplayDevice* displayDevice);
  virtual ~ExtendedFramework();

  DisplayDevice* getDisplayDevice() { return mDisplayDevice; };

protected:
  DisplayDevice* mDisplayDevice;
  Screen* mScreen;
  FontSprite* mFPSFont;

  virtual bool startup();
  virtual void shutdown();
  virtual void appHandler(NativeApplicationEvent event, NATIVE_WINDOW_HANDLE hWindow, void* appMessage);

  virtual bool beforeFrame(const double timeSinceLast);
  virtual bool afterFrame(const double timeSinceLast);
  virtual void fpsUpdated();
};

} /* namespace yaglib */

#endif /* GJ_GAME_FRAMEWORK_HEADER */
