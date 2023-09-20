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
 * @file  GjScrollHelper.h
 * @brief Helper classes for scrolling stuff
 *
 */
#ifndef GJ_SCROLL_HELPER_HEADER
#define GJ_SCROLL_HELPER_HEADER

#include "GjDefs.h"
#include "GjRectangles.h"

namespace yaglib 
{

class LinearVector
{
public:
  LinearVector(const double initial, const double acceleration, const double maxSpeed);

  void start();
  void stop();
  void toggle(const bool enable) { if(enable) start(); else stop(); };

  double update(const double timeSinceLast);          // returns distance covered
  __int64 integralUpdate(const double timeSinceLast); // only whole number distance returned

  bool isRunning() const { return mIsRunning; };

private:
  bool mIsRunning;
  double mCurrentSpeed;
  double mAcceleration;
  double mInitialSpeed;
  double mMaxSpeed;
  double mDistanceCovered;

  void updateSpeed(const double timeSinceLast);
};

typedef enum ScrollDirection
{
  SCROLL_LEFT, SCROLL_RIGHT, SCROLL_UP, SCROLL_DOWN
};

class FourWayScroller
{
public:
  FourWayScroller(const double initial, const double acceleration, const double maxSpeed);

  void change(const ScrollDirection which, const bool enable);
  int update(const double timeSinceLast, RECT& delta);

private:
  LinearVector mLeftScroller;
  LinearVector mRightScroller;
  LinearVector mUpScroller;
  LinearVector mDownScroller;
};

} /* namespace yaglib */

#endif /* GJ_SCROLL_HELPER_HEADER */
