/*
** realtech VR, Inc.
** http://www.realtech-vr.com
** Copyright (c) 2000-2003.  All Rights Reserved
**
** Source code from realtech VR is supplied under the terms of a license
** agreement and may not be copied or disclosed except in accordance with the
** terms of that agreement.  The various license agreements may be found at
** the realtech VR web site.  This file is Open Source
**
** FREE SOURCE CODE
** http://www.realtech-vr.com
**
** OpenGL 2.0 extensions
**
** Additional extensions (c) Copyright 1998 S3 Incorporated.
** Additional extensions (c) Copyright 2001 NVidia Corp.
** Additional extensions (c) Copyright 2000 3Dfx Interactive.
** Additional extensions (c) Copyright 2001-2002 ATI Incorporated.
**
*/
#define _EXPORT_WGLEXT

#if (defined(_WIN32) || defined(__WIN32__))
// #include "stdafx.h" // Uncomment this for pre-compiled support.
#pragma hdrstop
#include <windows.h>

#ifdef _USE_MCD
#define WGLGETPROC( type, func) func = (type) mcdGetProcAddress(#func); assert(func)
#define WGLGETPROCNOFAIL(type, func) func = (type) mcdGetProcAddress(#func);
#else
#define WGLGETPROC( type, func) func = (type) wglGetProcAddress(#func); assert(func)
#define WGLGETPROCNOFAIL(type, func) func = (type) wglGetProcAddress(#func);
#endif

#elif defined __linux__

#define WGLGETPROC( type, func) func = (type) glXGetProcAddressARB(#func); assert(func)
#define WGLGETPROCNOFAIL(type, func) func = (type) glXGetProcAddressARB(#func);

#elif defined __MACOS__

// pure undocumented extension
#ifndef GL_MAC_GET_PROC_ADDRESS_NV
#define GL_MAC_GET_PROC_ADDRESS_NV 0x84FC
#endif

typedef void *(*PFUNCMACGETPROCADDRESSNVFUNC)(char *string);
PFUNCMACGETPROCADDRESSNVFUNC	glMacGetProcAddressNV;
#define WGLGETPROC( type, func) func = (type) glMacGetProcAddressNV(#func); assert(func)
#define WGLGETPROCNOFAIL(type, func) func = (type) glMacGetProcAddressNV(#func);

#endif

#include <assert.h>
#include "wglext.h"

#if (defined(_WIN32) || defined(__WIN32__))
// ChoosePixelFormatEX

GLint ChoosePixelFormatEx(HDC pHdc, GLint *pBpp, GLint *pDepth, GLint *pStencil, GLint *pDbl, GLint *pAcc, GLint *pDib)
{
	GLint wbpp		= pBpp		== NULL ?  -1 : *pBpp;
	GLint wdepth	= pDepth	== NULL ?  16 : *pDepth;
	GLint wstencil  = pStencil	== NULL ?   0 : *pDepth;
	GLint wdbl		= pDbl		== NULL ?  -1 : *pDbl;
	GLint wacc		= pAcc		== NULL ?	1 : *pAcc;
	GLint wdib		= pDib		== NULL ? - 1 : *pDib;

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;

	GLint num = DescribePixelFormat(pHdc, 1, sizeof(pfd), &pfd);
	if(num == 0)
		return 0;

	GLuint maxqual = 0;
	GLint maxindex = 0;
	GLint max_bpp, max_depth, max_dbl, max_acc, max_dib, max_stencil;

	for(GLint i = 1; i <= num; i++)
	{
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;

		DescribePixelFormat(pHdc, i, sizeof(pfd), &pfd);

		GLint bpp = pfd.cColorBits;
		GLint depth = pfd.cDepthBits;
		GLint stencil = pfd.cStencilBits;
		bool pal = (pfd.iPixelType == PFD_TYPE_COLORINDEX);
		bool mcd = ((pfd.dwFlags & PFD_GENERIC_FORMAT) && (pfd.dwFlags & PFD_GENERIC_ACCELERATED));
		bool soft = ((pfd.dwFlags & PFD_GENERIC_FORMAT) &&!(pfd.dwFlags & PFD_GENERIC_ACCELERATED));
		bool icd = (!(pfd.dwFlags & PFD_GENERIC_FORMAT) &&!(pfd.dwFlags & PFD_GENERIC_ACCELERATED));
		bool opengl = (pfd.dwFlags & PFD_SUPPORT_OPENGL) ? true :false;
		bool window = (pfd.dwFlags & PFD_DRAW_TO_WINDOW) ? true :false;
		bool bitmap = (pfd.dwFlags & PFD_DRAW_TO_BITMAP) ? true :false;
		bool dbuff	= (pfd.dwFlags & PFD_DOUBLEBUFFER);

		GLuint q = 0;
		if(opengl && window)
			q += 0x10000;
		if(wdepth == -1 || (wdepth > 0 && depth > 0))
			q += 0x8000;
		if(wstencil == -1 || (wstencil > 0 && stencil > 0))
			q += 0x4000;
		if(wdbl == -1 || (wdbl == 0 && !dbuff) || (wdbl == 1 && dbuff))
			q += 0x2000;
		if(wacc == -1 || (wacc == 0 && soft) || (wacc == 1 && (mcd ||icd)))
			q += 0x1000;
		if(mcd || icd)
			q += 0x0040;
		if(icd)
			q += 0x0002;
		if(wbpp == -1 || (wbpp == bpp))
			q += 0x0800;
		if((wdib == -1) || (wdib == 0 && !bitmap) || (wdbl == 1 && bitmap))
			q += 0x10000;
		if(bpp >= 16)
			q += 0x0020;
		if(bpp == 16)
			q += 0x0008;
		if(wdepth == -1 || (wdepth == depth))
			q += 0x0400;

		if(depth >= 16)
			q += 0x0010;
		if(depth == 16)
			q += 0x0004;
		if(!pal)
			q += 0x0080;

		if(q > maxqual)
		{
			maxqual = q;
			maxindex = i;
			max_bpp = bpp;
			max_depth = depth;
			max_stencil = stencil;
			max_dbl = dbuff ? 1 : 0;
			max_acc = soft ? 0 : 1;
			max_dib = bitmap ? 1 : 0;
		}
	}
	if(maxindex == 0)
		return maxindex;

	if(pBpp != NULL)
		*pBpp = max_bpp;
	if(pDepth != NULL)
		*pDepth = max_depth;
	if(pStencil != NULL)
		*pStencil = max_stencil;
	if(pDbl != NULL)
		*pDbl = max_dbl;
	if(pAcc != NULL)
		*pAcc = max_acc;
	if(pDib !=NULL)
		*pDib = max_dib;

	return maxindex;
}

