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
#ifndef __V3XGL_HH
#define __V3XGL_HH

#include <stdio.h>
#include "_rlx32.h"

#if defined __BEOS__
#include "dwgl.h"

#elif defined _WIN32
#include <windows.h>
#include <gl/glaux.h>
#include "win32/wglext.h"
#define GL_VERSION_1_2
#define GL_VERSION_1_3
#define GL_VERSION_1_4
#define GL_GENERATE_MIPMAP GL_GENERATE_MIPMAP_SGIS
#define GL_UNPACK_CLIENT_STORAGE_APPLE 0x85B2
#define GL_MAX_TEXTURE_UNITS GL_MAX_TEXTURE_UNITS_ARB
#define GL_TEXTURE_RECTANGLE_ARB GL_TEXTURE_RECTANGLE_EXT
#define glColorTable glColorTableEXT

#elif (defined __APPLE__ && defined __MACH__) // MacOS X
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <AGL/agl.h>

#ifdef __BIG_ENDIAN__
#define USE_ARGB_TEXTURE
#endif 

#ifndef GL_ARB_texture_rectangle
#define GL_TEXTURE_RECTANGLE_ARB 0x84F5
#endif

#elif (defined __LINUX__)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#ifndef GL_ARB_texture_rectangle
#define GL_TEXTURE_RECTANGLE_ARB 0x84F5
#endif

#ifndef GL_UNPACK_CLIENT_STORAGE_APPLE
#define GL_UNPACK_CLIENT_STORAGE_APPLE 0x85B2
#endif

#endif

#include "gx_struc.h"
#include "gx_tools.h"
#include "systools.h"
#include "gx_csp.h"
#include "gx_rgb.h"

typedef struct 
{
    float	u, v;
    GLuint	handle;
	GLenum  target;
}GXSPRITEGL;

typedef struct 
{
	GLint	 handle;
	rgb24_t *palette;
	void    *tmpbuf;
}GL_TexHandle;

__extern_c
	    void						GL_ResetViewport(void);
	    void						GL_InstallExtensions();
		void						GL_FakeViewPort();
		extern struct RLXSYSTEM *	g_pRLX;

		extern GLbyte				gl_EXT_paletted_texture, 
									gl_SGIS_generate_mipmap, 
									gl_EXT_texture_rectangle,
									gl_ARB_texture_non_power_of_two,
									gl_APPLE_client_storage;
__end_extern_c

#endif // __V3XGL_HH
