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
 * @file  GjKeyTables.h
 * @brief Lookup tables for keyboard
 *
 * Will be used for translating keycodes into characters
 *
 */

#ifndef GJ_KEYTABLES_HEADER
#define GJ_KEYTABLES_HEADER

#include "GjDefs.h"
#include "GjKeyCodes.h"

namespace yaglib
{

/* shift key flags that are collected in a single bitmask value
 */
const int SH_LEFT_SHIFT    = 0x01;
const int SH_RIGHT_SHIFT   = 0x02;
const int SH_SHIFTKEYS     = (SH_LEFT_SHIFT | SH_RIGHT_SHIFT);

const int SH_LEFT_ALT      = 0x04;
const int SH_RIGHT_ALT     = 0x08;
const int SH_ALTKEYS       = (SH_LEFT_ALT | SH_RIGHT_ALT);

const int SH_LEFT_CTRL     = 0x10;
const int SH_RIGHT_CTRL    = 0x20;
const int SH_CTRLKEYS      = (SH_LEFT_CTRL | SH_RIGHT_CTRL);

const int SH_CAPSLOCK      = 0x40;
const int SH_NUMLOCK       = 0x80;

/* structure to hold keycode and shift mask combo
 */
typedef struct 
{
  int code;
  int shift;
} KeyShift, *PKeyShift;


/* table of shift keycodes, and the equivalent bit position in the mask
 */
static KeyShift keyShifts [] = 
{
  {KEY_LSHIFT, SH_LEFT_SHIFT},
  {KEY_RSHIFT, SH_RIGHT_SHIFT},
  {KEY_LMENU, SH_LEFT_ALT},
  {KEY_RMENU, SH_RIGHT_ALT},
  {KEY_LCONTROL, SH_LEFT_CTRL},
  {KEY_RCONTROL, SH_RIGHT_CTRL},
  {KEY_CAPITAL, SH_CAPSLOCK},
  {KEY_NUMLOCK, SH_NUMLOCK}
};

/* used in loops to delineate the number of items in table keyShifts
 */
const size_t keyShiftsCount = sizeof(keyShifts) / sizeof(KeyShift);

/* structure to hold keycodes and its equivalent translations:
 * one for the normal translation, and one when shifted 
 */
typedef struct tagKeyCombo {
  int code;
  char normal;
  char caps;
} KeyCombo, *PKeyCombo;

/* lookup table for keycodes and its corresponding character translations
 */
KeyCombo keyCombos [] = {

  (KEY_1,         '1',      '!'),
  (KEY_2,         '2',      '@'),
  (KEY_3,         '3',      '#'),
  (KEY_4,         '4',      '$'),
  (KEY_5,         '5',      '%'),
  (KEY_6,         '6',      '^'),
  (KEY_7,         '7',      '&'),
  (KEY_8,         '8',      '*'),
  (KEY_9,         '9',      '('),
  (KEY_0,         '0',      ')'),     /*10*/

  (KEY_ESCAPE,    27,      27),
  (KEY_MINUS,     '-',      '_'),
  (KEY_EQUALS,    '=',      '+'),
  (KEY_BACK,      8,       8),
  (KEY_TAB,       9,       9),
  (KEY_LBRACKET,  '[',      '/*'),
  (KEY_RBRACKET,  ']',      '*/'),
  (KEY_RETURN,    13,      13),
  (KEY_SEMICOLON, ',',      0),
  (KEY_APOSTROPHE,'\'',     '"'),     /*20*/

  (KEY_GRAVE,     '`',      '~'),
  (KEY_BACKSLASH, '\\',      '|'),
  (KEY_COMMA,     ',',      '<'),
  (KEY_PERIOD,    '.',      '>'),
  (KEY_SLASH,     '/',      '?'),
  (KEY_SPACE,     ' ',      ' '),
  (KEY_A,         'a',      'A'),
  (KEY_B,         'b',      'B'),
  (KEY_C,         'c',      'C'),
  (KEY_D,         'd',      'D'),     /*30*/

  (KEY_E,         'e',      'E'),
  (KEY_F,         'f',      'F'),
  (KEY_G,         'g',      'G'),
  (KEY_H,         'h',      'H'),
  (KEY_I,         'i',      'I'),
  (KEY_J,         'j',      'J'),
  (KEY_K,         'k',      'K'),
  (KEY_L,         'l',      'L'),
  (KEY_M,         'm',      'M'),
  (KEY_N,         'n',      'N'),     /*40*/

  (KEY_O,         'o',      'O'),
  (KEY_P,         'p',      'P'),
  (KEY_Q,         'q',      'Q'),
  (KEY_R,         'r',      'R'),
  (KEY_S,         's',      'S'),
  (KEY_T,         't',      'T'),
  (KEY_U,         'u',      'U'),
  (KEY_V,         'v',      'V'),
  (KEY_W,         'w',      'W'),
  (KEY_X,         'x',      'X'),     /*50*/

  (KEY_Y,         'y',      'Y'),
  (KEY_Z,         'z',      'Z')

};

const int keyCombosCount = sizeof(keyCombos) / sizeof(KeyCombo);

} /* namespace yaglib */

#endif /* GJ_KEYTABLES_HEADER */
