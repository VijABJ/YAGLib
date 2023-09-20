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

#include "GjBaseFramework.h"
#include "GjUnicodeUtils.h"
#include "GjResourceManagement.h"
#include "GjIniFiles.h"
#include "GjInputManagerDX.h"
#include "GjKeyCodes.h"
#include "GjWin32Window.h"
#include "GjStringUtils.h"
#include "GjBFS.h"
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
//#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>
using namespace yaglib;

BasicFramework::BasicFramework() : DeviceSink(), mAlreadyStartedUp(false), 
  mApplication(NULL), mDeviceManager(NULL), mAppInstance(0), mFPSTracker(SimpleTimer(1.0f)),
  mDeviceOptions(INTERCEPT_KEYBOARD), mShowFPS(true), mGTimer(NULL),
  mRedirector(NULL)
{
  mKeyboardListener = new KeyboardListener(this);
  mMouseButtonListener = new MouseButtonListener(this);
  mMouseMovementListener = new MouseMovementListener(this);
  mGamePadButtonListener = new GamePadButtonListener(this);
  mGamePadMovementListener = new GamePadMovementListener(this);
  mRedirector = new BasicFramework::Redirector(*this);
}

BasicFramework::~BasicFramework()
{
  SAFE_DELETE(mApplication);
  SAFE_DELETE(mDeviceManager);
  SAFE_DELETE(mKeyboardListener);
  SAFE_DELETE(mMouseButtonListener);
  SAFE_DELETE(mMouseMovementListener);
  SAFE_DELETE(mGamePadButtonListener);
  SAFE_DELETE(mGamePadMovementListener);
  SAFE_DELETE(mRedirector);
}

#define DEFAULT_CONFIGURATION_FILE        L"main.cfg"
#define DEFAULT_CONFIGURATION_EXTENSION   L"cfg"

bool BasicFramework::execute(HINSTANCE instance, TCHAR* winClass, TCHAR* winTitle,
  const bool fullScreen, const int startWidth, const int startHeight)
{
  mAppInstance = instance;
  mApplication = new NativeApplication(instance);
  InterfaceIntercept intercept(boost::bind(&BasicFramework::Redirector::handler, *mRedirector, _1, _2, _3));
  mApplication->subscribe(intercept, L"Main", true);
  mApplication->addWindow(
    new yaglib::Win32WindowWrapper(winClass, winTitle, instance, 
      !fullScreen, fullScreen, startWidth, startHeight));

  mApplication->run();
  SAFE_DELETE(mApplication);

  return true;
}

void BasicFramework::interceptKey(const int keyCode)
{
  mKeyboard->getButtonMonitor()->attach(mKeyboardListener, keyCode);
}

void BasicFramework::staticAppHandler(
  NativeApplicationEvent event, NATIVE_WINDOW_HANDLE hWindow, void* appMessage)
{
  appHandler(event, hWindow, appMessage);
}

void BasicFramework::appHandler(NativeApplicationEvent event, NATIVE_WINDOW_HANDLE hWindow, void* appMessage)
{
  // handle windows events here
  if(appMessage)
  {
    // the game framework need not update the window areas.
    // however, it was recommended to intercept and clear this
    // message to keep Windows happy
    Win32Params* wp = (Win32Params*)appMessage;
    if(wp->msg == WM_PAINT)
    {
      PAINTSTRUCT ps;
      BeginPaint(hWindow, &ps);
      EndPaint(hWindow, &ps);
      return;
    }
  }
  // handle the application-generated events
  switch(event)
  {
  case ApplicationStartup:
    if(!mAlreadyStartedUp)
      mAlreadyStartedUp = startup();
    break;
  case ApplicationShutdown:
    if(mAlreadyStartedUp)
    {
      shutdown();
      mAlreadyStartedUp = false;
    }
    break;
  case ApplicationIdle:
    perform();
    break;
  case WindowActivated:
    if(mGTimer) mGTimer->start();
    break;
  case WindowDeactivated:
    if(mGTimer) mGTimer->stop();
    break;
  }

  return;
}

