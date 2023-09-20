
#ifndef GJ_MAP_VIEW_HEADER
#define GJ_MAP_VIEW_HEADER

#include "GjDefs.h"
#include "GjBaseView.h"
#include "GjSprites.h"
#include "GjTileMap.h"
#include "GjTemplates.h"
#include "GjScrollHelper.h"
#include "GjGameLibConstants.h"
#include <vector>

const GJFLOAT VERTICAL_CLEARANCE = 120;

namespace yaglib
{

class MapView : public BaseView
{
public:
  MapView(BaseView* owner, const GJRECT& bounds, SpriteFactory* spriteFactory, BasicMap* map);
  virtual ~MapView();

  virtual bool consumeEvent(Event& event);
  virtual void frameUpdate(const double timeSinceLast);
  void centerAt(const GJFLOAT x, const GJFLOAT y, const GJFLOAT z);
  virtual void invalidate();

protected:
  virtual void drawThis();
  virtual void visibleListChanged();

private:
  BasicMap* mMap;
  GJFLOAT mMapWidth;
  GJFLOAT mMapBreadth;
  GJRECT mTileBounds;
  GJRECT mMapBounds;
  GJRECT mViewPort;
  typedef Grid2D<GJRECT> RectGrid;
  RectGrid* mRects;
  RectGrid* mAdjustedTerrainRects;
  RectGrid* mAdjustedFloorRects;
  RectGrid* mAdjustedDecalRects;
  FourWayScroller mScroller;
  //
  //typedef std::vector<Sprite*> SpriteList;
  //SpriteList mTerrainSprites;
  typedef std::vector<BasicTile*> TileQueue;
  TileQueue mVisibleList;

  MultiBlitSprite* mTerrainSprite;
  Sprite* mFloorSprite;
  int mMaxFloorId;
  GJPOINT mAdjustment;

  int mScrollStates;
  double mScrollTime;

  void adjust(const GJRECT& frame, const GJRECT& ref, GJRECT& target, const GJPOINT& adjustment);
  void updateVisibleTileList();   
};

} /* namespace yaglib */

#endif /* GJ_MAP_VIEW_HEADER */
