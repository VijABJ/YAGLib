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

#include "GjFGOperations.h"
#include "GjUnicodeUtils.h"
#include "GjBFS.h"
//#include <boost/filesystem.hpp>
#include <iostream>
using namespace yaglib;

void OperationCopy::process(FGPackage& package, OperationResultProcessor& resultProcessor)
{
  WideStringList files;
  if(bfs::list_files(package.getSourceFolder(), files) == 0)
    return;

  OperationResult result;
  WideString folderName = package.getSourceFolder();
  WideString targetFolder = package.getTargetName() + L"\\";
  for(WideStringList::iterator iter = files.begin(); iter != files.end(); iter++)
  {
    result.transparentColor = ColorQuad(0);
    result.frames.clear();
    WideString qualifiedName = folderName + L"\\" + *iter;

    FIBITMAP* dib = LoadImageFile(qualifiedName, 0);

    int width = FreeImage_GetWidth(dib), height = FreeImage_GetHeight(dib);
    result.size = Size(width, height);

    if(package.isTransparent())
    {
      result.transparentColor = package.getTransparentColor();
      if(package.isTransparencyPerFile())
        GetColorAt(dib, package.getTransparentLocation(), result.transparentColor);
    }

    result.frames.add(Rect(0, 0, width, height));
    resultProcessor(dib, targetFolder + *iter, result);

    FreeImage_Unload(dib);
  }
}

static int VALID_TEXTURE_SIZES[] = {64, 128, 256, 512};
const int MAX_SIZES = sizeof(VALID_TEXTURE_SIZES)/sizeof(int);

void findOptimumTextureSize(const Size& imageSize, const size_t imageCount, Size& finalSize)
{
  finalSize = Size(0, 0);

  int foundSize = 0;
  for(int i = 0; i < MAX_SIZES; i++)
  {
    int tSize = VALID_TEXTURE_SIZES[i];
    if((tSize < imageSize.width) || (tSize < imageSize.height))
      continue;

    int hCount = tSize/imageSize.width, vCount = tSize/imageSize.height;
    size_t maxFrames = hCount * vCount;
    if(maxFrames < imageCount)
      continue;

    foundSize = tSize;
    break;
  }

  if(foundSize == 0)
    throw std::exception("Can\'t find suitable texture size, image is too large");

  finalSize = Size(foundSize, foundSize);
}

void OperationCombine::process(FGPackage& package, OperationResultProcessor& resultProcessor)
{
  // get the names of the files...
  mFiles.clear();
  if(bfs::list_files(package.getSourceFolder(), mFiles) == 0)
    return;

  // get the size of the first image. ALL the images must be the same size!
  // later, maybe add provision to resize the images to fit the largest?
  WideString folderName = package.getSourceFolder();
  GetImageDimensions(folderName + L"\\" + mFiles[0], mImageSize);
  findOptimumTextureSize(mImageSize, mFiles.size(), mCombinedSize);

  // create the destination image
  FIBITMAP* dest = FreeImage_Allocate(mCombinedSize.width, mCombinedSize.height, 32);

  // ready for the loop
  Point target(0, 0);
  OperationResult result;
  result.size = mCombinedSize;
  result.transparentColor = ColorQuad(0);
  for(WideStringList::iterator iter = mFiles.begin(); iter != mFiles.end(); iter++)
  {
    WideString qualifiedName = folderName + L"\\" + *iter;

    FIBITMAP* dib = LoadImageFile(qualifiedName, 0);
    FreeImage_Paste(dest, dib, target.x, target.y, 256);
    Rect frame = Rect(target, Point(target.x+mImageSize.width, target.y+mImageSize.height));
    result.frames.add(frame);
    FreeImage_Unload(dib);

    target.x += mImageSize.width;
    if((mCombinedSize.width - target.x) < mImageSize.width)
    {
      target.x = 0;
      target.y += mImageSize.height;
    }
  }

  if(package.isTransparent())
    GetColorAt(dest, package.getTransparentLocation(), result.transparentColor);

  // process the result
  if(package.requiresPerFileSprite())
    result.mFiles = &mFiles;
  resultProcessor(dest, package.getTargetName(), result);

  // we're done with this
  FreeImage_Unload(dest);
}

