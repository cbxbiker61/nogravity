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
#include <stdlib.h>
#include <SDL.h>

#include "_rlx32.h"
#include "_rlx.h"
#include "_stub.h"

int STUB_TaskControl(void)
{
  // TODO: Support application events - this probably needs to be done in the keyboard handler.
  return FALSE;
}

int main(int argc, char *argv[])
{
  // TODO: Set up current directory.

  // When we finish, call SDL_Quit to make sure we're all tidied up.
  atexit(SDL_Quit);

  // Standard main function.
  STUB_OsStartup(NULL);
  STUB_Default();
  STUB_OsCustom(NULL);
  STUB_CheckUp(NULL);
  STUB_ReadyToRun();
  RLX.System.Running = TRUE;
  STUB_MainCode();
  STUB_Down();
  STUB_QuitRequested();

  return 0;
}
