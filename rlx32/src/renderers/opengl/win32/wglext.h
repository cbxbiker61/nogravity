#ifndef	__glext_h_
#define	__glext_h_

#ifdef __cplusplus
extern "C" {
#endif


/*
** realtech	VR,	Inc.
** http://www.realtech-vr.com
** Copyright © Realtech	VR 1995	- 2003 - http://www.v3x.net
**
** Source code from	realtech VR	is supplied	under the terms	of a license
** agreement and may not be	copied or disclosed	except in accordance with the
** terms of	that agreement.	 The various license agreements	may	be found at
** the realtech	VR web site.  This file	is under BSD license
**
** FREE	SOURCE CODE
** http://www.realtech-vr.com
**
** OpenGL 2.0 extensions
**
** Additional extensions (c) Copyright 1998	S3 Incorporated.
** Additional extensions (c) Copyright 2001	NVidia Corp.
** Additional extensions (c) Copyright 2000	3Dfx Interactive.
** Additional extensions (c) Copyright 2001-2002 ATI Incorporated.
** Additional extensions (c) Copyright 2003	3D Labs.
**
*/

#if	(defined(_WIN32) ||	defined(__WIN32__))
#ifndef	APIENTRY
#define	WIN32_LEAN_AND_MEAN	1
#include <windows.h>
#endif
#elif (defined (__linux__) || defined (__MACOS__))
#define	APIENTRY
#endif

#ifdef _USE_MCD
#include "mcd.h"
#else
#include <gl/gl.h>
#endif

#ifdef _EXPORT_WGLEXT
#define	WGLEXTERN
#else
#define	WGLEXTERN extern
#endif

/*****************************************************************************************************
**	GLX	EXTENSIONS
*****************************************************************************************************/
#if	defined	__linux__

#ifndef	GLX_ARB_get_proc_address
#define	GLX_ARB_get_proc_address 1

typedef	GLvoid * (*APIENTRY	PFNGLXGETPROCADDRESSARB)(const GLubyte *procName);

#endif

WGLEXTERN PFNGLXGETPROCADDRESSARB glXGetProcAddressARB;

#endif


/*****************************************************************************************************
**	WGL	EXTENSIONS
*****************************************************************************************************/

#if	(defined(_WIN32) ||	defined(__WIN32__))
/*
** WGL_ARB_pixel_format
**
**	Support:
**	 Rage 128 *	based :	Supported
**	 Radeon	  *	based :	Supported
*/
#ifndef	WGL_ARB_pixel_format
#define	WGL_ARB_pixel_format 1

#define	WGL_NUMBER_PIXEL_FORMATS_ARB		0x2000
#define	WGL_DRAW_TO_WINDOW_ARB				0x2001
#define	WGL_DRAW_TO_BITMAP_ARB				0x2002
#define	WGL_ACCELERATION_ARB				0x2003
#define	WGL_NEED_PALETTE_ARB				0x2004
#define	WGL_NEED_SYSTEM_PALETTE_ARB			0x2005
#define	WGL_SWAP_LAYER_BUFFERS_ARB			0x2006
#define	WGL_SWAP_METHOD_ARB					0x2007
#define	WGL_NUMBER_OVERLAYS_ARB				0x2008
#define	WGL_NUMBER_UNDERLAYS_ARB			0x2009
#define	WGL_TRANSPARENT_ARB					0x200A
#define	WGL_TRANSPARENT_RED_VALUE_ARB		0x2037
#define	WGL_TRANSPARENT_GREEN_VALUE_ARB		0x2038
#define	WGL_TRANSPARENT_BLUE_VALUE_ARB		0x2039
#define	WGL_TRANSPARENT_ALPHA_VALUE_ARB		0x203A
#define	WGL_TRANSPARENT_INDEX_VALUE_ARB		0x203B
#define	WGL_SHARE_DEPTH_ARB					0x200C
#define	WGL_SHARE_STENCIL_ARB				0x200D
#define	WGL_SHARE_ACCUM_ARB					0x200E
#define	WGL_SUPPORT_GDI_ARB					0x200F
#define	WGL_SUPPORT_OPENGL_ARB				0x2010
#define	WGL_DOUBLE_BUFFER_ARB				0x2011
#define	WGL_STEREO_ARB						0x2012
#define	WGL_PIXEL_TYPE_ARB					0x2013
#define	WGL_COLOR_BITS_ARB					0x2014
#define	WGL_RED_BITS_ARB					0x2015
#define	WGL_RED_SHIFT_ARB					0x2016
#define	WGL_GREEN_BITS_ARB					0x2017
#define	WGL_GREEN_SHIFT_ARB					0x2018
#define	WGL_BLUE_BITS_ARB					0x2019
#define	WGL_BLUE_SHIFT_ARB					0x201A
#define	WGL_ALPHA_BITS_ARB					0x201B
#define	WGL_ALPHA_SHIFT_ARB					0x201C
#define	WGL_ACCUM_BITS_ARB					0x201D
#define	WGL_ACCUM_RED_BITS_ARB				0x201E
#define	WGL_ACCUM_GREEN_BITS_ARB			0x201F
#define	WGL_ACCUM_BLUE_BITS_ARB				0x2020
#define	WGL_ACCUM_ALPHA_BITS_ARB			0x2021
#define	WGL_DEPTH_BITS_ARB					0x2022
#define	WGL_STENCIL_BITS_ARB				0x2023
#define	WGL_AUX_BUFFERS_ARB					0x2024
#define	WGL_NO_ACCELERATION_ARB				0x2025
#define	WGL_GENERIC_ACCELERATION_ARB		0x2026
#define	WGL_FULL_ACCELERATION_ARB			0x2027
#define	WGL_SWAP_EXCHANGE_ARB				0x2028
#define	WGL_SWAP_COPY_ARB					0x2029
#define	WGL_SWAP_UNDEFINED_ARB				0x202A
#define	WGL_TYPE_RGBA_ARB					0x202B
#define	WGL_TYPE_COLORINDEX_ARB				0x202C

typedef	BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBIVARBPROC)	(
										HDC	hDC,
										int	iPixelFormat,
										int	iLayerPlane,
										UINT nAttributes,
										const int *piAttributes,
										int	*piValues);
typedef	BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBFVARBPROC)	(
										HDC	hDC,
										int	iPixelFormat,
										int	iLayerPlane,
										UINT nAttributes,
										const int *piAttributes,
										FLOAT *pfValues);
typedef	BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (
										HDC	hDC,
										const int *piAttribIList,
										const FLOAT	*pfAttribFList,
										UINT nMaxFormats,
										int	*piFormats,
										UINT *nNumFormats);

#endif /* WGL_ARB_pixel_format */

#ifndef	WGL_NV_float_buffer
#define	WGL_FLOAT_COMPONENTS_NV						0x20B0
#define	WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_R_NV	0x20B1
#define	WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RG_NV	0x20B2
#define	WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGB_NV	0x20B3
#define	WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGBA_NV	0x20B4
#define	WGL_TEXTURE_FLOAT_R_NV						0x20B5
#define	WGL_TEXTURE_FLOAT_RG_NV						0x20B6
#define	WGL_TEXTURE_FLOAT_RGB_NV					0x20B7
#define	WGL_TEXTURE_FLOAT_RGBA_NV					0x20B8
#endif

#ifndef	WGL_NV_render_texture_rectangle
#define	WGL_BIND_TO_TEXTURE_RECTANGLE_RGB_NV		0x20A0
#define	WGL_BIND_TO_TEXTURE_RECTANGLE_RGBA_NV		0x20A1
#define	WGL_TEXTURE_RECTANGLE_NV					0x20A2
#endif

#ifndef	WGL_NV_render_depth_texture
#define	WGL_TEXTURE_DEPTH_COMPONENT_NV				0x20A6
#define	WGL_DEPTH_TEXTURE_FORMAT_NV					0x20A5
#define	WGL_BIND_TO_TEXTURE_DEPTH_NV				0x20A3
#define	WGL_BIND_TO_TEXTURE_RECTANGLE_DEPTH_NV		0x20A4
#define	WGL_DEPTH_COMPONENT_NV						0x20A7
#endif

WGLEXTERN PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
WGLEXTERN PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;
WGLEXTERN PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

#ifndef	WGL_ARB_extensions_string
#define	WGL_ARB_extensions_string 1
typedef	const char * (WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC) (HDC hdc);
#endif
WGLEXTERN PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;

/*
** WGL_ARB_make_current_read
**
**	Support:
**	 Rage 128 *	based :	Supported
**	 Radeon	  *	based :	Supported
*/
#ifndef	WGL_ARB_make_current_read
typedef	BOOL (WINAPI * PFNWGLMAKECONTEXTCURRENTARBPROC)	(
#define	WGL_ARB_make_current_read 1

										HDC	hDrawDC,
										HDC	hReadDC,
										HGLRC hGLRC);
typedef	HDC	(WINAPI	* PFNWGLGETCURRENTREADDCARBPROC) (VOID);

#endif /* WGL_ARB_make_current_read	*/

/*
** WGL_ARB_pbuffer
**
**	Support:
**	 Rage 128 *	based :	Supported
**	 Radeon	  *	based :	Supported
*/
#ifndef	WGL_ARB_pbuffer
#define	WGL_ARB_pbuffer	1

#define	WGL_DRAW_TO_PBUFFER_ARB				 0x202D
#define	WGL_MAX_PBUFFER_PIXELS_ARB			 0x202E
#define	WGL_MAX_PBUFFER_WIDTH_ARB			 0x202F
#define	WGL_MAX_PBUFFER_HEIGHT_ARB			 0x2030
#define	WGL_PBUFFER_LARGEST_ARB				 0x2033
#define	WGL_PBUFFER_WIDTH_ARB				 0x2034
#define	WGL_PBUFFER_HEIGHT_ARB				 0x2035
#define	WGL_PBUFFER_LOST_ARB				 0x2036

DECLARE_HANDLE(HPBUFFERARB);

typedef	HPBUFFERARB	(WINAPI	* PFNWGLCREATEPBUFFERARBPROC) (
										HDC	hDC,
										int	iPixelFormat,
										int	iWidth,
										int	iHeight,
										const int *piAttribList);
typedef	HDC	(WINAPI	* PFNWGLGETPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer);
typedef	int	(WINAPI	* PFNWGLRELEASEPBUFFERDCARBPROC) (
										HPBUFFERARB	hPbuffer,
										HDC	hDC);
typedef	BOOL (WINAPI * PFNWGLDESTROYPBUFFERARBPROC)	(HPBUFFERARB hPbuffer);
typedef	BOOL (WINAPI * PFNWGLQUERYPBUFFERARBPROC) (
										HPBUFFERARB	hPbuffer,
										int	iAttribute,
										int	*piValue);

WGLEXTERN PFNWGLCREATEPBUFFERARBPROC	wglCreatePbufferARB;
WGLEXTERN PFNWGLGETPBUFFERDCARBPROC		wglGetPbufferDCARB;
WGLEXTERN PFNWGLRELEASEPBUFFERDCARBPROC	wglReleasePbufferDCARB;
WGLEXTERN PFNWGLDESTROYPBUFFERARBPROC	wglDestroyPbufferARB;
WGLEXTERN PFNWGLQUERYPBUFFERARBPROC		wglQueryPbufferARB;

#endif /* WGL_ARB_pbuffer */


/*
** WGL_ARB_render_texture
**
**	Support:
**	 Rage 128 *	based :	Supported
**	 Radeon	  *	based :	Supported
*/
#ifndef	WGL_ARB_render_texture
#define	WGL_ARB_render_texture 1

#define	WGL_BIND_TO_TEXTURE_RGB_ARB			0x2070
#define	WGL_BIND_TO_TEXTURE_RGBA_ARB		0x2071
#define	WGL_TEXTURE_FORMAT_ARB				0x2072
#define	WGL_TEXTURE_TARGET_ARB				0x2073
#define	WGL_MIPMAP_TEXTURE_ARB				0x2074
#define	WGL_TEXTURE_RGB_ARB					0x2075
#define	WGL_TEXTURE_RGBA_ARB				0x2076
#define	WGL_NO_TEXTURE_ARB					0x2077
#define	WGL_TEXTURE_CUBE_MAP_ARB			0x2078
#define	WGL_TEXTURE_1D_ARB					0x2079
#define	WGL_TEXTURE_2D_ARB					0x207A
#define	WGL_MIPMAP_LEVEL_ARB				0x207B
#define	WGL_CUBE_MAP_FACE_ARB				0x207C
#define	WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB	0x207D
#define	WGL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB	0x207E
#define	WGL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB	0x207F
#define	WGL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB	0x2080
#define	WGL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB	0x2081
#define	WGL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB	0x2082
#define	WGL_FRONT_LEFT_ARB					0x2083
#define	WGL_FRONT_RIGHT_ARB					0x2084
#define	WGL_BACK_LEFT_ARB					0x2085
#define	WGL_BACK_RIGHT_ARB					0x2086
#define	WGL_AUX0_ARB						0x2087
#define	WGL_AUX1_ARB						0x2088
#define	WGL_AUX2_ARB						0x2089
#define	WGL_AUX3_ARB						0x208A
#define	WGL_AUX4_ARB						0x208B
#define	WGL_AUX5_ARB						0x208C
#define	WGL_AUX6_ARB						0x208D
#define	WGL_AUX7_ARB						0x208E
#define	WGL_AUX8_ARB						0x208F
#define	WGL_AUX9_ARB						0x2090

#ifndef	WGL_EXT_depth_float
#define	WGL_EXT_depth_float					1
#define	WGL_DEPTH_FLOAT_EXT					0x2040
#endif

typedef	BOOL (WINAPI * PFNWGLBINDTEXIMAGEARBPROC)(HPBUFFERARB hPbuffer,	int	iBuffer);
typedef	BOOL (WINAPI * PFNWGLRELEASETEXIMAGEARBPROC)(HPBUFFERARB hPbuffer, int iBuffer);
typedef	BOOL (WINAPI * PFNWGLSETPBUFFERATTRIBARBPROC)(HPBUFFERARB hPbuffer,
													   const int *piAttribList);
#endif

WGLEXTERN PFNWGLBINDTEXIMAGEARBPROC	wglBindTexImageARB;
WGLEXTERN PFNWGLRELEASETEXIMAGEARBPROC wglReleaseTexImageARB;
WGLEXTERN PFNWGLSETPBUFFERATTRIBARBPROC	wglSetPbufferAttribARB;

/*
** GL_ARB_matrix_palette
**
** Support:
**	 Rage	128	 based : Not Supported
**	 Radeon	7xxx based : Not Supported
**	 Radeon	8xxx based : Not Supported
**	 Radeon	9xxx based : Not Supported
*/
#ifndef	GL_ARB_matrix_palette
#define	GL_ARB_matrix_palette					1

#define	GL_MATRIX_PALETTE_ARB					0x8840
#define	GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB	0x8841
#define	GL_MAX_PALETTE_MATRICES_ARB				0x8842
#define	GL_CURRENT_PALETTE_MATRIX_ARB			0x8843
#define	GL_MATRIX_INDEX_ARRAY_ARB				0x8844
#define	GL_CURRENT_MATRIX_INDEX_ARB				0x8845
#define	GL_MATRIX_INDEX_ARRAY_SIZE_ARB			0x8846
#define	GL_MATRIX_INDEX_ARRAY_TYPE_ARB			0x8847
#define	GL_MATRIX_INDEX_ARRAY_STRIDE_ARB		0x8848
#define	GL_MATRIX_INDEX_ARRAY_POINTER_ARB		0x8849

typedef	GLvoid (APIENTRY * PFNGLCURRENTPALETTEMATRIXARBPROC) (GLint	index);
typedef	GLvoid (APIENTRY * PFNGLMATRIXINDEXUBVARBPROC) (GLint size,	GLubyte	*indices);
typedef	GLvoid (APIENTRY * PFNGLMATRIXINDEXUSVARBPROC) (GLint size,	GLushort *indices);
typedef	GLvoid (APIENTRY * PFNGLMATRIXINDEXUIVARBPROC) (GLint size,	GLuint *indices);
typedef	GLvoid (APIENTRY * PFNGLMATRIXINDEXPOINTERARBPROC) (GLint size,	GLenum type, GLsizei stride, GLvoid	*pointer);

#endif /* GL_ARB_matrix_palette	*/

/*
** GL_ARB_point_parameters
**
**	Support:
**	 Rage 128 *	based :
**	 Radeon	  *	based :
**	 R200	  *	based :
*/

#ifndef	GL_ARB_point_parameters
#define	GL_ARB_point_parameters	1

typedef	void (APIENTRY * PFNGLPOINTPARAMETERFARBPROC) (GLenum pname, GLfloat param);
typedef	void (APIENTRY * PFNGLPOINTPARAMETERFVARBPROC) (GLenum pname, const	GLfloat	*params);

WGLEXTERN PFNGLPOINTPARAMETERFARBPROC glPointParameterfARB;
WGLEXTERN PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB;

#define	GL_POINT_SIZE_MIN_ARB				0x8126
#define	GL_POINT_SIZE_MAX_ARB				0x8127
#define	GL_POINT_FADE_THRESHOLD_SIZE_ARB	0x8128
#define	GL_POINT_DISTANCE_ATTENUATION_ARB	0x8129


#endif

/*
** GL_ARB_vertex_program
**
**	Support:
**	 Rage 128  * based : Not Supported
**	 Radeon	   * based : Not Supported
**	 Radeon	8xxx based : Supported
**	 Radeon	9xxx based : Supported
*/
#ifndef	GL_ARB_vertex_program
#define	GL_ARB_vertex_program 1

#define	GL_VERTEX_PROGRAM_ARB								0x8620
#define	GL_VERTEX_PROGRAM_POINT_SIZE_ARB					0x8642
#define	GL_VERTEX_PROGRAM_TWO_SIDE_ARB						0x8643
#define	GL_COLOR_SUM_ARB									0x8458
#define	GL_PROGRAM_FORMAT_ASCII_ARB							0x8875
#define	GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB					0x8622
#define	GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB						0x8623
#define	GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB					0x8624
#define	GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB						0x8625
#define	GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB				0x886A
#define	GL_CURRENT_VERTEX_ATTRIB_ARB						0x8626
#define	GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB					0x8645
#define	GL_PROGRAM_LENGTH_ARB								0x8627
#define	GL_PROGRAM_FORMAT_ARB								0x8876
#define	GL_PROGRAM_BINDING_ARB								0x8677
#define	GL_PROGRAM_INSTRUCTIONS_ARB							0x88A0
#define	GL_MAX_PROGRAM_INSTRUCTIONS_ARB						0x88A1
#define	GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB					0x88A2
#define	GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB				0x88A3
#define	GL_PROGRAM_TEMPORARIES_ARB							0x88A4
#define	GL_MAX_PROGRAM_TEMPORARIES_ARB						0x88A5
#define	GL_PROGRAM_NATIVE_TEMPORARIES_ARB					0x88A6
#define	GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB				0x88A7
#define	GL_PROGRAM_PARAMETERS_ARB							0x88A8
#define	GL_MAX_PROGRAM_PARAMETERS_ARB						0x88A9
#define	GL_PROGRAM_NATIVE_PARAMETERS_ARB					0x88AA
#define	GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB				0x88AB
#define	GL_PROGRAM_ATTRIBS_ARB								0x88AC
#define	GL_MAX_PROGRAM_ATTRIBS_ARB							0x88AD
#define	GL_PROGRAM_NATIVE_ATTRIBS_ARB						0x88AE
#define	GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB					0x88AF
#define	GL_PROGRAM_ADDRESS_REGISTERS_ARB					0x88B0
#define	GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB				0x88B1
#define	GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB				0x88B2
#define	GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB			0x88B3
#define	GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB					0x88B4
#define	GL_MAX_PROGRAM_ENV_PARAMETERS_ARB					0x88B5
#define	GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB					0x88B6
#define	GL_PROGRAM_STRING_ARB								0x8628
#define	GL_PROGRAM_ERROR_POSITION_ARB						0x864B
#define	GL_CURRENT_MATRIX_ARB								0x8641
#define	GL_TRANSPOSE_CURRENT_MATRIX_ARB						0x88B7
#define	GL_CURRENT_MATRIX_STACK_DEPTH_ARB					0x8640
#define	GL_MAX_VERTEX_ATTRIBS_ARB							0x8869
#define	GL_MAX_PROGRAM_MATRICES_ARB							0x862F
#define	GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB				0x862E
#define	GL_PROGRAM_ERROR_STRING_ARB							0x8874
#define	GL_MATRIX0_ARB										0x88C0
#define	GL_MATRIX1_ARB										0x88C1
#define	GL_MATRIX2_ARB										0x88C2
#define	GL_MATRIX3_ARB										0x88C3
#define	GL_MATRIX4_ARB										0x88C4
#define	GL_MATRIX5_ARB										0x88C5
#define	GL_MATRIX6_ARB										0x88C6
#define	GL_MATRIX7_ARB										0x88C7
#define	GL_MATRIX8_ARB										0x88C8
#define	GL_MATRIX9_ARB										0x88C9
#define	GL_MATRIX10_ARB										0x88CA
#define	GL_MATRIX11_ARB										0x88CB
#define	GL_MATRIX12_ARB										0x88CC
#define	GL_MATRIX13_ARB										0x88CD
#define	GL_MATRIX14_ARB										0x88CE
#define	GL_MATRIX15_ARB										0x88CF
#define	GL_MATRIX16_ARB										0x88D0
#define	GL_MATRIX17_ARB										0x88D1
#define	GL_MATRIX18_ARB										0x88D2
#define	GL_MATRIX19_ARB										0x88D3
#define	GL_MATRIX20_ARB										0x88D4
#define	GL_MATRIX21_ARB										0x88D5
#define	GL_MATRIX22_ARB										0x88D6
#define	GL_MATRIX23_ARB										0x88D7
#define	GL_MATRIX24_ARB										0x88D8
#define	GL_MATRIX25_ARB										0x88D9
#define	GL_MATRIX26_ARB										0x88DA
#define	GL_MATRIX27_ARB										0x88DB
#define	GL_MATRIX28_ARB										0x88DC
#define	GL_MATRIX29_ARB										0x88DD
#define	GL_MATRIX30_ARB										0x88DE
#define	GL_MATRIX31_ARB										0x88DF

typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB1SARBPROC)(GLuint index,	GLshort	x);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB1FARBPROC)(GLuint index,	GLfloat	x);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB1DARBPROC)(GLuint index,	GLdouble x);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB2SARBPROC)(GLuint index,	GLshort	x, GLshort y);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB2FARBPROC)(GLuint index,	GLfloat	x, GLfloat y);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB2DARBPROC)(GLuint index,	GLdouble x,	GLdouble y);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB3SARBPROC)(GLuint index,	GLshort	x, GLshort y, GLshort z);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB3FARBPROC)(GLuint index,	GLfloat	x, GLfloat y, GLfloat z);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB3DARBPROC)(GLuint index,	GLdouble x,	GLdouble y,	GLdouble z);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4SARBPROC)(GLuint index,	GLshort	x, GLshort y, GLshort z, GLshort w);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4FARBPROC)(GLuint index,	GLfloat	x, GLfloat y, GLfloat z, GLfloat w);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4DARBPROC)(GLuint index,	GLdouble x,	GLdouble y,	GLdouble z,	GLdouble w);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NUBARBPROC)(GLuint index, GLubyte x, GLubyte y,	GLubyte	z, GLubyte w);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB1SVARBPROC)(GLuint index, const GLshort *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB1FVARBPROC)(GLuint index, const GLfloat *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB1DVARBPROC)(GLuint index, const GLdouble	*v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB2SVARBPROC)(GLuint index, const GLshort *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB2FVARBPROC)(GLuint index, const GLfloat *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB2DVARBPROC)(GLuint index, const GLdouble	*v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB3SVARBPROC)(GLuint index, const GLshort *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB3FVARBPROC)(GLuint index, const GLfloat *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB3DVARBPROC)(GLuint index, const GLdouble	*v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4BVARBPROC)(GLuint index, const GLbyte *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4SVARBPROC)(GLuint index, const GLshort *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4IVARBPROC)(GLuint index, const GLint *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4UBVARBPROC)(GLuint index, const	GLubyte	*v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4USVARBPROC)(GLuint index, const	GLushort *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4UIVARBPROC)(GLuint index, const	GLuint *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4FVARBPROC)(GLuint index, const GLfloat *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4DVARBPROC)(GLuint index, const GLdouble	*v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NBVARBPROC)(GLuint index, const	GLbyte *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NSVARBPROC)(GLuint index, const	GLshort	*v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NIVARBPROC)(GLuint index, const	GLint *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NUBVARBPROC)(GLuint	index, const GLubyte *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NUSVARBPROC)(GLuint	index, const GLushort *v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIB4NUIVARBPROC)(GLuint	index, const GLuint	*v);
typedef	GLvoid (APIENTRY *PFNGLVERTEXATTRIBPOINTERARBPROC)(GLuint index, GLint size, GLenum	type,
														   GLboolean normalized, GLsizei stride,
														   const GLvoid	*pointer);
