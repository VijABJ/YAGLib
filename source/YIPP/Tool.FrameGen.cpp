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

#include "GjDefs.h"
#include "GjIniFiles.h"
#include "GjUnicodeUtils.h"
#include "GjGameLib.h"
#include "GjTextures.h"
#include "GjSprites.h"
#include "GjFGPackages.h"
#include "GjFGOperations.h"
#include "GjStringUtils.h"
#include "GjBFS.h"
#include <iostream>
#include "FreeImage.h"

#pragma comment(lib, "YAGSupport.lib")
#pragma comment(lib, "YAGCore.lib")
#pragma comment(lib, "FreeImage.lib")

using namespace yaglib;

class ResultWriter : public OperationResultProcessor
{
public:
  ResultWriter(const WideString& destination) : mDestination(destination), mTextureConfig(NULL) {};
  virtual ~ResultWriter() ;
  virtual void process(FIBITMAP* dib, WideString targetFileName, OperationResult& result);
private:
  IniSettings* mTextureConfig;
  IniSettings* mSpriteConfig;
  WideString mDestination;
  WideStringList mAliases;
  WideString validateName(WideString& targetName);
};

ResultWriter::~ResultWriter() 
{ 
  if(mTextureConfig)
  {
    Settings* section = mTextureConfig->get(ALIASES_SECTION);
    for(WideStringList::iterator iter = mAliases.begin(); iter != mAliases.end(); iter++)
      section->add(*iter);

    mTextureConfig->save(mDestination + IMAGE_CONFIG_FILENAME);
    SAFE_DELETE(mTextureConfig); 
  }
  if(mSpriteConfig)
  {
    mSpriteConfig->save(mDestination + SPRITES_CONFIG_FILENAME);
    SAFE_DELETE(mSpriteConfig);
  }
};

WideString ResultWriter::validateName(WideString& targetName)
{
  WideString final = mDestination;
  WideString temp = targetName;
  size_t separator = temp.find('\\');
  while(separator != WideString::npos)
  {
    final += temp.substr(0, separator);
    temp = temp.substr(separator+1);
    if(!bfs::exists(final))
      CreateDirectory(final.c_str(), NULL);

    final += '\\';
    separator = temp.find('\\');
  }
  // now change the file extension. we'll be saving PNGs
  string_utils::chop_after_last(temp, WideString(L"."), false);
  final += temp + L".png";
  return final;
}

