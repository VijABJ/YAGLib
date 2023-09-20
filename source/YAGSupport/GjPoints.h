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
 * @file  GjPoints.h
 * @brief Templates for point classes
 *
 * Point class as a template
 *
 */

#ifndef GJ_POINT_TEMPLATES_HEADER
#define GJ_POINT_TEMPLATES_HEADER

#include "GjDefs.h"

namespace yaglib
{

template<class T>
class point
{
public:
  T x;
  T y;

  explicit point(const T value = 0)
  {
    x = y = value;
  };
  point(const POINT& p)
  {
    x = (T) p.x;
    y = (T) p.y;
  };
  point(const point<T>& p)
  {
    x = p.x; y = p.y;
  };
  point(const T _x, const T _y)
  {
    x = _x; y = _y;
  };
  point<T>& operator=(const point<T>& p)
  {
    if(&p != this)
    {
      x = p.x;
      y = p.y;
    }
    return *this;
  };
  point<T>& operator=(const T value)
  {
    x = y = value;
    return *this;
  };
  point<T>& operator+=(const point& p)
  {
    x += p.x;
    y += p.y;
    return *this;
  };
  point<T>& operator+=(const T value)
  {
    x += value;
    y += value;
    return *this;
  };
  point<T>& operator-=(const point& p)
  {
    x -= p.x;
    y -= p.y;
    return *this;
  };
  point<T>& operator-=(const T value)
  {
    x -= value;
    y -= value;
    return *this;
  };
  point<T>& operator*=(const point& p)
  {
    x *= p.x;
    y *= p.y;
    return *this;
  };
  point<T>& operator*=(const T value)
  {
    x *= value;
    y *= value;
    return *this;
  };
  point<T>& operator/=(const point& p)
  {
    if((p.x != 0) && (p.y != 0))
    {
      x /= p.x;
      y /= p.y;
    }
    return *this;
  };
  point<T>& operator/=(const T value)
  {
    if(value != 0)
    {
      x /= value;
      y /= value;
    }
    return *this;
  };
  point<T> operator+(const point<T>& p) const
  {
    return point(x + p.x, y + p.y);
  };
  point<T> operator+(const T value) const
  {
    return point(x + value, y + value);
  };
  point<T> operator-(const point<T>& p) const
  {
    return point(x - p.x, y - p.y);
  };
  point<T> operator-(const T value) const
  {
    return point(x - value, y - value);
  };
  point<T> operator*(const point<T>& p) const
  {
    return point(x * p.x, y * p.y);
  };
  point<T> operator*(const T value) const
  {
    return point(x * value, y * value);
  };

  bool operator==(const point<T>& p)
  {
    return (x == p.x) && (y == p.y);
  };
  bool operator!=(const point<T>& p)
  {
    return (x != p.x) || (y != p.y);
  };
  bool operator==(const T value)
  {
    return (x == value) && (y == value);
  };
  bool operator!=(const T value)
  {
    return (x != value) || (y != value);
  };
  operator POINT()
  {
    POINT p;
    p.x = (int) x;
    p.y = (int) y;
    return p;
  };
};

typedef point<int> Point;
typedef point<float> PointF;
typedef point<double> PointEx;
typedef point<GJFLOAT> GJPOINT;

////////////////////////////

template<class T>
class size
{
public:
  T width;
  T height;

  size(const POINT& p)
  {
    width = (T) p.x;
    height = (T) p.y;
  };
  size(const T value = 0)
  {
    width = height = 0;
  };
  size(const T _width, const T _height)
  {
    width = _width;
    height = _height;
  };
  size<T>& extend(const T hDelta, const T vDelta)
  {
    width += hDelta;
    height += vDelta;
    return *this;
  };
  bool operator==(const size<T>& sx)
  {
    return (width == sx.width) && (height == sx.height);
  };
  bool operator!=(const size<T>& sx)
  {
    return (width != sx.width) || (height != sx.height);
  };
};

typedef size<int> Size;
typedef size<float> SizeF;
typedef size<double> SizeEx;
typedef size<GJFLOAT> GJSIZE;

////////////////////////////

template<class T>
class point_triple
{
public:
  T x;
  T y;
  T z;

  point_triple(const T value = 0)
  {
    x = y = z = value;
  };
  point_triple(const POINT& p)
  {
    x = (T) p.x;
    y = (T) p.y;
    z = (T) 0;
  };
  point_triple(const point_triple& p)
  {
    x = p.x; 
    y = p.y;
    z = p.z;
  };
  point_triple(const T _x, const T _y, const T _z)
  {
    x = _x; 
    y = _y;
    z = _z;
  };
  point_triple& operator=(const point_triple& p)
  {
    if(&p != this)
    {
      x = p.x;
      y = p.y;
      z = p.z;
    }
    return *this;
  };
  point_triple& operator=(const T value)
  {
    x = y = z = value;
    return *this;
  };
  point_triple& operator+=(const point_triple& p)
  {
    x += p.x;
    y += p.y;
    z += p.z;
    return *this;
  };
  point_triple& operator+=(const T value)
  {
    x += value;
    y += value;
    z += value;
    return *this;
  };
  point_triple& operator-=(const point_triple& p)
  {
    x -= p.x;
    y -= p.y;
    z -= p.z;
    return *this;
  };
  point_triple& operator-=(const T value)
  {
    x -= value;
    y -= value;
    z -= value;
    return *this;
  };
  point_triple& operator*=(const point_triple& p)
  {
    x *= p.x;
    y *= p.y;
    z *= p.z;
    return *this;
  };
  point_triple& operator*=(const T value)
  {
    x *= value;
    y *= value;
    z *= value;
    return *this;
  };
  point_triple& operator/=(const point_triple& p)
  {
    if((p.x != 0) && (p.y != 0) && (p.z != 0))
    {
      x /= p.x;
      y /= p.y;
      z /= p.z;
    }
    return *this;
  };
  point_triple& operator/=(const T value)
  {
    if(value != 0)
    {
      x /= value;
      y /= value;
      z /= value;
    }
    return *this;
  };
  point_triple operator+(const point_triple& p) const
  {
    return point_triple(x + p.x, y + p.y, z + p.z);
  };
  point_triple operator+(const T value) const
  {
    return point_triple(x + value, y + value, z + value);
  };
  point_triple operator-(const point_triple& p) const
  {
    return point_triple(x - p.x, y - p.y, z - p.z);
  };
  point_triple operator-(const T value) const
  {
    return point_triple(x - value, y - value, z - value);
  };

  bool operator==(const point_triple& p)
  {
    return (x == p.x) && (y == p.y) && (z == p.z);
  };
  bool operator!=(const point_triple& p)
  {
    return (x != p.x) || (y != p.y) || (z != p.z);
  };
  bool operator==(const T value)
  {
    return (x == value) && (y == value) && (z == value);
  };
  bool operator!=(const T value)
  {
    return (x != value) || (y != value) || (z != value);
  };
  operator POINT()
  {
    POINT p;
    p.x = (int) x;
    p.y = (int) y;
    return p;
  };
};

typedef point_triple<int> Point3;
typedef point_triple<float> Point3F;
typedef point_triple<double> Point3Ex;
typedef point_triple<GJFLOAT> GJPOINT3;

}; /* namespace yaglib */

#endif /* GJ_POINT_TEMPLATES_HEADER */
