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
#include <SDL/SDL.h>
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
static int gl_lx, gl_ly, gl_bpp;

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
  GXDISPLAYMODEINFO *drv = (GXDISPLAYMODEINFO *)g_pRLX->mm_heap->malloc(sizeof(GXDISPLAYMODEINFO) * 2);
  const SDL_VideoInfo *vid;

  vid = SDL_GetVideoInfo();
  if (vid == NULL)
  {
    drv[0].BitsPerPixel = 0;
    return drv;
  }

  drv->lHeight = 1024;
  drv->lWidth = 768;
  drv->BitsPerPixel = vid->vfmt->BitsPerPixel;
  drv->mode = 0;

  drv[1].BitsPerPixel = 0;

  return drv;
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
    	g_pRLX->pfSetViewPort(&g_pRLX->pGX->View, gl_lx, gl_ly, gl_bpp);
	g_pRLX->pGX->View.ColorMask.RedMaskSize		= 8;
	g_pRLX->pGX->View.ColorMask.GreenMaskSize 	= 8;
	g_pRLX->pGX->View.ColorMask.BlueMaskSize		= 8;
	g_pRLX->pGX->View.ColorMask.RsvdMaskSize		= 8;
	g_pRLX->pGX->View.ColorMask.RedFieldPosition	= 0;
	g_pRLX->pGX->View.ColorMask.GreenFieldPosition = 8;
	g_pRLX->pGX->View.ColorMask.BlueFieldPosition  = 16;
	g_pRLX->pGX->View.ColorMask.RsvdFieldPosition  = 24;
	SetPrimitive();
	UNUSED(mode);
    return;
}

static int RLXAPI SetDisplayMode(GXDISPLAYMODEHANDLE mode)
{	
	return 0;
}

static GXDISPLAYMODEHANDLE RLXAPI SearchDisplayMode(int lx, int ly, int bpp)
{
    gl_lx  = lx;
    gl_ly  = ly;
    gl_bpp = bpp;

	if ((g_pRLX->Video.Config & RLXVIDEO_Windowed)==0)
	{
		return 0;
	}
    return 1;
}

static int RLXAPI CreateSurface(int numberOfSparePages)
{
    g_pRLX->pGX->View.lpBackBuffer   = NULL;




  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  g_pSurface = SDL_SetVideoMode(640, 480, 0, SDL_OPENGL | ((g_pRLX->Video.Config & RLXVIDEO_Windowed) ? 0 : SDL_FULLSCREEN));
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
  g_pRLX->pGX->View.DisplayMode = (u_int16_t)mode;
  g_pRLX->pGX->Client->GetDisplayInfo(mode);	
  return g_pRLX->pGX->Client->SetDisplayMode(mode);
}

static void RLXAPI Shutdown(void)
{
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
