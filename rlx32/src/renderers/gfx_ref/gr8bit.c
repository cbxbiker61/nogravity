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
* PROTOTYPE  :  void CALLING_C psetPixel_8bit(int32_t x, int32_t y, uint32_t color)
*
* DESCRIPTION :
*
*/
void CALLING_C psetPixel_8bit(int32_t x, int32_t y, uint32_t color)
{
    GFX_byte(g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x, color);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C H_Line_8bit(int32_t x, int32_t y, int32_t l, uint32_t color)
*
* DESCRIPTION :
*
*/
void CALLING_C H_Line_8bit(int32_t x, int32_t y, int32_t l, uint32_t color)
{
    if (l<0)
    {
        x+=l;
        l=-l;
    }
    GFX_memset(g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x, color, l );
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C V_Line_8bit(int32_t x, int32_t y, int32_t l, uint32_t color)
*
* DESCRIPTION :
*
*/
void CALLING_C V_Line_8bit(int32_t x, int32_t y, int32_t l, uint32_t color)
{
    uint8_t *v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x;
    if (l<0)
    {
        v+=l*g_pRLX->pGX->View.lPitch;
        l=-l;
    }
    for (;l!=0;v+=g_pRLX->pGX->View.lPitch, l--)  GFX_byte(v, color);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C filledRect_8bit(int32_t x, int32_t y, int32_t x2, int32_t y2, uint32_t color)
*
* DESCRIPTION :
*
*/
void CALLING_C filledRect_8bit(int32_t x, int32_t y, int32_t x2, int32_t y2, uint32_t color)
{
    int32_t lx=1+abs(x2-x), ly=1+abs(y2-y);
    uint8_t *v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x;
    for (;ly!=0;v+=g_pRLX->pGX->View.lPitch, ly--)
    GFX_memset(v, color, lx);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C shadedRect_8bit(int32_t x, int32_t y, int32_t x2, int32_t y2, void *pal)
*
* DESCRIPTION :
*
*/
void CALLING_C shadedRect_8bit(int32_t x, int32_t y, int32_t x2, int32_t y2, void *pal)
{
    int32_t lx=abs(x2-x)+1, ly=abs(y2-y)+1, l;
    uint8_t *v, *w, *real = (uint8_t*)pal;
    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x;
    for (;ly!=0;v+=g_pRLX->pGX->View.lPitch, ly--)
    for (w=v, l=lx;l!=0;w++, l--) GFX_byte(w, real[GFX_read(w)]);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  uint32_t CALLING_C getPixel_8bit (int32_t x, int32_t y)
*
* DESCRIPTION :
*
*/
uint32_t CALLING_C getPixel_8bit (int32_t x, int32_t y)
{
    return GFX_read(g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C meshedRect_8bit(int32_t x, int32_t y, int32_t x2, int32_t y2, uint32_t colour)
*
* DESCRIPTION :
*
*/
void CALLING_C meshedRect_8bit(int32_t x, int32_t y, int32_t x2, int32_t y2, uint32_t colour)
{
    int32_t lx=abs(x2-x)+1,
    ly=abs(y2-y)+1;
    uint8_t *v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x;
    for (;ly!=0;
    v+=g_pRLX->pGX->View.lPitch,
    ly--)
    {
        uint8_t *w=v+(ly&1);
        int32_t l=lx>>1;
        for (;l!=0;
        w+=2,
        l--) GFX_byte(w, colour);
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C wiredRect_8bit(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t colour)
*
* DESCRIPTION :
*
*/
void CALLING_C wiredRect_8bit(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t colour)
{
    H_Line_8bit(x1, y1, x2-x1, colour);
    H_Line_8bit(x1, y2, x2-x1, colour);
    V_Line_8bit(x1, y1, y2-y1, colour);
    V_Line_8bit(x2, y1, y2-y1, colour);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void CALLING_C B_Line_8bit(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t colour)
{
    int32_t d, dx, dy,
    aincr, bincr,
    xincr, yincr,
    x, y;
    if( abs(x2-x1) < abs(y2-y1) )
    {
        if( y1 > y2 )
        {
            XCHG( x1, x2, int32_t );
            XCHG( y1, y2, int32_t );
        }
        xincr = ( x2 > x1 ) ?  1 : -1;
        dy = y2 - y1;
        dx = abs( x2-x1 );
        d = 2 * dx - dy;
        aincr = 2 * (dx - dy);
        bincr = 2 * dx;
        x = x1;
        y = y1;
        g_pRLX->pGX->gi.drawPixel( x, y, colour );
        for( y=y1+1; y<= y2; ++y )
        {
            if ( d >= 0 )
            {
                x += xincr;
                d += aincr;
            }
            else
            d += bincr;
            g_pRLX->pGX->gi.drawPixel( x, y, colour );
        }
    }
    else
    {
        if( x1 > x2 )
        {
            XCHG( x1, x2, int32_t );
            XCHG( y1, y2, int32_t );
        }
        yincr = ( y2 > y1 ) ? 1 : -1;
        dx = x2 - x1;
        dy = abs( y2-y1 );
        d = 2 * dy - dx;
        aincr = 2 * (dy - dx);
        bincr = 2 * dy;
        x = x1;
        y = y1;
        g_pRLX->pGX->gi.drawPixel( x, y, colour );
        for(x=x1+1; x<=x2; ++x )
        {
            if( d >= 0 )
            {
                y += yincr;
                d += aincr;
            }
            else
            d += bincr;
            g_pRLX->pGX->gi.drawPixel( x, y, colour );
        }
    }
}

void CALLING_C A_Line_8bit(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t colour)
{
    int32_t d, dx, dy,
    aincr, bincr,
    xincr, yincr,
    x, y;
    if( abs(x2-x1) < abs(y2-y1) )
    {
        if( y1 > y2 )
        {
            XCHG( x1, x2, int32_t );
            XCHG( y1, y2, int32_t );
        }
        xincr = ( x2 > x1 ) ?  1 : -1;
        dy = y2 - y1;
        dx = abs( x2-x1 );
        d = 2 * dx - dy;
        aincr = 2 * (dx - dy);
        bincr = 2 * dx;
        x = x1;
        y = y1;
        g_pRLX->pGX->gi.TrspPixel( x, y, colour );
        for( y=y1+1; y<= y2; ++y )
        {
            if ( d >= 0 )
            {
                x += xincr;
                d += aincr;
            }
            else
            d += bincr;
            g_pRLX->pGX->gi.TrspPixel( x, y, colour );
        }
    }
    else
    {
        if( x1 > x2 )
        {
            XCHG( x1, x2, int32_t );
            XCHG( y1, y2, int32_t );
        }
        yincr = ( y2 > y1 ) ? 1 : -1;
        dx = x2 - x1;
        dy = abs( y2-y1 );
        d = 2 * dy - dx;
        aincr = 2 * (dy - dx);
        bincr = 2 * dy;
        x = x1;
        y = y1;
        g_pRLX->pGX->gi.TrspPixel( x, y, colour );
        for(x=x1+1; x<=x2; ++x )
        {
            if( d >= 0 )
            {
                y += yincr;
                d += aincr;
            }
            else
            d += bincr;
            g_pRLX->pGX->gi.TrspPixel( x, y, colour );
        }
    }
}

static GXGRAPHICINTERFACE GXDrv_8bit={
    B_Line_8bit,
	A_Line_8bit,
    H_Line_8bit,
    V_Line_8bit,
    wiredRect_8bit,
    shadedRect_8bit,
    meshedRect_8bit,
    filledRect_8bit,
    psetPixel_8bit,
	psetPixel_8bit,
    getPixel_8bit
};

void GX_GetGraphicInterfaceRef8(struct GXSYSTEM *pGX, GXGRAPHICINTERFACE *p)
{
	*p = GXDrv_8bit;
}

