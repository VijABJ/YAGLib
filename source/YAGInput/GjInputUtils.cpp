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

#include "GjInputUtils.h"
using namespace yaglib;

AxesMonitor::AxesMonitor(const int numberOfAxes)
{
  AxisInfo a;
  memset(&a, 0, sizeof(a));
  for(int i = 0; i < numberOfAxes; i++)
    mAxes.push_back(a);

  memset(&mBounds, 0, sizeof(mBounds));
  memset(&mClipper, 0, sizeof(mClipper));
}

AxesMonitor::~AxesMonitor()
{
}

void AxesMonitor::setPosition(const Point3& point, const int axisNumber)
{
  if((axisNumber >= 0) && ((size_t)axisNumber < mAxes.size()))
  {
    Point3 _position = mAxes[axisNumber].position;
    if(_position != point)
    {
      mAxes[axisNumber].position = point;
      notify(axisNumber);
    }
  }
}

void AxesMonitor::getPosition(Point3& point, const int axisNumber)
{
  if((axisNumber >= 0) && ((size_t)axisNumber < mAxes.size()))
    point = mAxes[axisNumber].position;
}

void AxesMonitor::setMovement(const Point3& delta, const int axisNumber)
{
  if((axisNumber >= 0) && ((size_t)axisNumber < mAxes.size()))
  {
    mAxes[axisNumber].movement = delta;
    //notify(axisNumber); // not needed since the position change triggers notifies
  }
}

void AxesMonitor::getMovement(Point3& delta, const int axisNumber)
{
  if((axisNumber >= 0) && ((size_t)axisNumber < mAxes.size()))
    delta = mAxes[axisNumber].movement;
}

int AxesMonitor::getNumberOfAxes()
{
  return (int)mAxes.size() + 1;
}

void AxesMonitor::setBounds(const Rect& bounds)
{
  mBounds = bounds;
}

const Rect& AxesMonitor::getBounds() const
{
  return mBounds;
}

void AxesMonitor::setClipper(const Rect& clipper)
{
  mClipper = clipper;
}

const Rect& AxesMonitor::getClipper() const
{
  return mClipper;
}

ButtonMonitor::ButtonMonitor(const int numberOfButtons) :
  mNumberOfButtons(numberOfButtons)
{
}

ButtonMonitor::~ButtonMonitor()
{
}

int ButtonMonitor::getNumberOfButtons() const
{
  return mNumberOfButtons;
}


