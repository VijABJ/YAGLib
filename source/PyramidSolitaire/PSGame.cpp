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

#include "PSGame.h"
#include "GjD3DDisplayDevice.h"
#include "GjButtons.h"
#include "PSSpriteNames.h"
#include <cmath>
using namespace yaglib;

CardView::CardView(BaseView* owner, CardTable* cardTable, const GJRECT& bounds, 
  SpriteFactory* spriteFactory, const Card& card) :
  DynamicPanel(owner, bounds, spriteFactory, SPRITE_EXTRA_CARDS), 
  mCardTable(cardTable), mCard(card), mCoveredCount(0)
{
  updateAppearance();
}

CardView::~CardView() 
{
  for(CoveredList::iterator iter = mCoveredCards.begin(); iter != mCoveredCards.end(); iter++)
    *iter = NULL;
}

bool CardView::consumeEvent(Event& event)
{
  DynamicPanel::consumeEvent(event);

  if((event.event == EVENT_MOUSE) && (event.mouse.type == MOUSE_LBUTTON_DBLCLICK))
  {
    if(mCard.suitIndex == 13)
      mCardTable->removeStraightAway(this);
  }

  return true;
}

void CardView::removeFromPlay()
{
  hide();
  for(CoveredList::iterator iter = mCoveredCards.begin(); iter != mCoveredCards.end(); iter++)
  {
    CardView* cv = *iter;
    cv->mCoveredCount--;
    cv->makeDraggable(cv->mCoveredCount == 0);
  }
}

void CardView::updateAppearance()
{
  if(mCard.faceUp)
  {
    WideString cardSprite = mCard.isBlack ? SPRITE_BLACK_CARDS : SPRITE_RED_CARDS;
    int frameNumber = mCard.isBlack ? mCard.cardNumber-1 : mCard.cardNumber-27;
    changeSprite(cardSprite);
    setSpriteFrameTo(frameNumber);
    refreshSprite();
  }
  else
  {
    changeSprite(SPRITE_EXTRA_CARDS);
    refreshSprite();
    setSpriteFrameTo(0);
  }
}

Card const& CardView::getCard() const
{
  return mCard;
}

void CardView::setCard(const Card& card)
{
  mCard = card;
  updateAppearance();
}

void CardView::addCoveredCard(CardView* cv)
{
  mCoveredCards.add(cv);
  cv->mCoveredCount++;
  makeDraggable(false);
}

void CardView::makeDraggable(const bool activate)
{
  setOption(VO_DRAGGABLE|VO_AUTODRAG|VO_TOPMOST_ONSELECT, activate); 
}

void CardView::statesChanged(const STATE oldState, const STATE newState)
{
  DynamicPanel::statesChanged(oldState, newState);
  // only handle dragging state changes
  if(((oldState|newState) & VS_DRAGGING) == 0)
    return;
  // if the dragging state was active before and after the state change, it didn't change!
  if((oldState & VS_DRAGGING) && (newState & VS_DRAGGING))
    return;

  if(newState & VS_DRAGGING)
  {
    // dragging began
    mOriginalBounds = mBounds;
  }
  else
  {
    // dragging ended
    mCardTable->dragEndedNotification(this);
    setBounds(mOriginalBounds);
  }
}

//
PSGame::PSGame() : BaseEventFramework(new D3DDisplayDevice()), mDeckView(NULL)
{
}

PSGame::~PSGame()
{
}

