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

#include "gx_bdirectwindow.h"
#include "rlx_system"
#include "rlx_gx"
#include "_rlx.h"
#include "systools.h"
#include "beos/stub.h"
#include "gx_init.h"

sysApplication		*	g_pApp;

__extern_c
extern struct RLXSYSTEM	    *	g_pRLX;
__end_extern_c

inline struct GXSYSTEM *GET_GX() { return g_pRLX->pGX; }

static void RLXAPI blitBuffer(uint32_t dest, uint32_t src)
{
	SYS_ASSERT(GET_GX()->Surfaces.lpSurface[dest+1]);
	SYS_ASSERT(GET_GX()->Surfaces.lpSurface[src+1]);

	sysMemCpy(	GET_GX()->Surfaces.lpSurface[dest+1],
					GET_GX()->Surfaces.lpSurface[src+1],
					GET_GX()->View.lSurfaceSize);
}

static void PageFlip()
{
	acquire_sem(GX_BDirectWindow::m_pInstance->m_SemID);

	if (GX_BDirectWindow::m_pInstance->m_bDirty)
	{
		int lx = GX_BDirectWindow::m_pInstance->fBounds.right - GX_BDirectWindow::m_pInstance->fBounds.left;
		int ly = GX_BDirectWindow::m_pInstance->fBounds.bottom - GX_BDirectWindow::m_pInstance->fBounds.top;
		if (GX_BDirectWindow::m_pInstance->lWidth!=lx || GX_BDirectWindow::m_pInstance->lHeight!=ly)
		{
			GET_GX()->Client->NotifyEvent(GX_EVENT_RESIZE, lx, ly);
		}
		GX_BDirectWindow::m_pInstance->m_bDirty = false;
	}
	else
	{
		GX_BDirectWindow::m_pInstance->Lock();
		uint8_t * src = GET_GX()->Surfaces.lpSurface[ 0 ];
		uint8_t * dest = (uint8_t*)GX_BDirectWindow::m_pInstance->m_pBits + GX_BDirectWindow::m_pInstance->fBounds.top * GX_BDirectWindow::m_pInstance->fRowBytes + GX_BDirectWindow::m_pInstance->fBounds.left * GET_GX()->View.BytePerPixel;
		int i;
		for (i=0;i<GX_BDirectWindow::m_pInstance->lHeight;i++, src+=GET_GX()->View.lPitch, dest+=GX_BDirectWindow::m_pInstance->fRowBytes)
			sysMemCpy(dest, src, GET_GX()->View.lPitch);

		GX_BDirectWindow::m_pInstance->Unlock();

	}

	release_sem(GX_BDirectWindow::m_pInstance->m_SemID);
}

static GXDISPLAYMODEHANDLE SearchDisplayMode(int width, int height, int bpp)
{
	SYS_ASSERT(GX_BDirectWindow::m_pInstance);
	SYS_ASSERT(width>0);
	SYS_ASSERT(height>0);
	GX_BDirectWindow::m_pInstance->lWidth = width;
	GX_BDirectWindow::m_pInstance->lHeight = height;

	return 1;
}

static void GetDisplayInfo(GXDISPLAYMODEHANDLE mode)
{
	g_pRLX->pfSetViewPort(&GET_GX()->View, GX_BDirectWindow::m_pInstance->lWidth,
						  GX_BDirectWindow::m_pInstance->lHeight,
						  GX_BDirectWindow::m_pInstance->BitsPerPixel);
	GXRGBCOMPONENT &mask = GET_GX()->View.ColorMask;
	switch(GX_BDirectWindow::m_pInstance->BitsPerPixel)
	{
		case 15:
			mask.RedMaskSize   = 5;  mask.RedFieldPosition	= 10;
			mask.GreenMaskSize = 5;  mask.GreenFieldPosition= 5;
			mask.BlueMaskSize  = 5;  mask.BlueFieldPosition = 0;
		break;
		case 16:
			mask.RedMaskSize   = 5;  mask.RedFieldPosition	= 11;
			mask.GreenMaskSize = 6;  mask.GreenFieldPosition= 5;
			mask.BlueMaskSize  = 5;  mask.BlueFieldPosition = 0;
		break;
		case 32:
		case 24:
			mask.RedMaskSize   = 8;  mask.RedFieldPosition	= 16;
			mask.GreenMaskSize = 8;  mask.GreenFieldPosition= 8;
			mask.BlueMaskSize  = 8;  mask.BlueFieldPosition = 0;
			mask.RsvdMaskSize  = 8;  mask.RsvdFieldPosition=  24;
		break;
		default:
		break;
	}

	GX_GetGraphicInterface(GET_GX());
	GET_GX()->gi.blit = blitBuffer;
	GET_GX()->View.Flip = PageFlip;
	GET_GX()->View.lPitch = GET_GX()->View.lWidth * GET_GX()->View.BytePerPixel;
}

static int SetDisplayMode(GXDISPLAYMODEHANDLE mode)
{
	GX_BDirectWindow::m_pInstance->ResizeTo(GX_BDirectWindow::m_pInstance->lWidth, GX_BDirectWindow::m_pInstance->lHeight);
	GX_BDirectWindow::m_pInstance->Center();

	GX_BDirectWindow::m_pInstance->Lock();
	GX_BDirectWindow::m_pInstance->Create();

	if (GX_BDirectWindow::m_pInstance->SupportsWindowMode())
		GET_GX()->Client->Capabilities|=GX_CAPS2_CANRENDERWINDOW;
	else
		GET_GX()->Client->Capabilities&=~GX_CAPS2_CANRENDERWINDOW;

	GET_GX()->Client->Capabilities|=GX_CAPS2_CANRENDERFULLSCREEN;

	GX_BDirectWindow::m_pInstance->Unlock();

	return 0;
}

