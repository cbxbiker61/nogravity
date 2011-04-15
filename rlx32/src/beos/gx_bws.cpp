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
#include "rlx_system"
#include "rlx_gx"

#include "_rlx32.h"
#include "beos/stub.h"
#include "gx_bwindowscreen.h"
#include "gx_init.h"

__extern_c
extern struct RLXSYSTEM	    *	g_pRLX;
__end_extern_c

#define GET_GX() g_pRLX->pGX

extern sysApplication		*	g_pApp;

#define INVALID_MODE  0x0

static const GK_DisplayModeSettings g_pDisplayModes[]=
{
	{ 640, 400, 8, B_8_BIT_640x400},
	{ 640, 480, 8, B_8_BIT_640x480},
	{ 800, 600, 8, B_8_BIT_800x600},
	{ 1024, 768, 8, B_8_BIT_1024x768},
	{ 1152, 900, 8, B_8_BIT_1152x900},
	{ 1600, 1200, 16, B_8_BIT_1600x1200},

	{ 640, 480, 16, B_16_BIT_640x480},
	{ 800, 600, 16, B_16_BIT_800x600},
	{ 1024, 768, 16, B_16_BIT_1024x768},
	{ 1152, 600, 16, B_16_BIT_1152x900},
	{ 1280, 1024, 16, B_16_BIT_1280x1024},
	{ 1600, 1200, 16, B_16_BIT_1600x1200},

	{ 640, 480, 32, B_32_BIT_640x480},
	{ 800, 600, 32, B_32_BIT_800x600},
	{ 1024, 768, 32, B_32_BIT_1024x768},
	{ 1152, 600, 32, B_32_BIT_1152x900},
	{ 1280, 1024, 32, B_32_BIT_1280x1024},
	{ 1600, 1200, 32, B_32_BIT_1600x1200},
	{ -1, -1, -1, 0}
};

// 8
static void hook_8_drawAnyLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
{
	GX_BWindowScreen::GetInstance()->draw_line_with_8_bit_depth(x1, y1+GX_BWindowScreen::GetInstance()->m_Offset, x2, y2+GX_BWindowScreen::GetInstance()->m_Offset, color, 0, 0, 0, 0, 0);
}
static void hook_8_drawHorizontalLine(int32_t x1, int32_t y1, int32_t lx, uint32_t color)
{
	GX_BWindowScreen::GetInstance()->draw_rect_with_8_bit_depth(x1, y1+GX_BWindowScreen::GetInstance()->m_Offset, x1+lx-1, y1+GX_BWindowScreen::GetInstance()->m_Offset, color);
}
static void hook_8_drawVerticalLine(int32_t x1, int32_t y1, int32_t lx, uint32_t color)
{
	GX_BWindowScreen::GetInstance()->draw_rect_with_8_bit_depth(x1, y1+GX_BWindowScreen::GetInstance()->m_Offset, x1, y1+lx+GX_BWindowScreen::GetInstance()->m_Offset, color);
}
static void hook_8_drawFilledRect(int32_t x, int32_t y, int32_t x2, int32_t y2, uint32_t color)
{
	GX_BWindowScreen::GetInstance()->draw_rect_with_8_bit_depth(x, y+GX_BWindowScreen::GetInstance()->m_Offset, x2, y2+GX_BWindowScreen::GetInstance()->m_Offset, color);
}
static void hook_8_clearBackBuffer(void)
{
	GX_BWindowScreen::GetInstance()->draw_rect_with_8_bit_depth(0, GX_BWindowScreen::GetInstance()->m_Offset, GET_GX()->View.lWidth, GET_GX()->View.lHeight+GX_BWindowScreen::GetInstance()->m_Offset-1, 0);
}

