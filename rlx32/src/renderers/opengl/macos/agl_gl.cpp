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



WindowPtr						g_hWnd;
struct RLXSYSTEM 		*	g_pRLX;

// GL Driver Specific
int32_t							gl_pRect[4];
static int           			gl_lx,
								gl_ly,
								gl_bpp;

AGLContext			 			g_pAGLC;
static Rect					*	g_pRect;

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
int _SYS_AGLTRACE(int err, int line)
{
	/*if (!err)
		aglReportError (aglGetError());
	*/
    if (err!=noErr)
        printf("%s(%d) : %s\n", __FILE__, line, aglErrorString(err));
    return err;
}
#define SYS_AGLTRACE(err) _SYS_AGLTRACE(err, __LINE__)
#else
#define SYS_AGLTRACE(err) err
#endif

static void CenterWindow(WindowRef ref)
{
    Rect rect, bounds;
    GetWindowPortBounds(ref, &rect);
    GetRegionBounds(GetGrayRgn(), &bounds);
    int lx = bounds.right - bounds.left;
    int ly = bounds.bottom - bounds.top;
    rect.left = bounds.left + ((lx - (rect.right - rect.left))/2);
    rect.top = bounds.top + ((ly - (rect.bottom - rect.top))/2);
    MoveWindow(ref, rect.left, rect.top, true);
}

static void RLXAPI Flip(void)
{
    glFinish();
   	aglSwapBuffers(g_pAGLC);
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
	g_pRLX->pGX->View.ColorMask.RedMaskSize		= 8;
	g_pRLX->pGX->View.ColorMask.GreenMaskSize 	= 8;
	g_pRLX->pGX->View.ColorMask.BlueMaskSize		= 8;
	g_pRLX->pGX->View.ColorMask.RsvdMaskSize		= 8;
	g_pRLX->pGX->View.ColorMask.RedFieldPosition	= 0;
	g_pRLX->pGX->View.ColorMask.GreenFieldPosition = 8;
	g_pRLX->pGX->View.ColorMask.BlueFieldPosition  = 16;
	g_pRLX->pGX->View.ColorMask.RsvdFieldPosition  = 24;
	SetPrimitive();
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
		boolean_t ret;
		g_cgDisplayMode = CGDisplayBestModeForParameters(g_cgDisplayID, bpp, lx, ly, &ret);
		return ModeHandleToDictRef(g_cgDisplayMode);
	}
    return 1;
}

static int RLXAPI CreateSurface(int numberOfSparePages)
{
    g_pRLX->pGX->View.lpBackBuffer   = NULL;
	GLint          attrib[32];
	GLint		*pAttrib =  attrib;

	*pAttrib = AGL_RGBA; pAttrib++;
	*pAttrib = AGL_NO_RECOVERY; pAttrib++;
	
	if (!(g_pRLX->Video.Config & RLXVIDEO_Windowed))
	{
		*pAttrib = AGL_FULLSCREEN; pAttrib++;
	}

	if (g_pRLX->pGX->View.Flags & GX_CAPS_MULTISAMPLING)
	{
		*pAttrib = AGL_SAMPLE_BUFFERS_ARB; pAttrib++;
		*pAttrib = 1; pAttrib++;
		*pAttrib = AGL_SAMPLES_ARB; pAttrib++;
		*pAttrib = g_pRLX->pGX->View.Multisampling; pAttrib++;
    }

	*pAttrib = AGL_DOUBLEBUFFER; pAttrib++;
	*pAttrib = AGL_DEPTH_SIZE; pAttrib++;
	*pAttrib = 16; pAttrib++;
	*pAttrib = AGL_NONE; pAttrib++;

    AGLPixelFormat fmt;
    GLboolean      ok;

    /* Choose an rgb pixel format */
    GDHandle gdhDisplay;
    ok = DMGetGDeviceByDisplayID((DisplayIDType)g_cgDisplayID, &gdhDisplay, false);
    SYS_ASSERT(ok == noErr);
    fmt = aglChoosePixelFormat(&gdhDisplay, 1, attrib);
    if(fmt == NULL)
    {
        ok = SYS_AGLTRACE(aglGetError());
     	return -1;
    }
    /* Create an AGL context */
    g_pAGLC = aglCreateContext(fmt, NULL);
    if(g_pAGLC == NULL)
		return -2;

	SizeWindow(g_hWnd, gl_lx, gl_ly, true);
    CenterWindow(g_hWnd);
    ShowWindow(g_hWnd);

	// copy portRect
	GetPortBounds(GetWindowPort(g_hWnd), g_pRect);

    /* Attach the window to the context */
    ok = SYS_AGLTRACE(aglSetDrawable(g_pAGLC, GetWindowPort(g_hWnd)));
    if(!ok)
		return -3;

    /* Make the context the current context */
    ok = SYS_AGLTRACE(aglSetCurrentContext(g_pAGLC));
    if(!ok)
		return -4;

    /* Pixel format is no more needed */
    aglDestroyPixelFormat(fmt);

    if (!(g_pRLX->Video.Config & RLXVIDEO_Windowed))
    {
		HideMenuBar();
        aglSetFullScreen(g_pAGLC, 0, 0, 0, 0);
        GLint swap = !!(g_pRLX->pGX->View.Flags & GX_CAPS_VSYNC);
        aglSetInteger(g_pAGLC, AGL_SWAP_INTERVAL, &swap);
    }

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
	if (!g_pAGLC)
		return;

	aglSetCurrentContext(NULL);
    aglSetDrawable(g_pAGLC, NULL);
    aglDestroyContext(g_pAGLC);
    g_pAGLC = 0;
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
	if (!g_pAGLC)
		return;

	if (g_cgOldDisplayModeRestore)
	{
		CGDisplaySwitchToMode(g_cgDisplayID, g_cgPrevDisplayMode);		
		CGReleaseAllDisplays();
		g_cgOldDisplayModeRestore = 0;
		ShowMenuBar();
	}
	
    return;
}

static int Open(void * hwnd)
{
	SYS_ASSERT(g_pAGLC == 0);
	SYS_ASSERT(hwnd);
	static Rect rect;
	g_pRect = &rect;
	g_hWnd = (WindowPtr) hwnd;
	g_cgDisplayID = CGMainDisplayID ();
	g_cgPrevDisplayMode = CGDisplayCurrentMode (g_cgDisplayID);		
	g_cgOldDisplayModeRestore = 0;
	g_cgDisplayModeList = CGDisplayAvailableModes (g_cgDisplayID);
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
    "OpenGL"
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
