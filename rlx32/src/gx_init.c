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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "systools.h"
#include "gx_struc.h"
#include "gx_init.h"
#include "gx_tools.h"
struct GXSYSTEM   GX;
SYS_MEMORYMANAGER  MM_video;

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void GX_SetupDefaultViewport(int x, int y, int bbp)
*
* DESCRIPTION :  
*
*/
void GX_SetupDefaultViewport(int x, int y, int bbp)
{
	SYS_ASSERT(x);
	SYS_ASSERT(y);
    GX.View.BitsPerPixel = (u_int8_t)bbp;
    GX.View.BytePerPixel = (u_int8_t)((GX.View.BitsPerPixel+1)>>3);
    GX.View.lPitch = x * GX.View.BytePerPixel;
    GX.View.lSurfaceSize = GX.View.lPitch * y;
    // Windows
    GX.View.lWidth = x;
    GX.View.lHeight  = y;
    GX.View.xmin = 0;
    GX.View.ymin = 0;
    GX.View.xmax = x - 1;
    GX.View.ymax = y - 1;
    GX.View.lRatio = (5*(x<<4))/(y*6);
    // Flags
    if ((GX.View.Flags&GX_CAPS_BACKBUFFERINVIDEO)
    &&(GX.View.Flags&GX_CAPS_FBLINEAR))
    GX.View.State|=GX_STATE_BACKBUFFERPAGE;
    return;
}


void GX_KernelAlloc()
{
	sysStrCpy(GX.csp_cfg.ext, "png");
}