typedef	GLvoid (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYARBPROC)(GLuint index);
typedef	GLvoid (APIENTRY *PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)(GLuint index);
typedef	GLvoid (APIENTRY *PFNGLPROGRAMSTRINGARBPROC)(GLenum	target,	GLenum format, GLsizei len,	const GLvoid *string);
typedef	GLvoid (APIENTRY *PFNGLBINDPROGRAMARBPROC)(GLenum target, GLuint program);
typedef	GLvoid (APIENTRY *PFNGLDELETEPROGRAMSARBPROC)(GLsizei n, const GLuint *programs);
typedef	GLvoid (APIENTRY *PFNGLGENPROGRAMSARBPROC)(GLsizei n, GLuint *programs);
typedef	GLvoid (APIENTRY *PFNGLPROGRAMENVPARAMETER4FARBPROC)(GLenum	target,	GLuint index, GLfloat x, GLfloat y,	GLfloat	z, GLfloat w);
typedef	GLvoid (APIENTRY *PFNGLPROGRAMENVPARAMETER4DARBPROC)(GLenum	target,	GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef	GLvoid (APIENTRY *PFNGLPROGRAMENVPARAMETER4FVARBPROC)(GLenum target, GLuint	index, const GLfloat *params);
typedef	GLvoid (APIENTRY *PFNGLPROGRAMENVPARAMETER4DVARBPROC)(GLenum target, GLuint	index, const GLdouble *params);
typedef	GLvoid (APIENTRY *PFNGLPROGRAMLOCALPARAMETER4FARBPROC)(GLenum target, GLuint index,	GLfloat	x, GLfloat y, GLfloat z, GLfloat w);
typedef	GLvoid (APIENTRY *PFNGLPROGRAMLOCALPARAMETER4DARBPROC)(GLenum target, GLuint index,	GLdouble x,	GLdouble y,	GLdouble z,	GLdouble w);
typedef	GLvoid (APIENTRY *PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)(GLenum target, GLuint index, const GLfloat *params);
typedef	GLvoid (APIENTRY *PFNGLPROGRAMLOCALPARAMETER4DVARBPROC)(GLenum target, GLuint index, const GLdouble	*params);
typedef	GLvoid (APIENTRY *PFNGLGETPROGRAMENVPARAMETERFVARBPROC)(GLenum target, GLuint index, GLfloat *params);
typedef	GLvoid (APIENTRY *PFNGLGETPROGRAMENVPARAMETERDVARBPROC)(GLenum target, GLuint index, GLdouble *params);
typedef	GLvoid (APIENTRY *PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC)(GLenum target, GLuint	index, GLfloat *params);
typedef	GLvoid (APIENTRY *PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC)(GLenum target, GLuint	index, GLdouble	*params);
typedef	GLvoid (APIENTRY *PFNGLGETPROGRAMIVARBPROC)(GLenum target, GLenum pname, GLint *params);
typedef	GLvoid (APIENTRY *PFNGLGETPROGRAMSTRINGARBPROC)(GLenum target, GLenum pname, GLvoid	*string);
typedef	GLvoid (APIENTRY *PFNGLGETVERTEXATTRIBDVARBPROC)(GLuint	index, GLenum pname, GLdouble *params);
typedef	GLvoid (APIENTRY *PFNGLGETVERTEXATTRIBFVARBPROC)(GLuint	index, GLenum pname, GLfloat *params);
typedef	GLvoid (APIENTRY *PFNGLGETVERTEXATTRIBIVARBPROC)(GLuint	index, GLenum pname, GLint *params);
typedef	GLvoid (APIENTRY *PFNGLGETVERTEXATTRIBPOINTERVARBPROC)(GLuint index, GLenum	pname, GLvoid **pointer);
typedef	GLboolean (APIENTRY	*PFNGLISPROGRAMARBPROC)(GLuint program);

//

WGLEXTERN PFNGLVERTEXATTRIB1SARBPROC glVertexAttrib1sARB;
WGLEXTERN PFNGLVERTEXATTRIB1FARBPROC glVertexAttrib1fARB;
WGLEXTERN PFNGLVERTEXATTRIB1DARBPROC glVertexAttrib1dARB;
WGLEXTERN PFNGLVERTEXATTRIB2SARBPROC glVertexAttrib2sARB;
WGLEXTERN PFNGLVERTEXATTRIB2FARBPROC glVertexAttrib2fARB;
WGLEXTERN PFNGLVERTEXATTRIB2DARBPROC glVertexAttrib2dARB;
WGLEXTERN PFNGLVERTEXATTRIB3SARBPROC glVertexAttrib3sARB;
WGLEXTERN PFNGLVERTEXATTRIB3FARBPROC glVertexAttrib3fARB;
WGLEXTERN PFNGLVERTEXATTRIB3DARBPROC glVertexAttrib3dARB;
WGLEXTERN PFNGLVERTEXATTRIB4SARBPROC glVertexAttrib4sARB;
WGLEXTERN PFNGLVERTEXATTRIB4FARBPROC glVertexAttrib4fARB;
WGLEXTERN PFNGLVERTEXATTRIB4DARBPROC glVertexAttrib4dARB;
WGLEXTERN PFNGLVERTEXATTRIB4NUBARBPROC glVertexAttrib4NubARB;
WGLEXTERN PFNGLVERTEXATTRIB1SVARBPROC glVertexAttrib1svARB;
WGLEXTERN PFNGLVERTEXATTRIB1FVARBPROC glVertexAttrib1fvARB;
WGLEXTERN PFNGLVERTEXATTRIB1DVARBPROC glVertexAttrib1dvARB;
WGLEXTERN PFNGLVERTEXATTRIB2SVARBPROC glVertexAttrib2svARB;
WGLEXTERN PFNGLVERTEXATTRIB2FVARBPROC glVertexAttrib2fvARB;
WGLEXTERN PFNGLVERTEXATTRIB2DVARBPROC glVertexAttrib2dvARB;
WGLEXTERN PFNGLVERTEXATTRIB3SVARBPROC glVertexAttrib3svARB;
WGLEXTERN PFNGLVERTEXATTRIB3FVARBPROC glVertexAttrib3fvARB;
WGLEXTERN PFNGLVERTEXATTRIB3DVARBPROC glVertexAttrib3dvARB;
WGLEXTERN PFNGLVERTEXATTRIB4BVARBPROC glVertexAttrib4bvARB;
WGLEXTERN PFNGLVERTEXATTRIB4SVARBPROC glVertexAttrib4svARB;
WGLEXTERN PFNGLVERTEXATTRIB4IVARBPROC glVertexAttrib4ivARB;
WGLEXTERN PFNGLVERTEXATTRIB4UBVARBPROC glVertexAttrib4ubvARB;
WGLEXTERN PFNGLVERTEXATTRIB4USVARBPROC glVertexAttrib4usvARB;
WGLEXTERN PFNGLVERTEXATTRIB4UIVARBPROC glVertexAttrib4uivARB;
WGLEXTERN PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB;
WGLEXTERN PFNGLVERTEXATTRIB4DVARBPROC glVertexAttrib4dvARB;
WGLEXTERN PFNGLVERTEXATTRIB4NBVARBPROC glVertexAttrib4NbvARB;
WGLEXTERN PFNGLVERTEXATTRIB4NSVARBPROC glVertexAttrib4NsvARB;
WGLEXTERN PFNGLVERTEXATTRIB4NIVARBPROC glVertexAttrib4NivARB;
WGLEXTERN PFNGLVERTEXATTRIB4NUBVARBPROC	glVertexAttrib4NubvARB;
WGLEXTERN PFNGLVERTEXATTRIB4NUSVARBPROC	glVertexAttrib4NusvARB;
WGLEXTERN PFNGLVERTEXATTRIB4NUIVARBPROC	glVertexAttrib4NuivARB;
WGLEXTERN PFNGLVERTEXATTRIBPOINTERARBPROC glVertexAttribPointerARB;
WGLEXTERN PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB;
WGLEXTERN PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB;
WGLEXTERN PFNGLPROGRAMSTRINGARBPROC	glProgramStringARB;
WGLEXTERN PFNGLBINDPROGRAMARBPROC glBindProgramARB;
WGLEXTERN PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB;
WGLEXTERN PFNGLGENPROGRAMSARBPROC glGenProgramsARB;
WGLEXTERN PFNGLPROGRAMENVPARAMETER4FARBPROC	glProgramEnvParameter4fARB;
WGLEXTERN PFNGLPROGRAMENVPARAMETER4DARBPROC	glProgramEnvParameter4dARB;
WGLEXTERN PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB;
WGLEXTERN PFNGLPROGRAMENVPARAMETER4DVARBPROC glProgramEnvParameter4dvARB;
WGLEXTERN PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;
WGLEXTERN PFNGLPROGRAMLOCALPARAMETER4DARBPROC glProgramLocalParameter4dARB;
WGLEXTERN PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB;
WGLEXTERN PFNGLPROGRAMLOCALPARAMETER4DVARBPROC glProgramLocalParameter4dvARB;
WGLEXTERN PFNGLGETPROGRAMENVPARAMETERFVARBPROC glGetProgramEnvParameterfvARB;
WGLEXTERN PFNGLGETPROGRAMENVPARAMETERDVARBPROC glGetProgramEnvParameterdvARB;
WGLEXTERN PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glGetProgramLocalParameterfvARB;
WGLEXTERN PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glGetProgramLocalParameterdvARB;
WGLEXTERN PFNGLGETPROGRAMIVARBPROC glGetProgramivARB;
WGLEXTERN PFNGLGETPROGRAMSTRINGARBPROC glGetProgramStringARB;
WGLEXTERN PFNGLGETVERTEXATTRIBDVARBPROC	glGetVertexAttribdvARB;
WGLEXTERN PFNGLGETVERTEXATTRIBFVARBPROC	glGetVertexAttribfvARB;
WGLEXTERN PFNGLGETVERTEXATTRIBIVARBPROC	glGetVertexAttribivARB;
WGLEXTERN PFNGLGETVERTEXATTRIBPOINTERVARBPROC glGetVertexAttribPointervARB;
WGLEXTERN PFNGLISPROGRAMARBPROC	glIsProgramARB;


#endif /* GL_ARB_vertex_program	*/


/*
** GL_ARB_fragment_program
**
** Support:
**	 Rage	128	 based : Not supported
**	 Radeon	7xxx based : Not supported
**	 Radeon	8xxx based : Not supported
**	 Radeon	9xxx based : Supported
*/

#ifndef	GL_ARB_fragment_program
#define	GL_ARB_fragment_program					1

/*	  Accepted by the <cap>	parameter of Disable, Enable, and IsEnabled,
	by the <pname> parameter of	GetBooleanv, GetIntegerv, GetFloatv,
	and	GetDoublev,	and	by the <target>	parameter of ProgramStringARB,
	BindProgramARB,	ProgramEnvParameter4[df][v]ARB,
	ProgramLocalParameter4[df][v]ARB, GetProgramEnvParameter[df]vARB,
	GetProgramLocalParameter[df]vARB, GetProgramivARB and
	GetProgramStringARB.
*/

#define	GL_FRAGMENT_PROGRAM_ARB							   0x8804

//	  Accepted by the <format> parameter of	ProgramStringARB:

#define	GL_PROGRAM_FORMAT_ASCII_ARB						   0x8875

//	  Accepted by the <pname> parameter	of GetProgramivARB:

#define	GL_PROGRAM_LENGTH_ARB							   0x8627
#define	GL_PROGRAM_FORMAT_ARB							   0x8876
#define	GL_PROGRAM_BINDING_ARB							   0x8677
#define	GL_PROGRAM_INSTRUCTIONS_ARB						   0x88A0
#define	GL_MAX_PROGRAM_INSTRUCTIONS_ARB					   0x88A1
#define	GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB				   0x88A2
#define	GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB			   0x88A3
#define	GL_PROGRAM_TEMPORARIES_ARB						   0x88A4
#define	GL_MAX_PROGRAM_TEMPORARIES_ARB					   0x88A5
#define	GL_PROGRAM_NATIVE_TEMPORARIES_ARB				   0x88A6
#define	GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB			   0x88A7
#define	GL_PROGRAM_PARAMETERS_ARB						   0x88A8
#define	GL_MAX_PROGRAM_PARAMETERS_ARB					   0x88A9
#define	GL_PROGRAM_NATIVE_PARAMETERS_ARB				   0x88AA
#define	GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB			   0x88AB
#define	GL_PROGRAM_ATTRIBS_ARB							   0x88AC
#define	GL_MAX_PROGRAM_ATTRIBS_ARB						   0x88AD
#define	GL_PROGRAM_NATIVE_ATTRIBS_ARB					   0x88AE
#define	GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB				   0x88AF
#define	GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB				   0x88B4
#define	GL_MAX_PROGRAM_ENV_PARAMETERS_ARB				   0x88B5
#define	GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB				   0x88B6

#define	GL_PROGRAM_ALU_INSTRUCTIONS_ARB					   0x8805
#define	GL_PROGRAM_TEX_INSTRUCTIONS_ARB					   0x8806
#define	GL_PROGRAM_TEX_INDIRECTIONS_ARB					   0x8807
#define	GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB			   0x8808
#define	GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB			   0x8809
#define	GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB			   0x880A
#define	GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB				   0x880B
#define	GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB				   0x880C
#define	GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB				   0x880D
#define	GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB		   0x880E
#define	GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB		   0x880F
#define	GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB		   0x8810


#endif /* GL_ARB_fragment_program */

/*
** GL_ARB_shader_objects
**
** Support:
**	 Rage	128	 based : Not supported
**	 Radeon	7xxx based : Not supported
**	 Radeon	8xxx based : Not supported
**	 Radeon	96xx based : Supported
*/

#ifndef	GL_ARB_shader_objects
#define	GL_ARB_shader_objects 1

typedef	int	GLhandleARB;
typedef	char GLcharARB;

#define	GL_PROGRAM_OBJECT_ARB					0x8B40
#define	GL_OBJECT_TYPE_ARB						0x8B4E
#define	GL_OBJECT_SUBTYPE_ARB					0x8B4F
#define	GL_OBJECT_DELETE_STATUS_ARB				0x8B80
#define	GL_OBJECT_COMPILE_STATUS_ARB			0x8B81
#define	GL_OBJECT_LINK_STATUS_ARB				0x8B82
#define	GL_OBJECT_VALIDATE_STATUS_ARB			0x8B83
#define	GL_OBJECT_INFO_LOG_LENGTH_ARB			0x8B84
#define	GL_OBJECT_ATTACHED_OBJECTS_ARB			0x8B85
#define	GL_OBJECT_ACTIVE_UNIFORMS_ARB			0x8B86
#define	GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB	0x8B87
#define	GL_OBJECT_SHADER_SOURCE_LENGTH_ARB		0x8B88
#define	GL_SHADER_OBJECT_ARB					0x8B48
#define	GL_FLOAT								0x1406
#define	GL_FLOAT_VEC2_ARB						0x8B50
#define	GL_FLOAT_VEC3_ARB						0x8B51
#define	GL_FLOAT_VEC4_ARB						0x8B52
#define	GL_INT									0x1404
#define	GL_INT_VEC2_ARB							0x8B53
#define	GL_INT_VEC3_ARB							0x8B54
#define	GL_INT_VEC4_ARB							0x8B55
#define	GL_BOOL_ARB								0x8B56
#define	GL_BOOL_VEC2_ARB						0x8B57
#define	GL_BOOL_VEC3_ARB						0x8B58
#define	GL_BOOL_VEC4_ARB						0x8B59
#define	GL_FLOAT_MAT2_ARB						0x8B5A
#define	GL_FLOAT_MAT3_ARB						0x8B5B
#define	GL_FLOAT_MAT4_ARB						0x8B5C

typedef	void (APIENTRY * PFNGLSHADERSOURCEARBPROC) (GLhandleARB	shaderObj, GLsizei count, const	GLcharARB **string,	const GLint	*length);

typedef	void (APIENTRY * PFNGLGETSHADERSOURCEARBPROC) (GLhandleARB obj,	GLsizei	maxLength, GLsizei *length,	GLcharARB *source);

typedef	void (APIENTRY * PFNGLCOMPILESHADERARBPROC)	(GLhandleARB shaderObj);
typedef	void (APIENTRY * PFNGLATTACHOBJECTARBPROC) (GLhandleARB	containerObject, GLhandleARB obj);
typedef	void (APIENTRY * PFNGLUSEPROGRAMOBJECTARBPROC) (GLhandleARB	programObj);
typedef	void (APIENTRY * PFNGLGETINFOLOGARBPROC) (GLhandleARB obj,GLsizei maxLength, GLsizei *length, GLcharARB	*infoLog);
typedef	void (APIENTRY * PFNGLGETATTACHEDOBJECTSARBPROC) (GLhandleARB containerObj,	GLsizei	maxCount, GLsizei *count, GLhandleARB *obj);
typedef	void (APIENTRY * PFNGLLINKPROGRAMARBPROC) (GLhandleARB programObj);
typedef	void (APIENTRY * PFNGLDELETEOBJECTARBPROC) (GLhandleARB	obj);
typedef	GLhandleARB	(APIENTRY *	PFNGLGETHANDLEARBPROC) (GLenum pname);
typedef	void (APIENTRY * PFNGLDETACHOBJECTARBPROC) (GLhandleARB	containerObj, GLhandleARB attachedObj);
typedef	GLhandleARB	(APIENTRY *	PFNGLCREATESHADEROBJECTARBPROC)	(GLenum	shaderType);
typedef	void (APIENTRY * PFNGLCOMPILESHADERARBPROC)	(GLhandleARB shaderObj);
typedef	GLhandleARB	(APIENTRY *	PFNGLCREATEPROGRAMOBJECTARBPROC) ();
typedef	void (APIENTRY * PFNGLATTACHOBJECTARBPROC) (GLhandleARB	containerObject, GLhandleARB obj);
typedef	void (APIENTRY * PFNGLLINKPROGRAMARBPROC) (GLhandleARB programObj);
typedef	void (APIENTRY * PFNGLUSEPROGRAMOBJECTARBPROC) (GLhandleARB	programObj);
typedef	void (APIENTRY * PFNGLVALIDATEPROGRAMARBPROC) (GLhandleARB programObj);
typedef	void (APIENTRY * PFNGLUNIFORM1FARBPROC)	(GLint location, GLfloat v0);
typedef	void (APIENTRY * PFNGLUNIFORM2FARBPROC)	(GLint location, GLfloat v0, GLfloat v1);
typedef	void (APIENTRY * PFNGLUNIFORM3FARBPROC)	(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef	void (APIENTRY * PFNGLUNIFORM4FARBPROC)	(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

typedef	void (APIENTRY * PFNGLUNIFORM1IARBPROC)	(GLint location, GLint v0);
typedef	void (APIENTRY * PFNGLUNIFORM2IARBPROC)	(GLint location, GLint v0, GLint v1);
typedef	void (APIENTRY * PFNGLUNIFORM3IARBPROC)	(GLint location, GLint v0, GLint v1, GLint v2);
typedef	void (APIENTRY * PFNGLUNIFORM4IARBPROC)	(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);

typedef	void (APIENTRY * PFNGLUNIFORM1FVARBPROC) (GLint	location, GLsizei count, GLfloat *value);
typedef	void (APIENTRY * PFNGLUNIFORM2FVARBPROC) (GLint	location, GLsizei count, GLfloat *value);
typedef	void (APIENTRY * PFNGLUNIFORM3FVARBPROC) (GLint	location, GLsizei count, GLfloat *value);
typedef	void (APIENTRY * PFNGLUNIFORM4FVARBPROC) (GLint	location, GLsizei count, GLfloat *value);

typedef	void (APIENTRY * PFNGLUNIFORM1IVARBPROC) (GLint	location, GLsizei count, GLint *value);
typedef	void (APIENTRY * PFNGLUNIFORM2IVARBPROC) (GLint	location, GLsizei count, GLint *value);
typedef	void (APIENTRY * PFNGLUNIFORM3IVARBPROC) (GLint	location, GLsizei count, GLint *value);
typedef	void (APIENTRY * PFNGLUNIFORM4IVARBPROC) (GLint	location, GLsizei count, GLint *value);

typedef	void (APIENTRY * PFNGLUNIFORMMATRIX2FVARBPROC) (GLint location,	GLuint count, GLboolean	transpose, const GLfloat *value);
typedef	void (APIENTRY * PFNGLUNIFORMMATRIX3FVARBPROC) (GLint location,	GLuint count, GLboolean	transpose, const GLfloat *value);
typedef	void (APIENTRY * PFNGLUNIFORMMATRIX4FVARBPROC) (GLint location,	GLuint count, GLboolean	transpose, const GLfloat *value);
typedef	void (APIENTRY * PFNGLDETACHOBJECTARBPROC) (GLhandleARB	containerObj, GLhandleARB attachedObj);
typedef	void (APIENTRY * PFNGLGETATTACHEDOBJECTSARBPROC) (GLhandleARB containerObj,	GLsizei	maxCount, GLsizei *count, GLhandleARB *obj);

typedef	void (APIENTRY * PFNGLGETOBJECTPARAMETERFVARBPROC)(GLhandleARB obj,	GLenum pname, GLfloat *params);

typedef	void (APIENTRY * PFNGLGETOBJECTPARAMETERIVARBPROC)(GLhandleARB obj,	GLenum pname, GLint	*params);

typedef	void (APIENTRY * PFNGLGETINFOLOGARBPROC) (GLhandleARB obj,GLsizei maxLength, GLsizei *length, GLcharARB	*infoLog);

typedef	void (APIENTRY * PFNGLGETATTACHEDOBJECTSARBPROC) (GLhandleARB containerObj,	GLsizei	maxCount, GLsizei *count, GLhandleARB *obj);

typedef	GLint (APIENTRY	* PFNGLGETUNIFORMLOCATIONARBPROC) (GLhandleARB programObj, const GLcharARB *name);

typedef	void (APIENTRY * PFNGLGETACTIVEUNIFORMARBPROC) (GLhandleARB	programObj,	GLuint index, GLsizei maxLength, GLsizei *length, GLint	*size, GLenum *type, GLcharARB *name);

typedef	void (APIENTRY * PFNGLGETUNIFORMFVARBPROC) (GLhandleARB	programObj,	GLint location,	GLfloat	*params);
typedef	void (APIENTRY * PFNGLGETUNIFORMIVARBPROC) (GLhandleARB	programObj,	GLint location,	GLint *params);

typedef	void (APIENTRY * PFNGLGETSHADERSOURCEARBPROC) (GLhandleARB obj,	GLsizei	maxLength, GLsizei *length,	GLcharARB *source);

WGLEXTERN PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
WGLEXTERN PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
WGLEXTERN PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
WGLEXTERN PFNGLDETACHOBJECTARBPROC glDetachObjectARB;
WGLEXTERN PFNGLATTACHOBJECTARBPROC glAttachObjectARB;

WGLEXTERN PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
WGLEXTERN PFNGLCOMPILESHADERARBPROC	glCompileShaderARB;
WGLEXTERN PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
WGLEXTERN PFNGLGETINFOLOGARBPROC glGetInfoLogARB;
WGLEXTERN PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;

WGLEXTERN PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
WGLEXTERN PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfvARB;
WGLEXTERN PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;

WGLEXTERN PFNGLUNIFORM1FARBPROC	glUniform1fARB;
WGLEXTERN PFNGLUNIFORM2FARBPROC	glUniform2fARB;
WGLEXTERN PFNGLUNIFORM3FARBPROC	glUniform3fARB;
WGLEXTERN PFNGLUNIFORM4FARBPROC	glUniform4fARB;

WGLEXTERN PFNGLUNIFORM1IARBPROC	glUniform1iARB;
WGLEXTERN PFNGLUNIFORM2IARBPROC	glUniform2iARB;
WGLEXTERN PFNGLUNIFORM3IARBPROC	glUniform3iARB;
WGLEXTERN PFNGLUNIFORM4IARBPROC	glUniform4iARB;

WGLEXTERN PFNGLUNIFORM1FVARBPROC glUniform1fvARB;
WGLEXTERN PFNGLUNIFORM2FVARBPROC glUniform2fvARB;
WGLEXTERN PFNGLUNIFORM3FVARBPROC glUniform3fvARB;
WGLEXTERN PFNGLUNIFORM4FVARBPROC glUniform4fvARB;

WGLEXTERN PFNGLUNIFORM1IVARBPROC glUniform1ivARB;
WGLEXTERN PFNGLUNIFORM2IVARBPROC glUniform2ivARB;
WGLEXTERN PFNGLUNIFORM3IVARBPROC glUniform3ivARB;
WGLEXTERN PFNGLUNIFORM4IVARBPROC glUniform4ivARB;

#endif 

/*
** GL_ARB_fragment_shader
**
** Support:
**	 Rage	128	 based : Not supported
**	 Radeon	7xxx based : Not supported
**	 Radeon	8xxx based : Not supported
**	 Radeon	96xx based : Supported
*/

#ifndef	GL_ARB_fragment_shader
#define	GL_ARB_fragment_shader 1

#define	GL_FRAGMENT_SHADER_ARB					0x8B30
#define	GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB	0x8B49

// Also	defined	in GL_FRAGMENT_PROGRAM_NV
#define	GL_MAX_TEXTURE_COORDS_ARB				0x8871
#define	GL_MAX_TEXTURE_IMAGE_UNITS_ARB			0x8872

#endif 

/*
** GL_ARB_vertex_shader
**
** Support:
**	 Rage	128	 based : Not supported
**	 Radeon	7xxx based : Not supported
**	 Radeon	8xxx based : Not supported
**	 Radeon	96xx based : Supported
*/

#ifndef	GL_ARB_vertex_shader
#define	GL_ARB_vertex_shader 1

#define	GL_VERTEX_SHADER_ARB					 0x8B31	 
#define	GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB	 0x8B4A
#define	GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB	 0x8B4C	
#define	GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB	 0x8B4D

#endif 

/*
** GL_ARB_shading_language_100
**
** Support:
**	 Rage	128	 based : Not supported
**	 Radeon	7xxx based : Not supported
**	 Radeon	8xxx based : Not supported
**	 Radeon	96xx based : Supported
*/

#ifndef	GL_ARB_shading_language_100
#define	GL_ARB_shading_language_100	1
#endif 

/*
** GL_ARB_point_sprite
**
** Support:
**	 Rage	128	 based : Not supported
**	 Radeon	7xxx based : Not supported
**	 Radeon	8xxx based : Not supported
**	 Radeon	96xx based : Supported
*/
#ifndef	GL_ARB_point_sprite
#define	GL_ARB_point_sprite			1
#define	GL_POINT_SPRITE_ARB						  0x8861
#define	GL_COORD_REPLACE_ARB					  0x8862
#endif 


/*
** GL_ARB_vertex_buffer_object
**
** Support:
**	 Rage	128	 based : Not supported
**	 Radeon	7xxx based : Not supported
**	 Radeon	8xxx based : Not supported
**	 Radeon	9xxx based : Supported
*/

#ifndef	GL_ARB_vertex_buffer_object
#define	GL_ARB_vertex_buffer_object	1

typedef	size_t GLsizeptrARB;
typedef	GLint GLintptrARB;
typedef	GLsizei	GLsizeiptrARB;

typedef	GLvoid (APIENTRY *PFNBINDBUFFERARB)(GLenum target, GLuint buffer);
typedef	GLvoid (APIENTRY *PFNDELETEBUFFERSARB)(GLsizei n, const	GLuint *buffers);
typedef	GLvoid (APIENTRY *PFNGENBUFFERSARB)(GLsizei	n, GLuint *buffers);
typedef	GLboolean (APIENTRY	*PFNISBUFFERARB)(GLuint	buffer);

typedef	GLvoid (APIENTRY *PFNBUFFERDATAARB)(GLenum target, GLsizeiptrARB size, const GLvoid	*data, GLenum usage);
typedef	GLvoid (APIENTRY *PFNBUFFERSUBDATAARB)(GLenum target, GLintptrARB offset, GLsizeiptrARB	size, const	GLvoid *data);
typedef	GLvoid (APIENTRY *PFNGETBUFFERSUBDATAARB)(GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid	*data);

typedef	GLvoid * (APIENTRY *PFNMAPBUFFERARB)(GLenum	target,	GLenum access);
typedef	GLboolean (APIENTRY	*PFNUNMAPBUFFERARB)(GLenum target);

typedef	GLvoid (APIENTRY *PFNGETBUFFERPARAMETERIVARB)(GLenum target, GLenum	pname, GLint *params);
typedef	GLvoid (APIENTRY *PFNGETBUFFERPOINTERVARB)(GLenum target, GLenum pname,	GLvoid **params);

#define	GL_ARRAY_BUFFER_ARB								0x8892
#define	GL_ELEMENT_ARRAY_BUFFER_ARB						0x8893

#define	GL_ARRAY_BUFFER_BINDING_ARB						0x8894
#define	GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB				0x8895
#define	GL_VERTEX_ARRAY_BUFFER_BINDING_ARB				0x8896
#define	GL_NORMAL_ARRAY_BUFFER_BINDING_ARB				0x8897
#define	GL_COLOR_ARRAY_BUFFER_BINDING_ARB				0x8898
#define	GL_INDEX_ARRAY_BUFFER_BINDING_ARB				0x8899
#define	GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB		0x889A
#define	GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB			0x889B
#define	GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB		0x889C
#define	GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB		0x889D
#define	GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB				0x889E

#define	GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB		0x889F

#define	GL_STREAM_DRAW_ARB								0x88E0
#define	GL_STREAM_READ_ARB								0x88E1
#define	GL_STREAM_COPY_ARB								0x88E2
#define	GL_STATIC_DRAW_ARB								0x88E4
#define	GL_STATIC_READ_ARB								0x88E5
#define	GL_STATIC_COPY_ARB								0x88E6
#define	GL_DYNAMIC_DRAW_ARB								0x88E8
#define	GL_DYNAMIC_READ_ARB								0x88E9
#define	GL_DYNAMIC_COPY_ARB								0x88EA

#define	GL_READ_ONLY_ARB								0x88B8
#define	GL_WRITE_ONLY_ARB								0x88B9
#define	GL_READ_WRITE_ARB								0x88BA

#define	GL_BUFFER_SIZE_ARB								0x8764
#define	GL_BUFFER_USAGE_ARB								0x8765
#define	GL_BUFFER_ACCESS_ARB							0x88BB
#define	GL_BUFFER_MAPPED_ARB							0x88BC

#define	GL_BUFFER_MAP_POINTER_ARB						0x88BD

WGLEXTERN PFNBINDBUFFERARB glBindBufferARB;
WGLEXTERN PFNDELETEBUFFERSARB glDeleteBuffersARB;
WGLEXTERN PFNGENBUFFERSARB glGenBuffersARB;
WGLEXTERN PFNISBUFFERARB glIsBufferARB;
WGLEXTERN PFNBUFFERDATAARB glBufferDataARB;
WGLEXTERN PFNBUFFERSUBDATAARB glBufferSubDataARB;
WGLEXTERN PFNGETBUFFERSUBDATAARB glGetBufferSubDataARB;
WGLEXTERN PFNMAPBUFFERARB glMapBufferARB;
WGLEXTERN PFNUNMAPBUFFERARB	glUnmapBufferARB;
WGLEXTERN PFNGETBUFFERPARAMETERIVARB glGetBufferParameterivARB;
WGLEXTERN PFNGETBUFFERPOINTERVARB glGetBufferPointervARB;

#endif // GL_ARB_vertex_buffer_object


/*****************************************************************************************************
**	EXT	EXTENSIONS
*****************************************************************************************************/

/*
** WGL_EXT_extensions_string
**
**	Support:
**	 Rage 128 *	based :	Supported
**	 Radeon	  *	based :	Supported
*/
#ifndef	WGL_EXT_extensions_string
#define	WGL_EXT_extensions_string 1

typedef	const char * (WINAPI * PFNWGLGETEXTENSIONSSTRINGEXTPROC) (VOID);

#endif /* WGL_EXT_extensions_string	*/
WGLEXTERN PFNWGLGETEXTENSIONSSTRINGEXTPROC	wglGetExtensionsStringEXT;

/*
** WGL_EXT_pixel_format
**
**	Support:
**	 Geforce2 or + : Supported
*/
#ifndef	WGL_EXT_pixel_format
#define	WGL_EXT_pixel_format	1

#define	WGL_NUMBER_PIXEL_FORMATS_EXT		 0x2000
#define	WGL_DRAW_TO_WINDOW_EXT				 0x2001
#define	WGL_DRAW_TO_BITMAP_EXT				 0x2002
#define	WGL_ACCELERATION_EXT				 0x2003
#define	WGL_NEED_PALETTE_EXT				 0x2004
#define	WGL_NEED_SYSTEM_PALETTE_EXT			 0x2005
#define	WGL_SWAP_LAYER_BUFFERS_EXT			 0x2006
#define	WGL_SWAP_METHOD_EXT					 0x2007
#define	WGL_NUMBER_OVERLAYS_EXT				 0x2008
#define	WGL_NUMBER_UNDERLAYS_EXT			 0x2009
#define	WGL_TRANSPARENT_EXT					 0x200A
#define	WGL_TRANSPARENT_VALUE_EXT			 0x200B
#define	WGL_SHARE_DEPTH_EXT					 0x200C
#define	WGL_SHARE_STENCIL_EXT				 0x200D
#define	WGL_SHARE_ACCUM_EXT					 0x200E
#define	WGL_SUPPORT_GDI_EXT					 0x200F
#define	WGL_SUPPORT_OPENGL_EXT				 0x2010
#define	WGL_DOUBLE_BUFFER_EXT				 0x2011
#define	WGL_STEREO_EXT						 0x2012
#define	WGL_PIXEL_TYPE_EXT					 0x2013
#define	WGL_COLOR_BITS_EXT					 0x2014
#define	WGL_RED_BITS_EXT					 0x2015
#define	WGL_RED_SHIFT_EXT					 0x2016
#define	WGL_GREEN_BITS_EXT					 0x2017
#define	WGL_GREEN_SHIFT_EXT					 0x2018
#define	WGL_BLUE_BITS_EXT					 0x2019
#define	WGL_BLUE_SHIFT_EXT					 0x201A
#define	WGL_ALPHA_BITS_EXT					 0x201B
#define	WGL_ALPHA_SHIFT_EXT					 0x201C
#define	WGL_ACCUM_BITS_EXT					 0x201D
#define	WGL_ACCUM_RED_BITS_EXT				 0x201E
#define	WGL_ACCUM_GREEN_BITS_EXT			 0x201F
#define	WGL_ACCUM_BLUE_BITS_EXT				 0x2020
#define	WGL_ACCUM_ALPHA_BITS_EXT			 0x2021
#define	WGL_DEPTH_BITS_EXT					 0x2022
#define	WGL_STENCIL_BITS_EXT				 0x2023
#define	WGL_AUX_BUFFERS_EXT					 0x2024
#define	WGL_NO_ACCELERATION_EXT				 0x2025
#define	WGL_GENERIC_ACCELERATION_EXT		 0x2026
#define	WGL_FULL_ACCELERATION_EXT			 0x2027
#define	WGL_SWAP_EXCHANGE_EXT				 0x2028
#define	WGL_SWAP_COPY_EXT					 0x2029
#define	WGL_SWAP_UNDEFINED_EXT				 0x202A
#define	WGL_TYPE_RGBA_EXT					 0x202B
#define	WGL_TYPE_COLORINDEX_EXT				 0x202C

typedef	BOOL (APIENTRY * PFNWGLGETPIXELFORMATATTRIBIVEXT)(
HDC	hdc, GLint iPixelFormat,GLint iLayerPlane, UINT	nAttributes,GLint *piAttributes,GLint *piValues);

typedef	BOOL (APIENTRY * PFNWGLGETPIXELFORMATATTRIBFVEXT)(
HDC	hdc, GLint iPixelFormat,GLint iLayerPlane, UINT	nAttributes,GLint *piAttributes,FLOAT *pfValues);

typedef	BOOL (APIENTRY * PFNWGLCHOOSEPIXELFORMATEXT)(
HDC	hdc, const GLint *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, GLint *piFormats, UINT *nNumFormats);

#endif // WGL_EXT_pixel_format

WGLEXTERN PFNWGLGETPIXELFORMATATTRIBIVEXT		wglGetPixelFormatAttribivEXT;
WGLEXTERN PFNWGLGETPIXELFORMATATTRIBFVEXT		wglGetPixelFormatAttribfvEXT;
WGLEXTERN PFNWGLCHOOSEPIXELFORMATEXT			wglChoosePixelFormatEXT;


/*
** WGL_EXT_pbuffer
**
**	Support:
**	 Geforce2 or + : Supported
*/

#ifndef	WGL_EXT_pbuffer
#define	WGL_EXT_pbuffer	1

#define	WGL_DRAW_TO_PBUFFER_EXT			  0x202D
#define	WGL_MAX_PBUFFER_PIXELS_EXT		  0x202E
#define	WGL_MAX_PBUFFER_WIDTH_EXT		  0x202F
#define	WGL_MAX_PBUFFER_HEIGHT_EXT		  0x2030
#define	WGL_OPTIMAL_PBUFFER_WIDTH_EXT	  0x2031
#define	WGL_OPTIMAL_PBUFFER_HEIGHT_EXT	  0x2032
#define	WGL_PBUFFER_LARGEST_EXT			  0x2033
#define	WGL_PBUFFER_WIDTH_EXT			  0x2034
#define	WGL_PBUFFER_HEIGHT_EXT			  0x2035

typedef	GLvoid * HPBUFFEREXT;
typedef	HPBUFFEREXT	(APIENTRY *PFNWGLCREATEPBUFFEREXT)(HDC hDC,	GLint iPixelFormat,	GLint iWidth, GLint	iHeight, const GLint *piAttribList);
typedef	HDC	(APIENTRY *PFNWGLGETPBUFFERDCEXT)(HPBUFFEREXT hPbuffer);
typedef	GLint (APIENTRY	*PFWGLRELEASEPBUFFERDCEXT)(HPBUFFEREXT hPbuffer, HDC hDC);
typedef	BOOL(APIENTRY *PFWGLDESTROYPBUFFEREXT)(HPBUFFEREXT hPbuffer);
typedef	BOOL(APIENTRY *PFWGLQUERYPBUFFEREXT)(HPBUFFEREXT hPbuffer, GLint iAttribute, GLint *piValue);

#endif

WGLEXTERN PFNWGLCREATEPBUFFEREXT			wglCreatePbufferEXT;
WGLEXTERN PFNWGLGETPBUFFERDCEXT				wglGetPbufferDCEXT;
WGLEXTERN PFWGLRELEASEPBUFFERDCEXT			wglReleasePbufferDCEXT;
WGLEXTERN PFWGLDESTROYPBUFFEREXT			wglDestroyPbufferEXT;
WGLEXTERN PFWGLQUERYPBUFFEREXT				wglQueryPbufferEXT;

/*
** WGL_EXT_swap_control
**
**	Support:
**	 Geforce2 or + : Supported
*/

#ifndef	WGL_EXT_swap_control
#define	WGL_EXT_swap_control 1

typedef	GLint (APIENTRY	* PFNWGLSWAPINTERVALEXTPROC) (GLint);
typedef	GLint (APIENTRY	* PFNWGLGETSWAPINTERVALEXTPROC)	(void);

#endif

WGLEXTERN PFNWGLSWAPINTERVALEXTPROC			wglSwapIntervalEXT;
WGLEXTERN PFNWGLGETSWAPINTERVALEXTPROC			wglGetSwapIntervalEXT;

/*
** GL_WIN_swap_hint
**
**	Support:
**	 Geforce2 or + : Supported
*/

#ifndef	GL_WIN_swap_hint
#define	GL_WIN_swap_hint 1
typedef	GLvoid (APIENTRY * PFNGLADDSWAPHINTRECTWINPROC)	(GLint x, GLint	y, GLsizei width, GLsizei height);
#endif
WGLEXTERN PFNGLADDSWAPHINTRECTWINPROC			glAddSwapHintRectWIN;


#endif // _WIN32


#ifndef	GL_EXT_texture_edge_clamp
#define	GL_EXT_texture_edge_clamp				1
#define	GL_CLAMP_TO_EDGE_EXT					0x812F
#endif

/*
** GL_EXT_stencil_two_side
**
**	Support:
**	 GeforceFX or +	: Supported
*/

#ifndef	GL_EXT_stencil_two_side
#define	GL_EXT_stencil_two_side	1

typedef	GLvoid (APIENTRY * PFNACTIVESTENCILFACEEXTPROC)	(GLenum	face);
WGLEXTERN PFNACTIVESTENCILFACEEXTPROC			glActiveStencilFaceEXT;

#define	GL_STENCIL_TEST_TWO_SIDE_EXT				 0x8910
#define	GL_ACTIVE_STENCIL_FACE_EXT					 0x8911
#endif


/*
** GL_EXT_cull_vertex
**
**	Support:
**	 Geforce2 or + : Supported
*/

#ifndef	GL_EXT_cull_vertex
#define	GL_EXT_cull_vertex	1

#define	GL_CULL_VERTEX_EXT						0x81AA
#define	GL_CULL_VERTEX_EYE_POSITION_EXT			0x81AB
#define	GL_CULL_VERTEX_OBJECT_POSITION_EXT		0x81AC

typedef	GLvoid (APIENTRY * PFNGLCULLPARAMETERDVEXTPROC)	(GLenum	pname, GLdouble* params);
typedef	GLvoid (APIENTRY * PFNGLCULLPARAMETERFVEXTPROC)	(GLenum	pname, GLfloat*	params);

#endif

/*
** GL_EXT_point_parameter
**
**	Support:
**	 Geforce2 or + : Supported
*/

#ifndef	GL_EXT_point_parameter
#define	GL_EXT_point_parameter	1

#define	GL_POINT_SIZE_MIN_EXT					0x8126
#define	GL_POINT_SIZE_MAX_EXT					0x8127
#define	GL_POINT_FADE_THRESHOLD_SIZE_EXT		0x8128
#define	GL_POINT_DISTANCE_ATTENUATION_EXT		0x8129

typedef	GLvoid (APIENTRY * PFNGLPOINTPARAMETERFEXTPROC)	(GLenum	pname, GLfloat param);
typedef	GLvoid (APIENTRY * PFNGLPOINTPARAMETERFVEXTPROC) (GLenum pname,	const GLfloat *params);

#endif

/*
** GL_EXT_separate_specular_color
**
**	Support:
**	 Geforce2 or + : Supported
*/
#ifndef	GL_EXT_separate_specular_color
#define	GL_EXT_separate_specular_color	1

#define	GL_CURRENT_SECONDARY_COLOR_EXT			0x8459
#define	GL_SECONDARY_COLOR_ARRAY_SIZE_EXT		0x845A
#define	GL_SECONDARY_COLOR_ARRAY_TYPE_EX		0x845B
#define	GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT		0x845C
#define	GL_SECONDARY_COLOR_ARRAY_POINTER_EXT	0x845D
#define	GL_SECONDARY_COLOR_ARRAY_EXT			0x845E
#define	GL_LIGHT_MODEL_COLOR_CONTROL			0x81F8
#define	GL_SINGLE_COLOR							0x81F9
#define	GL_SEPARATE_SPECULAR_COLOR				0x81FA

typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3BEXTPROC) (GLbyte red, GLbyte green,	GLbyte blue);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3SEXTPROC) (GLshort red, GLshort green, GLshort blue);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3IEXTPROC) (GLint	red, GLint green, GLint	blue);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3FEXTPROC) (GLfloat red, GLfloat green, GLfloat blue);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3DEXTPROC) (GLdouble red,	GLdouble green,	GLdouble blue);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3UBEXTPROC) (GLubyte red,	GLubyte	green, GLubyte blue);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3USEXTPROC) (GLushort	red, GLushort green, GLushort blue);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3UIEXTPROC) (GLuint red, GLuint green, GLuint	blue);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3BVEXTPROC) (const GLbyte	*v);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3SVEXTPROC) (const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3IVEXTPROC) (const GLint *v);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3FVEXTPROC) (const GLfloat *v);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3DVEXTPROC) (const GLdouble *v);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3UBVEXTPROC) (const GLubyte *v);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3USVEXTPROC) (const GLushort *v);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLOR3UIVEXTPROC) (const GLuint *v);
typedef	GLvoid (APIENTRY * PFNGLSECONDARYCOLORPOINTEREXTPROC) (GLint size, GLenum type,	GLsizei	stride,	GLvoid *pointer);

