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

#define V3X_EXPORT_DLL

#include <stdio.h>
#include <string.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "systools.h"
#include "gx_struc.h"
#include "gx_rgb.h"
#include "gx_init.h"
#include "v3xdefs.h"
#include "v3xrend.h"
#include "v3x_2.h"
#include "v3xref.h"
#include "v3xref.hpp"

__extern_c
extern struct RLXSYSTEM	    *	g_pRLX;
__end_extern_c

struct RLXSYSTEM *g_pRLX;

static uint8_t *render_ptr = NULL;
static int32_t   render_scan = 0;

static V3X_GXRenderClass pReference_8bit = {
    &GFX3Dg_8bit,
    &GFX3D_8bit,
    &GFX3Ds_8bit,
    &GFX3Df_8bit,
    &GFX3Ds_8bit,
    NULL, // N/A
    NULL    // N/A
};

static V3X_GXRenderClass pReference_16bit = {
    &Prim16_NonTex,
    &Prim16_Linear256x256x8b,
    &Prim16_Linear128x128x8b,
    &Prim16_Corrected256x256x8b,
    &Prim16_Linear128x128x8b,
    NULL, // TODO : (sauf les rendus 'shaded').
    NULL    // TODO
};

static V3X_GXRenderClass pReference_32bit = {
    &Prim32_NonTex,
    &Prim32_Linear256x256x8b,
    &Prim32_Linear128x128x8b,
    &Prim32_Corrected256x256x8b,
    &Prim32_Linear128x128x8b,
    NULL, // ? (mmx)
    NULL    //
};

