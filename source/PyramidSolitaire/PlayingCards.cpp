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

#include "PlayingCards.h"

const int NO_CARD = 0;
const int FIRST_CARD = 1;
const int LAST_CARD = 52;

CardDeck::CardDeck(const bool shuffleDeck)
{
  reset();
  if(shuffleDeck) shuffle();
}

CardDeck::~CardDeck()
{
}

void CardDeck::reset()
{
  clear();
  for(int i = FIRST_CARD; i <= LAST_CARD; i++)
    mDeckHand.push_back(Card(i));
}

void CardDeck::clear()
{
  mDeckHand.clear();
}

void CardDeck::shuffle(const int shuffleCount)
{
  if(mDeckHand.size() > 0)
    for(int i = 0; i < shuffleCount; i++)
    {
      int pos = rand() % LAST_CARD;
      DeckHand::iterator target = mDeckHand.begin();
      target += pos;

      Card c = *target;
      mDeckHand.erase(target);
      mDeckHand.push_back(c);
    }
}

bool CardDeck::isEmpty() const
{
  return mDeckHand.empty();
}

int CardDeck::remaining() const
{
  return static_cast<int>(mDeckHand.size());
}

bool CardDeck::contains(const int cardNumber) const
{
  for(DeckHand::const_iterator iter = mDeckHand.begin(); iter != mDeckHand.end(); iter++)
    if((*iter).cardNumber == cardNumber)
      return true;

  return false;
}

Card CardDeck::top(const bool remove)
{
  assert(mDeckHand.size() > 0);
  Card c = *(mDeckHand.begin());
  if(remove) 
    mDeckHand.erase(mDeckHand.begin());
  return c;
}

Card CardDeck::bottom(const bool remove)
{
  assert(mDeckHand.size() > 0);
  Card c = *(mDeckHand.end()-1);
  if(remove) 
    mDeckHand.erase(mDeckHand.end()-1);
  return c;
}

void CardDeck::addToTop(Card card)
{
  mDeckHand.insert(mDeckHand.begin(), card);
}

void CardDeck::addToBottom(Card card)
{
  mDeckHand.push_back(card);
}
