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
 * @file  GjTileMap.h
 * @brief Tiled map abstraction
 *
 */

#ifndef GJ_TILEMAP_HEADER
#define GJ_TILEMAP_HEADER

#include "GjDefs.h"
#include "GjSettings.h"
#include "GjTemplates.h"
#include "GjPoints.h"
#include <vector>

namespace yaglib
{

#define ISOMETRIC_SECTION_SETTINGS  L"isometric-settings"
#define ISOMETRIC_SECTION_TERRAIN   L"isometric-terrain"
#define ISOMETRIC_SECTION_INDICES   L"isometric-tile-indices"

typedef enum Direction
{
  North = 0,    NorthEast,    East,   SouthEast,
  South,        SouthWest,    West,   NorthWest
} Direction;

/*
  Directions legend:

               NorthWest
                  /\
                 /  \ 
            West/    \ North
               /      \
              /        \
   SouthWest <          > NorthEast
              \        /
               \      /
           South\    /East
                 \  /
                  \/
               SouthEast

*/


class BasicMap;
class BasicLevel;
class BasicTile;

typedef enum TerrainType
{
  TERRAIN_VOID = 0,   // empty nothingness...
  TERRAIN_LIMBO,      // no terrain, you better know how to fly
  TERRAIN_TERRA,      // ground. can walk here, possibly.
  TERRAIN_WATER       // water. swimming or floating required
} TerrainType;

typedef struct tagTerrain
{
  WideString name;    // unique name
  int index;          // basically precedence
  int frameIndex;     // frame index to terrain sheet
  TerrainType type;   // type of this terrain
  int modifier;       // for the path-finder
} Terrain;

class TerrainManager 
{
public:
  TerrainManager(MultipleSettings& settings);
  virtual ~TerrainManager();

  size_t size() const;
  Size getTileSize() const;
  bool exists(const int index) const;
  bool exists(WideString name) const;
  int indexOf(WideString name) const;
  Terrain const& operator()(const int index) const;

  int getTerrainIndex(WideString name) const;

private:
  typedef std::vector<Terrain> TerrainList;
  typedef std::map<WideString, int> TerrainIndices;
  TerrainList mItems;
  TerrainIndices mIndices;
  Size mTileSize;

  typedef std::vector<WideString> stringsList;
  void loadIsoSettings(MultipleSettings& settings);
  void process(const WideString name, stringsList& items);
};

class BasicMap
{
public:
  BasicMap(const int width/*x*/, const int breadth/*y*/, TerrainManager* tm);
  virtual ~BasicMap();

  int getWidth() const;
  int getBreadth() const;
  TerrainManager const& getTerrainManager() const;

  int addLevel();
  int getCurrentLevelCount() const;
  BasicLevel* operator()(const int levelNumber) const;

private:
  int mWidth;
  int mBreadth;
  typedef ObjectList<BasicLevel> LevelList;
  LevelList mLevels;
  TerrainManager* mTerrainManager;
};

class BasicLevel : public Grid2D<BasicTile*>
{
  friend class BasicTile;
public:
  BasicLevel(BasicMap& mapOwner, const int level);
  virtual ~BasicLevel();

  BasicMap& getMap();
  BasicTile* add(const int x, const int y, const int terrainId);
  void fillLevel(const int terrainId);

protected:
  BasicMap& mMap;
  int mLevel;

  void clear();
};

typedef struct tagDecal
{
  WideString name;
  int id;
} Decal;

class BasicTile
{
  friend class BasicLevel;
public:
  BasicTile(BasicLevel* levelOwner, const Point3& location);

  BasicLevel* getLevel() const;
  Point3 const& getLocation() const;

  int getTerrainId() const;
  int getTerrainFrameIndex() const { return mTerrainFrameIndex; };
  Terrain const& getTerrain() const;
  void setTerrainId(const int tId);

  int getFloorId() const;
  void setFloorId(const int floorId);

  Decal const& getDecal() const;
  void setDecal(WideString& decalName, const int id);

  bool isNeighbor(BasicTile* tile) const;
  bool hasNeighbor(const Direction where) const;
  BasicTile* getNeighbor(const Direction where) const;

  bool canStandIn() const;
  bool canWalkThru() const;
  bool canFloatThru() const;

  /*
  Passability = walkable/not, obstructs view/not, 
  */

protected:
  BasicLevel* mLevel;
  Point3 mLocation;
  int mTerrainId;
  int mTerrainFrameIndex;
  int mFloorId;
  Decal mDecal;
};

} /* namespace yaglib */

#endif /* GJ_TILEMAP_HEADER */
