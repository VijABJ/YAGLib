
#include "GjMapView.h"
#include "GjKeyCodes.h"
#include "GjResourceManagement.h"
using namespace yaglib;

const double INITIAL_SCROLL_SPEED = 60.0;
const double SCROLL_ACCELERATION  = 10.0;
const double MAX_SCROLL_SPEED     = 160.0;

MapView::MapView(BaseView* owner, const GJRECT& bounds, SpriteFactory* spriteFactory, BasicMap* map) : 
  BaseView(owner, bounds, spriteFactory), mMap(map), 
  mScroller(FourWayScroller(INITIAL_SCROLL_SPEED, SCROLL_ACCELERATION, MAX_SCROLL_SPEED)), 
  mRects(new RectGrid(map->getWidth(), map->getBreadth())), mFloorSprite(NULL), 
  mAdjustedTerrainRects(new RectGrid(map->getWidth(), map->getBreadth())), 
  mAdjustedFloorRects(new RectGrid(map->getWidth(), map->getBreadth())), 
  mAdjustedDecalRects(new RectGrid(map->getWidth(), map->getBreadth())),
  mMapWidth(static_cast<GJFLOAT>(map->getWidth())), 
  mMapBreadth(static_cast<GJFLOAT>(map->getBreadth())), mScrollStates(0), mScrollTime(0)
{
  Size tileSize = map->getTerrainManager().getTileSize();
  mTileBounds = GJRECT(0.0f, 0.0f, static_cast<GJFLOAT>(tileSize.width), static_cast<GJFLOAT>(tileSize.height));

  // load sprites for terrain
  mTerrainSprite = mSpriteSource->createMultiBlitSprite(L"terrain");
  mFloorSprite = mSpriteSource->createSprite(L"Floors");
  mMaxFloorId = mFloorSprite->getInfo()->frameCount;

  // calculate map bounds -- this is the bounding rectangle that can contain
  // the whole isometric map!
  GJFLOAT mapSpan = mMapWidth + mMapBreadth;
  mMapBounds = GJRECT(0, 0, floor(mTileBounds.width/2 * mapSpan), floor(mTileBounds.height/2 * mapSpan) + VERTICAL_CLEARANCE);

  // assign rect values for ALL the tiles
  const GJFLOAT tileHalfWidth = floor(mTileBounds.width/2), tileHalfHeight = floor(mTileBounds.height/2);
  GJPOINT reference = GJPOINT(floor(mMapBounds.width/2) - tileHalfWidth, VERTICAL_CLEARANCE);
  for(int y = 0; y < mMapBreadth; y++)
  {
    // determine the starting point
    GJFLOAT tLeft = reference.x - y*tileHalfWidth,
            tTop = reference.y + y*tileHalfHeight + y;

    for(int x = 0; x < mMapWidth; x++)
    {
      (*mRects)(x, y) = GJRECT(tLeft, tTop, tLeft + mTileBounds.width, tTop + mTileBounds.height);
      tLeft += tileHalfWidth;
      tTop += tileHalfHeight + 1;
    }
  }

  // make sure the first level is all water
  BasicLevel* level = (*map)(0);
  level->fillLevel(1);

  // calculate the active viewport
  centerAt(mMapWidth/2, mMapBreadth/2, 0);

  // we want periodic updates, so register ourselves
  wantFrameUpdates(true);
}

MapView::~MapView()
{
  wantFrameUpdates(false);
  SAFE_DELETE(mTerrainSprite);
  SAFE_DELETE(mFloorSprite);
  //
  SAFE_DELETE(mAdjustedTerrainRects);
  SAFE_DELETE(mAdjustedFloorRects);
  SAFE_DELETE(mAdjustedDecalRects);
  SAFE_DELETE(mRects);
}

