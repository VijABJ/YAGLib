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
 * @file  GjTemplates.h
 * @brief Template Declarations
 *
 * Standard templates to be used here, there, and everywhere.
 *
 */
#ifndef GJ_GJ_TEMPLATES_HEADER
#define GJ_GJ_TEMPLATES_HEADER

#include "GjDefs.h"

namespace yaglib 
{

/**
 * the basic observer/observable templates
 */
template<class T>
class ObserverBasic
{
public:
  virtual void update(T& subject) = 0;
};

template<class T>
class SamePointer : public std::unary_function<T*, bool>
{
public:
  explicit SamePointer(T* ptrToCheck) : mPtrToCheck(ptrToCheck) { };
  bool operator()(T* item) { return !item && !mPtrToCheck && (item == mPtrToCheck); };
private:
  T* mPtrToCheck;
};

template<class T>
class SubjectBasic
{
public:
  SubjectBasic() {};
  virtual ~SubjectBasic() {};

  class Invoke : public std::unary_function<T&, void>
  {
  public:
    explicit Invoke(T& host) : mHost(host) {};
    void operator()(ObserverBasic<T>* observer) { observer->update(mHost); };
  private:
    T& mHost;
  };

  void attach(ObserverBasic<T>* observer) 
  {
    mObservers.push_back(observer);
  };
  void detach(ObserverBasic<T>* observer)
  {
    if(!mObservers.empty())
      mObservers.erase(std::remove_if(mObservers.begin(), mObservers.end(), 
        SamePointer<ObserverBasic <T> >(observer)), mObservers.end());
  };
  void notify()
  {
    std::for_each(mObservers.begin(), mObservers.end(), Invoke(static_cast<T&>(*this)));
  };

private:
  typedef std::vector<ObserverBasic<T>*> ObserverList;
  ObserverList mObservers;
};

/**
 * observer/observable with passed ids
 */
template<class T>
class ObserverWithIds
{
public:
  virtual void update(T& subject, const int id) = 0;
};

template<class T>
class SubjectWithIds
{
public:
  SubjectWithIds() {};
  virtual ~SubjectWithIds() {};

  class Invoke : public std::unary_function<T&, void>
  {
  public:
    explicit Invoke(T& host, const int id) : mHost(host), mId(id) {};
    void operator()(ObserverWithIds<T>* observer) { observer->update(mHost, mId); };
  private:
    T& mHost;
    int mId;
  };

  void attach(ObserverWithIds<T>* observer) 
  {
    mObservers.push_back(observer);
  };
  void detach(ObserverWithIds<T>* observer)
  {
    if(!mObservers.empty())
      mObservers.erase(std::remove_if(mObservers.begin(), mObservers.end(), 
        SamePointer<ObserverWithIds <T> >(observer), mObservers.end()));
  };
  void notify(const int id)
  {
    std::for_each(mObservers.begin(), mObservers.end(), Invoke(static_cast<T&>(*this), id));
  };

private:
  typedef std::vector<ObserverWithIds<T>*> ObserverList;
  ObserverList mObservers;
};

/**
 * observer/observable with event push extensions
 */
template<class T, class _Aspect>
class ObserverEx
{
public:
  virtual void update(T& subject, const _Aspect aspect) = 0;
};

template<class T, class _Aspect, _Aspect _globalValue>
class SubjectEx
{
public:
  SubjectEx() {};
  virtual ~SubjectEx() 
  { 
    clear();
  };

  static const _Aspect GLOBAL_ASPECT = _globalValue;

  void attach(ObserverEx<T, _Aspect>* observer, const _Aspect aspect) 
  {
    ObserverList* observers = find(aspect);
    if(observers == NULL)
    {
      // there are no observers of this aspect yet, so create one
      // and place it in the observer map table
      observers = new ObserverList();
      mObservers[aspect] = observers;
    }
    else
    {
      // check to be sure we don't have this listener yet!
      // because if it happens to be registered more than once
      // to observe the same aspect, then bad things (tm) 
      // could happen!  true, we can just NOT insert the 
      // observer if it's already in the list.  but deleting
      // an existing copy, and then proceeding to re-insert
      // it anyway has two side-effects:
      // (1) the code is clean, not too much conditionals
      // (2) the order of observers changes
      remove(observers, observer);
    }

    observers->push_back(observer);
  };
  void detach(ObserverEx<T, _Aspect>* observer, const _Aspect aspect)
  {
    ObserverList* observers = find(aspect);
    if(observers != NULL)
      remove(observers, observer);
  };
  void notify(const _Aspect aspect)
  {
    // do the usual notify gig
    internalNotify(find(aspect), aspect);

    // if the aspect specified is not the global one, then
    // fire off another notify sequence
    if(aspect != GLOBAL_ASPECT)
      internalNotify(find(GLOBAL_ASPECT), aspect);
  };
private:
  typedef std::vector<ObserverEx<T, _Aspect>*> ObserverList;
  typedef std::map<_Aspect, ObserverList*> ObserverMap;