bool PSGame::startup()
{
  if(!BaseEventFramework::startup())
    return false;

  // set the background image
  mRoot->changeSprite(SPRITE_BACKGROUND);

  // create the buttons
  GJRECT bounds = GJRECT(0, 0, 150, 60); bounds.translate(100, 10);
  BaseView* view = new SimpleButton(mRoot, bounds, mScreen, L"white-button", STDMESSAGE_EXIT);
  view->setCaption(L"Exit Game");
  view->refreshSprite();
  GJFLOAT deltaX = view->getBounds().width + 10;

  bounds.translate_h(deltaX);
  view = new SimpleButton(mRoot, bounds, mScreen, L"white-button", MESSAGE_NEW_GAME);
  view->setCaption(L"New Game");

  // load and create the deck representation
  bounds = GJRECT(0, 0, 100, 100);
  mDeckView = new ImageButton(mRoot, bounds, mScreen, L"extra-cards", MESSAGE_DEAL);
  mDeckView->refreshSprite();
  GJRECT frame = mDeckView->getBounds();
  mCardSize = GJSIZE(frame.width, frame.height);
  mCardGaps = GJSIZE(floor(mCardSize.height/3), floor(mCardSize.width/3));

  bounds = GJRECT(0, 0, mCardSize.width, mCardSize.height);
  // precalculate some rectangles
  for(int row = 0; row < 7; row++)
  {
    frame = bounds;
    for(int col = 0; col <= row; col++)
    {
      mSlots.add(frame);
      frame.translate_h(mCardSize.width + mCardGaps.width);
    }
    bounds.translate(-floor((mCardSize.width + mCardGaps.width)/2), mCardGaps.height);
  }

  // now go over the list again and move it on-screen
  GJFLOAT hAdjustment = abs((mSlots.end()-1)->left) + mCardGaps.width;
  for(GJRECTS::iterator iter = mSlots.begin(); iter != mSlots.end(); iter++)
    iter->translate(hAdjustment, PLAY_MARGIN_TOP);

  // now adjust the deck location
  frame = mDeckView->getBounds();
  frame.translate(mCardGaps.width, bounds.bottom + PLAY_MARGIN_TOP);
  mDeckView->setBounds(frame);

  // create the dealt cards location
  frame.translate_h(mCardGaps.width + mCardSize.width);
  mNextTopDealt = new CardView(mRoot, this, frame, mScreen, Card(0));
  mNextTopDealt->hide();
  mNextTopDealt->makeDraggable();
  mTopDealt = new CardView(mRoot, this, frame, mScreen, Card(0));
  mTopDealt->hide();
  mTopDealt->makeDraggable();

  // start a new game immediately!
  newGame();

  return true;
}

void PSGame::shutdown()
{
  endCurrentGame();
  BaseEventFramework::shutdown();
}

void PSGame::endCurrentGame()
{
  mCardViews.clear();
}

void PSGame::newGame()
{
  for(CardViews::iterator iter = mCardViews.begin(); iter != mCardViews.end(); iter++)
  {
    CardView* cv = *iter;
    mRoot->removeChild(cv);
    *iter = NULL;
  }
  mCardViews.clear();
  

  mDeck.reset();
  mDeck.shuffle();

  GJRECTS::iterator frame = mSlots.begin();
  for(int row = 0; row < 7; row++)
  {
    int countTwoLevelsAbove = row < 2 ? 0 : ((row-1) * (1 + (row-1))) / 2;
    for(int col = 0; col <= row; col++)
    {
      Card c = mDeck.top(true); 
      c.faceUp = true;
      CardView* cv = new CardView(mRoot, this, *frame, mScreen, c);
      mCardViews.add(cv);
      // setup the covered flags
      if(row > 0)
      {
        int topLeftIndex = col == 0 ? -1 : countTwoLevelsAbove + col - 1;
        int topRightIndex = (col == row) ? -1 : countTwoLevelsAbove + col;

        if(topLeftIndex != -1)
        {
          CardView* covered = mCardViews[topLeftIndex];
          cv->addCoveredCard(covered);
        }
        if(topRightIndex != -1)
        {
          CardView* covered = mCardViews[topRightIndex];
          cv->addCoveredCard(covered);
        }
      }
      // next frame
      frame++;
    }
  }

  // update the draggability of all the cards
  for(CardViews::iterator iter = mCardViews.begin(); iter != mCardViews.end(); iter++)
  {
    CardView* cv = *iter;
    if(cv->getCoveredCount() == 0)
      cv->makeDraggable();
  }

  mDealt.clear();
  mTopDealt->hide();
  mNextTopDealt->hide();
  mCardsRemainingOnTable = 28; // # of cards that must be removed from the table
}

