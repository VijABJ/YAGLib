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
 * @file  GjCOMUtils.h
 * @brief Implementations for base COM interfaces
 *
 * Implementations for IUnknown and IClassFactory.  There are also
 * utiltity functions declared here.
 *
 * This file is intended to be used by code implementing interfaces.  
 * For code declaring interfaces, use GjCOMDefs.h instead.
 *
 */
#ifndef GJ_COM_UTILS_HEADER
#define GJ_COM_UTILS_HEADER

#include "GjCOMDefs.h"

#define CUnknown UnknownImpl

namespace yaglib
{

LPTSTR GUIDToString(GUID* guid);
HRESULT RegisterCOMServer(GUID* guid, HINSTANCE serverInstance, LPCTSTR desc);
HRESULT UnregisterCOMServer(GUID* guid);

/**
 * @brief Implements the IUknown interface
 */
class UnknownImpl : public IUnknown
{
public:
  
  // IUnknown interface
  DECLARE_IUNKNOWN_METHODS

  // constructor/destructor
  UnknownImpl();
  virtual ~UnknownImpl();

  /**
   * @brief Returns the number of all active interfaces.
   */
  static int getActiveInterfacesCount();

private:
  ULONG m_RefCount;
};

/**
 * @brief Implements the IClassFactory inteface.
 */
class ClassFactory : public UnknownImpl, public IClassFactory
{
public:
  // IUnknown interface
  DECLARE_IUNKNOWN_METHODS

  // IClassFactory interface
  DECLARE_ICLASSFACTORY_METHODS

  // constructor/destructor
  ClassFactory(GUID* classId);
  virtual ~ClassFactory();

private:
  BOOL m_Locked;
  GUID* m_ClassId;
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
typedef IUnknown* __stdcall FnCreateObject(GUID& guid);
typedef FnCreateObject* lpFnCreateObject;

/**
 * @brief internal class to contain an interface creating function.
 */
class RegisteredClass
{
public:
  RegisteredClass(GUID& guid, lpFnCreateObject createProc);
  ~RegisteredClass();

  IUnknown* create(GUID* guid = NULL);
  bool hasGUID(GUID& guid);
private:
  GUID m_GUID;
  lpFnCreateObject m_CreateProc;
};

/**
 * @brief internal class to contain a list of RegisteredClass wrappers.
 *
 * This class essentially encapsulates a class factory, and a list of 
 * all the GUIDs such factory supports.
 */
class RegisteredClasses
{
public:
  RegisteredClasses(GUID& guid);
  ~RegisteredClasses();

  bool hasGUID(GUID& guid);
  RegisteredClass* get(GUID& guid);
  void add(GUID& guid, lpFnCreateObject proc);
  void remove(GUID& guid);
  void clear();

protected:
  typedef std::vector<RegisteredClass*> ClassList;

private:
  GUID m_GUID;
  ClassList m_Classes;
};

/**
 * @brief internal class to list all the class factories available.
 *
 * This is a singleton since you will only ever need one of this.  It
 * manages a list of class factory encapsulation.  see RegisteredClasses.
 * If necessary, this class can instantiate an interface given the IID
 * of its factory, and the IID of the interface itself.
 */
class RegisteredProviders
{
public:
  static RegisteredProviders& Instance();
  void initialize();
  void shutdown();

  RegisteredClasses* find(GUID& guid);
  RegisteredClasses* add(GUID& guid);
  void add(GUID& guid, GUID& classId, lpFnCreateObject proc); 
  void remove(GUID& guid);
  void remove(GUID& guid, GUID& classId);

  bool isProviderRegistered(GUID& guid);
  bool isClassRegistered(GUID& guid, GUID& classId);

  IUnknown* create(GUID& guid, GUID& classId);

protected:
  RegisteredProviders();
  RegisteredProviders(const RegisteredProviders&);
  RegisteredProviders& operator=(const RegisteredProviders&);

  typedef std::vector<RegisteredClasses*> ProviderList;

private:
  ProviderList m_Providers;

  void clear();
  RegisteredClasses* getProvider(GUID& guid);
};

inline RegisteredClasses* RegisteredProviders::find(GUID& guid)
{
  return getProvider(guid);
}

inline bool RegisteredProviders::isProviderRegistered(GUID& guid) 
{ 
  return getProvider(guid) != NULL; 
}

#define ph (RegisteredProviders::Instance())


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
IUnknown* __stdcall CreateIUnknown(GUID* guid);
IUnknown* __stdcall CreateIClassFactory(GUID* guid);

} /* namespace yaglib */



#endif /* GJ_COM_UTILS_HEADER */