  ObserverMap mObservers;
  ObserverList mGlobalObservers;

protected:
  class Invoke : public std::unary_function<T&, void>
  {
  public:
    explicit Invoke(T& host, const _Aspect aspect) : mHost(host), mAspect(aspect) {};
    void operator()(ObserverEx<T, _Aspect>* observer) { observer->update(mHost, mAspect); };
  private:
    T& mHost;
    _Aspect mAspect;
  };

  class Cleanup : public std::unary_function<std::pair<_Aspect, ObserverList*>, void>
  {
  public:
    explicit Cleanup() {};
    void operator()(std::pair<_Aspect, ObserverList*> item) { delete item.second; };
  };

  void internalNotify(ObserverList* observers, const _Aspect aspect)
  {
    if(observers != NULL)
      std::for_each(observers->begin(), observers->end(), Invoke(static_cast<T&>(*this), aspect));
  };
  void clear()
  {
    std::for_each(mObservers.begin(), mObservers.end(), Cleanup());
    mObservers.clear();
    mGlobalObservers.clear();
  };
  void remove(ObserverList* observers, ObserverEx<T, _Aspect>* observer)
  {
    if(!observers->empty())
      observers->erase(std::remove_if(observers->begin(), observers->end(), 
        SamePointer<ObserverEx <T, _Aspect> >(observer)));
  };
  ObserverList* find(const _Aspect aspect)
  {
    if(aspect == GLOBAL_ASPECT)
      return &mGlobalObservers;
    else
    {
      if(!mObservers.empty() && (mObservers.find(aspect) != mObservers.end()))
        return mObservers[aspect];
      else
        return NULL;
      //ObserverMap::iterator p = mObservers.find(aspect);
      //return (p == mObservers.end()) ? NULL : p->second;
    }
  };
};

/* a 2d-grid, doesn't exactly mimic a 2d-array.  this is supposed to serve as 
   a base class for grid-like objects.  dynamic resizing is not provided (yet)
*/
template<class T>
class Grid2D
{
public:
  Grid2D(const int xSize, const int ySize = 0) 
  {
    assert(xSize > 0);
    mWidth = xSize;
    mHeight = ySize > 0 ? ySize : xSize;
    mTotalCells = mWidth * mHeight;
    mBuffer = new T[mTotalCells];
    memset(mBuffer, 0, sizeof(T) * mTotalCells);
  };
  virtual ~Grid2D()
  {
    delete [] mBuffer;
  };
  inline int getWidth() 
  {
    return mWidth;
  };
  inline int getHeight()
  {
    return mHeight;
  };
  inline int getTotalCells()
  {
    return mTotalCells;
  };
  inline void clear(T value)
  {
    for(int i = 0; i < mTotalCells; i++)
      mBuffer[i] = value;
  };
  inline bool valid(const int x, const int y)
  {
    return (x >= 0) && (x < mWidth) && (y >= 0) && (y < mHeight);
  };
  inline T& operator()(const int x, const int y)
  {
    return mBuffer[(y * mWidth) + x];
  };

private:
  int mWidth;
  int mHeight;
  int mTotalCells;
  T* mBuffer;
};

/* a vector of pointers to objects.  do not provide the pointer operator
   when instancing from this template.  

   that is, instead of: -> ObjectList<DataType*>
   use ->  ObjectList<DataType>
*/
template<class T>
class ObjectList
{
public:
  typedef std::vector<T*> _listType;
  typedef typename _listType::iterator iterator;
  typedef typename _listType::const_iterator const_iterator;
  _listType mItems;

