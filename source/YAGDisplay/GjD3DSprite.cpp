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

#include "GjD3DSprite.h"
using namespace yaglib;

const DWORD VERTEX_FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

struct vertex
{
  float x, y, z;
  D3DCOLOR color;
  float tu, tv;
  vertex(float _x, float _y, float _z, float _tu, float _tv, D3DCOLOR _color) :
    x(_x), y(_y), z(_z), tu(_tu), tv(_tv), color(_color)
  {};
};

D3DDrawable::D3DDrawable(D3DScreen* screen) : mScreen(screen), mVb(NULL), mVbSize(0),
  mDevice(screen->getDevice()), mDisplaySize(screen->getSize()), mD3DT(NULL)
{ 
  mDevice->AddRef();
  D3DXMatrixIdentity(&mMxIdentity);
}

D3DDrawable::~D3DDrawable()
{
  SAFE_RELEASE(mVb);
  SAFE_RELEASE(mDevice);
}

void D3DDrawable::allocateVertexBuffer(const int howManyVertices)
{
  // if requesting 0 vertices, we'll just cleanup and go
  if(howManyVertices == 0)
  {
    SAFE_RELEASE(mVb);
    mVbSize = 0;
    return;
  }
  // if our current vb can accomodate the desired # of verts, don't reallocate
  if((mVbSize >= howManyVertices) && (mVbSize < (howManyVertices+6)))
    return;
  // otherwise, we'll be reallocating it here...
  SAFE_RELEASE(mVb);

#ifdef USE_DIRECTX_9 
  HRESULT hr = mDevice->CreateVertexBuffer(howManyVertices * sizeof(vertex),
    D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, VERTEX_FVF, D3DPOOL_DEFAULT, &mVb, NULL);
#else 
  HRESULT hr = mDevice->CreateVertexBuffer(howManyVertices * sizeof(vertex),
    D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, VERTEX_FVF, D3DPOOL_DEFAULT, &mVb);
#endif

  if(SUCCEEDED(hr))
    mVbSize = howManyVertices;
}

void D3DDrawable::setStates()
{
  mScreen->SetTexture(0, mD3DT);
#ifdef USE_DIRECTX_9
	mDevice->SetStreamSource(0, mVb, 0, sizeof(vertex));
	mDevice->SetFVF(VERTEX_FVF);
	mDevice->SetVertexShader(0);
	mDevice->SetPixelShader(0);
#else	
	mDevice->SetStreamSource(0, mVb, sizeof(vertex));
	mDevice->SetVertexShader(VERTEX_FVF);
#endif

	// set device states
	mScreen->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	//mScreen->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
  //mScreen->SetRenderState(D3DRS_LIGHTING, TRUE);
  mScreen->SetRenderState(D3DRS_LIGHTING, FALSE);
  //mScreen->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
  mScreen->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	mScreen->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
  //mScreen->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//mScreen->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	mScreen->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
  mScreen->SetRenderState(D3DRS_ALPHAREF, 0x0);
  mScreen->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	//mScreen->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	mScreen->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	mScreen->SetRenderState(D3DRS_FOGENABLE, FALSE);
  
	// setup scene alpha blending
	mScreen->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	mScreen->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	mScreen->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// setup texture addressing settings
#ifdef USE_DIRECTX_9
	mScreen->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	mScreen->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
#else  	
	mScreen->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	mScreen->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
#endif

	// setup colour calculations
	mScreen->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
  //mScreen->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	mScreen->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	mScreen->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	// setup alpha calculations
	mScreen->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
  //mScreen->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	mScreen->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	mScreen->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

  // disable texture stages we do not need.
  mScreen->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	// setup filtering
#ifdef USE_DIRECTX_9
	mScreen->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	mScreen->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//mScreen->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	//mScreen->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
#else
	mScreen->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	mScreen->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	//mScreen->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_NONE);
	//mScreen->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_NONE);
#endif	
}

void D3DDrawable::d3d_draw(GJPOINT3 position)
{
  setStates();
  beforeDraw();

  D3DXMATRIX mxTranslation;
  D3DXMatrixTranslation(&mxTranslation, position.x, -position.y, position.z);
  mDevice->SetTransform(D3DTS_WORLD, &mxTranslation);

  mDevice->DrawPrimitive(mType, 0, mCount);
  mDevice->SetTransform(D3DTS_WORLD, &mMxIdentity);
}

D3DSprite::D3DSprite(D3DScreen* screen, const WideString spriteName) : 
  Sprite(spriteName), D3DDrawable(screen)
{
  allocateVertexBuffer(4); // just one quad
  mType = D3DPT_TRIANGLESTRIP;
  mCount = 2;

  mD3DT = (D3DTEXTURE) mTexture->getTextureData();
  prepareVertexBuffer();
}

