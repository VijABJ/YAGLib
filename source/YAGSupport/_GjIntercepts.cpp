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

#include "_GjIntercepts.h"
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
using namespace yaglib;

InterceptProxies::InterceptProxies()
{
}

InterceptProxies::~InterceptProxies()
{
}

void InterceptProxies::add(InterfaceIntercept intercept, 
  const WideString& name, const bool acceptsForward)
{
  mProxies.push_back(InterceptProxy(intercept, name, acceptsForward));
}

bool checkProxyName(const WideString& name, const InterceptProxy& proxy)
{
  return proxy.mTag == name;
}

void InterceptProxies::remove(const WideString& name)
{
  using namespace boost;
  using namespace boost::lambda;
  mProxies.erase(std::remove_if(mProxies.begin(), mProxies.end(), _1 == name));
}

class Invoke : public std::unary_function<InterceptProxy, void>
{
public:
  explicit Invoke(NativeApplicationEvent event, NATIVE_WINDOW_HANDLE handle, void* params) :
    mEvent(event), mHandle(handle), mParams(params), mIsForwarded(event == Forwarded)
    {};
  void operator()(InterceptProxy& proxy) 
  { 
    if(mIsForwarded && !proxy.mAcceptsForward) return;
    proxy.mIntercept(mEvent, mHandle, mParams);
  };
private:
  NativeApplicationEvent mEvent;
  NATIVE_WINDOW_HANDLE mHandle;
  void* mParams;
  bool mIsForwarded;
};

void InterceptProxies::operator()(NativeApplicationEvent event, NATIVE_WINDOW_HANDLE handle, void* params)
{
  Invoke inv(event, handle, params);
  std::for_each(mProxies.begin(), mProxies.end(), inv);
}

const size_t InterceptProxies::size() const
{
  return mProxies.size();
}