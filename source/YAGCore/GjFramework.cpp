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

#include "GjFramework.h"
#include "GjUnicodeUtils.h"
using namespace yaglib;

////
ExtendedFramework::ExtendedFramework(DisplayDevice* displayDevice) : 
  BasicFramework(), mDisplayDevice(displayDevice), mScreen(NULL),
  mFPSFont(NULL)
{
}

ExtendedFramework::~ExtendedFramework()
{
  SAFE_DELETE(mDisplayDevice);
}

bool ExtendedFramework::startup()
{
  mDisplayDevice->mApplication = getApplication();
  if(!mDisplayDevice->createScreenObject(&mScreen))
    return false;

  if(!BasicFramework::startup())
    return false;

  if(!mDisplayDevice->startup())
    return false;

  POINT p;
  p.x = getApplication()->getWindow()->getWidth();
  p.y = getApplication()->getWindow()->getHeight();

  mScreen->initialize(p.x, p.y, getApplication()->isFullScreen());

  mFPSFont = mScreen->createFontSprite(L"default");
  if(mFPSFont)
    mFPSFont->setPosition(0, 0, -40.0f);

  return true;
}

void ExtendedFramework::shutdown()
{
  SAFE_DELETE(mFPSFont);
  mScreen->shutdown();
  SAFE_DELETE(mScreen);
  BasicFramework::shutdown();
}

void ExtendedFramework::appHandler(
  NativeApplicationEvent event, NATIVE_WINDOW_HANDLE hWindow, void* appMessage)
{
  mDisplayDevice->appHandler(event, hWindow, appMessage);
  return BasicFramework::appHandler(event, hWindow, appMessage);
}

bool ExtendedFramework::beforeFrame(const double timeSinceLast)
{
  if(mScreen)
    return mScreen->_beginDrawing();
  
  return true;
}

bool ExtendedFramework::afterFrame(const double timeSinceLast)
{
  if(mScreen)
    mScreen->_endDrawing();

  return true;
}

void ExtendedFramework::fpsUpdated()
{
  if(mFPSFont)
  {
    char buf[60];
    _snprintf(buf, 60, "%d FPS", mFPS);
    WideString fpsText(from_char_p(buf));
    mFPSFont->setText(fpsText);
  }
}