// 16
static void hook_16_drawAnyLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
{
	GX_BWindowScreen::GetInstance()->draw_line_with_16_bit_depth(x1, y1+GX_BWindowScreen::GetInstance()->m_Offset, x2, y2+GX_BWindowScreen::GetInstance()->m_Offset, color, 0, 0, 0, 0, 0);
}
static void hook_16_drawHorizontalLine(int32_t x1, int32_t y1, int32_t lx, uint32_t color)
{
	GX_BWindowScreen::GetInstance()->draw_rect_with_16_bit_depth(x1, y1+GX_BWindowScreen::GetInstance()->m_Offset, x1+lx-1, y1+GX_BWindowScreen::GetInstance()->m_Offset, color);
}
static void hook_16_drawVerticalLine(int32_t x1, int32_t y1, int32_t lx, uint32_t color)
{
	GX_BWindowScreen::GetInstance()->draw_rect_with_16_bit_depth(x1, y1+GX_BWindowScreen::GetInstance()->m_Offset, x1, y1+lx+GX_BWindowScreen::GetInstance()->m_Offset, color);
}
static void hook_16_drawFilledRect(int32_t x, int32_t y, int32_t x2, int32_t y2, uint32_t color)
{
	GX_BWindowScreen::GetInstance()->draw_rect_with_16_bit_depth(x, y+GX_BWindowScreen::GetInstance()->m_Offset, x2, y2+GX_BWindowScreen::GetInstance()->m_Offset, color);
}
static void hook_16_clearBackBuffer(void)
{
	GX_BWindowScreen::GetInstance()->draw_rect_with_16_bit_depth(0, GX_BWindowScreen::GetInstance()->m_Offset, GET_GX()->View.lWidth, GET_GX()->View.lHeight+GX_BWindowScreen::GetInstance()->m_Offset-1, 0);
}

// 32
static void hook_32_drawAnyLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
{
	GX_BWindowScreen::GetInstance()->draw_line_with_32_bit_depth(x1, y1+GX_BWindowScreen::GetInstance()->m_Offset, x2, y2+GX_BWindowScreen::GetInstance()->m_Offset, color, 0, 0, 0, 0, 0);
}
static void hook_32_drawHorizontalLine(int32_t x1, int32_t y1, int32_t lx, uint32_t color)
{
	GX_BWindowScreen::GetInstance()->draw_rect_with_32_bit_depth(x1, y1+GX_BWindowScreen::GetInstance()->m_Offset, x1+lx-1, y1+GX_BWindowScreen::GetInstance()->m_Offset, color);
}
static void hook_32_drawVerticalLine(int32_t x1, int32_t y1, int32_t lx, uint32_t color)
{
	GX_BWindowScreen::GetInstance()->draw_rect_with_32_bit_depth(x1, y1+GX_BWindowScreen::GetInstance()->m_Offset, x1, y1+lx+GX_BWindowScreen::GetInstance()->m_Offset, color);
}
static void hook_32_drawFilledRect(int32_t x, int32_t y, int32_t x2, int32_t y2, uint32_t color)
{
	GX_BWindowScreen::GetInstance()->draw_rect_with_32_bit_depth(x, y+GX_BWindowScreen::GetInstance()->m_Offset, x2, y2+GX_BWindowScreen::GetInstance()->m_Offset, color);
}
static void hook_32_clearBackBuffer(void)
{
	GX_BWindowScreen::GetInstance()->draw_rect_with_32_bit_depth(0, GX_BWindowScreen::GetInstance()->m_Offset, GET_GX()->View.lWidth, GET_GX()->View.lHeight+GX_BWindowScreen::GetInstance()->m_Offset-1, 0);
}

