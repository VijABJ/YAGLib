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
 * @file  _GjIntercepts.h
 * @brief Intercept is basically a callback that binds user code to the framework
 *
 */
#ifndef __GJ_INTERCEPTS_HEADER
#define __GJ_INTERCEPTS_HEADER

#include "GjNativeDefs.h"
#include <assert.h>

namespace yaglib
{

struct InterceptProxy
{
  InterfaceIntercept mIntercept;
  WideString mTag;
  bool mAcceptsForward;

  InterceptProxy(InterfaceIntercept intercept, const WideString& tag, 
    const bool acceptsForward = false) :
    mIntercept(intercept), mTag(tag), mAcceptsForward(acceptsForward)
  {
  };

  const bool operator==(const WideString& name)
  {
    return mTag == name;
  };
};

class InterceptProxies
{
public:
  InterceptProxies();
  virtual ~InterceptProxies();

  void add(InterfaceIntercept intercept, const WideString& name, const bool acceptsForward = false);
  void remove(const WideString& name);

  void operator()(NativeApplicationEvent event, NATIVE_WINDOW_HANDLE handle, void* params);
  const size_t size() const;

private:
  typedef std::vector<InterceptProxy> ProxyList;
  ProxyList mProxies;
};

} /* namespace yaglib */

#endif /* __GJ_INTERCEPTS_HEADER */
