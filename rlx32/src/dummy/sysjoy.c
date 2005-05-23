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
*/
//-------------------------------------------------------------------------

#include "_rlx32.h"
#include "_rlx.h"
#include "sysctrl.h"

static int JoystickOpen(void *hnd, int force_feedback)
{  
    return 0;
}

static void JoystickRelease()
{  
}

static unsigned long JoystickUpdate(void *dev)
{ 
  return 0;
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
