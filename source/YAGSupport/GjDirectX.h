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
 * @file  GjDirectX.h
 * @brief DirectX includes and defines
 *
 * DirectX wrapper declarations to isolate differences between directx 8 and 9
 *
 */
#ifndef GJ_DIRECTX_DEFS_HEADER
#define GJ_DIRECTX_DEFS_HEADER

#include "GjDefs.h"

// undef to use DX9
#define USE_DIRECTX_9

#ifdef USE_DIRECTX_9
  #include <d3d9.h>
  #include <d3dx9.h>
  #include <dxerr9.h>
  #define D3D LPDIRECT3D9
  #define D3DCAPS D3DCAPS9
  #define D3DDEVICE LPDIRECT3DDEVICE9
  #define D3DVERTEXBUFFER LPDIRECT3DVERTEXBUFFER9
  #define D3DTEXTURE LPDIRECT3DTEXTURE9
  #define D3DVIEWPORT D3DVIEWPORT9
  #define LOCKDATA(data) ((void**)&data)
#else
  #include <d3d8.h>
  #include <d3dx8.h>
  #include <dxerr8.h>
  #define D3D LPDIRECT3D8
  #define D3DCAPS D3DCAPS8
  #define D3DDEVICE LPDIRECT3DDEVICE8
  #define D3DVERTEXBUFFER LPDIRECT3DVERTEXBUFFER8
  #define D3DTEXTURE LPDIRECT3DTEXTURE8
  #define D3DVIEWPORT D3DVIEWPORT8
  #define LOCKDATA(data) ((BYTE**)&data)
#endif


#endif /* GJ_DIRECTX_DEFS_HEADER */
