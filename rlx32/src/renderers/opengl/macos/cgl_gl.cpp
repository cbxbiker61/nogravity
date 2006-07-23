//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2004 - realtech VR

This file is part of Space Girl 1.9

Space Girl is free software; you can redistribute it and/or
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

#include "_rlx32.h"
#include "_rlx.h"

#include "systools.h"
#include "gx_struc.h"
#include "gx_init.h"
#include "gx_csp.h"
#include "gx_rgb.h"
#include "v3xdefs.h"
#include "v3x_2.h"
#include "v3xrend.h"
#include "../gl_v3x.h"
#include <OpenGL/OpenGL.h>


WindowPtr						g_hWnd;
struct RLXSYSTEM 	      	*	g_pRLX;

// GL Driver Specific
int32_t							gl_pRect[4];
static int           			gl_lx,
								gl_ly,
								gl_bpp;

CGLContextObj		 			g_pCGLC;

// CGL
static CFArrayRef				g_cgDisplayModeList;
static CFDictionaryRef			g_cgDisplayMode;
static CGDirectDisplayID		g_cgDisplayID;
static CFDictionaryRef			g_cgPrevDisplayMode;
static int						g_cgOldDisplayModeRestore;

static int32_t getDictLong (CFDictionaryRef refDict, CFStringRef key)
{
	int32_t int_value;
	CFNumberRef number_value = (CFNumberRef) CFDictionaryGetValue(refDict, key);
	if (!number_value) // if can't get a number for the dictionary
		return -1;  // fail
	if (!CFNumberGetValue(number_value, kCFNumberLongType, &int_value)) // or if cant convert it
		return -1; // fail
	return int_value; // otherwise return the int32_t value
}


#ifdef _DEBUG
int _SYS_CGLTRACE(int err, int line)
{
	/*if (!err)
		CGLReportError (CGLGetError());
	if (err!=noErr)
        printf("%s(%d) : %s\n", __FILE__, line, CGLErrorString(CGLGetError()));
    */
	return err;
}
#define SYS_CGLTRACE(err) _SYS_CGLTRACE(err, __LINE__)
#else
#define SYS_CGLTRACE(err) err
#endif

static void RLXAPI Flip(void)
{
   	CGLFlushDrawable(g_pCGLC);
    return;
}

static u_int8_t RLXAPI *Lock(void)
{	
    return NULL;
}

static void RLXAPI Unlock(void)
{
    return;
}

static GXDISPLAYMODEHANDLE ModeHandleToDictRef(CFDictionaryRef ref)
{
	int n = g_cgDisplayModeList ? CFArrayGetCount (g_cgDisplayModeList) : 0;
	int j;
	for(j = 0; j < n; j++)  
	{
		CFDictionaryRef modeDict = (CFDictionaryRef) CFArrayGetValueAtIndex (g_cgDisplayModeList, j);
		if (ref == modeDict)
			return j + 1;

	}
	return 0;
}

static CFDictionaryRef DictRefToModeHandle(GXDISPLAYMODEHANDLE mode)
{
	int n = g_cgDisplayModeList ? CFArrayGetCount (g_cgDisplayModeList) : 0;
	int j;
	for(j = 0; j < n; j++)  
	{
		CFDictionaryRef modeDict = (CFDictionaryRef) CFArrayGetValueAtIndex (g_cgDisplayModeList, j);
		if (j + 1 == mode)
			return modeDict;
	}
	return 0;
}

static GXDISPLAYMODEINFO RLXAPI *EnumDisplayList(int bpp)
{
	int n = CFArrayGetCount (g_cgDisplayModeList);
	int k = 0;
	int j;
	GXDISPLAYMODEINFO *drv = (GXDISPLAYMODEINFO *)g_pRLX->mm_heap->malloc(sizeof(GXDISPLAYMODEINFO) * (n+1));
	
	for(j = 0; j < n; j++)  
	{
		CFDictionaryRef modeDict = (CFDictionaryRef) CFArrayGetValueAtIndex (g_cgDisplayModeList, j);
		int thisBpp = getDictLong (modeDict,  kCGDisplayBitsPerPixel);
		if ((bpp == -1) || (bpp == thisBpp))
		{
			drv[k].lHeight = getDictLong (modeDict,  kCGDisplayHeight);
			drv[k].lWidth = getDictLong (modeDict,  kCGDisplayWidth);
			drv[k].BitsPerPixel = thisBpp;
			drv[k].mode = j;
			k++;
		}
	}
	
	drv[k].BitsPerPixel = 0;

    return drv;
}

