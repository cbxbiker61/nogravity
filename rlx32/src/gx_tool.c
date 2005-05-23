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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "systools.h"
#include "sysresmx.h"
#include "fixops.h"
#include "gx_struc.h"
#include "gx_init.h"
#include "gx_tools.h"
#include "gx_csp.h"
#include "gx_rgb.h"

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void IMG_stretch(u_int8_t *old_buf, u_int8_t *new_buf, int old_ly, int new_ly, int old_lx, int new_lx, int bytes)
*
* DESCRIPTION :
*
*/
void IMG_stretch(u_int8_t *old_buf, u_int8_t *new_buf, int old_ly, int new_ly, int old_lx, int new_lx, int bytes)
{
    u_int8_t *v=new_buf, *w;
    u_int16_t *v0 = (u_int16_t*)new_buf, *old_buf0=(u_int16_t*)old_buf, *w0;
    u_int32_t  *v1 = (u_int32_t*)new_buf, *old_buf1=(u_int32_t*)old_buf, *w1;
    rgb24_t *v2 = (    rgb24_t*)new_buf, *old_buf2=(    rgb24_t*)old_buf, *w2;
    // u_int32_t *v1 = (u_int16_t*)new_buf, *old_buf1=(u_int16_t*)old_buf, *w1;
    int32_t i, j;
    int32_t oy, dy=VDIV(old_ly, new_ly);
    int32_t ox, dx=VDIV(old_lx, new_lx);
    if ((old_ly==new_ly) && (old_lx==new_lx))
    {
        sysMemCpy(new_buf, old_buf, new_lx*new_ly*bytes);
        return;
    }
    switch(bytes) {
        case 1:
        for (oy=0, i=new_ly;i!=0;oy+=dy, i--)
        {
            w = old_buf + (oy>>16)*old_lx ;
            for (ox=0, j=new_lx;j!=0;ox+=dx, j--) *(v++)=w[ox>>16];
        }
        break;
        case 2:
        for (oy=0, i=new_ly;i!=0;oy+=dy, i--)
        {
            w0 = old_buf0 + (oy>>16)*old_lx ;
            for (ox=0, j=new_lx;j!=0;ox+=dx, v0++, j--)
            {
                u_int16_t *wx=w0+(ox>>16);
                *v0 = *wx;
            }
        }
        break;
        case 3:
        for (oy=0, i=new_ly;i!=0;oy+=dy, i--)
        {
            w2 = old_buf2 + (oy>>16)*old_lx ;
            for (ox=0, j=new_lx;j!=0;ox+=dx, j--) *(v2++)=w2[ox>>16];
        }
        break;
        case 4:
        for (oy=0, i=new_ly;i!=0;oy+=dy, i--)
        {
            w1 = old_buf1 + (oy>>16)*old_lx ;
            for (ox=0, j=new_lx;j!=0;ox+=dx, v1++, j--)
            {
                u_int32_t *wx = w1+(ox>>16);
                *v1 = *wx;
            }
        }
        break;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void IMG_pan(u_int8_t *old_buf, u_int8_t *new_buf, int old_ly, int new_ly, int old_lx, int new_lx)
*
* DESCRIPTION :   Modify the canvas size of a buffer
*
*/
void IMG_pan(u_int8_t *old_buf, u_int8_t *new_buf, int old_ly, int new_ly, int old_lx, int new_lx, int bytes)
{
    u_int8_t *v=new_buf, *w=old_buf;
    int32_t i, j;
	i=(old_ly<new_ly) ? old_ly : new_ly;
	j=(old_lx<new_lx) ? old_lx : new_lx;
    for (;i!=0;v+=new_lx*bytes, w+=old_lx*bytes, i--) sysMemCpy(v, w, j*bytes);
    old_ly++;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void IMG_rotate_image(u_int8_t *old, u_int8_t *tmp, int lx, int ly)
*
* DESCRIPTION :
*
*/
void IMG_rotate_image(u_int8_t *old, u_int8_t *tmp, int lx, int ly)
{
    u_int8_t *v=old, *w=tmp;
    int i, j;
    for (i=lx;i!=0;i--, old++)
    {
        v=old;
        for (j=ly;j!=0;j--, w++, v+=lx)  *w=*v;
    }
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void RGB_16to15(u_int16_t *map, u_int32_t sz)
*
* DESCRIPTION :
*
*/
static void RGB_15to16(u_int16_t *map, u_int32_t sz)
{
    rgb24_t a;
    for (;sz!=0;map++, sz--)
    {
        RGB_GetPixelFormat(&a, *map);
        *map = (u_int16_t)((a.b>>3) + ((a.g>>3)<<5) + ((a.r>>3)<<10));
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CSP_CaptureFrom(int32_t xx, int32_t yy, GXSPRITE *capt)
*
* DESCRIPTION :
*
*/
void CSP_CaptureFrom(int32_t xx, int32_t yy, GXSPRITE *capt)
{
    u_int8_t *v=GX.View.lpBackBuffer+(yy*GX.View.lPitch)+xx*GX.View.BytePerPixel, *w=capt->data;
    u_int32_t cx, lx=GX.View.BytePerPixel*capt->LX;
    for (cx=capt->LY;cx>0;cx--)
    {
        sysMemCpy(w, v, lx);
        w+=(int32_t)lx;
        v+=GX.View.lPitch;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  #define MAT_R(n) (u_int32_t) (Mat[n].r)
*
* DESCRIPTION :
*
*/
#define MAT_R(n) (u_int32_t) (Mat[n].r)
#define MAT_G(n) (u_int32_t) (Mat[n].g)
#define MAT_B(n) (u_int32_t) (Mat[n].b)
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void IMG_StretchBilinear(GXSPRITE *_new, GXSPRITE *_old, int bp, int strides)
*
* DESCRIPTION :  
*
*/
void IMG_StretchBilinear(GXSPRITE *_new, GXSPRITE *_old, int bp, int strides)
{
    int32_t dy = VDIV(_old->LY, _new->LY);
    int32_t dx = VDIV(_old->LX, _new->LX);
    int32_t old_sz = _old->LX * _old->LY * bp;
    int32_t new_sz = _new->LX * _new->LY * bp;
    if (!_new->data) _new->data = (u_int8_t*) MM_heap.malloc(new_sz);
    if (old_sz == new_sz)
    {
        sysMemCpy(_new->data, _old->data, old_sz);
        return;
    }
    {
        u_int8_t *v = (u_int8_t*)_new->data;
        int32_t i, j, y, x;
        for (y=0, i=_new->LY;i!=0;y+=dy, i--)
        {
            u_int32_t ofY = (y * _old->LX) >> 16;
            for (x=0, j=_new->LX;j!=0;x+=dx, v+=bp, j--)
            {
                rgb24_t e;
                rgb24_t Mat[9];
                u_int32_t off = (x>>16) + ofY;
                u_int8_t *ww = _old->data + off * bp;
                if ((_old->LX-1<off)&&(off<(u_int32_t)(old_sz - (int32_t)_old->LX+1)))
                {
                    switch(bp) {
                        case 1:
                        Mat[0] = GX.ColorTable[ww[ -(int32_t)_old->LX-1 ]];
                        Mat[1] = GX.ColorTable[ww[ -(int32_t)_old->LX   ]];
                        Mat[2] = GX.ColorTable[ww[ -(int32_t)_old->LX+1 ]];
                        Mat[3] = GX.ColorTable[ww[      -1     ]];
                        Mat[4] = GX.ColorTable[ww[       0     ]];
                        Mat[5] = GX.ColorTable[ww[       1     ]];
                        Mat[6] = GX.ColorTable[ww[  _old->LX-1 ]];
                        Mat[7] = GX.ColorTable[ww[  _old->LX   ]];
                        Mat[8] = GX.ColorTable[ww[  _old->LX+1 ]];
                        break;
                        case 2:
                        {
                            u_int16_t *w0 = (u_int16_t*)ww;
                            RGB_GetPixelFormat(Mat+0, w0[ -(int32_t)_old->LX-1]);
                            RGB_GetPixelFormat(Mat+1, w0[ -(int32_t)_old->LX  ]);
                            RGB_GetPixelFormat(Mat+2, w0[ -(int32_t)_old->LX+1]);
                            RGB_GetPixelFormat(Mat+3, w0[     -1     ]);
                            RGB_GetPixelFormat(Mat+4, w0[      0     ]);
                            RGB_GetPixelFormat(Mat+5, w0[      1     ]);
                            RGB_GetPixelFormat(Mat+6, w0[  _old->LX-1]);
                            RGB_GetPixelFormat(Mat+7, w0[  _old->LX  ]);
                            RGB_GetPixelFormat(Mat+8, w0[  _old->LX+1]);
                        }
                        break;
                    }
                    e.r = (u_int8_t)(( MAT_R(0)   + MAT_R(1)*2 + MAT_R(2)
                    + MAT_R(3)*2 + MAT_R(4)*4 + MAT_R(5)*2
                    + MAT_R(6)   + MAT_R(7)*2 + MAT_R(8)   )>>4);
                    e.g = (u_int8_t)(( MAT_G(0)   + MAT_G(1)*2 + MAT_G(2)
                    + MAT_G(3)*2 + MAT_G(4)*4 + MAT_G(5)*2
                    + MAT_G(6)   + MAT_G(7)*2 + MAT_G(8)   )>>4);
                    e.b = (u_int8_t)(( MAT_B(0)   + MAT_B(1)*2 + MAT_B(2)
                    + MAT_B(3)*2 + MAT_B(3)*4 + MAT_B(5)*2
                    + MAT_B(6)   + MAT_B(6)*2 + MAT_B(8)   )>>4);
                    if (bp==1) *v = (u_int8_t)RGB_findNearestColor(&e, GX.ColorTable);
                    else *(u_int16_t*)v = (u_int16_t)(RGB_PixelFormat(e.r, e.g, e.b));
                }
                else
                {
                    if (bp==1)    *v = *ww;
                    else *(u_int16_t*)v = *(u_int16_t*)ww;
                }
            }
            v+=strides;
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void IMG_StretchPoint(GXSPRITE *p, GXSPRITE *sp3, int bp, int pitch)
*
* DESCRIPTION :  
*
*/
void IMG_StretchPoint(GXSPRITE *p, GXSPRITE *sp3, int bp, int pitch)
{
    if (!p->data)
    p->data = (u_int8_t*)MM_heap.malloc(p->LX*p->LY*bp);
    IMG_stretch(sp3->data, p->data, sp3->LY, p->LY, sp3->LX, p->LX, bp);
    UNUSED(pitch);
    return;
}
