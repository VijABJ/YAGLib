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
 * @file  GjD3DTextures.h
 * @brief Direct3D textures
 *
 */
#ifndef GJ_DIRECT3D_TEXTURES_HEADER
#define GJ_DIRECT3D_TEXTURES_HEADER

#include "GjDefs.h"
#include "GjTextures.h"
#include "GjTemplates.h"
#include "GjDirectX.h"

namespace yaglib 
{

class D3DTextureLoader : public TextureLoader
{
public:
  D3DTextureLoader(D3DDEVICE device);
  virtual ~D3DTextureLoader();

  virtual Texture* create(const WideString fileName);
  virtual Texture* create(const void* buffer, const size_t bufferSize);
  virtual void destroy(Texture* texture);

  virtual void beforeReset();
  virtual void afterReset();

private:
  D3DDEVICE mDevice;
  typedef ObjectList<Texture> TextureList;
  TextureList mTextures;

  void readInfo(D3DTEXTURE d3dt, D3DXIMAGE_INFO& info, GJSIZE& actual, GJSIZE& orig);
  D3DTEXTURE load(const WideString fileName, D3DXIMAGE_INFO& info);
};

class D3DTexture : public Texture
{
  friend class D3DTextureLoader;
public:
  D3DTexture(TextureLoader* loader, const WideString fileName, D3DTEXTURE d3dt, GJSIZE& actual, GJSIZE& orig);

  virtual DWORD_PTR getTextureData() { return (DWORD_PTR)mTexture; };

private:
  D3DTEXTURE mTexture;
};


} /* namespace yaglib */

#endif /* GJ_DIRECT3D_TEXTURES_HEADER */