extern GXGRAPHICINTERFACE GI_OpenGL;
extern GXSPRITEINTERFACE CSP_OpenGL;

static void RLXAPI SetPrimitive()
{
	g_pRLX->pGX->View.Flip = Flip;
	g_pRLX->pGX->gi = GI_OpenGL;
	g_pRLX->pGX->csp = CSP_OpenGL;
	
	g_pRLX->pGX->csp_cfg.put.fonct = g_pRLX->pGX->csp.put;
    g_pRLX->pGX->csp_cfg.pset.fonct = g_pRLX->pGX->csp.pset;
    g_pRLX->pGX->csp_cfg.transp.fonct = g_pRLX->pGX->csp.Trsp50;
    g_pRLX->pGX->csp_cfg.op = g_pRLX->pGX->csp.put;
}

static void RLXAPI GetDisplayInfo(GXDISPLAYMODEHANDLE mode)
{
	g_pRLX->pfSetViewPort(&g_pRLX->pGX->View, gl_lx, gl_ly, gl_bpp);
	g_pRLX->pGX->View.ColorMask.RedMaskSize			= 8;
	g_pRLX->pGX->View.ColorMask.GreenMaskSize 		= 8;
	g_pRLX->pGX->View.ColorMask.BlueMaskSize		= 8;
	g_pRLX->pGX->View.ColorMask.RsvdMaskSize		= 8;
#ifdef __BIG_ENDIAN__
	g_pRLX->pGX->View.ColorMask.RedFieldPosition	= 0;
	g_pRLX->pGX->View.ColorMask.GreenFieldPosition  = 8;
	g_pRLX->pGX->View.ColorMask.BlueFieldPosition   = 16;
#else
	g_pRLX->pGX->View.ColorMask.RedFieldPosition	= 16;
	g_pRLX->pGX->View.ColorMask.GreenFieldPosition  = 8;
	g_pRLX->pGX->View.ColorMask.BlueFieldPosition   = 0;
#endif
	g_pRLX->pGX->View.ColorMask.RsvdFieldPosition	= 24;
	SetPrimitive();
	GL_FakeViewPort();
	UNUSED(mode);
    return;
}

static int RLXAPI SetDisplayMode(GXDISPLAYMODEHANDLE mode)
{	
	if ((g_pRLX->Video.Config & RLXVIDEO_Windowed)==0)
	{
		if (!g_cgOldDisplayModeRestore)
		{
			CGDisplayCapture(g_cgDisplayID);
		}
		CGDisplaySwitchToMode (g_cgDisplayID, DictRefToModeHandle(mode));
		g_cgOldDisplayModeRestore = 1;
	}
	return 0;
}

static GXDISPLAYMODEHANDLE RLXAPI SearchDisplayMode(int lx, int ly, int bpp)
{
    gl_lx  = lx;
    gl_ly  = ly;
    gl_bpp = bpp == -1 ? (*(*GetMainDevice())->gdPMap)->pixelSize : bpp;

	if ((g_pRLX->Video.Config & RLXVIDEO_Windowed)==0)
	{
		boolean_t ret = false;
		g_cgDisplayMode = CGDisplayBestModeForParameters(g_cgDisplayID, bpp, lx, ly, &ret);
		return ModeHandleToDictRef(g_cgDisplayMode);
	}
    return 1;
}


struct PixelFormatDesc
{
	CGLPixelFormatAttribute value;
	const char *text;
};

