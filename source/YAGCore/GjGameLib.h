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
 * @file  GjGameLib.h
 * @brief All-encompassing header file
 *
 */

#ifndef GJ_GAME_LIBRARY_HEADER
#define GJ_GAME_LIBRARY_HEADER

// utilities
#include "GjPoints.h"
#include "GjRectangles.h"
#include "GjTimers.h"
#include "GjClipper.h"
#include "GjUnicodeUtils.h"

// base input
#include "GjInputDevice.h"
#include "GjInputUtils.h"
#include "GjKeyCodes.h"

// directinput
#include "GjAxialDeviceDX.h"
#include "GjInputDeviceDX.h"
#include "GjJoystickDeviceDX.h"
#include "GjKeyboardDeviceDX.h"
#include "GjMouseDeviceDX.h"
#include "GjInputManagerDX.h"

// images
#include "GjBitmapImages.h"
#include "GjColors.h"

// sprites
#include "GjTextures.h"
#include "GjSprites.h"
//#include "GjSpriteMover.h"

// base framework
#include "GjBitmapFont.h"
#include "GjScreen.h"
#include "GjSound.h"
#include "GjDeviceSink.h"
#include "GjBaseFramework.h"
#include "GjFramework.h"
#include "GjResourceManagement.h"

#include "GjD3DDisplayDevice.h"

// directgraphics framework
#include "GjD3DTextures.h"
#include "GjD3DSprite.h"
#include "GjD3DScreen.h"

// windowing system
#include "GjEvents.h"
#include "GjBaseView.h"
#include "GjPanels.h"
#include "GjButtons.h"
#include "GjEventFrameworkBase.h"

// map engine
#include "GjScrollHelper.h"
#include "GjTileMap.h"
#include "GjMapView.h"

// library includes
#pragma comment(lib, "YAGSupport.lib")
#pragma comment(lib, "YAGDisplay.lib")
#pragma comment(lib, "YAGInput.lib")
#pragma comment(lib, "YAGCore.lib")
#pragma comment(lib, "freesl.lib")
// direct x libraries
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "ddraw.lib")
#ifdef USE_DIRECTX_9
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")
#else
#pragma comment(lib, "d3dx8.lib")
#pragma comment(lib, "d3d8.lib")
#endif


#endif /* GJ_GAME_LIBRARY_HEADER */
