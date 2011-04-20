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
#include "sysctrl.h"

#define PIPE_OPEN 1

static float g_fInvZFar;
static GLint pipe_pTex[2];
static GLuint pipe_iRender, pipe_prm;
static GLint gl_lx, gl_ly;

GLbyte gl_EXT_paletted_texture,
	   gl_SGIS_generate_mipmap,
	   gl_EXT_texture_rectangle,
	   gl_ARB_texture_non_power_of_two,
	   gl_APPLE_client_storage;

static GLuint _glewStrLen (const GLubyte* s)
{
  GLuint i=0;
  while (s+i != NULL && s[i] != '\0') i++;
  return i;
}

static GLuint _glewStrCLen (const GLubyte* s, GLubyte c)
{
  GLuint i=0;
  while (s+i != NULL && s[i] != '\0' && s[i] != c) i++;
  return i;
}

static GLboolean _glewStrSame (const GLubyte* a, const GLubyte* b, GLuint n)
{
  GLuint i=0;
  while (i < n && a+i != NULL && b+i != NULL && a[i] == b[i]) i++;
  return i == n;
}

GLboolean glewGetExtension (const char* name, GLubyte *s)
{
  GLubyte* p;
  GLubyte* end;
  GLuint len = _glewStrLen((const GLubyte*)name);
  p = (GLubyte*)s;
  if (0 == p) return GL_FALSE;
  end = p + _glewStrLen(p);
  while (p < end)
  {
    GLuint n = _glewStrCLen(p, ' ');
    if (len == n && _glewStrSame((const GLubyte*)name, p, n)) return GL_TRUE;
    p += n+1;
  }
  return GL_FALSE;
}

int GL_IsSupported(const char *extension)
{
	int ret = 0;
	// Extended features
	ret = glewGetExtension(extension, (GLubyte*)glGetString(GL_EXTENSIONS));

	if (extension)
	{
#ifdef _DEBUG
		if (ret)
			SYS_Debug("...using %s", extension);
        else
            SYS_Debug("X..%s not found", extension);
#endif
    }
    return ret;
}

void GL_InstallExtensions()
{
    GLint n;
    GLint g_nTextureMaxSize = 0;

	if (GL_IsSupported("GL_ARB_multitexture"))
    {
#ifdef GL_VERSION_1_3
        glGetIntegerv( GL_MAX_TEXTURE_UNITS, &n);
		if (n>1)
			g_pRLX->pV3X->Client->Capabilities|= GXSPEC_MULTITEXTURING;
#endif
    }

	if (GL_IsSupported("GL_ARB_texture_rectangle") ||
		GL_IsSupported("GL_EXT_texture_rectangle") ||
		GL_IsSupported("GL_NV_texture_rectangle"))
	{
		gl_EXT_texture_rectangle = 1;
		g_pRLX->pV3X->Client->Capabilities|= GXSPEC_NONPOWOF2;
	}
	else
	{
		gl_EXT_texture_rectangle = 0;
	}

	// Disable extensions
	gl_EXT_paletted_texture = 0; // GL_IsSupported("GL_EXT_paletted_texture");
	gl_ARB_texture_non_power_of_two = 0; // GL_IsSupported("GL_ARB_texture_non_power_of_two") || (gl_major_version>=2);

	// Enable extensions
	gl_APPLE_client_storage = GL_IsSupported("GL_APPLE_client_storage");
	gl_SGIS_generate_mipmap = GL_IsSupported("GL_SGIS_generate_mipmap"); // or OpenGL 1.4

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &n);
    while (n!=0)  {g_nTextureMaxSize++;n>>=1;}
	g_pRLX->pV3X->Client->texMaxSize = g_nTextureMaxSize;

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glShadeModel(GL_FLAT);
	glDisable(GL_LIGHTING);
}

typedef unsigned long (*PFMOUSECALLBACK)(void *);
static PFMOUSECALLBACK pMouseCallback;

static unsigned long Update(void *device)
{
	MSE_ClientDriver *m = g_pRLX->Control.mouse;
	unsigned long ret;
	ret = pMouseCallback(device);
	m->x = m->x * g_pRLX->pGX->View.lWidth / gl_lx;
	m->y = m->y * g_pRLX->pGX->View.lHeight / gl_ly;
	return ret;
}