#endif

WGLEXTERN PFNGLSECONDARYCOLOR3UBVEXTPROC		glSecondaryColor3ubvEXT;

/*
** GL_EXT_paletted_texture
**
**	Support:
**	 Geforce2 or + : Supported
*/

#ifndef	GL_EXT_paletted_texture
#define	GL_EXT_paletted_texture		1

typedef	GLvoid (APIENTRY * PFNGLCOLORTABLEEXTPROC) (GLenum target, GLenum internalFormat, GLsizei width, GLenum	format,GLenum type,	const GLvoid *data);
#endif

WGLEXTERN PFNGLCOLORTABLEEXTPROC				glColorTableEXT;

// GL_EXT_color_sum
#define	GL_COLOR_SUM_EXT						0x8458

// GL_EXT_clip_volume_hint
#define	GL_CLIP_VOLUME_CLIPPING_HINT_EXT		0x80F0


#ifndef	GL_EXT_texture_lod_bias
#define	GL_EXT_texture_lod_bias		1

#define	GL_MAX_TEXTURE_LOD_BIAS_EXT				0x84FD
#define	GL_TEXTURE_FILTER_CONTROL_EXT			0x8500
#define	GL_TEXTURE_LOD_BIAS_EXT					0x8501
#endif


/*
** GL_EXT_fog_coord
**
**	Support:
**	 Geforce2 or + : Supported
*/
#ifndef	GL_EXT_fog_coord
#define	GL_EXT_fog_coord	1

#define	GL_FOG_COORDINATE_SOURCE_EXT			 0x8450
#define	GL_FOG_COORDINATE_EXT					 0x8451
#define	GL_FRAGMENT_DEPTH_EXT					 0x8452
#define	GL_CURRENT_FOG_COORDINATE_EXT			 0x8453
#define	GL_FOG_COORDINATE_ARRAY_TYPE_EXT		 0x8454
#define	GL_FOG_COORDINATE_ARRAY_STRIDE_EXT		 0x8455
#define	GL_FOG_COORDINATE_ARRAY_POINTER_EXT		 0x8456
#define	GL_FOG_COORDINATE_ARRAY_EXT				 0x8457

typedef	GLvoid (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat coord);
typedef	GLvoid (APIENTRY * PFNGLFOGCOORDFVEXTPROC) (const GLfloat *coord);
typedef	GLvoid (APIENTRY * PFNGLFOGCOORDDEXTPROC) (GLdouble	coord);
typedef	GLvoid (APIENTRY * PFNGLFOGCOORDDVEXTPROC) (const GLdouble *coord);
typedef	GLvoid (APIENTRY * PFNGLFOGCOORDPOINTEREXTPROC)	(GLenum	type, GLsizei stride, const	GLvoid *pointer);

#endif

WGLEXTERN PFNGLFOGCOORDFEXTPROC					glFogCoordfEXT;

/*
** GL_EXT_vertex_array
**
**	Support:
**	 Geforce2 or + : Supported
*/

#ifndef	GL_EXT_multi_draw_arrays
#define	GL_EXT_multi_draw_arrays 1

typedef	void (APIENTRY * PFNGLMULTIDRAWARRAYSEXTPROC) (GLenum mode,	GLint *first, GLsizei *count, GLsizei primcount);
typedef	void (APIENTRY * PFNGLMULTIDRAWELEMENTSEXTPROC)	(GLenum	mode, const	GLsizei	*count,	GLenum type, const GLvoid* *indices, GLsizei primcount);

WGLEXTERN PFNGLMULTIDRAWARRAYSEXTPROC glMultiDrawArraysEXT;
WGLEXTERN PFNGLMULTIDRAWELEMENTSEXTPROC	glMultiDrawElementsEXT;

#endif

/*
** GL_EXT_vertex_array
**
**	Support:
**	 Geforce2 or + : Supported
*/
#ifndef	GL_EXT_vertex_array
#define	GL_EXT_vertex_array	1

#define	GL_VERTEX_ARRAY_EXT						0x8074
#define	GL_NORMAL_ARRAY_EXT						0x8075
#define	GL_COLOR_ARRAY_EXT						0x8076
#define	GL_INDEX_ARRAY_EXT						0x8077
#define	GL_TEXTURE_COORD_ARRAY_EXT				0x8078
#define	GL_EDGE_FLAG_ARRAY_EXT					0x8079
#define	GL_VERTEX_ARRAY_SIZE_EXT				0x807A
#define	GL_VERTEX_ARRAY_TYPE_EXT				0x807B
#define	GL_VERTEX_ARRAY_STRIDE_EXT				0x807C
#define	GL_VERTEX_ARRAY_COUNT_EXT				0x807D
#define	GL_NORMAL_ARRAY_TYPE_EXT				0x807E
#define	GL_NORMAL_ARRAY_STRIDE_EXT				0x807F
#define	GL_NORMAL_ARRAY_COUNT_EXT				0x8080
#define	GL_COLOR_ARRAY_SIZE_EXT					0x8081
#define	GL_COLOR_ARRAY_TYPE_EXT					0x8082
#define	GL_COLOR_ARRAY_STRIDE_EXT				0x8083
#define	GL_COLOR_ARRAY_COUNT_EXT				0x8084
#define	GL_INDEX_ARRAY_TYPE_EXT					0x8085
#define	GL_INDEX_ARRAY_STRIDE_EXT				0x8086
#define	GL_INDEX_ARRAY_COUNT_EXT				0x8087
#define	GL_TEXTURE_COORD_ARRAY_SIZE_EXT			0x8088
#define	GL_TEXTURE_COORD_ARRAY_TYPE_EXT			0x8089
#define	GL_TEXTURE_COORD_ARRAY_STRIDE_EXT		0x808A
#define	GL_TEXTURE_COORD_ARRAY_COUNT_EXT		0x808B
#define	GL_EDGE_FLAG_ARRAY_STRIDE_EXT			0x808C
#define	GL_EDGE_FLAG_ARRAY_COUNT_EXT			0x808D
#define	GL_VERTEX_ARRAY_POINTER_EXT				0x808E
#define	GL_NORMAL_ARRAY_POINTER_EXT				0x808F
#define	GL_COLOR_ARRAY_POINTER_EXT				0x8090
#define	GL_INDEX_ARRAY_POINTER_EXT				0x8091
#define	GL_TEXTURE_COORD_ARRAY_POINTER_EXT		0x8092
#define	GL_EDGE_FLAG_ARRAY_POINTER_EXT			0x8093

