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
#include "sysctrl.h"
#include "sysresmx.h"
#include "gx_struc.h"
#include "gx_init.h"
#include "gx_tools.h"
#include "gx_rgb.h"
#include "gx_csp.h"
#include "gx_init.h"
#include "SDL.h"

/*

  If you want to have a SDL software renderer,
  you need to add thoses files
  
  rlx32/src/renderers/gfx_ref/gr16bit.c
  rlx32/src/renderers/gfx_ref/gr32bit.c
  rlx32/src/renderers/gfx_ref/gr8bit.c
  rlx32/src/renderers/gfx_ref/gx_drv.c
  rlx32/src/renderers/gfx_ref/sp16bit.c
  rlx32/src/renderers/gfx_ref/sp32bit.c
  rlx32/src/renderers/v3x_ref/r08_128.c
  rlx32/src/renderers/v3x_ref/r08_256.c
  rlx32/src/renderers/v3x_ref/r16_128.c
  rlx32/src/renderers/v3x_ref/r16_256.c
  rlx32/src/renderers/v3x_ref/r32_246.c
  rlx32/src/renderers/v3x_ref/v3xsoft.c
  rlx32/src/renderers/v3x_ref/z08_256.c
  rlx32/src/renderers/v3x_ref/z16_256.c
  rlx32/src/renderers/v3x_ref/z32_256.c

*/

#define GET_GX() g_pRLX->pGX

__extern_c

void V3XRef_HardwareRegister(int bpp);
extern struct RLXSYSTEM *	g_pRLX;

__end_extern_c

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void Unlock(void)
*
* DESCRIPTION : Unlock frame buffer
*
*/

