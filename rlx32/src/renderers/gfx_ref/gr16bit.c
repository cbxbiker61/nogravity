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
* PROTOTYPE  :  void CALLING_C psetPixel_16bit(int32_t x, int32_t y, u_int32_t color)
*
* DESCRIPTION :  
*
*/
void CALLING_C psetPixel_16bit(int32_t x, int32_t y, u_int32_t color)
{
    GFX_word(g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*2, color);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C H_Line_16bit(int32_t x, int32_t y, int32_t l, u_int32_t color)
*
* DESCRIPTION :  
*
*/
void CALLING_C H_Line_16bit(int32_t x, int32_t y, int32_t l, u_int32_t color)
{
    if (l<0)
    {
        x+=l;
        l=-l;
    }
    GFX_memset2(g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*2, color, l );
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C V_Line_16bit(int32_t x, int32_t y, int32_t l, u_int32_t color)
*
* DESCRIPTION :  
*
*/
void CALLING_C V_Line_16bit(int32_t x, int32_t y, int32_t l, u_int32_t color)
{
    u_int8_t *v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*2;
    if (l<0)
    {
        v+=l*g_pRLX->pGX->View.lPitch;
        l=-l;
    }
    for (;l!=0;v+=g_pRLX->pGX->View.lPitch, l--)  GFX_word(v, color);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C filledRect_16bit(int32_t x, int32_t y, int32_t x2, int32_t y2, u_int32_t color)
*
* DESCRIPTION :  
*
*/
void CALLING_C filledRect_16bit(int32_t x, int32_t y, int32_t x2, int32_t y2, u_int32_t color)
{
    int32_t lx=abs(x2-x), ly=abs(y2-y);
    u_int8_t *v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*2;
    for (;ly!=0;v+=g_pRLX->pGX->View.lPitch, ly--) GFX_memset2(v, color, lx);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C shadedRect_16bit(int32_t x, int32_t y, int32_t x2, int32_t y2, void *pal)
*
* DESCRIPTION :  
*
*/
#define RGB_Luminance(a, b)        ((((a)&g_pRLX->pGX->View.RGB_Magic)+((b)&g_pRLX->pGX->View.RGB_Magic))>>1)

void CALLING_C shadedRect_16bit(int32_t x, int32_t y, int32_t x2, int32_t y2, void *pal)
{
    int32_t lx=abs(x2-x)+1, ly=abs(y2-y)+1, l;
    u_int8_t *v, *w;
    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*2;
    for (;ly!=0;v+=g_pRLX->pGX->View.lPitch, ly--)
    for (w=v, l=lx;l!=0;w+=2, l--) GFX_word(w, RGB_Luminance(*(u_int16_t*)w, g_pRLX->pGX->csp_cfg.color));
    UNUSED(pal);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  u_int32_t CALLING_C getPixel_16bit (int32_t x, int32_t y)
*
* DESCRIPTION :  
*
*/
u_int32_t CALLING_C getPixel_16bit (int32_t x, int32_t y)
{
    return GFX_read(g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*2);
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C wiredRect_16bit(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
*
* DESCRIPTION :  
*
*/
void CALLING_C wiredRect_16bit(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
{
    H_Line_16bit(x1, y1, x2-x1, colour);
    H_Line_16bit(x1, y2, x2-x1, colour);
    V_Line_16bit(x1, y1, y2-y1, colour);
    V_Line_16bit(x2, y1, y2-y1, colour);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C meshedRect_16bit(int32_t x, int32_t y, int32_t x2, int32_t y2, u_int32_t colour)
*
* DESCRIPTION :  
*
*/
void CALLING_C meshedRect_16bit(int32_t x, int32_t y, int32_t x2, int32_t y2, u_int32_t colour)
{
    int32_t lx=abs(x2-x)+1, 
    ly=abs(y2-y)+1;
    u_int8_t *v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*2;
    for (;ly!=0;
    v+=g_pRLX->pGX->View.lPitch, 
    ly--)
    {
        u_int16_t *w=(u_int16_t*)v+(ly&1);
        int32_t l=lx>>1;
        for (;l!=0;
        w+=2, 
        l--) GFX_word(w, colour);
    }
    return;
}

static GXGRAPHICINTERFACE GXDrv_16bit={
    B_Line_8bit, 
    A_Line_8bit, 
	H_Line_16bit, 
    V_Line_16bit, 
    wiredRect_16bit, 
    shadedRect_16bit, 
    meshedRect_16bit, 
    filledRect_16bit, 
    psetPixel_16bit, 
	psetPixel_16bit, 
    getPixel_16bit
};

void GX_GetGraphicInterfaceRef16(struct GXSYSTEM *pGX, GXGRAPHICINTERFACE *p)
{
	*p = GXDrv_16bit;
}
