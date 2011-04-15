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

sysApplication		*	g_pApp;
struct RLXSYSTEM	*	g_pRLX;

inline struct GXSYSTEM *GET_GX() { return g_pRLX->pGX; }

static uint8_t RLXAPI *Lock(void)
{
    return NULL;
}

static void RLXAPI Unlock(void)
{
}

extern GXGRAPHICINTERFACE GI_OpenGL;
extern GXSPRITEINTERFACE CSP_OpenGL;

static void RLXAPI GetDisplayInfo(GXDISPLAYMODEHANDLE mode)
{
	g_pRLX->pfSetViewPort(&GET_GX()->View, gl_lx, gl_ly, gl_bpp);
	GET_GX()->View.ColorMask.RedMaskSize	= 8;
	GET_GX()->View.ColorMask.GreenMaskSize = 8;
	GET_GX()->View.ColorMask.BlueMaskSize = 8;
	GET_GX()->View.ColorMask.RsvdMaskSize = 8;
	GET_GX()->View.ColorMask.RedFieldPosition = 0;
	GET_GX()->View.ColorMask.GreenFieldPosition = 8;
	GET_GX()->View.ColorMask.BlueFieldPosition  = 16;
	GET_GX()->View.ColorMask.RsvdFieldPosition  = 24;
	GET_GX()->gi = GI_OpenGL;
	GET_GX()->csp = CSP_OpenGL;
	GET_GX()->csp_cfg.put.fonct = GET_GX()->csp.put;
    GET_GX()->csp_cfg.pset.fonct = GET_GX()->csp.pset;
    GET_GX()->csp_cfg.transp.fonct = GET_GX()->csp.Trsp50;
    GET_GX()->csp_cfg.op = GET_GX()->csp.put;
	UNUSED(mode);
}

static GXDISPLAYMODEHANDLE RLXAPI SearchDisplayMode(int lx, int ly, int bpp)
{
    gl_lx  = lx;
    gl_ly  = ly;
    gl_bpp = 32;
    return 1;
}

static int RLXAPI CreateSurface(int numberOfSparePages)
{
    GL_InstallExtensions();
	GL_ResetViewport();
    return 0;
}

static void RLXAPI ReleaseSurfaces(void)
{
	GET_GX()->Surfaces.maxSurface = 0;
}

static int RLXAPI RegisterMode(GXDISPLAYMODEHANDLE mode)
{
    GET_GX()->View.DisplayMode = (uint16_t)mode;
    GET_GX()->Client->GetDisplayInfo(mode);
	return GET_GX()->Client->SetDisplayMode(mode);
}

static void RLXAPI Shutdown(void)
{
}

static int Open(void * hwnd)
{
	V3X_BDirectGLWindow::m_pInstance = new V3X_BDirectGLWindow( BRect(0, 0, 640, 480), g_pRLX->Dev.ApplicationName);
	g_pApp->SetWindowHandle(V3X_BDirectGLWindow::m_pInstance);
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
    0,
    GetDisplayInfo,
    0,
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
	GET_GX()->Client = &GX_OpenGL;
}

extern V3X_GXSystem V3X_OpenGL;

_V3XEXPORTUNC void RLXAPI V3X_EntryPoint(struct RLXSYSTEM *p)
{
	GX_EntryPoint(p);
    g_pRLX->pV3X->Client = &V3X_OpenGL;
	g_pApp = (sysApplication*)g_pRLX->pApplication;
}

