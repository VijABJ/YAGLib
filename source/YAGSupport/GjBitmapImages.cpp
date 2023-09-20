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

#include "GjBitmapImages.h"
#include "GjUnicodeUtils.h"
#include <boost/filesystem/operations.hpp>
#include <iostream>
#include <fstream>
#include <exception>
using namespace yaglib;

static const int WIN_BITMAP_SIGNATURE = 0x4D42;
static const size_t WIN_BITMAP_HEADER_SIZE = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

InMemoryWindowsBitmap::InMemoryWindowsBitmap(WideString fileName) :
  mIsValid(false), mImageSize(0), mFileHeader(NULL), mInfoHeader(NULL),
  mImageData(NULL), mPixelData(NULL), mWidth(-1), mHeight(-1)
{
  // load image file here...
  loadFromFile(fileName);
}

InMemoryWindowsBitmap::InMemoryWindowsBitmap(const int width, const int height) :
  mIsValid(false), mImageSize(0), mFileHeader(NULL), mInfoHeader(NULL),
  mImageData(NULL), mPixelData(NULL), mWidth(-1), mHeight(-1)
{
  // remember the request size
  mWidth = width; mHeight = height;
  calculatePitch();

  // allocate the memory, zero it, and derive the other pointers
  mImageSize = (mPitch * mHeight) + (int)WIN_BITMAP_HEADER_SIZE;
  mImageData = (void*) malloc(mImageSize);
  memset(mImageData, 0, mImageSize);
  deriveOtherPointers();

  // setup the header, we have to!
  mFileHeader->bfType = WIN_BITMAP_SIGNATURE;
  mFileHeader->bfOffBits = (int)WIN_BITMAP_HEADER_SIZE;

  mInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
  mInfoHeader->biCompression = 0;
  mInfoHeader->biPlanes = 1;
  mInfoHeader->biWidth = mWidth;
  mInfoHeader->biHeight = mHeight;
  mInfoHeader->biBitCount = 24;
  mInfoHeader->biSizeImage = mPitch * mHeight;
  mInfoHeader->biXPelsPerMeter = mInfoHeader->biYPelsPerMeter = 0x0B13;

  mIsValid = true;
}

InMemoryWindowsBitmap::~InMemoryWindowsBitmap()
{
  if(mImageData)
    delete mImageData;
}

void InMemoryWindowsBitmap::calculatePitch()
{
  mPitch = mWidth * 3;                  // 3 bytes per pixel! we might want to change this later
  int padding = mPitch % 4;             // see if it's not divisible by 4...
  if(padding) mPitch += (4 - padding);  // round up to next factor of 4 if necessary
}

void InMemoryWindowsBitmap::deriveOtherPointers()
{
  mFileHeader = (BITMAPFILEHEADER*)mImageData;
  mInfoHeader = (BITMAPINFOHEADER*)((char*)mImageData + sizeof(BITMAPFILEHEADER));
  mPixelData = (void*)((char*)mInfoHeader + sizeof(BITMAPINFOHEADER));
}

ColorTriple* InMemoryWindowsBitmap::getPixelAt(const int x, const int y)
{
  if(!mIsValid || (x < 0) || (x > mWidth) || (y < 0) || (y > mHeight))
    return NULL;

  // should extract the proper pixels...
  int row = mHeight - (y+1);
  void* p = (void*) ((long long)mPixelData + (row * mPitch) + (x * 3));
  return (ColorTriple*) p;
}

ColorTriple& InMemoryWindowsBitmap::operator()(const int x, const int y)
{
  assert(mIsValid && (x >= 0) && (x < mWidth) && (y >= 0) && (y < mHeight));

  ColorTriple* ct = (ColorTriple*) getPixelAt(x, y);
  return *ct;
}