void MapView::centerAt(const GJFLOAT x, const GJFLOAT y, const GJFLOAT z)
{
  // we're ignoring the z component now
  if((x < 0) || (x >= mMapWidth) || (y < 0) || (y >= mMapBreadth))
    return;

  GJRECT tileRect = (*mRects)(static_cast<int>(floor(x)), static_cast<int>(floor(y)));
  GJFLOAT vpLeft = tileRect.left - floor(mLocalBounds.width/2 + mTileBounds.width/2),
      vpTop = tileRect.top - floor(mLocalBounds.height/2 + mTileBounds.height/2) + VERTICAL_CLEARANCE;
  if(vpLeft < 0) 
    vpLeft = 0;
  if(vpTop < 0)
    vpTop = 0;
  //
  mViewPort = GJRECT(vpLeft, vpTop, vpLeft + mLocalBounds.width, vpTop + mLocalBounds.height);
  invalidate();
}

void MapView::invalidate()
{
  updateVisibleTileList();
}

void MapView::updateVisibleTileList()
{
  mVisibleList.clear();
  BasicLevel* level = (*mMap)(0);
  GJPOINT mAdjustment = GJPOINT(mViewPort.left, mViewPort.top);

  for(int x = 0; x < mMapWidth; x++)
    for(int y = 0; y < mMapBreadth; y++)
    {
      BasicTile* tile = (*level)(x, y);
      if(tile == NULL)
        continue;

      // get target *virtual* rectangle. if it's out of view, skip drawing it
      GJRECT ref = (*mRects)(x, y);
      if(mViewPort.outside(ref))
        continue;

      // add to visible list.
      mVisibleList.push_back(tile);

      // create adjusted versions
      // create one for the terrain rectangle first
      GJRECT target;
      //Sprite* sprite = mTerrainSprites[tile->getTerrainId()];
      GJRECT frame = mTerrainSprite->getTextureInfo()->byPixels[0];//sprite->getFrameBounds();
      adjust(frame, ref, target, mAdjustment);
      (*mAdjustedTerrainRects)(x, y) = target; 
      //
      // for the floor sprite, if any
      int floorId = tile->getFloorId();
      if((mFloorSprite != NULL) && (floorId >= 0) && (floorId < mMaxFloorId))
      {
        mFloorSprite->changeFrame(floorId);
        frame = mFloorSprite->getFrameBounds();
        adjust(frame, ref, target, mAdjustment);
        (*mAdjustedFloorRects)(x, y) = target;
      }
      else
        (*mAdjustedFloorRects)(x, y) = GJRECT(0, 0, 0, 0);
      //
      // for the decal sprite
      Decal const& decal = tile->getDecal();
      (*mAdjustedDecalRects)(x, y) = GJRECT(0, 0, 0, 0);
      if(decal.name.size() > 0)
      {
        Sprite* decalSprite = mSpriteSource->createSprite(decal.name);
        if(decalSprite != NULL)
        {
          decalSprite->changeFrame(decal.id);
          frame = decalSprite->getFrameBounds();
          adjust(frame, ref, target, mAdjustment);
          (*mAdjustedDecalRects)(x, y) = target;
        }
      }
    }

  visibleListChanged();
}

void MapView::visibleListChanged()
{
  int roadIndexBase = mMap->getTerrainManager().getTerrainIndex(L"Roads");
  mTerrainSprite->beginUpdate();
  for(TileQueue::iterator iter = mVisibleList.begin(); iter != mVisibleList.end(); iter++)
  {
    Point3 p = (*iter)->getLocation();

    // check for the presence of a road. if it's there, draw it instead
    GJRECT target = (*mAdjustedFloorRects)(p.x, p.y);
    if((target.width > 0.0f) && (target.height > 0.0f))
      mTerrainSprite->add(target, 0xffffffff, roadIndexBase + (*iter)->getFloorId());
    else
    {
      target = (*mAdjustedTerrainRects)(p.x, p.y);
      mTerrainSprite->add(target, 0xffffffff, (*iter)->getTerrainFrameIndex());
    }
  }
  mTerrainSprite->endUpdate();
  mTerrainSprite->setPosition(0.0f, 0.0f, mZValue);
}

