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

#define Tsize unsigned int


static __inline Tsize RGB_ADD(Tsize r0, Tsize r1)
{
    Tsize __c, r;
    RGBENDIAN *a=(RGBENDIAN*)&r0, *b=(RGBENDIAN*)&r1, *c=(RGBENDIAN*)&__c;
    r = (Tsize)a->r + (Tsize)b->r; c->r=r<255 ? (uint8_t)r : (uint8_t)255;
    r = (Tsize)a->g + (Tsize)b->g; c->g=r<255 ? (uint8_t)r : (uint8_t)255;
    r = (Tsize)a->b + (Tsize)b->b; c->b=r<255 ? (uint8_t)r : (uint8_t)255;
    return __c;
}

static __inline Tsize RGB_MUL(Tsize r0, Tsize r1)
{
    Tsize __c;
    RGBENDIAN *a=(RGBENDIAN*)&r0, *b=(RGBENDIAN*)&r1, *c=(RGBENDIAN*)&__c;
    c->r = (uint8_t)(((Tsize)a->r * (Tsize)b->r)>>8);
    c->g = (uint8_t)(((Tsize)a->g * (Tsize)b->g)>>8);
    c->b = (uint8_t)(((Tsize)a->b * (Tsize)b->b)>>8);
    return __c;
}

static __inline Tsize RGB_ALPHA(Tsize r0, Tsize r1)
{
    return ((((r0)&g_pRLX->pGX->View.RGB_Magic)+
		     ((r1)&g_pRLX->pGX->View.RGB_Magic))>>1);
}

static __inline void PUT(uint8_t *v, Tsize c)
{
	*(Tsize*)v = RGB_MUL(c, g_pRLX->pGX->csp_cfg.color);
}

static __inline void PSET(uint8_t *v, Tsize c)
{
	*(Tsize*)v = RGB_MUL(c, g_pRLX->pGX->csp_cfg.color);
}

static __inline void ADD(uint8_t *v, Tsize c)
{
	*(Tsize*)v = RGB_ADD(*(Tsize*)v, RGB_MUL(c, g_pRLX->pGX->csp_cfg.color));
}

static __inline void ALPHA(uint8_t *v, Tsize c)
{
	*(Tsize*)v = RGB_ALPHA(*(Tsize*)v, RGB_MUL(c, g_pRLX->pGX->csp_cfg.color));
}

static void CALLING_C pset (int32_t x, int32_t y, GXSPRITE *sp)
{
	GXSPRITESW *p = (GXSPRITESW*)sp->handle;
    uint8_t *v;
    int32_t oy=0, ox=0, lx=sp->LX, ly=sp->LY;
	int d;
    /*=============================================================*/
    SPRITE_CLIPPING
    /*=============================================================*/
	d = g_pRLX->pGX->View.lPitch - lx*sizeof(Tsize);
    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + sizeof(Tsize) * x;
	if (p->bpp == sizeof(Tsize))
	{
		Tsize *u = (Tsize*)sp->data + oy * (int32_t)sp->LX + ox;
		int i;
		for (i=0;i<ly;i++,v+=d, u+=sp->LX)
		{
			int j;
			for (j=0;j<lx;j++,v+=sizeof(Tsize))
				PSET(v, u[j]);
		}
	}
	else
	if (p->bpp == 1)
	{
		uint8_t *u = (uint8_t*)sp->data + oy * (int32_t)sp->LX + ox;
		int i;
		for (i=0;i<ly;i++,v+=d, u+=sp->LX)
		{
			int j;
			for (j=0;j<lx;j++,v+=sizeof(Tsize))
				PSET(v, p->palette[u[j]]);
		}
	}
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
    uint8_t *v;
	int d = 0;
    int32_t oy=0, ox=0, lx=sp->LX, ly=sp->LY;
    /*=============================================================*/
    SPRITE_CLIPPING
    /*=============================================================*/
    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + sizeof(Tsize) * x;
	d = g_pRLX->pGX->View.lPitch - lx*sizeof(Tsize);
	if (p->bpp == sizeof(Tsize))
	{
		Tsize *u = (Tsize*)sp->data + oy * (int32_t)sp->LX + ox;
		int i;
		for (i=0;i<ly;i++,v+=d, u+=sp->LX)
		{
			int j;
			for (j=0;j<lx;j++,v+=sizeof(Tsize))
				if (u[j])
				{
					PUT(v, u[j]);
				}
		}
	}
	else
	if (p->bpp == 1)
	{
		uint8_t *u = (uint8_t*)sp->data + oy * (int32_t)sp->LX + ox;
		int i;
		for (i=0;i<ly;i++,v+=d, u+=sp->LX)
		{
			int j;
			for (j=0;j<lx;j++,v+=sizeof(Tsize))
				if (u[j])
				{
					PUT(v, p->palette[u[j]]);
				}
		}
	}
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
    uint8_t *v;
    int32_t oy=0, ox=0, lx=sp->LX, ly=sp->LY;
	int d = 0;
    /*=============================================================*/
    SPRITE_CLIPPING
    /*=============================================================*/
    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + sizeof(Tsize) * x;
	d = g_pRLX->pGX->View.lPitch - lx*sizeof(Tsize);
	if (p->bpp == sizeof(Tsize))
	{
		Tsize *u = (Tsize*)sp->data + oy * (int32_t)sp->LX + ox;
		int i;
		for (i=0;i<ly;i++,v+=d, u+=sp->LX)
		{
			int j;
			for (j=0;j<lx;j++,v+=sizeof(Tsize))
				ALPHA(v, u[j]);
		}
	}
	else
	if (p->bpp == 1)
	{
		uint8_t *u = (uint8_t*)sp->data + oy * (int32_t)sp->LX + ox;
		int i;
		for (i=0;i<ly;i++,v+=d, u+=sp->LX)
		{
			int j;
			for (j=0;j<lx;j++,v+=sizeof(Tsize))
				ALPHA(v, p->palette[u[j]]);
		}
	}
}

