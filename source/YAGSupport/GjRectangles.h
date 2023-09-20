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
 * @file  GjRectangles.h
 * @brief Templates for rectangle classes
 *
 * Rectangle class as a template
 *
 */

#ifndef GJ_RECT_TEMPLATES_HEADER
#define GJ_RECT_TEMPLATES_HEADER

#include "GjPoints.h"
#include <cmath>
#include <vector>

// clipped result values
const int CLIPPED_VS_LEFT  = 0x0001;
const int CLIPPED_VS_TOP   = 0x0002;
const int CLIPPED_VS_RIGHT = 0x0004;
const int CLIPPED_VS_BOTTOM= 0x0008;

// results given when querying point position relative to rect
const int POINT_IS_ABOVE   = 0x0001;
const int POINT_IS_BELOW   = 0x0002;
const int POINT_IS_AT_LEFT = 0x0004;
const int POINT_IS_AT_RIGHT= 0x0008;

const int POINT_IS_ABOVE_LEFT   = POINT_IS_ABOVE | POINT_IS_AT_LEFT;
const int POINT_IS_ABOVE_RIGHT  = POINT_IS_ABOVE  | POINT_IS_AT_RIGHT;
const int POINT_IS_BELOW_LEFT   = POINT_IS_BELOW | POINT_IS_AT_LEFT;
const int POINT_IS_BELOW_RIGHT  = POINT_IS_BELOW | POINT_IS_AT_RIGHT;

namespace yaglib
{

template<class T>
class rectangle
{
public:
  T left;
  T top;
  T right;
  T bottom;

  T width;
  T height;

