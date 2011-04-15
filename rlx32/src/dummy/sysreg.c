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

#include <stdio.h>

#include "_rlx32.h"
#include "_rlx.h"
#include "sysini.h"
#include "systools.h"

STUB_Registry RLX={
 // System
 {
   // TODO: Understand what all these options mean and if the right ones are chosen.
   RLXSYSTEM_Enable,
   RLXOS_LINUX,
   FALSE,
   5,
   0,
   {
     NULL,
     NULL,
     NULL,
     "80386",
     "80486",
     "Pentium",
     "Pentium Pro",
     "Ix86"
   }
 },
 // Audio
 {
   RLXAUDIO_Enable+
   RLXAUDIO_Use16BIT+
   RLXAUDIO_UseSTEREO,
   0, 0, 8, 0,
   0,
   4, 32,
   {11025, 16537, 22050, 33075, 44100}
 },
 // Video
 {
   RLXVIDEO_Enable,
   RLXVIDEO_Primary,
   70,
   0,
   0
 },
 // 3D
 {
   0,
   RLX3D_OPENGL,
   2,
   {0, 0}
 },

 // Control
 {
   RLXCTRL_Enable+
   RLXCTRL_IntKeyboard,
   RLXCTRL_JoyAnalog,
 },
 // Joy
 {
   {
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
   }
 },
 // Network
 {
   0,
   {0, 0, 0}
 },
 // Dev
 {
   "Default",
   "application/x-vnd",
   "Realtech",
   0,
   0,
 },
 // App
 {
   "",
   RLXAPP_IsRunning,
   0,
   1L<<20
 },
 "rlxreg.ini",
 ""
};

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void STUB_RegistryReset(STUB_Registry *regs)
*
* DESCRIPTION :
*
*/
void STUB_RegistryReset(STUB_Registry *regs)
{
    *regs = RLX;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void STUB_RegistryRead(STUB_Registry *regs)
*
* DESCRIPTION :
*
*/
void STUB_RegistryRead(STUB_Registry *regs)
{
}

int STUB_RegistryWrite(STUB_Registry *regs)
{
   return 0;
}

