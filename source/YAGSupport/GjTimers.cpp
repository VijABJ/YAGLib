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

#include "GjTimers.h"
using namespace yaglib;

SimpleTimer::SimpleTimer(const double timeDelay) : mTimeDelay(timeDelay), mAccumulator(timeDelay) 
{ 
}

SimpleTimer::~SimpleTimer()
{
}

bool SimpleTimer::update(const double timeSinceLast)
{
  mAccumulator -= timeSinceLast;
  bool result = mAccumulator <= 0.0;
  if(result)
    mAccumulator = mTimeDelay;

  return result;
};

void SimpleTimer::changeDelayTo(const double timeDelay)
{
  mAccumulator = mTimeDelay = timeDelay;
}

TimerList::Timer::Timer() :
  SimpleTimer(-1), mId(-1), mOneShotOnly(true)
{
}

TimerList::Timer::Timer(const double timeDelay, const int id, const bool isOneShot) : 
  SimpleTimer(timeDelay), mId(id), mOneShotOnly(isOneShot)
{
}

TimerList::Timer::~Timer()
{
}

TimerList::TimerList() : SubjectWithIds<TimerList>()
{
}

TimerList::~TimerList()
{
}

void TimerList::change(const int id, const double timeDelay)
{
  ItemList::iterator iter = find(id);
  if(iter != mItems.end())
    iter->second.changeDelayTo(timeDelay);
}

void TimerList::add(const int id, const double timeDelay, const bool isOneShot)
{
  ItemList::iterator iter = find(id);
  if(iter != mItems.end())
    iter->second.changeDelayTo(timeDelay);
  else
    mItems[id] = Timer(timeDelay, id, isOneShot);
}

void TimerList::remove(const int id)
{
  ItemList::iterator iter = find(id);
  if(iter != mItems.end())
    mItems.erase(iter);
}

void TimerList::clear()
{
  mItems.clear();
}

bool TimerList::exists(const int id)
{
  return find(id) != mItems.end();
}

TimerList::ItemList::iterator TimerList::find(const int id)
{
  return mItems.empty() ? mItems.end() : mItems.find(id);
}

void TimerList::update(const double timeSinceLast)
{
  int expiredTimers = 0;
  for(ItemList::iterator iter = mItems.begin(); iter != mItems.end(); ++iter)
  {
    if(iter->second.update(timeSinceLast))
    {
      // trigger it, then check if we should remove it due to it being only one-shot
      notify(iter->second.mId);
      if(iter->second.mOneShotOnly)
      {
        iter->second.mId = -1;
        ++expiredTimers;
      }
    }
  }

  // is there anything we should delete
  while(expiredTimers > 0)
  {
    for(ItemList::iterator iter = mItems.begin(); iter != mItems.end(); ++iter)
      if(iter->second.mId == -1)
      {
        mItems.erase(iter);
        --expiredTimers;
        break;
      }
  }
}

GlobalTimer::GlobalTimer() : Singleton<GlobalTimer>(), mIsRunning(false), 
  mTicksPerSec(0), mStopTime(0), mLastElapsedTime(0), mBaseTime(0)
{
  LARGE_INTEGER tps;
  QueryPerformanceFrequency(&tps);
  mTicksPerSec = tps.QuadPart;
}

void GlobalTimer::reset()
{
  LARGE_INTEGER adjTime = getAdjustedTime();
  mBaseTime = mLastElapsedTime = adjTime.QuadPart;
  mStopTime = 0;
  mIsRunning = true;
}

void GlobalTimer::start()
{
  LARGE_INTEGER qpc;
  QueryPerformanceCounter(&qpc);

  if(!mIsRunning)
  {
    mBaseTime += qpc.QuadPart - mStopTime;
    mStopTime = 0;
    mLastElapsedTime = qpc.QuadPart;
    mIsRunning = true;
  }
}

void GlobalTimer::stop()
{
  if(mIsRunning)
  {
    LARGE_INTEGER qpc;
    QueryPerformanceCounter(&qpc);
    mStopTime = mLastElapsedTime = qpc.QuadPart;
    mIsRunning = false;
  }
}

double GlobalTimer::getAbsoluteTime()
{
  LARGE_INTEGER qpc;
  QueryPerformanceCounter(&qpc);

  return qpc.QuadPart / static_cast<double>(mTicksPerSec);
}

double GlobalTimer::getTime()
{
  LARGE_INTEGER adjTime = getAdjustedTime();

  return static_cast<double>(adjTime.QuadPart - mBaseTime) / static_cast<double>(mTicksPerSec);
}

double GlobalTimer::getElapsedTime()
{
  LARGE_INTEGER adjTime = getAdjustedTime();
  double elapsed = static_cast<double>(adjTime.QuadPart - mLastElapsedTime) / static_cast<double>(mTicksPerSec);
  mLastElapsedTime = adjTime.QuadPart;
  if(elapsed < 0.0f)
    elapsed = 0.0f;

  return elapsed;
}

void GlobalTimer::getValues(double& time, double& absTime, double& elapsedTime)
{
  LARGE_INTEGER adjTime = getAdjustedTime();

  elapsedTime = static_cast<double>(adjTime.QuadPart - mLastElapsedTime) / static_cast<double>(mTicksPerSec);
  mLastElapsedTime = adjTime.QuadPart;
  if(elapsedTime < 0.0f)
    elapsedTime = 0.0f;

  time = static_cast<double>(adjTime.QuadPart - mBaseTime) / static_cast<double>(mTicksPerSec);
  absTime = adjTime.QuadPart / static_cast<double>(mTicksPerSec);
}

bool GlobalTimer::isRunning()
{
  return mIsRunning;
}

LARGE_INTEGER GlobalTimer::getAdjustedTime()
{
  LARGE_INTEGER qpc;
  if(mStopTime != 0)
    qpc.QuadPart = mStopTime;
  else
    QueryPerformanceCounter(&qpc);

  return qpc;
}

void GlobalTimer::limitThreadAffinity()
{
  HANDLE hThisProcess = GetCurrentProcess();

  // Get the processor affinity mask for this process
  DWORD_PTR dwProcessAffinityMask = 0;
  DWORD_PTR dwSystemAffinityMask = 0;
  if(GetProcessAffinityMask(hThisProcess, &dwProcessAffinityMask, &dwSystemAffinityMask) != 0 && dwProcessAffinityMask)
  {
    // Find the lowest processor that our process is allows to run against
    DWORD_PTR dwAffinityMask = ( dwProcessAffinityMask & ((~dwProcessAffinityMask) + 1 ) );

    // Set this as the processor that our thread must always run against
    // This must be a subset of the process affinity mask
    HANDLE hCurrentThread = GetCurrentThread();
    if(INVALID_HANDLE_VALUE != hCurrentThread)
    {
      SetThreadAffinityMask(hCurrentThread, dwAffinityMask);
      CloseHandle(hCurrentThread);
    }
  }

  CloseHandle(hThisProcess);
}