  rectangle()
  {
    left = top = right = bottom = width = height = 0;
  };
  rectangle(const rectangle<T>& r)
  {
    left = r.left;
    top = r.top;
    right = r.right;
    bottom = r.bottom;
    recalcDimensions();
  };
  rectangle(const RECT& r)
  {
    left = r.left;    top = r.top;
    right = r.right;  bottom = r.bottom;
    recalcDimensions();
  };
  rectangle(const T _left, const T _top, const T _right = 0, const T _bottom = 0)
  {
    left = _left;   top = _top;
    right = _right; bottom = _bottom;
    recalcDimensions();
  };
  rectangle(const point<T>& topLeft, const T _width, const T _height)
  {
    left = topLeft.x;   
    top = topLeft.y;
    right = left + _width;
    bottom = top + _height;

    width = _width;
    height = _height;
  };
  rectangle(const point<T>& topLeft, const point<T>& bottomRight)
  {
    left = topLeft.x;
    top = topLeft.y;
    right = bottomRight.x;
    bottom = bottomRight.y;
    recalcDimensions();
  };
  point<T> getTopLeft() const
  {
    return point<T>(left, top);
  };
  point<T> getTopRight() const
  {
    return point<T>(right, top);
  };
  point<T> getBottomLeft() const
  {
    return point<T>(left, bottom);
  };
  point<T> getBottomRight() const
  {
    return point<T>(right, bottom);
  };
  rectangle<T>& normalize()
  {
    right -= left;
    bottom -= top;
    left = top = 0;
    return *this;
  };
  rectangle<T>& zero()
  {
    left = top = right = bottom = width = height = 0;
    return *this;
  }
  int whereIs(point<T>& p) const 
  {
    int result = 0;
    if(p.x < left)
      result |= POINT_IS_AT_LEFT;
    else if(p.x > right)
      result |= POINT_IS_AT_RIGHT;
    if(p.y < top)
      result |= POINT_IS_ABOVE;
    else if(p.y > bottom)
      result |= POINT_IS_BELOW;

    return result;
  };
  int clip(point<T>& p) const
  {
    int result = 0;
    if(p.x < left)
    {
      p.x = left;
      result |= CLIPPED_VS_LEFT;
    }
    if(p.x > right)
    {
      p.x = right;
      result |= CLIPPED_VS_RIGHT;
    }
    if(p.y < top)
    {
      p.y = top;
      result |= CLIPPED_VS_TOP;
    }
    if(p.y > bottom)
    {
      p.y = bottom;
      result |= CLIPPED_VS_BOTTOM;
    }
    return result;
  };
  int clip(point_triple<T>& p) const
  {
    int result = 0;
    if(p.x < left)
    {
      p.x = left;
      result |= CLIPPED_VS_LEFT;
    }
    if(p.x > right)
    {
      p.x = right;
      result |= CLIPPED_VS_RIGHT;
    }
    if(p.y < top)
    {
      p.y = top;
      result |= CLIPPED_VS_TOP;
    }
    if(p.y > bottom)
    {
      p.y = bottom;
      result |= CLIPPED_VS_BOTTOM;
    }
    return result;
  };
  // returns true if line was eliminated entirely
  bool clip(point<T>& p1, point<T>& p2) const
  {
    // optimization. do special cases first.
    // (0) if we're a null rect, everything gets rejected!
    if(isNull())
      return true;
    // (1) horizontal line
    if(p1.y == p2.y)
    {
      if((p1.y < top) || (p1.y > bottom))
        return true;
      if(p1.x < left)
        p1.x = left;
      if(p2.x > right)
        p2.x = right;
      return (p2.x <= p1.x);
    }
    // (2) vertical lines
    if(p1.x == p2.x)
    {
      if((p1.x < left) || (p1.x > right))
        return true;
      if(p1.y < top)
        p1.y = top;
      if(p2.y > bottom)
        p2.y = bottom;
      return (p2.y <= p1.y);
    }
    // (3) the hard part
    int lp1 = whereIs(p1), lp2 = whereIs(p2);
    do
    {
      // trivially accept if both are not out of bounds
      if((lp1 == 0) && (lp2 == 0))
        return false;
      // trivially reject if both points are in the same area
      if((lp1 & lp2) != 0)
        return true;
      // damn, we have to do clipping! look for special cases first.
      int pCode = (lp1 != 0) ? lp1 : lp2;
      point<T>* p = (lp1 != 0) ? &p1 : &p2;
      double slope = (double)(p2.x - p1.x)/(double)(p2.y - p1.y);
      //
      // TODO: the int casts below are only appropriate for when <T> is int. fix this later
      if((pCode & POINT_IS_ABOVE) != 0)
      {
        p->x = static_cast<T>(static_cast<int>((p2.x - (p2.y - top)*(slope))));
        p->y = top;
      }
      else if((pCode & POINT_IS_BELOW) != 0)
      {
        p->x = static_cast<T>(static_cast<int>((p2.x - (p2.y - bottom)*(slope))));
        p->y = bottom;
      }
      else if((pCode & POINT_IS_AT_LEFT) != 0)
      {
        p->y = static_cast<T>(static_cast<int>((p2.y - (p2.x - left)*(1/slope))));
        p->x = left;
      }
      else if((pCode & POINT_IS_AT_RIGHT) != 0)
      {
        p->y = static_cast<T>(static_cast<int>((p2.y - (p2.x - right)*(1/slope))));
        p->x = right;
      }

      if(lp1 != 0)
        lp1 = whereIs(p1);
      else
        lp2 = whereIs(p2);

    } while(true);

    return false;
  };
  int clip(rectangle<T>& r) const
  {
    int result = 0;
    // clipped horizontally (left/right)
    if(r.left < left)
    {
      r.left = left;
      result |= CLIPPED_VS_LEFT;
    } 
    if(r.right > right)
    {
      r.right = right;
      result |= CLIPPED_VS_RIGHT;
    }
    // clipped vertically (top/bottom)
    if(r.top < top)
    {
      r.top = top;
      result |= CLIPPED_VS_TOP;
    }
    if(r.bottom > bottom)
    {
      r.bottom = bottom;
      result |= CLIPPED_VS_BOTTOM;
    }
    // have r adjust if we clipped it
    if(result)
      r.recalcDimensions();
    // done!
    return result;
  };
  int cage(rectangle<T>& r) const
  {
    int modifications = 0;
    if(r.right > right)
    {
      r.translate_h(right - r.right);
      modifications++;
    }
    if(r.bottom > bottom)
    {
      r.translate_v(bottom - r.bottom);
      modifications++;
    }
    if(r.left < left)
    {
      r.translate_h(left - r.left);
      modifications++;
    }
    if(r.top < top)
    {
      r.translate_v(top - r.top);
      modifications++;
    }
    return modifications;
  };
  bool contains(const point<T>& p) const
  {
    return (p.x >= left) && (p.x <= right) &&
            (p.y >= top) && (p.y <= bottom);
  };
  bool outside(const rectangle<T>& r)
  {
    return (r.bottom <= top) || (r.right <= left) ||
           (r.top >= bottom) || (r.left >= right);
  };
  rectangle<T> intersection(const rectangle<T>& r) const
  {
    T _left = max(left, r.left);
    T _top = max(top, r.top);
    T _right = min(right, r.right);
    T _bottom = min(bottom, r.bottom);
    // corrections.  this is intentionally left
    // invalid in order to distinguish between
    // adjacency and plainly too distant
    if(_right < _left)
      _right = _left - 1;
    if(_bottom < _top)
      _bottom = _top - 1;

    return rectangle<T>(_left, _top, _right, _bottom);

  };
  point<T> centerOf() const
  {
    return point<T>(left + width / 2, top + height / 2);
  };
  bool overlaps(const rectangle<T>& r) const
  {
    return !(intersection(r).isNull());
  };
  bool overlaps(const rectangle<T>& r, rectangle<T>& rDest) const
  {
    rDest = intersection(r);
    return !rDest.isNull();
  };
  rectangle<T>& moveTo(const point<T> p)
  {
    left = p.x;  right = left + width;
    top = p.y;   bottom = top + height;
    return *this;
  };
  bool isNull() const
  {
    return (width <= 0) || (height <= 0);
  };
  operator RECT() const
  {
    RECT r;
    r.left  = static_cast<int>(left);
    r.top   = static_cast<int>(top);
    r.right = static_cast<int>(right);
    r.bottom= static_cast<int>(bottom);
    return r;
  };
  rectangle<T>& translate_h(const T value)
  {
    left += value;
    right += value;
    return *this;
  };
  rectangle<T>& translate_v(const T value)
  {
    top += value;
    bottom += value;
    return *this;
  };          
  rectangle<T>& translate(const T x, const T y)
  {
    translate_h(x);
    translate_v(y);
    return *this;
  };
  rectangle<T>& translate(const POINT& p)
  {
    translate(static_cast<T>(p.x), static_cast<T>(p.y));
    return *this;
  };
  rectangle<T>& translate(const point<T> p)
  {
    translate(p.x, p.y);
    return *this;
  };
  rectangle<T>& expand(const T value)
  {
    left -= value;
    top -= value;
    right += value;
    bottom += value;
    recalcDimensions();
    return *this;
  };
  rectangle<T>& resize(const T value)
  {
    right += value;
    bottom += value;
    recalcDimensions();
    return *this;
  };
  rectangle<T>& resize(const T dx, const T dy)
  {
    right += dx;
    bottom += dy;
    recalcDimensions();
    return *this;
  };
  rectangle<T>& scale(const T dx, const T dy)
  {
    left *= dx;
    top *= dy;
    right *= dx;
    bottom *= dy;
    recalcDimensions();
    return *this;
  };
  rectangle<T>& operator+=(const point<T> value)
  {
    left += value.x;
    top += value.y;
    right += value.x;
    bottom += value.y;
    return *this;
  };
  rectangle<T>& operator+=(const T value)
  {
    left += value;
    top += value;
    right += value;
    bottom += value;
    return *this;
  };
  rectangle<T>& operator-=(const point<T> value)
  {
    left -= value.x;
    top -= value.y;
    right -= value.x;
    bottom -= value.y;
    return *this;
  };
  rectangle<T>& operator-=(const T value)
  {
    left -= value;
    top -= value;
    right -= value;
    bottom -= value;
    return *this;
  };
  rectangle<T>& operator*=(const point<T> value)
  {
    left *= value.x;
    top *= value.y;
    right *= value.x;
    bottom *= value.y;
    recalcDimensions();
    return *this;
  };
  rectangle<T>& operator*=(const T value)
  {
    left *= value;
    top *= value;
    right *= value;
    bottom *= value;
    recalcDimensions();
    return *this;
  };
  rectangle<T>& operator/=(const point<T> value)
  {
    if((value.x != 0) && (value.y != 0))
    {
      left /= value.x;
      top /= value.y;
      right /= value.x;
      bottom /= value.y;
      recalcDimensions();
    }
    return *this;
  };
  rectangle<T>& operator/=(const T value)
  {
    if(value != 0)
    {
      left /= value;
      top /= value;
      right /= value;
      bottom /= value;
      recalcDimensions();
    }
    return *this;
  };
  rectangle<T>& setHeight(const T value)
  {
    height = value;
    bottom = top + height;
    return *this;
  };
  rectangle<T>& setWidth(const T value)
  {
    width = value;
    right = left + width;
    return *this;
  };
  void recalcDimensions()
  {
    width = right - left;
    if(width < 0)
      width = 0;
    height = bottom - top;
    if(height < 0)
      height = 0;
  };
};

typedef rectangle<int> Rect;
typedef rectangle<float> RectF;
typedef rectangle<double> RectEx;
typedef rectangle<GJFLOAT> GJRECT;

template<class T>
class rectlist
{
public:
  typedef std::vector< rectangle<T> > ItemList;
  typedef typename ItemList::iterator iterator;
  typedef typename ItemList::const_iterator const_iterator;

private:
  ItemList mItems;

public:
  rectlist() {};
  virtual ~rectlist() {};

