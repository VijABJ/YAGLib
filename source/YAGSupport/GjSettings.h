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
 * @file  GjSettings.h
 * @brief Settings classes/structs
 *
 */
#ifndef GJ_SETTINGS_UTILS_HEADER
#define GJ_SETTINGS_UTILS_HEADER

#include "GjDefs.h"
#include "GjTemplates.h"
#include <utility>

namespace yaglib 
{

class Settings;
typedef std::pair<WideString, WideString> StringPair;

class Setting
{
public:
  Setting(const WideString& name, const WideString& value);
  explicit Setting(const WideString& value);
  explicit Setting(const StringPair& pair);

  const WideString& getName() const;
  const WideString& getValue() const;
  void setValue(const WideString& newValue);

  const bool operator==(const WideString& name) const
  {
    return mPair.first == name;
  };

private:
  StringPair mPair;
};

class Settings 
{
public:
  typedef std::vector<Setting> _listType;
  typedef _listType::iterator iterator;
  typedef _listType::const_iterator const_iterator;

  iterator begin()
  {
    return mItems.begin();
  };
  iterator end()
  {
    return mItems.end();
  }

  Settings(const WideString name, const bool acceptDuplicates = false);

  void add(Setting setting);
  void add(StringPair pair);
  void add(const WideString value);
  void add(const WideString name, const WideString value);

  void clear();
  void erase(const WideString name, const bool allCopies = false);

  bool exists(const WideString& name) const;
  size_t size() const;

  Setting* get(const WideString name);
  Setting* get(const int index);

  Setting& operator[](const WideString name);
  Setting& operator[](const int index);

  WideString const& getName() const;
  bool duplicatesOk() const;

  const bool operator==(const WideString& name) const
  {
    return mName == name;
  };

private:
  WideString mName;
  _listType mItems;
  bool mAcceptDuplicates;
};

class MultipleSettings
{
public:
  MultipleSettings(const bool acceptDuplicates = true);

  Settings& operator[](const WideString name);
  Settings& operator[](const int index);

  Settings* get(const WideString name, const bool forceCreate = true);
  Settings* get(const int index);

  bool exists(const WideString groupName) const;
  bool duplicatesOk() const;
  size_t size() const;
  void clear();

  WideString read(const WideString group, const WideString name, const WideString defVal);
  int read(const WideString group, const WideString name, const int defVal);
  double read(const WideString group, const WideString name, const double defVal);
  bool read(const WideString group, const WideString name, const bool defVal);

private:
  typedef std::vector<Settings> MultiSettings;
  MultiSettings mItems;
  bool mAcceptDuplicates;

  Setting* find(const WideString group, const WideString name);
};

class SettingsStore
{
public:
  virtual void load(const WideString& objectName, MultipleSettings& settings) = 0;
  virtual void save(const WideString& objectName, MultipleSettings& settings) = 0;
};


// a singleton that does not automatically construct and cleanup
class GlobalSettings : public Singleton<GlobalSettings>
{
public:
  void loadFrom(SettingsStore& storage, const WideString objectName, const bool clearCurrentValues = true);
  void saveTo(SettingsStore& storage, const WideString objectName);

  MultipleSettings& getSettings() { return mSettings; };

private:
  MultipleSettings mSettings;
};

#define g_GlobalSettings (GlobalSettings::Instance())


} /* namespace yaglib */

#endif /* GJ_SETTINGS_UTILS_HEADER */