void MapView::adjust(const GJRECT& frame, const GJRECT& ref, GJRECT& target, const GJPOINT& mAdjustment)
{
  GJFLOAT diffWidth = ref.width - frame.width,
          diffHeight = ref.height - frame.height;

  target = GJRECT(ref.left + floor(diffWidth/2), ref.top + diffHeight,
                ref.right - floor(diffWidth/2), ref.bottom);

  target -= mAdjustment;
  toGlobal(target);
}

void MapView::drawThis()
{
  // draw the terrain/floors
  mTerrainSprite->draw();
  //
  GJRECT target;
  GJFLOAT zBase = getLayerZ();
  GJFLOAT zOfElements = mZValue + zBase;
  // draw the decals now
  //zOfElements += zBase;
  for(TileQueue::iterator iter = mVisibleList.begin(); iter != mVisibleList.end(); iter++)
  {
    Point3 p = (*iter)->getLocation();
    target = (*mAdjustedDecalRects)(p.x, p.y);
    if((target.width <= 0.0f) || (target.height <= 0.0f))
      continue;

    // if there is a decal set, then let's draw it
    Decal const& decal = (*iter)->getDecal();
    Sprite* decalSprite = mSpriteSource->createSprite(decal.name);
    decalSprite->changeFrame(decal.id);
    decalSprite->setPosition(target.left, target.top, zOfElements);
    decalSprite->draw();
  }
}

bool MapView::consumeEvent(Event& event)
{
  if(event.event == EVENT_KEYBOARD)
  {
    switch(event.kbd.keyCode)
    {
    case KEY_LEFT:
      mScroller.change(SCROLL_LEFT, event.kbd.keyPressed);
      return true;
    case KEY_UP:
      mScroller.change(SCROLL_UP, event.kbd.keyPressed);
      return true;
    case KEY_RIGHT:
      mScroller.change(SCROLL_RIGHT, event.kbd.keyPressed);
      return true;
    case KEY_DOWN:
      mScroller.change(SCROLL_DOWN, event.kbd.keyPressed);
      return true;
    case KEY_NUMPAD7:
      centerAt(0, mMapBreadth-1, 0);
      return true;
    case KEY_NUMPAD9:
      centerAt(0, 0, 0);
      return true;
    case KEY_NUMPAD3:
      centerAt(mMapWidth-1, 0, 0);
      return true;
    case KEY_NUMPAD1:
      centerAt(mMapWidth-1, mMapBreadth-1, 0);
      return true;
    case KEY_NUMPAD5:
      centerAt(mMapWidth/2, mMapBreadth/2, 0);
      return true;
    }
  }
  return BaseView::consumeEvent(event);
}

void MapView::frameUpdate(const double timeSinceLast)
{
  RECT delta;
  int uCount = mScroller.update(timeSinceLast, delta);

  // process scrolling if necessary
  if(uCount)
  {
    uCount = 0; // need to count how many updates we actually did
    if(delta.left && (mViewPort.left > 0))
    {
      mViewPort.translate_h(-delta.left);
      if(mViewPort.left < 0)
        mViewPort.translate_h(-1*mViewPort.left);
      uCount++;
    }
    if(delta.right && (mViewPort.right < mMapBounds.right))
    {
      mViewPort.translate_h(delta.right);
      if(mViewPort.right > mMapBounds.right)
        mViewPort.translate_h(mViewPort.right - mMapBounds.right);
      uCount++;
    }
    if(delta.top && (mViewPort.top > 0))
    {
      mViewPort.translate_v(-delta.top);
      if(mViewPort.top < 0)
        mViewPort.translate_v(-1*mViewPort.top);
      uCount++;
    }
    if(delta.bottom && (mViewPort.bottom < mMapBounds.bottom))
    {
      mViewPort.translate_v(delta.bottom);
      if(mViewPort.bottom > mMapBounds.bottom)
        mViewPort.translate_h(mViewPort.bottom - mMapBounds.bottom);
      uCount++;
    }
    if(uCount)
      invalidate();
  }
}