  iterator begin()
  {
    return mItems.begin();
  };
  iterator end()
  {
    return mItems.end();
  }

  ObjectList() {};
  virtual ~ObjectList() { clear(); };
  size_t size() const { return mItems.size(); };

  bool indexValid(const int index) const
  {
    return (index >= 0) && (index < (int)mItems.size());
  };
  const_iterator find(const int index) const 
  {
    if(indexValid(index))
    {
      const_iterator iter = mItems.begin();
      for(int _at = 0; _at != index; iter++, _at++);
      return iter;
    }
    else
      return mItems.end();
  };
  const_iterator find(T* item) const
  {
    for(const_iterator iter = mItems.begin(); iter != mItems.end(); iter++)
    {
      if(*iter == item)
        return iter;
    }
    return mItems.end();
  };
  int indexOf(T* item) const
  {
    int index = 0;
    for(const_iterator iter = mItems.begin(); iter != mItems.end(); iter++)
    {
      if(*iter == item)
        return index;
      ++index;
    }
    return -1;
  };
  bool isFirst(T* item) const
  {
    if(mItems.size() > 0)
    {
      const_iterator iter = find(0);
      return *iter == item;
    }
    
    return false;
  };
  bool isLast(T* item) const
  {
    if(mItems.size() > 0)
    {
      const_iterator iter = find((int)(mItems.size()-1));
      return *iter == item;
    }

    return false;
  };
  void erase(const int index)
  {
    iterator iter = indexToIterator(index);
    if(iter != mItems.end())
    {
      delete *iter;
      mItems.erase(iter);
    }
  };
  // remove will ONLY remove (ugh!) the item from the list.  the
  // item itself will NOT be freed.  instead, it is returned to
  // the caller.  this could be useful for re-ordering the items
  // already in the list  this is the only list-manipulation
  // method that do not free the item it's supposed to take
  // away from the list.  (contrast: erase(), clear())
  T* remove(const int index)
  {
    T* item = NULL;
    iterator iter = indexToIterator(index);

    if(iter != mItems.end())
    {
      item = *iter;
      mItems.erase(iter);
    }

    return item;
  };
  T* remove(T* item)
  {
    return remove(indexOf(item));
  };
  void clear()
  {
    for(iterator iter = mItems.begin(); iter != mItems.end(); iter++)
    {
      delete *iter;
      *iter = NULL;
    }

    mItems.clear();
  };
  void add(T* item)
  {
    if(!exists(item))
      mItems.push_back(item);
  };
  void append(T* item)
  {
    add(item);
  };
  void prepend(T* item)
  {
    if(!exists(item))
      mItems.insert(mItems.begin(), item);
  };
  bool exists(T* item)
  {
    return find(item) != mItems.end();
  };
  T* get(const int index) const
  {
    const_iterator iter = find(index);
    if(iter != mItems.end())
      return *iter;
    else
      return NULL;
  };
  T* operator[](const int index) const
  {
    return get(index);
  };
private:
  iterator indexToIterator(const int index)
  {
    if(!indexValid(index))
      return mItems.end();
    //
    int i = index;
    iterator iter = mItems.begin();
    for( ; i > 0; iter++, i--);
    return iter;
  }
};

// from game programming gems. this singleton originally by Scott Bilas
template <typename T> 
class Singleton
{
private:
  static T* mSingleton;
public:
  Singleton()
  {
    assert(!mSingleton);
    DWORD_PTR offset = (DWORD_PTR)(T*)1 - (DWORD_PTR)(Singleton <T>*)(T*)1;
    mSingleton = (T*)((DWORD_PTR)this + offset);
  }
  ~Singleton()
  {  
    assert(mSingleton);  
    mSingleton = NULL;  
  }
  static T& Instance()
  {  
    assert(mSingleton);  
    return *mSingleton;  
  }
  static T* InstancePtr()
  {  
    return mSingleton;  
  }
};

template <typename T> T* Singleton <T>::mSingleton = NULL;

// visitor/visitable
template<class T>
class visitor
{
public:
  virtual void visit(T& subject) = 0;
};

template<class T>
class visitable
{
public:
  virtual void accept(visitor<T>& visitor) = 0;
};

} /* namespace yaglib */

#endif /* GJ_GJ_TEMPLATES_HEADER */
