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

#include "GjSound.h"
#include "GjTemplates.h"
#include "GjUnicodeUtils.h"
#include "GjResourceManagement.h"
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
using namespace yaglib;

SoundItem::SoundItem(WideString name, WideString fileName) :
  mName(name), mFileName(fileName), mLoaded(false), mValid(true)
{
}

SoundItem::~SoundItem()
{
  unload();
}

bool SoundItem::load()
{
  if(!mLoaded && mValid)
  {
    mData = fslLoadSound(UTF8String(mFileName).c_str());
    mLoaded = mValid = (mData != 0);
  }

  return mLoaded && mValid;
}

void SoundItem::unload()
{
  if(mLoaded) 
  {
    fslSoundStop(mData);
    fslFreeSound(mData);
    mLoaded = false;
  }
}

void SoundItem::play(int repeatCount)
{
  if(!mLoaded && !load()) return;
  if(fslSoundIsPlaying(mData)) fslSoundStop(mData);

  fslSoundPlay(mData);
}

void SoundItem::stop()
{
  if(!mLoaded && !load()) return;
  if(fslSoundIsPlaying(mData)) fslSoundStop(mData);
}

bool SoundItem::isPlaying()
{
  return mLoaded && fslSoundIsPlaying(mData);
}

bool SoundItem::isValid() const
{
  return mValid;
}

bool SoundItem::isLoaded() const
{
  return mLoaded;
}

WideString const& SoundItem::getName() const
{
  return mName;
}

WideString const& SoundItem::getFileName() const
{
  return mFileName;
}

//
SoundManager::SoundManager() : mInitialized(false)
{
}

SoundManager::~SoundManager()
{
  shutdown();
}

bool SoundManager::initialize(HWND hWnd, MultipleSettings& settings)
{
  if(!mInitialized)
  {
    mInitialized = fslInit(FSL_SS_ALUT);
    if(mInitialized)
    {
      // well, there could be some more to be added later...
      fslSetAutoUpdate(true);
      // load the file list
      if(settings.exists(SOUND_SECTION))
      {
        Settings section = settings[SOUND_SECTION];
        for(int i = 0; i < static_cast<int>(section.size()); i++)
        {
          Setting item = section[i];
          add(item.getName(), item.getValue());
        }
      }
    }
  }
  return mInitialized;
}

void SoundManager::shutdown()
{
  if(mInitialized)
  {
    clear();
    fslShutDown();
    mInitialized = false;
  }
}

bool SoundManager::add(WideString name, WideString fileName)
{
  WideString verifiedFileName = g_ResourceManager[fileName];
  if(verifiedFileName.size() == 0)
    return false;

  remove(name); // don't want duplicates, remove the previous
  mItems.push_back(SoundItem(name, verifiedFileName));
  return true;
}

void SoundManager::remove(WideString name)
{
  using namespace boost;
  using namespace boost::lambda;
  std::remove_if(mItems.begin(), mItems.end(), _1 == name);
}

bool SoundManager::exists(WideString name)
{
  using namespace boost;
  using namespace boost::lambda;
  SoundItems::const_iterator iter = std::find_if(mItems.begin(), mItems.end(), _1 == name);
  return iter != mItems.end();
}

SoundItem* SoundManager::operator[](WideString name)
{
  using namespace boost;
  using namespace boost::lambda;
  SoundItems::iterator iter = std::find_if(mItems.begin(), mItems.end(), _1 == name);
  return iter != mItems.end() ? &(*iter) : NULL;
}

void SoundManager::clear()
{
  mItems.clear();
}

void SoundManager::quickPlay(WideString name)
{
  using namespace boost;
  using namespace boost::lambda;
  SoundItems::iterator iter = std::find_if(mItems.begin(), mItems.end(), _1 == name);
  if(iter != mItems.end())
    iter->play();
}

void SoundManager::quickLoadAndPlay(WideString name, WideString fileName)
{
  if(!exists(name))
    add(name, fileName);

  quickPlay(name);
}

bool SoundManager::isInitialized()
{
  return mInitialized;
}