void D3DSprite::prepareVertexBuffer()
{
  vertex* data = NULL;
  if(FAILED(mVb->Lock(0, 0, LOCKDATA(data), D3DLOCK_DISCARD)))
    throw std::exception("failed to allocate vertex buffer");

  // points:
  // 0 -left/top, 1-right/top, 2-left/bottom, 4-right/bottom
  data[0] = vertex(0.0f, mDisplaySize.height, 0.0f, mTexelRect.left, mTexelRect.top, mColor);
  data[1] = vertex(mSize.width, mDisplaySize.height, 0.0f, mTexelRect.right, mTexelRect.top, mColor);
  data[2] = vertex(0.0f, mDisplaySize.height - mSize.height, 0.0f, mTexelRect.left, mTexelRect.bottom, mColor);
  data[3] = vertex(mSize.width, mDisplaySize.height - mSize.height, 0.0f, mTexelRect.right, mTexelRect.bottom, mColor);

  mVb->Unlock();
}

void D3DSprite::frameChanged()
{
  Sprite::frameChanged();
  prepareVertexBuffer();
}

D3DMultiBlitSprite::D3DMultiBlitSprite(D3DScreen* screen, const WideString spriteName) :
  MultiBlitSprite(spriteName), D3DDrawable(screen)
{
  mType = D3DPT_TRIANGLELIST;
  mD3DT = (D3DTEXTURE) mTexture->getTextureData();
}

void D3DMultiBlitSprite::prepareVertexBuffer()
{
  if(mVb == NULL)
    return;

  vertex* data = NULL;
  if(FAILED(mVb->Lock(0, 0, LOCKDATA(data), D3DLOCK_DISCARD)))
    throw std::exception("failed to allocate vertex buffer");

  for(BlitList::iterator iter = mBlitList.begin(); iter != mBlitList.end(); iter++)
  {
    // check the frame number
    if((iter->frameIndex < mInfo->firstFrame) || (iter->frameIndex > mInfo->lastFrame))
      continue;
    //
    GJRECT tr = mTextureInfo->byTexels[iter->frameIndex];
    GJFLOAT left = iter->frameBounds.left, right = iter->frameBounds.right,   
            top = mDisplaySize.height - iter->frameBounds.top,
            bottom = mDisplaySize.height - iter->frameBounds.bottom,
            tLeft = tr.left, tTop = tr.top, tRight = tr.right, tBottom = tr.bottom;

    data[0] = vertex(left, top, 0.0f, tLeft, tTop, iter->color);
    data[1] = vertex(right, top, 0.0f, tRight, tTop, iter->color);
    data[2] = vertex(left, bottom, 0.0f, tLeft, tBottom, iter->color);
    //
    data[3] = vertex(left, bottom, 0.0f, tLeft, tBottom, iter->color);
    data[4] = vertex(right, top, 0.0f, tRight, tTop, iter->color);
    data[5] = vertex(right, bottom, 0.0f, tRight, tBottom, iter->color);
    //
    data += 6;
  }

  mVb->Unlock();
}

void D3DMultiBlitSprite::blitListUpdated()
{
  int count = static_cast<int>(mBlitList.size());
  allocateVertexBuffer(count * 6); // six vertices per quad of character
  mCount = count * 2;
  if(count)
    prepareVertexBuffer();
}

D3DFontSprite::D3DFontSprite(D3DScreen* screen, FontItem* fItem) : 
  FontSprite(fItem), D3DDrawable(screen)
{
  mType = D3DPT_TRIANGLELIST;
  mD3DT = (D3DTEXTURE) mTexture->getTextureData();
}

void D3DFontSprite::prepareVertexBuffer()
{
  if(mVb == NULL)
    return;

  vertex* data = NULL;
  if(FAILED(mVb->Lock(0, 0, LOCKDATA(data), D3DLOCK_DISCARD)))
    throw std::exception("failed to allocate vertex buffer");

  for(UINT i = 0; i < mCount/2; i++)
  {
    BMDrawItem di = mTextData[i];

    GJFLOAT left = di.rScreen.left, right = di.rScreen.right,
        top = mDisplaySize.height - di.rScreen.top,
        bottom = mDisplaySize.height - di.rScreen.bottom;
    GJFLOAT tLeft = di.rTexture.left, tTop = di.rTexture.top,
        tRight = di.rTexture.right, tBottom = di.rTexture.bottom;

    data[0] = vertex(left, top, 0.0f, tLeft, tTop, mColor);
    data[1] = vertex(right, top, 0.0f, tRight, tTop, mColor);
    data[2] = vertex(left, bottom, 0.0f, tLeft, tBottom, mColor);
    //
    data[3] = vertex(left, bottom, 0.0f, tLeft, tBottom, mColor);
    data[4] = vertex(right, top, 0.0f, tRight, tTop, mColor);
    data[5] = vertex(right, bottom, 0.0f, tRight, tBottom, mColor);
    //
    data += 6;
  }

  mVb->Unlock();
}

void D3DFontSprite::textChanged()
{
  FontSprite::textChanged();
  int charCount = static_cast<int>(mText.size());
  allocateVertexBuffer(charCount * 6); // six vertices per quad of character
  mCount = charCount * 2;              // two triangles per character
  if(charCount)
    prepareVertexBuffer();
}

void D3DFontSprite::textOptionsChanged()
{
  textChanged();
}

