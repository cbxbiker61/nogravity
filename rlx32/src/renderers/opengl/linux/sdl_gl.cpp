//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2004 - realtech VR

This file is part of Space Girl 1.9

Space Girl is free software; you can redistribute it and/or
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
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <GL/gl.h>

#include "_rlx32.h"
#include "_rlx.h"

#include "systools.h"
#include "gx_struc.h"
#include "gx_init.h"
#include "gx_csp.h"
#include "gx_rgb.h"
#include "v3xdefs.h"
#include "v3x_2.h"
#include "v3xrend.h"
#include "../gl_v3x.h"



struct RLXSYSTEM *g_pRLX;

// GL Driver Specific
static SDL_Surface *g_pSurface = NULL;
static GXDISPLAYMODEINFO *g_pDisplays = NULL;
static int g_Mode = -1;

static void RLXAPI Flip(void)
{
    glFinish();
    SDL_GL_SwapBuffers();
    return;
}

static u_int8_t RLXAPI *Lock(void)
{
    return NULL;
}

static void RLXAPI Unlock(void)
{
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
      modes = SDL_ListModes(&fmts[fmt_idx], SDL_OPENGL | ((g_pRLX->Video.Config & RLXVIDEO_Windowed) ? 0 : SDL_FULLSCREEN));

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
      modes = SDL_ListModes(&fmts[fmt_idx], SDL_OPENGL | ((g_pRLX->Video.Config & RLXVIDEO_Windowed) ? 0 : SDL_FULLSCREEN));

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

extern GXGRAPHICINTERFACE GI_OpenGL;
extern GXSPRITEINTERFACE CSP_OpenGL;
static void RLXAPI SetPrimitive()
{
  g_pRLX->pGX->View.Flip = Flip;
  g_pRLX->pGX->gi = GI_OpenGL;
  g_pRLX->pGX->csp = CSP_OpenGL;
	
  g_pRLX->pGX->csp_cfg.put.fonct = g_pRLX->pGX->csp.put;
  g_pRLX->pGX->csp_cfg.pset.fonct = g_pRLX->pGX->csp.pset;
  g_pRLX->pGX->csp_cfg.transp.fonct = g_pRLX->pGX->csp.Trsp50;
  g_pRLX->pGX->csp_cfg.op = g_pRLX->pGX->csp.put;
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

static int RLXAPI CreateSurface(int numberOfSparePages)
{
  SYS_ASSERT(g_pDisplays != NULL);
  SYS_ASSERT(g_Mode != -1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  g_pSurface = SDL_SetVideoMode(g_pDisplays[g_Mode].lWidth, g_pDisplays[g_Mode].lHeight, g_pDisplays[g_Mode].BitsPerPixel, SDL_OPENGL | ((g_pRLX->Video.Config & RLXVIDEO_Windowed) ? 0 : SDL_FULLSCREEN));
  if (g_pSurface == NULL)
  {
    return -1;
  }

  // Reset engine
  GL_InstallExtensions();
  GL_ResetViewport();

  g_pRLX->pGX->Surfaces.maxSurface = numberOfSparePages;

  return 0;
}

static void RLXAPI ReleaseSurfaces(void)
{
  SDL_FreeSurface(g_pSurface);
  g_pRLX->pGX->Surfaces.maxSurface = 0;
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

GXCLIENTDRIVER GX_OpenGL = {
    Lock,
    Unlock,
    EnumDisplayList,
    GetDisplayInfo,
    SetDisplayMode,
    SearchDisplayMode,
    CreateSurface,
    ReleaseSurfaces,
    NULL,
    NULL,
    NULL,
    RegisterMode,
    Shutdown,
    Open,
    NotifyEvent,    
    "OpenGL"
};

extern void SetPrimitiveSprites();

_RLXEXPORTFUNC void RLXAPI GX_EntryPoint(struct RLXSYSTEM *p)
{
    g_pRLX = p;
	SetPrimitiveSprites();
	g_pRLX->pGX->Client = &GX_OpenGL;
    return;
}

extern V3X_GXSystem V3X_OpenGL;

// _V3XEXPORTUNC 
void RLXAPI V3X_EntryPoint(struct RLXSYSTEM *p)
{
	GX_EntryPoint(p);
    g_pRLX->pV3X->Client = &V3X_OpenGL;
    return;
}
