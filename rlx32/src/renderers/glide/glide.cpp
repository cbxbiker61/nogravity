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
#ifdef _WIN32
#include <windows.h>
#endif

#include <glide.h>
#include "_rlx32.h"
#include "_rlx.h"

#include "systools.h"
#include "sysresmx.h"
#include "gx_struc.h"
#include "gx_init.h"
#include "gx_csp.h"
#include "gx_rgb.h"
#include "v3xdefs.h"
#include "v3x_2.h"
#include "v3xrend.h"
#include "glidemem.h"

#define RGB_565to888(colour) (((colour )&31)<< 3) |(((colour>>5 )&63)<<10)|(((colour>>11)&31)<<19)
#define USE_LFB

#ifdef _WIN32
static HWND g_hWnd;
#endif

typedef struct 
{
    u_int16_t lWidth, lHeight, BitsPerPixel;
    GrScreenResolution_t mode;
}GlideDisplayMode;

GlideDisplayMode static g_lpDisplayModes[12]=
{
    { 8, 8, 1    , 1},
    { 640, 480, 16, GR_RESOLUTION_640x480}, 
    { 512, 384, 16, GR_RESOLUTION_512x384}, 
    { 640, 400, 16, GR_RESOLUTION_640x400},
    { 800, 600, 16, GR_RESOLUTION_800x600}, 
    { 0, 0, 0, 0}
};
static GrMemManager		*	g_Mem;
static GrScreenResolution_t g_ScreenResolution;
static GrMipMapId_t			pipe_pTex[2];
static int					pipe_renderID;

static GrVertex			*	g_lpVertexBuffer;
static int					g_nWriteMode;
static GrLfbInfo_t			g_LfbInfo;
static float				g_fClipFar;
static struct RLXSYSTEM *	g_pRLX;

#define SNAP_COORD(v) ( ( v ) = ( float )( ( int32_t )( ( v ) * 16 ) ) / 16.0f )

static unsigned ZbufferClear(rgb24_t *color, V3XSCALAR z, void *bitmap)
{
    grBufferClear(
      g_pRLX->pV3X->Client->Capabilities&GXSPEC_ENABLEFOGGING
    ? g_pRLX->pV3X->ViewPort.backgroundColor
    : RGB_Make32bit(color->r, color->g, color->b, 0), 0, GR_ZDEPTHVALUE_FARTHEST);
    return 0;
}

static unsigned SetState(unsigned command, u_int32_t value)
{
    switch(command) 
	{
		case V3XCMD_SETZBUFFERSTATE:
			if (value)
				g_pRLX->pV3X->Client->Capabilities|=GXSPEC_ENABLEZBUFFER;
			else       
				g_pRLX->pV3X->Client->Capabilities&=~GXSPEC_ENABLEZBUFFER;
		return 1;
		case V3XCMD_SETZBUFFERCOMP:
			grDepthBufferFunction( GR_CMP_GEQUAL );
		return 1;
    }
    return 0;
}
static int HardwareSetup(void)
{
    g_lpVertexBuffer = (GrVertex*)g_pRLX->mm_heap->malloc(sizeof(GrVertex) * g_pRLX->pV3X->Buffer.MaxEdges);
    g_Mem = new GrMemManager;
    return 0;
}

static u_int8_t *Lock(void)
{
    return 0;
}

static void Unlock(void)
{
    return;
}

static void HardwareShutdown(void)
{
    grSstWinClose();
    grGlideShutdown();
    g_pRLX->mm_heap->free(g_lpVertexBuffer);
    return;
}

static int TextureModify(GXSPRITE *id, u_int8_t *newBuffer, const rgb24_t *colorTable)
{
    UNUSED(id);
    UNUSED(newBuffer);
    return 0;
}

static void FreeTexture(void *handle)
{
    g_Mem->free((GrMemOffset)handle);
    return;
}

