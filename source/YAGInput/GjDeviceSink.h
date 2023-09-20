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
 * @file  GjDeviceSink.h
 * @brief Class to hook devices
 *
 * Doesn't actually do a lot but provide a convenient starting
 * class that 'knows' how to hook the existing devices
 *
 */

#ifndef GJ_DEVICE_SINK_HEADER
#define GJ_DEVICE_SINK_HEADER

#include "GjDefs.h"
#include "GjInputUtils.h"
#include "GjInputDevice.h"

namespace yaglib
{

/**
 * @brief an interface for a device sink.  
 * 
 * The device listeners binds onto a device sink in order to dispense
 * the device events that have happened.<p>
 */
class DeviceSink
{
public:
  virtual void handleKeyboardChange(int buttonCode, bool pressed);
  virtual void handleMouseButtonChange(int buttonCode, bool pressed);
  virtual void handleMouseMovements(int axisNumber);
  virtual void handleGamePadButtonChange(int buttonCode, bool pressed);
  virtual void handleGamePadMovements(int axisNumber);
};

/**
 * @brief a class to bind device listeners to the device sink
 *
 * The member mSink is protected so that subclasses can access them 
 * conveniently.  Since a number of subclasses were made, this is
 * sort of a container and initializer for the member mSink.<p>
 */
class DeviceBinding
{
public:
  DeviceBinding(DeviceSink* host) : mSink(host) {};
  virtual ~DeviceBinding() {};
protected:
  DeviceSink* mSink;
};

/**
 * @brief binds the framework to the keyboard device
 */
class KeyboardListener : public ButtonObserver, private DeviceBinding
{
public:
  KeyboardListener(DeviceSink* host) : DeviceBinding(host) {};
  virtual ~KeyboardListener() {};
  virtual void update(ButtonMonitor& subject, const int buttonCode);
};

/**
 * @brief binds the framework to the Mouse buttons. 
 */ 
class MouseButtonListener : public ButtonObserver, private DeviceBinding
{
public:
  MouseButtonListener(DeviceSink* host) : DeviceBinding(host) {};
  virtual ~MouseButtonListener() {};
  virtual void update(ButtonMonitor& subject, const int buttonCode);
};

/**
 * @brief binds the framework to the Mouse movements.
 */ 
class MouseMovementListener : public AxesObserver, private DeviceBinding
{
public:
  MouseMovementListener(DeviceSink* host) : DeviceBinding(host) {};
  virtual ~MouseMovementListener() {};
  virtual void update(AxesMonitor& subject, const int axisNumber);
};

/**
 * @brief binds the framework to the GamePad buttons. 
 */ 
class GamePadButtonListener : public ButtonObserver, private DeviceBinding
{
public:
  GamePadButtonListener(DeviceSink* host) : DeviceBinding(host) {};
  virtual ~GamePadButtonListener() {};
  virtual void update(ButtonMonitor& subject, const int buttonCode);
};

/**
 * @brief binds the framework to the GamePad movements.
 */ 
class GamePadMovementListener : public AxesObserver, private DeviceBinding
{
public:
  GamePadMovementListener(DeviceSink* host) : DeviceBinding(host) {};
  virtual ~GamePadMovementListener() {};
  virtual void update(AxesMonitor& subject, const int axisNumber);
};

} /* namespace yaglib */

#endif /* GJ_DEVICE_SINK_HEADER */
