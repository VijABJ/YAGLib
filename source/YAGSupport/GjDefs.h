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
 * @file  GjDefs.h
 * @brief Standard/Common definitions and declarations
 *
 * Common types and defines are setup here, and some calling conventions.
 * All the other files in this library uses this one.
 *
 * @notes The whole library assumes that unicode is in effect.  However,
 *        this does not mean that char* cannot and will not be used in some
 *        parts -- as in some cases, this is unavoidable.
 *
 */
#ifndef GJ_DEFS_HEADER
#define GJ_DEFS_HEADER

#include "GjPlatform.h"

//#define USE_HIGH_PRECISION_FLOAT

#include <tchar.h>
#include <string>
#include <cstring>
#include <assert.h>

#ifndef NO_STD_TEMPLATES
#include <vector>
#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <functional>
#include <vector>
#include <stack>
#include <queue>
#include <bitset>
#endif /* NO_STD_TEMPLATES */

/** 
 * declarations of commonly-used types.  using TCHAR from windows headers
 * is not optimal since we want to eventually compile against another
 * platform.  for now, we define our own types.  when in windows,
 * the types will match anyway.
 */
typedef wchar_t           WideChar;     ///< a UTF16 character
typedef WideChar*         PWideChar;    ///< Pointer to a UTF16 character
typedef std::wstring      WideString;   ///< a UTF16 String
typedef WideString*       PWideString;  ///< Pointer to a UTF16 String

typedef unsigned long     Longint;      ///< Alias for unsigned long
typedef unsigned char     Byte;         ///< Alias for unsigned char
typedef Byte*             PByte;        ///< Pointer to Bytes

#ifdef USE_HIGH_PRECISION_FLOAT
typedef double            GJFLOAT;      ///< floating point type
#else
typedef float             GJFLOAT;      ///< floating point type
#endif

typedef unsigned char     BYTE8;

/* extra macro for deletion...
 */
#define SAFE_DELETE(p) if((p) != NULL) { delete (p); (p) = NULL; }
#define SAFE_RELEASE(p) if(p) { p->Release(); p = NULL; }

/** 
 * handles windows file inclusions, unless disabled 
 */

#ifdef GJPLATFORM_WINDOWS
  #ifndef NO_WINDOWS_HEADERS
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  //#include <comdef.h>
  #include <objbase.h>
  #endif /* NO_WINDOWS_HEADERS */
#endif

#endif /* GJ_DEFS_HEADER */