  iterator begin()
  {
    return mItems.begin();
  };
  iterator end()
  {
    return mItems.end();
  };
  void add(const rectangle<T>& r)
  {
    mItems.push_back(r);
  };
  void clear()
  {
    mItems.clear();
  };
  size_t size() const
  {
    return mItems.size();
  };
  rectangle<T> const* get(const int index) const
  {
    if((index >= 0) && (index < (static_cast<int>(mItems.size()))))
      return &(mItems[index]);
    else
      throw std::exception("rectlist: invalid index");
  };
  rectangle<T> const& operator[](const int index) const
  {
    return mItems[index];
  };

};

typedef rectlist<int> RectList;
typedef rectlist<float> RectFList;
typedef rectlist<double> RectExList;
typedef rectlist<GJFLOAT> GJRECTS;

inline GJRECT fromRect(const Rect& r)
{
  return GJRECT(static_cast<GJFLOAT>(r.left), static_cast<GJFLOAT>(r.top),
                static_cast<GJFLOAT>(r.right), static_cast<GJFLOAT>(r.bottom));
}

inline GJRECT fromRectF(const RectF& r)
{
#ifdef USE_HIGH_PRECISION_FLOAT
  return GJRECT(static_cast<GJFLOAT>(r.left), static_cast<GJFLOAT>(r.top),
                static_cast<GJFLOAT>(r.right), static_cast<GJFLOAT>(r.bottom));
#else
  return GJRECT(r);
#endif
}

}; /* namespace gjlib */

#endif /* GJ_RECT_TEMPLATES_HEADER */
