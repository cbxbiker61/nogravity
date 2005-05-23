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