typedef	GLvoid (APIENTRY * PFNGLARRAYELEMENTEXTPROC) (GLint	i);
typedef	GLvoid (APIENTRY * PFNGLCOLORPOINTEREXTPROC) (GLint	size, GLenum type, GLsizei stride, GLsizei count, const	GLvoid *pointer);
typedef	GLvoid (APIENTRY * PFNGLDRAWARRAYSEXTPROC) (GLenum mode, GLint first, GLsizei count);
typedef	GLvoid (APIENTRY * PFNGLEDGEFLAGPOINTEREXTPROC)	(GLsizei stride, GLsizei count,	const GLboolean	*pointer);
typedef	GLvoid (APIENTRY * PFNGLGETPOINTERVEXTPROC)	(GLenum	pname, GLvoid* *params);
typedef	GLvoid (APIENTRY * PFNGLINDEXPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const	GLvoid *pointer);
typedef	GLvoid (APIENTRY * PFNGLNORMALPOINTEREXTPROC) (GLenum type,	GLsizei	stride,	GLsizei	count, const GLvoid	*pointer);
typedef	GLvoid (APIENTRY * PFNGLTEXCOORDPOINTEREXTPROC)	(GLint size, GLenum	type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef	GLvoid (APIENTRY * PFNGLVERTEXPOINTEREXTPROC) (GLint size, GLenum type,	GLsizei	stride,	GLsizei	count, const GLvoid	*pointer);

WGLEXTERN PFNGLARRAYELEMENTEXTPROC glArrayElementEXT (GLint);
WGLEXTERN PFNGLCOLORPOINTEREXTPROCG	glColorPointerEXT (GLint, GLenum, GLsizei, GLsizei,	const GLvoid *);
WGLEXTERN PFNGLDRAWARRAYSEXTPROC glDrawArraysEXT (GLenum, GLint, GLsizei);
WGLEXTERN PFNGLEDGEFLAGPOINTEREXTPROC glEdgeFlagPointerEXT (GLsizei, GLsizei, const	GLboolean *);
WGLEXTERN PFNGLGETPOINTERVEXTPROC glGetPointervEXT (GLenum,	GLvoid*	*);
WGLEXTERN PFNGLINDEXPOINTEREXTPROC glIndexPointerEXT (GLenum, GLsizei, GLsizei,	const GLvoid *);
WGLEXTERN PFNGLNORMALPOINTEREXTPROC	glNormalPointerEXT (GLenum,	GLsizei, GLsizei, const	GLvoid *);
WGLEXTERN PFNGLTEXCOORDPOINTEREXTPROC glTexCoordPointerEXT (GLint, GLenum, GLsizei,	GLsizei, const GLvoid *);
WGLEXTERN PFNGLVERTEXPOINTEREXTPROC	glVertexPointerEXT (GLint, GLenum, GLsizei,	GLsizei, const GLvoid *);

#endif

/*
** GL_EXT_compiled_vertex_array
**
**	Support:
**	 Geforce2 or + : Supported
*/

#ifndef	GL_VERSION_1_2
#ifndef	GL_EXT_compiled_vertex_array
#define	GL_EXT_compiled_vertex_array 1

#define	GL_ARRAY_ELEMENT_LOCK_FIRST_EXT			0x81A8
#define	GL_ARRAY_ELEMENT_LOCK_COUNT_EXT			0x81A9

typedef	GLvoid (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef	GLvoid (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);

#endif // GL_EXT_compiled_vertex_array
WGLEXTERN PFNGLLOCKARRAYSEXTPROC		glLockArraysEXT;
WGLEXTERN PFNGLUNLOCKARRAYSEXTPROC		glUnlockArraysEXT;
#endif // GL_VERSION_1_2

/*
** GL_EXT_draw_range_elements
**
**	Support:
**	 Geforce2 or + : Supported
*/

#ifndef	 GL_EXT_draw_range_elements
#define	 GL_EXT_draw_range_elements	1

typedef	GLvoid (APIENTRY *	PFNGLDRAWRANGEELEMENTSEXTPROC)(	GLenum mode, GLuint	start, GLuint end, GLsizei count, GLenum type, const GLvoid	*indices);

#endif

WGLEXTERN PFNGLDRAWRANGEELEMENTSEXTPROC	glDrawRangeElementsEXT;

/*
** GL_EXT_vertex_weighting
**
**	Support:
**	 Geforce2 or + : Supported
*/

#ifndef	GL_EXT_vertex_weighting
#define	GL_EXT_vertex_weighting	1

#define	GL_VERTEX_WEIGHTING_EXT					 0x8509

#define	GL_MODELVIEW0_EXT						 0x1700
#define	GL_MODELVIEW1_EXT						 0x850a

#define	GL_CURRENT_VERTEX_WEIGHT_EXT			 0x850b
#define	GL_VERTEX_WEIGHT_ARRAY_EXT				 0x850c
#define	GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT			 0x850d
#define	GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT			 0x850e
#define	GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT		 0x850f
#define	GL_MODELVIEW0_STACK_DEPTH_EXT			 0x0BA3
#define	GL_MODELVIEW1_STACK_DEPTH_EXT			 0x8502
#define	GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT		 0x8510

typedef	GLvoid (APIENTRY *PFNGLVERTEXWEIGHTFEXTPROC)(float weight);
typedef	GLvoid (APIENTRY *PFNGLVERTEXWEIGHTFVEXTPROC)(float	*weight);
typedef	GLvoid (APIENTRY *PFNGLVERTEXWEIGHTPOINTEREXTPROC)(GLint size, enum	type, GLsizei stride, void *pointer);

#endif

WGLEXTERN PFNGLVERTEXWEIGHTFEXTPROC			glVertexWeightfEXT;
WGLEXTERN PFNGLVERTEXWEIGHTFVEXTPROC		glVertexWeightfvEXT;
WGLEXTERN PFNGLVERTEXWEIGHTPOINTEREXTPROC	glVertexWeightPointerEXT;

/* EXT_abgr	*/
#define	GL_ABGR_EXT								0x8000

/* EXT_bgra	*/
#define	GL_BGR_EXT								0x80E0
#define	GL_BGRA_EXT								0x80E1
#define	GL_MAX_ELEMENTS_VERTICES_EXT			0x80E8
#define	GL_MAX_ELEMENTS_INDICES_EXT				0x80E9


/* EXT_shared_texture_palette */
#define	GL_SHARED_TEXTURE_PALETTE_EXT			0x81FB

/* GL_EXT_packed_pixels	*/
#ifndef	GL_EXT_packed_pixels
#define	GL_EXT_packed_pixels 1

#define	GL_UNSIGNED_BYTE_3_3_2_EXT				0x8032
#define	GL_UNSIGNED_SHORT_4_4_4_4_EXT			0x8033
#define	GL_UNSIGNED_SHORT_5_5_5_1_EXT			0x8034
#define	GL_UNSIGNED_INT_8_8_8_8_EXT				0x8035
#define	GL_UNSIGNED_INT_10_10_10_2_EXT			0x8036

#endif

/* GL_EXT_texture_rectangle*/
#ifndef	GL_EXT_texture_rectangle
#define	GL_EXT_texture_rectangle			1

#define	GL_TEXTURE_RECTANGLE_EXT				0x84F5
#define	GL_TEXTURE_BINDING_RECTANGLE_EXT		0x84F6
#define	GL_PROXY_TEXTURE_RECTANGLE_EXT			0x84F7
#define	GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT		0x84F8

#endif /* GL_EXT_texture_rectangle */

/* EXT_clip_volume_hint	*/
#define	GL_CLIP_VOLUME_CLIPPING_HINT_EXT		0x80F0

/* EXT_stencil_wrap	*/
#define	GL_INCR_WRAP_EXT						0x8507
#define	GL_DECR_WRAP_EXT						0x8508


/* EXT_texture_filter_anisotropic */
#ifndef	GL_EXT_texture_filter_anisotropic
#define	GL_EXT_texture_filter_anisotropic	1

#define	GL_TEXTURE_MAX_ANISOTROPY_EXT			0x84fe
#define	GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT		0x84ff

#endif

/*
** GL_EXT_vertex_shader
**
**	Support:
**	 Rage 128 *	based :	Supported (software)
**	 Radeon	  *	based :	Supported (software)
**	 R200	  *	based :	Supported
*/
#ifndef	GL_EXT_vertex_shader
#define	GL_EXT_vertex_shader 1

#define	GL_VERTEX_SHADER_EXT								0x8780
#define	GL_VERTEX_SHADER_BINDING_EXT						0x8781
#define	GL_OP_INDEX_EXT										0x8782
#define	GL_OP_NEGATE_EXT									0x8783
#define	GL_OP_DOT3_EXT										0x8784
#define	GL_OP_DOT4_EXT										0x8785
#define	GL_OP_MUL_EXT										0x8786
#define	GL_OP_ADD_EXT										0x8787
#define	GL_OP_MADD_EXT										0x8788
#define	GL_OP_FRAC_EXT										0x8789
#define	GL_OP_MAX_EXT										0x878A
#define	GL_OP_MIN_EXT										0x878B
#define	GL_OP_SET_GE_EXT									0x878C
#define	GL_OP_SET_LT_EXT									0x878D
#define	GL_OP_CLAMP_EXT										0x878E
#define	GL_OP_FLOOR_EXT										0x878F
#define	GL_OP_ROUND_EXT										0x8790
#define	GL_OP_EXP_BASE_2_EXT								0x8791
#define	GL_OP_LOG_BASE_2_EXT								0x8792
#define	GL_OP_POWER_EXT										0x8793
#define	GL_OP_RECIP_EXT										0x8794
#define	GL_OP_RECIP_SQRT_EXT								0x8795
#define	GL_OP_SUB_EXT										0x8796
#define	GL_OP_CROSS_PRODUCT_EXT								0x8797
#define	GL_OP_MULTIPLY_MATRIX_EXT							0x8798
#define	GL_OP_MOV_EXT										0x8799
#define	GL_OUTPUT_VERTEX_EXT								0x879A
#define	GL_OUTPUT_COLOR0_EXT								0x879B
#define	GL_OUTPUT_COLOR1_EXT								0x879C
#define	GL_OUTPUT_TEXTURE_COORD0_EXT						0x879D
#define	GL_OUTPUT_TEXTURE_COORD1_EXT						0x879E
#define	GL_OUTPUT_TEXTURE_COORD2_EXT						0x879F
#define	GL_OUTPUT_TEXTURE_COORD3_EXT						0x87A0
#define	GL_OUTPUT_TEXTURE_COORD4_EXT						0x87A1
#define	GL_OUTPUT_TEXTURE_COORD5_EXT						0x87A2
#define	GL_OUTPUT_TEXTURE_COORD6_EXT						0x87A3
#define	GL_OUTPUT_TEXTURE_COORD7_EXT						0x87A4
#define	GL_OUTPUT_TEXTURE_COORD8_EXT						0x87A5
#define	GL_OUTPUT_TEXTURE_COORD9_EXT						0x87A6
#define	GL_OUTPUT_TEXTURE_COORD10_EXT						0x87A7
#define	GL_OUTPUT_TEXTURE_COORD11_EXT						0x87A8
#define	GL_OUTPUT_TEXTURE_COORD12_EXT						0x87A9
#define	GL_OUTPUT_TEXTURE_COORD13_EXT						0x87AA
#define	GL_OUTPUT_TEXTURE_COORD14_EXT						0x87AB
#define	GL_OUTPUT_TEXTURE_COORD15_EXT						0x87AC
#define	GL_OUTPUT_TEXTURE_COORD16_EXT						0x87AD
#define	GL_OUTPUT_TEXTURE_COORD17_EXT						0x87AE
#define	GL_OUTPUT_TEXTURE_COORD18_EXT						0x87AF
#define	GL_OUTPUT_TEXTURE_COORD19_EXT						0x87B0
#define	GL_OUTPUT_TEXTURE_COORD20_EXT						0x87B1
#define	GL_OUTPUT_TEXTURE_COORD21_EXT						0x87B2
#define	GL_OUTPUT_TEXTURE_COORD22_EXT						0x87B3
#define	GL_OUTPUT_TEXTURE_COORD23_EXT						0x87B4
#define	GL_OUTPUT_TEXTURE_COORD24_EXT						0x87B5
#define	GL_OUTPUT_TEXTURE_COORD25_EXT						0x87B6
#define	GL_OUTPUT_TEXTURE_COORD26_EXT						0x87B7
#define	GL_OUTPUT_TEXTURE_COORD27_EXT						0x87B8
#define	GL_OUTPUT_TEXTURE_COORD28_EXT						0x87B9
#define	GL_OUTPUT_TEXTURE_COORD29_EXT						0x87BA
#define	GL_OUTPUT_TEXTURE_COORD30_EXT						0x87BB
#define	GL_OUTPUT_TEXTURE_COORD31_EXT						0x87BC
#define	GL_OUTPUT_FOG_EXT									0x87BD
#define	GL_SCALAR_EXT										0x87BE
#define	GL_VECTOR_EXT										0x87BF
#define	GL_MATRIX_EXT										0x87C0
#define	GL_VARIANT_EXT										0x87C1
#define	GL_INVARIANT_EXT									0x87C2
#define	GL_LOCAL_CONSTANT_EXT								0x87C3
#define	GL_LOCAL_EXT										0x87C4
#define	GL_MAX_VERTEX_SHADER_INSTRUCTIONS_EXT				0x87C5
#define	GL_MAX_VERTEX_SHADER_VARIANTS_EXT					0x87C6
#define	GL_MAX_VERTEX_SHADER_INVARIANTS_EXT					0x87C7
#define	GL_MAX_VERTEX_SHADER_LOCAL_CONSTANTS_EXT			0x87C8
#define	GL_MAX_VERTEX_SHADER_LOCALS_EXT						0x87C9
#define	GL_MAX_OPTIMIZED_VERTEX_SHADER_INSTRUCTIONS_EXT		0x87CA
#define	GL_MAX_OPTIMIZED_VERTEX_SHADER_VARIANTS_EXT			0x87CB
#define	GL_MAX_OPTIMIZED_VERTEX_SHADER_INVARIANTS_EXT		0x87CC
#define	GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCAL_CONSTANTS_EXT	0x87CD
#define	GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCALS_EXT			0x87CE
#define	GL_VERTEX_SHADER_INSTRUCTIONS_EXT					0x87CF
#define	GL_VERTEX_SHADER_VARIANTS_EXT						0x87D0
#define	GL_VERTEX_SHADER_INVARIANTS_EXT						0x87D1
#define	GL_VERTEX_SHADER_LOCAL_CONSTANTS_EXT				0x87D2
#define	GL_VERTEX_SHADER_LOCALS_EXT							0x87D3
#define	GL_VERTEX_SHADER_OPTIMIZED_EXT						0x87D4
#define	GL_X_EXT											0x87D5
#define	GL_Y_EXT											0x87D6
#define	GL_Z_EXT											0x87D7
#define	GL_W_EXT											0x87D8
#define	GL_NEGATIVE_X_EXT									0x87D9
#define	GL_NEGATIVE_Y_EXT									0x87DA
#define	GL_NEGATIVE_Z_EXT									0x87DB
#define	GL_NEGATIVE_W_EXT									0x87DC
#define	GL_ZERO_EXT											0x87DD
#define	GL_ONE_EXT											0x87DE
#define	GL_NEGATIVE_ONE_EXT									0x87DF
#define	GL_NORMALIZED_RANGE_EXT								0x87E0
#define	GL_FULL_RANGE_EXT									0x87E1
#define	GL_CURRENT_VERTEX_EXT								0x87E2
#define	GL_MVP_MATRIX_EXT									0x87E3
#define	GL_VARIANT_VALUE_EXT								0x87E4
#define	GL_VARIANT_DATATYPE_EXT								0x87E5
#define	GL_VARIANT_ARRAY_STRIDE_EXT							0x87E6
#define	GL_VARIANT_ARRAY_TYPE_EXT							0x87E7
#define	GL_VARIANT_ARRAY_EXT								0x87E8
#define	GL_VARIANT_ARRAY_POINTER_EXT						0x87E9
#define	GL_INVARIANT_VALUE_EXT								0x87EA
#define	GL_INVARIANT_DATATYPE_EXT							0x87EB
#define	GL_LOCAL_CONSTANT_VALUE_EXT							0x87EC
#define	GL_LOCAL_CONSTANT_DATATYPE_EXT						0x87ED

typedef	GLvoid	  (APIENTRY	* PFNGLBEGINVERTEXSHADEREXTPROC) (void);
typedef	GLvoid	  (APIENTRY	* PFNGLENDVERTEXSHADEREXTPROC) (void);
typedef	GLvoid	  (APIENTRY	* PFNGLBINDVERTEXSHADEREXTPROC)	(GLuint	id);
typedef	GLuint	  (APIENTRY	* PFNGLGENVERTEXSHADERSEXTPROC)	(GLuint	range);
typedef	GLvoid	  (APIENTRY	* PFNGLDELETEVERTEXSHADEREXTPROC) (GLuint id);
typedef	GLvoid	  (APIENTRY	* PFNGLSHADEROP1EXTPROC) (GLenum op, GLuint	res, GLuint	arg1);
typedef	GLvoid	  (APIENTRY	* PFNGLSHADEROP2EXTPROC) (GLenum op, GLuint	res, GLuint	arg1,
													  GLuint arg2);
typedef	GLvoid	  (APIENTRY	* PFNGLSHADEROP3EXTPROC) (GLenum op, GLuint	res, GLuint	arg1,
													  GLuint arg2, GLuint arg3);
typedef	GLvoid	  (APIENTRY	* PFNGLSWIZZLEEXTPROC) (GLuint res,	GLuint in, GLenum outX,
													GLenum outY, GLenum	outZ, GLenum outW);
typedef	GLvoid	  (APIENTRY	* PFNGLWRITEMASKEXTPROC) (GLuint res, GLuint in, GLenum	outX,
													  GLenum outY, GLenum outZ,	GLenum outW);
typedef	GLvoid	  (APIENTRY	* PFNGLINSERTCOMPONENTEXTPROC) (GLuint res,	GLuint src,	GLuint num);
typedef	GLvoid	  (APIENTRY	* PFNGLEXTRACTCOMPONENTEXTPROC)	(GLuint	res, GLuint	src, GLuint	num);
typedef	GLuint	  (APIENTRY	* PFNGLGENSYMBOLSEXTPROC) (GLenum dataType,	GLenum storageType,
													   GLenum range, GLuint	components);
typedef	GLvoid	  (APIENTRY	* PFNGLSETINVARIANTEXTPROC)	(GLuint	id,	GLenum type, GLvoid	*addr);
typedef	GLvoid	  (APIENTRY	* PFNGLSETLOCALCONSTANTEXTPROC)	(GLuint	id,	GLenum type, GLvoid	*addr);
typedef	GLvoid	  (APIENTRY	* PFNGLVARIANTBVEXTPROC) (GLuint id, GLbyte	*addr);
typedef	GLvoid	  (APIENTRY	* PFNGLVARIANTSVEXTPROC) (GLuint id, GLshort *addr);
typedef	GLvoid	  (APIENTRY	* PFNGLVARIANTIVEXTPROC) (GLuint id, GLint *addr);
typedef	GLvoid	  (APIENTRY	* PFNGLVARIANTFVEXTPROC) (GLuint id, GLfloat *addr);
typedef	GLvoid	  (APIENTRY	* PFNGLVARIANTDVEXTPROC) (GLuint id, GLdouble *addr);
typedef	GLvoid	  (APIENTRY	* PFNGLVARIANTUBVEXTPROC) (GLuint id, GLubyte *addr);
typedef	GLvoid	  (APIENTRY	* PFNGLVARIANTUSVEXTPROC) (GLuint id, GLushort *addr);
typedef	GLvoid	  (APIENTRY	* PFNGLVARIANTUIVEXTPROC) (GLuint id, GLuint *addr);
typedef	GLvoid	  (APIENTRY	* PFNGLVARIANTPOINTEREXTPROC) (GLuint id, GLenum type,
														   GLuint stride, GLvoid *addr);
typedef	GLvoid	  (APIENTRY	* PFNGLENABLEVARIANTCLIENTSTATEEXTPROC)	(GLuint	id);
typedef	GLvoid	  (APIENTRY	* PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC) (GLuint id);
typedef	GLuint	  (APIENTRY	* PFNGLBINDLIGHTPARAMETEREXTPROC) (GLenum light, GLenum	value);
typedef	GLuint	  (APIENTRY	* PFNGLBINDMATERIALPARAMETEREXTPROC) (GLenum face, GLenum value);
typedef	GLuint	  (APIENTRY	* PFNGLBINDTEXGENPARAMETEREXTPROC) (GLenum unit, GLenum	coord,
																GLenum value);
typedef	GLuint	  (APIENTRY	* PFNGLBINDTEXTUREUNITPARAMETEREXTPROC)	(GLenum	unit, GLenum value);
typedef	GLuint	  (APIENTRY	* PFNGLBINDPARAMETEREXTPROC) (GLenum value);
typedef	GLboolean (APIENTRY	* PFNGLISVARIANTENABLEDEXTPROC)	(GLuint	id,	GLenum cap);
typedef	GLvoid	  (APIENTRY	* PFNGLGETVARIANTBOOLEANVEXTPROC) (GLuint id, GLenum value,
															   GLboolean *data);
typedef	GLvoid	  (APIENTRY	* PFNGLGETVARIANTINTEGERVEXTPROC) (GLuint id, GLenum value,
															   GLint *data);
typedef	GLvoid	  (APIENTRY	* PFNGLGETVARIANTFLOATVEXTPROC)	(GLuint	id,	GLenum value,
															   GLfloat *data);
typedef	GLvoid	  (APIENTRY	* PFNGLGETVARIANTPOINTERVEXTPROC) (GLuint id, GLenum value,
															   GLvoid **data);
typedef	GLvoid	  (APIENTRY	* PFNGLGETINVARIANTBOOLEANVEXTPROC)	(GLuint	id,	GLenum value,
																 GLboolean *data);
typedef	GLvoid	  (APIENTRY	* PFNGLGETINVARIANTINTEGERVEXTPROC)	(GLuint	id,	GLenum value,
																 GLint *data);
typedef	GLvoid	  (APIENTRY	* PFNGLGETINVARIANTFLOATVEXTPROC) (GLuint id, GLenum value,
															   GLfloat *data);
typedef	GLvoid	  (APIENTRY	* PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC)	(GLuint	id,	GLenum value,
																	 GLboolean *data);
typedef	GLvoid	  (APIENTRY	* PFNGLGETLOCALCONSTANTINTEGERVEXTPROC)	(GLuint	id,	GLenum value,
																	 GLint *data);
typedef	GLvoid	  (APIENTRY	* PFNGLGETLOCALCONSTANTFLOATVEXTPROC) (GLuint id, GLenum value,
																   GLfloat *data);


WGLEXTERN PFNGLBEGINVERTEXSHADEREXTPROC			glBeginVertexShaderEXT;
WGLEXTERN PFNGLENDVERTEXSHADEREXTPROC			glEndVertexShaderEXT;
WGLEXTERN PFNGLBINDVERTEXSHADEREXTPROC			glBindVertexShaderEXT;
WGLEXTERN PFNGLGENVERTEXSHADERSEXTPROC			glGenVertexShadersEXT;
WGLEXTERN PFNGLDELETEVERTEXSHADEREXTPROC		glDeleteVertexShaderEXT;
WGLEXTERN PFNGLSHADEROP1EXTPROC					glShaderOp1EXT;
WGLEXTERN PFNGLSHADEROP2EXTPROC					glShaderOp2EXT;
WGLEXTERN PFNGLSHADEROP3EXTPROC					glShaderOp3EXT;
WGLEXTERN PFNGLSWIZZLEEXTPROC					glSwizzleEXT;
WGLEXTERN PFNGLWRITEMASKEXTPROC					glWriteMaskEXT;
WGLEXTERN PFNGLINSERTCOMPONENTEXTPROC			glInsertComponentEXT;
WGLEXTERN PFNGLEXTRACTCOMPONENTEXTPROC			glExtractComponentEXT;
WGLEXTERN PFNGLGENSYMBOLSEXTPROC				glGenSymbolsEXT;
WGLEXTERN PFNGLSETINVARIANTEXTPROC				glSetInvariantEXT;
WGLEXTERN PFNGLSETLOCALCONSTANTEXTPROC			glSetLocalConstantEXT;
WGLEXTERN PFNGLVARIANTBVEXTPROC					glVariantbvEXT;
WGLEXTERN PFNGLVARIANTSVEXTPROC					glVariantsvEXT;
WGLEXTERN PFNGLVARIANTIVEXTPROC					glVariantivEXT;
WGLEXTERN PFNGLVARIANTFVEXTPROC					glVariantfvEXT;
WGLEXTERN PFNGLVARIANTDVEXTPROC					glVariantdvEXT;
WGLEXTERN PFNGLVARIANTUBVEXTPROC				glVariantubvEXT;
WGLEXTERN PFNGLVARIANTUSVEXTPROC				glVariantusvEXT;
WGLEXTERN PFNGLVARIANTUIVEXTPROC				glVariantuivEXT;
WGLEXTERN PFNGLVARIANTPOINTEREXTPROC			glVariantPointerEXT;
WGLEXTERN PFNGLENABLEVARIANTCLIENTSTATEEXTPROC	glEnableVariantClientStateEXT;
WGLEXTERN PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC	glDisableVariantClientStateEXT;
WGLEXTERN PFNGLBINDLIGHTPARAMETEREXTPROC		glBindLightParameterEXT;
WGLEXTERN PFNGLBINDMATERIALPARAMETEREXTPROC		glBindMaterialParameterEXT;
WGLEXTERN PFNGLBINDTEXGENPARAMETEREXTPROC		glBindTexGenParameterEXT;
WGLEXTERN PFNGLBINDTEXTUREUNITPARAMETEREXTPROC	glBindTextureUnitParameterEXT;
WGLEXTERN PFNGLBINDPARAMETEREXTPROC				glBindParameterEXT;
WGLEXTERN PFNGLISVARIANTENABLEDEXTPROC			glIsVariantEnabledEXT;
WGLEXTERN PFNGLGETVARIANTBOOLEANVEXTPROC		glGetVariantBooleanvEXT;
WGLEXTERN PFNGLGETVARIANTINTEGERVEXTPROC		glGetVariantIntegervEXT;
WGLEXTERN PFNGLGETVARIANTFLOATVEXTPROC			glGetVariantFloatvEXT;
WGLEXTERN PFNGLGETVARIANTPOINTERVEXTPROC		glGetVariantPointervEXT;
WGLEXTERN PFNGLGETINVARIANTBOOLEANVEXTPROC		glGetInvariantBooleanvEXT;
WGLEXTERN PFNGLGETINVARIANTINTEGERVEXTPROC		glGetInvariantIntegervEXT;
WGLEXTERN PFNGLGETINVARIANTFLOATVEXTPROC		glGetInvariantFloatvEXT;
WGLEXTERN PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC	glGetLocalConstantBooleanvEXT;
WGLEXTERN PFNGLGETLOCALCONSTANTINTEGERVEXTPROC	glGetLocalConstantIntegervEXT;
WGLEXTERN PFNGLGETLOCALCONSTANTFLOATVEXTPROC	glGetLocalConstantFloatvEXT;

#endif /* GL_EXT_vertex_shader */


/*
** GL_EXT_point_parameters
**
**	Support:
**	 Rage 128 *	based :
**	 Radeon	  *	based :
**	 R200	  *	based :
*/

#ifndef	GL_EXT_point_parameters
#define	GL_EXT_point_parameters	1

typedef	void (APIENTRY * PFNGLPOINTPARAMETERFEXTPROC) (GLenum pname, GLfloat param);
typedef	void (APIENTRY * PFNGLPOINTPARAMETERFVEXTPROC) (GLenum pname, const	GLfloat	*params);
typedef	void (APIENTRY * PFNGLPOINTPARAMETERFSGISPROC) (GLenum pname, GLfloat param);
typedef	void (APIENTRY * PFNGLPOINTPARAMETERFVSGISPROC)	(GLenum	pname, const GLfloat *params);

WGLEXTERN PFNGLPOINTPARAMETERFEXTPROC glPointParameterfEXT;
WGLEXTERN PFNGLPOINTPARAMETERFVEXTPROC glPointParameterfvEXT;
WGLEXTERN PFNGLPOINTPARAMETERFSGISPROC glPointParameterfSGIS;
WGLEXTERN PFNGLPOINTPARAMETERFVSGISPROC	glPointParameterfvSGIS;

#endif

/*
** GL_EXT_texture3D
**
**	Support:
**	 Rage 128 *	based :
**	 Radeon	  *	based :
**	 R200	  *	based :
*/
#ifndef	GL_EXT_texture3D
#define	GL_EXT_texture3D 1

typedef	GLvoid (APIENTRY * PFNGLTEXIMAGE3DEXT)(GLenum target,
			   GLint level,
			   GLenum internalformat,
			   GLsizei width,
			   GLsizei height,
			   GLsizei depth,
			   GLint border,
			   GLenum format,
			   GLenum type,
			   const GLvoid* pixels);

#define	GL_PACK_SKIP_IMAGES_EXT					0x806B
#define	GL_PACK_IMAGE_HEIGHT_EXT				0x806C
#define	GL_UNPACK_SKIP_IMAGES_EXT				0x806D
#define	GL_UNPACK_IMAGE_HEIGHT_EXT				0x806E
#define	GL_TEXTURE_3D_EXT						0x806F
#define	GL_PROXY_TEXTURE_3D_EXT					0x8070
#define	GL_TEXTURE_DEPTH_EXT					0x8071
#define	GL_TEXTURE_WRAP_R_EXT					0x8072
#define	GL_MAX_3D_TEXTURE_SIZE_EXT				0x8073

WGLEXTERN PFNGLTEXIMAGE3DEXT glTexImage3DEXT;

#endif // GL_EXT_texture3D

/*********************************************************************************************************
**	SGIS EXTENSIONS
*********************************************************************************************************/

#ifndef	GL_SGIS_generate_mipmap
#define	GL_SGIS_generate_mipmap	1

#define	GL_GENERATE_MIPMAP_SGIS									0x8191
#define	GL_GENERATE_MIPMAP_HINT_SGIS							0x8192

#endif

#ifndef	GL_SGIS_texture_lod
#define	GL_SGIS_texture_lod	 1

#define	GL_TEXTURE_MIN_LOD_SGIS									0x813A
#define	GL_TEXTURE_MAX_LOD_SGIS									0x813B
#define	GL_TEXTURE_BASE_LEVEL_SGIS								0x813C
#define	GL_TEXTURE_MAX_LEVEL_SGIS								0x813D

#endif
#ifndef	GL_SGIS_texture_border_clamp
#define	GL_SGIS_texture_border_clamp							1

#define	GL_CLAMP_TO_BORDER_SGIS									0x812D
#endif

#ifndef	GL_SGIX_shadow
#define	GL_SGIX_shadow 1

#define	GL_TEXTURE_COMPARE_SGIX									0x819A
#define	GL_TEXTURE_COMPARE_OPERATOR_SGIX						0x819B
#define	GL_TEXTURE_LEQUAL_R_SGIX								0x819C
#define	GL_TEXTURE_GEQUAL_R_SGIX								0x819D

#endif /* GL_SGIX_shadow */

#ifndef	GL_SGIX_depth_texture
#define	GL_SGIX_depth_texture 1

#define	GL_DEPTH_COMPONENT16_SGIX								0x81A5
#define	GL_DEPTH_COMPONENT24_SGIX								0x81A6
#define	GL_DEPTH_COMPONENT32_SGIX								0x81A7

#endif /* GL_SGIX_depth_texture	*/

/*********************************************************************************************************
**	ARB	EXTENSIONS
*********************************************************************************************************/

#ifndef	GL_ARB_multisample
#define	GL_ARB_multisample	1

typedef	GLvoid (APIENTRY * PFNGLSAMPLECOVERAGEARBPROC) (GLclampf value,	GLboolean invert);

#define	WGL_SAMPLE_BUFFERS_ARB				 0x2041
#define	WGL_SAMPLES_ARB						0x2042

#define	GL_MULTISAMPLE_ARB					0x809D
#define	GL_SAMPLE_ALPHA_TO_COVERAGE_ARB		0x809E
#define	GL_SAMPLE_ALPHA_TO_ONE_ARB			0x809F
#define	GL_SAMPLE_COVERAGE_ARB				0x80A0

#define	GL_MULTISAMPLE_BIT_ARB				0x20000000

#define	GL_SAMPLE_BUFFERS_ARB				0x80A8
#define	GL_SAMPLES_ARB						0x80A9
#define	GL_SAMPLE_COVERAGE_VALUE_ARB		0x80AA
#define	GL_SAMPLE_COVERAGE_INVERT_ARB		0x80AB

#endif

WGLEXTERN PFNGLSAMPLECOVERAGEARBPROC	glSampleCoverageARB;

/*
** GL_ARB_texture_env_dot3
**
**	Support:
**	 Rage 128 *	based :	Supported
**	 Radeon	  *	based :	Supported
*/

#ifndef	GL_ARB_texture_env_dot3
#define	GL_ARB_texture_env_dot3	1

#define	GL_DOT3_RGB_ARB							0x86AE
#define	GL_DOT3_RGBA_ARB						0x86AF
#endif

/*
** GL_ARB_transpose_matrix
**
**	Support:
**	 Rage 128 *	based :	Supported
**	 Radeon	  *	based :	Supported
*/
#ifndef	GL_ARB_transpose_matrix
#define	GL_ARB_transpose_matrix	1

typedef	GLvoid (APIENTRY *PFNGLLOADTRANSPOSEMATRIXFARB)	(float m[16]);
typedef	GLvoid (APIENTRY *PFNGLMULTTRANSPOSEMATRIXFARB)	(float m[16]);

#define	GL_TRANSPOSE_MODELVIEW_MATRIX_ARB	0x84E3
#define	GL_TRANSPOSE_PROJECTION_MATRIX_ARB	0x84E4
#define	GL_TRANSPOSE_TEXTURE_MATRIX_ARB		0x84E5
#define	GL_TRANSPOSE_COLOR_MATRIX_ARB		0x84E6

#endif

WGLEXTERN	PFNGLLOADTRANSPOSEMATRIXFARB		glLoadTransposeMatrixfARB;
WGLEXTERN	PFNGLMULTTRANSPOSEMATRIXFARB		glMultTransposeMatrixfARB;

/*
** GL_ARB_multitexture
**
**	Support:
**	 Rage 128 *	based :	Supported
**	 Radeon	  *	based :	Supported
*/

#ifndef	GL_ARB_multitexture
#define	GL_ARB_multitexture	1
#define	GL_ACTIVE_TEXTURE_ARB				0x84E0
#define	GL_CLIENT_ACTIVE_TEXTURE_ARB		0x84E1
#define	GL_MAX_TEXTURE_UNITS_ARB			0x84E2
#define	GL_TEXTURE0_ARB						0x84C0
#define	GL_TEXTURE1_ARB						0x84C1
#define	GL_TEXTURE2_ARB						0x84C2
#define	GL_TEXTURE3_ARB						0x84C3
#define	GL_TEXTURE4_ARB						0x84C4
#define	GL_TEXTURE5_ARB						0x84C5
#define	GL_TEXTURE6_ARB						0x84C6
#define	GL_TEXTURE7_ARB						0x84C7
#define	GL_TEXTURE8_ARB						0x84C8
#define	GL_TEXTURE9_ARB						0x84C9
#define	GL_TEXTURE10_ARB					0x84CA
#define	GL_TEXTURE11_ARB					0x84CB
#define	GL_TEXTURE12_ARB					0x84CC
#define	GL_TEXTURE13_ARB					0x84CD
#define	GL_TEXTURE14_ARB					0x84CE
#define	GL_TEXTURE15_ARB					0x84CF
#define	GL_TEXTURE16_ARB					0x84D0
#define	GL_TEXTURE17_ARB					0x84D1
#define	GL_TEXTURE18_ARB					0x84D2
#define	GL_TEXTURE19_ARB					0x84D3
#define	GL_TEXTURE20_ARB					0x84D4
#define	GL_TEXTURE21_ARB					0x84D5
#define	GL_TEXTURE22_ARB					0x84D6
#define	GL_TEXTURE23_ARB					0x84D7
#define	GL_TEXTURE24_ARB					0x84D8
#define	GL_TEXTURE25_ARB					0x84D9
#define	GL_TEXTURE26_ARB					0x84DA
#define	GL_TEXTURE27_ARB					0x84DB
#define	GL_TEXTURE28_ARB					0x84DC
#define	GL_TEXTURE29_ARB					0x84DD
#define	GL_TEXTURE30_ARB					0x84DE
#define	GL_TEXTURE31_ARB					0x84DF

typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD1DARBPROC)	(GLenum	target,	GLdouble s);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD1DVARBPROC) (GLenum target, const GLdouble	*v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD1FARBPROC)	(GLenum	target,	GLfloat	s);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD1FVARBPROC) (GLenum target, const GLfloat *v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD1IARBPROC)	(GLenum	target,	GLint s);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD1IVARBPROC) (GLenum target, const GLint *v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD1SARBPROC)	(GLenum	target,	GLshort	s);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD1SVARBPROC) (GLenum target, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD2DARBPROC)	(GLenum	target,	GLdouble s,	GLdouble t);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD2DVARBPROC) (GLenum target, const GLdouble	*v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC)	(GLenum	target,	GLfloat	s, GLfloat t);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD2FVARBPROC) (GLenum target, const GLfloat *v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD2IARBPROC)	(GLenum	target,	GLint s, GLint t);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD2IVARBPROC) (GLenum target, const GLint *v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD2SARBPROC)	(GLenum	target,	GLshort	s, GLshort t);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD2SVARBPROC) (GLenum target, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD3DARBPROC)	(GLenum	target,	GLdouble s,	GLdouble t,	GLdouble r);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD3DVARBPROC) (GLenum target, const GLdouble	*v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD3FARBPROC)	(GLenum	target,	GLfloat	s, GLfloat t, GLfloat r);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD3FVARBPROC) (GLenum target, const GLfloat *v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD3IARBPROC)	(GLenum	target,	GLint s, GLint t, GLint	r);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD3IVARBPROC) (GLenum target, const GLint *v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD3SARBPROC)	(GLenum	target,	GLshort	s, GLshort t, GLshort r);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD3SVARBPROC) (GLenum target, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD4DARBPROC)	(GLenum	target,	GLdouble s,	GLdouble t,	GLdouble r,	GLdouble q);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD4DVARBPROC) (GLenum target, const GLdouble	*v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD4FARBPROC)	(GLenum	target,	GLfloat	s, GLfloat t, GLfloat r, GLfloat q);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD4FVARBPROC) (GLenum target, const GLfloat *v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD4IARBPROC)	(GLenum	target,	GLint s, GLint t, GLint	r, GLint q);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD4IVARBPROC) (GLenum target, const GLint *v);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD4SARBPROC)	(GLenum	target,	GLshort	s, GLshort t, GLshort r, GLshort q);
typedef	GLvoid (APIENTRY * PFNGLMULTITEXCOORD4SVARBPROC) (GLenum target, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);
typedef	GLvoid (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC)	(GLenum	target);