void GL_FakeViewPort()
{
	MSE_ClientDriver *m = g_pRLX->Control.mouse;

	gl_lx = g_pRLX->pGX->View.lWidth;
	gl_ly = g_pRLX->pGX->View.lHeight;

	g_pRLX->pGX->View.lWidth = 640;
	g_pRLX->pGX->View.lHeight = 480;

	// Slightly scale for wide screen
	if ((float)g_pRLX->pGX->View.lWidth/ (float)g_pRLX->pGX->View.lHeight >= 1.6f)
	{
		g_pRLX->pGX->View.lWidth = 768;
	}

	g_pRLX->pGX->View.xmax = g_pRLX->pGX->View.lWidth-1;
	g_pRLX->pGX->View.ymax = g_pRLX->pGX->View.lHeight-1;

	if (m->Update!=Update)
	{
		pMouseCallback= m->Update;
		m->Update = Update;
	}
}


static int V3XAPI HardwareSetup(void)
{
    return 0;
}

static void V3XAPI HardwareShutdown(void)
{
}

static unsigned V3XAPI ZbufferClear(rgb24_t *color, V3XSCALAR z, void *bitmap)
{
	return 0;
    UNUSED(color);
    UNUSED(z);
    UNUSED(bitmap);
    if (bitmap)
		glDrawPixels(g_pRLX->pGX->View.lWidth,
					 g_pRLX->pGX->View.lHeight, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, bitmap);
    else
		glClear(GL_DEPTH_BUFFER_BIT);
    return 1;
}

static unsigned V3XAPI SetState(unsigned command, uint32_t value)
{
    switch(command) {
        case V3XCMD_SETBACKGROUNDCOLOR:
        {
            rgb24_t *cl = (rgb24_t*) value;
            g_pRLX->pV3X->ViewPort.backgroundColor = RGB_Make32bit(cl->r, cl->g, cl->b, 255);
        }
        return 1;
        case V3XCMD_SETZBUFFERSTATE:
        {
            if (value) g_pRLX->pV3X->Client->Capabilities|=GXSPEC_ENABLEZBUFFER;
            else g_pRLX->pV3X->Client->Capabilities&=~GXSPEC_ENABLEZBUFFER;
        }
        return 1;
        case V3XCMD_SETZBUFFERCOMP:
        {
        }
        return 1;
    }
    return 0;
}

static int V3XAPI TextureModify(GXSPRITE *sp, uint8_t *bitmap, const rgb24_t *colorTable)
{
	GL_TexHandle *pSprite = (GL_TexHandle *)sp->handle;

	uint8_t *src_buf = (uint8_t *)pSprite->tmpbuf;

    GLenum texture_fmt =
#ifdef __BIG_ENDIAN__
		GL_RGBA
#else
		GL_BGRA_EXT
#endif
        ;
    GLenum texture_type = GL_UNSIGNED_BYTE;

	if (!src_buf)
	{
		return -1;
	}

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

	glBindTexture(GL_TEXTURE_2D, pSprite->handle);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sp->LX, sp->LY, texture_fmt, texture_type, src_buf);
	glBindTexture(GL_TEXTURE_2D, 0);
    return 1;
}

static void V3XAPI FreeTexture(void *sp)
{
	GL_TexHandle *hnd = (GL_TexHandle *)sp;
    GLuint handle = hnd->handle;
    glDeleteTextures (1, (const GLuint *)&handle );
	if (hnd->tmpbuf)
	{
		g_pRLX->mm_std->free(hnd->tmpbuf);
		hnd->tmpbuf = 0;
	}
	g_pRLX->mm_std->free(hnd);
}

