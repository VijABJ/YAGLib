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
 * @file  GjTimers.h
 * @brief Timer classes
 *
 * Timer classes and related sundries.
 *
 */

#ifndef GJ_TIME_KEEPER_HEADER
#define GJ_TIME_KEEPER_HEADER

#include "GjDefs.h"
#include "GjTemplates.h"

namespace yaglib
{

class SimpleTimer
{
public:
  SimpleTimer(const double timeDelay);
  virtual ~SimpleTimer();

  /** 
   * returns true if the timer fired
   */
  bool update(const double timeSinceLast);
  void changeDelayTo(const double timeDelay);

private:
  double mTimeDelay;
  double mAccumulator;
};

class TimerList : public SubjectWithIds<TimerList>
{
public:
  TimerList();
  virtual ~TimerList();

  // internal class
  class Timer : public SimpleTimer
  {
    friend class TimerList;
  public:
    Timer();
    Timer(const double timeDelay, const int id, const bool isOneShot = false);
    virtual ~Timer();

  private:
    bool mOneShotOnly;
    int mId;
  };

  void change(const int id, const double timeDelay);
  void add(const int id, const double timeDelay, const bool isOneShot = false);
  void remove(const int id);
  void clear();

  bool exists(const int id);
  void update(const double timeSinceLast);

private:
  typedef std::map<int, Timer> ItemList;
  ItemList mItems;

  ItemList::iterator find(const int id);
};

typedef ObserverWithIds<TimerList> TimerListObserver;

class GlobalTimer : public Singleton<GlobalTimer>
{
public:
  GlobalTimer();

  void reset();
  void start();
  void stop();
  double getAbsoluteTime();
  double getTime();
  double getElapsedTime();
  void getValues(double& time, double& absTime, double& elapsedTime);
  bool isRunning();

  void limitThreadAffinity();

private:
  LARGE_INTEGER getAdjustedTime();

  bool mIsRunning;
  LONGLONG mTicksPerSec;
  LONGLONG mStopTime;
  LONGLONG mLastElapsedTime;
  LONGLONG mBaseTime;
};


}; /* namespace yaglib */

#endif /* GJ_TIME_KEEPER_HEADER */