#endif

WGLEXTERN PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTextureARB;
WGLEXTERN PFNGLACTIVETEXTUREARBPROC				glActiveTextureARB;
WGLEXTERN PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB;
WGLEXTERN PFNGLMULTITEXCOORD4FARBPROC			glMultiTexCoord4fARB;
WGLEXTERN PFNGLMULTITEXCOORD2FVARBPROC			glMultiTexCoord2fvARB;
WGLEXTERN PFNGLMULTITEXCOORD4FVARBPROC			glMultiTexCoord4fvARB;

/*
** GL_ARB_compression
**
**	Support:
**	 Rage 128 *	based :	Supported
**	 Radeon	  *	based :	Supported
*/

#ifndef	GL_ARB_compression
#define	GL_ARB_compression 1

#define	GL_COMPRESSED_ALPHA_ARB					0x84E9
#define	GL_COMPRESSED_LUMINANCE_ARB				0x84EA
#define	GL_COMPRESSED_LUMINANCE_ALPHA_ARB		0x84EB
#define	GL_COMPRESSED_INTENSITY_ARB				0x84EC
#define	GL_COMPRESSED_RGB_ARB					0x84ED
#define	GL_COMPRESSED_RGBA_ARB					0x84EE
#define	GL_TEXTURE_COMPRESSION_HINT_ARB			0x84EF
#define	GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB	0x86A0
#define	GL_TEXTURE_COMPRESSED_ARB				0x86A1

#define	GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB	0x86A2
#define	GL_COMPRESSED_TEXTURE_FORMATS_ARB		0x86A3

typedef	GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE3DARB)(GLenum	target,	GLint level,
			GLint internalformat, GLsizei width, GLsizei height, GLsizei depth,	GLint border, GLsizei imageSize, const void	*data);
typedef	GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DARB)(GLenum	target,	GLint level,
			GLint internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const	void *data);
typedef	GLvoid (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE1DARB)(GLenum	target,	GLint level,
			GLint internalformat, GLsizei width, GLint border, GLsizei imageSize, const	void *data);
typedef	GLvoid (APIENTRY * PFNGLGETCOMPRESSEDTEXIMAGEARB)(GLenum target, GLint lod,	const void *img);

WGLEXTERN PFNGLCOMPRESSEDTEXIMAGE1DARB			glCompressedTexImage1DARB;
WGLEXTERN PFNGLCOMPRESSEDTEXIMAGE2DARB			glCompressedTexImage2DARB;
WGLEXTERN PFNGLCOMPRESSEDTEXIMAGE3DARB			glCompressedTexImage3DARB;
WGLEXTERN PFNGLGETCOMPRESSEDTEXIMAGEARB			glGetCompressedTexImageARB;

#endif //


/*
** GL_ARB_vertex_blend
**
**	Support:
**	 Rage 128 *	based :	Supported
**	 Radeon	  *	based :	Supported
*/

#ifndef	GL_ARB_vertex_blend
#define	GL_ARB_vertex_blend					1

#define	GL_MAX_VERTEX_UNITS_ARB				0x86A4
#define	GL_ACTIVE_VERTEX_UNITS_ARB			0x86A5
#define	GL_WEIGHT_SUM_UNITY_ARB				0x86A6
#define	GL_VERTEX_BLEND_ARB					0x86A7
#define	GL_CURRENT_WEIGHT_ARB				0x86A8
#define	GL_WEIGHT_ARRAY_TYPE_ARB			0x86A9
#define	GL_WEIGHT_ARRAY_STRIDE_ARB			0x86AA
#define	GL_WEIGHT_ARRAY_SIZE_ARB			0x86AB
#define	GL_WEIGHT_ARRAY_POINTER_ARB			0x86AC
#define	GL_WEIGHT_ARRAY_ARB					0x86AD
#define	GL_MODELVIEW0_ARB					0x1700
#define	GL_MODELVIEW1_ARB					0x850a
#define	GL_MODELVIEW2_ARB					0x8722
#define	GL_MODELVIEW3_ARB					0x8723
#define	GL_MODELVIEW4_ARB					0x8724
#define	GL_MODELVIEW5_ARB					0x8725
#define	GL_MODELVIEW6_ARB					0x8726
#define	GL_MODELVIEW7_ARB					0x8727
#define	GL_MODELVIEW8_ARB					0x8728
#define	GL_MODELVIEW9_ARB					0x8729
#define	GL_MODELVIEW10_ARB					0x872A
#define	GL_MODELVIEW11_ARB					0x872B
#define	GL_MODELVIEW12_ARB					0x872C
#define	GL_MODELVIEW13_ARB					0x872D
#define	GL_MODELVIEW14_ARB					0x872E
#define	GL_MODELVIEW15_ARB					0x872F
#define	GL_MODELVIEW16_ARB					0x8730
#define	GL_MODELVIEW17_ARB					0x8731
#define	GL_MODELVIEW18_ARB					0x8732
#define	GL_MODELVIEW19_ARB					0x8733
#define	GL_MODELVIEW20_ARB					0x8734
#define	GL_MODELVIEW21_ARB					0x8735
#define	GL_MODELVIEW22_ARB					0x8736
#define	GL_MODELVIEW23_ARB					0x8737
#define	GL_MODELVIEW24_ARB					0x8738
#define	GL_MODELVIEW25_ARB					0x8739
#define	GL_MODELVIEW26_ARB					0x873A
#define	GL_MODELVIEW27_ARB					0x873B
#define	GL_MODELVIEW28_ARB					0x873C
#define	GL_MODELVIEW29_ARB					0x873D
#define	GL_MODELVIEW30_ARB					0x873E
#define	GL_MODELVIEW31_ARB					0x873F

typedef	GLvoid (APIENTRY *PFNGLWEIGHTBVARBPROC)(GLint size,	GLbyte *weights);
typedef	GLvoid (APIENTRY *PFNGLWEIGHTSVARBPROC)(GLint size,	GLshort	*weights);
typedef	GLvoid (APIENTRY *PFNGLWEIGHTIVARBPROC)(GLint size,	GLint *weights);
typedef	GLvoid (APIENTRY *PFNGLWEIGHTFVARBPROC)(GLint size,	GLfloat	*weights);
typedef	GLvoid (APIENTRY *PFNGLWEIGHTDVARBPROC)(GLint size,	GLdouble *weights);
typedef	GLvoid (APIENTRY *PFNGLWEIGHTUBVARBPROC)(GLint size, GLubyte *weights);
typedef	GLvoid (APIENTRY *PFNGLWEIGHTUSVARBPROC)(GLint size, GLushort *weights);
typedef	GLvoid (APIENTRY *PFNGLWEIGHTUIVARBPROC)(GLint size, GLuint	*weights);
typedef	GLvoid (APIENTRY *PFNGLWEIGHTPOINTERARBPROC)(GLint size, GLenum	type, GLsizei stride, GLvoid *pointer);
typedef	GLvoid (APIENTRY *PFNGLVERTEXBLENDARBPROC)(GLint count);

WGLEXTERN PFNGLWEIGHTBVARBPROC				glWeightbvARB;
WGLEXTERN PFNGLWEIGHTSVARBPROC				glWeightsvARB;
WGLEXTERN PFNGLWEIGHTIVARBPROC				glWeightivARB;
WGLEXTERN PFNGLWEIGHTFVARBPROC				glWeightfvARB;
WGLEXTERN PFNGLWEIGHTDVARBPROC				glWeightdvARB;
WGLEXTERN PFNGLWEIGHTUBVARBPROC				glWeightubvARB;
WGLEXTERN PFNGLWEIGHTUSVARBPROC				glWeightusvARB;
WGLEXTERN PFNGLWEIGHTUIVARBPROC				glWeightuivARB;
WGLEXTERN PFNGLWEIGHTPOINTERARBPROC			glWeightPointerARB;
WGLEXTERN PFNGLVERTEXBLENDARBPROC			glVertexBlendARB;

#endif /* GL_ARB_vertex_blend */


/*
** GL_ARB_window_pos
**
**	Support:
**	 Rage 128  * based : Supported
**	 Radeon	   * based : Supported
**	 Chaplin   * based : Supported
*/
#ifndef	GL_ARB_window_pos
#define	GL_ARB_window_pos						  1

typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS2DARBPROC)	(GLdouble x, GLdouble y);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS2FARBPROC)	(GLfloat x,	GLfloat	y);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS2IARBPROC)	(GLint x, GLint	y);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS2SARBPROC)	(GLshort x,	GLshort	y);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS2IVARBPROC) (const	GLint *p);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS2SVARBPROC) (const	GLshort	*p);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS2FVARBPROC) (const	GLfloat	*p);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS2DVARBPROC) (const	GLdouble *p);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS3IARBPROC)	(GLint x, GLint	y, GLint z);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS3SARBPROC)	(GLshort x,	GLshort	y, GLshort z);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS3FARBPROC)	(GLfloat x,	GLfloat	y, GLfloat z);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS3DARBPROC)	(GLdouble x, GLdouble y, GLdouble z);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS3IVARBPROC) (const	GLint *p);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS3SVARBPROC) (const	GLshort	*p);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS3FVARBPROC) (const	GLfloat	*p);
typedef	GLvoid (APIENTRY * PFNGLWINDOWPOS3DVARBPROC) (const	GLdouble *p);

#endif /* GL_ARB_window_pos	*/


/*
** GL_ARB_texture_cube_map
**
**	Support:
**	 Geforce * based : Supported
**	 Rage 128 *	based :	Supported
**	 Radeon	  *	based :	Supported
*/

#ifndef	GL_ARB_texture_cube_map
#define	GL_ARB_texture_cube_map					1

#define	GL_NORMAL_MAP_ARB						0x8511
#define	GL_REFLECTION_MAP_ARB					0x8512
#define	GL_TEXTURE_CUBE_MAP_ARB					0x8513
#define	GL_TEXTURE_BINDING_CUBE_MAP_ARB			0x8514
#define	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB		0x8515
#define	GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB		0x8516
#define	GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB		0x8517
#define	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB		0x8518
#define	GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB		0x8519
#define	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB		0x851A
#define	GL_PROXY_TEXTURE_CUBE_MAP_ARB			0x851B
#define	GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB		0x851C

#endif // GL_ARB_texture_cube_map


/*
** GL_ARB_texture_env_combine
**
**	Support:
**	 Geforce * based : Supported
**	 Rage 128 *	based :	Supported
**	 Radeon	  *	based :	Supported
*/

#ifndef	GL_ARB_texture_env_combine
#define	GL_ARB_texture_env_combine 1

#define	GL_COMBINE_ARB							0x8570
#define	GL_COMBINE_RGB_ARB						0x8571
#define	GL_COMBINE_ALPHA_ARB					0x8572
#define	GL_RGB_SCALE_ARB						0x8573
#define	GL_ADD_SIGNED_ARB						0x8574
#define	GL_INTERPOLATE_ARB						0x8575
#define	GL_CONSTANT_ARB							0x8576
#define	GL_SUBTRACT_ARB							0x84E7
#define	GL_PRIMARY_COLOR_ARB					0x8577
#define	GL_PREVIOUS_ARB							0x8578
#define	GL_SOURCE0_RGB_ARB						0x8580
#define	GL_SOURCE1_RGB_ARB						0x8581
#define	GL_SOURCE2_RGB_ARB						0x8582
#define	GL_SOURCE0_ALPHA_ARB					0x8588
#define	GL_SOURCE1_ALPHA_ARB					0x8589
#define	GL_SOURCE2_ALPHA_ARB					0x858A
#define	GL_OPERAND0_RGB_ARB						0x8590
#define	GL_OPERAND1_RGB_ARB						0x8591
#define	GL_OPERAND2_RGB_ARB						0x8592
#define	GL_OPERAND0_ALPHA_ARB					0x8598
#define	GL_OPERAND1_ALPHA_ARB					0x8599
#define	GL_OPERAND2_ALPHA_ARB					0x859A

// GL_TEXTURE0_ARB is accepted if GL_ARB_texture_env_crossbar available

#endif // GL_ARB_texture_env_combine


/*
** GL_ARB_vertex_program
**
**	Support:
**	 None
**
*/

#ifndef	GL_ARB_vertex_program
#define	GL_ARB_vertex_program	1

typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB1SARB)(GLuint index, GLshort x);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB1FARB)(GLuint index, GLfloat x);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB1DARB)(GLuint index, GLdouble x);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB2SARB)(GLuint index, GLshort x,	GLshort	y);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB2FARB)(GLuint index, GLfloat x,	GLfloat	y);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB2DARB)(GLuint index, GLdouble x, GLdouble y);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB3SARB)(GLuint index, GLshort x,	GLshort	y, GLshort z);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB3FARB)(GLuint index, GLfloat x,	GLfloat	y, GLfloat z);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB3DARB)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4SARB)(GLuint index, GLshort x,	GLshort	y, GLshort z, GLshort w);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4FARB)(GLuint index, GLfloat x,	GLfloat	y, GLfloat z, GLfloat w);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4DARB)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4NUBARB)(GLuint	index, GLubyte x, GLubyte y, GLubyte z,	GLubyte	w);

typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB1SVARB)(GLuint index, const	GLshort	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB1FVARB)(GLuint index, const	GLfloat	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB1DVARB)(GLuint index, const	GLdouble *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB2SVARB)(GLuint index, const	GLshort	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB2FVARB)(GLuint index, const	GLfloat	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB2DVARB)(GLuint index, const	GLdouble *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB3SVARB)(GLuint index, const	GLshort	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB3FVARB)(GLuint index, const	GLfloat	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB3DVARB)(GLuint index, const	GLdouble *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4BVARB)(GLuint index, const	GLbyte *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4SVARB)(GLuint index, const	GLshort	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4IVARB)(GLuint index, const	GLint *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4UBVARB)(GLuint	index, const GLubyte *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4USVARB)(GLuint	index, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4UIVARB)(GLuint	index, const GLuint	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4FVARB)(GLuint index, const	GLfloat	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4DVARB)(GLuint index, const	GLdouble *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4NBVARB)(GLuint	index, const GLbyte	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4NSVARB)(GLuint	index, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4NIVARB)(GLuint	index, const GLint *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4NUBVARB)(GLuint index,	const GLubyte *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4NUSVARB)(GLuint index,	const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4NUIVARB)(GLuint index,	const GLuint *v);

typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBPOINTERARB)(GLuint index, GLint	size, GLenum type,
								GLboolean normalized, GLsizei stride,
								const GLvoid *pointer);

typedef	GLvoid (APIENTRY * PFNGLENABLEVERTEXATTRIBARRAYARB)(GLuint index);
typedef	GLvoid (APIENTRY * PFNGLDISABLEVERTEXATTRIBARRAYARB)(GLuint	index);

typedef	GLvoid (APIENTRY * PFNGLPROGRAMSTRINGARB)(GLenum target, GLenum	format,	GLsizei	len,
						  const	GLvoid *string);

typedef	GLvoid (APIENTRY * PFNGLBINDPROGRAMARB)(GLenum target, GLuint program);

typedef	GLvoid (APIENTRY * PFNGLDELETEPROGRAMSARB)(GLsizei n, const	GLuint *programs);

typedef	GLvoid (APIENTRY * PFNGLGENPROGRAMSARB)(GLsizei	n, GLuint *programs);

typedef	GLvoid (APIENTRY * PFNGLPROGRAMENVPARAMETER4DARB)(GLenum target, GLuint	index,
								  GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef	GLvoid (APIENTRY * PFNGLPROGRAMENVPARAMETER4DVARB)(GLenum target, GLuint index,
								   const GLdouble *params);
typedef	GLvoid (APIENTRY * PFNGLPROGRAMENVPARAMETER4FARB)(GLenum target, GLuint	index,
								  GLfloat x, GLfloat y,	GLfloat	z, GLfloat w);
typedef	GLvoid (APIENTRY * PFNGLPROGRAMENVPARAMETER4FVARB)(GLenum target, GLuint index,
								   const GLfloat *params);

typedef	GLvoid (APIENTRY * PFNGLPROGRAMLOCALPARAMETER4DARB)(GLenum target, GLuint index,
									GLdouble x,	GLdouble y,	GLdouble z,	GLdouble w);
typedef	GLvoid (APIENTRY * PFNGLPROGRAMLOCALPARAMETER4DVARB)(GLenum	target,	GLuint index,
									 const GLdouble	*params);
typedef	GLvoid (APIENTRY * PFNGLPROGRAMLOCALPARAMETER4FARB)(GLenum target, GLuint index,
									GLfloat	x, GLfloat y, GLfloat z, GLfloat w);
typedef	GLvoid (APIENTRY * PFNGLPROGRAMLOCALPARAMETER4FVARB)(GLenum	target,	GLuint index,
									 const GLfloat *params);

typedef	GLvoid (APIENTRY * PFNGLGETPROGRAMENVPARAMETERDVARB)(GLenum	target,	GLuint index,
									 GLdouble *params);
typedef	GLvoid (APIENTRY * PFNGLGETPROGRAMENVPARAMETERFVARB)(GLenum	target,	GLuint index,
									 GLfloat *params);

typedef	GLvoid (APIENTRY * PFNGLGETPROGRAMLOCALPARAMETERDVARB)(GLenum target, GLuint index,
									   GLdouble	*params);
typedef	GLvoid (APIENTRY * PFNGLGETPROGRAMLOCALPARAMETERFVARB)(GLenum target, GLuint index,
									   GLfloat *params);

typedef	GLvoid (APIENTRY * PFNGLGETPROGRAMIVARB)(GLenum	target,	GLenum pname, GLint	*params);

typedef	GLvoid (APIENTRY * PFNGLGETPROGRAMSTRINGARB)(GLenum	target,	GLenum pname, GLvoid *string);

typedef	GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBDVARB)(GLuint index,	GLenum pname, GLdouble *params);
typedef	GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBFVARB)(GLuint index,	GLenum pname, GLfloat *params);
typedef	GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBIVARB)(GLuint index,	GLenum pname, GLint	*params);

typedef	GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBPOINTERVARB)(GLuint index, GLenum pname,	GLvoid **pointer);

typedef	GLboolean (APIENTRY	* PFNGLISPROGRAMARB)(GLuint	program);


/*
New	Tokens

	Accepted by	the	<cap> parameter	of Disable,	Enable,	and	IsEnabled, by the
	<pname>	parameter of GetBooleanv, GetIntegerv, GetFloatv, and GetDoublev,
	and	by the <target>	parameter of ProgramStringARB, BindProgramARB,
	ProgramEnvParameter4[df][v]ARB,	ProgramLocalParameter4[df][v]ARB,
	GetProgramEnvParameter[df]vARB,	GetProgramLocalParameter[df]vARB,
	GetProgramivARB, and GetProgramStringARB.

  */
#define	GL_VERTEX_PROGRAM_ARB							   0x8620

/*
	Accepted by	the	<cap> parameter	of Disable,	Enable,	and	IsEnabled, and by
	the	<pname>	parameter of GetBooleanv, GetIntegerv, GetFloatv, and
	GetDoublev:
*/

