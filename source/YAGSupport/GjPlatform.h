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
 * @file  GjPlatform.h
 * @brief Platform detection, and flag setups
 *
 */
#ifndef GJ_PLATFORM_HEADER
#define GJ_PLATFORM_HEADER

#ifndef __cplusplus
#error C++ is required
#endif

/**
 * this header should eventually contain platform-specific defines and 
 * symbols.  for now, only windows is supported
 */

/**
 * detect what platform this is 
 */
#ifdef _WIN32 
#define GJPLATFORM_WINDOWS
#elif _WIN64
#define GJPLATFORM_WINDOWS
#else
#error Did not detect a supported platform
#endif

/**
 * if using microsoft C++, turn off some annoying flags
 */
#ifdef _MSC_VER
#pragma warning(disable : 4996) 
#ifndef _CRT_SECURE_NO_DEPRECATE 
#define _CRT_SECURE_NO_DEPRECATE
#endif
#endif

/**
 * we want unicode all around, so define some macros to effect it. 
 * essentially, we turn off multibyte character handling, and enable
 * native unicode handling
 *
 * TODO: non-msc compilers and/or *nix platforms may require different handling
 */

#ifdef _MBCS
#undef _MBCS
#endif

#ifdef MBCS
#undef MBCS
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef UNICODE
#define UNICODE
#endif

#endif /* GJ_PLATFORM_HEADER */