static void CALLING_C TrspADD (int32_t x, int32_t y, GXSPRITE *sp)
{
    GXSPRITESW *p = (GXSPRITESW*)sp->handle;
    uint8_t *v;
    int32_t oy=0, ox=0, lx=sp->LX, ly=sp->LY;
	int d = 0;
    /*=============================================================*/
    SPRITE_CLIPPING
    /*=============================================================*/
    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + sizeof(Tsize) * x;
	d = g_pRLX->pGX->View.lPitch - lx*sizeof(Tsize);
	if (p->bpp == sizeof(Tsize))
	{
		Tsize *u = (Tsize*)sp->data + oy * (int32_t)sp->LX + ox;
		int i;
		for (i=0;i<ly;i++,v+=d, u+=sp->LX)
		{
			int j;
			for (j=0;j<lx;j++,v+=sizeof(Tsize))
				ADD(v, u[j]);
		}
	}
	else
	if (p->bpp == 1)
	{
		uint8_t *u = (uint8_t*)sp->data + oy * (int32_t)sp->LX + ox;
		int i;
		for (i=0;i<ly;i++,v+=d, u+=sp->LX)
		{
			int j;
			for (j=0;j<lx;j++,v+=sizeof(Tsize))
				ADD(v, p->palette[u[j]]);
		}
	}
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
    uint8_t *v = g_pRLX->pGX->View.lpBackBuffer + x * sizeof(Tsize) + g_pRLX->pGX->View.lPitch * y;
	int d = g_pRLX->pGX->View.lPitch - lx*sizeof(Tsize);

	GXSPRITESW *p = (GXSPRITESW*)sp->handle;
    int32_t dy = (sp->LY<<16) / ly;
    int32_t dx = (sp->LX<<16) / lx;

	{
		int ey = dy * ly;
		int ex = dx * lx;
		if (p->bpp == sizeof(Tsize))
		{
			int Y = 0;
			for (;Y<ey;Y+=dy, v+=d)
			{
				int X=0;
				const Tsize *w = (Tsize*)sp->data + (Y>>16) * sp->LX ;
				for (; X<ex ;X+=dx, v+=sizeof(Tsize))
				{
					const Tsize *t = w + (X>>16);
					PSET(v, *t);
				}
			}
		}
		else
		if (p->bpp == 1)
		{
			int Y = 0;
			for (;Y<ey;Y+=dy, v+=d)
			{
				int X=0;
				const uint8_t *w = (uint8_t*)sp->data + (Y>>16) * sp->LX;

				for (; X<ex; X+=dx, v+=sizeof(Tsize))
				{
					const uint8_t *t = w + (X>>16);
					PSET(v, p->palette[*t]);
				}
			}
		}
	}
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
    uint8_t *v;
    int32_t j;
    int32_t oy=0, ox=0, dx, dy, d;
    //=============================================================
	SPRITE_CLIPPING_2
    //=============================================================
    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*sizeof(Tsize);
    d = g_pRLX->pGX->View.lPitch - lx * sizeof(Tsize);

	{
		int ey = oy + dy * ly;
		int ex = ox + dx * lx;
		if (p->bpp == sizeof(Tsize))
		{
			int Y = oy;
			for (;Y<ey;Y+=dy, v+=d)
			{
				int X = ox;
				const Tsize *w = (Tsize*)sp->data + (Y>>16) * sp->LX ;
				for (; X<ex ;X+=dx, v+=sizeof(Tsize))
				{
					const Tsize *t = w + (X>>16);
					if (*t)
						PUT(v, *t);
				}
			}
		}
		else
		if (p->bpp == 1)
		{
			int Y = oy;
			for (;Y<ey;Y+=dy, v+=d)
			{
				int X = ox;
				const uint8_t *w = (uint8_t*)sp->data + (Y>>16) * sp->LX;

				for (; X<ex; X+=dx, v+=sizeof(Tsize))
				{
					const uint8_t *t = w + (X>>16);
					if (*t)
						PUT(v, p->palette[*t]);
				}
			}
		}
	}
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C zoom_TrspAdd(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly, uint8_t **real)
*
* DESCRIPTION :
*
*/
static void CALLING_C zoom_TrspADD(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly)
{
    GXSPRITESW *p = (GXSPRITESW*)sp->handle;
	uint8_t *v;
    int32_t j;
    int32_t oy=0, ox=0, dx, dy, d;
      //=============================================================
	SPRITE_CLIPPING_2
    //=============================================================

    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*sizeof(Tsize);
    d = g_pRLX->pGX->View.lPitch - lx * sizeof(Tsize);

	{
		int ey = oy + dy * ly;
		int ex = ox + dx * lx;
		if (p->bpp == sizeof(Tsize))
		{
			int Y = oy;
			for (;Y<ey;Y+=dy, v+=d)
			{
				int X = ox;
				const Tsize *w = (Tsize*)sp->data + (Y>>16) * sp->LX ;
				for (; X<ex ;X+=dx, v+=sizeof(Tsize))
				{
					const Tsize *t = w + (X>>16);
					if (*t)
						ADD(v, *t);
				}
			}
		}
		else
		if (p->bpp == 1)
		{
			int Y = oy;
			for (;Y<ey;Y+=dy, v+=d)
			{
				int X = ox;
				const uint8_t *w = (uint8_t*)sp->data + (Y>>16) * sp->LX;

				for (; X<ex; X+=dx, v+=sizeof(Tsize))
				{
					const uint8_t *t = w + (X>>16);
					if (*t)
						ADD(v, p->palette[*t]);
				}
			}
		}
	}
}

static void CALLING_C zoom_Trsp50(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly)
{
    GXSPRITESW *p = (GXSPRITESW*)sp->handle;
    uint8_t *v;
    int32_t j;
    int32_t oy=0, ox=0, dx, dy, d;
      //=============================================================
	SPRITE_CLIPPING_2
    //=============================================================

    v = g_pRLX->pGX->View.lpBackBuffer + g_pRLX->pGX->View.lPitch * y + x*sizeof(Tsize);
    d = g_pRLX->pGX->View.lPitch - lx * sizeof(Tsize);


	{
		int ey = oy + dy * ly;
		int ex = ox + dx * lx;
		if (p->bpp == sizeof(Tsize))
		{
			int Y = oy;
			for (;Y<ey;Y+=dy, v+=d)
			{
				int X = ox;
				const Tsize *w = (Tsize*)sp->data + (Y>>16) * sp->LX ;
				for (; X<ex ;X+=dx, v+=sizeof(Tsize))
				{
					const Tsize *t = w + (X>>16);
					ALPHA(v, *t);
				}
			}
		}
		else
		if (p->bpp == 1)
		{
			int Y = oy;
			for (;Y<ey;Y+=dy, v+=d)
			{
				int X = ox;
				const uint8_t *w = (uint8_t*)sp->data + (Y>>16) * sp->LX;

				for (; X<ex; X+=dx, v+=sizeof(Tsize))
				{
					const uint8_t *t = w + (X>>16);
					ALPHA(v, p->palette[*t]);
				}
			}
		}
	}
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

void GX_GetSpriteInterfaceRef32(struct GXSYSTEM *pGX, GXSPRITEINTERFACE *p)
{
	*p = g_gi;
}

