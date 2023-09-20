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
 * @file  GjSound.h
 * @brief Sound manager
 *
 * Essentially a wrapper for freesl
 *
 */

#ifndef GJ_SOUND_HEADER
#define GJ_SOUND_HEADER

#include "GjDefs.h"
#include "GjSettings.h"
#include "FreeSL.h"

namespace yaglib
{

#define SOUND_SECTION L"sounds"

class SoundItem
{
public:
  SoundItem() : mName(L""), mFileName(L"") {};
  SoundItem(WideString name, WideString fileName);
  virtual ~SoundItem();

  bool load();
  void unload();

  bool isValid() const;
  bool isLoaded() const;

  void play(int repeatCount = 0);
  void stop();
  bool isPlaying();
  
  WideString const& getName() const;
  WideString const& getFileName() const;

  const bool operator==(const WideString& name)
  {
    return mName == name;
  };

private:
  WideString mName;
  WideString mFileName;
  FSLsound mData;
  bool mValid;
  bool mLoaded;
};

class SoundManager 
{
public:
  SoundManager();
  virtual ~SoundManager();

  bool initialize(HWND hWnd, MultipleSettings& settings);
  void shutdown();
  bool isInitialized();

  bool add(WideString name, WideString fileName);
  void remove(WideString name);
  bool exists(WideString name);
  SoundItem* operator[](WideString name);
  void clear();

  void quickPlay(WideString name);
  void quickLoadAndPlay(WideString name, WideString fileName);

private:
  bool mInitialized;
  typedef std::vector<SoundItem> SoundItems;
  SoundItems mItems;
};

} /* namespace yaglib */


#endif /* GJ_SOUND_HEADER */