static u_int8_t *ChooseBestTextureFormat(const GXSPRITE *sp, unsigned bpp, unsigned *format)
{
    u_int8_t *src_buf = NULL;
    *format = GR_TEXFMT_16BIT;
    switch(bpp) 
	{
		case 12:
		case 32: // Convert en 4444
			src_buf = (u_int8_t*)g_pRLX->mm_std->malloc(sp->LX*sp->LY*2);
			g_pRLX->pfSmartConverter(src_buf, g_pRLX->pGX->ColorTable, 2, sp->data, g_pRLX->pGX->ColorTable, bpp==12 ? 2 : 4, sp->LX*sp->LY);
		break;
		case 4:
		case 8:  // Pas de mode palettized, on laisse en 16 bit FIXME
			*format = GR_TEXFMT_RGB_565;
			src_buf = (u_int8_t*)g_pRLX->mm_std->malloc(sp->LX*sp->LY*2);
			g_pRLX->pfSmartConverter(src_buf, g_pRLX->pGX->ColorTable, 2, sp->data, g_pRLX->pGX->ColorTable, 1, sp->LX*sp->LY);
		break;
		case 15:
			*format = GR_TEXFMT_RGB_565;
			src_buf = sp->data;
		break;
		case 16:
			*format = GR_TEXFMT_RGB_565;
			src_buf = sp->data;
		break;
		case 24:  // map 24bit
			*format = GR_TEXFMT_RGB_565;
			src_buf = (u_int8_t*)g_pRLX->mm_std->malloc(sp->LX*sp->LY*2);
			g_pRLX->pfSmartConverter(src_buf, g_pRLX->pGX->ColorTable, 2, sp->data, g_pRLX->pGX->ColorTable, 3, sp->LX*sp->LY);
		break;
    }
    return src_buf;
}
static void V3XAPI *UploadTexture(const GXSPRITE *sp, const rgb24_t *colorTable, int bpp, unsigned options)
{
    GrMipMapId_t pHandle = GR_NULL_MIPMAP_HANDLE;
    GrLOD_t lod=0;
    u_int8_t *bytes;

    int x = g_pRLX->mm_heap->active;
    int filter = g_pRLX->pV3X->Client->Capabilities&GXSPEC_ENABLEFILTERING ? GR_TEXTUREFILTER_BILINEAR : GR_TEXTUREFILTER_POINT_SAMPLED;
    unsigned internal_fmt;
    GrMemOffset handle = g_Mem->alloc(sp->LX  * sp->LY  * 2);
    if (!handle)
       return 0;

    g_pRLX->mm_heap->active = 0;
    bytes = ChooseBestTextureFormat(sp, bpp, &internal_fmt);

    while((sp->LX>>lod)!=1)
		lod++;
    lod = 8 - lod;

	pHandle = guTexAllocateMemory(
		GR_TMU0,                   // Tmu
		GR_MIPMAPLEVELMASK_BOTH,   // parity
		sp->LX,                    // size X
		sp->LY,                    // size Y
		internal_fmt,
		GR_MIPMAP_DISABLE,         // No Mipsrc_buf
		lod,                       // Largest
		lod,                       //
		GR_ASPECT_1x1, 
		GR_TEXTURECLAMP_WRAP, 
		GR_TEXTURECLAMP_WRAP, 
		filter, 
		filter,
		0.0F, 
		FXFALSE 
	);
    if ( pHandle != GR_NULL_MIPMAP_HANDLE )  
		guTexDownloadMipMap( pHandle, bytes, NULL);

    if (bytes!=sp->data)  
		g_pRLX->mm_std->free(bytes);
    
	g_pRLX->mm_heap->active = x;

	return (u_int8_t *)pHandle;
}

