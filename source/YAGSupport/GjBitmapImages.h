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
 * @file  GjBitmapImages.h
 * @brief bitmap images
 *
 */
#ifndef GJ_BITMAP_IMAGES_HEADER
#define GJ_BITMAP_IMAGES_HEADER

#include "GjDefs.h"
#include "GjColors.h"
#include "GjRectangles.h"

namespace yaglib 
{

class BitmapBase
{
public:
  virtual bool isValid() = 0;
  virtual int getWidth() = 0;
  virtual int getHeight() = 0;
  virtual int getPitch() = 0;
  virtual DWORD getDataSize() = 0;

  virtual void* getImagePointer() = 0;
};

class Bitmap24Base : public BitmapBase
{
public:
  virtual ColorTriple* getPixelAt(const int x, const int y) = 0;
  virtual ColorTriple& operator()(const int x, const int y) = 0;
};

class Bitmap32Base : public BitmapBase
{
public:
  virtual ColorQuad* getPixelAt(const int x, const int y) = 0;
  virtual ColorQuad& operator()(const int x, const int y) = 0;
};

class InMemoryWindowsBitmap : public Bitmap24Base
{
public:
  InMemoryWindowsBitmap(WideString fileName);
  InMemoryWindowsBitmap(const int width, const int height);
  virtual ~InMemoryWindowsBitmap();

  bool save(WideString fileName);

  virtual bool isValid();
  virtual int getWidth();
  virtual int getHeight();
  virtual int getPitch();
  virtual DWORD getDataSize();

  virtual void* getImagePointer();
  void* getPixelMapPointer();
  virtual ColorTriple* getPixelAt(const int x, const int y);
  virtual ColorTriple& operator()(const int x, const int y);

private:
  int mWidth;
  int mHeight;
  int mPitch;
  bool mIsValid;
  DWORD mImageSize; // size of the whole file image
  BITMAPFILEHEADER* mFileHeader;
  BITMAPINFOHEADER* mInfoHeader;
  void* mImageData;
  void* mPixelData;

  void loadFromFile(WideString fileName);
  void calculatePitch();
  void deriveOtherPointers();
};

class Bitmap32 : public Bitmap32Base
{
public:
  Bitmap32(const int width = 0, const int height = 0);
  virtual ~Bitmap32();

  bool loadFromFile(WideString fileName, const bool multiFrames = false, 
    const bool transparent = false, const ColorQuad& transparentColor = 0);

  bool save(WideString fileName);
  bool load(WideString fileName);

  void const* getBuffer() const;
  int getSizeInBytes() const;
  int getSizeInPixels() const;

  bool locationValid(const int x, const int y);
  bool validColumn(const int x);
  bool validRow(const int y);
  void put(const int x, const int y, const BYTE8 red, 
    const BYTE8 green, const BYTE8 blue, const BYTE8 alpha = 0xff);

  void setTransparentColor(const BYTE8 red, const BYTE8 green, 
    const BYTE8 blue, const BYTE8 alpha = 0xff);
  void setTransparentColor(const ColorQuad& color);
  void clearTransparentColor();
  ColorQuad& getTransparentColor();
  bool isTransparent();

  virtual bool isValid();
  virtual int getWidth();
  virtual int getHeight();
  virtual int getPitch();
  virtual DWORD getDataSize();
  virtual void* getImagePointer();

  virtual ColorQuad* getPixelAt(const int x, const int y);
  virtual ColorQuad& operator()(const int x, const int y);

protected:
  void reallocateBuffer();

private:
  int mWidth;
  int mHeight;
  ColorQuad* mBuffer;
  int mRowSizeInBytes;
  int mSizeInBytes; // of the buffer
  int mSizeInPixels; // of the virtual image
  bool mIsValid;

  ColorQuad mTransparentColor;
  bool mIsTransparent;

  bool loadFromWindowsBitmapFile(WideString fileName);
};


} /* namespace yaglib */

#endif /* GJ_BITMAP_IMAGES_HEADER */
