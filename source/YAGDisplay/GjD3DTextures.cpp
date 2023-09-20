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

#include "GjD3DTextures.h"
#include "GjClipper.h"
#include "GjResourceManagement.h"
#include "GjMetaData.h"
using namespace yaglib;

D3DTextureLoader::D3DTextureLoader(D3DDEVICE device) : TextureLoader(), mDevice(device)
{
  if(device == NULL)
    throw std::exception("Invalid device passed");

  mDevice->AddRef();
}

D3DTextureLoader::~D3DTextureLoader()
{
  mTextures.clear();
  SAFE_RELEASE(mDevice);
}

void D3DTextureLoader::readInfo(D3DTEXTURE d3dt, D3DXIMAGE_INFO& info, GJSIZE& actual, GJSIZE& orig)
{
  orig = GJSIZE(static_cast<GJFLOAT>(info.Width), static_cast<GJFLOAT>(info.Height));

  D3DSURFACE_DESC sd;
  if(SUCCEEDED(d3dt->GetLevelDesc(0, &sd)))
    actual = GJSIZE(static_cast<GJFLOAT>(sd.Width), static_cast<GJFLOAT>(sd.Height));
  else
    actual = orig;
}

D3DTEXTURE D3DTextureLoader::load(const WideString fileName, D3DXIMAGE_INFO& info)
{
  D3DCOLOR colorKey = 0;
  TextureMeta const* tm = g_MetaDataManager.getTextureMeta(fileName);
  if((tm != NULL) && (tm->isTransparent()))
  {
    ColorQuad cq = tm->transparentColor;
    colorKey = (D3DCOLOR)(int)cq;
  }

  DataPack dp;
  if(!g_ResourceManager.lookup(dp, fileName, GROUP_NAME_ANY, false))
    return NULL;

  D3DTEXTURE d3dt;
  HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(mDevice, dp.getData(), static_cast<UINT>(dp.getSize()), 
    D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
    /*D3DX_FILTER_NONE*/D3DX_DEFAULT, D3DX_DEFAULT, colorKey, &info, 0, &d3dt);

  return SUCCEEDED(hr) ? d3dt : NULL;
}

Texture* D3DTextureLoader::create(const WideString fileName)
{
  D3DXIMAGE_INFO info;
  D3DTEXTURE d3dt = load(fileName, info);
  if(!d3dt)
    return NULL;

  GJSIZE actual, orig;
  readInfo(d3dt, info, actual, orig);

  D3DTexture* texture = new D3DTexture(this, fileName, d3dt, actual, orig);
  mTextures.add(texture);
  return texture;
}

Texture* D3DTextureLoader::create(const void* buffer, const size_t bufferSize)
{
  D3DXIMAGE_INFO info;
  D3DTEXTURE d3dt;
  HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(mDevice, buffer, static_cast<UINT>(bufferSize), 
    D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
    D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, 0, &d3dt);

  if(FAILED(hr))
    return NULL;

  GJSIZE actual, orig;
  readInfo(d3dt, info, actual, orig);

  D3DTexture* texture = new D3DTexture(this, L"", d3dt, actual, orig);
  mTextures.add(texture); //?
  return texture;
}

void D3DTextureLoader::destroy(Texture* texture)
{
  if(texture)
  {
    mTextures.remove(texture);
    delete texture;
  }
}

void D3DTextureLoader::beforeReset()
{ 
  for(TextureList::iterator iter = mTextures.begin(); iter != mTextures.end(); iter++)
  {
    D3DTexture* tex = reinterpret_cast<D3DTexture*>(*iter);
    if(tex->isFromFile())
      SAFE_RELEASE(tex->mTexture);
  }
}

void D3DTextureLoader::afterReset()
{
  for(TextureList::iterator iter = mTextures.begin(); iter != mTextures.end(); iter++)
  {
    D3DTexture* tex = reinterpret_cast<D3DTexture*>(*iter);
    D3DXIMAGE_INFO info;
    if(tex->isFromFile())
      tex->mTexture = load(tex->getFileName(), info);
  }
}

///////////////////////

D3DTexture::D3DTexture(TextureLoader* loader, const WideString fileName, D3DTEXTURE d3dt, GJSIZE& actual, GJSIZE& orig) :
  Texture(loader, fileName), mTexture(d3dt)
{
  mSize = actual;
  mOriginalSize = orig;
};

