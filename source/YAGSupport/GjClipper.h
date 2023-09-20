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
 * @file  GjClipper.h
 * @brief Global clipper class
 *
 * singleton clipper manager
 *
 */
#ifndef GJ_CLIPPER_HEADER
#define GJ_CLIPPER_HEADER

#include "GjDefs.h"
#include "GjRectangles.h"
#include "GjTemplates.h"

namespace yaglib 
{

template<class T>
class clipper
{
public:
  rectangle<T> const& getBounds() const
  {
    return mBounds;
  };
  void setBounds(const rectangle<T>& bounds)
  {
    mActive = mBounds = bounds;
  };
  rectangle<T> const& get() const
  {
    return mActive;
  };
  void set(rectangle<T> r, const bool intersectActive = true, const bool replaceActive = false)
  {
    if(intersectActive) mActive.clip(r);
    if(!replaceActive) mClipStack.push(mActive);
    mActive = r;
  };
  void rewind()
  {
    if(mClipStack.size() > 0)
    {
      mActive = mClipStack.top();
      mClipStack.pop();
    }
  };
  int clip(point<T>& p) const
  {
    return mActive.clip(p);
  };
  int clip(point<T>& p1, point<T>& p2) const
  {
    return mActive.clip(p1, p2);
  };
  int clip(rectangle<T>& r) const
  {
    return mActive.clip(r);
  };

private:
  rectangle<T> mBounds;
  rectangle<T> mActive;
  std::stack< rectangle<T> > mClipStack;
};

typedef clipper<GJFLOAT> GJCLIPPER;

} /* namespace yaglib */

#endif /* GJ_CLIPPER_HEADER */