bool BasicFramework::startup()
{
  // create the GlobalSettings instance
  GlobalSettings* gs = new GlobalSettings();
  ResourceManager* rm = new ResourceManager();
  mGTimer = new GlobalTimer();

  // we need the configuration filename. check first for the exeName.cfg
  // if that is not found, then use the default configuration. if THAT
  // still doesn't exist, then there is simply nothing to load
  {
    WideString configFile = string_utils::chop_after_last_copy(
      NativeApplication::ApplicationFile(), WideString(L"."), true) +
      DEFAULT_CONFIGURATION_EXTENSION;

    if(!bfs::exists(configFile) || bfs::is_directory(configFile))
      configFile = DEFAULT_CONFIGURATION_FILE;
    // 
    if(bfs::exists(configFile))
      g_GlobalSettings.loadFrom(IniSettingsStore(), configFile);
  }

  // make sure the singletons are initialized
  g_ResourceManager.initialize(gs->getSettings());

  // create the device manager, and initialize it
  mDeviceManager = new InputDeviceManagerDX();
  mDeviceManager->initialize(*mApplication, L"");

  // store pointers for the devices
  mKeyboard = mDeviceManager->getDevice(IDT_KEYBOARD);
  mMouse = mDeviceManager->getDevice(IDT_MOUSE);
  mGamePad = mDeviceManager->getDevice(IDT_JOYSTICK);

  // set the mouse borders, and mouse configuration
  if(mMouse)
  {
    int viewWidth = mApplication->getWindow()->getWidth(),
        viewHeight = mApplication->getWindow()->getHeight();
    mMouse->setBuffered(true);
    mMouse->getAxesMonitor()->setBounds(Rect(Point(0, 0), Point(viewWidth, viewHeight)));
  }

  // attach device listeners.  whereas the keyboard will be hooked regardless, all
  // the other devices are optional and controlled via the device options bit flags

  mDeviceOptions |= INTERCEPT_KEYBOARD;
  mKeyboard->getButtonMonitor()->attach(mKeyboardListener, KEY_ESCAPE);

  if(mMouse && ((mDeviceOptions & INTERCEPT_MOUSE) != 0))
  {
    if(mDeviceOptions & INTERCEPT_MOUSE_BUTTONS)
      mMouse->getButtonMonitor()->attach(mMouseButtonListener, ButtonMonitor::ALL_BUTTONS);

    if(mDeviceOptions & INTERCEPT_MOUSE_MOVEMENTS)
      mMouse->getAxesMonitor()->attach(mMouseMovementListener, AxesMonitor::ALL_AXES);
  }

  if(mGamePad && ((mDeviceOptions & INTERCEPT_GAMEPAD) != 0))
  {
    if(mDeviceOptions & INTERCEPT_GAMEPAD_BUTTONS)
      mGamePad->getButtonMonitor()->attach(mGamePadButtonListener, ButtonMonitor::ALL_BUTTONS);

    if(mDeviceOptions & INTERCEPT_GAMEPAD_MOVEMENTS)
      mGamePad->getAxesMonitor()->attach(mGamePadMovementListener, AxesMonitor::ALL_AXES);
  }

  // initialize the sound manager
  mSoundManager = new SoundManager();
  mSoundManager->initialize(mApplication->getWindowHandle(), g_GlobalSettings.getSettings());

  mFramesSoFar = mFPS = 0;
  mGTimer->start();

  return true;
}

void BasicFramework::shutdown()
{
  SAFE_DELETE(mSoundManager);
  SAFE_DELETE(mDeviceManager);
  SAFE_DELETE(mGTimer);

  delete ResourceManager::InstancePtr();
  delete GlobalSettings::InstancePtr();
}

void BasicFramework::perform()
{
  if(mDeviceManager == NULL) 
    return;

  double timeSinceLast = mGTimer->getElapsedTime();
  mDeviceManager->update();

  // calculate FPS, we might want to display it
  if(mFPSTracker.update(timeSinceLast))
  {
    mFPS = mFramesSoFar;
    if(mShowFPS) fpsUpdated();
    mFramesSoFar = 0;
  }
  else
    mFramesSoFar++;

  beforeFrame(timeSinceLast);
  frame(timeSinceLast);
  afterFrame(timeSinceLast);
}

void BasicFramework::handleKeyboardChange(int buttonCode, bool pressed)
{
  if(buttonCode == KEY_ESCAPE)
    PostQuitMessage(0);
}

