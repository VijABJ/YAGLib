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

#define GJ_COM_UTILS_CPP__
#include "GjCOMUtils.h"
#include <cstdlib>
#include <cstdio>
using namespace yaglib;

#define HKCR HKEY_CLASSES_ROOT

LPTSTR yaglib::GUIDToString(GUID* guid)
{
  if(!guid) return(NULL);

  const int bufferSize = 64;
  char buffer [bufferSize];
  _snprintf(buffer, bufferSize,
    "{%08x-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
    guid->Data1, guid->Data2, guid->Data3, guid->Data4 [0], guid->Data4 [1],
    guid->Data4 [2], guid->Data4 [3], guid->Data4 [4],
    guid->Data4 [5], guid->Data4 [6], guid->Data4 [7]);

  int nLen =(int) strlen(buffer);
  int nRequired = MultiByteToWideChar(CP_ACP, 0, buffer,  nLen, NULL, 0);
  wchar_t* wcBuffer =(wchar_t*) malloc(nRequired);
  MultiByteToWideChar(CP_ACP, 0, buffer, nLen, wcBuffer, nRequired);

  return(wcBuffer);
}

HRESULT RegisterCOMServer(GUID* guid, HINSTANCE serverInstance, LPCTSTR desc)
{
  if((guid == NULL) ||(desc == NULL)) return(E_POINTER);
  if(serverInstance == INVALID_HANDLE_VALUE) return(E_INVALIDARG);

  // convert the guid to string format that we can write out
  LPTSTR guidString = GUIDToString(guid);

  LONG lRet;
  HKEY key;
  const int bufSize = 512;
  TCHAR buff [bufSize];
  
  // create CLSID key for this server
  wsprintf(buff, L"CLSID\\%s", guidString);
  free(guidString);

  lRet = RegCreateKeyEx(HKCR, buff, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL);
  if(lRet != ERROR_SUCCESS ) return HRESULT_FROM_WIN32(lRet);

  // The default value of the key is a human-readable description of the coclass.
  lRet = RegSetValueEx(key, NULL, 0, REG_SZ,(Byte*) desc, lstrlen(desc));
  if(lRet != ERROR_SUCCESS ) return HRESULT_FROM_WIN32(lRet);
  RegCloseKey(key);
    
  // Create the InProcServer32 key, which holds info about our coclass.
  lstrcat(buff, L"\\InProcServer32");
  lRet = RegCreateKeyEx(HKCR, buff, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL);
  if(lRet != ERROR_SUCCESS ) return HRESULT_FROM_WIN32(lRet);

  // The default value of the InProcServer32 key holds the full path to our DLL.
  /*lRet = */GetModuleFileName(serverInstance, buff, bufSize);
  //if(lRet != ERROR_SUCCESS ) return HRESULT_FROM_WIN32(lRet);

  lRet = RegSetValueEx(key, NULL, 0, REG_SZ,(Byte*) buff, lstrlen(buff));
  if(lRet != ERROR_SUCCESS ) return HRESULT_FROM_WIN32(lRet);

  // set the threading model for our server
  lstrcpy(buff, L"Apartment");
  lRet = RegSetValueEx(key, L"ThreadingModel", 0, REG_SZ,(Byte*) buff, lstrlen(buff));
  if(lRet != ERROR_SUCCESS ) return HRESULT_FROM_WIN32(lRet);
  RegCloseKey(key);

  return(NOERROR);
}

