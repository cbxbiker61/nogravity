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

static int MouseOpen(void *hnd)
{
  // SDL only supports a single mouse device with 3 buttons and 3 axes.
  sMOU->device = NULL;
  sMOU->numControllers = 1;
  sMOU->numButtons = 3;
  sMOU->numAxes = 3;

  // Ensure that joystick events are processed automatically by SDL.
  SDL_JoystickEventState(SDL_ENABLE);

  return TRUE;
}

static void MouseRelease()
{
  // Nothing to do.
}

static void MouseShow()
{
  // Just call through into SDL.
  SDL_ShowCursor(1);
}

static void MouseHide()
{
  // Just call through into SDL.
  SDL_ShowCursor(0);
}

static void MouseSetPosition(uint32_t x, uint32_t y)
{
  // Just call through into SDL.
  SDL_WarpMouse((Uint16)x, (Uint16)y);
}

static unsigned long MouseUpdate(void *dev)
{
  Uint8 buttons;

  // Copy the current button state to be the old button state.
  memcpy(sMOU->steButtons, sMOU->rgbButtons, sMOU->numButtons);

  // Get the button state and mouse movement.
  buttons = SDL_GetRelativeMouseState(&sMOU->lX, &sMOU->lY);

  // For each button, record its new state.
  sMOU->rgbButtons[0] = (uint8_t)((buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) ? 1 : 0);
  sMOU->rgbButtons[1] = (uint8_t)((buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) ? 1 : 0);
  sMOU->rgbButtons[2] = (uint8_t)((buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) ? 1 : 0);

  // SDL treats the Z axis (mouse wheel) as buttons, rather than an axis.
  sMOU->lZ = ((buttons & SDL_BUTTON(SDL_BUTTON_WHEELUP)) ? 1 : 0) -
             ((buttons & SDL_BUTTON(SDL_BUTTON_WHEELDOWN)) ? 1 : 0);

  // Get the absolute mouse position.
  (void)SDL_GetMouseState(&sMOU->x, &sMOU->y);

  return TRUE;
}

_RLXEXPORTFUNC MSE_ClientDriver *MSE_SystemGetInterface_STD(void)
{
  static MSE_ClientDriver driver =
  {
    MouseOpen,
    MouseRelease,
    MouseShow,
    MouseHide,
    MouseSetPosition,
    MouseUpdate
  };

  // Set the driver.
  sMOU = &driver;

  return sMOU;
}