__extern_c
uint16_t RGB_MUL16(uint16_t a, uint16_t b);
void V3XRef_HardwareRegister(int bpp);
__end_extern_c

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3XRef_HardwareSetup(void)
*
* DESCRIPTION :
*
*/
static int V3XAPI V3XRef_HardwareSetup(void)
{
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XRef_HardwareShutdown(void)
*
* DESCRIPTION :
*
*/
static void V3XAPI V3XRef_HardwareShutdown(void)
{
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XRef_HardwareRegister(int bpp)
*
* DESCRIPTION :
*
*/
static int g_BitsPerPixel;
void V3XAPI V3XRef_HardwareRegister(int bpp)
{
    g_pRLX->pV3X->Client->Capabilities&=~GXSPEC_RGBLIGHTING;
    g_pRLX->pV3X->Setup.flags&=~V3XOPTION_TRUECOLOR;
    g_pRLX->pV3X->Client->reduce = g_pRLX->System.DietMode;
    switch(bpp) {
        case 2:
        g_pRLX->pV3X->Client->primitive = &pReference_16bit;
        break;
        case 3:
        case 4:
        g_pRLX->pV3X->Client->primitive = &pReference_32bit;
        break;
        case 1:
        default:
        g_pRLX->pV3X->Client->primitive = &pReference_8bit;
        break;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static void V3XAPI *V3XRef_TextureAlloc(const GXSPRITE *sp, const rgb24_t *colorTable, int bpp, unsigned options)
{
    uint32_t size = sp->LX * sp->LY;
	int i;
	V3XTEXTURESW * p = (V3XTEXTURESW*)g_pRLX->mm_heap->malloc(sizeof(V3XTEXTURESW));
    p->texture = (uint8_t*)g_pRLX->mm_heap->malloc(size);

    if (bpp > 8)
    {
		g_pRLX->pfSmartConverter(p->texture, NULL, 1, sp->data, NULL, (bpp+1)>>3, size);
		bpp = 8;
    }
    else
	{
		sysMemCpy(p->texture, sp->data, size);
	}

	p->bpp = bpp >> 3;

	for (i=0;i<256;i++)
		p->palette[i] = g_pRLX->pfSetPixelFormat(colorTable[i].r, colorTable[i].g, colorTable[i].b);
	return (uint8_t*)p;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XRef_TextureFree(GXSPRITE *ptr)
*
* DESCRIPTION :
*
*/
static void V3XAPI V3XRef_TextureFree(void *handle)
{
	V3XTEXTURESW * p = (V3XTEXTURESW*)handle;
    g_pRLX->mm_heap->free(p->texture);
	g_pRLX->mm_heap->free(p);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3XRef_TextureModify(GXSPRITE *ptr, uint8_t *data)
*
* DESCRIPTION :
*
*/
static int V3XAPI V3XRef_TextureModify(GXSPRITE *ptr, uint8_t *data, const rgb24_t *colorTable)
{
	V3XTEXTURESW * p = (V3XTEXTURESW*)ptr->handle;
	size_t sz = ptr->LX * ptr->LY;
	int i;

	sysMemCpy(p->texture, data, sz);
	SYS_ASSERT(p->bpp == 1);

	for (i=0;i<256;i++)
		p->palette[i] = g_pRLX->pfSetPixelFormat(colorTable[i].r, colorTable[i].g, colorTable[i].b);

	return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static unsigned V3XAPI V3XRef_ZbufferClear(uint32_t color)
*
* DESCRIPTION :
*
*/
static unsigned V3XAPI V3XRef_ZbufferClear(rgb24_t *color, V3XSCALAR z, void *bitmap)
{
    UNUSED(color);
    UNUSED(z);
    UNUSED(bitmap);
    return TRUE;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static unsigned V3XAPI V3XRef_SetState(unsigned command, uint32_t value)
{
    switch(command) {
        case V3XCMD_SETZBUFFERSTATE:
        {
            g_pRLX->pV3X->Client->Capabilities &= ~GXSPEC_ENABLEZBUFFER;
        }
        return 0;
        case V3XCMD_SETZBUFFERCOMP:
        {
        }
        return 0;
    }
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void V3x_RenderDisplay(void)
*
* DESCRIPTION :
*
*/
uint32_t g_DiffuseTable[64];
uint32_t g_Gradient[64];
uint32_t g_MixTable[64][256];
V3XMATERIAL *pipe_pMat;

static inline uint32_t RGB_MUL32(uint32_t r0, uint32_t r1)
{
    uint32_t __c;
    RGBENDIAN *a=(RGBENDIAN*)&r0, *b=(RGBENDIAN*)&r1, *c=(RGBENDIAN*)&__c;
    c->r = (uint8_t)(((uint32_t)a->r * (uint32_t)b->r)>>8);
    c->g = (uint8_t)(((uint32_t)a->g * (uint32_t)b->g)>>8);
    c->b = (uint8_t)(((uint32_t)a->b * (uint32_t)b->b)>>8);
    return __c;
}

static inline uint32_t SetPixelFormat32(uint32_t r, uint32_t g, uint32_t b)
{
    return r|(g<<8)|(b<<16);
}

static inline uint16_t SetPixelFormat16(uint32_t r, uint32_t g, uint32_t b)
{
    return (b>>3)|((g>>2)<<5)|((r>>3)<<11);
}

static void ComputeMix(const V3XMATERIAL *pMat)
{
	int i;
	V3XTEXTURESW *p = (V3XTEXTURESW*)pMat->texture[0].handle;

	if ( g_BitsPerPixel == 32)
	{
		for (i=0;i<64;i++)
		{
			int j;
			uint32_t *d = g_MixTable[i];
			for (j=0;j<256;j++)
			{
				d[j] = RGB_MUL32(p->palette[j], g_Gradient[i]);
			}
		}
	}
	else
	if ( g_BitsPerPixel == 16)
	{
		for (i=0;i<64;i++)
		{
			int j;
			uint32_t *d = g_MixTable[i];
			for (j=0;j<256;j++)
			{
				d[j] = RGB_MUL16(p->palette[j], g_Gradient[i]);
			}
		}
	}
}

static void InitRamp()
{
	int i;

	if ( g_BitsPerPixel == 32)
	{
		for (i=0;i<64;i++)
		{
			g_Gradient[i] = SetPixelFormat32(i*4,i*4,i*4);
		}
	}
	else
	if ( g_BitsPerPixel == 16)
	{
		for (i=0;i<64;i++)
		{
			g_Gradient[i] = SetPixelFormat16(i*4,i*4,i*4);
		}
	}
}

static void ComputeRamp(const V3XMATERIAL *pMat)
{
	int i;
	if ( g_BitsPerPixel == 32)
	{
		for (i=0;i<64;i++)
		{
			g_DiffuseTable[i] = SetPixelFormat32(i*4,i*4,i*4);
		}
	}
	else
	if ( g_BitsPerPixel == 16)
	{
		for (i=0;i<64;i++)
		{
			g_DiffuseTable[i] = SetPixelFormat16(i*4,i*4,i*4);
		}
	}
}

int V3XMaterial_IsEqual( const V3XMATERIAL *a, const V3XMATERIAL *b)
{

	if ((!a)||(!b))
		return 0;

	if (a == b)
		return 1;

	if (memcmp(&a->ambient, &b->ambient, 3 * 3))
		return 0;

	if (a->texture->handle!=b->texture->handle)
		return 0;

	return 1;
}

static V3XPOLY_L V3XAPI *V3XRef_Cast(V3XPOLY *fce)
{
    V3XPOLY_L *f = (V3XPOLY_L*) (g_pRLX->pV3X->Buffer.ClippedFaces + g_pRLX->pV3X->Buffer.MaxClippedFaces-1);
    V3XMATERIAL *Mat = (V3XMATERIAL*)fce->Mat;
    int i, n=1;

	if (!V3XMaterial_IsEqual(pipe_pMat, Mat))
	{
		pipe_pMat = Mat;
		if (Mat->info.Shade)
		{
			if (Mat->info.Texturized)
				ComputeMix(Mat);
			else
				ComputeRamp(Mat);
		}
	}

    if (g_pRLX->pV3X->Buffer.MaxClipped>=g_pRLX->pV3X->Buffer.MaxClippedFaces - 1) return NULL;
    f->Mat = fce->Mat;
    f->numEdges = fce->numEdges;
    f->visible = fce->visible;
    if ((Mat->info.Perspective)&&(fce->visible!=2))
    {
        sysMemCpy(f->ZTab, fce->ZTab, 12*f->numEdges);
        n = 0;
    }

    for (i=0;i<f->numEdges;i++)
    {
        f->dispTab[i].x = (int32_t)fce->dispTab[i].x;
        f->dispTab[i].y = (int32_t)fce->dispTab[i].y;
        f->dispTab[i].z = (int32_t)fce->dispTab[i].z;
        if ((Mat->info.Texturized)&&(n))
        {
            f->uvTab[0][i].u = (int32_t)(fce->uvTab[0][i].u * 255.f);
            f->uvTab[0][i].v = (int32_t)(fce->uvTab[0][i].v * 255.f);
            if (Mat->info.Environment&V3XENVMAPTYPE_DOUBLE)
            {
                f->uvTab[1][i].u = (int32_t)(fce->uvTab[1][i].u * 255.f);
                f->uvTab[1][i].v = (int32_t)(fce->uvTab[1][i].v * 255.f);
            }
        }
        if (Mat->info.Shade)
        {
			f->shade[i] = ((int32_t)fce->shade[i]) >> 2;
        }
    }
	if (Mat->info.Shade == 1)
	{
		f->shade[0] = (f->shade[0]+f->shade[1]+f->shade[2])/3;
	}

    return f;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3XAPI V3XRef_PolyRender(V3XPOLY **f, unsigned i)
*
* Description :
*
*/
static void V3XAPI V3XRef_PolyRender(V3XPOLY **f, int i)
{
    uint8_t *s = g_pRLX->pGX->View.lpBackBuffer;
    int32_t   l = g_pRLX->pGX->View.lPitch;
    if (render_ptr)
    {
        g_pRLX->pGX->View.lpBackBuffer = render_ptr;
        g_pRLX->pGX->View.lPitch  = render_scan;
    }
    for (;i>0;f++, i--)
    {
        V3XMATERIAL *Mat=((V3XMATERIAL*)(**f).Mat);
        V3XPOLY *fp = (V3XPOLY*) V3XRef_Cast(*f);
        if (fp)   Mat->render_clip(fp);
    }
    if (render_ptr)
    {
        g_pRLX->pGX->View.lpBackBuffer = s;
        g_pRLX->pGX->View.lPitch  = l;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3XAPI V3XRef_RenderDisplay(void)
*
* DESCRIPTION :
*
*/
static void V3XAPI V3XRef_RenderDisplay(void)
{
	pipe_pMat = 0;
	InitRamp();

    V3XRef_PolyRender(g_pRLX->pV3X->Buffer.RenderedFaces, g_pRLX->pV3X->Buffer.MaxFaces);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3XAPI V3XRef_Done(void)
*
* DESCRIPTION :
*
*/
static void V3XAPI V3XRef_Done(void)
{
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static void V3XAPI V3XRef_DrawPrimitives(V3XVECTOR *vertexes, uint16_t *indexTab, unsigned NumIndexes, unsigned NumVertexes, int option, rgb32_t *color)
{
    if (option==V3XRCLASS_point)
    {
        if (indexTab)
        {
            for (;NumIndexes;NumIndexes--, indexTab++)
            {
                V3XVECTOR *v0 = vertexes + indexTab[0];
                rgb32_t *c = color + indexTab[0];
				g_pRLX->pGX->gi.drawPixel(  (int32_t)v0->x, (int32_t)v0->y,
		        g_pRLX->pfSetPixelFormat(c->r, c->g, c->b));
	    }
	}
	else
	{
	    for (;NumVertexes;NumVertexes--, vertexes++, color++)
	    {
		    g_pRLX->pGX->gi.drawPixel((int32_t)vertexes[0].x, (int32_t)vertexes[0].y,
		    g_pRLX->pfSetPixelFormat(color->r, color->g, color->b));
	    }
	}
    }
    else
    {
	if (indexTab)
	{
	    for (;NumIndexes;NumIndexes-=2, indexTab+=2)
	    {
		V3XVECTOR *v0 = vertexes + indexTab[0];
		V3XVECTOR *v1 = vertexes + indexTab[1];
		rgb32_t *c = color + indexTab[0];
		g_pRLX->pGX->gi.drawAnyLine((int32_t)v0->x,
		    (int32_t)v0->y,
		    (int32_t)v1->x,
		    (int32_t)v1->y,
		    g_pRLX->pfSetPixelFormat(c->r, c->g, c->b));
	    }
	}
	else
	{
	    for (;NumVertexes;NumVertexes-=2, vertexes+=2, color+=2)
	    {
		    g_pRLX->pGX->gi.drawAnyLine((int32_t)vertexes[0].x,
		    (int32_t)vertexes[0].y,
		    (int32_t)vertexes[1].x,
		    (int32_t)vertexes[1].y,
		    g_pRLX->pfSetPixelFormat(color->r, color->g, color->b));
	    }
        }
    }
}

static void Begin(void)
{
	g_BitsPerPixel = g_pRLX->pGX->View.BitsPerPixel;
	return;
}

V3X_GXSystem V3X_GXRefDriver={
    NULL,
    V3XRef_RenderDisplay,
    V3XRef_TextureAlloc,
    V3XRef_TextureFree,
    V3XRef_TextureModify,
    V3XRef_HardwareSetup,
    V3XRef_HardwareShutdown,
    V3XRef_SetState,
    V3XRef_ZbufferClear,
    V3XRef_PolyRender,
    Begin,
    V3XRef_Done,
    V3XRef_DrawPrimitives,
    "Software",
    3*256+15,
    GXSPEC_OPACITYTRANSPARENT
    |GXSPEC_ALPHABLENDING_ADD
    |GXSPEC_ALPHABLENDING_MID
   , // Capabilities
    0x000000,
    0, // reduce
};

_V3XEXPORTFUNC void RLXAPI V3X_EntryPoint(struct RLXSYSTEM *p)
{
	GX_EntryPoint(p);
	g_pRLX = p;
    g_pRLX->pV3X->Client = &V3X_GXRefDriver;
}