__extern_c
void V3XRef_HardwareRegister(int bpp);
__end_extern_c

static int RegisterMode(GXDISPLAYMODEHANDLE mode)
{
	GET_GX()->View.Flags&=~GX_CAPS_FBINTERLEAVED;
	GET_GX()->View.DisplayMode = mode;
	V3XRef_HardwareRegister(GET_GX()->View.BytePerPixel);
	GET_GX()->Client->GetDisplayInfo(mode);
	return GET_GX()->Client->SetDisplayMode(mode);
}

static GXDISPLAYMODEINFO *EnumDisplayList(int bpp)
{
	return NULL;
}

static uint8_t *Lock(void)
{
	GET_GX()->View.lpBackBuffer = GET_GX()->Surfaces.lpSurface[ 0 ];
	GET_GX()->View.State |= GX_STATE_LOCKED;
	return GET_GX()->View.lpBackBuffer;
}

static void Unlock(void)
{
	GET_GX()->View.State &= ~GX_STATE_LOCKED;
	GET_GX()->View.lpBackBuffer = NULL;
}

static void ReleaseSurfaces(void)
{
	int i;
	for (i=0;i<8;i++)
	{
		if (GET_GX()->Surfaces.lpSurface[i])
			free(GET_GX()->Surfaces.lpSurface[i]);
		GET_GX()->Surfaces.lpSurface[i] = NULL;
		GET_GX()->Surfaces.flags[i] = 0;
	}

	GET_GX()->View.lpBackBuffer = NULL;
	GET_GX()->Surfaces.maxSurface = 0;
}

static int CreateSurface(int pages)
{
	int i = 0;
	GET_GX()->Surfaces.maxSurface = pages;

	GET_GX()->View.State&=~GX_STATE_BACKBUFFERPAGE;

	for (i=0;i< pages;i++)
	{
		GET_GX()->Surfaces.lpSurface[i] = (uint8_t *) malloc(GET_GX()->View.lSurfaceSize + GET_GX()->View.lPitch);
		if (!GET_GX()->Surfaces.lpSurface[i])
			return -2;
		sysMemZero(GET_GX()->Surfaces.lpSurface[i], GET_GX()->View.lSurfaceSize);
		GET_GX()->Surfaces.flags[i] = 0 ;
	}
	GET_GX()->View.lpBackBuffer = NULL;
	return 0;
}

static unsigned NotifyEvent(enum GX_EVENT_MODE mode, int x, int y)
{
	if (GX_EVENT_RESIZE == mode)
	{
		int n = GET_GX()->Surfaces.maxSurface;
		GX_BDirectWindow::m_pInstance->lWidth = x;
		GX_BDirectWindow::m_pInstance->lHeight = y;
		g_pRLX->pfSetViewPort(&GET_GX()->View, x, y, GX_BDirectWindow::m_pInstance->BitsPerPixel);
		// GET_GX()->View.State|=GX_STATE_VIEWPORTCHANGED;
		if (n)
		{
			GET_GX()->Client->ReleaseSurfaces();
			return GET_GX()->Client->CreateSurface(n);
		}
	}
	return 0;
}

static int Open(void *hwnd)
{
	UNUSED(hwnd);
	GX_BDirectWindow::m_pInstance = new GX_BDirectWindow( BRect(0, 0, 640, 480), g_pRLX->Dev.ApplicationName);
	g_pApp->SetWindowHandle(GX_BDirectWindow::m_pInstance);
	return 0;
}

static void Shutdown()
{
	if (GX_BDirectWindow::m_pInstance)
	{
		GX_BDirectWindow::m_pInstance->Quit();
		GX_BDirectWindow::m_pInstance = 0;
	}
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
		uint8_t * src_buf = (uint8_t*)g_pRLX->mm_std->malloc(sp->LX * sp->LY * BytePerPixel);
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
	sp->handle = p;
	p->bpp = bpp;
    UNUSED(sp);
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
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  unsigned UpdateSprite(GXSPRITE *sp, const uint8_t *bitmap, const rgb24_t *colorTable)
*
* DESCRIPTION :
*
*/
static unsigned UpdateSprite(GXSPRITE *sp, const uint8_t *bitmap, const rgb24_t *colorTable)
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
* PROTOTYPE  : GXCLIENTDRIVER DX_SubSystem
*
* DESCRIPTION :
*
*/
GXCLIENTDRIVER GX_BDirectWin = {
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
	"BDirectWindow"
};

extern GXCLIENTDRIVER GX_BWinScrn;
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void GET_GX()->ClientEntryPoint(void)
*
* DESCRIPTION :
*
*/
void GX_EntryPoint(struct RLXSYSTEM *p)
{
	g_pRLX = p;
	g_pApp = (sysApplication*)g_pRLX->pApplication;
	// TODO: Change here to use BDirectWindow or BWindowscreen
	bool bFs = true;
	GET_GX()->Client = bFs ? &GX_BWinScrn : &GX_BDirectWin;
}

