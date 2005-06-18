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

#include "gl_v3x.h"
#include "_rlx.h"
#include "v3xdefs.h"
#include "v3xrend.h"
#include "fixops.h"

static void  CALLING_C drawAnyLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
{
    rgb24_t cl;
    g_pRLX->pfGetPixelFormat(&cl, colour); 
    glColor3ubv((const GLubyte *)&cl);
    glBegin( GL_LINES );
    glVertex2i( x1, y1 );
    glVertex2i( x2, y2 );
    glEnd();
    return;
}

static void  CALLING_C drawHorizontalLine(int32_t x1, int32_t y1, int32_t lx, u_int32_t colour)
{
    rgb24_t cl;
    g_pRLX->pfGetPixelFormat(&cl, colour);
    glColor3ubv((const GLubyte *)&cl);
    glBegin( GL_LINES );
    glVertex2i( x1, y1 );
    glVertex2i( x1+lx, y1 );
    glEnd();
    return;
}

static void  CALLING_C drawVerticalLine(int32_t x1, int32_t y1, int32_t lx, u_int32_t colour)
{
    rgb24_t cl;
    g_pRLX->pfGetPixelFormat(&cl, colour);
    glColor3ubv((const GLubyte *)&cl);
    glBegin( GL_LINES );
    glVertex2i( x1, y1 );
    glVertex2i( x1, y1+lx );
    glEnd();
    return;
}

static void  CALLING_C drawWiredRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
{
    rgb24_t cl;
    g_pRLX->pfGetPixelFormat(&cl, colour);	
    glColor3ubv((const GLubyte *)&cl);
    glBegin(GL_LINE_LOOP);
    
    glVertex2i( x1, y2 );
    glVertex2i( x2, y2 );
    glVertex2i( x2, y1 );
	glVertex2i( x1, y1 );
    glEnd();
    return;
}