static void Unlock(void)
{
    // See SDL_UnlockSurface
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static u_int8_t *Lock(void)
*
* DESCRIPTION :  Lock framebuffer. Returns a pointer to the framebuffer + fills information (pitch = bytes / row)
*
*/

static u_int8_t *Lock(void)
{
    // See SDL_LockSurface
    GET_GX()->View.lPitch = 0;
    GET_GX()->View.lpBackBuffer = 0;
    return GET_GX()->View.lpBackBuffer;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C Flip(void)
*
* DESCRIPTION :
*
*/

static void CALLING_C Flip(void)
{
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  GXDISPLAYMODEINFO *EnumDisplayList(int bpp)
*
* DESCRIPTION :
*
*/
GXDISPLAYMODEINFO *EnumDisplayList(int bpp)
{
    return NULL;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void SearchDisplayMode(int lx, int ly, int bpp)
*
* DESCRIPTION :
*
*/

static int SearchDisplayMode(int lx, int ly, int bpp)
{
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void SetDisplayMode(int mode)
*
* DESCRIPTION :
*
*/
static int SetDisplayMode(int mode)
{
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void ReleaseSurfaces(void)
*
* DESCRIPTION :  
*
*/
static void ReleaseSurfaces(void)
{
   // See SDL_FreeSurface
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void Shutdown(void)
*
* DESCRIPTION :  
*
*/
static void Shutdown(void)
{
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C ClearBackBuffer(void)
*
* DESCRIPTION :
*
*/
static void CALLING_C ClearBackBuffer(void)
{
    // int SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color); ?
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CreateSurface(int npages)
*
* DESCRIPTION :
*
*/
static int CreateSurface(int BackBufferCount)
{
    // See SDL_CreateRGBSurface. Ignores BackBufferCount
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION : Fonctions Graphiques
*
*/
static void CALLING_C sdl_Idle(void)
{
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C sdl_Clear(void)
*
* DESCRIPTION :
*
*/
static void CALLING_C sdl_Clear(void)
{
    // TODO: Clear screen
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C sdl_Blit(u_int32_t dest, u_int32_t src)
*
* DESCRIPTION :
*
*/
static void CALLING_C sdl_Blit(u_int32_t dest, u_int32_t src)
{
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C sdl_SoftClear(void)
*
* DESCRIPTION :
*
*/
static void CALLING_C sdl_SoftClear(void)
{
    sysMemZero(GET_GX()->View.lpBackBuffer, GET_GX()->View.lSurfaceSize);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/

static int RegisterMode(int mode)
{
    GET_GX()->View.DisplayMode = (u_int16_t)mode;
    GET_GX()->Client->GetDisplayInfo(mode);
	V3XRef_HardwareRegister(GET_GX()->View.BytePerPixel);
    return GET_GX()->Client->SetDisplayMode(mode);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void UploadSprite(GXSPRITE *sp)
*
* DESCRIPTION :
*
*/

static void UploadSprite(GXSPRITE *sp, rgb24_t *colorTable, int bpp)
{
	GXSPRITESW *p = (GXSPRITESW*) g_pRLX->mm_heap->malloc(sizeof(GXSPRITESW));
	int i;
	if (bpp == 3)
	{
		int BytePerPixel = GET_GX()->View.BytePerPixel;
		u_int8_t * src_buf = (u_int8_t*)g_pRLX->mm_std->malloc(sp->LX * sp->LY * BytePerPixel);
		g_pRLX->pfSmartConverter(src_buf, NULL, BytePerPixel,
								 sp->data, colorTable, bpp, sp->LX*sp->LY);
		g_pRLX->mm_heap->free(sp->data);
		sp->data = src_buf;
		bpp = BytePerPixel;
	}
	else
	if (bpp == 1)
	{
		for (i=0;i<256;i++)
			p->palette[i] = g_pRLX->pfSetPixelFormat(colorTable[i].r, colorTable[i].g, colorTable[i].b);
	}
	else
	{
		SYS_ASSERT(bpp);
	}
	sp->handle = p;
	p->bpp = bpp;
    UNUSED(sp);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void ReleaseSprite(GXSPRITE *sp)
*
* DESCRIPTION :
*
*/
static void ReleaseSprite(GXSPRITE *sp)
{
	GXSPRITESW *p = (GXSPRITESW*) sp->handle;
	g_pRLX->mm_heap->free(p);
	sp->data = NULL;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  unsigned UpdateSprite(GXSPRITE *sp, const u_int8_t *bitmap, const rgb24_t *colorTable)
*
* DESCRIPTION :
*
*/
static unsigned UpdateSprite(GXSPRITE *sp, const u_int8_t *bitmap, const rgb24_t *colorTable)
{
	GXSPRITESW *p = (GXSPRITESW*) sp->handle;
	int i;
	for (i=0;i<256;i++)
		p->palette[i] = g_pRLX->pfSetPixelFormat(colorTable[i].r, colorTable[i].g, colorTable[i].b);
	sysMemCpy(sp->data, bitmap, sp->LX * sp->LY);
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void GX_SetPrimitive(void)
*
* DESCRIPTION :
*
*/

static void SetPrimitive(void)
{
    GX_GetGraphicInterface(GET_GX());
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void GetDisplayInfo(int mode)
*
* DESCRIPTION :
*
*/
static void GetDisplayInfo(int mode)
{
    if (g_pRLX->Video.Config&RLXVIDEO_Windowed)
    {
		g_pRLX->pfSetViewPort(&g_pRLX->pGX->View, g_lx, g_ly, g_bpp);
		SetPrimitive();
		GET_GX()->View.Flip = Flip;
    }
    else
    {
		GXDISPLAYMODEINFO *ls = g_lpDisplayModeInfo;
		while (ls->mode!=0)
		{
			if (ls->mode == mode)
				break;
			ls++;
		}
		SYS_ASSERT(ls->mode == mode);
		g_pRLX->pfSetViewPort(&g_pRLX->pGX->View, ls->lWidth, ls->lHeight, ls->BitsPerPixel);
		SetPrimitive();
		GET_GX()->View.Flip = Flip;
		g_lx = ls->lWidth;
		g_ly = ls->lHeight;
		g_bpp = ls->BitsPerPixel;
    }    
    return;
}


static unsigned NotifyEvent(enum GX_EVENT_MODE mode, int x, int y)
{
    UNUSED(mode);
    UNUSED(x);
    UNUSED(y);
    return mode;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int Open(void *hwnd)
*
* Description :
*
*/
static int Open(void *hwnd)
{ 
    // SDL_Init ?
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void GET_GX()->ClientEntryPoint(void)
*
* DESCRIPTION :
*
*/
void GX_EntryPoint(struct RLXSYSTEM *p)
{
    GXCLIENTDRIVER driver = {
        Lock,
        Unlock,
        EnumDisplayList,
        GetDisplayInfo,
        SetDisplayMode,
        SearchDisplayMode,
        CreateSurface,
        ReleaseSurfaces,
        UploadSprite,
        ReleaseSprite,
        UpdateSprite,
        RegisterMode,
        Shutdown,
        Open,
        NotifyEvent,
    	"SDL Graphics"
    };
	g_pRLX = p;
	GET_GX()->Client = &driver;
    return;
}