static void V3XAPI *UploadTexture(const GXSPRITE *sp, const rgb24_t *colorTable, int bpp, unsigned options)
{
    int internal_fmt = 0;
    int num_colors = 0;
    int texture_fmt = 0;
    int32_t bs = (bpp+1)>>3;
    int32_t bp = 0;
    GLenum texture_type = GL_UNSIGNED_BYTE;
	GL_TexHandle *hnd;
    uint8_t *src_buf;
    GLuint handle;
	hnd = (GL_TexHandle *)g_pRLX->mm_std->malloc(sizeof(GL_TexHandle));
	hnd->palette = NULL;
    src_buf = sp->data;
	SYS_ASSERT(src_buf);

    if ((!gl_EXT_paletted_texture) && (bpp<=8))
    {
        bpp = options & V3XTEXDWNOPTION_COLORKEY ? 32 : 24;
        bp = (bpp+1)>>3;

#ifdef USE_ARGB_TEXTURE

        if (bp == 4 && (!(g_pRLX->pV3X->Client->Capabilities & GXSPEC_ENABLECOMPRESSION)))
        {
			g_pRLX->pGX->View.ColorMask.RedFieldPosition   = 8;
			g_pRLX->pGX->View.ColorMask.GreenFieldPosition = 16;
			g_pRLX->pGX->View.ColorMask.BlueFieldPosition  = 24;
			g_pRLX->pGX->View.ColorMask.RsvdFieldPosition  = 0;
        }
#endif

        src_buf = g_pRLX->pfSmartConverter(
			NULL, (rgb24_t*)colorTable, bp,
			sp->data, (rgb24_t*)colorTable, bs,
			sp->LX*sp->LY
		);

		internal_fmt = bp;
#ifdef __BIG_ENDIAN__
		texture_fmt = bp==4 ? GL_RGBA : GL_RGB;
#else
		texture_fmt = bp==4 ? GL_BGRA_EXT : GL_BGR_EXT;
#endif

#ifdef USE_ARGB_TEXTURE
        if (bp == 4 && (!(g_pRLX->pV3X->Client->Capabilities & GXSPEC_ENABLECOMPRESSION)))
        {
			g_pRLX->pGX->View.ColorMask.RedFieldPosition   = 0;
			g_pRLX->pGX->View.ColorMask.GreenFieldPosition = 8;
			g_pRLX->pGX->View.ColorMask.BlueFieldPosition  = 16;
			g_pRLX->pGX->View.ColorMask.RsvdFieldPosition  = 24;
            texture_type = GL_UNSIGNED_INT_8_8_8_8_REV;
            texture_fmt = GL_BGRA_EXT;
        }
#endif
    }
    else
	{
		switch (bpp )
		{
			case 8:
				internal_fmt = GL_COLOR_INDEX8_EXT;
				texture_fmt = GL_COLOR_INDEX;
				num_colors = 256;
			break;

			case 24:
				internal_fmt = 3;
	#ifdef __BIG_ENDIAN__
				texture_fmt = GL_RGB;
	#else
				texture_fmt = GL_BGR_EXT;
	#endif

			break;
			case 32:
				internal_fmt = 4;
	#ifdef __BIG_ENDIAN__
				texture_fmt = GL_RGBA;
	#else
				texture_fmt = GL_BGRA_EXT;
	#endif
			break;
		}
	}

    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle );

#ifdef GL_GENERATE_MIPMAP
	if (gl_SGIS_generate_mipmap)
	{
		glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	}
#endif

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	   gl_SGIS_generate_mipmap && g_pRLX->pV3X->Client->Capabilities&GXSPEC_ENABLEFILTERING ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
	  GL_LINEAR);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	if ((options & V3XTEXDWNOPTION_DYNAMIC)==0)
	{
		if (g_pRLX->pV3X->Client->Capabilities & GXSPEC_ENABLECOMPRESSION)
		{
			if (texture_fmt == GL_RGB)
				internal_fmt = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			else
			if (texture_fmt == GL_RGBA)
				internal_fmt = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		}
		hnd->tmpbuf = 0;
	}
	else
	{
		hnd->tmpbuf = (uint8_t*)g_pRLX->mm_std->malloc(sp->LX*sp->LY*4);
		if (gl_APPLE_client_storage)
		{
			glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, TRUE);
		}
		memcpy(hnd->tmpbuf, src_buf, sp->LX*sp->LY*4);
	}

