
#include "GjVFSFolder.h"
using namespace yaglib;
using namespace yaglib::vfs;

Folder::Folder(const WideString& name, Entities* container) :
  Entity(name, container), m_Folders(NULL), m_Files(NULL)
{
  m_Folders = new Folders(this);
  m_Files = new Files(this);
}

Folder::~Folder()
{
  delete m_Folders;
  delete m_Files;
}

void Folder::clear()
{
  m_Folders->clear();
  m_Files->clear();
}

void Folder::propagateIdentityChange()
{
  for(Entities::iterator iter = m_Folders->begin(); iter != m_Folders->end(); iter++)
    (*iter)->assembleQualifiedName();

  for(Entities::iterator iter = m_Files->begin(); iter != m_Files->end(); iter++)
    (*iter)->assembleQualifiedName();
}

void Folder::containerChanged()
{
  propagateIdentityChange();
}

void Folder::nameChanged()
{
  Entity::nameChanged();
  propagateIdentityChange();
}

bool Folder::nameAvailable(const WideString& nameToCheck)
{
  return (m_Folders->find(nameToCheck) == NULL) &&
         (m_Files->find(nameToCheck) == NULL);
}

void Folder::attach(Entity* entity)
{
  if(entity != NULL) 
  {
    if(entity->isFolder())
      m_Folders->attach(entity);
    else
      m_Files->attach(entity);
  }
}

void Folder::detach(Entity* entity, bool free)
{
  if(entity != NULL) 
  {
    if(entity->isFolder())
      m_Folders->detach(entity, free);
    else
      m_Files->detach(entity, free);
  }
}

Folder* Folder::createFolder(const WideString& name)
{
  if(!nameAvailable(name)) return NULL;

  Folder* folder = new Folder(name, NULL);
  m_Folders->attach(folder);
  return folder;
}

File* Folder::createFile(const WideString& name)
{
  if(!nameAvailable(name)) return NULL;

  File* file = new File(name, NULL);
  m_Files->attach(file);
  return file;
}

int Folder::getIdSize()
{
  // the 2 ints are for the file and folder count we will store
  int result = Entity::getIdSize() + (sizeof(int) * 2);

  Folders* folders = getFolders();
  for(Entities::iterator iter = folders->begin(); iter != folders->end(); iter++)
    result += (*iter)->getIdSize();

  Files* files = getFiles();
  for(Entities::iterator iter = files->begin(); iter != files->end(); iter++)
    result += (*iter)->getIdSize();

  return result;
}

void Folder::saveId(std::ofstream& dest, int& offset)
{
  Entity::saveId(dest, offset);

  Folders* folders = getFolders();
  int count = (int)folders->size();
  dest.write((char*)&count, sizeof(int));
  for(Entities::iterator iter = folders->begin(); iter != folders->end(); iter++)
    (*iter)->saveId(dest, offset);


  Files* files = getFiles();
  count = (int)files->size();
  dest.write((char*)&count, sizeof(int));
  for(Entities::iterator iter = files->begin(); iter != files->end(); iter++)
    (*iter)->saveId(dest, offset);
}

Entity* Folder::loadId(std::ifstream& source)
{
  Entity::loadId(source);
  int count;

  source.read((char*)&count, sizeof(int));
  if(count > 0)
    for(int i = 0; i < count; i++)
    {
      Folder* folder = createFolder(L"");
      folder->loadId(source);
    }

  source.read((char*)&count, sizeof(int));
  if(count > 0)
    for(int i = 0; i < count; i++)
    {
      File* file = createFile(L"");
      file->loadId(source);
    }

  return this;
}

void Folder::saveData(std::ofstream& dest)
{
  Folders* folders = getFolders();
  for(Entities::iterator iter = folders->begin(); iter != folders->end(); iter++)
    (*iter)->saveData(dest);

  Files* files = getFiles();
  for(Entities::iterator iter = files->begin(); iter != files->end(); iter++)
    (*iter)->saveData(dest);
}

const bool Folder::isFolder() const
{
  return true;
}

Folders* const Folder::getFolders() const
{
  return m_Folders;
}

Files* const Folder::getFiles() const
{
  return m_Files;
}

Files::Files(Entity* owner) : Entities(owner)
{
}

File* Files::operator[](int index)
{
  return dynamic_cast<File*>(find(index));
}

File* Files::operator[](const WideString& name)
{
  return dynamic_cast<File*>(find(name));
}

Folders::Folders(Entity* owner) : Entities(owner)
{
}

Folder* Folders::operator[](int index)
{
  return dynamic_cast<Folder*>(find(index));
}

Folder* Folders::operator[](const WideString& name)
{
  return dynamic_cast<Folder*>(find(name));
}

