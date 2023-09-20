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
 * @file  GjColors.h
 * @brief Color classes
 *
 */
#ifndef GJ_COLOR_WRAPPERS_HEADER
#define GJ_COLOR_WRAPPERS_HEADER

#include "GjDefs.h"

namespace yaglib 
{

typedef struct ColorTriple
{
  BYTE8 blue;
  BYTE8 green;
  BYTE8 red;

  ColorTriple()
  {
    red = green = blue = 0;
  };
  ColorTriple(const int color)
  {
    red   = (BYTE8) ((color >> 16) & 0xff);
    green = (BYTE8) ((color >> 8) & 0xff);
    blue  = (BYTE8) (color & 0xff);
  };
  ColorTriple(const BYTE8 _red, const BYTE8 _green, const BYTE8 _blue)
  {
    red = _red;
    green = _green;
    blue = _blue;
  };
  operator int() const
  {
    return (((int)red) << 16) | (((int)green) << 8)  | ((int)blue);
  };
} ColorTriple;

typedef struct ColorQuad
{
  BYTE8 blue;
  BYTE8 green;
  BYTE8 red;
  BYTE8 alpha;

  ColorQuad()
  {
    red = green = blue = 0;
    alpha = 0xff;
  };
  ColorQuad(const int color)
  {
    alpha = (BYTE8) ((color >> 24) & 0xff);
    red   = (BYTE8) ((color >> 16) & 0xff);
    green = (BYTE8) ((color >> 8) & 0xff);
    blue  = (BYTE8) (color & 0xff);
  };
  ColorQuad(const BYTE8 _red, const BYTE8 _green, 
    const BYTE8 _blue, const BYTE8 _alpha = 0xff)
  {
    assign(_red, _green, _blue, _alpha);
  };
  void assign(const BYTE8 _red, const BYTE8 _green, 
    const BYTE8 _blue, const BYTE8 _alpha = 0xff)
  {
    red = _red;
    green = _green;
    blue = _blue;
    alpha = _alpha;
  };
  operator int() const
  {
    return (((int)alpha) << 24) | (((int)red) << 16) |
           (((int)green) << 8)  | ((int)blue);
  };
} ColorQuad;



} /* namespace yaglib */

#endif /* GJ_COLOR_WRAPPERS_HEADER */
