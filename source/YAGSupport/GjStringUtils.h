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
 * @file  GjStringUtils.h
 * @brief Additional string utilities, follows boost::string_algo conventions
 *
 */
#ifndef GJ_STRING_UTILS_HEADER
#define GJ_STRING_UTILS_HEADER

#include <string>
#include <utility>
#include <boost/algorithm/string.hpp>

namespace yaglib
{

namespace string_utils
{

template<class StringType, class CharType> void squeeze(StringType& input, const CharType suspect)
{
  size_t pos = input.find(suspect);
  while(pos != StringType::npos)
  {
    input.erase(pos, 1);
    pos = input.find(suspect);
  }
}

template<class StringType, class CharType> StringType squeeze_copy(
  const StringType& input, const CharType suspect)
{
  StringType result(input);
  squeeze<StringType, CharType>(result, suspect);
  return result;
}

template<class StringType> void squeeze(StringType& input, const StringType& suspect)
{
  size_t pos = input.find(suspect);
  size_t len = suspect.size();
  while(pos != StringType::npos)
  {
    input.erase(pos, len);
    pos = input.find(suspect);
  }
}

template<class StringType> StringType squeeze_copy(const StringType& input, const StringType& suspect)
{
  StringType result(input);
  squeeze<StringType>(result, suspect);
  return result;
}

template<class StringType> void strip_left(StringType& input, const size_t lastPosition)
{
  if(lastPosition != StringType::npos)
    input.erase(0, lastPosition);
}

template<class StringType> StringType strip_left_copy(const StringType& input, const size_t lastPosition)
{
  String result(input);
  strip_left<StringType>(result, lastPosition);
  return result;
}

template<class StringType> void strip_right(StringType& input, const size_t firstPosition)
{
  if(firstPosition != StringType::npos)
    input.erase(firstPosition, input.size());
}

template<class StringType> StringType strip_right_copy(const StringType& input, const size_t firstPosition)
{
  StringType result(input);
  strip_right<StringType>(result, firstPosition);
  return result;
}

template<class StringType> void chop_before(StringType& input, const StringType& target)
{
  strip_left<StringType>(input, input.find_first_of(target));
}

template<class StringType> StringType chop_before_copy(const StringType& input, const StringType& target)
{
  return strip_left_copy<StringType>(input, input.find_first_of(target));
}

template<class StringType> void chop_after_first(StringType& input, 
  const StringType& target, const bool includeTarget = false)
{
  size_t pos = input.find_first_of(target);
  if(pos != StringType::npos)
  {
    size_t firstPosition = includeTarget ? pos : pos + target.size();
    strip_right<StringType>(input, firstPosition);
  }
}

template<class StringType> StringType chop_after_first_copy(const StringType& input, 
  const StringType& target, const bool includeTarget = false)
{
  size_t pos = input.find_first_of(target);
  if(pos != StringType::npos)
  {
    size_t firstPosition = includeTarget ? pos : pos + target.size();
    return strip_right_copy<StringType>(input, firstPosition);
  }
  return StringType(input);
}

template<class StringType> void chop_after_last(StringType& input, 
  const StringType& target, const bool includeTarget = false)
{
  size_t pos = input.find_last_of(target);
  if(pos != StringType::npos)
  {
    size_t firstPosition = includeTarget ? pos + target.size() : pos;
    strip_right<StringType>(input, firstPosition);
  }
}

template<class StringType> StringType chop_after_last_copy(const StringType& input, 
  const StringType& target, const bool includeTarget = false)
{
  size_t pos = input.find_last_of(target);
  if(pos != StringType::npos)
  {
    size_t firstPosition = includeTarget ? pos + target.size() : pos;
    return strip_right_copy<StringType>(input, firstPosition);
  }
  return StringType(input);
}

template<class StringType, class CharType> void conditional_append(
  StringType& input, const CharType c)
{
  if((input.size() == 0) || (*(--input.end()) != c))
    input.push_back(c);
}

template<class StringType, class CharType> StringType conditional_append_copy(
  const StringType& input, const CharType c)
{
  StringType dest(input);
  if((dest.size() == 0) || (*(--dest.end()) != c))
    dest.push_back(c);

  return dest;
}

template<class StringType, class CharType> std::pair<StringType,StringType> 
  make_string_pair(const StringType& input, const CharType separator)
{
  std::pair<StringType, StringType> _result(L"", L"");
  size_t equalChar = input.find(separator);
  if(equalChar != StringType::npos)
  {
    _result.first = input.substr(0, equalChar);
    _result.second = input.substr(equalChar+1);
  }
  else
    _result.first = input;

  using namespace boost;

  trim(_result.first);
  trim(_result.second);
  return _result;
}

template<class StringType> bool parse_bool(const StringType& input, const bool default_value = false)
{
  return (input.size() == 0) ? default_value :
    ((input[0] == '1') || (input[0] == 't') || (input[0] == 'T') || (input[0] == 'y') || (input[0] == 'Y'));
}

template<class StringType> int parse_int(const StringType& input, const int default_value = 0)
{
  int result = default_value;
  _stscanf(input.c_str(), L"%d", &result);
  return result;
}

template<class StringType> double parse_double(const StringType& input, const double default_value = 0.0)
{
  double result = default_value;
  _stscanf(input.c_str(), L"%f", &result);
  return result;
}

template<class StringType> int parse_hex_int(const StringType& input, const int default_value = 0)
{
  int result = default_value;
  _stscanf(input.c_str(), L"%x", &result);
  return result;
}

template<class StringType> StringType to_s(const int value)
{
  char buf[32];
  sprintf(buf, "%d", value);
  return StringType(buf);
}

template<class StringType> StringType to_s(const double value)
{
  char buf[32];
  sprintf(buf, "%f", value);
  return StringType(buf);
}

template<class StringType> StringType to_s(const bool value)
{
  return value ? StringType(L"true") : StringType(L"false");
}

} /* namespace string_utils */

} /* namespace yaglib */

#endif /* GJ_STRING_UTILS_HEADER */
