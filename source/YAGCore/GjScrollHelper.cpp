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

#include "GjScrollHelper.h"
#include <cmath>
using namespace yaglib;

LinearVector::LinearVector(const double initial, const double acceleration, 
  const double maxSpeed) : mIsRunning(false), mCurrentSpeed(0.0f), mAcceleration(acceleration),
  mInitialSpeed(initial), mMaxSpeed(maxSpeed), mDistanceCovered(0.0f)
{
}

void LinearVector::start()
{
  if(!mIsRunning)
  {
    mIsRunning = true;
    mCurrentSpeed = mInitialSpeed;
  }
}

void LinearVector::stop()
{
  if(mIsRunning)
  {
    mIsRunning = false;
    mDistanceCovered = 0.0f;
  }
}

void LinearVector::updateSpeed(const double timeSinceLast)
{
  mCurrentSpeed *= mAcceleration;
  if(mCurrentSpeed > mMaxSpeed)
    mCurrentSpeed = mMaxSpeed;
}

double LinearVector::update(const double timeSinceLast)
{
  double delta = mDistanceCovered;
  if(mIsRunning)
  {
    delta += mCurrentSpeed * timeSinceLast;
    updateSpeed(timeSinceLast);
  }
  return delta;
}

__int64 LinearVector::integralUpdate(const double timeSinceLast)
{
  __int64 delta = 0;
  if(mIsRunning)
  {
    mDistanceCovered += (mCurrentSpeed * timeSinceLast);
    if(mDistanceCovered >= 1.0)
    {
      delta = static_cast<__int64>(floor(mDistanceCovered));
      mDistanceCovered -= static_cast<double>(delta);
    }
  }
  return delta;
}

FourWayScroller::FourWayScroller(const double initial, const double acceleration, const double maxSpeed) :
  mLeftScroller(LinearVector(initial, acceleration, maxSpeed)),
  mRightScroller(LinearVector(initial, acceleration, maxSpeed)),
  mUpScroller(LinearVector(initial, acceleration, maxSpeed)),
  mDownScroller(LinearVector(initial, acceleration, maxSpeed))
{
}

void FourWayScroller::change(const ScrollDirection which, const bool enable)
{
  switch(which)
  {
  case SCROLL_LEFT:
    mLeftScroller.toggle(enable);
    break;
  case SCROLL_RIGHT:
    mRightScroller.toggle(enable);
    break;
  case SCROLL_UP:
    mUpScroller.toggle(enable);
    break;
  case SCROLL_DOWN:
    mDownScroller.toggle(enable);
    break;
  }
}

int FourWayScroller::update(const double timeSinceLast, RECT& delta)
{
  int updateCount = 0;
  //
  delta.left = mLeftScroller.isRunning() && !mRightScroller.isRunning() ?
    static_cast<LONG>(mLeftScroller.integralUpdate(timeSinceLast)) : 0;
  delta.right = mRightScroller.isRunning() && !mLeftScroller.isRunning() ?
    static_cast<LONG>(mRightScroller.integralUpdate(timeSinceLast)) : 0;
  delta.top = mUpScroller.isRunning() && !mDownScroller.isRunning() ?
    static_cast<LONG>(mUpScroller.integralUpdate(timeSinceLast)) : 0;
  delta.bottom = mDownScroller.isRunning() && !mUpScroller.isRunning() ?
    static_cast<LONG>(mDownScroller.integralUpdate(timeSinceLast)) : 0;
  //
  if(delta.left > 0) updateCount++;
  if(delta.right > 0) updateCount++;
  if(delta.top > 0) updateCount++;
  if(delta.bottom > 0) updateCount++;
  //
  return updateCount;
}