static void ChangeRender(V3XMATERIAL *pMat)
{
	pipe_renderID = pMat->RenderID;
	if (pMat->info.Texturized)
	{
		switch(pMat->info.Shade) 
		{
			case 1:
			case 2:
			case 3: guColorCombineFunction( GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB); break;
			case 0: guColorCombineFunction( GR_COLORCOMBINE_TEXTURE_ADD_ITRGB); break;
		}
		grTexCombineFunction(GR_TMU0, GR_TEXTURECOMBINE_DECAL);
		if ((pMat->info.Opacity)||(pMat->info.Sprite))
		{
			grChromakeyMode( GR_CHROMAKEY_ENABLE );
			grChromakeyValue( 0x0 );
		}
		else
			grChromakeyMode( GR_CHROMAKEY_DISABLE );
	}
	else
	{
		if (pMat->info.Shade)
			guColorCombineFunction( GR_COLORCOMBINE_ITRGB );
		else
		{
			guColorCombineFunction( GR_COLORCOMBINE_CCRGB );
			grConstantColorValue(RGB_Make32bit(pMat->diffuse.r, pMat->diffuse.g, pMat->diffuse.r, pMat->alpha));
		}
		grTexCombineFunction(GR_TMU0, GR_TEXTURECOMBINE_ZERO);
	}
	// Transparency
	switch(pMat->info.Transparency) 
	{
		case V3XBLENDMODE_SUB: 
			guAlphaSource( GR_ALPHASOURCE_ITERATED_ALPHA );
			grAlphaBlendFunction(
			GR_BLEND_SRC_COLOR, 
			GR_BLEND_ONE_MINUS_SRC_COLOR,
			GR_BLEND_ONE, 
			GR_BLEND_ZERO);
			grAlphaTestFunction( GR_CMP_ALWAYS );
		break;
		case V3XBLENDMODE_ADD:
			guAlphaSource( GR_ALPHASOURCE_ITERATED_ALPHA );
			grAlphaBlendFunction(
			GR_BLEND_ONE, 
			GR_BLEND_ONE, 
			GR_BLEND_ONE, 
			GR_BLEND_ZERO);
			grAlphaTestFunction( GR_CMP_ALWAYS );
		break;
		case V3XBLENDMODE_ALPHA:
			guAlphaSource( GR_ALPHASOURCE_ITERATED_ALPHA );
			grAlphaBlendFunction(
			GR_BLEND_SRC_ALPHA,
			GR_BLEND_ONE_MINUS_SRC_ALPHA, 
			GR_BLEND_ONE,
			GR_BLEND_ZERO);
			grAlphaTestFunction( GR_CMP_ALWAYS );
		break;
		case V3XBLENDMODE_NONE:
			grAlphaBlendFunction(
			GR_BLEND_ONE,
			GR_BLEND_ZERO, 
			GR_BLEND_ONE, 
			GR_BLEND_ZERO);
		break;
	}
}
static void RenderPoly(V3XPOLY **fe, int count)
{
    for (;count!=0;fe++, count--)
    {
		V3XPOLY *fce = *fe;
		V3XMATERIAL *pMat = (V3XMATERIAL*)fce->Mat;
		if (pMat->info.Shade)  
			V3XRGB_Composing(g_pRLX->pV3X->Buffer.rgb, fce);

		if (pMat->Render==255) 
			pMat->render_clip(fce);	
		else
		{
			if (!pMat->RenderID) 
				return;
			if (pMat->RenderID!=pipe_renderID)
				ChangeRender(pMat);
		}

		if ((pMat->info.Texturized)&&(pipe_pTex[0]!=(GrMipMapId_t)pMat->texture[0].handle))
		{
			pipe_pTex[0] = (GrMipMapId_t)pMat->texture[0].handle;
			if (pipe_pTex[0])
				guTexSource( pipe_pTex[0] );
			else
				grTexCombineFunction(GR_TMU0, GR_TEXTURECOMBINE_ZERO);
		}
		{
			GrVertex *v = g_lpVertexBuffer;
			int i=0;
			for (;i<fce->numEdges;i++, v++)
			{
				v->x =  fce->dispTab[i].x;
				v->y =  fce->dispTab[i].y;
				v->z = -fce->dispTab[i].z;
				v->oow = (pMat->info.Perspective) ? fce->ZTab[i].oow : 1.f/v->z;
				v->ooz = g_fClipFar * v->z;
				if (pMat->info.Shade)
				{
					v->r = g_pRLX->pV3X->Buffer.rgb[i].r;
					v->g = g_pRLX->pV3X->Buffer.rgb[i].g;
					v->b = g_pRLX->pV3X->Buffer.rgb[i].b;
				}
				else
				{
					v->r =  v->g = 	v->b = 0;
				}
				v->a = pMat->alpha;
				if (pMat->info.Texturized)
				{
					if (pMat->info.Perspective )
					{
						v->tmuvtx[0].oow = v->oow;
						v->tmuvtx[0].sow = fce->ZTab[i].uow * 255.f;
						v->tmuvtx[0].tow = fce->ZTab[i].vow * 255.f;
					}
					else
					{
						v->tmuvtx[0].oow = v->oow = 1.f;
						v->tmuvtx[0].sow = fce->uvTab[0][i].u * 255.f;
						v->tmuvtx[0].tow = fce->uvTab[0][i].v * 255.f;
					}
					if (pMat->info.Environment>=8)
					{
						v->tmuvtx[1].oow = v->oow = 1.f;
						v->tmuvtx[1].sow = fce->uvTab[1][i].u * 255.f;
						v->tmuvtx[1].tow = fce->uvTab[1][i].v * 255.f;
					}
				}
				SNAP_COORD(v->x);
				SNAP_COORD(v->y);
			}

			if (g_pRLX->pV3X->Client->Capabilities&GXSPEC_FORCEWIREFRAME)
			{
				int i, j=fce->numEdges-1;
				for (i=0;i<fce->numEdges;i++)
				{
					grDrawLine( g_lpVertexBuffer+i, g_lpVertexBuffer+j );
					j = i;
				}
			}
			else
#ifdef FXCLIPPING
			guDrawPolygonVertexListWithClip(fce->numEdges, g_lpVertexBuffer);
#else
			grDrawPolygonVertexList(fce->numEdges, g_lpVertexBuffer);
#endif
		}
		if (pMat->info.Environment>=8)
		{
			int i;
			GrVertex *v = g_lpVertexBuffer;
			pipe_pTex[0] = (GrMipMapId_t)pMat->texture[1].handle;
			pipe_renderID = 0;
			guTexSource( pipe_pTex[0] );
			guAlphaSource( GR_ALPHASOURCE_ITERATED_ALPHA );
			grAlphaBlendFunction(
			GR_BLEND_SRC_ALPHA, 
			GR_BLEND_ONE_MINUS_SRC_ALPHA, 
			GR_BLEND_ONE, 
			GR_BLEND_ZERO);
			grAlphaTestFunction( GR_CMP_ALWAYS );
			for (i=0;i<fce->numEdges;i++, v++)
			{
				v->tmuvtx[0] = v->tmuvtx[1];
			}
			grDrawPolygonVertexList(fce->numEdges, g_lpVertexBuffer);
		}
	}
    return;
}

