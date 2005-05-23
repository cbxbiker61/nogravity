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
#include "dummy_v3x.h"
#include "_rlx.h"
#include "v3xdefs.h"
#include "v3xrend.h"
#include "sysctrl.h"

static int  HardwareSetup(void)
{
    return 0;
}

static void  HardwareShutdown(void)
{
    return;
}

static unsigned  ZbufferClear(rgb24_t *color, V3XSCALAR z, void *bitmap)
{
    return 1;
}

static unsigned SetState(unsigned command, u_int32_t value)
{
     return 0;
}

static int TextureModify(GXSPRITE *sp, u_int8_t *bitmap, const rgb24_t *colorTable)
{
    return 1;
}

static void FreeTexture(void *sp)
{
    return;
}

static void * UploadTexture(const GXSPRITE *sp, const rgb24_t *colorTable, int bpp, unsigned options)
{
    return 0;
}

static void RenderPoly(V3XPOLY **fe, int count)
{
    return;
}


static void StartList(void)
{
    return;
}


static void EndList(void)
{
    return;
}

static void RenderDisplay(void)
{
    return;
}

static void DrawPrimitives(V3XVECTOR *vertexes, u_int16_t *indexTab, unsigned NumIndexes, unsigned NumVertexes, int option, rgb32_t *color)
{
    return;
}

V3X_GXSystem V3X_OpenGS=
{
    NULL, 
    RenderDisplay, 
    UploadTexture,
    FreeTexture, 
    TextureModify, 
    HardwareSetup,
    HardwareShutdown, 
    SetState, 
    ZbufferClear,
    RenderPoly, 
    StartList, 
    EndList, 
    DrawPrimitives, 
    "GS",
    256+10, 
    GXSPEC_ZBUFFER|
    GXSPEC_OPACITYTRANSPARENT|
    GXSPEC_SPRITEAREPOLY|
    GXSPEC_UVNORMALIZED|
	GXSPEC_FULLHWSPRITE|
    GXSPEC_RESIZABLEMAP|
    GXSPEC_ALPHABLENDING_ADD|
    GXSPEC_HARDWAREBLENDING|
    GXSPEC_ANTIALIASEDLINE|
    GXSPEC_ENABLEFILTERING|
    GXSPEC_HARDWARE|
    GXSPEC_ENABLEPERSPECTIVE|
    0, 
    0x000000, 
    1 // Texture en (256>>1)=128x128 par defaut
};


