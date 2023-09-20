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
 * format for folder name contain embedded options. it MUST begin with @
 *    @targetName#option1#option2...
 *
 * option can be any or all of the following:
 *    c - copy only, generate 1 frame to reprent the file
 *    g - old-style gridded, re-frame and copy
 *    f - generate sprite name for each file (combineOp only)
 *    t - transparent
 *    tf<fileName> - file where transparent color can be read off, defaults to subject file
 *    tx<int>, ty<int> - position of transparent color, defaults to -1, -1, i.e., none
 *
 */

#include "GjFGPackages.h"
#include "GjUnicodeUtils.h"
#include "GjStringUtils.h"
#include <boost/algorithm/string.hpp>
using namespace yaglib;

FGPackage::FGPackage(const WideString& folderName) : mSourceFolder(folderName),
  mIsTransparent(false), mTransparentColor(ColorQuad()), mOperation(foCombine),
  mPerFileSprite(false), mPerFileTransparency(true), mTransparentLocation(Point(0, 0))
{
  initialize();
}

FGPackage::~FGPackage()
{
}

void FGPackage::initialize()
{
  if(mSourceFolder[0] != '@') 
    throw std::exception("Invalid folder name. No @ character detected");

  size_t optChar = mSourceFolder.find_first_of('#');
  WideString options;
  if(optChar != WideString::npos)
  {
    mTargetName = mSourceFolder.substr(1, optChar-1);
    options = mSourceFolder.substr(optChar+1);
  }
  else
    mTargetName = mSourceFolder.substr(1);

  // process the options given
  if(options.size() > 0)
  {
    Point tLocation(-1, -1);
    WideString tFileSource;

    WideStringList optionList;
    boost::split(optionList, options, boost::is_any_of(L"#"), boost::token_compress_on);
    for(WideStringList::iterator iter = optionList.begin(); iter != optionList.end(); iter++)
    {
      WideString opt = *iter;
      switch(opt[0])
      {
      case 'c':
        mOperation = foCopy;
        break;
      case 'g':
        mOperation = foReframe;
        break;
      case 'f':
        mPerFileSprite = true;
        break;
      case 't':
        mIsTransparent = true;
        if(opt.size() > 1)
          switch(opt[1])
          {
          case 'f':
            tFileSource = opt.substr(2);
            break;
          case 'x':
            tLocation.x = string_utils::parse_int(opt.substr(2));
            break;
          case 'y':
            tLocation.y = string_utils::parse_int(opt.substr(2));
            break;
          }
        break;
      default:
        throw std::exception("Unrecognized option encountered");
        break;
      }
    }

    if((tLocation.x != -1) && (tLocation.y != -1))
      mTransparentLocation = tLocation;

    // resolve transparency flags
    if(tFileSource.size() > 0)
    {
      mPerFileTransparency = false;
      tFileSource = mSourceFolder + L"\\" + tFileSource;
      GetColorAt(tFileSource, mTransparentLocation, mTransparentColor);
    }

  }
}

WideString const& FGPackage::getSourceFolder() const
{
  return mSourceFolder;
}

WideString const& FGPackage::getTargetName() const
{
  return mTargetName;
}

bool FGPackage::requiresPerFileSprite() const
{
  return mPerFileSprite;
}

bool FGPackage::isTransparent() const
{
  return mIsTransparent;
}

bool FGPackage::isTransparencyPerFile() const
{
  return mPerFileTransparency;
}

ColorQuad const& FGPackage::getTransparentColor() const
{
  return mTransparentColor;
}

yaglib::Point const& FGPackage::getTransparentLocation() const
{
  return mTransparentLocation;
}

FrameOperation FGPackage::getOperation() const
{
  return mOperation;
}

