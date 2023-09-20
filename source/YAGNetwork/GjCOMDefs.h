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
 * @file  GjCOMDefs.h
 * @brief Declarations/macros for COM programming
 *
 * Defines, typedefs, and macros to support COM coding.
 *
 */
#ifndef GJ_COM_DEFS_HEADER
#define GJ_COM_DEFS_HEADER

#include "GjDefs.h"

#define MAKE_INTF_PTR(intf) \
  typedef intf*  I##intf

#define INTERFACE_BEGIN(intf) \
  class intf \
  {  \
  public:

#define INTERFACE_BEGIN_(intf, intfBase) \
  class intf : public intfBase \
  { \
  public:

#define INTERFACE_ENDS };

#define IMETHOD(retVal, methodName) virtual retVal __stdcall methodName 
#define ICALL(retType) retType STDMETHODCALLTYPE
#define IPROC(retType) retType __stdcall

#define IMETHOD_(methodName) IMETHOD(HRESULT, methodName)
#define ICALL_ ICALL(HRESULT)
#define IPROC_ ICALL(HRESULT)

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// macros for AddRef, Release, False testing, and OK Testing
#define _AddRef(obj) if((obj) != NULL)((IUnknown*)(obj))->AddRef()
#define _Release(obj) if((obj) != NULL)((IUnknown*)(obj))->Release(),(obj) = NULL

#define _False(hr)(hr == S_FALSE)
#define _OK(hr)(hr == NOERROR)

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// macros for declaring lots of stuff commonly redeclared. this takes
// away some of the nastiness of COM declarations and implementations
#define DECLARE_IUNKNOWN_METHODS \
  IMETHOD(HRESULT, QueryInterface)(const IID FAR& riid, void** ppvObj); \
  IMETHOD(ULONG, AddRef)(); \
  IMETHOD(ULONG, Release)();

#define DECLARE_ICLASSFACTORY_METHODS \
  IMETHOD(HRESULT, CreateInstance)(IUnknown* pUnkOuter, REFIID riid, void** ppvObject); \
  IMETHOD(HRESULT, LockServer)(BOOL fLock);

#define DEFINE_ADDREF(thisObj) \
  ICALL(ULONG) thisObj::AddRef() { return(CUnknown::AddRef()); }

#define DEFINE_RELEASE(thisObj) \
  ICALL(ULONG) thisObj::Release() { return(CUnknown::Release()); }

#define DEFINE_QUERYINTERFACE_BEGIN(thisObj) \
  ICALL(HRESULT) thisObj::QueryInterface(const IID FAR& riid, void** ppvObj) \
  {  \
    *ppvObj = NULL;

#define DEFINE_QUERYINTERFACE_CHECK(classIID, classRef) \
    if(IsEqualGUID(riid, classIID)) *ppvObj = static_cast<classRef*>(this);

#define DEFINE_QUERYINTERFACE_ADDREF() \
    if(*ppvObj) { \
      _AddRef(*ppvObj); \
      return(NOERROR); \
    } 
#define DEFINE_QUERYINTERFACE_END() \
    return(E_NOINTERFACE); \
  }

#define DEFINE_QUERYINTERFACE_CHAIN(baseObj) \
    return(baseObj::QueryInterface(riid, ppvObj)); \
  }

#define DEFINE_QUERYINTERFACE(thisObj, baseObj, objIID, objType) \
  DEFINE_QUERYINTERFACE_BEGIN(thisObj) \
  DEFINE_QUERYINTERFACE_CHECK(objIID, objType) \
  DEFINE_QUERYINTERFACE_ADDREF() \
  DEFINE_QUERYINTERFACE_CHAIN(baseObj) 

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define CHECK_PTR(p) if(((POINTER)(p)) == NULL) return(E_POINTER)


#endif /* GJ_COM_DEFS_HEADER */
