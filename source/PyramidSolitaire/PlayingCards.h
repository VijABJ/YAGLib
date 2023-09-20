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
 * @file  PlayingCards.h
 * @brief Encapsulates playing cards/decks/etc
 *
 */
#ifndef GJ_PLAYING_CARDS_HEADER
#define GJ_PLAYING_CARDS_HEADER

#include "GjDefs.h"

struct Card
{
  int deckNumber; // so we know which deck it's from
  int cardNumber; // 0 - none, 1-52 is the card number
  int suitIndex;  // card # in its own suit (1-13)
  bool faceUp;    // if true, card is faceup
  bool isBlack;   // true for cards 1-26
  // methods follows
  Card()
  {
    deckNumber = cardNumber = 0;
    isBlack = faceUp = false;
  };
  Card(const int _cardNumber, const bool _faceUp = false, const int _deckNumber = 0)
  {
    deckNumber = _deckNumber;
    cardNumber = _cardNumber;
    suitIndex = cardNumber % 13;
    if((suitIndex == 0) && (cardNumber != 0))
      suitIndex = 13;
    faceUp = _faceUp && (_cardNumber > 0);
    isBlack = cardNumber <= 26;
  };
  void flip()
  {
    faceUp = !faceUp;
  };
  bool sameDeck(const Card& other) const
  {
    return (other.deckNumber == deckNumber);
  };
  bool operator==(const Card& other) const
  {
    return other.cardNumber == cardNumber;
  };
};

const int DEFAULT_SHUFFLE_COUNT = 200;

class CardDeck
{
public:
  CardDeck(const bool shuffleDeck = false);
  virtual ~CardDeck();

  typedef std::vector<Card> DeckHand;
  DeckHand::const_iterator begin() { return mDeckHand.begin(); };
  DeckHand::const_iterator end() { return mDeckHand.end(); };
  std::size_t size() { return mDeckHand.size(); };

  void reset();
  void clear();
  void shuffle(const int shuffleCount = DEFAULT_SHUFFLE_COUNT);
  bool isEmpty() const;
  int remaining() const;
  bool contains(const int cardNumber) const;

  Card top(const bool remove = false);
  Card bottom(const bool remove = false);
  void addToTop(Card card);
  void addToBottom(Card card);

private:
  DeckHand mDeckHand;
};

#endif /* GJ_PLAYING_CARDS_HEADER */