void InMemoryWindowsBitmap::loadFromFile(WideString fileName)
{
  if(!boost::filesystem::exists(fileName) || boost::filesystem::is_directory(fileName))
    return;

  std::ifstream source(UTF8String(fileName).c_str(), std::ios::binary);
  if(source.bad())
    return;

  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmih;

  // load the headers from the file
  source.read((char*)&bmfh, static_cast<std::streamsize>(sizeof(bmfh)));
  source.read((char*)&bmih, static_cast<std::streamsize>(sizeof(bmih)));

  // check be sure that this is indeed a valid bitmap file.
  // the second check is for compressed bitmaps, which
  // we don't support. 
  if((bmfh.bfType == WIN_BITMAP_SIGNATURE) && (bmih.biCompression != 0))
    return;

  // get the size of the file
  source.seekg(0, std::ios_base::end);
  size_t size = source.tellg();
  source.seekg(0, std::ios_base::beg);

  // read it in
  mImageData = (void*) malloc(mImageSize);
  source.read((char*)mImageData, static_cast<std::streamsize>(size));
  
  // fix the pointers we'll need...
  deriveOtherPointers();

  // now initialize the member image vars
  mWidth = mInfoHeader->biWidth;
  mHeight = mInfoHeader->biHeight;
  calculatePitch();

  // should be good to go
  mIsValid = true;
}

bool InMemoryWindowsBitmap::save(WideString fileName)
{
  if(!mIsValid) 
    return false;

  FILE* imageFile = fopen(UTF8String(fileName).c_str(), "wb");
  if(imageFile == NULL)
    return false;

  fwrite(mImageData, 1, mImageSize, imageFile);
  fclose(imageFile);

  return true;
}

bool InMemoryWindowsBitmap::isValid()
{
  return mIsValid;
}

int InMemoryWindowsBitmap::getWidth()
{
  return mWidth;
}

int InMemoryWindowsBitmap::getHeight()
{
  return mHeight;
}

int InMemoryWindowsBitmap::getPitch()
{
  return mPitch;
}

DWORD InMemoryWindowsBitmap::getDataSize()
{
  return mImageSize;
}

void* InMemoryWindowsBitmap::getImagePointer()
{
  return mImageData;
}

void* InMemoryWindowsBitmap::getPixelMapPointer()
{
  return mPixelData;
}

// header structure
typedef struct ImageHeader
{
  int signature;
  int dataSize;
  int width;
  int height;
  int sizeInPixels;
  int sizeInBytes;
} ImageHeader;

static const int ZIF_SIGNATURE = 0x0046495A;
static const size_t ZIF_HEADER_SIZE = sizeof(ImageHeader);

// fixed header values
static const ImageHeader validHeader =
{
  ZIF_SIGNATURE, 0, 0, 0, 0, 0  
};

Bitmap32::Bitmap32(const int width, const int height) :
  mWidth(width), mHeight(height), mBuffer(NULL), mIsTransparent(false),
  mSizeInBytes(0), mSizeInPixels(0), mIsValid(false)
{
  reallocateBuffer();
}

Bitmap32::~Bitmap32()
{
  mWidth = mHeight = 0;
  reallocateBuffer();
}

void Bitmap32::reallocateBuffer()
{
  mIsValid = false;

  // deallocate our previous buffer if there was any!
  if(mBuffer)
  {
    delete [] mBuffer;
    mBuffer = NULL;
  }

  // determine how many pixels are in the image
  // if this is zero, no need to continue
  mSizeInPixels = mWidth * mHeight;
  if(mSizeInPixels == 0)
    return;

  // calculate the size, in bytes, of the buffer we need
  mRowSizeInBytes = mWidth * sizeof(ColorQuad);
  mSizeInBytes = mSizeInPixels * sizeof(ColorQuad);
  mBuffer = new ColorQuad [mSizeInPixels];
  memset(mBuffer, 0, mSizeInBytes);

  // we're done!
  mIsValid = true;
}

void Bitmap32::put(const int x, const int y, const BYTE8 red, 
  const BYTE8 green, const BYTE8 blue, const BYTE8 alpha)
{
  ColorQuad* quad = mBuffer + (y * mWidth) + x;
  quad->assign(red, green, blue, alpha);
}

ColorQuad* Bitmap32::getPixelAt(const int x, const int y)
{
  if(!locationValid(x, y))
    return NULL;

  return &((*this)(x, y));
}

ColorQuad& Bitmap32::operator()(const int x, const int y)
{
  assert(locationValid(x, y));

  ColorQuad* quad = mBuffer + (y * mWidth) + x;
  return *quad;
}