HRESULT UnregisterCOMServer(GUID* guid)
{
  if(guid == NULL) return(E_POINTER);

  // convert the guid to string format that we can write out
  LPTSTR guidString = GUIDToString(guid);
  const int bufSize = 512;
  TCHAR buff [bufSize];

  wsprintf(buff, L"CLSID\\%s\\InProcServer32", guidString);
  free(guidString);
  RegDeleteKey(HKCR, buff);

  wsprintf(buff, L"CLSID\\%s", guidString);
  RegDeleteKey(HKCR, buff);

  return(NOERROR);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// UnknownImpl 

UnknownImpl::UnknownImpl() : m_RefCount(0)
{
  AddRef();
}

UnknownImpl::~UnknownImpl()
{
}

DEFINE_QUERYINTERFACE_BEGIN(UnknownImpl)
DEFINE_QUERYINTERFACE_CHECK(IID_IUnknown, IUnknown)
DEFINE_QUERYINTERFACE_ADDREF()
DEFINE_QUERYINTERFACE_END()

static int activeInterfaces = 0;

int UnknownImpl::getActiveInterfacesCount() 
{ 
  return activeInterfaces; 
}

ICALL(ULONG) UnknownImpl::AddRef()
{
  InterlockedIncrement((LONG*) &m_RefCount);
  InterlockedIncrement((LONG*) &activeInterfaces);
  return(m_RefCount);
}

ICALL(ULONG) UnknownImpl::Release()
{
  // just in case we've already been deleted... 
  if(m_RefCount <= 0) return(0);

  //
  InterlockedDecrement((LONG*) &m_RefCount);
  InterlockedDecrement((LONG*) &activeInterfaces);

  // delete ourselves if necessary
  if(m_RefCount <= 0) 
  {
    delete this;
    return(0);
  }

  return(m_RefCount);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// ClassFactory

ClassFactory::ClassFactory(GUID* classId) : UnknownImpl(),
  m_Locked(false), m_ClassId(classId)
{

}

ClassFactory::~ClassFactory()
{
}

DEFINE_QUERYINTERFACE(ClassFactory, UnknownImpl, IID_IClassFactory, IClassFactory)
DEFINE_ADDREF(ClassFactory)
DEFINE_RELEASE(ClassFactory)

ICALL(HRESULT) ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject)
{
  // be sure the destination pointer is valid
  if(!ppvObject) return(E_POINTER);

  // we don't support aggregation so return error if caller expects us to
  if(pUnkOuter) return(CLASS_E_NOAGGREGATION);

  // check if we're a registered class at all...
  if(!m_ClassId) return(E_NOINTERFACE);

  // create an instance via the object manager
  //*ppvObject = ph.CreateObject(m_ClassId,(GUID*) &riid);
  return((*ppvObject) ? NOERROR : E_NOINTERFACE);
}

ICALL(HRESULT) ClassFactory::LockServer(BOOL fLock)
{
  m_Locked = fLock;
  return(NOERROR);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// registration wrapper class

RegisteredClass::RegisteredClass(GUID& guid, lpFnCreateObject createProc)
{
  memcpy(&m_GUID, &guid, sizeof(GUID));
  m_CreateProc = createProc;
}

RegisteredClass::~RegisteredClass()
{
}

IUnknown* RegisteredClass::create(GUID* guid)
{
  return (*m_CreateProc)(guid == NULL ? m_GUID : *guid);
}

bool RegisteredClass::hasGUID(GUID& guid)
{
  return IsEqualGUID(guid, m_GUID) == 0;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// RegisteredClasses

RegisteredClasses::RegisteredClasses(GUID& guid)
{
  memcpy(&m_GUID, &guid, sizeof(GUID));
}

RegisteredClasses::~RegisteredClasses()
{
  clear();
}

bool RegisteredClasses::hasGUID(GUID& guid)
{
  return IsEqualGUID(guid, m_GUID) == 0;
}

RegisteredClass* RegisteredClasses::get(GUID& guid)
{
  std::vector<RegisteredClass*>::iterator iter = m_Classes.begin();
  while(iter != m_Classes.end())
  {
    if((*iter)->hasGUID(guid))
      return *iter;
    iter++;
  }

  return NULL;
}

void RegisteredClasses::add(GUID& guid, lpFnCreateObject proc)
{
  if(get(guid)) 
    m_Classes.push_back(new RegisteredClass(guid, proc)); 
}

void RegisteredClasses::remove(GUID& guid)
{
  std::vector<RegisteredClass*>::iterator iter = m_Classes.begin();
  while(iter != m_Classes.end())
  {
    if((*iter)->hasGUID(guid))
    {
      m_Classes.erase(iter);
      delete *iter;
      break;
    }
    iter++;
  }
}

void RegisteredClasses::clear()
{
  std::vector<RegisteredClass*>::iterator iter = m_Classes.begin();
  while(iter != m_Classes.end())
  {
    m_Classes.erase(iter);
    iter++;
  }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// RegisteredProviders 

RegisteredProviders::RegisteredProviders()
{
}

RegisteredProviders::RegisteredProviders(const RegisteredProviders&)
{
}

RegisteredProviders& RegisteredProviders::operator=(const RegisteredProviders&)
{
  return Instance();
}

RegisteredProviders& RegisteredProviders::Instance()
{
  static RegisteredProviders instance;
  return instance;
}

void RegisteredProviders::initialize()
{
  // nothing to do since the list is now static
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// IT IS VERY IMPORTANT THAT THIS IS CALLED TO DO CLEANUPS!
void RegisteredProviders::shutdown()
{
  clear();
}

void RegisteredProviders::clear()
{
  std::vector<RegisteredClasses*>::iterator iter = m_Providers.begin();
  while(iter != m_Providers.end())
  {
    m_Providers.erase(iter);
    iter++;
  }
}

RegisteredClasses* RegisteredProviders::getProvider(GUID& guid)
{
  std::vector<RegisteredClasses*>::iterator iter = m_Providers.begin();
  while(iter != m_Providers.end())
  {
    if((*iter)->hasGUID(guid)) return *iter;
    iter++;
  }

  return NULL;
}

RegisteredClasses* RegisteredProviders::add(GUID& guid)
{
  RegisteredClasses* classList = find(guid);
  if(!classList)
  {
    classList = new RegisteredClasses(guid);
    m_Providers.push_back(classList);
  }

  return classList;
}

void RegisteredProviders::add(GUID& guid, GUID& classId, lpFnCreateObject proc)
{
  RegisteredClasses* provider = add(guid);
  provider->add(classId, proc);
}

void RegisteredProviders::remove(GUID& guid)
{
  std::vector<RegisteredClasses*>::iterator iter = m_Providers.begin();
  while(iter != m_Providers.end())
  {
    if((*iter)->hasGUID(guid)) 
    {
      m_Providers.erase(iter);
      break;
    }
    iter++;
  }
}

void RegisteredProviders::remove(GUID& guid, GUID& classId)
{
  RegisteredClasses* provider = find(guid);
  if(provider != NULL)
    provider->remove(classId);
}

bool RegisteredProviders::isClassRegistered(GUID& guid, GUID& classId)
{
  RegisteredClasses* provider = getProvider(guid);
  if(!provider) return false;
  return provider->get(classId) != NULL;
}

IUnknown* RegisteredProviders::create(GUID& guid, GUID& classId)
{
  RegisteredClasses* provider = getProvider(guid);
  if(!provider) return NULL;
  RegisteredClass* wrapper = provider->get(classId);
  if(!wrapper) return NULL;

  return wrapper->create(&classId);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Miscellaneous 

IUnknown* __stdcall CreateIUnknown(GUID* guid)
{
  IUnknown* unk = NULL;
  if(IsEqualGUID(*guid, IID_IUnknown)) unk = static_cast<IUnknown*>(new UnknownImpl());

  return(unk);
}

IUnknown* __stdcall CreateIClassFactory(GUID* guid)
{
  IUnknown* unk = NULL;
  if(IsEqualGUID(*guid, IID_IClassFactory)) 
  {
    ClassFactory* cf = new ClassFactory((GUID*) &IID_IClassFactory);
    IClassFactory* fac = static_cast<IClassFactory*>(cf);
	fac->QueryInterface(IID_IUnknown, (void**) &unk);
	fac->Release();
	delete cf;
  }
  return(unk);
}


