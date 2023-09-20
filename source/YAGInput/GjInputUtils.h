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
 * @file  GjInputUtils.h
 * @brief Utility classes for input classes
 *
 * Basically observer/subject versions for input devices
 *
 */

#ifndef GJ_INPUT_UTILS_HEADER
#define GJ_INPUT_UTILS_HEADER

#include "GjDefs.h"
#include "GjPoints.h"
#include "GjRectangles.h"
#include "GjTemplates.h"

namespace yaglib
{

class AxesMonitor : public SubjectEx<AxesMonitor, int, -1>
{
public:
  AxesMonitor(const int numberOfAxes = 1);
  virtual ~AxesMonitor();

  static const int ALL_AXES = GLOBAL_ASPECT;
  static const int DEFAULT_AXIS = 0;

  int getNumberOfAxes();
  void setPosition(const Point3& point, const int axisNumber = DEFAULT_AXIS);
  void getPosition(Point3& point, const int axisNumber = DEFAULT_AXIS);
  void setMovement(const Point3& delta, const int axisNumber = DEFAULT_AXIS);
  void getMovement(Point3& delta, const int axisNumber = DEFAULT_AXIS);

  void setBounds(const Rect& bounds);
  const Rect& getBounds() const;
  void setClipper(const Rect& clipper);
  const Rect& getClipper() const;

private:
  typedef struct AxisInfo
  {
    Point3 movement;
    Point3 position;
  } AxisInfo;
  typedef std::vector<AxisInfo> AxesList;
  AxesList mAxes;

  Rect mBounds;
  Rect mClipper;
};

class AxesObserver : public ObserverEx<AxesMonitor, int> 
{
};

class ButtonMonitor : public SubjectEx<ButtonMonitor, int, -1>
{
public:
  ButtonMonitor(const int numberOfButtons);
  virtual ~ButtonMonitor();

  static const int ALL_BUTTONS = GLOBAL_ASPECT;

  int getNumberOfButtons() const;

  virtual bool isPressed(const int buttonCode) const = 0;
  virtual bool isReleased(const int buttonCode) const = 0;

private:
  int mNumberOfButtons;
};

class ButtonObserver : public ObserverEx<ButtonMonitor, int>
{
};

} /* namespace yaglib */

#endif /* GJ_INPUT_UTILS_HEADER */