static void DEBUG_PIXEL_FORMAT(CGLPixelFormatObj pix)
{
	const struct PixelFormatDesc p[] = {	
   {kCGLPFAAllRenderers, "All renderers"},
   {kCGLPFADoubleBuffer, "Double buffer"},
   {kCGLPFAStereo, "Stereo"},
   {kCGLPFAAuxBuffers, "Aux Buffers"},
   {kCGLPFAColorSize, "Color Size"},
   {kCGLPFAAlphaSize, "Alpha Size"},
   {kCGLPFADepthSize, "Depth Size"},
   {kCGLPFAStencilSize, "Stencil Size"},
   {kCGLPFAAccumSize, "Accum Size"},
   {kCGLPFAMinimumPolicy, "Minimum Policy"},
   {kCGLPFAMaximumPolicy, "Maximum Policy"},
   {kCGLPFAOffScreen, "Off Screen"},
   {kCGLPFAFullScreen, "Full Screen"},
   {kCGLPFASampleBuffers, "Sample Buffers"},
   {kCGLPFASamples, "Samples"} ,
   {kCGLPFAAuxDepthStencil, "Depth Stencil"},
   {kCGLPFAColorFloat, "Color Float"},
   {kCGLPFAMultisample, "Multi Sample"},
   {kCGLPFASupersample, "Super Sample"},
   {kCGLPFASampleAlpha, "Sample Alpha"},
   {kCGLPFARendererID, "RendererID"},
   {kCGLPFASingleRenderer, "Single Renderer"},
   {kCGLPFANoRecovery, "No Recovery"},
   {kCGLPFAAccelerated, "Accelerated"},
   {kCGLPFAClosestPolicy, "Closest Policy"},
   {kCGLPFARobust, "Robust"},
   {kCGLPFABackingStore, "Backing Store"},
   {kCGLPFAMPSafe, "MPSafe"},
   {kCGLPFAWindow, "Window"},
   {kCGLPFAMultiScreen, "Multiscreen"},
   {kCGLPFACompliant, "Compliant"},
   {kCGLPFADisplayMask, "Display Mask"},
   {kCGLPFAPBuffer, "PBuffer"},
   {kCGLPFARemotePBuffer, "Remote PBuffer"},
   {kCGLPFAVirtualScreenCount, "Accum Size"},
   {(CGLPixelFormatAttribute)0,0}
   };
   int i = 0;
   while (p[i].value)
   {
		long value;
		CGLDescribePixelFormat(pix, 0, p[i].value, &value);
		if (p[i].value == kCGLPFARendererID)
		printf("... %s: 0x%08lx\n", p[i].text, value);
		else
		printf("... %s: %ld\n", p[i].text, value);
		i++;
	}

}
static int RLXAPI CreateSurface(int numberOfSparePages)
{
    CGOpenGLDisplayMask displayMask = CGDisplayIDToOpenGLDisplayMask( g_cgDisplayID ) ;
    static CGLPixelFormatAttribute attribs[32];
   	CGLPixelFormatAttribute	*pAttrib =  attribs;
    CGLPixelFormatObj pixelFormatObj ;
    long numPixelFormats = 0;

	*pAttrib = kCGLPFAFullScreen; pAttrib++;
	*pAttrib = kCGLPFASingleRenderer; pAttrib++; 

	*pAttrib = kCGLPFADisplayMask; pAttrib++;
	*pAttrib = (CGLPixelFormatAttribute)displayMask; pAttrib++;

	*pAttrib = kCGLPFADoubleBuffer; pAttrib++;
	*pAttrib = kCGLPFAColorSize; pAttrib++;
	*pAttrib = (CGLPixelFormatAttribute)gl_bpp; pAttrib++;
	
	 if ((g_pRLX->pGX->View.Flags & GX_CAPS_MULTISAMPLING))
	{
		*pAttrib = kCGLPFASampleBuffers; pAttrib++;
		*pAttrib = (CGLPixelFormatAttribute)1;  pAttrib++; 

		*pAttrib = kCGLPFASamples;  pAttrib++;
		*pAttrib = (CGLPixelFormatAttribute)g_pRLX->pGX->View.Multisampling; pAttrib++;
	}

	if (SYS_CGLTRACE(CGLChoosePixelFormat( attribs, &pixelFormatObj, &numPixelFormats )))
    	return -1;

    if (!numPixelFormats)
        return -2;

	DEBUG_PIXEL_FORMAT(pixelFormatObj);

    if (SYS_CGLTRACE(CGLCreateContext( pixelFormatObj, NULL, &g_pCGLC )))
       return -3;

    CGLDestroyPixelFormat( pixelFormatObj ) ;

    long swapInterval = !!(g_pRLX->pGX->View.Flags & GX_CAPS_VSYNC);
    SYS_CGLTRACE(CGLSetParameter( g_pCGLC, kCGLCPSwapInterval, &swapInterval));
    SYS_CGLTRACE(CGLSetCurrentContext( g_pCGLC ));
    if (SYS_CGLTRACE(CGLSetFullScreen( g_pCGLC )))
	{
		CGDisplaySwitchToMode(g_cgDisplayID, g_cgPrevDisplayMode);
		CGReleaseAllDisplays();
	
		return -5;
	}

	HideMenuBar();

    // Reset engine
    GL_InstallExtensions();
	GL_ResetViewport();

	g_pRLX->pGX->Surfaces.maxSurface = numberOfSparePages;;

	if (g_pRLX->pGX->View.Flags & GX_CAPS_MULTISAMPLING)
	{
		glEnable(GL_MULTISAMPLE_ARB);
	}

    return 0;
}

