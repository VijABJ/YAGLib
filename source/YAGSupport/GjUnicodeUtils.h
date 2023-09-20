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
 * @file  GjUnicodeUtils.h
 * @brief Extra unicode functions
 *
 * Any other unicode support classes, functions, etc, gets placed
 * here.  The core class here is UTF8String, which is basically a
 * a bridge class between WideString and char*.
 *
 */
#ifndef GJ_UNICODE_UTILS_HEADER
#define GJ_UNICODE_UTILS_HEADER

#include "GjDefs.h"

namespace yaglib
{

class UTF8String
{
public:
  UTF8String(const WideString& value);
  UTF8String(const char* value);
  ~UTF8String();

  WideString asWideString() const;
 
  const char* c_str() const;
  int c_len() const;

  UTF8String& operator=(const WideString& value);
  UTF8String& operator=(const char* value);

  static bool isLegal(char* source, int nRemaining);

private:
  char* m_Buffer;
  int m_BufSize;

  void clear();
  void calculateAndAllocateSpace(WideString& value);
  void convertToUTF8(WideString& value);

  void assign(const WideString& value);
  void assign(const char* value);
};

inline WideString from_char_p(const char* value)
{
  return UTF8String(value).asWideString();
}

} /* namespace yaglib */

#endif /* GJ_UNICODE_UTILS_HEADER */