#ifdef GL_VERSION_1_2
    if (num_colors)
    {
		hnd->palette = (rgb24_t*)g_pRLX->mm_std->malloc(1024);
		if (hnd->palette)
		{
			sysMemCpy(hnd->palette, colorTable, 768);
			glColorTable(GL_TEXTURE_2D, GL_RGB, 256, GL_RGB,GL_UNSIGNED_BYTE, hnd->palette);
		}
	}
#endif

	glTexImage2D( GL_TEXTURE_2D, 0, internal_fmt, sp->LX, sp->LY, 0, texture_fmt, texture_type,	(options & V3XTEXDWNOPTION_DYNAMIC) && hnd->tmpbuf ? hnd->tmpbuf : src_buf);
	hnd->handle = handle;

	if (src_buf!=sp->data)
	{
		g_pRLX->mm_heap->free(src_buf);
	}

	if ((gl_APPLE_client_storage) && (options & V3XTEXDWNOPTION_DYNAMIC))
	{
		glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, FALSE);
	}

    return (void*)hnd;
}

static void ChangeRender(const V3XMATERIAL *pMat)
{
	pipe_iRender = pMat->lod;
    if (pipe_prm&PIPE_OPEN)
	{
		glEnd();
		pipe_prm&=~PIPE_OPEN;
	}

    switch(pMat->info.Transparency)
	{
        case V3XBLENDMODE_SUB:
			glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
			glEnable(GL_BLEND);
        break;
        case V3XBLENDMODE_ADD:
			glBlendFunc(GL_ONE, GL_ONE);
			glEnable(GL_BLEND);
        break;
        case V3XBLENDMODE_ALPHA:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
        break;
        default:
			glDisable(GL_BLEND);
        break;
    }

	if (g_pRLX->pV3X->Client->Capabilities&(GXSPEC_ENABLEZBUFFER|GXSPEC_ENABLEWBUFFER))
	{
		if (pMat->info.Transparency)
			glDepthMask(GL_FALSE);
		else
			glDepthMask(GL_TRUE);
	}

	glShadeModel(pMat->info.Shade>1 ? GL_SMOOTH : GL_FLAT );

}

static void SetTexture(GL_TexHandle *hnd, int tmu)
{
	GLuint handle = hnd->handle;
    if (handle)
	{
	    if (pipe_prm&PIPE_OPEN)
		{
			glEnd();
			pipe_prm&=~PIPE_OPEN;
		}

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, handle );

#ifdef GL_VERSION_1_2
		if (hnd->palette)
			glColorTable(GL_TEXTURE_2D, GL_RGB, 256, GL_RGB, GL_UNSIGNED_BYTE, hnd->palette);
#endif
		pipe_pTex[tmu] = handle;
	}
}

#define xMUL8(a,b) (((unsigned)(a)*(unsigned)(b))>>8)

static const V3XMATERIAL *pipe_pMat;

#define SET_VTX() glVertex3f( fce->dispTab[i].x, fce->dispTab[i].y, 1.f - (fce->dispTab[i].z * g_fInvZFar));

#define SET_DIFFUSE(i) glColor4ub(min(255, (pipe_pMat->ambient.r>>2) + xMUL8(fce->rgb[i].r, pipe_pMat->diffuse.r)), min(255, (pipe_pMat->ambient.g>>2) + xMUL8(fce->rgb[i].g, pipe_pMat->diffuse.g)), min(255, (pipe_pMat->ambient.b>>2) + xMUL8(fce->rgb[i].b, pipe_pMat->diffuse.b)), fce->rgb[i].a);

#define SET_CONSTANT() glColor4ub(pipe_pMat->diffuse.r, pipe_pMat->diffuse.b, pipe_pMat->diffuse.g, pipe_pMat->alpha);

#define SET_UV(tmu) glTexCoord2f(fce->uvTab[tmu][i].u, fce->uvTab[tmu][i].v);

#define SET_UVC(tmu)  glTexCoord4f(fce->ZTab[i].uow, fce->ZTab[i].vow, fce->ZTab[i].oow, fce->ZTab[i].oow);


static void render(const V3XPOLY *fce, int i)
{
	SET_VTX();
}