void wglQueryDisplayModes(DEVMODE **ppmode, GLint *pNumModes)
{
	GLint n = 0;
	DEVMODE  devmode;
	devmode.dmSize = sizeof(DEVMODE);

	while (EnumDisplaySettings(NULL, n, &devmode))
		n++;

	if (pNumModes)
		*pNumModes = n;

	/* fill an array with all the devmodes so we don't have to keep
		grabbing 'em. */
	if (ppmode)
	{
		DEVMODE * p = new DEVMODE[ n + 1];
		n = 0;
		p[n].dmSize = sizeof(DEVMODE);

		while (EnumDisplaySettings(NULL, n, p + n))
		{
			n++;
			p[n].dmSize = sizeof(DEVMODE);
		}
		*ppmode = p;
	}
}

// ARB Pixel Format
GLint wglIsExtensionSupportedARB(HDC hdc, const char *extension)
{
	static const char *extensions = NULL;
	const char *start;
	char *where, *terminator;
	if (!extension)
	{
		extensions = NULL;
		return 1;
	}
	if (!wglGetExtensionsStringARB)
		return 0;

	where = strchr(extension, ' ');
	if (where || *extension == '\0')
		return 0;
	if (!extensions)
		extensions = wglGetExtensionsStringARB(hdc);
	start = extensions;
	for (;;) {
		where = strstr((const char *) start, extension);
		if (!where)
			break;
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ') {
			if (*terminator == ' ' || *terminator == '\0') {
				return 1;
			}
		}
		start = terminator;
	}
	return 0;
}

int wglGetProcAddressesARB(HDC hDC)
{
	WGLGETPROCNOFAIL(PFNWGLGETEXTENSIONSSTRINGARBPROC, wglGetExtensionsStringARB);
	WGLGETPROCNOFAIL(PFNWGLGETEXTENSIONSSTRINGEXTPROC, wglGetExtensionsStringEXT);

	if (wglGetExtensionsStringARB)
	{
		if (wglIsExtensionSupportedARB(hDC, "WGL_ARB_pixel_format"))
		{
			WGLGETPROC( PFNWGLGETPIXELFORMATATTRIBIVARBPROC, wglGetPixelFormatAttribivARB);
			WGLGETPROC( PFNWGLGETPIXELFORMATATTRIBFVARBPROC, wglGetPixelFormatAttribfvARB);
			WGLGETPROC( PFNWGLCHOOSEPIXELFORMATARBPROC,	wglChoosePixelFormatARB);
		}

		if (wglIsExtensionSupportedARB(hDC, "WGL_ARB_pbuffer"))
		{
			WGLGETPROC( PFNWGLCREATEPBUFFERARBPROC, wglCreatePbufferARB);
			WGLGETPROC( PFNWGLGETPBUFFERDCARBPROC, wglGetPbufferDCARB);
			WGLGETPROC( PFNWGLRELEASEPBUFFERDCARBPROC, wglReleasePbufferDCARB);
			WGLGETPROC( PFNWGLDESTROYPBUFFERARBPROC, wglDestroyPbufferARB);
			WGLGETPROC( PFNWGLQUERYPBUFFERARBPROC, wglQueryPbufferARB);
		}

		if (wglIsExtensionSupportedARB(hDC, "WGL_ARB_render_texture"))
		{
			WGLGETPROC( PFNWGLBINDTEXIMAGEARBPROC, wglBindTexImageARB);
			WGLGETPROC( PFNWGLRELEASETEXIMAGEARBPROC, wglReleaseTexImageARB);
			WGLGETPROC( PFNWGLSETPBUFFERATTRIBARBPROC, wglSetPbufferAttribARB);
		}

		if (wglIsExtensionSupportedARB(hDC, "WGL_ARB_multisample"))
		{
			WGLGETPROC( PFNGLSAMPLECOVERAGEARBPROC,	glSampleCoverageARB);
		}
	}
	else
	if (wglGetExtensionsStringEXT)
	{
		if (wglIsExtensionSupported("WGL_EXT_pixel_format"))
		{
			WGLGETPROC( PFNWGLGETPIXELFORMATATTRIBIVEXT, wglGetPixelFormatAttribivEXT);
			WGLGETPROC( PFNWGLGETPIXELFORMATATTRIBFVEXT, wglGetPixelFormatAttribfvEXT);
			WGLGETPROC( PFNWGLCHOOSEPIXELFORMATEXT,	wglChoosePixelFormatEXT);
		}

		if (wglIsExtensionSupported("WGL_EXT_pbuffer"))
		{
			WGLGETPROC( PFNWGLCREATEPBUFFEREXT, wglCreatePbufferEXT);
			WGLGETPROC( PFNWGLGETPBUFFERDCEXT, wglGetPbufferDCEXT);
			WGLGETPROC( PFWGLRELEASEPBUFFERDCEXT, wglReleasePbufferDCEXT);
			WGLGETPROC( PFWGLDESTROYPBUFFEREXT, wglDestroyPbufferEXT);
			WGLGETPROC( PFWGLQUERYPBUFFEREXT, wglQueryPbufferEXT);
		}
	}
	return -1;
}

#endif // __WIN32__

// public extension

GLint wglIsExtensionSupported(const char *extension)
{
	static const GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;
	if (!extension)
	{
		extensions = NULL;
		return 1;
	}
	where = (GLubyte *) strchr(extension, ' ');
	if (where || *extension == '\0')
		return 0;
	if (!extensions)
		extensions = glGetString(GL_EXTENSIONS);
	start = extensions;
	for (;;) {
		where = (GLubyte *) strstr((const char *) start, extension);
		if (!where)
			break;
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ') {
			if (*terminator == ' ' || *terminator == '\0') {
				return 1;
			}
		}
		start = terminator;
	}
	return 0;
}

