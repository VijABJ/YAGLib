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

#include "GjUnicodeUtils.h"
#include <cstdio>
#include <cstdlib>
using namespace yaglib;

typedef unsigned long UTF32;    /* at least 32 bits */
typedef unsigned short UTF16;   /* at least 16 bits */
typedef unsigned char UTF8;	    /* typically 8 bits */

static const int HALF_SHIFT  = 10; /* used for shifting by 10 bits */
static const UTF32 HALF_BASE = 0x0010000UL;
static const UTF32 HALF_MASK = 0x3FFUL;

#define UNI_SUR_HIGH_START  (UTF32)0xD800
#define UNI_SUR_HIGH_END    (UTF32)0xDBFF
#define UNI_SUR_LOW_START   (UTF32)0xDC00
#define UNI_SUR_LOW_END     (UTF32)0xDFFF

#define UNI_REPLACEMENT_CHAR (UTF32)0x0000FFFD

/* --------------------------------------------------------------------- */

/*
 * Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
 * left as-is for anyone who may want to do such conversion, which was
 * allowed in earlier algorithms.
 */
static const char trailingBytesForUTF8[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/*
 * Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence.
 */
static const UTF32 offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL, 
                   0x03C82080UL, 0xFA082080UL, 0x82082080UL };

/*
 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
 * for *legal* UTF-8 will be 4 or fewer bytes total.
 */
static const UTF8 firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

// UTF8String
UTF8String::UTF8String(const WideString& value) : m_Buffer(NULL), m_BufSize(0)
{
  assign(value);
}

UTF8String::UTF8String(const char* value) : m_Buffer(NULL), m_BufSize(0)
{
  assign(value);
}

UTF8String::~UTF8String()
{
  clear();
}

const char* UTF8String::c_str() const 
{ 
  return m_Buffer; 
}

int UTF8String::c_len() const 
{ 
  return (m_BufSize == 0) ? 0 : (m_BufSize-1); 
}

void UTF8String::assign(const WideString& value)
{
  clear();
  calculateAndAllocateSpace(const_cast<WideString&>(value));
  convertToUTF8(const_cast<WideString&>(value));
}

void UTF8String::assign(const char* value)
{
  clear();
  if(value)
  {
    m_BufSize = (int) strlen(value) + 1;
    m_Buffer = new char [m_BufSize];
    strncpy(m_Buffer, value, m_BufSize);
  }
}

void UTF8String::calculateAndAllocateSpace(WideString& value)
{
  // count the number of bytes we have to allocate
  // in order to store the UTF8 equivalent
  int nCount = 0;
  UTF32 ch, ch2;
  for(WideString::iterator iter = value.begin(); iter != value.end(); iter++)
  {
    ch = (UTF32) *iter;
    // if this is a surrogate, process next char as well
    if((ch >= UNI_SUR_HIGH_START) && (ch <= UNI_SUR_HIGH_END)) 
    {
      iter++;
      if(iter == value.end()) break;
      ch2 = (UTF32) *iter;
      /* If it's a low surrogate, convert to UTF32. */
      if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) 
      {
        ch = ((ch - UNI_SUR_HIGH_START) << HALF_SHIFT)
              + (ch2 - UNI_SUR_LOW_START) + HALF_BASE;
      }
      else
      // invalid second surrogate, just discard it and back up one char
      {
        --iter;
        continue;
      }
    }

    /* Figure out how many bytes the result will require */
    if (ch < (UTF32)0x80) 
      nCount += 1;
    else if (ch < (UTF32)0x800) 
      nCount += 2;
    else if (ch < (UTF32)0x10000) 
      nCount += 3;
    else if (ch < (UTF32)0x110000) 
      nCount += 4;
    else 
      nCount += 3;
    // use UNI_REPLACEMENT_CHAR in place of ch
  } // for(iterator...

  nCount++; // add extra for the null terminator
  m_BufSize = nCount;
  m_Buffer = new char[m_BufSize];
}