#define	GL_VERTEX_PROGRAM_POINT_SIZE_ARB				   0x8642
#define	GL_VERTEX_PROGRAM_TWO_SIDE_ARB					   0x8643
#define	GL_COLOR_SUM_ARB								   0x8458
/*
	Accepted by	the	<format> parameter of ProgramStringARB:
*/
#define	GL_PROGRAM_FORMAT_ASCII_ARB						   0x8875
/*
	Accepted by	the	<pname>	parameter of GetVertexAttrib[dfi]vARB:
*/
#define	GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB				   0x8622
#define	GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB					   0x8623
#define	GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB				   0x8624
#define	GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB					   0x8625
#define	GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB			   0x886A
#define	GL_CURRENT_VERTEX_ATTRIB_ARB					   0x8626
/*
	Accepted by	the	<pname>	parameter of GetVertexAttribPointervARB:
*/
#define	GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB				   0x8645
/*
	Accepted by	the	<pname>	parameter of GetProgramivARB:
*/
#define	GL_PROGRAM_LENGTH_ARB							   0x8627
#define	GL_PROGRAM_FORMAT_ARB							   0x8876
#define	GL_PROGRAM_BINDING_ARB							   0x8677
#define	GL_PROGRAM_INSTRUCTIONS_ARB						   0x88A0
#define	GL_MAX_PROGRAM_INSTRUCTIONS_ARB					   0x88A1
#define	GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB				   0x88A2
#define	GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB			   0x88A3
#define	GL_PROGRAM_TEMPORARIES_ARB						   0x88A4
#define	GL_MAX_PROGRAM_TEMPORARIES_ARB					   0x88A5
#define	GL_PROGRAM_NATIVE_TEMPORARIES_ARB				   0x88A6
#define	GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB			   0x88A7
#define	GL_PROGRAM_PARAMETERS_ARB						   0x88A8
#define	GL_MAX_PROGRAM_PARAMETERS_ARB					   0x88A9
#define	GL_PROGRAM_NATIVE_PARAMETERS_ARB				   0x88AA
#define	GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB			   0x88AB
#define	GL_PROGRAM_ATTRIBS_ARB							   0x88AC
#define	GL_MAX_PROGRAM_ATTRIBS_ARB						   0x88AD
#define	GL_PROGRAM_NATIVE_ATTRIBS_ARB					   0x88AE
#define	GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB				   0x88AF
#define	GL_PROGRAM_ADDRESS_REGISTERS_ARB				   0x88B0
#define	GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB			   0x88B1
#define	GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB			   0x88B2
#define	GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB		   0x88B3
#define	GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB				   0x88B4
#define	GL_MAX_PROGRAM_ENV_PARAMETERS_ARB				   0x88B5
#define	GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB				   0x88B6
/*
	Accepted by	the	<pname>	parameter of GetProgramStringARB:
*/
#define	GL_PROGRAM_STRING_ARB							   0x8628
/*
	Accepted by	the	<pname>	parameter of GetBooleanv, GetIntegerv,
	GetFloatv, and GetDoublev:
*/
#define	GL_PROGRAM_ERROR_POSITION_ARB					   0x864B
#define	GL_CURRENT_MATRIX_ARB							   0x8641
#define	GL_TRANSPOSE_CURRENT_MATRIX_ARB					   0x88B7
#define	GL_CURRENT_MATRIX_STACK_DEPTH_ARB				   0x8640
#define	GL_MAX_VERTEX_ATTRIBS_ARB						   0x8869
#define	GL_MAX_PROGRAM_MATRICES_ARB						   0x862F
#define	GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB			   0x862E
/*
	Accepted by	the	<name> parameter of	GetString:
*/
#define	GL_PROGRAM_ERROR_STRING_ARB						   0x8874
/*
	Accepted by	the	<mode> parameter of	MatrixMode:
*/
#define	GL_MATRIX0_ARB									   0x88C0
#define	GL_MATRIX1_ARB									   0x88C1
#define	GL_MATRIX2_ARB									   0x88C2
#define	GL_MATRIX3_ARB									   0x88C3
#define	GL_MATRIX4_ARB									   0x88C4
#define	GL_MATRIX5_ARB									   0x88C5
#define	GL_MATRIX6_ARB									   0x88C6
#define	GL_MATRIX7_ARB									   0x88C7
#define	GL_MATRIX8_ARB									   0x88C8
#define	GL_MATRIX9_ARB									   0x88C9
#define	GL_MATRIX10_ARB									   0x88CA
#define	GL_MATRIX11_ARB									   0x88CB
#define	GL_MATRIX12_ARB									   0x88CC
#define	GL_MATRIX13_ARB									   0x88CD
#define	GL_MATRIX14_ARB									   0x88CE
#define	GL_MATRIX15_ARB									   0x88CF
#define	GL_MATRIX16_ARB									   0x88D0
#define	GL_MATRIX17_ARB									   0x88D1
#define	GL_MATRIX18_ARB									   0x88D2
#define	GL_MATRIX19_ARB									   0x88D3
#define	GL_MATRIX20_ARB									   0x88D4
#define	GL_MATRIX21_ARB									   0x88D5
#define	GL_MATRIX22_ARB									   0x88D6
#define	GL_MATRIX23_ARB									   0x88D7
#define	GL_MATRIX24_ARB									   0x88D8
#define	GL_MATRIX25_ARB									   0x88D9
#define	GL_MATRIX26_ARB									   0x88DA
#define	GL_MATRIX27_ARB									   0x88DB
#define	GL_MATRIX28_ARB									   0x88DC
#define	GL_MATRIX29_ARB									   0x88DD
#define	GL_MATRIX30_ARB									   0x88DE
#define	GL_MATRIX31_ARB									   0x88DF

WGLEXTERN PFNGLGETPROGRAMIVARB				glGetProgramivARB;

#endif


/*****************************************************************************************************
**	S3 EXTENSIONS
*****************************************************************************************************/
#ifndef	GL_S3_s3tc
#define	GL_S3_s3tc	1

#define	GL_RGB_S3TC								0x83A0
#define	GL_RGB4_S3TC							0x83A1
#define	GL_COMPRESSED_RGB_S3TC_DXT1_EXT			0x83F0
#define	GL_COMPRESSED_RGBA_S3TC_DXT1_EXT		0x83F1
#define	GL_COMPRESSED_RGBA_S3TC_DXT3_EXT		0x83F2
#define	GL_COMPRESSED_RGBA_S3TC_DXT5_EXT		0x83F3

#endif

/*****************************************************************************************************
**	3DFX EXTENSIONS
*****************************************************************************************************/

#ifndef	GL_3DFX_tbuffer
#define	GL_3DFX_tbuffer	1
typedef	GLvoid (APIENTRY * PGNGLTBUFFERMASK3DFX) (GLuint mask);
#endif
WGLEXTERN PGNGLTBUFFERMASK3DFX				glTbufferMask3DFX;

#ifndef	GL_3DFX_texture_compression_FXT1
#define	GL_3DFX_texture_compression_FXT1	1

#define	GL_COMPRESSED_RGBA_FXT1_3DFX		0x86B1
#define	GL_COMPRESSED_RGB_FXT1_3DFX			0x86B0
#endif

#ifndef	GL_3DFX_multisample
#define	GL_3DFX_multisample					1

#define	GLX_SAMPLE_BUFFERS_3DFX				0x8050
#define	GLX_SAMPLES_3DFX					0x8051
#define	WGL_SAMPLE_BUFFERS_3DFX				0x2060
#define	WGL_SAMPLES_3DFX					0x2061
#define	GL_MULTISAMPLE_3DFX					0x86B2
#define	GL_SAMPLE_BUFFERS_3DFX				0x86B3
#define	GL_SAMPLES_3DFX						0x86B4
#define	GL_MULTISAMPLE_BIT_3DFX				0x20000000

#endif


/*******************************************************************************************************
**	nVidia EXTENSIONS
********************************************************************************************************/

/*
** GL_NV_copy_depth_to_color
**
** Support:
**	 Geforce
*/

#ifndef	GL_NV_copy_depth_to_color
#define	GL_NV_copy_depth_to_color			1

#define GL_DEPTH_STENCIL_TO_RGBA_NV          0x886E
#define GL_DEPTH_STENCIL_TO_BGRA_NV          0x886F
#endif


/*
** GL_NV_depth_clamp
**
** Support:
**	 Geforce
*/

#ifndef	GL_NV_depth_clamp
#define	GL_NV_depth_clamp	1

#define	GL_DEPTH_CLAMP_NV					0x864F
#endif

/*
** GL_NV_texgen_emboss
**
** Support:
**	 Geforce
*/

#ifndef	GL_NV_texgen_emboss
#define	GL_NV_texgen_emboss		1

#define	GL_EMBOSS_MAP_NV					0x855F
#define	GL_EMBOSS_LIGHT_NV					0x855D
#define	GL_EMBOSS_CONSTANT_NV				0x855E
#endif

/*
** NV_texgen_reflection
**
** Support:
**	 Geforce
*/

#ifndef	GL_NV_texgen_reflection
#define	GL_NV_texgen_reflection	1

#define	GL_NORMAL_MAP_NV					0x8511
#define	GL_REFLECTION_MAP_NV				0x8512
#endif

/*
** NV_multisample_filter_hint
**
** Support:
**	 Geforce
*/

#ifndef	GL_NV_multisample_filter_hint
#define	GL_NV_multisample_filter_hint 1

#define	GL_MULTISAMPLE_FILTER_HINT_NV		0x8534

#endif


/*
** NV_float_buffer
**
** Support:
**	 Geforce
*/

#ifndef	GL_NV_float_buffer
#define	GL_NV_float_buffer 1

#define	GL_FLOAT_R_NV						 0x8880
#define	GL_FLOAT_RG_NV						 0x8881
#define	GL_FLOAT_RGB_NV						 0x8882
#define	GL_FLOAT_RGBA_NV					 0x8883
#define	GL_FLOAT_R32_NV						 0x8885
#define	GL_FLOAT_R16_NV						 0x8884
#define	GL_FLOAT_R32_NV						 0x8885
#define	GL_FLOAT_RG16_NV					 0x8886
#define	GL_FLOAT_RG32_NV					 0x8887
#define	GL_FLOAT_RGB16_NV					 0x8888
#define	GL_FLOAT_RGB32_NV					 0x8889
#define	GL_FLOAT_RGBA16_NV					 0x888A
#define	GL_FLOAT_RGBA32_NV					 0x888B
#define	GL_TEXTURE_FLOAT_COMPONENTS_NV		 0x888C
#define	GL_FLOAT_CLEAR_COLOR_VALUE_NV		 0x888D
#define	GL_FLOAT_RGBA_MODE_NV				 0x888E

#endif
/*
** NV_texture_env_combine4
**
** Support:
**	 Geforce
*/

#ifndef	GL_NV_texture_env_combine4
#define	GL_NV_texture_env_combine4	1

#define	GL_COMBINE4_NV						0x8503
#define	GL_SOURCE3_RGB_NV					0x8583
#define	GL_SOURCE3_ALPHA_NV					0x858B
#define	GL_OPERAND3_RGB_NV					0x8593
#define	GL_OPERAND3_ALPHA_NV				0x859B
#endif


/*
** GL_NV_light_max_exponent
**
** Support:
**	 Geforce
*/
#ifndef	GL_NV_light_max_exponent
#define	GL_NV_light_max_exponent	1

#define	GL_MAX_SHININESS_NV					 0x8504
#define	GL_MAX_SPOT_EXPONENT_NV				 0x8505

#endif


/*
** GL_NV_fog_distance
**
** Support:
**	 Geforce
*/
#ifndef	GL_NV_fog_distance
#define	GL_NV_fog_distance			1

#define	GL_FOG_DISTANCE_MODE_NV				 0x855A
#define	GL_EYE_RADIAL_NV					 0x855B
#define	GL_EYE_PLANE_ABSOLUTE_NV			 0x855C

#endif

/*
** GL_NV_packed_depth_stencil
**
** Support:
**	 Geforce
*/
#ifndef	GL_NV_packed_depth_stencil
#define	GL_NV_packed_depth_stencil	1

#define	GL_DEPTH_STENCIL_NV					 0x84F9
#define	GL_UNSIGNED_INT_24_8_NV				 0x84FA

#endif

/*
** GL_NV_texture_rectangle
**
** Support:
**	 Geforce
*/
#ifndef	GL_NV_texture_rectangle
#define	GL_NV_texture_rectangle		1

#define	GL_TEXTURE_RECTANGLE_NV				 0x84F5
#define	GL_TEXTURE_BINDING_RECTANGLE_NV		 0x84F6
#define	GL_PROXY_TEXTURE_RECTANGLE_NV		 0x84F7
#define	GL_MAX_RECTANGLE_TEXTURE_SIZE_NV	 0x84F8

#endif

/*
** GL_NV_vertex_array_range
**
** Support:
**	 Geforce
*/

#ifndef	GL_NV_vertex_array_range
#define	GL_NV_vertex_array_range 1

#define	GL_VERTEX_ARRAY_RANGE_NV				0x851D
#define	GL_VERTEX_ARRAY_RANGE_LENGTH_NV			0x851E
#define	GL_VERTEX_ARRAY_RANGE_VALID_NV			0x851F
#define	GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV	0x8520
#define	GL_VERTEX_ARRAY_RANGE_POINTER_NV		0x8521
#define	GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV	0x8533

typedef	GLvoid	* (APIENTRY	*PFNWGLALLOCATEMEMORYNVPROC) (GLint	size, float	readfreq, float	writefreq, float priority);
typedef	GLvoid	  (APIENTRY	*PFNWGLFREEMEMORYNVPROC) (void*	pointer);
typedef	GLvoid	  (APIENTRY	*PFNGLFLUSHVERTEXARRAYRANGENVPROC) (void);
typedef	GLvoid	  (APIENTRY	*PFNGLVERTEXARRAYRANGENVPROC) (GLsizei size, const GLvoid *pointer);

WGLEXTERN PFNGLFLUSHVERTEXARRAYRANGENVPROC	glFlushVertexArrayRangeNV;
WGLEXTERN PFNGLVERTEXARRAYRANGENVPROC		glVertexArrayRangeNV;
WGLEXTERN PFNWGLALLOCATEMEMORYNVPROC		wglAllocateMemoryNV;
WGLEXTERN PFNWGLFREEMEMORYNVPROC			wglFreeMemoryNV;
#endif // NV_vertex_array_range

/*
** GL_NV_fence
**
** Support:
**	 Geforce
*/

#ifndef	GL_NV_fence
#define	GL_NV_fence	1

#define	GL_ALL_COMPLETED_NV						 0x84F2
#define	GL_FENCE_STATUS_NV						 0x84F3
#define	GL_FENCE_CONDITION_NV					 0x84F4

typedef	GLvoid		(APIENTRY *	PFNGLDELETEFENCESNVPROC) (GLsizei n, const GLuint *fences);
typedef	GLvoid		(APIENTRY *	PFNGLGENFENCESNVPROC) (GLsizei n, GLuint *fences);
typedef	GLboolean	(APIENTRY *	PFNGLISFENCENVPROC)	(GLuint	fence);
typedef	GLboolean	(APIENTRY *	PFNGLTESTFENCENVPROC) (GLuint fence);
typedef	GLvoid		(APIENTRY *	PFNGLGETFENCEIVNVPROC) (GLuint fence, GLenum pname,	GLint *params);
typedef	GLvoid		(APIENTRY *	PFNGLFINISHFENCENVPROC)	(GLuint	fence);
typedef	GLvoid		(APIENTRY *	PFNGLSETFENCENVPROC) (GLuint fence,	GLenum condition);

WGLEXTERN PFNGLGENFENCESNVPROC				glGenFencesNV;
WGLEXTERN PFNGLDELETEFENCESNVPROC			glDeleteFencesNV;
WGLEXTERN PFNGLSETFENCENVPROC				glSetFenceNV;
WGLEXTERN PFNGLTESTFENCENVPROC				glTestFenceNV;
WGLEXTERN PFNGLFINISHFENCENVPROC			glFinishFenceNV;
WGLEXTERN PFNGLISFENCENVPROC				glIsFenceNV;
WGLEXTERN PFNGLGETFENCEIVNVPROC				glGetFenceivNV;

#endif // NV_fence

/*
**	GL_NV_register_combiners
**
** Support:
**	 Geforce
*/

#ifndef	GL_NV_register_combiners
#define	GL_NV_register_combiners 1

typedef	GLvoid (APIENTRY * PFNGLCOMBINERPARAMETERFVNVPROC) (GLenum pname, const	GLfloat	*params);
typedef	GLvoid (APIENTRY * PFNGLCOMBINERPARAMETERFNVPROC) (GLenum pname, GLfloat param);
typedef	GLvoid (APIENTRY * PFNGLCOMBINERPARAMETERIVNVPROC) (GLenum pname, const	GLint *params);
typedef	GLvoid (APIENTRY * PFNGLCOMBINERPARAMETERINVPROC) (GLenum pname, GLint param);
typedef	GLvoid (APIENTRY * PFNGLCOMBINERINPUTNVPROC) (GLenum stage,	GLenum portion,	GLenum variable, GLenum	input, GLenum mapping, GLenum componentUsage);
typedef	GLvoid (APIENTRY * PFNGLCOMBINEROUTPUTNVPROC) (GLenum stage, GLenum	portion, GLenum	abOutput, GLenum cdOutput, GLenum sumOutput, GLenum	scale, GLenum bias,	GLboolean abDotProduct,	GLboolean cdDotProduct,	GLboolean muxSum);
typedef	GLvoid (APIENTRY * PFNGLFINALCOMBINERINPUTNVPROC) (GLenum variable,	GLenum input, GLenum mapping, GLenum componentUsage);
typedef	GLvoid (APIENTRY * PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat *params);
typedef	GLvoid (APIENTRY * PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint *params);
typedef	GLvoid (APIENTRY * PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC)	(GLenum	stage, GLenum portion, GLenum pname, GLfloat *params);
typedef	GLvoid (APIENTRY * PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC)	(GLenum	stage, GLenum portion, GLenum pname, GLint *params);
typedef	GLvoid (APIENTRY * PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC)	(GLenum	variable, GLenum pname,	GLfloat	*params);
typedef	GLvoid (APIENTRY * PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC)	(GLenum	variable, GLenum pname,	GLint *params);

WGLEXTERN PFNGLCOMBINERPARAMETERFVNVPROC				glCombinerParameterfvNV;
WGLEXTERN PFNGLCOMBINERPARAMETERIVNVPROC				glCombinerParameterivNV;
WGLEXTERN PFNGLCOMBINERPARAMETERFNVPROC					glCombinerParameterfNV;
WGLEXTERN PFNGLCOMBINERPARAMETERINVPROC					glCombinerParameteriNV;
WGLEXTERN PFNGLCOMBINERINPUTNVPROC						glCombinerInputNV;
WGLEXTERN PFNGLCOMBINEROUTPUTNVPROC						glCombinerOutputNV;
WGLEXTERN PFNGLFINALCOMBINERINPUTNVPROC					glFinalCombinerInputNV;
WGLEXTERN PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC		glGetCombinerInputParameterfvNV;
WGLEXTERN PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC		glGetCombinerInputParameterivNV;
WGLEXTERN PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC		glGetCombinerOutputParameterfvNV;
WGLEXTERN PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC		glGetCombinerOutputParameterivNV;
WGLEXTERN PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC	glGetFinalCombinerInputParameterfvNV;
WGLEXTERN PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC	glGetFinalCombinerInputParameterivNV;

#define	GL_REGISTER_COMBINERS_NV			  0x8522
#define	GL_COMBINER0_NV						  0x8550
#define	GL_COMBINER1_NV						  0x8551
#define	GL_COMBINER2_NV						  0x8552
#define	GL_COMBINER3_NV						  0x8553
#define	GL_COMBINER4_NV						  0x8554
#define	GL_COMBINER5_NV						  0x8555
#define	GL_COMBINER6_NV						  0x8556
#define	GL_COMBINER7_NV						  0x8557
#define	GL_VARIABLE_A_NV					  0x8523
#define	GL_VARIABLE_B_NV					  0x8524
#define	GL_VARIABLE_C_NV					  0x8525
#define	GL_VARIABLE_D_NV					  0x8526
#define	GL_VARIABLE_E_NV					  0x8527
#define	GL_VARIABLE_F_NV					  0x8528
#define	GL_VARIABLE_G_NV					  0x8529
#define	GL_CONSTANT_COLOR0_NV				  0x852A
#define	GL_CONSTANT_COLOR1_NV				  0x852B
#define	GL_PRIMARY_COLOR_NV					  0x852C
#define	GL_SECONDARY_COLOR_NV				  0x852D
#define	GL_SPARE0_NV						  0x852E
#define	GL_SPARE1_NV						  0x852F
#define	GL_UNSIGNED_IDENTITY_NV				  0x8536
#define	GL_UNSIGNED_INVERT_NV				  0x8537
#define	GL_EXPAND_NORMAL_NV					  0x8538
#define	GL_EXPAND_NEGATE_NV					  0x8539
#define	GL_HALF_BIAS_NORMAL_NV				  0x853A
#define	GL_HALF_BIAS_NEGATE_NV				  0x853B
#define	GL_SIGNED_IDENTITY_NV				  0x853C
#define	GL_SIGNED_NEGATE_NV					  0x853D
#define	GL_E_TIMES_F_NV						  0x8531
#define	GL_SPARE0_PLUS_SECONDARY_COLOR_NV	  0x8532
#define	GL_SCALE_BY_TWO_NV					  0x853E
#define	GL_SCALE_BY_FOUR_NV					  0x853F
#define	GL_SCALE_BY_ONE_HALF_NV				  0x8540
#define	GL_BIAS_BY_NEGATIVE_ONE_HALF_NV		  0x8541
#define	GL_ISCARD_NV						  0x8530
#define	GL_COMBINER_INPUT_NV				  0x8542
#define	GL_COMBINER_MAPPING_NV				  0x8543
#define	GL_COMBINER_COMPONENT_USAGE_NV		  0x8544
#define	GL_COMBINER_AB_DOT_PRODUCT_NV		  0x8545
#define	GL_COMBINER_CD_DOT_PRODUCT_NV		  0x8546
#define	GL_COMBINER_MUX_SUM_NV				  0x8547
#define	GL_COMBINER_SCALE_NV				  0x8548
#define	GL_COMBINER_BIAS_NV					  0x8549
#define	GL_COMBINER_AB_OUTPUT_NV			  0x854A
#define	GL_COMBINER_CD_OUTPUT_NV			  0x854B
#define	GL_COMBINER_SUM_OUTPUT_NV			  0x854C
#define	GL_NUM_GENERAL_COMBINERS_NV			  0x854E
#define	GL_COLOR_SUM_CLAMP_NV				  0x854F
#define	GL_MAX_GENERAL_COMBINERS_NV			  0x854D

#endif

/*
** GL_NV_register_combiners2
**
** Support:
**	 Geforce
*/

#ifndef	GL_NV_register_combiners2
#define	GL_NV_register_combiners2 1

typedef	GLvoid (APIENTRY * PFNGLCOMBINERSTAGEPARAMETERFVNVPROC)	(GLenum	stage, GLenum pname, const GLfloat *params);
typedef	GLvoid (APIENTRY * PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC) (GLenum stage, GLenum pname,	GLfloat	*params);

WGLEXTERN PFNGLCOMBINERSTAGEPARAMETERFVNVPROC			glCombinerStageParameterfvNV;
WGLEXTERN PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC		glGetCombinerStageParameterfvNV;

#define	GL_PER_STAGE_CONSTANTS_NV			  0x8535

#endif


/*
** GL_NV_evaluators
**
** Support:
**	 Geforce
*/

#ifndef	GL_NV_evaluators
#define	GL_NV_evaluators	1

typedef	GLvoid (APIENTRY *PFNGLMAPCONTROLPOINTSNV)(enum	target,	GLuint index, enum type,
							GLsizei	ustride, GLsizei vstride,
							GLint uorder, GLint	vorder,
							GLboolean packed,
							const void *points);

typedef	GLvoid (APIENTRY *PFNGLMAPPARAMETERIVNV)(enum target, enum pname, const	GLint *params);
typedef	GLvoid (APIENTRY *PFNGLMAPPARAMETERFVNV)(enum target, enum pname, const	float *params);

typedef	GLvoid (APIENTRY *PFNGLGETMAPCONTROLPOINTSNV)(enum target, GLuint index, enum type,
						   GLsizei ustride,	GLsizei	vstride,
						   GLboolean packed, void *points);

typedef	GLvoid (APIENTRY *PFNGLGETMAPPARAMETERIVNV)(enum target, enum pname, GLint *params);
typedef	GLvoid (APIENTRY *PFNGLGETMAPPARAMETERFVNV)(enum target, enum pname, float *params);
typedef	GLvoid (APIENTRY *PFNGLGETMAPATTRIBPARAMETERIVNV)(enum target, GLuint index, enum pname,
							   GLint *params);
typedef	GLvoid (APIENTRY *PFNGLGETMAPATTRIBPARAMETERFVNV)(enum target, GLuint index, enum pname,
							   float *params);

typedef	GLvoid (APIENTRY *PFNGLEVALMAPSNV)(enum	target,	enum mode);



typedef	GLvoid (APIENTRY *PFNGLMAPCONTROLPOINTSNV)(enum	target,	GLuint index, enum type,
						GLsizei	ustride, GLsizei vstride,
						GLint uorder, GLint	vorder,
						GLboolean packed,
						const void *points);

typedef	GLvoid (APIENTRY *PFNGLMAPPARAMETERIVNV)(enum target, enum pname, const	GLint *params);
typedef	GLvoid (APIENTRY *PFNGLMAPPARAMETERFVNV)(enum target, enum pname, const	float *params);

typedef	GLvoid (APIENTRY *PFNGLGETMAPCONTROLPOINTSNV)(enum target, GLuint index, enum type,
						   GLsizei ustride,	GLsizei	vstride,
						   GLboolean packed, void *points);

typedef	GLvoid (APIENTRY *PFNGLGETMAPPARAMETERIVNV)(enum target, enum pname, GLint *params);
typedef	GLvoid (APIENTRY *PFNGLGETMAPPARAMETERFVNV)(enum target, enum pname, float *params);
typedef	GLvoid (APIENTRY *PFNGLGETMAPATTRIBPARAMETERIVNV)(enum target, GLuint index, enum pname,
							   GLint *params);
typedef	GLvoid (APIENTRY *PFNGLGETMAPATTRIBPARAMETERFVNV)(enum target, GLuint index, enum pname,
							   float *params);

typedef	GLvoid (APIENTRY *PFNGLEVALMAPSNV)(enum	target,	enum mode);
#define	GL_EVAL_2D_NV									0x86C0
#define	GL_EVAL_TRIANGULAR_2D_NV						0x86C1
#define	GL_MAP_TESSELLATION_NV							0x86C2
#define	GL_MAP_ATTRIB_U_ORDER_NV						0x86C3
#define	GL_MAP_ATTRIB_V_ORDER_NV						0x86C4
#define	GL_EVAL_FRACTIONAL_TESSELLATION_NV				0x86C5
#define	GL_EVAL_VERTEX_ATTRIB0_NV						0x86C6
#define	GL_EVAL_VERTEX_ATTRIB1_NV						0x86C7
#define	GL_EVAL_VERTEX_ATTRIB2_NV						0x86C8
#define	GL_EVAL_VERTEX_ATTRIB3_NV						0x86C9
#define	GL_EVAL_VERTEX_ATTRIB4_NV						0x86CA
#define	GL_EVAL_VERTEX_ATTRIB5_NV						0x86CB
#define	GL_EVAL_VERTEX_ATTRIB6_NV						0x86CC
#define	GL_EVAL_VERTEX_ATTRIB7_NV						0x86CD
#define	GL_EVAL_VERTEX_ATTRIB8_NV						0x86CE
#define	GL_EVAL_VERTEX_ATTRIB9_NV						0x86CF
#define	GL_EVAL_VERTEX_ATTRIB10_NV						0x86D0
#define	GL_EVAL_VERTEX_ATTRIB11_NV						0x86D1
#define	GL_EVAL_VERTEX_ATTRIB12_NV						0x86D2
#define	GL_EVAL_VERTEX_ATTRIB13_NV						0x86D3
#define	GL_EVAL_VERTEX_ATTRIB14_NV						0x86D4
#define	GL_EVAL_VERTEX_ATTRIB15_NV						0x86D5
#define	GL_MAX_MAP_TESSELLATION_NV						0x86D6
#define	GL_MAX_RATIONAL_EVAL_ORDER_NV					0x86D7

#endif

/*
** GL_NV_vertex_program	(DX8 vertex	shaders)
** http://oss.sgi.com/projects/ogl-sample/registry/NV/vertex_program.txt
**
** Support:
**	 Geforce
*/

#ifndef	GL_NV_vertex_program
#define	GL_NV_vertex_program 1

#define	GL_VERTEX_PROGRAM_NV							  0x8620
#define	GL_VERTEX_PROGRAM_POINT_SIZE_NV					  0x8642
#define	GL_VERTEX_PROGRAM_TWO_SIDE_NV					  0x8643

#define	GL_VERTEX_STATE_PROGRAM_NV						  0x8621

#define	GL_ATTRIB_ARRAY_SIZE_NV							  0x8623
#define	GL_ATTRIB_ARRAY_STRIDE_NV						  0x8624
#define	GL_ATTRIB_ARRAY_TYPE_NV							  0x8625
#define	GL_CURRENT_ATTRIB_NV							  0x8626

#define	GL_PROGRAM_PARAMETER_NV							  0x8644

#define	GL_ATTRIB_ARRAY_POINTER_NV						  0x8645

#define	GL_PROGRAM_TARGET_NV							  0x8646
#define	GL_PROGRAM_LENGTH_NV							  0x8627
#define	GL_PROGRAM_RESIDENT_NV							  0x8647

#define	GL_PROGRAM_STRING_NV							  0x8628

#define	GL_TRACK_MATRIX_NV								  0x8648
#define	GL_TRACK_MATRIX_TRANSFORM_NV					  0x8649

