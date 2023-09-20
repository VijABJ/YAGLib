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

#include "GjTileMap.h"
#include "GjUnicodeUtils.h"
#include "GjStringUtils.h"
#include <boost/algorithm/string.hpp>
using namespace yaglib;

TerrainManager::TerrainManager(MultipleSettings& settings) 
{
  loadIsoSettings(settings);
}

TerrainManager::~TerrainManager()
{
}

void TerrainManager::loadIsoSettings(MultipleSettings& settings)
{
  if(settings.exists(ISOMETRIC_SECTION_INDICES))
  {
    Settings section = settings[ISOMETRIC_SECTION_INDICES];
    for(int i = 0; i < static_cast<int>(section.size()); i++)
    {
      Setting& item = section[i];
      mIndices[item.getName()] = _wtoi(item.getValue().c_str());
    }
  }

  if(settings.exists(ISOMETRIC_SECTION_TERRAIN))
  {
    Settings section = settings[ISOMETRIC_SECTION_TERRAIN];
    stringsList items;
    for(int i = 0; i < static_cast<int>(section.size()); i++)
    {
      Setting& item = section[i];
      items.clear();
      WideString value = item.getValue();

      boost::split(items, value, boost::is_any_of(L","), boost::token_compress_on);
      process(item.getName(), items);
    }
  }

  mTileSize = Size(64, 31);
  if(settings.exists(ISOMETRIC_SECTION_SETTINGS))
  {
    Settings section = settings[ISOMETRIC_SECTION_SETTINGS];
    if(section.exists(L"width"))
      mTileSize.width = string_utils::parse_int(section[L"width"].getValue());
    if(section.exists(L"height"))
      mTileSize.height = string_utils::parse_int(section[L"height"].getValue());
  }
}

void TerrainManager::process(const WideString name, stringsList& items)
{
  Terrain t;
  if(items.size() < 2)
    items.push_back(L"1");

  t.name = name;
  t.index = (int) mItems.size();
  t.frameIndex = getTerrainIndex(name);
  t.type = (TerrainType) string_utils::parse_int(items[0]);
  t.modifier = string_utils::parse_int(items[1]);

  mItems.push_back(t);
}

size_t TerrainManager::size() const
{
  return mItems.size();
}

Size TerrainManager::getTileSize() const
{
  return mTileSize;
}

bool TerrainManager::exists(const int index) const
{
  return (index >= 0) && (index < (int) mItems.size());
}

bool TerrainManager::exists(WideString name) const
{
  return indexOf(name) >= 0;
}

int TerrainManager::indexOf(WideString name) const
{
  int index = 0;
  for(TerrainList::const_iterator iter = mItems.begin(); iter != mItems.end(); iter++, index++)
    if(iter->name == name)
      return index;

  return -1;
}

int TerrainManager::getTerrainIndex(WideString name) const
{
  TerrainIndices::const_iterator iter = mIndices.empty() ? mIndices.end() : mIndices.find(name);
  return iter != mIndices.end() ? iter->second : -1;
}

Terrain const& TerrainManager::operator()(const int index) const
{
  return mItems[index];
}


////
BasicMap::BasicMap(const int width/*x*/, const int breadth/*y*/, TerrainManager* tm) :
  mWidth(width), mBreadth(breadth), mTerrainManager(tm)
{
  addLevel();
}

BasicMap::~BasicMap()
{
  mLevels.clear();
}

int BasicMap::getWidth() const
{
  return mWidth;
}

int BasicMap::getBreadth() const
{
  return mBreadth;
}

TerrainManager const& BasicMap::getTerrainManager() const
{
  return *mTerrainManager;
}

int BasicMap::getCurrentLevelCount() const
{
  return (int) mLevels.size();
}

BasicLevel* BasicMap::operator()(const int levelNumber) const
{
  if((levelNumber >= 0) && (levelNumber < (int) mLevels.size()))
    return mLevels[levelNumber];
  else
    return NULL;
}

int BasicMap::addLevel()
{
  BasicLevel* level = new BasicLevel(*this, (int) mLevels.size());
  mLevels.append(level);
  return (int) mLevels.size();
}


///////////////////
BasicLevel::BasicLevel(BasicMap& mapOwner, const int level) :
  Grid2D<BasicTile*>(mapOwner.getWidth(), mapOwner.getBreadth()),  
  mMap(mapOwner), mLevel(level)
{
  Grid2D<BasicTile*>::clear(NULL);
}

