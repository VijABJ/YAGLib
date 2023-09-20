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
 * @file  GjTextures.h
 * @brief Basic texture
 *
 */
#ifndef GJ_TEXTURES_HEADER
#define GJ_TEXTURES_HEADER

#include "GjDefs.h"
#include "GjColors.h"
#include "GjPoints.h"
#include "GjRectangles.h"
#include "GjTemplates.h"

namespace yaglib 
{

class Texture;
class TextureLoader;
class TextureManager;

class TextureLoader
{
public:
  virtual Texture* create(const WideString fileName) = 0;
  virtual Texture* create(const void* buffer, const size_t bufferSize) = 0;
  virtual void destroy(Texture* texture) = 0;

  virtual void beforeReset() = 0;
  virtual void afterReset() = 0;
};

class Texture
{
  friend class TextureManager;
public:
  Texture(TextureLoader* loader, const WideString fileName);
  virtual ~Texture() { /*mLoader->destroy(this);*/ };

  TextureLoader* getLoader() { return mLoader; };
  GJSIZE const& getSize() const { return mSize; };
  GJSIZE const& getOriginalSize() const { return mOriginalSize; };
  WideString const& getFileName() const { return mFileName; };
  bool isFromFile() const { return mFileName.size() == 0; };

  virtual DWORD_PTR getTextureData() = 0;

protected:
  TextureLoader* mLoader;
  GJSIZE mSize;
  GJSIZE mOriginalSize;
  WideString mFileName;

private:
  int mRefCount;
};

class TextureManager : public Singleton<TextureManager>
{
public:
  TextureManager(TextureLoader* loader) : Singleton<TextureManager>(), mLoader(loader) {};
  virtual ~TextureManager() { destroyAll(); };

  TextureLoader* getLoader() { return mLoader; };

  void destroy(Texture* texture);
  void destroyAll();

  Texture* create(const void* buffer, const size_t bufferSize);
  Texture* operator[](const WideString textureName);

private:
  TextureLoader* mLoader;
  typedef std::map<WideString, Texture*> TextureList;
  TextureList mTextures;

  Texture* find(const WideString& fileName);
  void _release(Texture* texture);
};

#define g_TextureManager      (TextureManager::Instance())


} /* namespace yaglib */

#endif /* GJ_TEXTURES_HEADER */