bool Bitmap32::loadFromFile(WideString fileName, const bool multiFrames, 
  const bool transparent, const ColorQuad& transparentColor)
{
  if(!loadFromWindowsBitmapFile(fileName))
    return false;

  // if transparent, then set it
  clearTransparentColor();
  if(transparent)
    setTransparentColor(transparentColor);

  return true;
}

bool Bitmap32::loadFromWindowsBitmapFile(WideString fileName)
{
  InMemoryWindowsBitmap iwb(fileName);
  bool result = iwb.isValid();
  if(result)
  {
    mWidth = iwb.getWidth();
    mHeight = iwb.getHeight();
    reallocateBuffer();

    for(int x = 0; x < mWidth; x++)
      for(int y = 0; y < mHeight; y++)
      {
        ColorTriple& ct = iwb(x, y);
        put(x, y, ct.red, ct.green, ct.blue);
      }
  }

  return result;
}

bool Bitmap32::save(WideString fileName)
{
  ImageHeader header;
  memcpy(&header, &validHeader, sizeof(header));
  header.height = mHeight;
  header.width = mWidth;
  header.sizeInBytes = mSizeInBytes;
  header.sizeInPixels = mSizeInPixels;
  header.dataSize = mSizeInBytes;

  std::ofstream dest(UTF8String(fileName).c_str(), std::ios::binary|std::ios::out|std::ios::trunc);
  if(dest.bad())
    return false;

  dest.write((char*)&header, static_cast<std::streamsize>(sizeof(header)));
  dest.write((char*)mBuffer, static_cast<std::streamsize>(mSizeInBytes));
  return true;
}

bool Bitmap32::load(WideString fileName)
{
  if(!boost::filesystem::exists(fileName) || boost::filesystem::is_directory(fileName))
    return false;

  std::ifstream source(UTF8String(fileName).c_str(), std::ios::binary);
  if(source.bad())
    return false;

  ImageHeader header;
  source.read((char*)&header, static_cast<std::streamsize>(sizeof(ImageHeader)));
  if(header.signature != validHeader.signature)
    return false;

  // setup the image properties
  mWidth = header.width;
  mHeight = header.height;
  reallocateBuffer();

  source.read((char*)mBuffer, static_cast<std::streamsize>(mSizeInBytes));
  return true;
}

void Bitmap32::setTransparentColor(const BYTE8 red, const BYTE8 green, 
  const BYTE8 blue, const BYTE8 alpha)
{
  mTransparentColor.assign(red, green, blue, alpha);
  mIsTransparent = true;
}

void Bitmap32::setTransparentColor(const ColorQuad& color)
{
  mTransparentColor = color;
  mIsTransparent = (((int)color) & 0xFFFFFF) != 0;
}

bool Bitmap32::isValid()
{
  return mIsValid;
}

int Bitmap32::getWidth()
{
  return mWidth;
}

int Bitmap32::getHeight()
{
  return mHeight;
}

int Bitmap32::getPitch()
{
  return 0;
}

DWORD Bitmap32::getDataSize()
{
  return mWidth * mHeight * sizeof(ColorQuad);
}

void* Bitmap32::getImagePointer()
{
  return NULL;
}

void const* Bitmap32::getBuffer() const
{
  return (void*)mBuffer;
}

int Bitmap32::getSizeInBytes() const
{
  return mSizeInBytes;
}

int Bitmap32::getSizeInPixels() const
{
  return mSizeInPixels;
}

bool Bitmap32::locationValid(const int x, const int y)
{
  return (x >= 0) && (x < mWidth) && (y >= 0) && (y < mHeight);
}

bool Bitmap32::validColumn(const int x)
{
  return (x >= 0) && (x < mWidth);
}

bool Bitmap32::validRow(const int y)
{
  return (y >= 0) && (y < mHeight);
}

void Bitmap32::clearTransparentColor()
{
  mIsTransparent = false;
}

ColorQuad& Bitmap32::getTransparentColor()
{
  return mTransparentColor;
}

bool Bitmap32::isTransparent()
{
  return mIsTransparent;
}

