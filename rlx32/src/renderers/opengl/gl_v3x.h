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

#elif (defined __APPLE__ && defined __MACH__) // MacOS X
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <AGL/agl.h>
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
}GL_TexHandle;

__extern_c
	    void						GL_ResetViewport(void);
	    void						GL_InstallExtensions();
		extern struct RLXSYSTEM *	g_pRLX;

		extern GLbyte				gl_EXT_paletted_texture, 
									gl_SGIS_generate_mipmap, 
									gl_EXT_texture_rectangle;
__end_extern_c

#endif // __V3XGL_HH
