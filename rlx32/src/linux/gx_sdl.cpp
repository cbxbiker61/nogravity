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
Linux/SDL Port: 2005 - Matt Williams
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
#include "SDL/SDL.h"

#define GET_GX() g_pRLX->pGX

__extern_c

void V3XRef_HardwareRegister(int bpp);
extern struct RLXSYSTEM *	g_pRLX;

static SDL_Surface *g_pSurface = NULL;
static GXDISPLAYMODEINFO *g_pDisplays = NULL;
static int g_Mode = -1;

__end_extern_c

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C Flip(void)
*
* DESCRIPTION :
*
*/

static void CALLING_C Flip(void)
{
  SDL_Flip(g_pSurface);
  return;
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
    g_pRLX->pGX->View.Flip = Flip;
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
  SDL_LockSurface(g_pSurface);
  GET_GX()->View.lPitch = g_pSurface->pitch;;
  GET_GX()->View.lpBackBuffer = (u_int8_t *)g_pSurface->pixels;
  return GET_GX()->View.lpBackBuffer;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void Unlock(void)
*
* DESCRIPTION : Unlock frame buffer
*
*/

static void Unlock(void)
{
  SDL_UnlockSurface(g_pSurface);
  return;
}

static GXDISPLAYMODEINFO RLXAPI *EnumDisplayList(int bpp)
{
  static const SDL_Rect hardcoded_mode_640_by_480 = {0, 0, 640, 480};
  static const SDL_Rect hardcoded_mode_800_by_600 = {0, 0, 800, 600};
  static const SDL_Rect *hardcoded_modes[] =
  {
    &hardcoded_mode_640_by_480,
    &hardcoded_mode_800_by_600,
    NULL
  };
  const SDL_VideoInfo *vid_info;
  SDL_PixelFormat fmts[3];
  int num_fmts;
  int fmt_idx;
  SDL_Rect **modes;
  int mode_idx;
  GXDISPLAYMODEINFO *displays;
  int display_idx = 0;

  if (g_pDisplays == NULL)
  {
    memset(&fmts, 0, sizeof(SDL_PixelFormat));

    // If we're operating in windowed mode, only allow the current pixel format.
    if (g_pRLX->Video.Config & RLXVIDEO_Windowed)
    {
      num_fmts = 1;
      vid_info = SDL_GetVideoInfo();
      memcpy(&fmts[0], vid_info->vfmt, sizeof(SDL_PixelFormat));
    }
    // Alternatively, in full-screen mode, allow any pixel format.
    else
    {
      num_fmts = 3;
      fmts[0].BitsPerPixel = 16;
      fmts[0].BytesPerPixel = 2;
      fmts[1].BitsPerPixel = 24;
      fmts[1].BytesPerPixel = 3;
      fmts[2].BitsPerPixel = 32;
      fmts[2].BytesPerPixel = 4;
    }
  
    for (fmt_idx = 0; fmt_idx < num_fmts; fmt_idx ++)
    {
      modes = SDL_ListModes(&fmts[fmt_idx], SDL_DOUBLEBUF | ((g_pRLX->Video.Config & RLXVIDEO_Windowed) ? 0 : SDL_FULLSCREEN));

      if (modes == (SDL_Rect **)-1)
      {
		modes = (SDL_Rect **)hardcoded_modes;
      }

      if (modes != NULL)
      {
        for (mode_idx = 0; modes[mode_idx] != NULL ; mode_idx ++)
        {
          display_idx ++;
        }
      }
    }

    g_pDisplays = (GXDISPLAYMODEINFO *)g_pRLX->mm_heap->malloc((display_idx + 1) * sizeof(GXDISPLAYMODEINFO));

    display_idx = 0;

    for (fmt_idx = 0; fmt_idx < num_fmts; fmt_idx ++)
    {
      modes = SDL_ListModes(&fmts[fmt_idx], SDL_DOUBLEBUF | ((g_pRLX->Video.Config & RLXVIDEO_Windowed) ? 0 : SDL_FULLSCREEN));

      if (modes == (SDL_Rect **)-1)
      {
	modes = (SDL_Rect **)hardcoded_modes;
      }

      if (modes != NULL)
      {
        for (mode_idx = 0; modes[mode_idx] != NULL ; mode_idx ++)
        {
          g_pDisplays[display_idx].lWidth = modes[mode_idx]->w;
          g_pDisplays[display_idx].lHeight = modes[mode_idx]->h;
          g_pDisplays[display_idx].BitsPerPixel = fmts[fmt_idx].BitsPerPixel;
          g_pDisplays[display_idx].mode = display_idx;
          display_idx ++;
        }
      }
    }

    g_pDisplays[display_idx].lWidth = 0;
    g_pDisplays[display_idx].lHeight = 0;
    g_pDisplays[display_idx].BitsPerPixel = 0;
    g_pDisplays[display_idx].mode = 0;
  }

  for (display_idx = 0; g_pDisplays[display_idx].BitsPerPixel != 0; display_idx ++)
  {
    // Do nothing.
  }

  displays = (GXDISPLAYMODEINFO *)g_pRLX->mm_heap->malloc((display_idx + 1) * sizeof(GXDISPLAYMODEINFO));

  memcpy(displays, g_pDisplays, (display_idx + 1) * sizeof(GXDISPLAYMODEINFO));

  return displays;
}

static void RLXAPI GetDisplayInfo(GXDISPLAYMODEHANDLE mode)
{
  SYS_ASSERT(g_pDisplays != NULL);

  // If we've been given a negative (invalid) mode and we have a valid mode...
  if ((mode < 0) &&
      (g_pDisplays[0].BitsPerPixel != 0))
  {
    // Use it.
    mode = 0;
  }

  g_pRLX->pfSetViewPort(&g_pRLX->pGX->View, g_pDisplays[mode].lWidth, g_pDisplays[mode].lHeight, g_pDisplays[mode].BitsPerPixel);
  if (g_pDisplays[mode].BitsPerPixel == 16)
  {
    g_pRLX->pGX->View.ColorMask.RedMaskSize = 5;
    g_pRLX->pGX->View.ColorMask.GreenMaskSize = 6;
    g_pRLX->pGX->View.ColorMask.BlueMaskSize = 5;
    g_pRLX->pGX->View.ColorMask.RsvdMaskSize = 0;
    g_pRLX->pGX->View.ColorMask.RedFieldPosition = 0;
    g_pRLX->pGX->View.ColorMask.GreenFieldPosition = 5;
    g_pRLX->pGX->View.ColorMask.BlueFieldPosition = 11;
    g_pRLX->pGX->View.ColorMask.RsvdFieldPosition = 16;
  }
  else if (g_pDisplays[mode].BitsPerPixel == 24)
  {
    g_pRLX->pGX->View.ColorMask.RedMaskSize = 8;
    g_pRLX->pGX->View.ColorMask.GreenMaskSize = 8;
    g_pRLX->pGX->View.ColorMask.BlueMaskSize = 8;
    g_pRLX->pGX->View.ColorMask.RsvdMaskSize = 0;
    g_pRLX->pGX->View.ColorMask.RedFieldPosition = 0;
    g_pRLX->pGX->View.ColorMask.GreenFieldPosition = 8;
    g_pRLX->pGX->View.ColorMask.BlueFieldPosition = 16;
    g_pRLX->pGX->View.ColorMask.RsvdFieldPosition = 24;
  }
  else if (g_pDisplays[mode].BitsPerPixel == 32)
  {
    g_pRLX->pGX->View.ColorMask.RedMaskSize = 8;
    g_pRLX->pGX->View.ColorMask.GreenMaskSize = 8;
    g_pRLX->pGX->View.ColorMask.BlueMaskSize = 8;
    g_pRLX->pGX->View.ColorMask.RsvdMaskSize = 8;
    g_pRLX->pGX->View.ColorMask.RedFieldPosition = 0;
    g_pRLX->pGX->View.ColorMask.GreenFieldPosition = 8;
    g_pRLX->pGX->View.ColorMask.BlueFieldPosition = 16;
    g_pRLX->pGX->View.ColorMask.RsvdFieldPosition = 24;
  }
  SetPrimitive();
  return;
}

static int RLXAPI SetDisplayMode(GXDISPLAYMODEHANDLE mode)
{
  // If we've been given a negative (invalid) mode and we have a valid mode...
  if ((mode < 0) &&
      (g_pDisplays[0].BitsPerPixel != 0))
  {
    // Use it.
    mode = 0;
  }

  g_Mode = mode;
  return 0;
}

static GXDISPLAYMODEHANDLE RLXAPI SearchDisplayMode(int lx, int ly, int bpp)
{
  GXDISPLAYMODEHANDLE mode;
  SYS_ASSERT(g_pDisplays != NULL);

  // Search through the displays, looking for one that matches the criteria.
  for (mode = 0; g_pDisplays[mode].BitsPerPixel != 0; mode ++)
  {
    if ((g_pDisplays[mode].lWidth == lx) &&
        (g_pDisplays[mode].lHeight == ly) &&
	(g_pDisplays[mode].BitsPerPixel == bpp))
    {
      break;
    }
  }

  // If the mode is out of range, i.e. we did not find the requested mode...
  if (g_pDisplays[mode].BitsPerPixel == 0)
  {
    // If we have at least one valid mode....
    if (g_pDisplays[0].BitsPerPixel != 0)
    {
      // Use it.
      mode = 0;
    }
    else
    {
      // Return -1.  This may cause the program to crash.
      mode = - 1;
    }
  }

  return mode;
}

static void RLXAPI ReleaseSurfaces(void)
{
  SDL_FreeSurface(g_pSurface);
  g_pSurface = NULL;
  return;
}

static int RLXAPI RegisterMode(GXDISPLAYMODEHANDLE mode)
{
  // If we've been given a negative (invalid) mode and we have a valid mode...
  if ((mode < 0) &&
      (g_pDisplays[0].BitsPerPixel != 0))
  {
    // Use it.
    mode = 0;
  }
  g_pRLX->pGX->View.DisplayMode = (u_int16_t)mode;
  g_pRLX->pGX->Client->GetDisplayInfo(mode);
  V3XRef_HardwareRegister(GET_GX()->View.BytePerPixel);
  return g_pRLX->pGX->Client->SetDisplayMode(mode);
}

static void RLXAPI Shutdown(void)
{
	if (g_pDisplays != NULL)
	{
		g_pRLX->mm_heap->free(g_pDisplays);
	}

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

static int Open(void *hnd)
{
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	return 0;
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
* PROTOTYPE  :  static void CreateSurface(int npages)
*
* DESCRIPTION :
*
*/
static int CreateSurface(int BackBufferCount)
{
  SYS_ASSERT(g_pSurface == NULL);
  SYS_ASSERT(g_pDisplays != NULL);
  SYS_ASSERT(g_Mode != -1);
  g_pSurface = SDL_SetVideoMode(g_pDisplays[g_Mode].lWidth,
				g_pDisplays[g_Mode].lHeight,
				g_pDisplays[g_Mode].BitsPerPixel,
				SDL_DOUBLEBUF | ((g_pRLX->Video.Config & RLXVIDEO_Windowed) ? 0 : SDL_FULLSCREEN));
  return 0;
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
	  int RedFieldPosition = GET_GX()->View.ColorMask.RedFieldPosition; // see gx_rgb.c: 465
	  GET_GX()->View.ColorMask.RedFieldPosition = 8; // Force to BGR conversion
	  u_int8_t * src_buf = (u_int8_t*)g_pRLX->mm_std->malloc(sp->LX * sp->LY * BytePerPixel);
	  g_pRLX->pfSmartConverter(src_buf, NULL, BytePerPixel,
				   sp->data, colorTable, bpp, sp->LX*sp->LY);
	  g_pRLX->mm_heap->free(sp->data);
	  sp->data = src_buf;
	  bpp = BytePerPixel;
	  GET_GX()->View.ColorMask.RedFieldPosition = RedFieldPosition; // restore redfield 
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
* PROTOTYPE  :  void GET_GX()->ClientEntryPoint(void)
*
* DESCRIPTION :
*
*/
void GX_EntryPoint(struct RLXSYSTEM *p)
{
    static GXCLIENTDRIVER driver = {
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
    	"SDL Software Renderer"
    };
	g_pRLX = p;
	GET_GX()->Client = &driver;
    return;
}
