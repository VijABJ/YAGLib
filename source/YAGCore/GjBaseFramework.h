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
 * @file  GjBaseFramework.h
 * @brief The base framework.  Only input and sound modules are binded
 *
 * The base framework
 *
 */
#ifndef GJ_BASE_FRAMEWORK_HEADER
#define GJ_BASE_FRAMEWORK_HEADER

#include "GjDefs.h"
#include "GjNativeApp.h"
#include "GjInputUtils.h"
#include "GjInputDevice.h"
#include "GjDeviceSink.h"
#include "GjSound.h"
#include "GjTimers.h"

namespace yaglib 
{

/**
 * @brief this is a basic framework for games
 * 
 * The application class is wrapped neatly here.  Also, the handling of
 * input devices are incorporated.<p>
 *
 * Notes for subclasses:<p>
 *
 * In your constructor, be sure to set the value of mDeviceOptions
 * by OR`ing any or all of the INTERCEPT_XXXXXX constants.  Note
 * that the keyboard will *always* be included when the time comes
 * to actually hook the devices.<p>
 *
 * To actually handle the device events, override one or all of the
 * handleXXXXXXChange() members for your desired device.  If you want
 * to know which device are available, you can just simply check one
 * of the members: mKeyboard, mMouse, mGamePad.<p>
 *
 * If, for some reason, you want to intercept windows events, override
 * the member function appHandler().  It is the one that is called
 * by the message pump wrapper.<p>
 *
 * For additional initialization code, override startup(), be sure to 
 * call the version here.  The same is true for the shutdown() 
 * member when you want additional cleanup.  Note that when overriding
 * startup(), call this version *first*, and for shutdown(), call this
 * *version* last!
 *
 * Although the member perform() is virtual, it would be better to 
 * override frame() instead when processing your render cycles.  
 * beforeFrame() and afterFrame() can also be overriden if you wish
 * to separate your periodic updates into distinct segments.<p>
 *
 * IMPORTANT: handleXXXXXChange() will be invoked prior to any of the
 * frame() members.  Check this version's perform() if you need
 * more details.<p>
 *
 * If you specify for the gamepad and the mouse to be hooked, either
 * the buttons or the axes, ALL available buttons/axes will be hooked.
 * The keyboard only intercepts the ESC key and interprets this is
 * a user-requested shutdown.  To hook additional keys, call 
 * interceptKey() with the keycode of your choice, or the constant
 * gjlib::gamelib::ButtonMonitor::ALL_BUTTONS to hook all keys.
 *
 */
class BasicFramework : public DeviceSink
{
public:
  BasicFramework();
  virtual ~BasicFramework();

  // member function that traps windows events. this is called
  // by the windows message pump, though in a roundabout way
  virtual void appHandler(NativeApplicationEvent event, NATIVE_WINDOW_HANDLE hWindow, void* appMessage);

  // call this to begin execution
  bool execute(HINSTANCE instance, TCHAR* winClass, TCHAR* winTitle,
    const bool fullScreen = false, const int startWidth = 800, 
    const int startHeight = 600);

  static const int INTERCEPT_KEYBOARD = 0x0001;
  static const int INTERCEPT_MOUSE_BUTTONS = 0x0010;
  static const int INTERCEPT_MOUSE_MOVEMENTS = 0x0020;
  static const int INTERCEPT_MOUSE = INTERCEPT_MOUSE_BUTTONS | INTERCEPT_MOUSE_MOVEMENTS;
  static const int INTERCEPT_GAMEPAD_BUTTONS = 0x0100;
  static const int INTERCEPT_GAMEPAD_MOVEMENTS = 0x0200;
  static const int INTERCEPT_GAMEPAD = INTERCEPT_GAMEPAD_BUTTONS | INTERCEPT_GAMEPAD_MOVEMENTS;

  // getters
  NativeApplication* getApplication() const { return mApplication; }; 
  InputDeviceManager* getDeviceManager() const { return mDeviceManager; };
  HINSTANCE getAppInstance() const { return mAppInstance; };
  SoundManager* getSoundManager() const { return mSoundManager; };

protected:
  // grant the binding classes friend access so that they can call 
  // framework members
  friend class KeyboardListener;
  friend class MouseButtonListener;
  friend class MouseMovementListener;
  friend class GamePadButtonListener;
  friend class GamePadMovementListener;

  // the device pointers, for convenience
  InputDevice* mKeyboard;
  InputDevice* mMouse;
  InputDevice* mGamePad;

  // the device listeners
  KeyboardListener* mKeyboardListener;
  MouseButtonListener* mMouseButtonListener;
  MouseMovementListener* mMouseMovementListener;
  GamePadButtonListener* mGamePadButtonListener;
  GamePadMovementListener* mGamePadMovementListener;

  // keep track of FPS
  int mFPS;
  bool mShowFPS;
  int mFramesSoFar;
  GlobalTimer* mGTimer;
  SimpleTimer mFPSTracker;

  // device intercept options
  int mDeviceOptions;

  // framework member functions for handling device happenings
  virtual void handleKeyboardChange(int buttonCode, bool pressed);

  // helper for hooking keyboard keys.  if you want to hook ALL
  // button changes, use the constant:
  //    gjlib::gamelib::ButtonMonitor::ALL_BUTTONS
  void interceptKey(const int keyCode);

  // members for initializing and shutting down the system
  virtual bool startup();
  virtual void shutdown();
  virtual void perform();
  virtual void fpsUpdated() {};

  // members called periodically as the system runs
  virtual bool beforeFrame(const double timeSinceLast) { return true; };
  virtual bool frame(const double timeSinceLast) = 0;
  virtual bool afterFrame(const double timeSinceLast) { return true; };

private:

  class Redirector
  {
  public:
    Redirector(BasicFramework& owner) : mOwner(owner) {};
    void handler(NativeApplicationEvent event, NATIVE_WINDOW_HANDLE hWindow, void* appMessage)
    { mOwner.appHandler(event, hWindow, appMessage); };
  private:
    BasicFramework& mOwner;
  };

  bool mAlreadyStartedUp;
  NativeApplication* mApplication;
  InputDeviceManager* mDeviceManager;
  HINSTANCE mAppInstance;
  SoundManager* mSoundManager;
  Redirector* mRedirector;

  void staticAppHandler(NativeApplicationEvent event, NATIVE_WINDOW_HANDLE hWindow, void* appMessage);
};

} /* namespace yaglib */

#endif /* GJ_BASE_FRAMEWORK_HEADER */