#define	GL_MAX_TRACK_MATRIX_STACK_DEPTH_NV				  0x862E
#define	GL_MAX_TRACK_MATRICES_NV						  0x862F
#define	GL_CURRENT_MATRIX_STACK_DEPTH_NV				  0x8640
#define	GL_CURRENT_MATRIX_NV							  0x8641
#define	GL_VERTEX_PROGRAM_BINDING_NV					  0x864A
#define	GL_PROGRAM_ERROR_POSITION_NV					  0x864B

#define	GL_MODELVIEW_PROJECTION_NV						  0x8629

#define	GL_MATRIX0_NV									  0x8630
#define	GL_MATRIX1_NV									  0x8631
#define	GL_MATRIX2_NV									  0x8632
#define	GL_MATRIX3_NV									  0x8633
#define	GL_MATRIX4_NV									  0x8634
#define	GL_MATRIX5_NV									  0x8635
#define	GL_MATRIX6_NV									  0x8636
#define	GL_MATRIX7_NV									  0x8637

#define	GL_IDENTITY_NV									  0x862A
#define	GL_INVERSE_NV									  0x862B
#define	GL_TRANSPOSE_NV									  0x862C
#define	GL_INVERSE_TRANSPOSE_NV							  0x862D

#define	GL_VERTEX_ATTRIB_ARRAY0_NV						  0x8650
#define	GL_VERTEX_ATTRIB_ARRAY1_NV						  0x8651
#define	GL_VERTEX_ATTRIB_ARRAY2_NV						  0x8652
#define	GL_VERTEX_ATTRIB_ARRAY3_NV						  0x8653
#define	GL_VERTEX_ATTRIB_ARRAY4_NV						  0x8654
#define	GL_VERTEX_ATTRIB_ARRAY5_NV						  0x8655
#define	GL_VERTEX_ATTRIB_ARRAY6_NV						  0x8656
#define	GL_VERTEX_ATTRIB_ARRAY7_NV						  0x8657
#define	GL_VERTEX_ATTRIB_ARRAY8_NV						  0x8658
#define	GL_VERTEX_ATTRIB_ARRAY9_NV						  0x8659
#define	GL_VERTEX_ATTRIB_ARRAY10_NV						  0x865A
#define	GL_VERTEX_ATTRIB_ARRAY11_NV						  0x865B
#define	GL_VERTEX_ATTRIB_ARRAY12_NV						  0x865C
#define	GL_VERTEX_ATTRIB_ARRAY13_NV						  0x865D
#define	GL_VERTEX_ATTRIB_ARRAY14_NV						  0x865E
#define	GL_VERTEX_ATTRIB_ARRAY15_NV						  0x865F

#define	GL_MAP1_VERTEX_ATTRIB0_4_NV						  0x8660
#define	GL_MAP1_VERTEX_ATTRIB1_4_NV						  0x8661
#define	GL_MAP1_VERTEX_ATTRIB2_4_NV						  0x8662
#define	GL_MAP1_VERTEX_ATTRIB3_4_NV						  0x8663
#define	GL_MAP1_VERTEX_ATTRIB4_4_NV						  0x8664
#define	GL_MAP1_VERTEX_ATTRIB5_4_NV						  0x8665
#define	GL_MAP1_VERTEX_ATTRIB6_4_NV						  0x8666
#define	GL_MAP1_VERTEX_ATTRIB7_4_NV						  0x8667
#define	GL_MAP1_VERTEX_ATTRIB8_4_NV						  0x8668
#define	GL_MAP1_VERTEX_ATTRIB9_4_NV						  0x8669
#define	GL_MAP1_VERTEX_ATTRIB10_4_NV					  0x866A
#define	GL_MAP1_VERTEX_ATTRIB11_4_NV					  0x866B
#define	GL_MAP1_VERTEX_ATTRIB12_4_NV					  0x866C
#define	GL_MAP1_VERTEX_ATTRIB13_4_NV					  0x866D
#define	GL_MAP1_VERTEX_ATTRIB14_4_NV					  0x866E
#define	GL_MAP1_VERTEX_ATTRIB15_4_NV					  0x866F

#define	GL_MAP2_VERTEX_ATTRIB0_4_NV						  0x8670
#define	GL_MAP2_VERTEX_ATTRIB1_4_NV						  0x8671
#define	GL_MAP2_VERTEX_ATTRIB2_4_NV						  0x8672
#define	GL_MAP2_VERTEX_ATTRIB3_4_NV						  0x8673
#define	GL_MAP2_VERTEX_ATTRIB4_4_NV						  0x8674
#define	GL_MAP2_VERTEX_ATTRIB5_4_NV						  0x8675
#define	GL_MAP2_VERTEX_ATTRIB6_4_NV						  0x8676
#define	GL_MAP2_VERTEX_ATTRIB7_4_NV						  0x8677
#define	GL_MAP2_VERTEX_ATTRIB8_4_NV						  0x8678
#define	GL_MAP2_VERTEX_ATTRIB9_4_NV						  0x8679
#define	GL_MAP2_VERTEX_ATTRIB10_4_NV					  0x867A
#define	GL_MAP2_VERTEX_ATTRIB11_4_NV					  0x867B
#define	GL_MAP2_VERTEX_ATTRIB12_4_NV					  0x867C
#define	GL_MAP2_VERTEX_ATTRIB13_4_NV					  0x867D
#define	GL_MAP2_VERTEX_ATTRIB14_4_NV					  0x867E
#define	GL_MAP2_VERTEX_ATTRIB15_4_NV					  0x867F

typedef	GLvoid (APIENTRY * PFNGLBINDPROGRAMNVPROC)(GLenum target, GLuint id);
typedef	GLvoid (APIENTRY * PFNGLDELETEPROGRAMSNVPROC)(GLsizei n, const GLuint *ids);
typedef	GLvoid (APIENTRY * PFNGLEXECUTEPROGRAMNVPROC)(GLenum target, GLuint	id,	const GLfloat *params);
typedef	GLvoid (APIENTRY * PFNGLGENPROGRAMSNVPROC)(GLsizei n, GLuint *ids);
typedef	GLboolean (APIENTRY	* PFNGLAREPROGRAMSRESIDENTNVPROC)(GLsizei n, const GLuint *ids,	GLboolean *residences);
typedef	GLvoid (APIENTRY * PFNGLREQUESTRESIDENTPROGRAMSNVPROC)(GLsizei n, GLuint *ids);
typedef	GLvoid (APIENTRY * PFNGLGETPROGRAMPARAMETERFVNVPROC)(GLenum	target,	GLuint index, GLenum pname,	GLfloat	*params);
typedef	GLvoid (APIENTRY * PFNGLGETPROGRAMPARAMETERDVNVPROC)(GLenum	target,	GLuint index, GLenum pname,	GLdouble *params);
typedef	GLvoid (APIENTRY * PFNGLGETPROGRAMIVNVPROC)(GLuint id, GLenum pname, GLint *params);
typedef	GLvoid (APIENTRY * PFNGLGETPROGRAMSTRINGNVPROC)(GLuint id, GLenum pname, GLubyte *program);
typedef	GLvoid (APIENTRY * PFNGLGETTRACKMATRIXIVNVPROC)(GLenum target, GLuint address, GLenum pname, GLint *params);
typedef	GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBDVNVPROC)(GLuint	index, GLenum pname, GLdouble *params);
typedef	GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBFVNVPROC)(GLuint	index, GLenum pname, GLfloat *params);
typedef	GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBIVNVPROC)(GLuint	index, GLenum pname, GLint *params);
typedef	GLvoid (APIENTRY * PFNGLGETVERTEXATTRIBPOINTERVNVPROC)(GLuint index, GLenum	pname, void	**pointer);
typedef	GLboolean (APIENTRY	* PFNGLISPROGRAMNVPROC)(GLuint id);
typedef	GLvoid (APIENTRY * PFNGLLOADPROGRAMNVPROC)(GLenum target, GLuint id, GLsizei len, const	GLubyte	*program);
typedef	GLvoid (APIENTRY * PFNGLPROGRAMPARAMETER4FNVPROC)(GLenum target, GLuint	index, GLfloat x, GLfloat y, GLfloat z,	GLfloat	w);
typedef	GLvoid (APIENTRY * PFNGLPROGRAMPARAMETER4DNVPROC)(GLenum target, GLuint	index, GLdouble	x, GLdouble	y, GLdouble	z, GLdouble	w);
typedef	GLvoid (APIENTRY * PFNGLPROGRAMPARAMETER4DVNVPROC)(GLenum target, GLuint index,	const GLdouble *params);
typedef	GLvoid (APIENTRY * PFNGLPROGRAMPARAMETER4FVNVPROC)(GLenum target, GLuint index,	const GLfloat *params);
typedef	GLvoid (APIENTRY * PFNGLPROGRAMPARAMETERS4DVNVPROC)(GLenum target, GLuint index, GLuint	num, const GLdouble	*params);
typedef	GLvoid (APIENTRY * PFNGLPROGRAMPARAMETERS4FVNVPROC)(GLenum target, GLuint index, GLuint	num, const GLfloat *params);
typedef	GLvoid (APIENTRY * PFNGLTRACKMATRIXNVPROC)(GLenum target, GLuint address, GLenum matrix, GLenum	transform);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBPOINTERNVPROC)(GLuint index, GLint size, GLenum	type, GLsizei stride, const	void *pointer);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB1SNVPROC)(GLuint index,	GLshort	x);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB1FNVPROC)(GLuint index,	GLfloat	x);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB1DNVPROC)(GLuint index,	GLdouble x);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB2SNVPROC)(GLuint index,	GLshort	x, GLshort y);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB2FNVPROC)(GLuint index,	GLfloat	x, GLfloat y);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB2DNVPROC)(GLuint index,	GLdouble x,	GLdouble y);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB3SNVPROC)(GLuint index,	GLshort	x, GLshort y, GLshort z);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB3FNVPROC)(GLuint index,	GLfloat	x, GLfloat y, GLfloat z);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB3DNVPROC)(GLuint index,	GLdouble x,	GLdouble y,	GLdouble z);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4SNVPROC)(GLuint index,	GLshort	x, GLshort y, GLshort z, GLshort w);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4FNVPROC)(GLuint index,	GLfloat	x, GLfloat y, GLfloat z, GLfloat w);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4DNVPROC)(GLuint index,	GLdouble x,	GLdouble y,	GLdouble z,	GLdouble w);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4UBNVPROC)(GLuint index, GLubyte x,	GLubyte	y, GLubyte z, GLubyte w);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB1SVNVPROC)(GLuint index, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB1FVNVPROC)(GLuint index, const GLfloat *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB1DVNVPROC)(GLuint index, const GLdouble	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB2SVNVPROC)(GLuint index, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB2FVNVPROC)(GLuint index, const GLfloat *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB2DVNVPROC)(GLuint index, const GLdouble	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB3SVNVPROC)(GLuint index, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB3FVNVPROC)(GLuint index, const GLfloat *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB3DVNVPROC)(GLuint index, const GLdouble	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4SVNVPROC)(GLuint index, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4FVNVPROC)(GLuint index, const GLfloat *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4DVNVPROC)(GLuint index, const GLdouble	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIB4UBVNVPROC)(GLuint index, const	GLubyte	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBS1SVNVPROC)(GLuint index, GLsizei n, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBS1FVNVPROC)(GLuint index, GLsizei n, const GLfloat *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBS1DVNVPROC)(GLuint index, GLsizei n, const GLdouble	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBS2SVNVPROC)(GLuint index, GLsizei n, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBS2FVNVPROC)(GLuint index, GLsizei n, const GLfloat *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBS2DVNVPROC)(GLuint index, GLsizei n, const GLdouble	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBS3SVNVPROC)(GLuint index, GLsizei n, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBS3FVNVPROC)(GLuint index, GLsizei n, const GLfloat *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBS3DVNVPROC)(GLuint index, GLsizei n, const GLdouble	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBS4SVNVPROC)(GLuint index, GLsizei n, const GLshort *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBS4FVNVPROC)(GLuint index, GLsizei n, const GLfloat *v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBS4DVNVPROC)(GLuint index, GLsizei n, const GLdouble	*v);
typedef	GLvoid (APIENTRY * PFNGLVERTEXATTRIBS4UBVNVPROC)(GLuint	index, GLsizei n, const	GLubyte	*v);

WGLEXTERN PFNGLBINDPROGRAMNVPROC				glBindProgramNV;
WGLEXTERN PFNGLDELETEPROGRAMSNVPROC				glDeleteProgramsNV;
WGLEXTERN PFNGLEXECUTEPROGRAMNVPROC				glExecuteProgramNV;
WGLEXTERN PFNGLGENPROGRAMSNVPROC				glGenProgramsNV;
WGLEXTERN PFNGLAREPROGRAMSRESIDENTNVPROC		glAreProgramsResidentNV;
WGLEXTERN PFNGLREQUESTRESIDENTPROGRAMSNVPROC	glRequestResidentProgramsNV;
WGLEXTERN PFNGLGETPROGRAMPARAMETERFVNVPROC		glGetProgramParameterfvNV;
WGLEXTERN PFNGLGETPROGRAMPARAMETERDVNVPROC		glGetProgramParameterdvNV;
WGLEXTERN PFNGLGETPROGRAMIVNVPROC				glGetProgramivNV;
WGLEXTERN PFNGLGETPROGRAMSTRINGNVPROC			glGetProgramStringNV;
WGLEXTERN PFNGLGETTRACKMATRIXIVNVPROC			glGetTrackMatrixivNV;
WGLEXTERN PFNGLGETVERTEXATTRIBDVNVPROC			glGetVertexAttribdvNV;
WGLEXTERN PFNGLGETVERTEXATTRIBFVNVPROC			glGetVertexAttribfvNV;
WGLEXTERN PFNGLGETVERTEXATTRIBIVNVPROC			glGetVertexAttribivNV;
WGLEXTERN PFNGLGETVERTEXATTRIBPOINTERVNVPROC	glGetVertexAttribPointervNV;
WGLEXTERN PFNGLISPROGRAMNVPROC					glIsProgramNV;
WGLEXTERN PFNGLLOADPROGRAMNVPROC				glLoadProgramNV;
WGLEXTERN PFNGLPROGRAMPARAMETER4FNVPROC			glProgramParameter4fNV;
WGLEXTERN PFNGLPROGRAMPARAMETER4DNVPROC			glProgramParameter4dNV;
WGLEXTERN PFNGLPROGRAMPARAMETER4DVNVPROC		glProgramParameter4dvNV;
WGLEXTERN PFNGLPROGRAMPARAMETER4FVNVPROC		glProgramParameter4fvNV;
WGLEXTERN PFNGLPROGRAMPARAMETERS4DVNVPROC		glProgramParameters4dvNV;
WGLEXTERN PFNGLPROGRAMPARAMETERS4FVNVPROC		glProgramParameters4fvNV;
WGLEXTERN PFNGLTRACKMATRIXNVPROC				glTrackMatrixNV;
WGLEXTERN PFNGLVERTEXATTRIBPOINTERNVPROC		glVertexAttribPointerNV;
WGLEXTERN PFNGLVERTEXATTRIB1SNVPROC				glVertexAttrib1sNV;
WGLEXTERN PFNGLVERTEXATTRIB1FNVPROC				glVertexAttrib1fNV;
WGLEXTERN PFNGLVERTEXATTRIB1DNVPROC				glVertexAttrib1dNV;
WGLEXTERN PFNGLVERTEXATTRIB2SNVPROC				glVertexAttrib2sNV;
WGLEXTERN PFNGLVERTEXATTRIB2FNVPROC				glVertexAttrib2fNV;
WGLEXTERN PFNGLVERTEXATTRIB2DNVPROC				glVertexAttrib2dNV;
WGLEXTERN PFNGLVERTEXATTRIB3SNVPROC				glVertexAttrib3sNV;
WGLEXTERN PFNGLVERTEXATTRIB3FNVPROC				glVertexAttrib3fNV;
WGLEXTERN PFNGLVERTEXATTRIB3DNVPROC				glVertexAttrib3dNV;
WGLEXTERN PFNGLVERTEXATTRIB4SNVPROC				glVertexAttrib4sNV;
WGLEXTERN PFNGLVERTEXATTRIB4FNVPROC				glVertexAttrib4fNV;
WGLEXTERN PFNGLVERTEXATTRIB4DNVPROC				glVertexAttrib4dNV;
WGLEXTERN PFNGLVERTEXATTRIB4UBNVPROC			glVertexAttrib4ubNV;
WGLEXTERN PFNGLVERTEXATTRIB1SVNVPROC			glVertexAttrib1svNV;
WGLEXTERN PFNGLVERTEXATTRIB1FVNVPROC			glVertexAttrib1fvNV;
WGLEXTERN PFNGLVERTEXATTRIB1DVNVPROC			glVertexAttrib1dvNV;
WGLEXTERN PFNGLVERTEXATTRIB2SVNVPROC			glVertexAttrib2svNV;
WGLEXTERN PFNGLVERTEXATTRIB2FVNVPROC			glVertexAttrib2fvNV;
WGLEXTERN PFNGLVERTEXATTRIB2DVNVPROC			glVertexAttrib2dvNV;
WGLEXTERN PFNGLVERTEXATTRIB3SVNVPROC			glVertexAttrib3svNV;
WGLEXTERN PFNGLVERTEXATTRIB3FVNVPROC			glVertexAttrib3fvNV;
WGLEXTERN PFNGLVERTEXATTRIB3DVNVPROC			glVertexAttrib3dvNV;
WGLEXTERN PFNGLVERTEXATTRIB4SVNVPROC			glVertexAttrib4svNV;
WGLEXTERN PFNGLVERTEXATTRIB4FVNVPROC			glVertexAttrib4fvNV;
WGLEXTERN PFNGLVERTEXATTRIB4DVNVPROC			glVertexAttrib4dvNV;
WGLEXTERN PFNGLVERTEXATTRIB4UBVNVPROC			glVertexAttrib4ubvNV;
WGLEXTERN PFNGLVERTEXATTRIBS1SVNVPROC			glVertexAttribs1svNV;
WGLEXTERN PFNGLVERTEXATTRIBS1FVNVPROC			glVertexAttribs1fvNV;
WGLEXTERN PFNGLVERTEXATTRIBS1DVNVPROC			glVertexAttribs1dvNV;
WGLEXTERN PFNGLVERTEXATTRIBS2SVNVPROC			glVertexAttribs2svNV;
WGLEXTERN PFNGLVERTEXATTRIBS2FVNVPROC			glVertexAttribs2fvNV;
WGLEXTERN PFNGLVERTEXATTRIBS2DVNVPROC			glVertexAttribs2dvNV;
WGLEXTERN PFNGLVERTEXATTRIBS3SVNVPROC			glVertexAttribs3svNV;
WGLEXTERN PFNGLVERTEXATTRIBS3FVNVPROC			glVertexAttribs3fvNV;
WGLEXTERN PFNGLVERTEXATTRIBS3DVNVPROC			glVertexAttribs3dvNV;
WGLEXTERN PFNGLVERTEXATTRIBS4SVNVPROC			glVertexAttribs4svNV;
WGLEXTERN PFNGLVERTEXATTRIBS4FVNVPROC			glVertexAttribs4fvNV;
WGLEXTERN PFNGLVERTEXATTRIBS4DVNVPROC			glVertexAttribs4dvNV;
WGLEXTERN PFNGLVERTEXATTRIBS4UBVNVPROC			glVertexAttribs4ubvNV;

#endif

#ifndef	GL_NV_fragment_program
#define	GL_NV_fragment_program 1

#define	GL_FRAGMENT_PROGRAM_NV							  0x8870
#define	GL_MAX_TEXTURE_COORDS_NV						  0x8871
#define	GL_MAX_TEXTURE_IMAGE_UNITS_NV					  0x8872
#define	GL_FRAGMENT_PROGRAM_BINDING_NV					  0x8873
#define	GL_MAX_FRAGMENT_PROGRAM_LOCAL_PARAMETERS_NV		  0x8868
#define	GL_PROGRAM_ERROR_STRING_NV						  0x8874

typedef	void (APIENTRY * PFNGLPROGRAMNAMEDPARAMETER4FNVPROC) (GLuint id, GLsizei len, const	GLubyte	*name, GLfloat x, GLfloat y, GLfloat z,	GLfloat	w);
typedef	void (APIENTRY * PFNGLPROGRAMNAMEDPARAMETER4DNVPROC) (GLuint id, GLsizei len, const	GLubyte	*name, GLdouble	x, GLdouble	y, GLdouble	z, GLdouble	w);
typedef	void (APIENTRY * PFNGLPROGRAMNAMEDPARAMETER4FVNVPROC) (GLuint id, GLsizei len, const GLubyte *name,	const GLfloat v[]);
typedef	void (APIENTRY * PFNGLPROGRAMNAMEDPARAMETER4DVNVPROC) (GLuint id, GLsizei len, const GLubyte *name,	const GLdouble v[]);
typedef	void (APIENTRY * PFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC)	(GLuint	id,	GLsizei	len, const GLubyte *name, GLfloat *params);
typedef	void (APIENTRY * PFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC)	(GLuint	id,	GLsizei	len, const GLubyte *name, GLdouble *params);

WGLEXTERN PFNGLPROGRAMNAMEDPARAMETER4FNVPROC glProgramNamedParameter4fNV;
WGLEXTERN PFNGLPROGRAMNAMEDPARAMETER4DNVPROC glProgramNamedParameter4dNV;
WGLEXTERN PFNGLPROGRAMNAMEDPARAMETER4FVNVPROC glProgramNamedParameter4fvNV;
WGLEXTERN PFNGLPROGRAMNAMEDPARAMETER4DVNVPROC glProgramNamedParameter4dvNV;
WGLEXTERN PFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC	glGetProgramNamedParameterfvNV;
WGLEXTERN PFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC	glGetProgramNamedParameterdvNV;

#endif //

/*
** GL_NV_occlusion_query
** http://www.nvidia.com/dev_content/nvopenglspecs/GL_NV_occlusion_query.txt
**
** Support:
**	 Geforce 3 / GeForce 4 Ti
*/

#ifndef	GL_NV_occlusion_query
#define	GL_NV_occlusion_query 1

typedef	GLvoid		(APIENTRY *	PFNGLGENOCCLUSIONQUERIESNV)(GLsizei	n, GLuint *ids);
typedef	GLvoid		(APIENTRY *	PFNGLDELETEOCCLUSIONQUERIESNV)(GLsizei n, const	GLuint *ids);
typedef	GLboolean	(APIENTRY *	PFNGLISOCCLUSIONQUERYNV)(GLuint	id);
typedef	GLvoid		(APIENTRY *	PFNGLBEGINOCCLUSIONQUERYNV)(GLuint id);
typedef	GLvoid		(APIENTRY *	PFNGLENDOCCLUSIONQUERYNV)(void);
typedef	GLvoid		(APIENTRY *	PFNGLGETOCCLUSIONQUERYIVNV)(GLuint id, GLenum pname, GLint *params);
typedef	GLvoid		(APIENTRY *	PFNGLGETOCCLUSIONQUERYUIVNV)(GLuint	id,	GLenum pname, GLuint *params);

#define	GL_PIXEL_COUNTER_BITS_NV					0x8864
#define	GL_CURRENT_OCCLUSION_QUERY_ID_NV			0x8865
#define	GL_PIXEL_COUNT_NV							0x8866
#define	GL_PIXEL_COUNT_AVAILABLE_NV					0x8867

WGLEXTERN PFNGLGENOCCLUSIONQUERIESNV				glGenOcclusionQueriesNV;
WGLEXTERN PFNGLDELETEOCCLUSIONQUERIESNV				glDeleteOcclusionQueriesNV;
WGLEXTERN PFNGLISOCCLUSIONQUERYNV					glIsOcclusionQueryNV;
WGLEXTERN PFNGLBEGINOCCLUSIONQUERYNV				glBeginOcclusionQueryNV;
WGLEXTERN PFNGLENDOCCLUSIONQUERYNV					glEndOcclusionQueryNV;
WGLEXTERN PFNGLGETOCCLUSIONQUERYIVNV				glGetOcclusionQueryivNV;
WGLEXTERN PFNGLGETOCCLUSIONQUERYUIVNV				glGetOcclusionQueryuivNV;

#endif

#ifndef	GL_HP_occlusion_query
#define	GL_HP_occlusion_query 1

#define	GL_OCCLUSION_TEST_HP						0x8165
#define	GL_OCCLUSION_TEST_RESULT_HP					0x8166

#endif // GL_HP_occlusion_query


/*
** GL_NV_point_sprite
**
** Support:
**	 Geforce 3 / GeForce 4 Ti
*/

#ifndef	GL_NV_point_sprite
#define	GL_NV_point_sprite 1

#define	GL_POINT_SPRITE_NV							0x8861
#define	GL_COORD_REPLACE_NV							0x8862
#define	GL_POINT_SPRITE_R_MODE_NV					0x8863

#endif // GL_NV_point_sprite


/*************************************************************************************************************
**	ATI	EXTENSIONS
*************************************************************************************************************/

/*
** GL_ATI_envmap_bumpmap
**
** Support:
**	 Rage 128 *	based  : Not Supported
**	 Radeon	  *	based  : Supported
**	 R200	  *	based  : Supported
*/
#ifndef	GL_ATI_envmap_bumpmap
#define	GL_ATI_envmap_bumpmap 1

#define	GL_BUMP_ROT_MATRIX_ATI					   0x8775
#define	GL_BUMP_ROT_MATRIX_SIZE_ATI				   0x8776
#define	GL_BUMP_NUM_TEX_UNITS_ATI				   0x8777
#define	GL_BUMP_TEX_UNITS_ATI					   0x8778
#define	GL_DUDV_ATI								   0x8779
#define	GL_DU8DV8_ATI							   0x877A
#define	GL_BUMP_ENVMAP_ATI						   0x877B
#define	GL_BUMP_TARGET_ATI						   0x877C

typedef	GLvoid (APIENTRY * PFNGLTEXBUMPPARAMETERIVATIPROC) (GLenum pname, GLint	*param);
typedef	GLvoid (APIENTRY * PFNGLTEXBUMPPARAMETERFVATIPROC) (GLenum pname, GLfloat *param);
typedef	GLvoid (APIENTRY * PFNGLGETTEXBUMPPARAMETERIVATIPROC) (GLenum pname, GLint *param);
typedef	GLvoid (APIENTRY * PFNGLGETTEXBUMPPARAMETERFVATIPROC) (GLenum pname, GLfloat *param);

WGLEXTERN PFNGLTEXBUMPPARAMETERIVATIPROC			glTexBumpParameterivATI;
WGLEXTERN PFNGLTEXBUMPPARAMETERFVATIPROC			glTexBumpParameterfvATI;
WGLEXTERN PFNGLGETTEXBUMPPARAMETERIVATIPROC			glGetTexBumpParameterivATI;
WGLEXTERN PFNGLGETTEXBUMPPARAMETERFVATIPROC			glGetTexBumpParameterfvATI;

#endif /* GL_ATI_envmap_bumpmap	*/


/*
** GL_ATI_texture_env_combine3
**
** Support:
**	Rage 128 * based  :	Not	Supported
**	Radeon	 * based  :	Supported
*/
#ifndef	GL_ATI_texture_env_combine3
#define	GL_ATI_texture_env_combine3

#define	GL_MODULATE_ADD_ATI					0x8744
#define	GL_MODULATE_SIGNED_ADD_ATI			0x8745
#define	GL_MODULATE_SUBTRACT_ATI			0x8746