static void GetGraphicInterface(int bpp, struct _gx_graphic_interface *pGi)
{
	switch(bpp)
	{
		case 8:
		if (GX_BWindowScreen::GetInstance()->draw_line_with_8_bit_depth)
			pGi->drawAnyLine = hook_8_drawAnyLine;
		if (GX_BWindowScreen::GetInstance()->draw_rect_with_8_bit_depth)
		{
			pGi->clearBackBuffer = hook_8_clearBackBuffer;
			pGi->drawFilledRect = hook_8_drawFilledRect;
			pGi->drawHorizontalLine = hook_8_drawHorizontalLine;
			pGi->drawVerticalLine = hook_8_drawVerticalLine;
		}
		break;

		case 16:
		if (GX_BWindowScreen::GetInstance()->draw_line_with_16_bit_depth)
			pGi->drawAnyLine = hook_16_drawAnyLine;

		if (GX_BWindowScreen::GetInstance()->draw_rect_with_16_bit_depth)
		{
			pGi->clearBackBuffer = hook_16_clearBackBuffer;
			pGi->drawFilledRect = hook_16_drawFilledRect;
			pGi->drawHorizontalLine = hook_16_drawHorizontalLine;
			pGi->drawVerticalLine = hook_16_drawVerticalLine;
		}
		break;

		case 32:
		if (GX_BWindowScreen::GetInstance()->draw_line_with_32_bit_depth)
			pGi->drawAnyLine = hook_32_drawAnyLine;

		if (GX_BWindowScreen::GetInstance()->draw_rect_with_32_bit_depth)
		{
			pGi->clearBackBuffer = hook_32_clearBackBuffer;
			pGi->drawFilledRect = hook_32_drawFilledRect;
			pGi->drawHorizontalLine = hook_32_drawHorizontalLine;
			pGi->drawVerticalLine = hook_32_drawVerticalLine;
		}
		break;

		default:
		break;
	}
}

static char	*	video_top_heap;
static char	*	video_bot_heap;
static char *	video_cur_heap;

static void *video_malloc(size_t size)
{
	void *ptr = (char*)video_cur_heap;
	video_cur_heap+=size;
	return ptr;
}

static void video_initheap(void *top, size_t size)
{
	video_bot_heap = (char*)top;
	video_top_heap = video_bot_heap + size;
	video_cur_heap = video_bot_heap;
}

static void RLXAPI setPalette (uint32_t firstReg, uint32_t numRegs, const rgb24_t *src)
{
	rgb_color palette[256];
	rgb_color *dst = palette;
	int i = 0;
	for (;i<256;i++, dst++, src++)
	{
	   dst->red	= src->r;
	   dst->green = src->g;
	   dst->blue = src->b;
	}

	GX_BWindowScreen::GetInstance()->Lock();
	GX_BWindowScreen::GetInstance()->SetColorList(palette, firstReg, firstReg+numRegs-1);
	GX_BWindowScreen::GetInstance()->Unlock();
}

static void RLXAPI PageFlipMem(void)
{
	long ecx;
	uint8_t *edi = GET_GX()->View.lpFrontBuffer + GX_BWindowScreen::GetInstance()->m_Offset;
	uint8_t *esi = GET_GX()->Surfaces.lpSurface[1];
	for (ecx = GET_GX()->View.ymax;ecx!=0;ecx--)
	{
		sysMemCpy(edi, esi, GET_GX()->View.lPitch);
		esi+=GET_GX()->View.lPitch;
		edi+=GX_BWindowScreen::GetInstance()->m_Pitch;
	}
	if (GX_BWindowScreen::GetInstance()->sync && (GET_GX()->View.Flags & GX_CAPS_VSYNC))
		GX_BWindowScreen::GetInstance()->sync();
}

static void RLXAPI PageFlip(void)
{
	GET_GX()->View.State^=GX_STATE_BACKBUFFERPAGE;
	if (GX_BWindowScreen::GetInstance()->sync && (GET_GX()->View.Flags & GX_CAPS_VSYNC))
		GX_BWindowScreen::GetInstance()->sync();

	GX_BWindowScreen::GetInstance()->ShowPage(GET_GX()->View.State&GX_STATE_BACKBUFFERPAGE);
}

static GXDISPLAYMODEHANDLE SearchDisplayMode(int width, int height, int bpp)
{
	const GK_DisplayModeSettings *p = g_pDisplayModes;
	if (bpp == -1)
	   bpp = 32;

	while (((p->lWidth!=width)||(p->lHeight!=height)||(p->BitsPerPixel!=bpp))&&(p->Space!=INVALID_MODE))
		  p++;

	return (GXDISPLAYMODEHANDLE)p->Space;
}