static void render_s(const V3XPOLY *fce, int i)
{
	SET_DIFFUSE(i);
	SET_VTX();
}

static void render_1t(const V3XPOLY *fce, int i)
{
	SET_UV(0);
	SET_VTX();
}

static void render_1tc(const V3XPOLY *fce, int i)
{
	SET_UVC(0);
	SET_VTX();
}

static void render_1ts(const V3XPOLY *fce, int i)
{
	SET_DIFFUSE(i);
	SET_UV(0);
	SET_VTX();
}

static void render_1tcs(const V3XPOLY *fce, int i)
{
	SET_DIFFUSE(i);
	SET_UVC(0);
	SET_VTX();
}

typedef void (*PFRENDERFUNC)(const V3XPOLY *fce, int i);
static PFRENDERFUNC func;

static void ChangeMaterial(const V3XMATERIAL *pMat)
{
	if (pMat->info.Texturized)
	{
		GL_TexHandle *hnd = (GL_TexHandle*)pMat->texture[0].handle;
		if (hnd)
		{
			if (hnd->handle != pipe_pTex[0])
				SetTexture(hnd, 0);
		}
		else
			pipe_pTex[0] = 0;
	}
	else
	{
		if (pipe_pTex[0])
		{
			if (pipe_prm&PIPE_OPEN)
			{
				glEnd();
				pipe_prm&=~PIPE_OPEN;
			}

			glDisable( GL_TEXTURE_2D );
			pipe_pTex[0] = 0;
		}
	}

	if (pMat->info.Texturized)
    {
		if (pMat->info.Perspective)
		{
			func = pMat->info.Shade>1 ? render_1tcs : render_1tc;
		}
		else
		{
			func = pMat->info.Shade>1 ? render_1ts : render_1t;
		}
	}
	else
	{
		func = pMat->info.Shade ? render_s : render;
	}
}

static void V3XAPI RenderPoly(V3XPOLY **fe, int count)
{

	pipe_pMat = 0;
    pipe_prm = 0;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    for (;count!=0;fe++, count--)
    {
        V3XPOLY *fce = *fe;
        const V3XMATERIAL *pMat = (const V3XMATERIAL*)fce->Mat;
        if (pMat->Render==255)
        {
            if (pipe_prm&PIPE_OPEN)
			{
				glEnd();
				pipe_prm&=~PIPE_OPEN;
			}
			SYS_ASSERT(pMat->render_clip);
            pMat->render_clip(fce);
        }
        else
        {
			// Prepare shade value
			if (pipe_pMat != pMat)
			{
				pipe_pMat = pMat;
				ChangeMaterial(pMat);
				if (pMat->lod!=pipe_iRender)
					ChangeRender(pMat);
			}

            if (!(pipe_prm&PIPE_OPEN))
            {
                if ((pMat->Render == V3XRCLASS_wired))
					glBegin( GL_LINE_LOOP );
				else
					glBegin(GL_TRIANGLES);
                pipe_prm|=PIPE_OPEN;
            }

            // Begin polygons draw

			int i =0, a = 1, b = 2;
			if (pipe_pMat->info.Shade == 0)
			{
				SET_CONSTANT();
			}
			else
			{
				if (pipe_pMat->info.Shade == 1)
				{
					SET_DIFFUSE(0);
				}
			}

			if (pMat->RenderID != V3XRCLASS_shadow)
            for (i=0;i<fce->numEdges-2;i++)
			{
				func(fce, 0);
				func(fce, a);
				func(fce, b);
				a = b;
				b++;
			}
        }
    }
    if (pipe_prm&PIPE_OPEN)  glEnd();
	glPopAttrib();

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glShadeModel(GL_FLAT);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3XAPI StartList(void)
*
* DESCRIPTION :
*
*/
static void V3XAPI StartList(void)
{
	pipe_iRender = 0;
    g_fInvZFar = 1.f / g_pRLX->pV3X->Clip.Far;
    pipe_pTex[0] = 0;
    pipe_pTex[1] = 0;
    glPolygonMode(GL_FRONT_AND_BACK, g_pRLX->pV3X->Client->Capabilities&GXSPEC_FORCEWIREFRAME ?  GL_LINE : GL_FILL);
    if (g_pRLX->pV3X->Client->Capabilities&GXSPEC_ENABLEZBUFFER)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);
    }
    else
    {
        glDisable( GL_DEPTH_TEST );
    }
    if (g_pRLX->pV3X->Client->Capabilities&GXSPEC_ENABLEDITHERING)
		glEnable( GL_DITHER );
    else
		glDisable( GL_DITHER );

	g_pRLX->pGX->View.State|=GX_STATE_SCENEBEGUN;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3XAPI EndList(void)
