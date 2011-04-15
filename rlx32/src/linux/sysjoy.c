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

static int JoystickOpen(void *hnd, int force_feedback)
{
  int ok = FALSE;
  int idx;
  SDL_Joystick *joy;

  // Initialize SDL's joystick subsystem.
  SDL_InitSubSystem(SDL_INIT_JOYSTICK);

  // Ensure that joystick events are processed automatically by SDL.
  SDL_JoystickEventState(SDL_ENABLE);

  // For each joystick device...
  for (idx = 0; idx < SDL_NumJoysticks(); idx ++)
  {
    // Open the device.  If this fails, just skip it.
    joy = SDL_JoystickOpen(idx);
    if (joy == NULL)
    {
      continue;
    }

    // If it has fewer than 3 axes, we can't use it.
    if (SDL_JoystickNumAxes(joy) < 3)
    {
      continue;
    }

    // This is a valid joystick, so increment the count.
    sJOY->numControllers++;

    // If this joystick is better than the current best...
    if (SDL_JoystickNumButtons(joy) > sJOY->numButtons)
    {
      // Close the current best joystick device.
      if (sJOY->device != NULL)
      {
	SDL_JoystickClose(sJOY->device);
      }

      // Update the record to point to this joystick.
      sJOY->device = joy;
      sJOY->numButtons = SDL_JoystickNumButtons(joy);
      sJOY->numAxes = SDL_JoystickNumAxes(joy);
      sJOY->numPOVs = SDL_JoystickNumHats(joy);

      // We've now definitely succeeded.
      ok = TRUE;
    }
    // Alternatively, if this joystick is worse than the current best...
    else
    {
      // Close this joystick device.
      SDL_JoystickClose(joy);
    }
  }

  // If we failed to get a joystick device...
  if (!ok)
  {
    // Terminate SDL's joystick subsystem.
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
  }

  return ok;
}

static void JoystickRelease()
{
  SDL_Joystick *joy = (SDL_Joystick *)sJOY->device;

  // If we have a joystick device...
  if (joy != NULL)
  {
    // Close it.
    SDL_JoystickClose(joy);

    // Forget about the joystick device.
    sJOY->device = NULL;

    // Terminate SDL's joystick subsystem.
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
  }
}

static unsigned long JoystickUpdate(void *dev)
{
  SDL_Joystick *joy = (SDL_Joystick *)sJOY->device;
  int idx;
  unsigned long ok = FALSE;

  // If we have a joystick device...
  if (joy != NULL)
  {
    // Copy the current button state to be the old button state.
    memcpy(sJOY->steButtons, sJOY->rgbButtons, sJOY->numButtons);

    // For each button, record its new state.
    for (idx = 0; idx < sJOY->numButtons; idx ++)
    {
      sJOY->rgbButtons[idx] = (uint8_t)SDL_JoystickGetButton(joy, idx);
    }

    // For each hat, record its new state.
    for (idx = 0; idx < sJOY->numPOVs; idx ++)
    {
      switch (SDL_JoystickGetHat(joy, idx))
      {
        case SDL_HAT_CENTERED:  sJOY->rgdwPOV[idx] =     1; break;
        case SDL_HAT_UP:        sJOY->rgdwPOV[idx] =     0; break;
        case SDL_HAT_RIGHTUP:   sJOY->rgdwPOV[idx] =  4500; break;
        case SDL_HAT_RIGHT:     sJOY->rgdwPOV[idx] =  9000; break;
        case SDL_HAT_RIGHTDOWN: sJOY->rgdwPOV[idx] = 13500; break;
        case SDL_HAT_DOWN:      sJOY->rgdwPOV[idx] = 18000; break;
        case SDL_HAT_LEFTDOWN:  sJOY->rgdwPOV[idx] = 22500; break;
        case SDL_HAT_LEFT:      sJOY->rgdwPOV[idx] = 27000; break;
        case SDL_HAT_LEFTUP:    sJOY->rgdwPOV[idx] = 31500; break;
      }
    }

    // For each axis, record its new state.
    // TODO: Support configuration of which axis is which.
    sJOY->lX = (int)SDL_JoystickGetAxis(joy, 0) + 32768;
    sJOY->lY = (int)SDL_JoystickGetAxis(joy, 1) + 32768;
    sJOY->lZ = (int)SDL_JoystickGetAxis(joy, 2) + 32768;

    // We've succeeded.
    ok = TRUE;
  }

  return ok;
}

_RLXEXPORTFUNC JOY_ClientDriver *JOY_SystemGetInterface_STD(void)
{
  static JOY_ClientDriver driver =
  {
    JoystickOpen,
    JoystickRelease,
    JoystickUpdate
  };

  // Set the driver.
  sJOY = &driver;

  return sJOY;
}