static void RLXAPI ReleaseSurfaces(void)
{
	if (!g_pCGLC)
		return;

	CGLSetCurrentContext( NULL );
    CGLClearDrawable( g_pCGLC );
    CGLDestroyContext( g_pCGLC );

    g_pCGLC = 0;
	g_pRLX->pGX->Surfaces.maxSurface = 0;
    return;
}

static int RLXAPI RegisterMode(GXDISPLAYMODEHANDLE mode)
{
    g_pRLX->pGX->View.DisplayMode = (uint16_t)mode;
    g_pRLX->pGX->Client->GetDisplayInfo(mode);	
	return g_pRLX->pGX->Client->SetDisplayMode(mode);
}

static void RLXAPI Shutdown(void)
{
	if (!g_pCGLC)
		return;

	CGDisplaySwitchToMode(g_cgDisplayID, g_cgPrevDisplayMode);
	CGReleaseAllDisplays();
	ShowMenuBar();

    return;
}

static int Open(void * hwnd)
{
    CGLRendererInfoObj renderer;
    long numRenderer;
	CGDirectDisplayID l[32];
	CGDisplayCount count;

	SYS_ASSERT(g_pCGLC == 0);
	UNUSED(hwnd);
	CGGetActiveDisplayList (sizeof(l), l, &count);

#ifdef _DEBUG
	// Debug in multiple monitor. Use the secondary monitor for rendering
	g_cgDisplayID = l[count-1];
#else
	g_cgDisplayID = CGMainDisplayID ();
#endif

	g_cgPrevDisplayMode = CGDisplayCurrentMode (g_cgDisplayID);
	g_cgDisplayModeList = CGDisplayAvailableModes (g_cgDisplayID);

    CGLQueryRendererInfo(CGDisplayIDToOpenGLDisplayMask(g_cgDisplayID), &renderer, &numRenderer);
    CGLDestroyRendererInfo(renderer);

    return 0;
}

static unsigned NotifyEvent(enum GX_EVENT_MODE mode, int x, int y)
{
    UNUSED(mode);
    UNUSED(x);
    UNUSED(y);
    return mode;
}

GXCLIENTDRIVER GX_OpenGL = {
    Lock,
    Unlock,
    EnumDisplayList,
    GetDisplayInfo,
    SetDisplayMode,
    SearchDisplayMode,
    CreateSurface,
    ReleaseSurfaces,
    NULL,
    NULL,
    NULL,
    RegisterMode,
    Shutdown,
    Open,
    NotifyEvent,    
    "OpenGL/CGL",
	0x10 // ARGB
};

extern void SetPrimitiveSprites();

_RLXEXPORTFUNC void RLXAPI GX_EntryPoint(struct RLXSYSTEM *p)
{
    g_pRLX = p;
	SetPrimitiveSprites();
	g_pRLX->pGX->Client = &GX_OpenGL;
    return;
}

extern V3X_GXSystem V3X_OpenGL;

// _V3XEXPORTUNC 
void RLXAPI V3X_EntryPoint(struct RLXSYSTEM *p)
{
	GX_EntryPoint(p);
    g_pRLX->pV3X->Client = &V3X_OpenGL;
    return;
}