static void APIENTRY glMultiDrawArrays(GLenum mode,	GLint *first, GLsizei *count, GLsizei primcount)
{
	int i;
	for(i=0; i<primcount; i++) {
	   if (*(count+i)>0) glDrawArrays(mode, *(first+i), *(count+i));
	}

}
static void APIENTRY glMultiDrawElements(GLenum	mode, const	GLsizei	*count,	GLenum type, const GLvoid* *indices, GLsizei primcount)
{
	int i;
	for(i=0; i<primcount; i++) {
		if (*(count+i)>0) glDrawElements(mode, *(count+i), type, *(indices+i));
	}
}

static GLvoid APIENTRY glDrawRangeElements(GLenum mode, GLuint start, GLuint end
				, GLsizei count, GLenum type, const GLvoid *indices)
{
	return glDrawElements(mode, count, type, indices);
}

void wglGetProcAddresses()
{
	// win32
#ifdef _WIN32
	if (wglIsExtensionSupported("WGL_EXT_swap_control"))
	{
		WGLGETPROC( PFNWGLSWAPINTERVALEXTPROC, wglSwapIntervalEXT);
		WGLGETPROC( PFNWGLGETSWAPINTERVALEXTPROC, wglGetSwapIntervalEXT);
	}

	if (wglIsExtensionSupported("GL_WIN_swap_hint"))
	{
		WGLGETPROC( PFNGLADDSWAPHINTRECTWINPROC, glAddSwapHintRectWIN);
	}

#elif defined macintosh
	glGetPointerv(GL_MAC_GET_PROC_ADDRESS_NV, (GLvoid*) &glMacGetProcAddressNV);
	if (!glMacGetProcAddressNV)
	{
		// Bad luck
	}
#endif

	// 3dfx
	if (wglIsExtensionSupported("GL_3DFX_tbuffer"))
	{
		 WGLGETPROC( PGNGLTBUFFERMASK3DFX, glTbufferMask3DFX);
	}


	// ATI
	if (wglIsExtensionSupported("GL_ATI_vertex_array_object"))
	{
         WGLGETPROC( PFNGLNEWOBJECTBUFFERATIPROC, glNewObjectBufferATI);
		 WGLGETPROC( PFNGLISOBJECTBUFFERATIPROC, glIsObjectBufferATI);
		 WGLGETPROC( PFNGLUPDATEOBJECTBUFFERATIPROC, glUpdateObjectBufferATI);
		 WGLGETPROC( PFNGLGETOBJECTBUFFERFVATIPROC, glGetObjectBufferfvATI);
		 WGLGETPROC( PFNGLGETOBJECTBUFFERIVATIPROC, glGetObjectBufferivATI);
		 WGLGETPROC( PFNGLFREEOBJECTBUFFERATIPROC, glFreeObjectBufferATI);
		 WGLGETPROC( PFNGLARRAYOBJECTATIPROC, glArrayObjectATI);
		 WGLGETPROC( PFNGLGETARRAYOBJECTFVATIPROC, glGetArrayObjectfvATI);
		 WGLGETPROC( PFNGLGETARRAYOBJECTIVATIPROC, glGetArrayObjectivATI);
		 WGLGETPROC( PFNGLVARIANTARRAYOBJECTATIPROC, glVariantArrayObjectATI);
		 WGLGETPROC( PFNGLGETVARIANTARRAYOBJECTFVATIPROC, glGetVariantArrayObjectfvATI);
		 WGLGETPROC( PFNGLGETVARIANTARRAYOBJECTIVATIPROC, glGetVariantArrayObjectivATI);

		if (wglIsExtensionSupported("GL_ATI_element_array"))
		{
			WGLGETPROC( PFNGLELEMENTPOINTERATIPROC,	glElementPointerATI);
			WGLGETPROC( PFNGLDRAWELEMENTARRAYATIPROC, glDrawElementArrayATI);
			WGLGETPROC( PFNGLDRAWRANGEELEMENTARRAYATIPROC, glDrawRangeElementArrayATI);
		}

		if (wglIsExtensionSupported("GL_ATI_map_object_buffer"))
		{
			WGLGETPROC( PFNGLMAPOBJECTBUFFERATI, glMapObjectBufferATI);
			WGLGETPROC( PFNGLUNMAPOBJECTBUFFERATI, glUnmapObjectBufferATI);
		}
	}

	if (wglIsExtensionSupported("GL_ATI_fragment_shader"))
	{
	     WGLGETPROC( PFNGLGENFRAGMENTSHADERSATIPROC, glGenFragmentShadersATI);
		 WGLGETPROC( PFNGLBINDFRAGMENTSHADERATIPROC, glBindFragmentShaderATI);
		 WGLGETPROC( PFNGLDELETEFRAGMENTSHADERATIPROC, glDeleteFragmentShaderATI);
		 WGLGETPROC( PFNGLBEGINFRAGMENTSHADERATIPROC, glBeginFragmentShaderATI);
		 WGLGETPROC( PFNGLENDFRAGMENTSHADERATIPROC, glEndFragmentShaderATI);
		 WGLGETPROC( PFNGLPASSTEXCOORDATIPROC, glPassTexCoordATI);
		 WGLGETPROC( PFNGLSAMPLEMAPATIPROC, glSampleMapATI);
		 WGLGETPROC( PFNGLCOLORFRAGMENTOP1ATIPROC, glColorFragmentOp1ATI);
		 WGLGETPROC( PFNGLCOLORFRAGMENTOP2ATIPROC, glColorFragmentOp2ATI);
		 WGLGETPROC( PFNGLCOLORFRAGMENTOP3ATIPROC, glColorFragmentOp3ATI);
		 WGLGETPROC( PFNGLALPHAFRAGMENTOP1ATIPROC, glAlphaFragmentOp1ATI);
		 WGLGETPROC( PFNGLALPHAFRAGMENTOP2ATIPROC, glAlphaFragmentOp2ATI);
		 WGLGETPROC( PFNGLALPHAFRAGMENTOP3ATIPROC, glAlphaFragmentOp3ATI);
		 WGLGETPROC( PFNGLSETFRAGMENTSHADERCONSTANTATIPROC, glSetFragmentShaderConstantATI);
	}

	if (wglIsExtensionSupported("GL_ATI_envmap_bumpmap"))
	{
		WGLGETPROC( PFNGLTEXBUMPPARAMETERIVATIPROC, glTexBumpParameterivATI);
		WGLGETPROC( PFNGLTEXBUMPPARAMETERFVATIPROC, glTexBumpParameterfvATI);
		WGLGETPROC( PFNGLGETTEXBUMPPARAMETERIVATIPROC, glGetTexBumpParameterivATI);
		WGLGETPROC( PFNGLGETTEXBUMPPARAMETERFVATIPROC, glGetTexBumpParameterfvATI);
	}

	if (wglIsExtensionSupported("GL_ATI_pn_triangles"))
	{
		WGLGETPROC( PFNGLPNTRIANGLESIATIPROC, glPNTrianglesiATI);
		WGLGETPROC( PFNGLPNTRIANGLESFATIPROC, glPNTrianglesfATI);
	}

	// EXT
	if (wglIsExtensionSupported("GL_EXT_separate_specular_color"))
	{
		WGLGETPROC( PFNGLSECONDARYCOLOR3UBVEXTPROC, glSecondaryColor3ubvEXT);
	}

	if (wglIsExtensionSupported("GL_EXT_fog_coord"))
	{
		WGLGETPROC ( PFNGLFOGCOORDFEXTPROC, glFogCoordfEXT);
	}

	if (wglIsExtensionSupported("GL_EXT_paletted_texture"))
	{
		WGLGETPROC( PFNGLCOLORTABLEEXTPROC, glColorTableEXT);
	}

#ifndef GL_VERSION_1_2
    if (wglIsExtensionSupported("GL_EXT_compiled_vertex_array"))
	{
        WGLGETPROC( PFNGLLOCKARRAYSEXTPROC,	glLockArraysEXT);
		WGLGETPROC( PFNGLUNLOCKARRAYSEXTPROC, glUnlockArraysEXT);
    }
#endif

	if (wglIsExtensionSupported("GL_EXT_multi_draw_arrays"))
	{
		WGLGETPROC( PFNGLMULTIDRAWARRAYSEXTPROC, glMultiDrawArraysEXT);
		WGLGETPROC( PFNGLMULTIDRAWELEMENTSEXTPROC, glMultiDrawElementsEXT);
	}
	else
	{
		glMultiDrawArraysEXT = glMultiDrawArrays;
		glMultiDrawElementsEXT = glMultiDrawElements;
	}

	if (wglIsExtensionSupported("GL_EXT_draw_range_elements"))
	{
		WGLGETPROC( PFNGLDRAWRANGEELEMENTSEXTPROC, glDrawRangeElementsEXT);
	}
	else
	{
		glDrawRangeElementsEXT = glDrawRangeElements;
	}

	if (wglIsExtensionSupported("GL_EXT_vertex_shader"))
	{
		WGLGETPROC( PFNGLBEGINVERTEXSHADEREXTPROC, glBeginVertexShaderEXT);
		WGLGETPROC( PFNGLENDVERTEXSHADEREXTPROC, glEndVertexShaderEXT);
		WGLGETPROC( PFNGLBINDVERTEXSHADEREXTPROC, glBindVertexShaderEXT);
		WGLGETPROC( PFNGLGENVERTEXSHADERSEXTPROC, glGenVertexShadersEXT);
		WGLGETPROC( PFNGLDELETEVERTEXSHADEREXTPROC, glDeleteVertexShaderEXT);
		WGLGETPROC( PFNGLSHADEROP1EXTPROC, glShaderOp1EXT);
		WGLGETPROC( PFNGLSHADEROP2EXTPROC, glShaderOp2EXT);
		WGLGETPROC( PFNGLSHADEROP3EXTPROC, glShaderOp3EXT);
		WGLGETPROC( PFNGLSWIZZLEEXTPROC, glSwizzleEXT);
		WGLGETPROC( PFNGLWRITEMASKEXTPROC, glWriteMaskEXT);
		WGLGETPROC( PFNGLINSERTCOMPONENTEXTPROC, glInsertComponentEXT);
		WGLGETPROC( PFNGLEXTRACTCOMPONENTEXTPROC, glExtractComponentEXT);
		WGLGETPROC( PFNGLGENSYMBOLSEXTPROC, glGenSymbolsEXT);
		WGLGETPROC( PFNGLSETINVARIANTEXTPROC, glSetInvariantEXT);
		WGLGETPROC( PFNGLSETLOCALCONSTANTEXTPROC, glSetLocalConstantEXT);
		WGLGETPROC( PFNGLVARIANTBVEXTPROC, glVariantbvEXT);
		WGLGETPROC( PFNGLVARIANTSVEXTPROC, glVariantsvEXT);
		WGLGETPROC( PFNGLVARIANTIVEXTPROC, glVariantivEXT);
		WGLGETPROC( PFNGLVARIANTFVEXTPROC, glVariantfvEXT);
		WGLGETPROC( PFNGLVARIANTDVEXTPROC, glVariantdvEXT);
		WGLGETPROC( PFNGLVARIANTUBVEXTPROC, glVariantubvEXT);
		WGLGETPROC( PFNGLVARIANTUSVEXTPROC, glVariantusvEXT);
		WGLGETPROC( PFNGLVARIANTUIVEXTPROC, glVariantuivEXT);
		WGLGETPROC( PFNGLVARIANTPOINTEREXTPROC, glVariantPointerEXT);
		WGLGETPROC( PFNGLENABLEVARIANTCLIENTSTATEEXTPROC, glEnableVariantClientStateEXT);
		WGLGETPROC( PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC, glDisableVariantClientStateEXT);
		WGLGETPROC( PFNGLBINDLIGHTPARAMETEREXTPROC, glBindLightParameterEXT);
		WGLGETPROC( PFNGLBINDMATERIALPARAMETEREXTPROC, glBindMaterialParameterEXT);
		WGLGETPROC( PFNGLBINDTEXGENPARAMETEREXTPROC, glBindTexGenParameterEXT);
		WGLGETPROC( PFNGLBINDTEXTUREUNITPARAMETEREXTPROC, glBindTextureUnitParameterEXT);
		WGLGETPROC( PFNGLBINDPARAMETEREXTPROC, glBindParameterEXT);
		WGLGETPROC( PFNGLISVARIANTENABLEDEXTPROC, glIsVariantEnabledEXT);
		WGLGETPROC( PFNGLGETVARIANTBOOLEANVEXTPROC, glGetVariantBooleanvEXT);
		WGLGETPROC( PFNGLGETVARIANTINTEGERVEXTPROC, glGetVariantIntegervEXT);
		WGLGETPROC( PFNGLGETVARIANTFLOATVEXTPROC, glGetVariantFloatvEXT);
		WGLGETPROC( PFNGLGETVARIANTPOINTERVEXTPROC, glGetVariantPointervEXT);
		WGLGETPROC( PFNGLGETINVARIANTBOOLEANVEXTPROC, glGetInvariantBooleanvEXT);
		WGLGETPROC( PFNGLGETINVARIANTINTEGERVEXTPROC, glGetInvariantIntegervEXT);
		WGLGETPROC( PFNGLGETINVARIANTFLOATVEXTPROC, glGetInvariantFloatvEXT);
		WGLGETPROC( PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC, glGetLocalConstantBooleanvEXT);
		WGLGETPROC( PFNGLGETLOCALCONSTANTINTEGERVEXTPROC, glGetLocalConstantIntegervEXT);
		WGLGETPROC( PFNGLGETLOCALCONSTANTFLOATVEXTPROC, glGetLocalConstantFloatvEXT);
	}

	if (wglIsExtensionSupported("GL_EXT_vertex_weighting"))
	{
		WGLGETPROC( PFNGLVERTEXWEIGHTFEXTPROC, glVertexWeightfEXT);
		WGLGETPROC( PFNGLVERTEXWEIGHTFVEXTPROC,	glVertexWeightfvEXT);
		WGLGETPROC( PFNGLVERTEXWEIGHTPOINTEREXTPROC, glVertexWeightPointerEXT);
	}

	if (wglIsExtensionSupported("GL_EXT_point_parameters"))
	{
		WGLGETPROC( PFNGLPOINTPARAMETERFEXTPROC, glPointParameterfEXT);
		WGLGETPROC( PFNGLPOINTPARAMETERFVEXTPROC, glPointParameterfvEXT);
	}

	// ARB
	if (wglIsExtensionSupported("GL_ARB_multitexture"))
	{
		WGLGETPROC( PFNGLACTIVETEXTUREARBPROC, glActiveTextureARB);
		WGLGETPROC( PFNGLMULTITEXCOORD2FARBPROC, glMultiTexCoord2fARB);
		WGLGETPROC( PFNGLMULTITEXCOORD4FARBPROC, glMultiTexCoord4fARB);
		WGLGETPROC( PFNGLMULTITEXCOORD2FVARBPROC, glMultiTexCoord2fvARB);
		WGLGETPROC( PFNGLMULTITEXCOORD4FVARBPROC, glMultiTexCoord4fvARB);
		WGLGETPROC( PFNGLCLIENTACTIVETEXTUREARBPROC, glClientActiveTextureARB);
	}

	if (wglIsExtensionSupported("GL_ARB_texture_compression"))
	{
		WGLGETPROC( PFNGLCOMPRESSEDTEXIMAGE1DARB, glCompressedTexImage1DARB);
		WGLGETPROC( PFNGLCOMPRESSEDTEXIMAGE2DARB, glCompressedTexImage2DARB);
		WGLGETPROC( PFNGLCOMPRESSEDTEXIMAGE3DARB, glCompressedTexImage3DARB);
		WGLGETPROC( PFNGLGETCOMPRESSEDTEXIMAGEARB, glGetCompressedTexImageARB);
	}

	if (wglIsExtensionSupported("GL_ARB_transpose_matrix"))
	{
		WGLGETPROC( PFNGLLOADTRANSPOSEMATRIXFARB, glLoadTransposeMatrixfARB);
		WGLGETPROC( PFNGLMULTTRANSPOSEMATRIXFARB, glMultTransposeMatrixfARB);
	}

	if (wglIsExtensionSupported("GL_ARB_point_parameters"))
	{
		WGLGETPROC( PFNGLPOINTPARAMETERFARBPROC, glPointParameterfARB);
		WGLGETPROC( PFNGLPOINTPARAMETERFVARBPROC, glPointParameterfvARB);
	}

	if (wglIsExtensionSupported("GL_ARB_vertex_blend"))
	{
		WGLGETPROC( PFNGLWEIGHTBVARBPROC, glWeightbvARB);
		WGLGETPROC( PFNGLWEIGHTSVARBPROC, glWeightsvARB);
		WGLGETPROC( PFNGLWEIGHTIVARBPROC, glWeightivARB);
		WGLGETPROC( PFNGLWEIGHTFVARBPROC, glWeightfvARB);
		WGLGETPROC( PFNGLWEIGHTDVARBPROC, glWeightdvARB);
		WGLGETPROC( PFNGLWEIGHTUBVARBPROC, glWeightubvARB);
		WGLGETPROC( PFNGLWEIGHTUSVARBPROC, glWeightusvARB);
		WGLGETPROC( PFNGLWEIGHTUIVARBPROC, glWeightuivARB);
		WGLGETPROC( PFNGLWEIGHTPOINTERARBPROC, glWeightPointerARB);
		WGLGETPROC( PFNGLVERTEXBLENDARBPROC, glVertexBlendARB);
	}

	if (wglIsExtensionSupported("GL_ARB_vertex_program"))
	{
		WGLGETPROC( PFNGLVERTEXATTRIB1SARBPROC, glVertexAttrib1sARB);
		WGLGETPROC( PFNGLVERTEXATTRIB1FARBPROC, glVertexAttrib1fARB);
		WGLGETPROC( PFNGLVERTEXATTRIB1DARBPROC, glVertexAttrib1dARB);
		WGLGETPROC( PFNGLVERTEXATTRIB2SARBPROC, glVertexAttrib2sARB);
		WGLGETPROC( PFNGLVERTEXATTRIB2FARBPROC, glVertexAttrib2fARB);
		WGLGETPROC( PFNGLVERTEXATTRIB2DARBPROC, glVertexAttrib2dARB);
		WGLGETPROC( PFNGLVERTEXATTRIB3SARBPROC, glVertexAttrib3sARB);
		WGLGETPROC( PFNGLVERTEXATTRIB3FARBPROC, glVertexAttrib3fARB);
		WGLGETPROC( PFNGLVERTEXATTRIB3DARBPROC, glVertexAttrib3dARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4SARBPROC, glVertexAttrib4sARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4FARBPROC, glVertexAttrib4fARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4DARBPROC, glVertexAttrib4dARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4NUBARBPROC, glVertexAttrib4NubARB);
		WGLGETPROC( PFNGLVERTEXATTRIB1SVARBPROC, glVertexAttrib1svARB);
		WGLGETPROC( PFNGLVERTEXATTRIB1FVARBPROC, glVertexAttrib1fvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB1DVARBPROC, glVertexAttrib1dvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB2SVARBPROC, glVertexAttrib2svARB);
		WGLGETPROC( PFNGLVERTEXATTRIB2FVARBPROC, glVertexAttrib2fvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB2DVARBPROC, glVertexAttrib2dvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB3SVARBPROC, glVertexAttrib3svARB);
		WGLGETPROC( PFNGLVERTEXATTRIB3FVARBPROC, glVertexAttrib3fvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB3DVARBPROC, glVertexAttrib3dvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4BVARBPROC, glVertexAttrib4bvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4SVARBPROC, glVertexAttrib4svARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4IVARBPROC, glVertexAttrib4ivARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4UBVARBPROC, glVertexAttrib4ubvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4USVARBPROC, glVertexAttrib4usvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4UIVARBPROC, glVertexAttrib4uivARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4FVARBPROC, glVertexAttrib4fvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4DVARBPROC, glVertexAttrib4dvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4NBVARBPROC, glVertexAttrib4NbvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4NSVARBPROC, glVertexAttrib4NsvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4NIVARBPROC, glVertexAttrib4NivARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4NUBVARBPROC, glVertexAttrib4NubvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4NUSVARBPROC, glVertexAttrib4NusvARB);
		WGLGETPROC( PFNGLVERTEXATTRIB4NUIVARBPROC, glVertexAttrib4NuivARB);
		WGLGETPROC( PFNGLVERTEXATTRIBPOINTERARBPROC, glVertexAttribPointerARB);
		WGLGETPROC( PFNGLENABLEVERTEXATTRIBARRAYARBPROC, glEnableVertexAttribArrayARB);
		WGLGETPROC( PFNGLDISABLEVERTEXATTRIBARRAYARBPROC, glDisableVertexAttribArrayARB);
		WGLGETPROC( PFNGLPROGRAMSTRINGARBPROC, glProgramStringARB);
		WGLGETPROC( PFNGLBINDPROGRAMARBPROC, glBindProgramARB);
		WGLGETPROC( PFNGLDELETEPROGRAMSARBPROC, glDeleteProgramsARB);
		WGLGETPROC( PFNGLGENPROGRAMSARBPROC, glGenProgramsARB);
		WGLGETPROC( PFNGLPROGRAMENVPARAMETER4FARBPROC, glProgramEnvParameter4fARB);
		WGLGETPROC( PFNGLPROGRAMENVPARAMETER4DARBPROC, glProgramEnvParameter4dARB);
		WGLGETPROC( PFNGLPROGRAMENVPARAMETER4FVARBPROC, glProgramEnvParameter4fvARB);
		WGLGETPROC( PFNGLPROGRAMENVPARAMETER4DVARBPROC, glProgramEnvParameter4dvARB);
		WGLGETPROC( PFNGLPROGRAMLOCALPARAMETER4FARBPROC, glProgramLocalParameter4fARB);
		WGLGETPROC( PFNGLPROGRAMLOCALPARAMETER4DARBPROC, glProgramLocalParameter4dARB);
		WGLGETPROC( PFNGLPROGRAMLOCALPARAMETER4FVARBPROC, glProgramLocalParameter4fvARB);
		WGLGETPROC( PFNGLPROGRAMLOCALPARAMETER4DVARBPROC, glProgramLocalParameter4dvARB);
		WGLGETPROC( PFNGLGETPROGRAMENVPARAMETERFVARBPROC, glGetProgramEnvParameterfvARB);
		WGLGETPROC( PFNGLGETPROGRAMENVPARAMETERDVARBPROC, glGetProgramEnvParameterdvARB);
		WGLGETPROC( PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC, glGetProgramLocalParameterfvARB);
		WGLGETPROC( PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC, glGetProgramLocalParameterdvARB);
		WGLGETPROC( PFNGLGETPROGRAMIVARBPROC, glGetProgramivARB);
		WGLGETPROC( PFNGLGETPROGRAMSTRINGARBPROC, glGetProgramStringARB);
		WGLGETPROC( PFNGLGETVERTEXATTRIBDVARBPROC, glGetVertexAttribdvARB);
		WGLGETPROC( PFNGLGETVERTEXATTRIBFVARBPROC, glGetVertexAttribfvARB);
		WGLGETPROC( PFNGLGETVERTEXATTRIBIVARBPROC, glGetVertexAttribivARB);
		WGLGETPROC( PFNGLGETVERTEXATTRIBPOINTERVARBPROC, glGetVertexAttribPointervARB);
		WGLGETPROC( PFNGLISPROGRAMARBPROC, glIsProgramARB);
	}

	if (wglIsExtensionSupported("GL_ARB_vertex_buffer_object"))
	{
		WGLGETPROC( PFNBINDBUFFERARB,glBindBufferARB);
		WGLGETPROC( PFNDELETEBUFFERSARB, glDeleteBuffersARB);
		WGLGETPROC( PFNGENBUFFERSARB, glGenBuffersARB);
		WGLGETPROC( PFNISBUFFERARB, glIsBufferARB);
		WGLGETPROC( PFNBUFFERDATAARB, glBufferDataARB);
		WGLGETPROC( PFNBUFFERSUBDATAARB, glBufferSubDataARB);
		WGLGETPROC( PFNGETBUFFERSUBDATAARB, glGetBufferSubDataARB);
		WGLGETPROC( PFNMAPBUFFERARB, glMapBufferARB);
		WGLGETPROC( PFNUNMAPBUFFERARB, glUnmapBufferARB);
		WGLGETPROC( PFNGETBUFFERPARAMETERIVARB, glGetBufferParameterivARB);
		WGLGETPROC( PFNGETBUFFERPOINTERVARB, glGetBufferPointervARB);
	}

	// NV
	if (wglIsExtensionSupported("GL_NV_vertex_program"))
	{
		WGLGETPROC( PFNGLBINDPROGRAMNVPROC, glBindProgramNV);
		WGLGETPROC( PFNGLDELETEPROGRAMSNVPROC, glDeleteProgramsNV);
		WGLGETPROC( PFNGLEXECUTEPROGRAMNVPROC, glExecuteProgramNV);
		WGLGETPROC( PFNGLGENPROGRAMSNVPROC, glGenProgramsNV);
		WGLGETPROC( PFNGLAREPROGRAMSRESIDENTNVPROC, glAreProgramsResidentNV);
		WGLGETPROC( PFNGLREQUESTRESIDENTPROGRAMSNVPROC, glRequestResidentProgramsNV);
		WGLGETPROC( PFNGLGETPROGRAMPARAMETERFVNVPROC, glGetProgramParameterfvNV);
		WGLGETPROC( PFNGLGETPROGRAMPARAMETERDVNVPROC, glGetProgramParameterdvNV);
		WGLGETPROC( PFNGLGETPROGRAMIVNVPROC, glGetProgramivNV);
		WGLGETPROC( PFNGLGETPROGRAMSTRINGNVPROC, glGetProgramStringNV);
		WGLGETPROC( PFNGLGETTRACKMATRIXIVNVPROC, glGetTrackMatrixivNV);
		WGLGETPROC( PFNGLGETVERTEXATTRIBDVNVPROC, glGetVertexAttribdvNV);
		WGLGETPROC( PFNGLGETVERTEXATTRIBFVNVPROC, glGetVertexAttribfvNV);
		WGLGETPROC( PFNGLGETVERTEXATTRIBIVNVPROC, glGetVertexAttribivNV);
		WGLGETPROC( PFNGLGETVERTEXATTRIBPOINTERVNVPROC, glGetVertexAttribPointervNV);
		WGLGETPROC( PFNGLISPROGRAMNVPROC, glIsProgramNV);
		WGLGETPROC( PFNGLLOADPROGRAMNVPROC, glLoadProgramNV);
		WGLGETPROC( PFNGLPROGRAMPARAMETER4FNVPROC, glProgramParameter4fNV);
		WGLGETPROC( PFNGLPROGRAMPARAMETER4DNVPROC, glProgramParameter4dNV);
		WGLGETPROC( PFNGLPROGRAMPARAMETER4DVNVPROC, glProgramParameter4dvNV);
		WGLGETPROC( PFNGLPROGRAMPARAMETER4FVNVPROC, glProgramParameter4fvNV);
		WGLGETPROC( PFNGLPROGRAMPARAMETERS4DVNVPROC, glProgramParameters4dvNV);
		WGLGETPROC( PFNGLPROGRAMPARAMETERS4FVNVPROC, glProgramParameters4fvNV);
		WGLGETPROC( PFNGLTRACKMATRIXNVPROC, glTrackMatrixNV);
		WGLGETPROC( PFNGLVERTEXATTRIBPOINTERNVPROC, glVertexAttribPointerNV);
		WGLGETPROC( PFNGLVERTEXATTRIB1FNVPROC, glVertexAttrib1fNV);
		WGLGETPROC( PFNGLVERTEXATTRIB2FNVPROC, glVertexAttrib2fNV);
		WGLGETPROC( PFNGLVERTEXATTRIB3FNVPROC, glVertexAttrib3fNV);
		WGLGETPROC( PFNGLVERTEXATTRIB4FNVPROC, glVertexAttrib4fNV);
		WGLGETPROC( PFNGLVERTEXATTRIB4UBNVPROC,	glVertexAttrib4ubNV);
		WGLGETPROC( PFNGLVERTEXATTRIB1FVNVPROC, glVertexAttrib1fvNV);
		WGLGETPROC( PFNGLVERTEXATTRIB2FVNVPROC,	glVertexAttrib2fvNV);
		WGLGETPROC( PFNGLVERTEXATTRIB3FVNVPROC,	glVertexAttrib3fvNV);
		WGLGETPROC( PFNGLVERTEXATTRIB4FVNVPROC,	glVertexAttrib4fvNV);
		WGLGETPROC( PFNGLVERTEXATTRIB4UBVNVPROC, glVertexAttrib4ubvNV);
		WGLGETPROC( PFNGLVERTEXATTRIBS1FVNVPROC, glVertexAttribs1fvNV);
		WGLGETPROC( PFNGLVERTEXATTRIBS2FVNVPROC, glVertexAttribs2fvNV);
		WGLGETPROC( PFNGLVERTEXATTRIBS3FVNVPROC, glVertexAttribs3fvNV);
		WGLGETPROC( PFNGLVERTEXATTRIBS4FVNVPROC, glVertexAttribs4fvNV);
		WGLGETPROC( PFNGLVERTEXATTRIBS4UBVNVPROC, glVertexAttribs4ubvNV);
	}

	if (wglIsExtensionSupported("GL_NV_register_combiners"))
	{
		WGLGETPROC( PFNGLCOMBINERPARAMETERFVNVPROC, glCombinerParameterfvNV);
		WGLGETPROC( PFNGLCOMBINERPARAMETERIVNVPROC, glCombinerParameterivNV);
		WGLGETPROC( PFNGLCOMBINERPARAMETERFNVPROC, glCombinerParameterfNV);
		WGLGETPROC( PFNGLCOMBINERPARAMETERINVPROC, glCombinerParameteriNV);
		WGLGETPROC( PFNGLCOMBINERINPUTNVPROC, glCombinerInputNV);
		WGLGETPROC( PFNGLCOMBINEROUTPUTNVPROC, glCombinerOutputNV);
		WGLGETPROC( PFNGLFINALCOMBINERINPUTNVPROC, glFinalCombinerInputNV);
		WGLGETPROC( PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC, glGetCombinerInputParameterfvNV);
		WGLGETPROC( PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC, glGetCombinerInputParameterivNV);
		WGLGETPROC( PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC, glGetCombinerOutputParameterfvNV);
		WGLGETPROC( PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC, glGetCombinerOutputParameterivNV);
		WGLGETPROC( PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC, glGetFinalCombinerInputParameterfvNV);
		WGLGETPROC( PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC, glGetFinalCombinerInputParameterivNV);
	}

    if (wglIsExtensionSupported("GL_NV_vertex_array_range"))
	{
		WGLGETPROC( PFNGLFLUSHVERTEXARRAYRANGENVPROC, glFlushVertexArrayRangeNV);
		WGLGETPROC( PFNGLVERTEXARRAYRANGENVPROC, glVertexArrayRangeNV);
		WGLGETPROC( PFNWGLALLOCATEMEMORYNVPROC, wglAllocateMemoryNV);
		WGLGETPROC( PFNWGLFREEMEMORYNVPROC, wglFreeMemoryNV);

		if (wglIsExtensionSupported("GL_NV_fence"))
		{
			WGLGETPROC( PFNGLGENFENCESNVPROC, glGenFencesNV);
			WGLGETPROC( PFNGLDELETEFENCESNVPROC, glDeleteFencesNV);
			WGLGETPROC( PFNGLSETFENCENVPROC, glSetFenceNV);
			WGLGETPROC( PFNGLTESTFENCENVPROC, glTestFenceNV);
			WGLGETPROC( PFNGLFINISHFENCENVPROC,	glFinishFenceNV);
			WGLGETPROC( PFNGLISFENCENVPROC,	glIsFenceNV);
			WGLGETPROC( PFNGLGETFENCEIVNVPROC, glGetFenceivNV);
		}
	}
	if (wglIsExtensionSupported("GL_NV_occlusion_query"))
	{
		WGLGETPROC( PFNGLGENOCCLUSIONQUERIESNV, glGenOcclusionQueriesNV);
		WGLGETPROC( PFNGLDELETEOCCLUSIONQUERIESNV, glDeleteOcclusionQueriesNV);
		WGLGETPROC( PFNGLISOCCLUSIONQUERYNV, glIsOcclusionQueryNV);
		WGLGETPROC( PFNGLBEGINOCCLUSIONQUERYNV, glBeginOcclusionQueryNV);
		WGLGETPROC( PFNGLENDOCCLUSIONQUERYNV, glEndOcclusionQueryNV);
		WGLGETPROC( PFNGLGETOCCLUSIONQUERYIVNV, glGetOcclusionQueryivNV);
		WGLGETPROC( PFNGLGETOCCLUSIONQUERYUIVNV, glGetOcclusionQueryuivNV);
	}

	if (wglIsExtensionSupported("GL_ARB_shader_objects"))
	{
		WGLGETPROC( PFNGLCREATEPROGRAMOBJECTARBPROC, glCreateProgramObjectARB);
		WGLGETPROC( PFNGLCREATESHADEROBJECTARBPROC, glCreateShaderObjectARB);
		WGLGETPROC( PFNGLDELETEOBJECTARBPROC, glDeleteObjectARB);
		WGLGETPROC( PFNGLDETACHOBJECTARBPROC, glDetachObjectARB);
		WGLGETPROC( PFNGLATTACHOBJECTARBPROC, glAttachObjectARB);

		WGLGETPROC( PFNGLSHADERSOURCEARBPROC, glShaderSourceARB);
		WGLGETPROC( PFNGLCOMPILESHADERARBPROC, glCompileShaderARB);
		WGLGETPROC( PFNGLLINKPROGRAMARBPROC, glLinkProgramARB);
		WGLGETPROC( PFNGLGETINFOLOGARBPROC, glGetInfoLogARB);
		WGLGETPROC( PFNGLUSEPROGRAMOBJECTARBPROC, glUseProgramObjectARB);

		WGLGETPROC( PFNGLGETOBJECTPARAMETERIVARBPROC, glGetObjectParameterivARB);
		WGLGETPROC( PFNGLGETOBJECTPARAMETERFVARBPROC, glGetObjectParameterfvARB);
		WGLGETPROC( PFNGLGETUNIFORMLOCATIONARBPROC, glGetUniformLocationARB);

		WGLGETPROC( PFNGLUNIFORM1FARBPROC, glUniform1fARB);
		WGLGETPROC( PFNGLUNIFORM2FARBPROC, glUniform2fARB);
		WGLGETPROC( PFNGLUNIFORM3FARBPROC, glUniform3fARB);
		WGLGETPROC( PFNGLUNIFORM4FARBPROC, glUniform4fARB);

		WGLGETPROC( PFNGLUNIFORM1IARBPROC, glUniform1iARB);
		WGLGETPROC( PFNGLUNIFORM2IARBPROC, glUniform2iARB);
		WGLGETPROC( PFNGLUNIFORM3IARBPROC, glUniform3iARB);
		WGLGETPROC( PFNGLUNIFORM4IARBPROC, glUniform4iARB);

		WGLGETPROC( PFNGLUNIFORM1FVARBPROC, glUniform1fvARB);
		WGLGETPROC( PFNGLUNIFORM2FVARBPROC, glUniform2fvARB);
		WGLGETPROC( PFNGLUNIFORM3FVARBPROC, glUniform3fvARB);
		WGLGETPROC( PFNGLUNIFORM4FVARBPROC, glUniform4fvARB);

		WGLGETPROC( PFNGLUNIFORM1IVARBPROC, glUniform1ivARB);
		WGLGETPROC( PFNGLUNIFORM2IVARBPROC, glUniform2ivARB);
		WGLGETPROC( PFNGLUNIFORM3IVARBPROC, glUniform3ivARB);
		WGLGETPROC( PFNGLUNIFORM4IVARBPROC, glUniform4ivARB);
	}
}

