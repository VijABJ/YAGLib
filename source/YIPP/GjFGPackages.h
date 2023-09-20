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
 * @file  GjFGPackages.h
 * @brief info package, used for listing tasks for frame generation
 *
 * see .cpp file for implementation notes
 *
 */
#ifndef GJ_FG_PACKAGES_HEADER
#define GJ_FG_PACKAGES_HEADER

#include "GjDefs.h"
#include "GjColors.h"
#include "GjPoints.h"
#include "GjFreeImageUtils.h"

typedef std::vector<WideString> WideStringList;

typedef enum FrameOperation 
{
  foCopy,       // copy as is
  foReframe,    // old-style grid, reframe with grid
  foCombine     // combine existing images, the DEFAULT
};

class FGPackage
{
public:
  FGPackage(const WideString& folderName);
  virtual ~FGPackage();

  WideString const& getSourceFolder() const;
  WideString const& getTargetName() const;
  bool requiresPerFileSprite() const;
  bool isTransparent() const;
  bool isTransparencyPerFile() const;
  yaglib::ColorQuad const& getTransparentColor() const;
  yaglib::Point const& getTransparentLocation() const;
  FrameOperation getOperation() const;

private:
  WideString mSourceFolder;
  WideString mTargetName;
  bool mPerFileSprite;
  bool mIsTransparent;
  bool mPerFileTransparency;
  yaglib::ColorQuad mTransparentColor;
  yaglib::Point mTransparentLocation;
  FrameOperation mOperation;

  void initialize();
};

#endif /* GJ_FG_PACKAGES_HEADER */