FIBITMAP* OperationReframe::extraProcessing(FIBITMAP* dib, OperationResult& result)
{
  mFrames.clear();
  int bytespp = FreeImage_GetLine(dib) / result.size.width;

  std::vector<int> columns;
  std::vector<int> rows;

  // first, get the color of the bounding frames
  BYTE* bits = FreeImage_GetScanLine(dib, result.size.height-1);
  ColorQuad border = ColorQuad(bits[FI_RGBA_RED], bits[FI_RGBA_GREEN], bits[FI_RGBA_BLUE], bits[FI_RGBA_ALPHA]);

  // find the column separators
  bits = FreeImage_GetScanLine(dib, result.size.height-2);
  columns.push_back(0);
  bits += bytespp;
  for(int x = 1; x < (result.size.width-2); x++)
  {
    ColorQuad thisColor = ColorQuad(bits[FI_RGBA_RED], bits[FI_RGBA_GREEN], bits[FI_RGBA_BLUE], bits[FI_RGBA_ALPHA]);
    if((int)thisColor == (int)border)
      columns.push_back(x);

    bits += bytespp;
  }
  columns.push_back(result.size.width-1);

  // find the row separators
  int adjHeight = result.size.height-1;
  rows.push_back(0);
  for(int y = adjHeight-1; y > 0; y--)
  {
    bits = FreeImage_GetScanLine(dib, y);
    bits += bytespp;
    ColorQuad thisColor = ColorQuad(bits[FI_RGBA_RED], bits[FI_RGBA_GREEN], bits[FI_RGBA_BLUE], bits[FI_RGBA_ALPHA]);
    if((int)thisColor == (int)border)
      rows.push_back(adjHeight-y);

  }
  rows.push_back(adjHeight);

  // now write out the frames...
  for(unsigned y = 0; y < rows.size()-1; y++)
    for(unsigned x = 0; x < columns.size()-1; x++)
    {
      Rect r(columns[x]+1, rows[y]+1, columns[x+1]-1, rows[y+1]-1);
      mFrames.add(r);
    }

  Size required = Size(static_cast<int>(result.size.width - columns.size() + columns.size() - 1), 
                       static_cast<int>(result.size.height - rows.size() + rows.size() - 1));

#ifdef OPTIMIZE_TEXTURE_SIZE
  int largest = max(required.width, required.height);
  Size wantSize(-1, -1);
  for(int i = 0; i < MAX_SIZES; i++)
  {
    int tSize = VALID_TEXTURE_SIZES[i];
    if(tSize >= largest)
    {
      wantSize = Size(tSize);
      break;
    }
  }
#else
  Size wantSize = required;
#endif
  result.size = wantSize;

  FIBITMAP* dest = FreeImage_Allocate(wantSize.width, wantSize.height, 32);
  FillImageWithColor(dest, ColorQuad(0, 0, 0, 0));
  Point pos(0, 0);
  int hAdjustment = 0;
  for(RectList::iterator iter = mFrames.begin(); iter != mFrames.end(); iter++)
  {
    Rect r = *iter;
    FIBITMAP* clone = FreeImage_Copy(dib, r.left, r.top, r.right+1, r.bottom+1);
    int cloneWidth = FreeImage_GetWidth(clone), cloneHeight = FreeImage_GetHeight(clone);

    if(cloneHeight > hAdjustment)
      hAdjustment = cloneHeight;
    if((pos.x + cloneWidth) > wantSize.width)
    {
      pos.x = 0;
      pos.y += hAdjustment + 1;
      hAdjustment = 0;
    }

    result.frames.add(Rect(pos.x, pos.y, pos.x+cloneWidth,pos.y+cloneHeight));
    FreeImage_Paste(dest, clone, pos.x, pos.y, 256);
    pos.x += cloneWidth + 1;
    FreeImage_Unload(clone);
  }

  return dest;
}

void OperationReframe::process(FGPackage& package, OperationResultProcessor& resultProcessor)
{
  WideStringList files;
  if(bfs::list_files(package.getSourceFolder(), files) == 0)
    return;

  OperationResult result;
  WideString folderName = package.getSourceFolder();
  WideString targetFolder = package.getTargetName() + L"\\";
  for(WideStringList::iterator iter = files.begin(); iter != files.end(); iter++)
  {
    result.transparentColor = ColorQuad(0);
    result.frames.clear();
    WideString qualifiedName = folderName + L"\\" + *iter;

    FIBITMAP* dib = LoadImageFile(qualifiedName, 0);

    int width = FreeImage_GetWidth(dib), height = FreeImage_GetHeight(dib);
    result.size = Size(width, height);

    if(package.isTransparent())
    {
      result.transparentColor = package.getTransparentColor();
      if(package.isTransparencyPerFile())
        GetColorAt(dib, package.getTransparentLocation(), result.transparentColor);
    }

    FIBITMAP* final = extraProcessing(dib, result);
    resultProcessor(final, targetFolder + *iter, result);

    FreeImage_Unload(final);
    FreeImage_Unload(dib);
  }
}
