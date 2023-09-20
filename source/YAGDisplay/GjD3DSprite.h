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
 * @file  GjD3DSprite.h
 * @brief Direct3D sprite
 *
 */
#ifndef GJ_DIRECT3D_SPRITE_HEADER
#define GJ_DIRECT3D_SPRITE_HEADER

#include "GjDefs.h"
#include "GjSprites.h"
#include "GjDirectX.h"
#include "GjD3DScreen.h"

namespace yaglib 
{

class D3DDrawable
{
public:
  D3DDrawable(D3DScreen* screen);
  virtual ~D3DDrawable();

protected:
  D3DScreen* mScreen;
  D3DDEVICE mDevice;
  D3DVERTEXBUFFER mVb;
  int mVbSize;
  D3DXMATRIX mMxIdentity;
  D3DTEXTURE mD3DT;
  GJSIZE mDisplaySize;
  // for the draw primitive
  D3DPRIMITIVETYPE mType;
  UINT mCount;

  void d3d_draw(GJPOINT3 position);
  void setStates();
  void allocateVertexBuffer(const int howManyVertices);
  virtual void prepareVertexBuffer() = 0;
  virtual void beforeDraw() {};
};

class D3DSprite : public Sprite, public D3DDrawable
{
public:
  D3DSprite(D3DScreen* screen, const WideString spriteName);
  virtual ~D3DSprite() {};

  virtual void draw() { d3d_draw(mPosition); };

protected:
  virtual void prepareVertexBuffer();
  virtual void frameChanged();
  virtual void sizeChanged() { prepareVertexBuffer(); };
  virtual void colorChanged() { prepareVertexBuffer(); };
};

class D3DMultiBlitSprite : public MultiBlitSprite, public D3DDrawable
{
public:
  D3DMultiBlitSprite(D3DScreen* screen, const WideString spriteName);
  virtual ~D3DMultiBlitSprite() {};

  virtual void draw() { d3d_draw(mPosition); };

protected:
  virtual void prepareVertexBuffer();
  virtual void blitListUpdated();
};

class D3DFontSprite : public FontSprite, public D3DDrawable
{
public:
  D3DFontSprite(D3DScreen* screen, FontItem* fItem);
  virtual ~D3DFontSprite() {};

  virtual void draw() { if(mVb) d3d_draw(mPosition); };

protected:
  virtual void prepareVertexBuffer();
  virtual void sizeChanged() { prepareVertexBuffer(); };
  virtual void colorChanged() { prepareVertexBuffer(); };
  virtual void textChanged();
  virtual void textOptionsChanged();
};

} /* namespace yaglib */

#endif /* GJ_DIRECT3D_SPRITE_HEADER */