static void StartList(void)
{
    pipe_renderID = 0;
    pipe_pTex[0] = GR_NULL_MIPMAP_HANDLE;
#ifdef FXCLIPPING
    grClipWindow( g_pRLX->pGX->View.xmin, g_pRLX->pGX->View.ymin, g_pRLX->pGX->View.xmax+1, g_pRLX->pGX->View.ymax+1 );
#endif
    g_pRLX->pGX->View.State|=GX_STATE_SCENEBEGUN;
    g_fClipFar = -65535.f / g_pRLX->pV3X->Clip.Far;
    s = g_pRLX->pV3X->Client->Capabilities&GXSPEC_ENABLEZBUFFER;
    grDepthMask( s ? FXTRUE : FXFALSE);
    grDepthBufferMode( s ? GR_DEPTHBUFFER_ZBUFFER : GR_DEPTHBUFFER_DISABLE);
    grDepthBufferFunction( GR_CMP_GEQUAL );
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : static void EndList(void)
*
* DESCRIPTION : End a polygon list
*
*/
static void EndList(void)
{
    g_pRLX->pGX->View.State&=~GX_STATE_SCENEBEGUN;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : static void RenderDisplay(void)
*
* DESCRIPTION : Render a list of polygons
*
*/
static void RenderDisplay(void)
{
    int n = g_pRLX->pV3X->Buffer.MaxFaces;
    V3XPOLY **f=g_pRLX->pV3X->Buffer.RenderedFaces;
    if (!n) 
		return;
    RenderPoly(f, n);
    return;
}

static void DrawPrimitives(V3XVECTOR *vertexes, u_int16_t *indexTab, unsigned NumIndexes, unsigned NumVertexes, int option, rgb32_t *color)
{
    UNUSED(vertexes);
    UNUSED(indexTab);
    UNUSED(NumIndexes);
    UNUSED(NumVertexes);
    UNUSED(option);
    UNUSED(color);
    return;
}

static GXDISPLAYMODEINFO  *EnumDisplayList(int bpp)
{
    GXDISPLAYMODEINFO *pList;
    int i=0, n=1;
    // Cherche le mode correspondant
    do
    {
	if (bpp == g_lpDisplayModes[i].BitsPerPixel) n++;
        i++;
    }while(g_lpDisplayModes[i].lWidth!=0);
    if (!n) return NULL;
    pList = MM_CALLOC(n+1, GXDISPLAYMODEINFO);
    i=0;
    n=0;
    do
    {
		if ((g_lpDisplayModes[i].BitsPerPixel==bpp)
		&&(g_lpDisplayModes[i].lWidth))
        {
			pList[n].mode = (short)g_lpDisplayModes[i].mode;
			pList[n].lWidth = g_lpDisplayModes[i].lWidth;
			pList[n].lHeight = g_lpDisplayModes[i].lHeight;
			n++;
        }
        i++;
    }while(g_lpDisplayModes[i].lWidth!=0);
    pList[n].mode=0;
    return pList;
}

static void CALLING_C Wait(void)
{
    grSstIdle();
    return;
}

static void CALLING_C clearBackBuffer(void)
{
    grBufferClear( g_pRLX->pV3X->ViewPort.backgroundColor, 0, GR_WDEPTHVALUE_FARTHEST );
    return;
}

static void CALLING_C clearVideo(void)
{
    int i;
    int l = g_pRLX->pGX->View.State&GX_STATE_LOCKED;
    for (i=0;i<2;i++)
    {
		grBufferClear( 0, 0, GR_WDEPTHVALUE_FARTHEST );
		grBufferSwap( 1 );
    }
    return;
}

static void CALLING_C PageFlip(void)
{
    grBufferSwap(1);
    return;
}

static void CALLING_C blit(u_int32_t dest, u_int32_t src)
{
    UNUSED(dest);
    UNUSED(src);
    return;
}

static void CALLING_C setPalette(u_int32_t a, u_int32_t b, void * pal)
{
    UNUSED(a);
    UNUSED(b);
    UNUSED(pal);
    return;
}

static void CALLING_C drawAnyLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
{
    GrVertex *v=g_lpVertexBuffer;
    int i;
    int s = g_pRLX->pGX->View.State&GX_STATE_LOCKED;
    sysMemZero(v, sizeof(GrVertex)*2);
    for (i=0;i<2;i++) {v[i].oow = v[i].z = 1.; v[i].ooz=65535.f;}
    v[0].x = (float)x1;
    v[0].y = (float)y1;
    v[1].x = (float)x2;
    v[1].y = (float)y2;
    if (pipe_renderID!=1024)
    {
	pipe_renderID=1024;
	grColorCombine( GR_COMBINE_FUNCTION_LOCAL, 
	GR_COMBINE_FACTOR_NONE, 
	GR_COMBINE_LOCAL_CONSTANT, 
	GR_COMBINE_OTHER_NONE, 
	FXFALSE );
	grAlphaCombine( GR_COMBINE_FUNCTION_LOCAL,
	GR_COMBINE_FACTOR_NONE, 
	GR_COMBINE_LOCAL_ITERATED, 
        GR_COMBINE_OTHER_NONE, 
        FXFALSE );
	grAlphaBlendFunction(
        GR_BLEND_SRC_ALPHA, 
        GR_BLEND_ONE_MINUS_SRC_ALPHA, 
        GR_BLEND_ONE, 
        GR_BLEND_ZERO);
    }
    if (colour>>16)
    {
        int i;
        guColorCombineFunction( GR_COLORCOMBINE_ITRGB );
        for (i=0;i<2;i++)
        {
            v[i].r = (float)(((colour    )&31)<< 3);
            v[i].g = (float)(((colour>>6 )&63)<< 2);
            v[i].b = (float)(((colour>>11)&31)<< 3);
	    v[i].a =  (float)((colour>>8)&0xff);
            colour>>=16;
        }
    }
    else
    {
        v[0].a = 255;
        v[1].a = 255;
        guColorCombineFunction( GR_COLORCOMBINE_CCRGB );
	grConstantColorValue( RGB_565to888(colour) );
    }
    if (g_pRLX->pV3X->Client->Capabilities&GXSPEC_ANTIALIASEDLINE)
    grAADrawLine(v+0, v+1);
    else
    grDrawLine(v+0, v+1);
    if (s) Lock();
    return;
}

static void CALLING_C drawHorizontalLine(int32_t x1, int32_t y1, int32_t lx, u_int32_t colour)
{
    drawAnyLine(x1, y1, x1+lx, y1, colour);
    return;
}

static void CALLING_C drawVerticalLine(int32_t x1, int32_t y1, int32_t lx, u_int32_t colour)
{
    drawAnyLine(x1, y1, x1, y1+lx, colour);
    return;
}

static void CALLING_C drawWiredRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
{
    drawAnyLine(x1, y1, x1, y2, colour);
    drawAnyLine(x1, y2, x2, y2, colour);
    drawAnyLine(x2, y2, x2, y1, colour);
    drawAnyLine(x2, y1, x1, y1, colour);
    return;
}

static void CALLING_C drawFilledRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
{
    int i;
    int s = g_pRLX->pGX->View.State&GX_STATE_LOCKED;
    GrVertex *v=g_lpVertexBuffer;
    sysMemZero(v, sizeof(GrVertex)*4);
    v[0].x = (float)x1;   v[0].y = (float)y1;
    v[1].x = (float)x1;   v[1].y = (float)(y2+1);
    v[2].x = (float)(x2+1); v[2].y = (float)(y2+1);
    v[3].x = (float)(x2+1); v[3].y = (float)y1;
    for (i=0;i<4;i++) {v[i].oow = v[i].z = 1.; v[i].ooz=65535.f;}
    grChromakeyMode( GR_CHROMAKEY_DISABLE );
    guColorCombineFunction( GR_COLORCOMBINE_CCRGB );
    grTexCombineFunction(GR_TMU0, GR_TEXTURECOMBINE_ZERO);
    guAlphaSource( GR_ALPHASOURCE_ITERATED_ALPHA );
    grAlphaBlendFunction(
    GR_BLEND_ONE, 
    GR_BLEND_ZERO,
    GR_BLEND_ZERO, 
    GR_BLEND_ZERO);
    grConstantColorValue( RGB_565to888(colour));
    grDrawPlanarPolygonVertexList(4, g_lpVertexBuffer);
    if (s) Lock();
    return;
}

static void CALLING_C drawShadedRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, void *palette)
{
    GrVertex *v=g_lpVertexBuffer;
    int i;
    int s = g_pRLX->pGX->View.State&GX_STATE_LOCKED;
    UNUSED(palette);

    sysMemZero(v, sizeof(GrVertex)*4);
    for (i=0;i<4;i++) {v[i].oow = v[i].z = 1.; v[i].ooz=65535.f;}
    v[0].x = (float)x1; v[0].y = (float)y1;
    v[1].x = (float)x1; v[1].y = (float)(y2+1);
    v[2].x = (float)(x2+1); v[2].y = (float)(y2+1);
    v[3].x = (float)(x2+1); v[3].y = (float)y1;
    v[0].a = v[1].a = v[2].a = v[3].a = (float)g_pRLX->pGX->csp_cfg.alpha;
    grChromakeyMode( GR_CHROMAKEY_DISABLE );
    guColorCombineFunction( GR_COLORCOMBINE_CCRGB );
    grTexCombineFunction(GR_TMU0, GR_TEXTURECOMBINE_ZERO);
    guAlphaSource( GR_ALPHASOURCE_ITERATED_ALPHA );
    grAlphaBlendFunction(
    GR_BLEND_SRC_ALPHA,
    GR_BLEND_ONE_MINUS_SRC_ALPHA, 
    GR_BLEND_ONE, 
    GR_BLEND_ZERO);
    grAlphaTestFunction( GR_CMP_ALWAYS );
    grConstantColorValue(  RGB_565to888(g_pRLX->pGX->csp_cfg.color) );
    grDrawPolygonVertexList(4, g_lpVertexBuffer);
    if (s) 
		Lock();
    return;
}

static void GetDisplayInfo(int mode)
{
    int id=-1, i=0;
    do
    {
	if (mode == g_lpDisplayModes[i].mode) id=i;
        i++;
    }while((g_lpDisplayModes[i].lWidth!=0)&&(id<0));
    // Remplit Structures g_pRLX->pGX->View
	SYS_ASSERT(id!=-1);
    GX_SetupViewport(g_lpDisplayModes[id].lWidth, g_lpDisplayModes[id].lHeight, g_lpDisplayModes[id].BitsPerPixel);
    g_pRLX->pGX->View.ColorMask.RedMaskSize =5;  g_pRLX->pGX->View.ColorMask.RedFieldPosition =11;
    g_pRLX->pGX->View.ColorMask.GreenMaskSize =6;  g_pRLX->pGX->View.ColorMask.GreenFieldPosition=5;
    g_pRLX->pGX->View.ColorMask.BlueMaskSize =5;  g_pRLX->pGX->View.ColorMask.BlueFieldPosition =0;
    g_pRLX->pGX->View.RGB_Magic =
    (((1L<<g_pRLX->pGX->View.ColorMask.RedMaskSize  )-2L) << g_pRLX->pGX->View.ColorMask.RedFieldPosition   )
    | (((1L<<g_pRLX->pGX->View.ColorMask.GreenMaskSize)-2L) << g_pRLX->pGX->View.ColorMask.GreenFieldPosition )
    | (((1L<<g_pRLX->pGX->View.ColorMask.BlueMaskSize )-2L) << g_pRLX->pGX->View.ColorMask.BlueFieldPosition  );
        g_pRLX->pGX->gi.waitDrawing = Wait;

	g_pRLX->pGX->gi.clearBackBuffer = clearBackBuffer;
    g_pRLX->pGX->gi.blit = blit;
    if (g_pRLX->pGX->View.Flags&GX_CAPS_3DSYSTEM)
    {
        g_pRLX->pGX->gi.drawAnyLine = drawAnyLine;
        g_pRLX->pGX->gi.drawHorizontalLine = drawHorizontalLine;
        g_pRLX->pGX->gi.drawVerticalLine = drawVerticalLine;
        g_pRLX->pGX->gi.drawWiredRect = drawWiredRect;
        g_pRLX->pGX->gi.drawFilledRect = drawFilledRect;
		g_pRLX->pGX->gi.drawShadedRect = drawShadedRect;
    }
    g_pRLX->pGX->gi.clearVideo = clearVideo;
    g_pRLX->pGX->View.Flip = PageFlip;
    g_pRLX->pGX->gi.setPalette = setPalette;
    g_pRLX->pV3X->ViewPort.backgroundColor = 0;


    g_pRLX->pGX->View.lPitch = 1024*2;
    return;
}

static int SetDisplayMode(int mode)
{
    int md=0;
    GrScreenRefresh_t  refresh = GR_REFRESH_70Hz;
    while (g_lpDisplayModes[md].mode!=mode)
		md++;
    if ( g_ScreenResolution != g_lpDisplayModes[md].mode  )
    {
		g_ScreenResolution = g_lpDisplayModes[md].mode;
		if (!grSstWinOpen( 0, g_ScreenResolution, refresh, GR_COLORFORMAT_ARGB, GR_ORIGIN_UPPER_LEFT, 2, 1 ) )
		{
			SYS_ASSERT(0);
			return 0;
		}
    }
    switch (g_lpDisplayModes[md].BitsPerPixel) {
        case 8:
			g_nWriteMode= 0;
        break;
        case 15:
			g_nWriteMode= GR_LFBWRITEMODE_555 ;
        break;
        case 16:
			g_nWriteMode= GR_LFBWRITEMODE_565 ;
        break;
        case 24:
			g_nWriteMode= GR_LFBWRITEMODE_888 ;
        break;
        case 32:
			g_nWriteMode= GR_LFBWRITEMODE_8888 ;
        break;
    }
    return 0;
}

static void ReleaseSurfaces(void)
{
    unsigned i;
    for (i=2;i<g_pRLX->pGX->Surfaces.maxSurface+1;i++)
    {
        if (g_pRLX->pGX->Surfaces.lpSurface[i])
        {
            free(g_pRLX->pGX->Surfaces.lpSurface[i]);
            g_pRLX->pGX->Surfaces.flags[i] = 0;
        }
    }
    for (i=0;i<g_pRLX->pGX->Surfaces.maxSurface+1;i++) g_pRLX->pGX->Surfaces.lpSurface[i]=NULL;
    g_pRLX->pGX->Surfaces.maxSurface = 0;
    return;
}

static int CreateSurface(int page)
{
    int i;
    MM_video.heapalloc(g_pRLX->pGX->View.lpFrontBuffer, g_pRLX->pGX->View.lVideoSize);
    MM_video.reset();
    for (i=0;i<2;i++)
    {
        g_pRLX->pGX->Surfaces.lpSurface[i] = g_pRLX->pGX->View.lpFrontBuffer;
        g_pRLX->pGX->Surfaces.flags[i] = 1;
    }

    for (i=2;i<page+1;i++)
    {
        g_pRLX->pGX->Surfaces.lpSurface[i] = (u_int8_t*)g_pRLX->mm_std->malloc(g_pRLX->pGX->View.lSurfaceSize+1024);
		SYS_ASSERT(g_pRLX->pGX->Surfaces.lpSurface[i]);
        if (g_pRLX->pGX->Surfaces.lpSurface[i])
			sysMemZero(g_pRLX->pGX->Surfaces.lpSurface[i], g_pRLX->pGX->View.lSurfaceSize+1024);
        g_pRLX->pGX->Surfaces.flags[i] = 2;
    }
    g_pRLX->pGX->View.lpBackBuffer = NULL;
    g_pRLX->pGX->View.State &= ~GX_STATE_BACKBUFFERPAGE;
    g_pRLX->pGX->Surfaces.maxSurface = page;
    return 0;
}

static int SearchDisplayMode(int lx, int ly, int bpp)
{
    GlideDisplayMode *ls = g_lpDisplayModes;
    while(ls->mode!=0)
    {
        if ((lx == ls->lWidth)
        &&  (ly == ls->lHeight)
        &&  (bpp == ls->BitsPerPixel )) return ls->mode;
        ls++;
    }
    return 0;
}

static void RegisterMode(int mode)
{
    g_pRLX->pGX->View.DisplayMode = (u_int16_t)mode;
    g_pRLX->pGX->Client->GetDisplayInfo(mode);
    g_pRLX->pGX->Client->SetDisplayMode(mode);
    return;
}


static void UploadSprite(GXSPRITE *sp, rgb24_t *colorTable, int bpp)
{
    UNUSED(sp);
    return;
}

static void ReleaseSprite(GXSPRITE *sp)
{
    g_pRLX->mm_heap->free(sp->data); 
	sp->data = NULL;
    return;
}

static unsigned UpdateSprite(GXSPRITE *sp, const u_int8_t *bitmap, const rgb24_t *colorTable)
{
    return 0;
}

static void Shutdown(void)
{
    return;
}

static int Open(void *hWnd)
{
	char tex[64];
    GrHwConfiguration hwconfig;
    grGlideInit();
    grGlideGetVersion(tex);
    sprintf(g_pRLX->pV3X->Client->s_DrvName, "Glide(tm) %s", tex);
    if ( !grSstQueryHardware( &hwconfig ) )
        return -1;
	grSstSelect( 0 );
#ifdef _WIN32
    g_hWnd = (HWND)hWnd;
#endif
    return 1;
}

static unsigned NotifyEvent(enum GX_EVENT_MODE mode, int x, int y)
{
    UNUSED(mode);
    UNUSED(x);
    UNUSED(y);
    return mode;
}

V3X_GXSystem V3X_Glide={
    NULL, 
    RenderDisplay, 
    UploadTexture, 
    FreeTexture, 
    TextureModify, 
    HardwareSetup, 
    HardwareShutdown,     
    SetState, 
    ZbufferAlloc,
    RenderPoly,
    StartList,
    EndList, 
    DrawPrimitives, 
    "Glide",
    256+10,
    GXSPEC_ZBUFFER+
    GXSPEC_FOG+
    GXSPEC_OPACITYTRANSPARENT+
    GXSPEC_SPRITEAREPOLY+
    GXSPEC_UVNORMALIZED+
    GXSPEC_RESIZABLEMAP+
    GXSPEC_ALPHABLENDING_ADD+
    GXSPEC_HARDWAREBLENDING+
    GXSPEC_ANTIALIASEDLINE+
#ifdef FXCLIPPING
    GXSPEC_XYCLIPPING+
#endif
    GXSPEC_ENABLEFILTERING|
    GXSPEC_HARDWARE|
    GXSPEC_ENABLEPERSPECTIVE
   , // Capabilities
    0x000000, 
    1
};


GXCLIENTDRIVER GX_Glide = {
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
    "Glide"
};

void GX_EntryPoint(struct RLXSYSTEM *p)
{
	g_pRLX = p;
    g_pRLX->pGX->Client = &GX_Glide; 
    return;
}

void V3X_EntryPoint(struct RLXSYSTEM *p)
{
	GX_EntryPoint(p);
    g_pRLX->pV3X->Client = &V3X_Glide;
    return;
}