BasicLevel::~BasicLevel()
{
  clear();
}

BasicMap& BasicLevel::getMap()
{
  return mMap;
}

void BasicLevel::clear()
{
  for(int x = 0; x < getWidth(); x++)
    for(int y = 0; y < getHeight(); y++)
    {
      BasicTile* tile = (*this)(x, y);
      if(tile) delete tile;
      (*this)(x, y) = NULL;
    }
}

BasicTile* BasicLevel::add(const int x, const int y, const int terrainId)
{
  if(!valid(x, y))
    return NULL;

  BasicTile* tile = (*this)(x, y);
  if(tile == NULL)
  {
    tile = new BasicTile(this, Point3(x, y, mLevel));
    (*this)(x, y) = tile;
  }
  tile->setTerrainId(terrainId);

  return tile;
}

void BasicLevel::fillLevel(const int terrainId)
{
  if(terrainId < 0)
  {
    int tCap = static_cast<int>(mMap.getTerrainManager().size());
    for(int x = 0; x < getWidth(); x++)
      for(int y = 0; y < getHeight(); y++)
      {
        BasicTile* tile = add(x, y, rand() % tCap);
        // debug code
        //if((rand() % 100) <= 20)
        //  tile->setFloorId(rand() % 50);
      }
  }
  else
    for(int x = 0; x < getWidth(); x++)
      for(int y = 0; y < getHeight(); y++)
        add(x, y, terrainId);
}

////////////
BasicTile::BasicTile(BasicLevel* levelOwner, const Point3& location) : mLevel(levelOwner), 
  mLocation(location), mTerrainId(-1), mTerrainFrameIndex(-1), mFloorId(-1)
{
  mDecal.name = L"";
  mDecal.id = -1;
}

BasicLevel* BasicTile::getLevel() const
{
  return mLevel;
}

Point3 const& BasicTile::getLocation() const
{
  return mLocation;
}

bool BasicTile::isNeighbor(BasicTile* tile) const
{
  return (tile != NULL) && (tile != this) && (mLevel == tile->mLevel) &&
    (abs(mLocation.x - tile->mLocation.x) <= 1) &&
    (abs(mLocation.y - tile->mLocation.y) <= 1);
}

static Point displacement[] = 
{
  Point(0, -1), // north
  Point(1, -1), // north-east
  Point(1, 0),  // east
  Point(1, 1),  // south-east
  Point(0, 1),  // south
  Point(-1, 1), // south-west
  Point(-1, 0), // west
  Point(-1, -1) // north-west
};

bool BasicTile::hasNeighbor(const Direction where) const
{
  Point p = Point(mLocation.x, mLocation.y) + displacement[where];
  return mLevel->valid(p.x, p.y);
}

BasicTile* BasicTile::getNeighbor(const Direction where) const
{
  Point p = Point(mLocation.x, mLocation.y) + displacement[where];
  return (mLevel->valid(p.x, p.y)) ? (*mLevel)(p.x, p.y) : NULL;
}

int BasicTile::getTerrainId() const
{
  return mTerrainId;
}

Terrain const& BasicTile::getTerrain() const
{
  return mLevel->mMap.getTerrainManager()(mTerrainId);
}

void BasicTile::setTerrainId(const int tId)
{
  mTerrainId = tId;
  if(tId <= 0)
  {
    mFloorId = -1;
    mTerrainFrameIndex = -1;
  }
  else
    mTerrainFrameIndex = mLevel->mMap.getTerrainManager()(mTerrainId).frameIndex;
}

int BasicTile::getFloorId() const
{
  return mFloorId;
}
void BasicTile::setFloorId(const int floorId)
{
  if(mTerrainId > 0)
    mFloorId = floorId;
}

Decal const& BasicTile::getDecal() const
{
  return mDecal;
}

void BasicTile::setDecal(WideString& decalName, const int id)
{
  mDecal.name = decalName;
  mDecal.id = id;
}

bool BasicTile::canStandIn() const
{
  return (mTerrainId > 1) || (mFloorId >= 0);
}

bool BasicTile::canWalkThru() const
{
  return false;
}

bool BasicTile::canFloatThru() const
{
  return false;
}


