#pragma once
//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2005 - realtech VR

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
#ifndef __DS_H
#define __DS_H

#define MAX_STREAM       8
#define DSOUNDMAXBUFFER  32
#define DSH_RESERVED     0x1

// A Highlevel Secondary buffer for Direct Sound
typedef struct {
    LPDIRECTSOUNDBUFFER    pbuffer;     // Sound Buffer
    LPDIRECTSOUND3DBUFFER  pbuffer3D;   // Sound 3D Buffer
    LPKSPROPERTYSET        pReverb;     // Reverb (for EAX)
    V3XA_HANDLE            *sample;      // Proprietary Sample handle
    unsigned               flags;
}DS_handle;

__extern_c
  void DS_Err(int code, HRESULT result);
  extern LPDIRECTSOUND           g_lpDSDevice;                      //  Objet principal direct sound
  extern LPDIRECTSOUNDBUFFER     g_lpPrimaryBuffer;
  extern LPDIRECTSOUND3DLISTENER g_lpDS3DListener;
  extern DS_handle               g_pDSHandles[DSOUNDMAXBUFFER];     //  buffers sons pour stocker les samples
  extern DS_handle               DS_SecBuffer;

__end_extern_c
#endif