void PSGame::dealCards()
{
  // if deck is empty, try moving cards from the dealt deck back to this one
  if(mDeck.isEmpty())
  {
    while(!mDealt.isEmpty())
    {
      Card c = mDealt.top(true);
      c.faceUp = false;
      mDeck.addToTop(c);
    }
    mTopDealt->hide();
    mNextTopDealt->hide();
  }
  else
  {
    Card underTop = mDealt.isEmpty() ? Card(0) : mDealt.top();
    Card onTop = mDeck.top(true);
    onTop.faceUp = true;
    mDealt.addToTop(onTop);

    for(int i = 0; i < 2; i++)
    {
      if(mDeck.isEmpty())
        break;

      underTop = onTop;
      onTop = mDeck.top(true);
      onTop.faceUp = true;
      mDealt.addToTop(onTop);
    }

    // update the proxy views
    mTopDealt->setCard(onTop);
    mTopDealt->show();
    if(underTop.cardNumber != 0)
    {
      mNextTopDealt->show();
      mNextTopDealt->setCard(underTop);
    }
    else
      mNextTopDealt->hide();
  }
}

void PSGame::removeStraightAway(CardView* card)
{
  if(card == mTopDealt)
  {
    mDealt.top(true);
    if(mDealt.size() == 0)
    {
      mTopDealt->hide();
      mNextTopDealt->hide();
    }
    else
    {
      mTopDealt->setCard(mDealt.top());
      if(mDealt.size() == 1)
        mNextTopDealt->hide();
      else
        mNextTopDealt->setCard(*(mDealt.begin()+1));
    }
  }
  else
  {
    card->removeFromPlay();
    mCardsRemainingOnTable--;
  }
}

void PSGame::dragEndedNotification(CardView* card)
{
  GJRECT subjBounds = card->getBounds();
  // find the card underneath this one
  CardView* lastMatch = NULL;
  GJFLOAT prevOverlap = 0;
  for(CardViews::iterator iter = mCardViews.begin(); iter != mCardViews.end(); iter++)
  {
    CardView* possible = *iter;
    if((*iter == card) || !possible->getState(VS_VISIBLE) || (possible->getCoveredCount() > 0))
      continue;

    GJRECT possBounds = possible->getBounds();
    GJRECT intersection = subjBounds.intersection(possBounds);
    if((intersection.width <= 0) || (intersection.height <= 0))
      continue;

    // found one! remember this one if we don't already have one.
    // if we have found oen previously, then make sure this intersection
    // is larger than the previous one
    if(!lastMatch )
    {
      lastMatch = possible;
      prevOverlap = intersection.width * intersection.height;
    }
    else
    {
      GJFLOAT thisArea = intersection.width * intersection.height;
      if(thisArea > prevOverlap)
      {
        lastMatch = possible;
        prevOverlap = thisArea;
      }
    }
  }
  // did we find a match?
  if(lastMatch)
  {
    int sum = lastMatch->getCard().suitIndex + card->getCard().suitIndex;
    if(sum == 13)
    {
      lastMatch->removeFromPlay();
      mCardsRemainingOnTable--;
      removeStraightAway(card);
    }
  }
}


bool PSGame::consumeEvent(Event& event)
{
  BaseEventFramework::consumeEvent(event);

  switch(event.event)
  {
  case EVENT_MESSAGE:
    switch(event.msg.messageId)
    {
    case MESSAGE_DEAL:
      dealCards();
      break;
    case MESSAGE_NEW_GAME:
      newGame();
      break;
    }
  }

  return true;
}

bool PSGame::frame(const double timeSinceLast)
{
  if(!BaseEventFramework::frame(timeSinceLast))
    return false;

  return true;
}



