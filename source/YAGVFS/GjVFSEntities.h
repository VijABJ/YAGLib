
#ifndef GJ_VFS_ENTITIES_HEADER
#define GJ_VFS_ENTITIES_HEADER

#include "GjDefs.h"
#include "GjVFSDefs.h"

#include <vector>
#include <fstream>

namespace yaglib
{
  namespace vfs
  {

class Entities;
class Entity;

/** this interface should load the data given the arguments */
class DemandLoader
{
public:
  virtual PByte Load(int offset, size_t size) = 0;
};

class Entity : public DemandLoader
{
public:
  Entity(const WideString& name, Entities* container);
  virtual ~Entity();

  const Entities* getContainer() const;
  const Entity* getOwner() const;

  const WideString& getName() const;
  const WideString& getQualifiedName();

  void setName(const WideString& newName);
  const virtual bool isFolder() const;

  virtual void assembleQualifiedName();
  // supports demand-loading...
  virtual PByte Load(int offset, size_t size);

  // following four functions takes care of data persistence
  virtual int getIdSize();
  virtual void saveId(std::ofstream& dest, int& offset);
  virtual Entity* loadId(std::ifstream& source);
  virtual void saveData(std::ofstream& dest);

protected:
  // state change handlers
  virtual void containerChanged();
  virtual void nameChanged();
  virtual bool nameAvailable(const WideString& nameToCheck);
  bool canChangeToName(const WideString& nameToCheck);

private:
  friend class Entities;

  Entities* m_Container;
  WideString m_Name;
  WideString m_QualifiedName;
};

class Entities
{
public:
  Entities(Entity* owner) : m_Owner(owner) {};

  const Entity* getOwner() const;

  typedef std::vector<Entity*>::iterator iterator;
  iterator begin();
  iterator end();
  const size_t size() const;

  int getIndex(Entity* entity);
  int getIndex(const WideString& name);

  Entity* find(const WideString& name);
  Entity* find(int index);

  void attach(Entity* entity);
  void detach(Entity* entity, bool free = false);
  void detach(WideString& name, bool free = false);
  void detach(int index, bool free = false);

  void clear();

  bool nameAvailable(const WideString& nameToCheck);

private:
  Entity* m_Owner;
  std::vector<Entity*> m_Items;
};

  } /* namespace vfs */
} /* namespace yaglib */


#endif /* GJ_VFS_ENTITIES_HEADER */