static void GetDisplayInfo(GXDISPLAYMODEHANDLE mode)
{
	const GK_DisplayModeSettings *p = g_pDisplayModes;
	GET_GX()->View.DisplayMode = mode;
	while ((p->Space!=INVALID_MODE)&&(p->Space != mode))
	{
		p++;
	}
	if (p->Space == INVALID_MODE)
		return  ;

	GX_BWindowScreen::GetInstance()->m_DisplayMode = p;
	g_pRLX->pfSetViewPort(&GET_GX()->View, p->lWidth, p->lHeight, p->BitsPerPixel);
	GXRGBCOMPONENT &mask = GET_GX()->View.ColorMask;
	switch(p->BitsPerPixel)
	 {
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

	if (GET_GX()->View.Flags&GX_CAPS_BACKBUFFERINVIDEO)
	{
		GET_GX()->View.lPitch = GX_BWindowScreen::GetInstance()->m_Pitch;
		GET_GX()->View.Flip = PageFlip;
		GetGraphicInterface(GET_GX()->View.BitsPerPixel, &GET_GX()->gi);
	}
	else
	{
		GET_GX()->View.lPitch = GET_GX()->View.lWidth * GET_GX()->View.BytePerPixel;
		GET_GX()->View.Flip = PageFlipMem;
	}

	GET_GX()->gi.setPalette = setPalette;
}

static int SetDisplayMode(GXDISPLAYMODEHANDLE mode)
{
	int ret = 0;
	int npage = GET_GX()->View.Flags&GX_CAPS_BACKBUFFERINVIDEO ? 2 : 1;
	if ((uint32)mode != GX_BWindowScreen::GetInstance()->m_Space)
	{
		GX_BWindowScreen::GetInstance()->m_Space = mode;
		int ret = GX_BWindowScreen::GetInstance()->InitDisplayMode(GX_BWindowScreen::GetInstance()->m_DisplayMode->Space);
		if (!ret)
		{
			return -1;
		}
		else
		{
			GET_GX()->View.lpFrontBuffer = (unsigned char*)GX_BWindowScreen::GetInstance()->m_pGci->frame_buffer;
			if (GET_GX()->View.lpFrontBuffer)
				sysMemZero(GET_GX()->View.lpFrontBuffer, GX_BWindowScreen::GetInstance()->m_pGci->bytes_per_row * GX_BWindowScreen::GetInstance()->m_pGci->height);
		}

		int lx = GX_BWindowScreen::GetInstance()->m_pGci->width;
		int ly = npage * GX_BWindowScreen::GetInstance()->m_pGci->height;
		// Verifying that double buffering is possible...

		if	((GX_BWindowScreen::GetInstance()->SetFrameBuffer(lx, ly) != B_OK)
		  || (GX_BWindowScreen::GetInstance()->MoveDisplayArea(0, GX_BWindowScreen::GetInstance()->m_pGci->height)!=B_OK)
		  || (GX_BWindowScreen::GetInstance()->MoveDisplayArea(0, 0) != B_OK))
		{
			GET_GX()->View.Flags&=~GX_CAPS_BACKBUFFERINVIDEO;
			GX_BWindowScreen::GetInstance()->SetFrameBuffer(GX_BWindowScreen::GetInstance()->m_pGci->width, GX_BWindowScreen::GetInstance()->m_pGci->height);
			ret = -1;
		}
	}

	GX_BWindowScreen::GetInstance()->m_Offset = 0;
	GX_BWindowScreen::GetInstance()->MoveDisplayArea(0, 0);

	return ret;
}

__extern_c
void V3XRef_HardwareRegister(int bpp);
__end_extern_c

static int RegisterMode(GXDISPLAYMODEHANDLE mode)
{
	GET_GX()->View.DisplayMode = mode;
	GET_GX()->Client->GetDisplayInfo(mode);
	V3XRef_HardwareRegister(GET_GX()->View.BytePerPixel);
	return GET_GX()->Client->SetDisplayMode(mode);
}

static GXDISPLAYMODEINFO *EnumDisplayList(int bpp)
{
	const GK_DisplayModeSettings *p = g_pDisplayModes;
	GXDISPLAYMODEINFO *pResult = (GXDISPLAYMODEINFO*) g_pRLX->mm_heap->malloc( 64 * sizeof(GXDISPLAYMODEINFO));
	GXDISPLAYMODEINFO *dst = pResult;
	for (; p->Space != INVALID_MODE; p++)
	{
		if ((p->BitsPerPixel == bpp) || (bpp == -1))
		{
			dst->lWidth = p->lWidth;
			dst->lHeight = p->lHeight;
			dst->BitsPerPixel = p->BitsPerPixel;
			dst->mode = (GXDISPLAYMODEHANDLE)p->Space;
			dst++;
		}
	}
	sysMemSet(dst, 0, sizeof(GXDISPLAYMODEINFO));
	return pResult;
}

static void ReleaseSurfaces(void)
{
	unsigned i;
	for (i=0;i<GET_GX()->Surfaces.maxSurface;i++)
	{
		if (GET_GX()->Surfaces.flags[i] == 1)
		{
			g_pRLX->mm_std->free(GET_GX()->Surfaces.lpSurface[i]);
		}

		GET_GX()->Surfaces.lpSurface[i] = NULL;
		GET_GX()->Surfaces.flags[i] = 0;
	}
	GET_GX()->View.lpBackBuffer = NULL;
	GET_GX()->Surfaces.maxSurface	 = 0;
}

static uint8_t *Lock(void)
{
	int page = 1;

	if (GET_GX()->View.Flags&GX_CAPS_BACKBUFFERINVIDEO)
	{
		page = GET_GX()->View.State&GX_STATE_BACKBUFFERPAGE ? 0 : 1;
	}

	if (GET_GX()->View.Flags&GX_CAPS_DRAWFRONTBACK)
	{
		page ^= 1;
	}

	GET_GX()->View.lpBackBuffer = GET_GX()->Surfaces.lpSurface[ page ];
	GET_GX()->View.State |= GX_STATE_LOCKED;
	return GET_GX()->View.lpBackBuffer;
}

static void Unlock(void)
{
	GET_GX()->View.State &= ~GX_STATE_LOCKED;
	GET_GX()->View.lpBackBuffer = NULL;
}

static int CreateSurface(int pages)
{
	int i = 0;

	video_initheap(GX_BWindowScreen::GetInstance()->m_pGci->frame_buffer, GET_GX()->View.lVideoSize);

	GET_GX()->View.lpBackBuffer	= NULL;
	GET_GX()->Surfaces.maxSurface = pages;
	GET_GX()->View.State&=~GX_STATE_BACKBUFFERPAGE;

	for (i=0;i<pages;i++)
	{
		uint8_t *p = 0;
		if (i == 0 || (GET_GX()->View.Flags&GX_CAPS_BACKBUFFERINVIDEO))
		{
		   p = (uint8_t*) video_malloc(GET_GX()->View.lSurfaceSize);
		   GET_GX()->Surfaces.flags[i] = 0;
		}
        else
        {
		   p = (uint8_t*) g_pRLX->mm_std->malloc(GET_GX()->View.lSurfaceSize);
	       GET_GX()->Surfaces.flags[i] = 1;
        }

		if (!p)
			return -3;

		GET_GX()->Surfaces.lpSurface[i] = p;

	}
	return 0;
}

static void Shutdown(void)
{
	if (GX_BWindowScreen::GetInstance())
	{
		GX_BWindowScreen::GetInstance()->Lock();
		GX_BWindowScreen::GetInstance()->Quit();
	}
}

static int Open(void *hwnd)
{
	status_t error;
    UNUSED(hwnd);
    BWindow *h = new GX_BWindowScreen(g_pRLX->Dev.ApplicationName, &error);
	g_pApp->SetWindowHandle(h);
    if ( error != B_OK )
		return error;
	else
		return 0;

}

static unsigned NotifyEvent(enum GX_EVENT_MODE mode, int x, int y)
{
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

GXCLIENTDRIVER GX_BWinScrn = {
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