void ResultWriter::process(FIBITMAP* dib, WideString targetFileName, OperationResult& result)
{
  if(mTextureConfig == NULL)
  {
    mTextureConfig = new IniSettings(mDestination + IMAGE_CONFIG_FILENAME);
    mTextureConfig->clear();
    mSpriteConfig = new IniSettings(mDestination + SPRITES_CONFIG_FILENAME);
    mSpriteConfig->clear();
  }

  // create alpha channels for transparent colors instead
  if((int)result.transparentColor != 0)
  {
    ConvertTransparencyToAlpha(dib, result.transparentColor);
    result.transparentColor = 0;
  }

  //
  WideString nameToSaveTo = validateName(targetFileName);
  FreeImage_Save(FIF_PNG, dib, UTF8String(nameToSaveTo).c_str());

  char buf[100];
  WideString sectionName = targetFileName;
  string_utils::chop_after_last(sectionName, WideString(L"."), false);
  mAliases.push_back(sectionName + L"=" + sectionName + L".png");
  //
  WideString spriteName = sectionName;
  size_t slashPos = spriteName.find('\\');
  if(slashPos != WideString::npos)
    spriteName = spriteName.substr(slashPos+1);
  //
  sectionName += L".png";

  Settings* section = mTextureConfig->get(sectionName);
  // add the transparent color if there is one
  if(((int)result.transparentColor) != 0)
  {
    _snprintf(buf, 100, "%08X", (int)result.transparentColor);
    section->add(L"transparentColor", from_char_p(buf));
  }
  // add the dimensions
  _snprintf(buf, 100, "width=%d", result.size.width);
  section->add(from_char_p(buf));
  _snprintf(buf, 100, "height=%d", result.size.height);
  section->add(from_char_p(buf));
  // add the number of frames
  _snprintf(buf, 100, "frames=%d", static_cast<int>(result.frames.size()));
  section->add(from_char_p(buf));

  int frameNumber = 0;
  section = mTextureConfig->get(L"#" + sectionName);
  for(RectList::iterator iter = result.frames.begin(); iter != result.frames.end(); iter++, frameNumber++)
  {
    Rect r = *iter;
    _snprintf(buf, 100, "%d=%d,%d,%d,%d", frameNumber, r.left, r.top, r.right, r.bottom);
    section->add(from_char_p(buf));
  }

  // create the main section in the sprite config if necessary
  section = mSpriteConfig->get(SPRITES_MAIN_SECTION);

  // add sprite entry
  _snprintf(buf, 100, "%s=%s,0,%d", UTF8String(spriteName).c_str(), UTF8String(sectionName).c_str(), result.frames.size()-1);
  section->add(from_char_p(buf));

  // add extra sprites if necessary
  if(result.mFiles)
  {
    // step 1, remove the file extensions
    size_t seriesTagCount = 0;
    for(WideStringList::iterator iter = result.mFiles->begin(); iter != result.mFiles->end(); iter++)
    {
      WideString fileItem = *iter;
      string_utils::chop_after_last(fileItem, WideString(L"."), true);
      *iter = fileItem;
      if(fileItem.find('#') != WideString::npos)
        seriesTagCount++;
    }

    if(seriesTagCount < result.mFiles->size())
    {
      int index = 0;
      for(WideStringList::iterator iter = result.mFiles->begin(); iter != result.mFiles->end(); iter++, index++)
      {
        _snprintf(buf, 100, "%s=%s,%d,%d", UTF8String(*iter).c_str(), UTF8String(sectionName).c_str(), index, index);
        section->add(from_char_p(buf));
      }
    }
    else
    {
      // chop off the portions after the # character
      for(WideStringList::iterator iter = result.mFiles->begin(); iter != result.mFiles->end(); iter++)
      {
        WideString fileItem = *iter;
        string_utils::chop_after_last(fileItem, WideString(L"#"), true);
        *iter = fileItem;
      }

      int index = 0, lastIndex = 0;
      WideString lastName(L"");
      for(WideStringList::iterator iter = result.mFiles->begin(); iter != result.mFiles->end(); iter++, index++)
      {
        if(lastName != *iter)
        {
          if(lastName.size() > 0)
          {
            _snprintf(buf, 100, "%s=%s,%d,%d", UTF8String(lastName).c_str(), UTF8String(sectionName).c_str(), lastIndex, index-1);
            section->add(from_char_p(buf));
          }
          lastName = *iter;
          lastIndex = index;
        }
      }
      if(lastIndex < static_cast<int>(result.mFiles->size()))
      {
        _snprintf(buf, 100, "%s=%s,%d,%d", UTF8String(lastName).c_str(), UTF8String(sectionName).c_str(), lastIndex, result.mFiles->size()-1);
        section->add(from_char_p(buf));
      }
    }
  }
}

int _tmain(int argc, _TCHAR* argv[])
{
  FreeImage_Initialise();

  // determine where to place the files
  WideString appPath = NativeApplication::ApplicationPath();
  string_utils::conditional_append(appPath, '\\');
  WideString destination(appPath);
  if(argc > 1)
  {
    // destination folder specified, be sure this folder exists!
    destination += WideString(argv[1]);
    if(!bfs::exists(destination))
      CreateDirectory(destination.c_str(), NULL);
    destination += '\\';
  }

  ResultWriter writer(destination);
  WideStringList list;
  ObjectList<FGPackage> packages;

  // first, scan the folders for those that needs processing
  bfs::list_files(appPath, list, false, true);
  //OSFolders::List(L"*.*", list);
  for(WideStringList::iterator iter = list.begin(); iter != list.end(); iter++)
  {
    WideString folderName = *iter;
    if(folderName[0] == '@')
      packages.add(new FGPackage(folderName));
  }

  OperationCopy copyOp;
  OperationCombine combineOp;
  OperationReframe reframeOp;

  for(ObjectList<FGPackage>::iterator iter = packages.begin(); iter != packages.end(); iter++)
  {
    FGPackage* pck = *iter;
    std::wcout << L"Processing " << pck->getSourceFolder() << L"..." << std::endl;
    switch(pck->getOperation())
    {
    case foCopy:
      copyOp(*pck, writer);
      break;
    case foReframe:
      reframeOp(*pck, writer);
      break;
    case foCombine:
      combineOp(*pck, writer);
      break;
    }
  }

  FreeImage_DeInitialise();
	return 0;
}

