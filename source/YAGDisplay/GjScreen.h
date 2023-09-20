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
 * @file  GjScreen.h
 * @brief Wrapper for the display device
 *
 * This is partially abstract.  Subclasses are required to implement actually
 * changing/manipulating the display device
 *
 */

#ifndef GJ_SCREEN_MANAGER_HEADER
#define GJ_SCREEN_MANAGER_HEADER

#include "GjDefs.h"
#include "GjRectangles.h"
#include "GjBitmapImages.h"
#include "GjClipper.h"
#include "GjBitmapFont.h"
#include "GjTextures.h"
#include "GjMetaData.h"
#include "GjSprites.h"

namespace yaglib
{

class Screen : public SpriteFactory
{
public:
  Screen(HWND windowHandle);

  bool initialize(const int width, const int height, const bool fullScreen = false);
  void shutdown();

  virtual void clear(const ColorQuad& color = 0) = 0;
  virtual bool _beginDrawing(const ColorQuad& background = 0) = 0;
  virtual void _endDrawing() = 0;

  TextureLoader* getTextureLoader() { return mLoader; };
  bool isInitialized() const { return mIsInitialized; };
  GJSIZE getSize() const { return mSize; };

  static GJCLIPPER& getClipper(); 

protected:
  GJRECT mExtents;
  GJSIZE mSize;
  HWND mWindowHandle;
  bool mFullScreen;
  bool mIsInitialized;
  //
  TextureLoader* mLoader;
  MetaDataManager* mMetas;
  TextureManager* mTextureManager;
  FontManager* mFonts;

  virtual bool _initialize() = 0;
  virtual void _shutdown() = 0;
};

#define g_Clipper (Screen::getClipper())

}; /* namespace yaglib */

#endif /* GJ_SCREEN_MANAGER_HEADER */
