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
 * @file  PSGame.h
 * @brief Pyramid Solitaire Game Class
 *
 */
#ifndef GJ_PSOL_GAME_CLASS_HEADER
#define GJ_PSOL_GAME_CLASS_HEADER

#include "GjDefs.h"
#include "GjPoints.h"
#include "GjEventFrameworkBase.h"
#include "GjTemplates.h"
#include "GjBaseView.h"
#include "GjPanels.h"
#include "PlayingCards.h"

const int MESSAGE_NEW_GAME = 200;
const int MESSAGE_DEAL     = 201;

const float PLAY_MARGIN_TOP = 100;

class CardView;

class CardTable
{
public:
  virtual void dragEndedNotification(CardView* card) = 0;
  virtual void removeStraightAway(CardView* card) = 0;
};

class CardView : public yaglib::DynamicPanel
{
public: 
  CardView(yaglib::BaseView* owner, CardTable* cardTable, const yaglib::GJRECT& bounds, 
    yaglib::SpriteFactory* spriteFactory, const Card& card);
  virtual ~CardView();

  Card const& getCard() const;
  void setCard(const Card& card);
  
  void updateAppearance();
  void makeDraggable(const bool activate = true);

  void addCoveredCard(CardView* cv);
  int getCoveredCount() const { return mCoveredCount; };
  void removeFromPlay();

protected:
  virtual void statesChanged(const yaglib::STATE oldState, const yaglib::STATE newState);
  virtual bool consumeEvent(yaglib::Event& event);

private:
  CardTable* mCardTable;
  Card mCard;
  yaglib::GJRECT mOriginalBounds;
  int mCoveredCount;
  typedef yaglib::ObjectList<CardView> CoveredList;
  CoveredList mCoveredCards;
};

class PSGame : public yaglib::BaseEventFramework, public CardTable
{
public:
  PSGame();
  virtual ~PSGame();

  virtual bool startup();
  virtual void shutdown();

  virtual void dragEndedNotification(CardView* card);
  virtual void removeStraightAway(CardView* card);

protected:
  virtual bool consumeEvent(yaglib::Event& event);
  virtual bool frame(const double timeSinceLast);

private:
  CardDeck mDeck;
  CardDeck mDealt;
  typedef yaglib::ObjectList<CardView> CardViews;
  CardViews mCardViews;
  yaglib::BaseView* mDeckView;
  yaglib::GJSIZE mCardSize;
  yaglib::GJSIZE mCardGaps;
  yaglib::GJRECTS mSlots; // precalculate rectangles
  // where the dealt cards are managed
  int mCardsRemainingOnTable;
  CardView* mTopDealt;
  CardView* mNextTopDealt;

  void endCurrentGame();
  void newGame();
  void dealCards();
};


#endif /* GJ_PSOL_GAME_CLASS_HEADER */
