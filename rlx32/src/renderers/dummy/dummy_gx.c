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

static void CALLING_C drawAnyLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
{
    return;
}

static void CALLING_C drawHorizontalLine(int32_t x1, int32_t y1, int32_t lx, u_int32_t colour)
{
    return;
}

static void CALLING_C drawVerticalLine(int32_t x1, int32_t y1, int32_t lx, u_int32_t colour)
{
    return;
}

static void CALLING_C drawWiredRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
{
    return;
}

static void CALLING_C drawShadedRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, void *palette)
{
    return;
}

static void CALLING_C drawMeshedRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
{
    return;
}

static void CALLING_C drawFilledRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
{
    return;
}

static void CALLING_C drawPixel(int32_t x, int32_t y, u_int32_t colour)
{
    return;
}

static void CALLING_C clearBackBuffer(void)
{
    return;
}

static void CALLING_C clearVideo(void)
{
    return;
}

static void CALLING_C waitDrawing(void)
{
    return;
}

static void CALLING_C setPalette(u_int32_t a, u_int32_t b, void * pal)
{
    return;
}

static u_int32_t CALLING_C getPixel(int32_t x, int32_t y)
{
    return 0;
}

static void CALLING_C blit(u_int32_t dest, u_int32_t src)
{
}

static void CALLING_C setCursor(int32_t x, int32_t y)
{

}

static void CALLING_C copyCursor(u_int8_t *pImage)
{
}

static void DrawSpritePoly(int32_t x, int32_t y, GXSPRITE *sp, int mode)
{
    return;
}

static void CALLING_C gl_cspput(int32_t x, int32_t y, GXSPRITE *sp)
{
    DrawSpritePoly(x, y, sp, 1);
}

static void CALLING_C gl_csppset(int32_t x, int32_t y, GXSPRITE *sp)
{
    DrawSpritePoly(x, y, sp, 2);
}

static void CALLING_C gl_cspadd(int32_t x, int32_t y, GXSPRITE *sp)
{
    DrawSpritePoly(x, y, sp, 3);
}

static void CALLING_C gl_csp50(int32_t x, int32_t y, GXSPRITE *sp)
{
    DrawSpritePoly(x, y, sp, 4);
}

static void CALLING_C gl_cspsub(int32_t x, int32_t y, GXSPRITE *sp)
{
    DrawSpritePoly(x, y, sp, 5);
}

static void CALLING_C gl_cspalpha(int32_t x, int32_t y, GXSPRITE *sp)
{
    DrawSpritePoly(x, y, sp, 4);
}

static void DrawSpritePolyZoom(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly, int mode)
{
    return;
}

static void CALLING_C gl_cspput_zoom(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly)
{
    DrawSpritePolyZoom(sp, x,y, lx, ly, 1);
}

static void CALLING_C gl_csppset_zoom(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly)
{
    DrawSpritePolyZoom(sp, x,y, lx, ly, 2);
}

static void CALLING_C gl_cspadd_zoom(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly)
{
    DrawSpritePolyZoom(sp, x,y, lx, ly, 3);
}

static void CALLING_C gl_csp50_zoom(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly)
{
    DrawSpritePolyZoom(sp, x,y, lx, ly, 4);
}

static void CALLING_C gl_cspsub_zoom(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly)
{
    DrawSpritePolyZoom(sp, x,y, lx, ly, 5);
}

static void CALLING_C gl_cspalpha_zoom(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly)
{
    DrawSpritePolyZoom(sp, x,y, lx, ly, 4);
}


static void RLXAPI UploadSprite(GXSPRITE *sp, rgb24_t *colorTable, int bpp)
{
    return;
}

static void RLXAPI FreeSprite(GXSPRITE *sp)
{
    return;
}

static unsigned RLXAPI UpdateSprite(GXSPRITE *sp, const u_int8_t *bitmap, const rgb24_t *colorTable)
{
    return 0;
}

GXGRAPHICINTERFACE GI_GS =
{
    drawAnyLine, 
	drawAnyLine, 
    drawHorizontalLine, 
    drawVerticalLine, 
    drawWiredRect, 
    drawShadedRect, 
    drawMeshedRect, 
    drawFilledRect, 
    drawPixel, 
	drawPixel, 
    getPixel, 
    clearBackBuffer, 
    clearVideo, 
    blit,
    waitDrawing, 
    setPalette, 
    setCursor, 
    copyCursor
};

GXSPRITEINTERFACE CSP_GS = {
    0,
    gl_cspput,
    gl_csppset,
    gl_csppset,
    //
    gl_csp50,
    gl_cspadd,
    gl_cspsub,
    gl_cspalpha,
    //
    gl_csppset_zoom,
    gl_cspput_zoom,
    gl_csp50_zoom,
    gl_cspadd_zoom,
    gl_cspsub_zoom,
    gl_cspalpha_zoom
};

GXCLIENTDRIVER GX_GS;

void SetPrimitiveSprites()
{
	GX_GS.UploadSprite = UploadSprite;
	GX_GS.UpdateSprite = UpdateSprite;
	GX_GS.ReleaseSprite = FreeSprite;
}

void V3X_EntryPoint(struct RLXSYSTEM *p)
{
}
