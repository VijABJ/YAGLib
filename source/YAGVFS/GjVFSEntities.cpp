
#include "GjVFSEntities.h"
#include "GjUnicodeUtils.h"
using namespace yaglib;
using namespace yaglib::vfs;

Entity::Entity(const WideString& name, Entities* container) :
  m_Name(name), m_Container(container), m_QualifiedName(L"")
{
}

Entity::~Entity()
{
  // detach us from our container if necessary
  if(m_Container != NULL)
    m_Container->detach(this);
}

const WideString& Entity::getQualifiedName()
{
  if(m_QualifiedName.length() == 0)
    assembleQualifiedName();

  return m_QualifiedName;
}

void Entity::setName(const WideString& newName)
{
  if(canChangeToName(newName))
  {
    m_Name = newName;
    nameChanged();
  }
}

void Entity::assembleQualifiedName()
{
  if((m_Container == NULL) || (m_Container->getOwner() == NULL))
    m_QualifiedName = m_Name;
  else
  {
    Entity* owner = const_cast<Entity*>(m_Container->getOwner());
    m_QualifiedName = owner->getQualifiedName() +
      WideString(PATH_SEPARATOR) + m_Name;
  }
}

PByte Entity::Load(int offset, size_t size)
{
  Entity* owner = const_cast<Entity*>(getOwner());
  if(owner != NULL)
    return owner->Load(offset, size);

  return NULL;
}

const Entity* Entity::getOwner() const
{
  return (m_Container != NULL) ? m_Container->getOwner() : NULL;
};

int Entity::getIdSize()
{
  // the default size will be the length of our name, and the size of an int
  // this is because we will save the length of our name in the stream later
  UTF8String utf8(m_Name);
  return utf8.c_len() + sizeof(int) + 1;
}

void Entity::saveId(std::ofstream& dest, int& offset)
{
  int length = 0;
  if(m_Name.size() > 0)
  {
    UTF8String utf8(m_Name);
    length = utf8.c_len();
    length++;
    dest.write((char*)&length, sizeof(int));

    char* data = const_cast<char*>(utf8.c_str());
    dest.write(data, length);
  }
  else
  {
    dest.write((char*)&length, sizeof(int));
  }
}

Entity* Entity::loadId(std::ifstream& source)
{
  int length;

  source.read((char*)&length, sizeof(int));
  if(length)
  {
    char* data = new char[length];
    source.read(data, length);

    UTF8String utf8(data);
    utf8.ToWideString(m_Name);

    delete [] data;
  }
  else
    m_Name = L"";

  return this;
}

void Entity::saveData(std::ofstream& dest)
{
  // the default entity does not have any data
}

void Entity::containerChanged()
{
  assembleQualifiedName();
}

void Entity::nameChanged()
{
  assembleQualifiedName();
}

bool Entity::nameAvailable(const WideString& nameToCheck)
{
  // this is called, or will be called, by sub-items!
  return true;
}

bool Entity::canChangeToName(const WideString& nameToCheck)
{
  // this is called by ourselves, so we can centralize name-checking.
  // the default is to ask the container about it, if there is a
  // container currently holding us.
  if(m_Container == NULL)
    return true;

  return m_Container->nameAvailable(nameToCheck);
}

// Entities
int Entities::getIndex(Entity* entity)
{
  int index = 0;
  for(iterator iter = begin(); iter != end(); iter++, index++)
    if((*iter) == entity) return index;

  return -1;
}

int Entities::getIndex(const WideString& name)
{
  int index = 0;
  for(iterator iter = begin(); iter != end(); iter++, index++)
    if((*iter)->getName() == name) return index;

  return -1;
}

Entity* Entities::find(const WideString& name)
{
  for(iterator iter = begin(); iter != end(); iter++)
    if((*iter)->getName() == name) return *iter;

  return NULL;
}

Entity* Entities::find(int index) 
{
  for(iterator iter = begin(); iter != end(); iter++, index--)
    if(index <= 0) return *iter;

  return NULL;
}


void Entities::attach(Entity* entity)
{
  if(getIndex(entity) < 0)
  {
    m_Items.push_back(entity);
    entity->m_Container = this;
    entity->containerChanged();
  }
}

void Entities::detach(Entity* entity, bool free)
{
  detach(getIndex(entity), free);
}

void Entities::detach(WideString& name, bool free)
{
  detach(getIndex(name), free);
}

void Entities::detach(int index, bool free)
{
  // reject bad indices
  if((index < 0) || (index >= (int)m_Items.size()))
    return;

  // loop to the item to be deleted
  iterator iter = begin();
  while(index)
  {
    index--;
    iter++;
  }

  // set entity container to NULL, to prevent infinite
  // invocation of detach()
  (*iter)->m_Container = NULL;

  // recover memory if requested
  if(free)
    delete *iter;
  else
  {
    (*iter)->m_Container = NULL;
    (*iter)->containerChanged();
  }

  // remove from the list
  m_Items.erase(iter);
}

void Entities::clear()
{
  for(iterator iter = begin(); iter != end(); iter++)
  {
    (*iter)->m_Container = NULL;
    delete *iter;
  }

  m_Items.clear();
}

bool Entities::nameAvailable(const WideString& nameToCheck)
{
  if(m_Owner == NULL)
    return true;

  return m_Owner->nameAvailable(nameToCheck);
}

const Entities* Entity::getContainer() const 
{ 
  return m_Container; 
}

const WideString& Entity::getName() const 
{ 
  return m_Name; 
}

const bool Entity::isFolder() const
{
  return false;
}

const Entity* Entities::getOwner() const 
{ 
  return m_Owner; 
}

Entities::iterator Entities::begin() 
{ 
  return m_Items.begin(); 
}

Entities::iterator Entities::end() 
{ 
  return m_Items.end(); 
}

const size_t Entities::size() const
{ 
  return m_Items.size(); 
}

