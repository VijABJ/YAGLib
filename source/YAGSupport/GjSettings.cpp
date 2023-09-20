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

#include "GjSettings.h"
#include "GjUnicodeUtils.h"
#include "GjStringUtils.h"
#include <boost/algorithm/string.hpp>
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
using namespace yaglib;

Setting::Setting(const WideString& name, const WideString& value) : mPair(name, value)
{
}

Setting::Setting(const WideString& value) : mPair(string_utils::make_string_pair(value, '='))
{
}

Setting::Setting(const StringPair& pair) : mPair(pair)
{
}

const WideString& Setting::getName() const 
{ 
  return mPair.first; 
}

const WideString& Setting::getValue() const 
{ 
  return mPair.second; 
}

void Setting::setValue(const WideString& newValue) 
{ 
  mPair.second = newValue; 
}

//
Settings::Settings(const WideString name, const bool acceptDuplicates) :
  mName(name), mAcceptDuplicates(acceptDuplicates)
{
}

// note: if we already have a setting with the same name, and duplicates
//       are not allowed, then the previous one WILL be changed instead
void Settings::add(Setting setting)
{
  using namespace boost;
  using namespace boost::lambda;
  _listType::iterator result = std::find_if(mItems.begin(), mItems.end(), _1 == setting.getName());
  if((result != mItems.end()) && !mAcceptDuplicates)
    result->setValue(setting.getValue());
  else
    mItems.push_back(setting);
}

void Settings::add(StringPair pair)
{
  add(Setting(pair));
}

void Settings::add(const WideString value)
{
  add(Setting(value));
}

void Settings::add(const WideString name, const WideString value)
{
  add(Setting(name, value));
}

void Settings::clear()
{
  mItems.clear();
}

void Settings::erase(const WideString name, const bool allCopies)
{
  using namespace boost;
  using namespace boost::lambda;
  if(allCopies)
    std::remove_if(mItems.begin(), mItems.end(), _1 == name);
  else
  {
    _listType::iterator result = std::find_if(mItems.begin(), mItems.end(), _1 == name);
    if(result != mItems.end())
      mItems.erase(result);
  }
}

bool Settings::exists(const WideString& name) const
{
  using namespace boost;
  using namespace boost::lambda;
  _listType::const_iterator result = std::find_if(mItems.begin(), mItems.end(), _1 == name);
  return result != mItems.end();
}

size_t Settings::size() const
{
  return mItems.size();
}

Setting* Settings::get(const WideString name) 
{
  using namespace boost;
  using namespace boost::lambda;
  _listType::iterator result = std::find_if(mItems.begin(), mItems.end(), _1 == name);
  if(result != mItems.end())
    return &(*result);
  else
    return NULL;
}

Setting* Settings::get(const int index) 
{
  if((index >= 0) && (index < static_cast<int>(mItems.size())))
    return &(mItems[index]);
  else
    return NULL;
}

Setting& Settings::operator[](const WideString name) 
{
  Setting* item = get(name);
  assert(item != NULL);
  return *item;
}

Setting& Settings::operator[](const int index)
{
  Setting* item = get(index);
  assert(item != NULL);
  return *item;
}

WideString const& Settings::getName() const
{
  return mName;
}

bool Settings::duplicatesOk() const
{
  return mAcceptDuplicates;
}


MultipleSettings::MultipleSettings(const bool acceptDuplicates) : mAcceptDuplicates(acceptDuplicates)
{
}

Settings& MultipleSettings::operator[](const WideString name)
{
  using namespace boost;
  using namespace boost::lambda;
  MultiSettings::iterator result = std::find_if(mItems.begin(), mItems.end(), _1 == name);
  if(result == mItems.end())
  {
    mItems.push_back(Settings(name, mAcceptDuplicates));
    result = mItems.end()-1;
  }

  return *result;
}

Settings& MultipleSettings::operator[](const int index)
{
  assert((index >= 0) && (index < static_cast<int>(mItems.size())));
  return mItems[index];
}

Settings* MultipleSettings::get(const WideString name, const bool forceCreate)
{
  using namespace boost;
  using namespace boost::lambda;
  MultiSettings::iterator result = std::find_if(mItems.begin(), mItems.end(), _1 == name);
  if((result == mItems.end()) && forceCreate)
  {
    mItems.push_back(Settings(name, mAcceptDuplicates));
    result = mItems.end()-1;
  }

  return result != mItems.end() ? &(*result) : NULL;
}

Settings* MultipleSettings::get(const int index)
{
  if((index < 0) || (index >= static_cast<int>(mItems.size())))
    return NULL;

  return &(mItems[index]);
}

bool MultipleSettings::exists(const WideString groupName) const
{
  using namespace boost;
  using namespace boost::lambda;
  MultiSettings::const_iterator result = std::find_if(mItems.begin(), mItems.end(), _1 == groupName);
  return result != mItems.end();
}

bool MultipleSettings::duplicatesOk() const
{
  return mAcceptDuplicates;
}

size_t MultipleSettings::size() const
{
  return mItems.size();
}

void MultipleSettings::clear()
{
  mItems.clear();
}

Setting* MultipleSettings::find(const WideString group, const WideString name) 
{
  using namespace boost;
  using namespace boost::lambda;
  MultiSettings::iterator result = std::find_if(mItems.begin(), mItems.end(), _1 == group);
  return (result == mItems.end()) ? NULL : result->get(name);
}

WideString MultipleSettings::read(const WideString group, const WideString name, const WideString defVal)
{
  Setting const* item = find(group, name);
  if(item)
    return item->getValue();
  else
    return defVal;
}

int MultipleSettings::read(const WideString group, const WideString name, const int defVal)
{
  Setting const* item = find(group, name);
  if(item)
    return string_utils::parse_int(item->getValue(), defVal);
  else
    return defVal;
}

double MultipleSettings::read(const WideString group, const WideString name, const double defVal)
{
  Setting const* item = find(group, name);
  if(item)
    return string_utils::parse_double(item->getValue(), defVal);
  else
    return defVal;
}

bool MultipleSettings::read(const WideString group, const WideString name, const bool defVal)
{
  Setting const* item = find(group, name);
  if(item)
    return string_utils::parse_bool(item->getValue(), defVal);
  else
    return defVal;
}


void GlobalSettings::loadFrom(SettingsStore& storage, const WideString objectName, const bool clearCurrentValues)
{
  if(clearCurrentValues)
    mSettings.clear();

  storage.load(objectName, mSettings);
}

void GlobalSettings::saveTo(SettingsStore& storage, const WideString objectName)
{
  storage.save(objectName, mSettings);
}

