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
*/
//-------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "_stub_be.h"
#include "systools.h"
#include "sysresmx.h"
#include "gx_struc.h"            
#include "gx_init.h"
#include "gx_csp.h"
#include "gx_rgb.h"
#include "v3xdefs.h"
#include "v3x_2.h"
#include "v3xrend.h"

static int g_lx, g_ly, g_bpp;

static void CALLING_C Flip(void)
{
    glFinish();
    STUB_gl->SwapBuffers();
    STUB_gl->UnlockGL();
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
    // WTF ?
    return NULL;
}

static void RLXAPI GetDisplayInfo(GXDISPLAYMODEHANDLE mode)
{   
    g_pRLX->pGX->View.Flip = Flip;
    g_pRLX->pGX->gi = GX_OpenGL;
    g_pRLX->pGX->csp = CSP_OpenGL;
    g_pRLX->pGX->csp_cfg.put.fonct = g_pRLX->pGX->csp.put;
    g_pRLX->pGX->csp_cfg.pset.fonct = g_pRLX->pGX->csp.pset;
    g_pRLX->pGX->csp_cfg.transp.fonct = g_pRLX->pGX->csp.Trsp50;
    g_pRLX->pGX->csp_cfg.op = g_pRLX->pGX->csp.put;
    return;
}

static int RLXAPI SetDisplayMode(GXDISPLAYMODEHANDLE mode)
{
    STUB_App->SetWindowSize(g_pRLX->pGX->View.lWidth, g_pRLX->pGX->View.lHeight); // ??
    return 0;
}

static GXDISPLAYMODEHANDLE RLXAPI SearchDisplayMode(int lx, int ly, int bpp)
{
    GXDISPLAYMODEHANDLE mode = 1;
    if (g_pRLX->Video.Config&RLXVIDEO_Windowed)
    {
        g_lx = lx;
        g_ly = ly;
        g_bpp = bpp;
    }
    else
    {
    	// ???
    }
    return 1;
}

static int RLXAPI CreateSurface(int numberOfSparePages)
{
    unsigned i;
    unsigned sz = g_pRLX->pGX->View.lWidth * g_pRLX->pGX->View.lHeight * sizeof(rgb32_t);
    for (i=0;i<2;i++)
    {
        g_pRLX->pGX->Surfaces.lpSurface[i] = NULL;
        g_pRLX->pGX->Surfaces.flags[i] = 1;
    }
    g_pRLX->pGX->Surfaces.maxSurface = numberOfSparePages;
    for (i=2;i<g_pRLX->pGX->Surfaces.maxSurface+1;i++)
    {
        g_pRLX->pGX->Surfaces.lpSurface[i] = (u_int8_t*)g_pRLX->mm_std->.malloc(sz);
        if (g_pRLX->pGX->Surfaces.lpSurface[i])
        SysMemZero(g_pRLX->pGX->Surfaces.lpSurface[i], sz);
        g_pRLX->pGX->Surfaces.flags[i] = 2;
    }
    g_pRLX->pGX->View.lpBackBuffer = NULL;
    g_pRLX->pGX->View.Flags &= ~GX_CAPS_BACKBUFFERPAGE;
    g_pRLX->pGX->View.Flags |= GX_CAPS_BACKBUFFERINVIDEO;
    return 0;
}

static void RLXAPI ReleaseSurfaces(void)
{
    unsigned i;
    for (i=2;i<g_pRLX->pGX->Surfaces.maxSurface+1;i++)
    {
        if (g_pRLX->pGX->Surfaces.lpSurface[i])
        {
            g_pRLX->g_pRLX->mm_std->->free(g_pRLX->pGX->Surfaces.lpSurface[i]);
            g_pRLX->pGX->Surfaces.flags[i] = 0;
        }
    }
    for (i=0;i<g_pRLX->pGX->Surfaces.maxSurface+1;i++) 
        g_pRLX->pGX->Surfaces.lpSurface[i]=NULL;
    g_pRLX->pGX->Surfaces.maxSurface = 0;
    return;
}

static int RLXAPI RegisterMode(int mode)
{
    g_pRLX->pGX->View.DisplayMode = (u_int16_t)mode;
    g_pRLX->pGX->Client->GetDisplayInfo(mode);
    
    return g_pRLX->pGX->Client->SetDisplayMode(mode);;
}

static void RLXAPI Shutdown(void)
{       
    return;
}

static int Open(void *hwnd)
{
    return 1;
}

static unsigned NotifyEvent(enum GX_EVENT_MODE mode, int x, int y)
{
    UNUSED(mode);
    UNUSED(x);
    UNUSED(y);
    return mode;

}
static GXCLIENTDRIVER GX_OpenGL = {
    Lock, 
    Unlock, 
    EnumDisplayList, 
    GetDisplayInfo, 
    SetDisplayMode, 
    SearchDisplayMode, 
    CreateSurface, 
    ReleaseSurfaces, 
    GL_DownloadSprite, 
    GL_ReleaseSprite, 
    GL_UpdateSprite, 
    RegisterMode, 
    Shutdown, 
    Open,
    NotifyEvent,
    "OpenGL"
};

_RLXEXPORTFUNC void RLXAPI GX_EntryPoint(struct RLXSYSTEM *p)
{
    g_pRLX = p;
	g_pRLX->pGX->Client = &GX_OpenGL;
    return;
}

_RLXEXPORTFUNC void RLXAPI V3X_EntryPoint(struct RLXSYSTEM *p)
{
	GX_EntryPoint(p);
    g_pRLX->pV3X->Client = &V3X_OpenGL;
    return;
}
