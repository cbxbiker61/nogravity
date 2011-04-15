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
#include <stdio.h>
#include <stdlib.h>
#include "_rlx32.h"
#include "systools.h"
#include "gx_struc.h"
#include "gx_rgb.h"
#include "sysresmx.h"
#include "gx_tools.h"
#include "gx_csp.h"
#include "gx_c.h"
#include "_rlx.h"

extern struct RLXSYSTEM *g_pRLX;

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C psetPixel_32bit(int32_t x, int32_t y, uint32_t color)
*
* DESCRIPTION :
*
*/
void CALLING_C psetPixel_32bit(int32_t x, int32_t y, uint32_t color)
{
    GFX_dword(g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*sizeof(uint32_t), color);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C H_Line_32bit(int32_t x, int32_t y, int32_t l, uint32_t color)
*
* DESCRIPTION :
*
*/
void CALLING_C H_Line_32bit(int32_t x, int32_t y, int32_t l, uint32_t color)
{
    if (l<0)
    {
        x+=l;
        l=-l;
    }
    GFX_memset4(g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*sizeof(uint32_t), color, l );
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C V_Line_32bit(int32_t x, int32_t y, int32_t l, uint32_t color)
*
* DESCRIPTION :
*
*/
void CALLING_C V_Line_32bit(int32_t x, int32_t y, int32_t l, uint32_t color)
{
    uint8_t *v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*sizeof(uint32_t);
    if (l<0)
    {
        v+=l*g_pRLX->pGX->View.lPitch;
        l=-l;
    }
    for (;l!=0;v+=g_pRLX->pGX->View.lPitch, l--)  GFX_dword(v, color);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C filledRect_32bit(int32_t x, int32_t y, int32_t x2, int32_t y2, uint32_t color)
*
* DESCRIPTION :
*
*/
void CALLING_C filledRect_32bit(int32_t x, int32_t y, int32_t x2, int32_t y2, uint32_t color)
{
    int32_t lx=abs(x2-x), ly=abs(y2-y);
    uint8_t *v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*sizeof(uint32_t);
    for (;ly!=0;v+=g_pRLX->pGX->View.lPitch, ly--) GFX_memset4(v, color, lx);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C shadedRect_32bit(int32_t x, int32_t y, int32_t x2, int32_t y2, void *pal)
*
* DESCRIPTION :
*
*/
#define RGB_Luminance(a, b)        ((((a)&g_pRLX->pGX->View.RGB_Magic)+((b)&g_pRLX->pGX->View.RGB_Magic))>>1)

void CALLING_C shadedRect_32bit(int32_t x, int32_t y, int32_t x2, int32_t y2, void *pal)
{
    int32_t lx=abs(x2-x)+1, ly=abs(y2-y)+1, l;
    uint8_t *v, *w;
    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*sizeof(uint32_t);
    for (;ly!=0;v+=g_pRLX->pGX->View.lPitch, ly--)
    for (w=v, l=lx;l!=0;w+=sizeof(uint32_t), l--)
		GFX_dword(w, RGB_Luminance(*(uint32_t*)w, g_pRLX->pGX->csp_cfg.color));
    UNUSED(pal);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  uint32_t CALLING_C getPixel_32bit (int32_t x, int32_t y)
*
* DESCRIPTION :
*
*/
uint32_t CALLING_C getPixel_32bit (int32_t x, int32_t y)
{
    return GFX_read(g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*sizeof(uint32_t));
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C wiredRect_32bit(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t colour)
*
* DESCRIPTION :
*
*/
void CALLING_C wiredRect_32bit(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t colour)
{
    H_Line_32bit(x1, y1, x2-x1, colour);
    H_Line_32bit(x1, y2, x2-x1, colour);
    V_Line_32bit(x1, y1, y2-y1, colour);
    V_Line_32bit(x2, y1, y2-y1, colour);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C meshedRect_32bit(int32_t x, int32_t y, int32_t x2, int32_t y2, uint32_t colour)
*
* DESCRIPTION :
*
*/
void CALLING_C meshedRect_32bit(int32_t x, int32_t y, int32_t x2, int32_t y2, uint32_t colour)
{
    int32_t lx=abs(x2-x)+1,
    ly=abs(y2-y)+1;
    uint8_t *v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*sizeof(uint32_t);
    for (;ly!=0;
    v+=g_pRLX->pGX->View.lPitch,
    ly--)
    {
        uint32_t *w=(uint32_t*)v+(ly&1);
        int32_t l=lx>>1;
        for (;l!=0;
        w+=2,
        l--) GFX_dword(w, colour);
    }
}

static GXGRAPHICINTERFACE GXDrv_32bit={
    B_Line_8bit,
    A_Line_8bit,
	H_Line_32bit,
    V_Line_32bit,
    wiredRect_32bit,
    shadedRect_32bit,
    meshedRect_32bit,
    filledRect_32bit,
    psetPixel_32bit,
	psetPixel_32bit,
    getPixel_32bit
};

void GX_GetGraphicInterfaceRef32(struct GXSYSTEM *pGX, GXGRAPHICINTERFACE *p)
{
	*p = GXDrv_32bit;
}

