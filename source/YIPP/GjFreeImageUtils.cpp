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

#include "GjFreeImageUtils.h"
#include "GjUnicodeUtils.h"
using namespace yaglib;

FIBITMAP* LoadImageFile(WideString wsFileName, int flag)
{
  UTF8String utf(wsFileName);
  const char* fileName = utf.c_str();

  FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(fileName, 0);
  if(fif == FIF_UNKNOWN)
    fif = FreeImage_GetFIFFromFilename(fileName);

  FIBITMAP* dib = NULL;
  if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) 
  {
    FIBITMAP* image = FreeImage_Load(fif, fileName, flag);
    if(image != NULL)
    {
      dib = FreeImage_ConvertTo32Bits(image);
      FreeImage_Unload(image);
    }
  }

  return dib;
}

void GetColorAt(FIBITMAP* dib, const Point& location, ColorQuad& color)
{
  int width = FreeImage_GetWidth(dib), height = FreeImage_GetHeight(dib);
  int bytespp = FreeImage_GetLine(dib) / width;

  Point p = location;
  // FreeImage has inverted scanline, that is, the first line is actually the last!
  p.y = (height - 1) - p.y;

  // we need to read ONE pixel data! how hard can it be?
  BYTE* bits = FreeImage_GetScanLine(dib, p.y);
  bits += (bytespp * p.x);
  color = ColorQuad(bits[FI_RGBA_RED], bits[FI_RGBA_GREEN], bits[FI_RGBA_BLUE], bits[FI_RGBA_ALPHA]);
}

void GetColorAt(WideString wsFileName, const Point& location, ColorQuad& color)
{
  FIBITMAP* dib = LoadImageFile(wsFileName, 0);
  GetColorAt(dib, location, color);
  FreeImage_Unload(dib);
}

void GetImageDimensions(WideString wsFileName, Size& size)
{
  FIBITMAP* dib = LoadImageFile(wsFileName, 0);
  size.width = FreeImage_GetWidth(dib);
  size.height = FreeImage_GetHeight(dib);
  FreeImage_Unload(dib);
}

void ConvertTransparencyToAlpha(FIBITMAP* dib, const yaglib::ColorQuad tColor)
{
  int width = FreeImage_GetWidth(dib);
  int height = FreeImage_GetHeight(dib);
  int bytespp = FreeImage_GetLine(dib) / width;

  int adjHeight = height-1;
  for(int y = adjHeight; y >= 0; y--)
  {
    BYTE* bits = FreeImage_GetScanLine(dib, y);
    for(int x = 0; x < width; x++)
    {
      ColorQuad thisColor = ColorQuad(bits[FI_RGBA_RED], bits[FI_RGBA_GREEN], bits[FI_RGBA_BLUE], bits[FI_RGBA_ALPHA]);
      if(thisColor == tColor)
      {
        bits[FI_RGBA_ALPHA] = 0;
        bits[FI_RGBA_RED] = 0;
        bits[FI_RGBA_GREEN] = 0;
        bits[FI_RGBA_BLUE] = 0;
      }
      bits += bytespp;
    }
  }
}

void FillImageWithColor(FIBITMAP* dib, const yaglib::ColorQuad color)
{
  int width = FreeImage_GetWidth(dib);
  int height = FreeImage_GetHeight(dib);
  int bytespp = FreeImage_GetLine(dib) / width;

  int adjHeight = height-1;
  for(int y = adjHeight; y >= 0; y--)
  {
    BYTE* bits = FreeImage_GetScanLine(dib, y);
    for(int x = 0; x < width; x++)
    {
      bits[FI_RGBA_ALPHA] = color.alpha;
      bits[FI_RGBA_RED] = color.red;
      bits[FI_RGBA_GREEN] = color.green;
      bits[FI_RGBA_BLUE] = color.blue;
      bits += bytespp;
    }
  }
}
