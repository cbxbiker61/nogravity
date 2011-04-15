//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2004 - realtech VR

This file is part of No Gravity 1.9

No Gravity is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 1996 - Stephane Denis
Prepared for public release: 02/24/2004 - Stephane Denis, realtech VR
Linux/SDL Port: 2005 - Matt Williams
*/
//-------------------------------------------------------------------------

#include <SDL/SDL.h>

#include "_rlx32.h"
#include "_rlx.h"
#include "sysctrl.h"

static int KeyboardOpen(void *hnd)
{
  // Nothing to do.

  return TRUE;
}

static void KeyboardRelease(void)
{
  // Nothing to do.
}

static char *KeyboardNameScanCode(int scn)
{
  static const char *name[] =
  {
    "", "ESCAPE", "1", "2", "3", "4", "5", "6",
    "7", "8", "9", "0", "-", "=", "BACKSPACE", "TAB",
    "Q", "W", "E", "R", "T", "Y", "U", "I",
    "O", "P", "[", "]", "RETURN", "CTRL", "A", "S",
    "D", "F", "G", "H", "J", "K", "L", ";",
    "'", "~", "LEFT SHIFT", "\\", "Z", "X", "C", "V",
    "B", "N", "M", ",", ".", "/", "RIGHT SHIFT", "PRINT SCREEN",
    "ALT", "SPACE", "CAPS LOCK", "F1", "F2", "F3", "F4", "F5",

    "F6", "F7", "F8", "F9", "F10", "NUM LOCK", "SCROLL LOCK", "KEYPAD 7",
    "KEYPAD 8", "KEYPAD 9", "KEYPAD -", "KEYPAD 4", "KEYPAD 5", "KEYPAD 6", "KEYPAD +", "KEYPAD 1",
    "KEYPAD 2", "KEYPAD 3", "KEYPAD 0", "KEYPAD .", "", "", "", "F11",
    "F12", "", "", "LEFT WINDOWS", "RIGHT WINDOWS", "MENU", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "JOY LEFT", "JOY RIGHT",
    "JOY UP", "JOY DOWN", "", "", "", "", "", "",
    "JOY BUTTON 1", "JOY BUTTON 2", "JOY BUTTON 3", "", "", "", "", "",

    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",

    "", "", "", "", "", "", "", "HOME",
    "UP", "PAGE UP", "", "LEFT", "", "RIGHT", "", "END",
    "DOWN", "PAGE DOWN", "INSERT", "DELETE", "", "", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
  };

  return (char *)name[scn];
}

static unsigned long KeyboardUpdate(void *dev)
{
  SDL_Event evt;
  static const enum s_keymap map[] =
  {
    0, 0, 0, 0, 0, 0, 0, 0,
    s_backspace, s_tab, 0, 0, 0, s_return, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, s_esc, 0, 0, 0, 0,
    s_space, 0, 0, 0, 0, 0, 0, s_quote,
    0, 0, 0, 0, s_coma, s_minus, s_period, s_slash,
    s_0, s_1, s_2, s_3, s_4, s_5, s_6, s_7,
    s_8, s_9, 0, s_semicolon, 0, s_equals, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, s_opensquare, 0, s_closesquare, 0, 0,
    s_tilda, s_a, s_b, s_c, s_d, s_e, s_f, s_g,
    s_h, s_i, s_j, s_k, s_l, s_m, s_n, s_o,
    s_p, s_q, s_r, s_s, s_t, s_u, s_v, s_w,
    s_x, s_y, s_z, 0, 0, 0, 0, s_delete,

    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,

    s_numinsert, s_numend, s_numdown, s_numpagedown, s_numleft, s_num5, s_numright, s_numhome,
    s_numup, s_numpageup, s_numdelete, 0, 0, s_numminus, s_numplus, 0,
    0, s_up, s_down, s_right, s_left, s_insert, s_home, s_end,
    s_pageup, s_pagedown, s_f1, s_f2, s_f3, s_f4, s_f5, s_f6,
    s_f7, s_f8, s_f9, s_f10, s_f11, s_f12, 0, 0,
    0, 0, 0, 0, s_numlock, s_capslock, s_scrolllock, s_rightshift,
    s_leftshift, s_ctrl, s_ctrl, s_alt, s_alt, 0, 0, s_winleft,
    s_winright, s_alt, 0, 0, s_printscreen, s_printscreen, 0, s_winapp,

    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
  };
  static uint8_t keys[SKEY_SCANTABLESIZE] = {0};

  // Copy the current key state to be the old key state.
  memcpy(sKEY->steButtons, sKEY->rgbButtons, SKEY_SCANTABLESIZE);

  // For each event we receive...
  while (SDL_PollEvent(&evt) != 0)
  {
    // If it's a key-related event and the key is valid...
    if (((evt.type == SDL_KEYDOWN) ||
         (evt.type == SDL_KEYUP)) &&
	(evt.key.keysym.sym < sizeof(map)) &&
        (map[evt.key.keysym.sym] != 0))
    {
      // If it was a key pressed event...
      if (evt.type == SDL_KEYDOWN)
      {
        // Set the relevant bit.
        SKEY_SET_BIT(keys, map[evt.key.keysym.sym], 1);

        // Update the current scan code pressed.
        sKEY->scanCode = map[evt.key.keysym.sym];

        // If the character is a valid ASCII character...
        if ((evt.key.keysym.sym >= 32) &&
            (evt.key.keysym.sym < 127))
        {
	  // Update the current character pressed field.
          sKEY->charCode = (char)evt.key.keysym.sym;
        }
      }
      // Alternatively, if it was a key released event...
      else
      {
        // Clear the relevant bit.
        SKEY_SET_BIT(keys, map[evt.key.keysym.sym], 0);

        // Clear the current scan code pressed field.
        sKEY->scanCode = 0;

        // Clear the current character pressed field.
        sKEY->charCode = 0;
      }
    }
  }

  // Copy our private key state into the keyboard state structure.
  memcpy(sKEY->rgbButtons, keys, SKEY_SCANTABLESIZE);

  return TRUE;
}

_RLXEXPORTFUNC KEY_ClientDriver *KEY_SystemGetInterface_STD(void)
{
  static KEY_ClientDriver driver =
  {
    KeyboardOpen,
    KeyboardRelease,
    KeyboardNameScanCode,
    KeyboardUpdate
  };

  // Set the driver.
  sKEY = &driver;

  return sKEY;
}