#endif

/*
** GL_ATI_texture_env_subtract
**
** Support:
**	Rage 128 * based  :	Not	Supported
**	Radeon	 * based  :	Supported
*/
#ifndef	GL_ATI_texture_env_subtract
#define	GL_ATI_texture_env_subtract	1

#define	GL_SUBTRACT_ATI						0x6072

#endif

/*
** GL_ATI_texture_mirror_once
**
** Support:
**	Rage 128 * based : Supported
**	Radeon	 * based : Supported
*/
#ifndef	GL_ATI_texture_mirror_once
#define	GL_ATI_texture_mirror_once 1

#define	GL_MIRROR_CLAMP_ATI					0x8742
#define	GL_MIRROR_CLAMP_TO_EDGE_ATI			0x8743

#endif

/*
** GL_ATI_vertex_streams
**
**	Support:
**	 Rage 128 *	based :	Supported
**	 Radeon	  *	based :	Supported
*/
#ifndef	GL_ATI_vertex_streams
#define	GL_ATI_vertex_streams				1

#define	GL_MAX_VERTEX_STREAMS_ATI			0x6050
#define	GL_VERTEX_SOURCE_ATI				0x6051
#define	GL_VERTEX_STREAM0_ATI				0x6030
#define	GL_VERTEX_STREAM1_ATI				0x6031
#define	GL_VERTEX_STREAM2_ATI				0x6032
#define	GL_VERTEX_STREAM3_ATI				0x6033
#define	GL_VERTEX_STREAM4_ATI				0x6034
#define	GL_VERTEX_STREAM5_ATI				0x6035
#define	GL_VERTEX_STREAM6_ATI				0x6036
#define	GL_VERTEX_STREAM7_ATI				0x6037
#define	GL_VERTEX_STREAM8_ATI				0x6038
#define	GL_VERTEX_STREAM9_ATI				0x6039
#define	GL_VERTEX_STREAM10_ATI				0x603A
#define	GL_VERTEX_STREAM11_ATI				0x603B
#define	GL_VERTEX_STREAM12_ATI				0x603C
#define	GL_VERTEX_STREAM13_ATI				0x603D
#define	GL_VERTEX_STREAM14_ATI				0x603E
#define	GL_VERTEX_STREAM15_ATI				0x603F
#define	GL_VERTEX_STREAM16_ATI				0x6040
#define	GL_VERTEX_STREAM17_ATI				0x6041
#define	GL_VERTEX_STREAM18_ATI				0x6042
#define	GL_VERTEX_STREAM19_ATI				0x6043
#define	GL_VERTEX_STREAM20_ATI				0x6044
#define	GL_VERTEX_STREAM21_ATI				0x6045
#define	GL_VERTEX_STREAM22_ATI				0x6046
#define	GL_VERTEX_STREAM23_ATI				0x6047
#define	GL_VERTEX_STREAM24_ATI				0x6048
#define	GL_VERTEX_STREAM25_ATI				0x6049
#define	GL_VERTEX_STREAM26_ATI				0x604A
#define	GL_VERTEX_STREAM27_ATI				0x604B
#define	GL_VERTEX_STREAM28_ATI				0x604C
#define	GL_VERTEX_STREAM29_ATI				0x604D
#define	GL_VERTEX_STREAM30_ATI				0x604E
#define	GL_VERTEX_STREAM31_ATI				0x604F

typedef	void (APIENTRY * PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC) (GLenum stream);
typedef	void (APIENTRY * PFNGLVERTEXBLENDENVIATIPROC) (GLenum pname, GLint param);
typedef	void (APIENTRY * PFNGLVERTEXBLENDENVFATIPROC) (GLenum pname, GLfloat param);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM2SATIPROC) (GLenum stream, GLshort x,	GLshort	y);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM2SVATIPROC) (GLenum stream, const	GLshort	*v);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM2IATIPROC) (GLenum stream, GLint x, GLint	y);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM2IVATIPROC) (GLenum stream, const	GLint *v);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM2FATIPROC) (GLenum stream, GLfloat x,	GLfloat	y);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM2FVATIPROC) (GLenum stream, const	GLfloat	*v);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM2DATIPROC) (GLenum stream, GLdouble x, GLdouble y);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM2DVATIPROC) (GLenum stream, const	GLdouble *v);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM3SATIPROC) (GLenum stream, GLshort x,	GLshort	y, GLshort z);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM3SVATIPROC) (GLenum stream, const	GLshort	*v);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM3IATIPROC) (GLenum stream, GLint x, GLint	y, GLint z);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM3IVATIPROC) (GLenum stream, const	GLint *v);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM3FATIPROC) (GLenum stream, GLfloat x,	GLfloat	y, GLfloat z);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM3FVATIPROC) (GLenum stream, const	GLfloat	*v);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM3DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM3DVATIPROC) (GLenum stream, const	GLdouble *v);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM4SATIPROC) (GLenum stream, GLshort x,	GLshort	y, GLshort z, GLshort w);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM4SVATIPROC) (GLenum stream, const	GLshort	*v);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM4IATIPROC) (GLenum stream, GLint x, GLint	y, GLint z,	GLint w);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM4IVATIPROC) (GLenum stream, const	GLint *v);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM4FATIPROC) (GLenum stream, GLfloat x,	GLfloat	y, GLfloat z, GLfloat w);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM4FVATIPROC) (GLenum stream, const	GLfloat	*v);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM4DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef	void (APIENTRY * PFNGLVERTEXSTREAM4DVATIPROC) (GLenum stream, const	GLdouble *v);
typedef	void (APIENTRY * PFNGLNORMALSTREAM3BATIPROC) (GLenum stream, GLbyte	x, GLbyte y, GLbyte	z);
typedef	void (APIENTRY * PFNGLNORMALSTREAM3BVATIPROC) (GLenum stream, const	GLbyte *v);
typedef	void (APIENTRY * PFNGLNORMALSTREAM3SATIPROC) (GLenum stream, GLshort x,	GLshort	y, GLshort z);
typedef	void (APIENTRY * PFNGLNORMALSTREAM3SVATIPROC) (GLenum stream, const	GLshort	*v);
typedef	void (APIENTRY * PFNGLNORMALSTREAM3IATIPROC) (GLenum stream, GLint x, GLint	y, GLint z);
typedef	void (APIENTRY * PFNGLNORMALSTREAM3IVATIPROC) (GLenum stream, const	GLint *v);
typedef	void (APIENTRY * PFNGLNORMALSTREAM3FATIPROC) (GLenum stream, GLfloat x,	GLfloat	y, GLfloat z);
typedef	void (APIENTRY * PFNGLNORMALSTREAM3FVATIPROC) (GLenum stream, const	GLfloat	*v);
typedef	void (APIENTRY * PFNGLNORMALSTREAM3DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z);
typedef	void (APIENTRY * PFNGLNORMALSTREAM3DVATIPROC) (GLenum stream, const	GLdouble *v);
#endif /* GL_ATI_vertex_streams	*/

/*
** GL_ATI_fragment_shader
**
** Support:
**	 Rage 128 *	based  : Not Supported
**	 Radeon	  *	based  : Not Supported
**	 R200	  *	based  : Supported
*/
#ifndef	GL_ATI_fragment_shader
#define	GL_ATI_fragment_shader 1

#define	GL_FRAGMENT_SHADER_ATI						0x8920
#define	GL_REG_0_ATI								0x8921
#define	GL_REG_1_ATI								0x8922
#define	GL_REG_2_ATI								0x8923
#define	GL_REG_3_ATI								0x8924
#define	GL_REG_4_ATI								0x8925
#define	GL_REG_5_ATI								0x8926
#define	GL_REG_6_ATI								0x8927
#define	GL_REG_7_ATI								0x8928
#define	GL_REG_8_ATI								0x8929
#define	GL_REG_9_ATI								0x892A
#define	GL_REG_10_ATI								0x892B
#define	GL_REG_11_ATI								0x892C
#define	GL_REG_12_ATI								0x892D
#define	GL_REG_13_ATI								0x892E
#define	GL_REG_14_ATI								0x892F
#define	GL_REG_15_ATI								0x8930
#define	GL_REG_16_ATI								0x8931
#define	GL_REG_17_ATI								0x8932
#define	GL_REG_18_ATI								0x8933
#define	GL_REG_19_ATI								0x8934
#define	GL_REG_20_ATI								0x8935
#define	GL_REG_21_ATI								0x8936
#define	GL_REG_22_ATI								0x8937
#define	GL_REG_23_ATI								0x8938
#define	GL_REG_24_ATI								0x8939
#define	GL_REG_25_ATI								0x893A
#define	GL_REG_26_ATI								0x893B
#define	GL_REG_27_ATI								0x893C
#define	GL_REG_28_ATI								0x893D
#define	GL_REG_29_ATI								0x893E
#define	GL_REG_30_ATI								0x893F
#define	GL_REG_31_ATI								0x8940
#define	GL_CON_0_ATI								0x8941
#define	GL_CON_1_ATI								0x8942
#define	GL_CON_2_ATI								0x8943
#define	GL_CON_3_ATI								0x8944
#define	GL_CON_4_ATI								0x8945
#define	GL_CON_5_ATI								0x8946
#define	GL_CON_6_ATI								0x8947
#define	GL_CON_7_ATI								0x8948
#define	GL_CON_8_ATI								0x8949
#define	GL_CON_9_ATI								0x894A
#define	GL_CON_10_ATI								0x894B
#define	GL_CON_11_ATI								0x894C
#define	GL_CON_12_ATI								0x894D
#define	GL_CON_13_ATI								0x894E
#define	GL_CON_14_ATI								0x894F
#define	GL_CON_15_ATI								0x8950
#define	GL_CON_16_ATI								0x8951
#define	GL_CON_17_ATI								0x8952
#define	GL_CON_18_ATI								0x8953
#define	GL_CON_19_ATI								0x8954
#define	GL_CON_20_ATI								0x8955
#define	GL_CON_21_ATI								0x8956
#define	GL_CON_22_ATI								0x8957
#define	GL_CON_23_ATI								0x8958
#define	GL_CON_24_ATI								0x8959
#define	GL_CON_25_ATI								0x895A
#define	GL_CON_26_ATI								0x895B
#define	GL_CON_27_ATI								0x895C
#define	GL_CON_28_ATI								0x895D
#define	GL_CON_29_ATI								0x895E
#define	GL_CON_30_ATI								0x895F
#define	GL_CON_31_ATI								0x8960
#define	GL_MOV_ATI									0x8961
#define	GL_ADD_ATI									0x8963
#define	GL_MUL_ATI									0x8964
#define	GL_SUB_ATI									0x8965
#define	GL_DOT3_ATI									0x8966
#define	GL_DOT4_ATI									0x8967
#define	GL_MAD_ATI									0x8968
#define	GL_LERP_ATI									0x8969
#define	GL_CND_ATI									0x896A
#define	GL_CND0_ATI									0x896B
#define	GL_DOT2_ADD_ATI								0x896C
#define	GL_SECONDARY_INTERPOLATOR_ATI				0x896D
#define	GL_NUM_FRAGMENT_REGISTERS_ATI				0x896E
#define	GL_NUM_FRAGMENT_CONSTANTS_ATI				0x896F
#define	GL_NUM_PASSES_ATI							0x8970
#define	GL_NUM_INSTRUCTIONS_PER_PASS_ATI			0x8971
#define	GL_NUM_INSTRUCTIONS_TOTAL_ATI				0x8972
#define	GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_ATI	0x8973
#define	GL_NUM_LOOPBACK_COMPONENTS_ATI				0x8974
#define	GL_COLOR_ALPHA_PAIRING_ATI					0x8975
#define	GL_SWIZZLE_STR_ATI							0x8976
#define	GL_SWIZZLE_STQ_ATI							0x8977
#define	GL_SWIZZLE_STR_DR_ATI						0x8978
#define	GL_SWIZZLE_STQ_DQ_ATI						0x8979
#define	GL_SWIZZLE_STRQ_ATI							0x897A
#define	GL_SWIZZLE_STRQ_DQ_ATI						0x897B
#define	GL_RED_BIT_ATI								0x00000001
#define	GL_GREEN_BIT_ATI							0x00000002
#define	GL_BLUE_BIT_ATI								0x00000004
#define	GL_2X_BIT_ATI								0x00000001
#define	GL_4X_BIT_ATI								0x00000002
#define	GL_8X_BIT_ATI								0x00000004
#define	GL_HALF_BIT_ATI								0x00000008
#define	GL_QUARTER_BIT_ATI							0x00000010
#define	GL_EIGHTH_BIT_ATI							0x00000020
#define	GL_SATURATE_BIT_ATI							0x00000040
#define	GL_COMP_BIT_ATI								0x00000002
#define	GL_NEGATE_BIT_ATI							0x00000004
#define	GL_BIAS_BIT_ATI								0x00000008

typedef	GLuint (APIENTRY *PFNGLGENFRAGMENTSHADERSATIPROC)(GLuint range);
typedef	GLvoid (APIENTRY *PFNGLBINDFRAGMENTSHADERATIPROC)(GLuint id);
typedef	GLvoid (APIENTRY *PFNGLDELETEFRAGMENTSHADERATIPROC)(GLuint id);
typedef	GLvoid (APIENTRY *PFNGLBEGINFRAGMENTSHADERATIPROC)(GLvoid);
typedef	GLvoid (APIENTRY *PFNGLENDFRAGMENTSHADERATIPROC)(GLvoid);
typedef	GLvoid (APIENTRY *PFNGLPASSTEXCOORDATIPROC)(GLuint dst,	GLuint coord, GLenum swizzle);
typedef	GLvoid (APIENTRY *PFNGLSAMPLEMAPATIPROC)(GLuint	dst, GLuint	interp,	GLenum swizzle);
typedef	GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP1ATIPROC)(GLenum op, GLuint dst, GLuint dstMask,
									   GLuint dstMod, GLuint arg1, GLuint arg1Rep,
									   GLuint arg1Mod);
typedef	GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP2ATIPROC)(GLenum op, GLuint dst, GLuint dstMask,
									   GLuint dstMod, GLuint arg1, GLuint arg1Rep,
									   GLuint arg1Mod, GLuint arg2,	GLuint arg2Rep,
									   GLuint arg2Mod);
typedef	GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP3ATIPROC)(GLenum op, GLuint dst, GLuint dstMask,
									   GLuint dstMod, GLuint arg1, GLuint arg1Rep,
									   GLuint arg1Mod, GLuint arg2,	GLuint arg2Rep,
									   GLuint arg2Mod, GLuint arg3,	GLuint arg3Rep,
									   GLuint arg3Mod);
typedef	GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP1ATIPROC)(GLenum op, GLuint dst, GLuint dstMod,
									   GLuint arg1,	GLuint arg1Rep,	GLuint arg1Mod);
typedef	GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP2ATIPROC)(GLenum op, GLuint dst, GLuint dstMod,
									   GLuint arg1,	GLuint arg1Rep,	GLuint arg1Mod,
									   GLuint arg2,	GLuint arg2Rep,	GLuint arg2Mod);
typedef	GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP3ATIPROC)(GLenum op, GLuint dst, GLuint dstMod,
									   GLuint arg1,	GLuint arg1Rep,	GLuint arg1Mod,
									   GLuint arg2,	GLuint arg2Rep,	GLuint arg2Mod,
									   GLuint arg3,	GLuint arg3Rep,	GLuint arg3Mod);
typedef	GLvoid (APIENTRY *PFNGLSETFRAGMENTSHADERCONSTANTATIPROC)(GLuint	dst, const GLfloat *value);

#endif /* GL_ATI_fragment_shader */


WGLEXTERN PFNGLGENFRAGMENTSHADERSATIPROC			glGenFragmentShadersATI;
WGLEXTERN PFNGLBINDFRAGMENTSHADERATIPROC			glBindFragmentShaderATI;
WGLEXTERN PFNGLDELETEFRAGMENTSHADERATIPROC			glDeleteFragmentShaderATI;
WGLEXTERN PFNGLBEGINFRAGMENTSHADERATIPROC			glBeginFragmentShaderATI;
WGLEXTERN PFNGLENDFRAGMENTSHADERATIPROC				glEndFragmentShaderATI;
WGLEXTERN PFNGLPASSTEXCOORDATIPROC					glPassTexCoordATI;
WGLEXTERN PFNGLSAMPLEMAPATIPROC						glSampleMapATI;
WGLEXTERN PFNGLCOLORFRAGMENTOP1ATIPROC				glColorFragmentOp1ATI;
WGLEXTERN PFNGLCOLORFRAGMENTOP2ATIPROC				glColorFragmentOp2ATI;
WGLEXTERN PFNGLCOLORFRAGMENTOP3ATIPROC				glColorFragmentOp3ATI;
WGLEXTERN PFNGLALPHAFRAGMENTOP1ATIPROC				glAlphaFragmentOp1ATI;
WGLEXTERN PFNGLALPHAFRAGMENTOP2ATIPROC				glAlphaFragmentOp2ATI;
WGLEXTERN PFNGLALPHAFRAGMENTOP3ATIPROC				glAlphaFragmentOp3ATI;
WGLEXTERN PFNGLSETFRAGMENTSHADERCONSTANTATIPROC		glSetFragmentShaderConstantATI;


/*
** GL_ATI_pn_triangles
**
** Support
**	 Rage 128 *	based  : Not Supported
**	 Radeon	  *	based  : Not Supported
**	 R200	  *	based  : Supported
*/
#ifndef	GL_ATI_pn_triangles
#define	GL_ATI_pn_triangles	1

#define	GL_PN_TRIANGLES_ATI							0x87F0
#define	GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI	0x87F1
#define	GL_PN_TRIANGLES_POINT_MODE_ATI				0x87F2
#define	GL_PN_TRIANGLES_NORMAL_MODE_ATI				0x87F3
#define	GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI		0x87F4
#define	GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI		0x87F5
#define	GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI		0x87F6
#define	GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI		0x87F7
#define	GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI	0x87F8

typedef	GLvoid (APIENTRY *PFNGLPNTRIANGLESIATIPROC)(GLenum pname, GLint	param);
typedef	GLvoid (APIENTRY *PFNGLPNTRIANGLESFATIPROC)(GLenum pname, GLfloat param);

WGLEXTERN PFNGLPNTRIANGLESIATIPROC				glPNTrianglesiATI;
WGLEXTERN PFNGLPNTRIANGLESFATIPROC				glPNTrianglesfATI;

#endif


/*
** GL_ATI_map_object_buffer
**
** Support:
**	 Rage 128 *	based :	Not	supported
**	 Radeon	  *	based :	Supported
**	 Chaplin  *	based :	Supported
*/
#ifndef	GL_ATI_map_object_buffer
#define	GL_ATI_map_object_buffer		1

typedef	GLvoid *(APIENTRY *	PFNGLMAPOBJECTBUFFERATI)(GLuint	buffer);
typedef	GLvoid (APIENTRY * PFNGLUNMAPOBJECTBUFFERATI)(GLuint buffer);

#endif

WGLEXTERN PFNGLMAPOBJECTBUFFERATI	glMapObjectBufferATI;
WGLEXTERN PFNGLUNMAPOBJECTBUFFERATI	 glUnmapObjectBufferATI;

/*
** GL_ATI_vertex_array_object
**
** Support:
**	 Rage 128 *	based :	Not	supported
**	 Radeon	  *	based :	Supported
**	 Chaplin  *	based :	Supported
*/
#ifndef	GL_ATI_vertex_array_object
#define	GL_ATI_vertex_array_object		1

#define	GL_STATIC_ATI					0x8760
#define	GL_DYNAMIC_ATI					0x8761
#define	GL_PRESERVE_ATI					0x8762
#define	GL_DISCARD_ATI					0x8763

#define	GL_OBJECT_BUFFER_SIZE_ATI		0x8764
#define	GL_OBJECT_BUFFER_USAGE_ATI		0x8765
#define	GL_ARRAY_OBJECT_BUFFER_ATI		0x8766
#define	GL_ARRAY_OBJECT_OFFSET_ATI		0x8767

typedef	GLuint (APIENTRY * PFNGLNEWOBJECTBUFFERATIPROC)(GLsizei	size, const	GLvoid *pointer, GLenum	usage);
typedef	GLboolean (APIENTRY	* PFNGLISOBJECTBUFFERATIPROC)(GLuint buffer);
typedef	GLvoid (APIENTRY * PFNGLUPDATEOBJECTBUFFERATIPROC)(GLuint buffer, GLuint offset, GLsizei size,
														 const GLvoid *pointer,	GLenum preserve);
typedef	GLvoid (APIENTRY * PFNGLGETOBJECTBUFFERFVATIPROC)(GLuint buffer, GLenum	pname, GLfloat *params);
typedef	GLvoid (APIENTRY * PFNGLGETOBJECTBUFFERIVATIPROC)(GLuint buffer, GLenum	pname, GLint *params);
typedef	GLvoid (APIENTRY * PFNGLFREEOBJECTBUFFERATIPROC)(GLuint	buffer);
typedef	GLvoid (APIENTRY * PFNGLARRAYOBJECTATIPROC)(GLenum array, GLint	size, GLenum type,
												  GLsizei stride, GLuint buffer, GLuint	offset);
typedef	GLvoid (APIENTRY * PFNGLGETARRAYOBJECTFVATIPROC)(GLenum	array, GLenum pname, GLfloat *params);
typedef	GLvoid (APIENTRY * PFNGLGETARRAYOBJECTIVATIPROC)(GLenum	array, GLenum pname, GLint *params);
typedef	GLvoid (APIENTRY * PFNGLVARIANTARRAYOBJECTATIPROC)(GLuint id, GLenum type,
														 GLsizei stride, GLuint	buffer,	GLuint offset);
typedef	GLvoid (APIENTRY * PFNGLGETVARIANTARRAYOBJECTFVATIPROC)(GLuint id, GLenum pname, GLfloat *params);
typedef	GLvoid (APIENTRY * PFNGLGETVARIANTARRAYOBJECTIVATIPROC)(GLuint id, GLenum pname, GLint *params);

WGLEXTERN PFNGLNEWOBJECTBUFFERATIPROC					glNewObjectBufferATI;
WGLEXTERN PFNGLISOBJECTBUFFERATIPROC					glIsObjectBufferATI;
WGLEXTERN PFNGLUPDATEOBJECTBUFFERATIPROC				glUpdateObjectBufferATI;
WGLEXTERN PFNGLGETOBJECTBUFFERFVATIPROC					glGetObjectBufferfvATI;
WGLEXTERN PFNGLGETOBJECTBUFFERIVATIPROC					glGetObjectBufferivATI;
WGLEXTERN PFNGLFREEOBJECTBUFFERATIPROC					glFreeObjectBufferATI;
WGLEXTERN PFNGLARRAYOBJECTATIPROC						glArrayObjectATI;
WGLEXTERN PFNGLGETARRAYOBJECTFVATIPROC					glGetArrayObjectfvATI;
WGLEXTERN PFNGLGETARRAYOBJECTIVATIPROC					glGetArrayObjectivATI;
WGLEXTERN PFNGLVARIANTARRAYOBJECTATIPROC				glVariantArrayObjectATI;
WGLEXTERN PFNGLGETVARIANTARRAYOBJECTFVATIPROC			glGetVariantArrayObjectfvATI;
WGLEXTERN PFNGLGETVARIANTARRAYOBJECTIVATIPROC			glGetVariantArrayObjectivATI;
#endif

/*
** GL_ATI_element_array
**
** Support:
**	 Rage 128 *	based :	Not	supported
**	 Radeon	  *	based :	Supported
**	 Chaplin  *	based :	Supported
*/
#ifndef	GL_ATI_element_array
#define	GL_ATI_element_array			1

#define	GL_ELEMENT_ARRAY_ATI									0x8768

#define	GL_ELEMENT_ARRAY_TYPE_ATI								0x8769
#define	GL_ELEMENT_ARRAY_POINTER_ATI							0x876A

typedef	GLvoid (APIENTRY * PFNGLELEMENTPOINTERATIPROC)(GLenum type,	const GLvoid *pointer);
typedef	GLvoid (APIENTRY * PFNGLDRAWELEMENTARRAYATIPROC)(GLenum	mode, GLsizei count);
typedef	GLvoid (APIENTRY * PFNGLDRAWRANGEELEMENTARRAYATIPROC)(GLenum mode, GLuint start, GLuint	end, GLsizei count);

#endif // GL_ATI_element_array

WGLEXTERN PFNGLELEMENTPOINTERATIPROC				glElementPointerATI;
WGLEXTERN PFNGLDRAWELEMENTARRAYATIPROC				glDrawElementArrayATI;
WGLEXTERN PFNGLDRAWRANGEELEMENTARRAYATIPROC			glDrawRangeElementArrayATI;

#ifndef	GL_ATIX_point_sprites
#define	GL_ATIX_point_sprites			1

#define	GL_TEXTURE_POINT_MODE_ATIX								0x60b0
#define	GL_TEXTURE_POINT_ONE_COORD_ATIX							0x60b1
#define	GL_TEXTURE_POINT_SPRITE_ATIX							0x60b2
#define	GL_POINT_SPRITE_CULL_MODE_ATIX							0x60b3
#define	GL_POINT_SPRITE_CULL_CENTER_ATIX						0x60b4
#define	GL_POINT_SPRITE_CULL_CLIP_ATIX							0x60b5

#endif // GL_ATIX_point_sprites

#ifndef	GL_ATIX_texture_env_route
#define	GL_ATIX_texture_env_route		1

#define	GL_SECONDARY_COLOR_ATIX									0x8747
#define	GL_TEXTURE_OUTPUT_RGB_ATIX								0x8748
#define	GL_TEXTURE_OUTPUT_ALPHA_ATIX							0x8749

#endif // GL_ATIX_texture_env_route

/*******************************************************************************************
  WGLEXT Library
*******************************************************************************************/
// WIN32
#if	(defined(_WIN32) ||	defined(__WIN32__))

// Choose Pixelformat with pBpp	= color	depth, pDepth =	zBuffer	Depth, pDbl	= Double buffer	on/off,	pAcc = hw accel	on/off
extern GLint			ChoosePixelFormatEx(HDC	pHdc, GLint	*pBpp, GLint *pDepth, GLint	*pStencil, GLint *pDbl,	GLint *pAcc, GLint *pDib);

// Same	using the ARB 'hidden' interface
extern GLint			ChoosePixelFormatARB(HDC pHdc, GLint *pBpp,	GLint *pDepth, GLint *pStencil,	GLint *pDbl, GLint *pAcc, GLint	*pDib);

// Register	all	functions :)
extern void				wglGetProcAddresses();
extern int				wglGetProcAddressesARB(HDC hdc); //	ARB	'hidden' functions

// Is Extension	supported ?
extern GLint			wglIsExtensionSupported(const char *extension);
extern GLint			wglIsExtensionSupportedARB(HDC hdc,	const char *extension);	// ARB 'hidden'	extensions

// Get all available display mode
void					wglQueryDisplayModes(DEVMODE **ppmode, GLint *pNumModes);

// LINUX
#elif defined __linux__

extern void				glxGetProcAddresses();
extern GLint			glxIsExtensionSupported(const char *extension);
void					glxQueryDisplayModes(DEVMODE **ppmode, GLint *pNumModes);

// MACOS
#elif defined __MACOS__

#endif

#ifdef __cplusplus
}
#endif

#endif /* __glext_h_ */