static void  CALLING_C drawShadedRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, void *palette)
{
    rgb32_t cl; 
	g_pRLX->pfGetPixelFormat((rgb24_t*)&cl, g_pRLX->pGX->csp_cfg.color);
	cl.a = (u_int8_t)g_pRLX->pGX->csp_cfg.alpha;
    glEnable(GL_BLEND);    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ubv((const GLubyte *)&cl);
    glBegin( GL_QUADS );
    glVertex2i( x1, y1 );
    glVertex2i( x1, y2 );
    glVertex2i( x2+1, y2 );
    glVertex2i( x2+1, y1 );
    glEnd();
    glDisable(GL_BLEND);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void  CALLING_C drawMeshedRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
*
* DESCRIPTION :  
*
*/
static void  CALLING_C drawMeshedRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
{
    rgb32_t cl;
    g_pRLX->pfGetPixelFormat((rgb24_t*)&cl, colour); cl.a = (u_int8_t)g_pRLX->pGX->csp_cfg.alpha;
    glColor4ubv((const GLubyte *)&cl);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBegin( GL_QUADS );
    glVertex2i( x1, y1 );
    glVertex2i( x1, y2 );
    glVertex2i( x2+1, y2 );
    glVertex2i( x2+1, y1 );
    glEnd();
    glDisable(GL_BLEND);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void  CALLING_C drawFilledRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
*
* DESCRIPTION :  
*
*/
static void  CALLING_C drawFilledRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
{
    rgb24_t cl;
    g_pRLX->pfGetPixelFormat(&cl, colour);
    glColor3ubv((const GLubyte *)&cl);
    glBegin( GL_QUADS );
    glVertex2i( x1, y1 );
    glVertex2i( x1, y2 );
    glVertex2i( x2+1, y2 );
    glVertex2i( x2+1, y1 );
    glEnd();
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void  CALLING_C drawPixel(int32_t x, int32_t y, u_int32_t colour)
*
* DESCRIPTION :  
*
*/
static void  CALLING_C drawPixel(int32_t x, int32_t y, u_int32_t colour)
{
    rgb24_t cl;
    g_pRLX->pfGetPixelFormat(&cl, colour);
    glColor3ubv((const GLubyte *)&cl);
    glBegin( GL_POINTS );
    glVertex2i( x, y );
    glEnd();
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C clearBackBuffer(void)
*
* DESCRIPTION :  
*
*/
static void CALLING_C clearBackBuffer(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C clearVideo(void)
*
* DESCRIPTION :  
*
*/
static void CALLING_C clearVideo(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C waitDrawing(void)
*
* DESCRIPTION :  
*
*/
static void CALLING_C waitDrawing(void)
{
    glFinish();
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C setPalette(u_int32_t a, u_int32_t b, void * pal)
*
* DESCRIPTION :
*
*/
static void CALLING_C setPalette(u_int32_t a, u_int32_t b, void * pal)
{
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static u_int32_t CALLING_C getPixel(int32_t x, int32_t y)
*
* DESCRIPTION :  
*
*/
static u_int32_t CALLING_C getPixel(int32_t x, int32_t y)
{
    u_int32_t buffer;
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &buffer);
    return buffer;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void  CALLING_C blit(u_int32_t dest, u_int32_t src)
*
* DESCRIPTION :  
*
*/
static void CALLING_C blit(u_int32_t dest, u_int32_t src)
{
	if (src == 0)
	{
		glReadPixels(0,0,g_pRLX->pGX->View.lWidth, g_pRLX->pGX->View.lHeight, GL_RGB, GL_UNSIGNED_BYTE, 
			g_pRLX->pGX->Surfaces.lpSurface[dest]);
	}
	else
	if (dest == 0)
	{
		glDrawPixels(g_pRLX->pGX->View.lWidth, g_pRLX->pGX->View.lHeight, GL_RGB, GL_UNSIGNED_BYTE, 
			g_pRLX->pGX->Surfaces.lpSurface[src]);
	}

}

static void CALLING_C setCursor(int32_t x, int32_t y){}
static void CALLING_C copyCursor(u_int8_t *map){}

static void SetPolyRenderState(GXSPRITE *sp, unsigned mode, rgb32_t *cl)
{
    GXSPRITEGL* pSprite = (GXSPRITEGL*)sp->handle;
    g_pRLX->pfGetPixelFormat((rgb24_t*)cl, g_pRLX->pGX->csp_cfg.color);    
	
	glEnable(pSprite->target);
	glBindTexture(pSprite->target, pSprite->handle);

	switch(mode) {
        case 1: // Opacity	
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			cl->a = 255; 
        break;
        case 2:  // Opaque
			glDisable(GL_BLEND);
			cl->a = 255; 
        break;
        case 3:  // Additive
			glEnable(GL_BLEND);			
			glBlendFunc(GL_ONE, GL_ONE);
			cl->a = 255; 
        break;
        case 4:  // Alpha			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			cl->a = (u_int8_t)g_pRLX->pGX->csp_cfg.alpha;
        break;
		case 5:  // Sub
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			cl->a = 255; 
        break;
    }    
    return;
}   

static void DrawSpritePoly(int32_t x, int32_t y, GXSPRITE *sp, int mode)
{
    GXSPRITEGL *pSprite = (GXSPRITEGL *)sp->handle;
	rgb32_t cl;
    int lx = x + sp->LX;
    int ly = y + sp->LY;
	SYS_ASSERT(pSprite);
    SetPolyRenderState(sp, mode, &cl);
	
    glBegin(GL_QUADS);
		glColor4ubv((const GLubyte *)&cl);
		glTexCoord2f(0.f, 0.f);     
		glVertex2i(x, y );
		glTexCoord2f(0.f, pSprite->v); 
		glVertex2i(x, ly);
		glTexCoord2f(pSprite->u, pSprite->v); 
		glVertex2i(lx, ly);
		glTexCoord2f(pSprite->u, 0.f);     
		glVertex2i(lx, y);
    glEnd();
	
	glDisable(pSprite->target);
	if (mode>=3)
	{
		glDisable(GL_BLEND);
	}
	
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
	rgb32_t cl;
    GXSPRITEGL *pSprite = (GXSPRITEGL *)sp->handle;
	SYS_ASSERT(pSprite);
    SetPolyRenderState(sp, mode, &cl);
    glBegin(GL_QUADS);
		glColor4ubv((const GLubyte *)&cl);
		glTexCoord2f(0.f, 0.f);     
		glVertex2i(x, y );
		glTexCoord2f(0.f, pSprite->v); 
		glVertex2i(x, y+ly);
		glTexCoord2f(pSprite->u, pSprite->v); 
		glVertex2i(x+lx, y+ly);
		glTexCoord2f(pSprite->u, 0.f);     
		glVertex2i(x+lx, y);
    glEnd();
	glDisable(GL_BLEND);
	glDisable(pSprite->target);
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

static int isPowerOfTwo(int w, int *j)
{
	int n = 0;
	while((1<<n)<w)
		n++;

	*j = n;

	if ((1<<n) == w)
		return 1;

	
	return 0;
}

// Stretch an image
static u_int8_t * StretchPoint(const u_int8_t *src_buf, int src_lx, int src_ly, int dst_lx, int dst_ly, int bytesPerPixel)
{
	u_int8_t *dst_buf = 0;
	int dy = VDIV(src_ly, dst_ly);
	int dx = VDIV(src_lx, dst_lx);

	if (bytesPerPixel == 4)
	{
		dst_buf = (u_int8_t*)g_pRLX->mm_std->malloc(dst_lx * dst_ly * bytesPerPixel);
		u_int32_t *d = (u_int32_t*)dst_buf;
		int v16, i;
		for (v16=0, i=dst_ly;i!=0;v16+=dy, i--)
		{
			const u_int32_t *w = (const u_int32_t*)src_buf + (v16>>16)* src_lx;
			int u16, j;
			for (u16=0, j=dst_lx;j!=0;u16+=dx, d++, j--)
				*d = w[u16>>16];
		}
	}
	else
	if (bytesPerPixel == 3)
	{
		dst_buf = (u_int8_t*)g_pRLX->mm_std->malloc(dst_lx * dst_ly * bytesPerPixel);
		rgb24_t *d = (rgb24_t*)dst_buf;
		int v16, i;
		for (v16=0, i=dst_ly;i!=0;v16+=dy, i--)
		{
			const rgb24_t *w = (const rgb24_t*)src_buf + (v16>>16)* src_lx;
			int u16, j;
			for (u16=0, j=dst_lx;j!=0;u16+=dx, d++, j--)
				*d = w[u16>>16];
		}
	}
	return dst_buf;
}


static void RLXAPI GL_DownloadSprite(GXSPRITE *sp, rgb24_t *colorTable, int bpp)
{
	GXSPRITEGL *pSprite = (GXSPRITEGL*) g_pRLX->mm_std->malloc(sizeof(GXSPRITEGL));
	u_int8_t *src_buf = sp->data;
	int lx, ly, src_lx, src_ly;
	GLenum texture_type = GL_UNSIGNED_BYTE;
	GLenum texture_fmt =
#ifdef __BIG_ENDIAN__
	 GL_RGBA
#else
 	 GL_BGRA_EXT
#endif
	 ;

	int t1 = isPowerOfTwo(sp->LX, &lx);
	int t2 = isPowerOfTwo(sp->LY, &ly);

	if ((t1+t2 == 2) || gl_ARB_texture_non_power_of_two)
	{
		pSprite->u = 1.f;
		pSprite->v = 1.f;
		pSprite->target = GL_TEXTURE_2D;
	}
	else	
	{	
		pSprite->u = (float)sp->LX;
		pSprite->v = (float)sp->LY;
		pSprite->target = GL_TEXTURE_RECTANGLE_ARB;
	}

	if ((bpp == 3) && src_buf)
	{
		texture_fmt = GL_RGB;
	}
	else
	if ((bpp == 1) && src_buf)
	{
		src_buf = (u_int8_t*)g_pRLX->mm_std->malloc(sp->LX * sp->LY * 4);

#ifdef USE_ARGB_TEXTURE
    	g_pRLX->pGX->View.ColorMask.RedFieldPosition   = 8;
    	g_pRLX->pGX->View.ColorMask.GreenFieldPosition = 16;
    	g_pRLX->pGX->View.ColorMask.BlueFieldPosition  = 24;
    	g_pRLX->pGX->View.ColorMask.RsvdFieldPosition  = 0;
#endif
		g_pRLX->pfSmartConverter(src_buf, NULL, 4, sp->data, colorTable, 1, sp->LX*sp->LY);

#ifdef USE_ARGB_TEXTURE
    	g_pRLX->pGX->View.ColorMask.RedFieldPosition   = 0;
    	g_pRLX->pGX->View.ColorMask.GreenFieldPosition = 8;
    	g_pRLX->pGX->View.ColorMask.BlueFieldPosition  = 16;
    	g_pRLX->pGX->View.ColorMask.RsvdFieldPosition  = 24;
        texture_type = GL_UNSIGNED_INT_8_8_8_8_REV;
        texture_fmt = GL_BGRA_EXT; // Assume all renderers support this ..
#endif

		bpp = 4;
	}

	src_lx = sp->LX;
	src_ly = sp->LY;   
    if ((!(g_pRLX->pV3X->Client->Capabilities&GXSPEC_NONPOWOF2)) && (pSprite->target == GL_TEXTURE_RECTANGLE_ARB))
	{
		u_int8_t *dst_buf;
		if (sp->data!=src_buf)
			dst_buf = src_buf;
		else
			dst_buf = 0;
		src_buf = StretchPoint(src_buf, sp->LX, sp->LY, 1<<lx, 1<<ly, bpp);
		pSprite->u = 1.f;
		pSprite->v = 1.f;
		pSprite->target = GL_TEXTURE_2D;
		if (dst_buf)
			g_pRLX->mm_std->free(dst_buf);
		src_lx = 1<<lx;
		src_ly = 1<<ly;
	}

	glGenTextures(1, &pSprite->handle);
	glBindTexture(pSprite->target, pSprite->handle);
	if (gl_APPLE_client_storage && (sp->data==src_buf))
	{
		glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, TRUE);
	}
	
#ifdef GL_VERSION_1_4
	if (gl_SGIS_generate_mipmap)
	{
		glTexParameteri(pSprite->target, GL_GENERATE_MIPMAP, GL_FALSE);
	}
#endif

    glTexParameteri( pSprite->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri( pSprite->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( pSprite->target, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( pSprite->target, GL_TEXTURE_WRAP_T, GL_CLAMP );

	if (src_buf)
	{
		glTexImage2D(pSprite->target, 0, bpp, src_lx, src_ly, 0, texture_fmt, texture_type, src_buf);

		if (sp->data!=src_buf)
			g_pRLX->mm_std->free(src_buf);
	}

	sp->handle = pSprite;

	glBindTexture(pSprite->target, 0);
	if (gl_APPLE_client_storage)
	{
		glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, FALSE);
	}
    return;
}

static void RLXAPI GL_ReleaseSprite(GXSPRITE *sp)
{
	GXSPRITEGL *pSprite = (GXSPRITEGL *)sp->handle;
    if (sp->data) 
	{
		g_pRLX->mm_heap->free(sp->data); 
		sp->data = NULL;
	}

    if (pSprite)
    {        
        if (pSprite->handle)
		{
            glDeleteTextures (1, (const GLuint *)&pSprite->handle );
		}
        g_pRLX->mm_std->free(sp->handle);
        sp->handle = NULL;
    }
    return;
}

static unsigned RLXAPI GL_UpdateSprite(GXSPRITE *sp, const u_int8_t *bitmap, const rgb24_t *colorTable)
{
	GXSPRITEGL *pSprite = (GXSPRITEGL *)sp->handle;
	if (pSprite)
    {
       GLenum texture_fmt =
#ifdef __BIG_ENDIAN__
		GL_RGBA
#else
 		GL_BGRA_EXT
#endif
        ;
        GLenum texture_type = GL_UNSIGNED_BYTE;
		u_int8_t *src_buf = (u_int8_t*)g_pRLX->mm_std->malloc(sp->LX*sp->LY*4);
		
#ifdef USE_ARGB_TEXTURE
    	g_pRLX->pGX->View.ColorMask.RedFieldPosition   = 8;
    	g_pRLX->pGX->View.ColorMask.GreenFieldPosition = 16;
    	g_pRLX->pGX->View.ColorMask.BlueFieldPosition  = 24;
    	g_pRLX->pGX->View.ColorMask.RsvdFieldPosition  = 0;
#endif
		g_pRLX->pfSmartConverter(src_buf, NULL, 4, (void*)bitmap, (rgb24_t*)colorTable, 1, sp->LX*sp->LY);

#ifdef USE_ARGB_TEXTURE
    	g_pRLX->pGX->View.ColorMask.RedFieldPosition   = 0;
    	g_pRLX->pGX->View.ColorMask.GreenFieldPosition = 8;
    	g_pRLX->pGX->View.ColorMask.BlueFieldPosition  = 16;
    	g_pRLX->pGX->View.ColorMask.RsvdFieldPosition  = 24;
        texture_type = GL_UNSIGNED_INT_8_8_8_8_REV;
        texture_fmt = GL_BGRA_EXT; // Assume all renderers support this ..
#endif

      	glBindTexture(pSprite->target, pSprite->handle);
		glTexSubImage2D(pSprite->target, 0, 0, 0, sp->LX, sp->LY, texture_fmt, texture_type, src_buf);
		g_pRLX->mm_std->free(src_buf);
		glBindTexture(pSprite->target, 0);

    }
    return 0;
}

GXGRAPHICINTERFACE GI_OpenGL = 
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


GXSPRITEINTERFACE CSP_OpenGL = {
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

extern GXCLIENTDRIVER GX_OpenGL ;

void SetPrimitiveSprites()
{
	GX_OpenGL.UploadSprite = GL_DownloadSprite;
	GX_OpenGL.UpdateSprite = GL_UpdateSprite;
	GX_OpenGL.ReleaseSprite = GL_ReleaseSprite;
}
