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

#include "GjTextures.h"
#include "GjResourceManagement.h"
#include "GjUnicodeUtils.h"
#include "GjIniFiles.h"
#include "GjMetaData.h"
using namespace yaglib;

Texture::Texture(TextureLoader* loader, const WideString fileName) : 
  mLoader(loader), mRefCount(1), mSize(0), mOriginalSize(0), mFileName(fileName) 
{
}

void TextureManager::destroy(Texture* texture)
{
  _release(texture);
};

void TextureManager::destroyAll()
{
  for(TextureList::iterator iter = mTextures.begin(); iter != mTextures.end(); iter++)
  {
    Texture* texture = iter->second;
    iter->second = NULL;
    mLoader->destroy(texture);
  }
  mTextures.clear();
}

Texture* TextureManager::create(const void* buffer, const size_t bufferSize)
{
  if(mLoader)
    return mLoader->create(buffer, bufferSize);
  else
    return NULL;
};

Texture* TextureManager::operator[](const WideString textureName)
{
  if(mLoader)
  {
    TextureList::iterator subj = mTextures.empty() ? mTextures.end() : mTextures.find(textureName);
    if(subj != mTextures.end())
    {
      subj->second->mRefCount++;
      return subj->second;
    }
    //
    TextureMeta const* meta = g_MetaDataManager.getTextureMeta(textureName);
    if(meta)
    {
      Texture* tex = mLoader->create(textureName);//emeta->fileName);
      mTextures[textureName] = tex;
      return tex;
    }
  }

  return NULL;
}

void TextureManager::_release(Texture* texture)
{
  if(mLoader && texture)
  {
    texture->mRefCount--;
    for(TextureList::iterator iter = mTextures.begin(); iter != mTextures.end(); iter++)
    {
      if(iter->second == texture)
      {
        Texture* texture = iter->second;
        iter->second = NULL;
        mLoader->destroy(texture);
        mTextures.erase(iter);
        break;
      }
    }
  }
}

