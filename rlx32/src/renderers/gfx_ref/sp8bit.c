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
#include "_rlx32.h"
#include "systools.h"
#include "gx_struc.h"
#include "gx_rgb.h"
#include "sysresmx.h"
#include "gx_tools.h"
#include "gx_csp.h"
#include "_rlx.h"
extern struct RLXSYSTEM *g_pRLX;

#include "gx_c.h"
#include "sp.h"

#define Tsize u_int8_t

static __inline void PUT(u_int8_t *v, Tsize c)
{
	*(Tsize*)v = c;
}

static __inline void PSET(u_int8_t *v, Tsize c)
{
	*(Tsize*)v = c;
}

static __inline void ADD(u_int8_t *v, Tsize c)
{
	*(Tsize*)v = c;
}

static __inline void ALPHA(u_int8_t *v, Tsize c)
{
	*(Tsize*)v = c;
}

static void CALLING_C pset (int32_t x, int32_t y, GXSPRITE *sp)
{
	GXSPRITESW *p = (GXSPRITESW*)sp->handle;
    u_int8_t *v;    
    int32_t oy=0, ox=0, lx=sp->LX, ly=sp->LY;
	int d;
    /*=============================================================*/
    SPRITE_CLIPPING
    /*=============================================================*/
	d = g_pRLX->pGX->View.lPitch - lx*sizeof(Tsize);
    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + sizeof(Tsize) * x;
	if (p->bpp == 1)
	{
		u_int8_t *u = (u_int8_t*)sp->data + oy * (int32_t)sp->LX + ox;
		int i;
		for (i=0;i<ly;i++,v+=d, u+=sp->LX)
		{
			int j;
			for (j=0;j<lx;j++,v+=sizeof(Tsize)) 
				PSET(v, (Tsize)p->palette[u[j]]);
		}
	}
    return;
   
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C put (int32_t x, int32_t y, GXSPRITE *sp)
*
* DESCRIPTION :  
*
*/
static void CALLING_C put (int32_t x, int32_t y, GXSPRITE *sp)
{
    GXSPRITESW *p = (GXSPRITESW*)sp->handle;
    u_int8_t *v;    
	int d = 0;
    int32_t oy=0, ox=0, lx=sp->LX, ly=sp->LY;
    /*=============================================================*/
    SPRITE_CLIPPING
    /*=============================================================*/
    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + sizeof(Tsize) * x;
	d = g_pRLX->pGX->View.lPitch - lx*sizeof(Tsize);
	if (p->bpp == 1)
	{
		u_int8_t *u = (u_int8_t*)sp->data + oy * (int32_t)sp->LX + ox;
		int i;
		for (i=0;i<ly;i++,v+=d, u+=sp->LX)
		{
			int j;
			for (j=0;j<lx;j++,v+=sizeof(Tsize)) 				
				if (u[j])
					PUT(v, (Tsize)p->palette[u[j]]);
		}
	}
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C TrspADDbit (int32_t x, int32_t y, GXSPRITE *sp)
*
* DESCRIPTION :  
*
*/

static void CALLING_C Trsp50 (int32_t x, int32_t y, GXSPRITE *sp)
{
    GXSPRITESW *p = (GXSPRITESW*)sp->handle;
    u_int8_t *v;    
    int32_t oy=0, ox=0, lx=sp->LX, ly=sp->LY;
	int d = 0;
    /*=============================================================*/
    SPRITE_CLIPPING
    /*=============================================================*/
    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + sizeof(Tsize) * x;
	d = g_pRLX->pGX->View.lPitch - lx*sizeof(Tsize);
	if (p->bpp == 1)
	{
		u_int8_t *u = (u_int8_t*)sp->data + oy * (int32_t)sp->LX + ox;
		int i;
		for (i=0;i<ly;i++,v+=d, u+=sp->LX)
		{
			int j;
			for (j=0;j<lx;j++,v+=sizeof(Tsize)) 
				ALPHA(v, (Tsize)p->palette[u[j]]);
		}
	}
  
    return;
}

static void CALLING_C TrspADD (int32_t x, int32_t y, GXSPRITE *sp)
{
    GXSPRITESW *p = (GXSPRITESW*)sp->handle;
    u_int8_t *v;    
    int32_t oy=0, ox=0, lx=sp->LX, ly=sp->LY;
	int d = 0;
    /*=============================================================*/
    SPRITE_CLIPPING
    /*=============================================================*/
    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + sizeof(Tsize) * x;
	d = g_pRLX->pGX->View.lPitch - lx*sizeof(Tsize);
	if (p->bpp == 1)
	{
		u_int8_t *u = (u_int8_t*)sp->data + oy * (int32_t)sp->LX + ox;
		int i;
		for (i=0;i<ly;i++,v+=d, u+=sp->LX)
		{
			int j;
			for (j=0;j<lx;j++,v+=sizeof(Tsize)) 
				ADD(v, (Tsize)p->palette[u[j]]);
		}
	}
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/

static void CALLING_C zoom_pset(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly)
{
    u_int8_t *v = g_pRLX->pGX->View.lpBackBuffer + x * sizeof(Tsize) + g_pRLX->pGX->View.lPitch * y;   
	int d = g_pRLX->pGX->View.lPitch - lx*sizeof(Tsize);

	GXSPRITESW *p = (GXSPRITESW*)sp->handle;
    int32_t dy = (sp->LY<<16) / ly;
    int32_t dx = (sp->LX<<16) / lx;

	if (p->bpp == 1)
	{
		int y = 0;
		int ey = dy * ly;
		int ex = dx * lx;
		for (;y<ey;y+=dy, v+=d)
		{
			u_int8_t *w = (u_int8_t*)sp->data + (y>>16) * sp->LX ;			
			int x = 0;
			for (x=0; x<ex; x+=dx, v+=sizeof(Tsize)) 			
				PSET(v, (Tsize)p->palette[w[x>>16]]);
		}
	}
	
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C zoom_put(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly)
*
* DESCRIPTION :  
*
*/

static void CALLING_C zoom_put(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly)
{
    GXSPRITESW *p = (GXSPRITESW*)sp->handle; 
    u_int8_t *v;
    int32_t j;
    int32_t oy=0, ox=0, dx, dy, d;
    //=============================================================
	SPRITE_CLIPPING_2
    //=============================================================
    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*sizeof(Tsize);
    d = g_pRLX->pGX->View.lPitch - lx * sizeof(Tsize);

  	if (p->bpp == 1)
	{
		int y = 0;
		int ey = dy * ly;
		int ex = dx * lx;
		for (;y<ey;y+=dy, v+=d)
		{
			const u_int8_t *w = (u_int8_t*)sp->data + (y>>16) * sp->LX ;			
			int x = 0;
			for (x=0; x<ex; x+=dx, v+=sizeof(Tsize)) 
			{
				const u_int8_t *t = w + (x>>16);
				if (*t)				
					PUT(v, (Tsize)p->palette[*t]);
			}
		}
	}
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C zoom_TrspAdd(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly, u_int8_t **real)
*
* DESCRIPTION :  
*
*/

static void CALLING_C zoom_TrspADD(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly)
{    
    GXSPRITESW *p = (GXSPRITESW*)sp->handle;
	u_int8_t *v;
    int32_t j;
    int32_t oy=0, ox=0, dx, dy, d;
      //=============================================================
	SPRITE_CLIPPING_2
    //=============================================================

    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*sizeof(Tsize);
    d = g_pRLX->pGX->View.lPitch - lx * sizeof(Tsize);

  	if (p->bpp == 1)
	{
		int y = 0;
		int ey = dy * ly;
		int ex = dx * lx;
		for (;y<ey;y+=dy, v+=d)
		{
			u_int8_t *w = (u_int8_t*)sp->data + (y>>16) * sp->LX ;			
			int x = 0;
			for (x=0; x<ex; x+=dx, v+=sizeof(Tsize)) 
				ADD(v, (Tsize)p->palette[w[x>>16]]);
		}
	}
    return;
}

static void CALLING_C zoom_Trsp50(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly)
{
    GXSPRITESW *p = (GXSPRITESW*)sp->handle;
    u_int8_t *v;
    int32_t j;
    int32_t oy=0, ox=0, dx, dy, d;
      //=============================================================
	SPRITE_CLIPPING_2
    //=============================================================

    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*sizeof(Tsize);
    d = g_pRLX->pGX->View.lPitch - lx * sizeof(Tsize);

  	if (p->bpp == 1)
	{
		int y = 0;
		int ey = dy * ly;
		int ex = dx * lx;
		for (;y<ey;y+=dy, v+=d)
		{
			u_int8_t *w = (u_int8_t*)sp->data + (y>>16) * sp->LX ;			
			int x = 0;
			for (x=0; x<ex; x+=dx, v+=sizeof(Tsize)) 
				ALPHA(v, (Tsize)p->palette[w[x>>16]]);
		}
	}
  
    return;
}

static GXSPRITEINTERFACE g_gi={
    0, 
    put, 
    pset, 
    pset, 
    Trsp50, 
    TrspADD, 
    Trsp50, 
    Trsp50, 
    zoom_pset, 
    zoom_put, 
    zoom_Trsp50, 
    zoom_TrspADD, 
    zoom_TrspADD, 
    zoom_Trsp50
};

void GX_GetSpriteInterfaceRef8(struct GXSYSTEM *pGX, GXSPRITEINTERFACE *p)
{
	*p = g_gi;
}