*
* DESCRIPTION :
*
*/
static void V3XAPI EndList(void)
{
    g_pRLX->pGX->View.State&=~GX_STATE_SCENEBEGUN;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3XAPI RenderDisplay(void)
*
* DESCRIPTION :
*
*/
static void V3XAPI RenderDisplay(void)
{
    int n = g_pRLX->pV3X->Buffer.MaxFaces;
    V3XPOLY **f = g_pRLX->pV3X->Buffer.RenderedFaces;
    if (!n)
		return;
    RenderPoly(f, n);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3XAPI DrawPrimitives(V3XVECTOR *vertexes, uint16_t *indexTab, unsigned NumIndexes, unsigned NumVertexes, int option, rgb32_t *color)
*
* DESCRIPTION :
*
*/
static void V3XAPI DrawPrimitives(V3XVECTOR *vertexes, uint16_t *indexTab, unsigned NumIndexes, unsigned NumVertexes, int option, rgb32_t *color)
{
	return;
    if (option&256)
    {
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_BLEND);
    }
	glDisable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
    if (g_pRLX->pV3X->Client->Capabilities&GXSPEC_ENABLEZBUFFER)
	{
		glEnable(GL_DEPTH_TEST);
	}
    option&=255;
    glBegin( option == V3XRCLASS_point ? GL_POINTS : GL_LINES );
    if (indexTab)
    {
        for (;NumIndexes!=0;NumIndexes--, indexTab++)
        {
            V3XVECTOR *v = vertexes + (*indexTab);
            V3XSCALAR ooz = 1.f - (v->z * g_fInvZFar);
            if (color)
				glColor4ubv((const GLubyte *)(color + (*indexTab)));
            glVertex3f( v->x, v->y, ooz);
        }
    }
    else
    {
        for (;NumVertexes!=0;NumVertexes--, vertexes++)
        {
            V3XSCALAR ooz = 1.f - (vertexes->z * g_fInvZFar);
            if (color)
            {
				glColor4ubv((const GLubyte *)color);
                color++;
            }

            glVertex3f( vertexes->x, vertexes->y, ooz);
        }
    }
    glEnd();
    glDisable(GL_BLEND);
    glShadeModel(GL_FLAT);
}

V3X_GXSystem V3X_OpenGL=
{
    NULL,
    RenderDisplay,
    UploadTexture,
    FreeTexture,
    TextureModify,
    HardwareSetup,
    HardwareShutdown,
    SetState,
    ZbufferClear,
    RenderPoly,
    StartList,
    EndList,
    DrawPrimitives,
    "OpenGL",
    256+10,
    GXSPEC_ZBUFFER|
    GXSPEC_OPACITYTRANSPARENT|
    GXSPEC_SPRITEAREPOLY|
    GXSPEC_UVNORMALIZED|
	GXSPEC_FULLHWSPRITE|
    GXSPEC_RESIZABLEMAP|
    GXSPEC_ALPHABLENDING_ADD|
    GXSPEC_HARDWAREBLENDING|
    GXSPEC_ANTIALIASEDLINE|
    GXSPEC_ENABLEFILTERING|
    GXSPEC_HARDWARE|
    GXSPEC_ENABLEPERSPECTIVE|
    0,
    0x000000,
    1 // Texture en (256>>1)=128x128 par defaut
};

void GL_ResetViewport(void)
{
	glViewport(0, 0, gl_lx, gl_ly);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, g_pRLX->pGX->View.lWidth, g_pRLX->pGX->View.lHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

