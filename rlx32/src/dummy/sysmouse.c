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

static int MouseOpen(void *hnd)
{
  // SDL only supports a single mouse device with 3 buttons and 3 axes.
  sMOU->device = NULL;
  sMOU->numControllers = 1;
  sMOU->numButtons = 16;
  sMOU->numAxes = 3;

  return TRUE;
}

static void MouseRelease()
{
  // Nothing to do.
}

static void MouseShow()
{
  // Just call through into SDL.
}

static void MouseHide()
{
  // Just call through into SDL.
}

static void MouseSetPosition(u_int32_t x, u_int32_t y)
{
  // Just call through into SDL.
}

static unsigned long MouseUpdate(void *dev)
{
  return TRUE;
}

_RLXEXPORTFUNC MSE_ClientDriver *MSE_SystemGetInterface_STD(void)
{
  return sMOU;
}