void UTF8String::convertToUTF8(WideString& value)
{
  char* dest = m_Buffer;
  char* target;

  UTF32 ch, ch2;
  unsigned short bytesToWrite = 0;
  const UTF32 byteMask = 0xBF;
  const UTF32 byteMark = 0x80; 

  for(WideString::iterator iter = value.begin(); iter != value.end(); iter++)
  {
    ch = (UTF32) *iter;
    // if this is a surrogate, process next char as well
    if((ch >= UNI_SUR_HIGH_START) && (ch <= UNI_SUR_HIGH_END)) 
    {
      iter++;
      if(iter == value.end()) break;
      ch2 = (UTF32) *iter;
      /* If it's a low surrogate, convert to UTF32. */
      if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) 
      {
        ch = ((ch - UNI_SUR_HIGH_START) << HALF_SHIFT)
                  + (ch2 - UNI_SUR_LOW_START) + HALF_BASE;
      }
      else
      // invalid second surrogate, just discard it and back up one char
      {
        --iter;
        continue;
      }
    }

	  /* Figure out how many bytes the result will require */
    if (ch < (UTF32)0x80) 
      bytesToWrite = 1;
    else if (ch < (UTF32)0x800) 
      bytesToWrite = 2;
    else if (ch < (UTF32)0x10000) 
      bytesToWrite = 3;
    else if (ch < (UTF32)0x110000) 
      bytesToWrite = 4;
    else 
    {
      bytesToWrite = 3;
      ch = UNI_REPLACEMENT_CHAR;
    }

    target = dest + bytesToWrite;
    dest = target;
    switch (bytesToWrite) 
    { /* note: everything falls through. */
      case 4: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
      case 3: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
      case 2: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
      case 1: *--target =  (UTF8)(ch | firstByteMark[bytesToWrite]);
    }

  } // for(iterator...

  *dest = '\0';
}

UTF8String& UTF8String::operator=(const WideString& value)
{
  assign(value);
  return *this;
}

UTF8String& UTF8String::operator=(const char* value)
{
  assign(value);
  return *this;
}

bool UTF8String::isLegal(char* source, int nRemaining) 
{
  int nTrailer = trailingBytesForUTF8[*source];
  if(nRemaining < nTrailer) return false;

  const UTF8 *srcptr = (UTF8*)(source + nRemaining);
  UTF8 ch;

  switch(nTrailer)
  {
    default:
      return false;
    case 3: 
      if ((ch = (*--srcptr)) < 0x80 || ch > 0xBF) return false;
    case 2: 
      if ((ch = (*--srcptr)) < 0x80 || ch > 0xBF) return false;
    case 1: 
      if ((ch = (*--srcptr)) > 0xBF) return false;
      switch (*source) 
      {
        /* no fall-through in this inner switch */
        case 0xE0: if (ch < 0xA0) return false; break;
        case 0xED: if (ch > 0x9F) return false; break;
        case 0xF0: if (ch < 0x90) return false; break;
        case 0xF4: if (ch > 0x8F) return false; break;
        default:   if (ch < 0x80) return false;
      }
    case 0: 
      {
        unsigned char cb = static_cast<unsigned char>(*source);
        if ((cb >= 0x80) && (cb < 0xC2)) 
          return false;
      }
  }

  return ((unsigned char)*source <= 0xF4);
}

WideString UTF8String::asWideString() const
{
  WideString dest(L"");
  if(m_Buffer)
  {
    int nRemaining = m_BufSize - 1;
    char* utf8Value = m_Buffer;
    while(nRemaining > 0)
    {
      if(!isLegal(utf8Value, nRemaining)) break;
      int nTrailer = trailingBytesForUTF8[*utf8Value];

      UTF32 ch = 0;
      switch(nTrailer)
      {
        case 3: 
          ch += ((UTF8)*utf8Value++); 
          ch <<= 6;
        case 2: 
          ch += ((UTF8)*utf8Value++); 
          ch <<= 6;
        case 1: 
          ch += ((UTF8)*utf8Value++); 
          ch <<= 6;
        case 0: 
          ch += *utf8Value++;
      }
      nRemaining -= nTrailer + 1;

      dest.push_back((WideChar) ch);
    }
  }

  return dest;
}

void UTF8String::clear()
{
  if(m_Buffer)
  {
    delete [] m_Buffer;
    m_Buffer = NULL;
    m_BufSize = 0;
  }
}

