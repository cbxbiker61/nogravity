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
#include <stdlib.h>
#include <windows.h>
#include "../gl_v3x.h"
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

// V3X
struct RLXSYSTEM *	g_pRLX;
static HDC g_hDC;
static HGLRC g_hGLRC;
static HWND g_hWnd;

extern GXGRAPHICINTERFACE GI_OpenGL;
extern GXCLIENTDRIVER GX_OpenGL;
extern GXSPRITEINTERFACE CSP_OpenGL;
extern V3X_GXSystem V3X_OpenGL;

// Display mode
static int						gl_CurrentPixelFmt;
static DEVMODE			*		gl_pDisplayModes,
						*		gl_pCurrentDisplayMode;
static int32_t					gl_lx,
								gl_ly;
static int						gl_bpp,
								gl_nDisplayModeCount;
static GXDISPLAYMODEHANDLE		gl_nCurrentDisplayModeGL;

#define GetWindowStyle(h) GetWindowLong(h, GWL_STYLE)
#define GetWindowExStyle(h) GetWindowLong(h, GWL_EXSTYLE)

static DWORD old_Style;
void W32_WindowLayoutSetStyle(HWND hWnd, bool bWindowed)
{
	if (!bWindowed)
	{
		DWORD g_Style = GetWindowLong(hWnd, GWL_STYLE);
		old_Style = g_Style;
		g_Style|=(WS_VISIBLE|WS_POPUP);
		g_Style&=~(WS_CAPTION);
#if !_WIN32_WCE
		g_Style&=~WS_THICKFRAME;
#endif
		SetWindowLong(hWnd, GWL_STYLE, g_Style);
	}
	else
	{
		if (old_Style)
			SetWindowLong(hWnd, GWL_STYLE, old_Style);
		else
			SetWindowLong(hWnd, GWL_STYLE, WS_POPUP|WS_CAPTION);
	}
}
static void W32_WindowLayoutSetSize(HWND hWnd, RECT *pRect, int dwWidth, int dwHeight, bool bFullScrn, bool bResizable) // Must be in called at D3Dx window creation
{	
	DWORD dwStyle = GetWindowStyle(hWnd);
	DWORD dwExStyle = GetWindowExStyle( hWnd);
	RECT  rc;				
	// If we are still a WS_POPUP window we should convert to a normal app
	// window so we look like a windows app.
	if (!bFullScrn)
	{
#if _WIN32_WCE
		dwExStyle|= bResizable ? WS_EX_WINDOWEDGE : 0;						// Window Extended Style
		dwStyle|= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;	// Windows Style
#else
		dwExStyle|=WS_EX_APPWINDOW | (bResizable ? WS_EX_WINDOWEDGE : 0);						// Window Extended Style
		dwStyle|= WS_OVERLAPPED| WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;	// Windows Style
		if (bResizable)
			dwStyle|= WS_THICKFRAME | WS_MAXIMIZEBOX;

#endif
	}
	else
	{
		dwStyle|= WS_POPUP;
	}
	SetWindowLong(hWnd, GWL_STYLE, dwStyle);
	SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle);

	if (!bFullScrn)
	{
		RECT  rcWork;
		
		// Center Window
		SetRect( &rc, 0, 0, dwWidth, dwHeight );
		SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWork, 0 );
		
		int cx = (rcWork.right - rcWork.left - (rc.right - rc.left)) >>1;
		int cy = (rcWork.bottom - rcWork.top - (rc.bottom - rc.top)) >>1;		

		rc.left+=cx;
		rc.right+=cx;
		rc.top+=cy;
		rc.bottom+=cy;

		//  Make sure our window does not hang outside of the work area
		if( rc.left < rcWork.left ) rc.left = rcWork.left;
		if( rc.top  < rcWork.top )  rc.top  = rcWork.top;

		AdjustWindowRectEx( &rc,
							GetWindowStyle(hWnd), 
#if _WIN32_WCE
							0,
#else
	  						GetMenu(hWnd) != NULL,
#endif
							GetWindowExStyle(hWnd) );




		SetWindowPos( hWnd, NULL,
					  rc.left, rc.top, 
					  rc.right - rc.left, 
					  rc.bottom - rc.top,
					  SWP_NOZORDER | SWP_NOACTIVATE );


	}
	else
	{		
		SetRect( &rc, 0, 0, dwWidth, dwHeight );
		SetWindowPos( hWnd, HWND_TOPMOST, rc.left, rc.top, 
					 rc.right - rc.left, 
					 rc.bottom - rc.top, NULL);
	}

	if (pRect)
		*pRect = rc;
	return;
}

static void CALLING_C Flip(void)
{	
	g_hDC = GetDC(g_hWnd);
    wglMakeCurrent(g_hDC, g_hGLRC);
    SwapBuffers(g_hDC);
	ReleaseDC(g_hWnd, g_hDC);
    return;
}

extern void SetPrimitiveSprites();

static u_int8_t RLXAPI *Lock(void)
{
	if (!g_hDC)
	{
		g_hDC = GetDC(g_hWnd);
		wglMakeCurrent(g_hDC, g_hGLRC);
	}
    return NULL;
}

static void RLXAPI Unlock(void)
{
	if (g_hDC)
	{
		ReleaseDC(g_hWnd, g_hDC);
		g_hDC = 0;
	}
    return;
}

void SYS_Debug(char *fmt, ...)
{
    FILE *in;
    in = fopen("debug.txt", "a+t");
    if (in)
    {
        char buffer[8192];
        va_list argptr;
        va_start(argptr, fmt);
        vsprintf(buffer, fmt, argptr);
        fprintf(in, "%s\n", buffer);
		strcat(buffer, "\n");
		OutputDebugString(buffer);
        va_end(argptr);
        fclose(in);
    }
    return;
}

char *SYS_GetLastError()
{

	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
	);
	return (char*)lpMsgBuf;
}

// set gamma ramp
static void setGammaRamp(const rgb24_t *ramp) 
{	
    WORD RamdacTable[256*3];
    for( int i=0; i<256; i++ ) 
	{
		RamdacTable[i*3+0] = ((WORD)ramp[i].r)<<8;
        RamdacTable[i*3+1] = ((WORD)ramp[i].g)<<8;
		RamdacTable[i*3+2] = ((WORD)ramp[i].b)<<8;
    }

	g_hDC = GetDC(NULL);
    BOOL ret = SetDeviceGammaRamp( g_hDC, RamdacTable );
	ReleaseDC(NULL, g_hDC);
}

int GL_IsSupported(const char *extension);
GLboolean glewGetExtension (const char* name, GLubyte *p);

static int GL_SetupPixelFormat(HWND hWnd)
{
    PIXELFORMATDESCRIPTOR pfd;
	int retVal = 0;
	int colorIndexMode	= 0;
	int stereo          = 0;
	int dib				= 0;
	int bpp				= gl_bpp;
	int depth			= 16;
	int stencil			= 0;
	int dbl				= 1;
	int acc				= 1;
	int fs              = !(g_pRLX->Video.Config&RLXVIDEO_Windowed);
	int multisample     = g_pRLX->pGX->View.Flags & GX_CAPS_MULTISAMPLING ? g_pRLX->pGX->View.Multisampling : 0;
    HDC hDCFront;
    gl_CurrentPixelFmt = -1;

	if (multisample)
	{
	   // Create dummy windows and class. Access to WGL extensions. Painful.
		WNDCLASS wcTemp;
		HWND hWndTemp = 0;
		HDC hDCTemp = 0;

		wcTemp.style         = 0;
	    wcTemp.hInstance     = GetModuleHandle(NULL);
	    wcTemp.hIcon         = NULL;
	    wcTemp.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wcTemp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	    wcTemp.lpszMenuName  = NULL;
	    wcTemp.cbClsExtra    = 0;
	    wcTemp.cbWndExtra    = 0;
		wcTemp.lpfnWndProc   = DefWindowProc;
	    wcTemp.lpszClassName = "Dummy";
	   	RegisterClass(&wcTemp);
    
		hWndTemp = CreateWindowEx(0, wcTemp.lpszClassName, wcTemp.lpszClassName, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1,
										0, 0, wcTemp.hInstance, NULL);
		hDCTemp = GetDC(hWndTemp);
		gl_CurrentPixelFmt = ChoosePixelFormat( hDCTemp, &pfd );
		SetPixelFormat(hDCTemp, gl_CurrentPixelFmt, &pfd);
		g_hGLRC = wglCreateContext(hDCTemp);

		wglMakeCurrent(hDCTemp, g_hGLRC);
		wglGetProcAddresses();
		wglGetProcAddressesARB(hDCTemp);
		if (wglGetExtensionsStringARB
			&& wglIsExtensionSupportedARB(hDCTemp, "WGL_ARB_pixel_format"))
		{
			while(1)
			{
				// Get max multisample
				GLfloat		 fattrib[32] = {0,0};
				GLint        iattrib[32];
				GLint		* piAttrib = iattrib;
				
				int numFormat = 0;
				GLint values[2];

				iattrib[0] = WGL_NUMBER_PIXEL_FORMATS_ARB;
				wglGetPixelFormatAttribivARB(hDCTemp, 0, 0, 1, iattrib, values);
				numFormat = values[0];

				if (acc)
				{
		    		*piAttrib = WGL_SUPPORT_OPENGL_ARB; piAttrib++;
		   			*piAttrib = GL_TRUE; piAttrib++;
					*piAttrib = WGL_ACCELERATION_ARB; piAttrib++;
					*piAttrib = WGL_FULL_ACCELERATION_ARB; piAttrib++;
				}

				if (bpp)
				{
					*piAttrib = WGL_COLOR_BITS_ARB; piAttrib++;
					*piAttrib = bpp == 32 ? 24 : bpp; piAttrib++;
				}

				if (depth)
				{
					*piAttrib = WGL_DEPTH_BITS_ARB; piAttrib++;
					*piAttrib = stencil ? 24 : depth; piAttrib++;
				}

				if (bpp)
				{
					*piAttrib = WGL_ALPHA_BITS_ARB; piAttrib++;
					*piAttrib = bpp == 32 ? 8 : 0; piAttrib++;
				}

				if (stencil)
				{
					*piAttrib = WGL_STENCIL_BITS_ARB; piAttrib++;
					*piAttrib = stencil; piAttrib++;
				}

				if (dbl)
				{
					*piAttrib = WGL_DOUBLE_BUFFER_ARB; piAttrib++;
					*piAttrib = GL_TRUE; piAttrib++;
				}

				if (stereo)
				{
					*piAttrib = WGL_STEREO_ARB; piAttrib++;
					*piAttrib = GL_TRUE; piAttrib++;
				}

				if (!fs)
				{
					*piAttrib = WGL_DRAW_TO_WINDOW_ARB; piAttrib++;
					*piAttrib = GL_TRUE; piAttrib++;
				}

				if (dib)
				{
					*piAttrib = WGL_DRAW_TO_BITMAP_ARB; piAttrib++;
					*piAttrib = 1; piAttrib++;
				}

				if (multisample)
				{
					*piAttrib = WGL_SAMPLE_BUFFERS_ARB; piAttrib++;
					*piAttrib = GL_TRUE; piAttrib++;
					*piAttrib = WGL_SAMPLES_ARB; piAttrib++;
					*piAttrib = multisample; piAttrib++;
				}

				*piAttrib = 0; piAttrib++;
				*piAttrib = 0; piAttrib++;

				GLuint numFormats = 0;

				if ((wglChoosePixelFormatARB(hDCTemp,
											iattrib,
											fattrib,
											1,
											&gl_CurrentPixelFmt,
											&numFormats) == GL_TRUE) && (numFormats>=1))
				{
					break;
				}
				else
				{
			   		gl_CurrentPixelFmt = -1;
					if (multisample)
			       		multisample/=2;
					else
						break;
				}
			}
		}

		wglMakeCurrent(NULL, NULL);
		ReleaseDC(hWndTemp, hDCTemp);
		wglDeleteContext(g_hGLRC);
		UnregisterClass(wcTemp.lpszClassName, wcTemp.hInstance);
		DestroyWindow(hWndTemp);
	}

	g_hDC = hDCFront = GetDC(hWnd);

	if (gl_CurrentPixelFmt == -1)
	{
		gl_CurrentPixelFmt = ChoosePixelFormatEx(hDCFront, &bpp, &depth, &stencil, &dbl, &acc, &dib);
	}

	retVal = SetPixelFormat(g_hDC, gl_CurrentPixelFmt, &pfd);
	SYS_ASSERT(retVal == TRUE);
	DescribePixelFormat(g_hDC, gl_CurrentPixelFmt, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	g_pRLX->pGX->View.Flip = Flip;
	g_pRLX->pGX->View.ColorMask.RedMaskSize	= pfd.cRedBits;
	g_pRLX->pGX->View.ColorMask.GreenMaskSize = pfd.cGreenBits;
	g_pRLX->pGX->View.ColorMask.BlueMaskSize = pfd.cBlueBits;
	g_pRLX->pGX->View.ColorMask.RsvdMaskSize = pfd.cAlphaBits;
	g_pRLX->pGX->View.ColorMask.RedFieldPosition = pfd.cRedShift;
	g_pRLX->pGX->View.ColorMask.GreenFieldPosition = pfd.cGreenShift;
	g_pRLX->pGX->View.ColorMask.BlueFieldPosition = pfd.cBlueShift;
	g_pRLX->pGX->View.ColorMask.RsvdFieldPosition = pfd.cAlphaShift;
	g_pRLX->pV3X->ViewPort.zDepth = pfd.cDepthBits;
	if (multisample)
	{
		glEnable(GL_MULTISAMPLE_ARB);
	}

	return 0;
}

static GXDISPLAYMODEINFO RLXAPI *EnumDisplayList(int bpp)
{
   	DEVMODE *drv;
	int j=0, i;
	GXDISPLAYMODEINFO *drv1, *drv0;
	SYS_ASSERT(gl_pDisplayModes);
	SYS_ASSERT(gl_nDisplayModeCount);

	for (drv = gl_pDisplayModes, i=0;i<gl_nDisplayModeCount;i++, drv++)
	{
		if ((drv->dmBitsPerPel==(unsigned)bpp) || (bpp == -1))
			j++;
	}

	drv1 = (GXDISPLAYMODEINFO*)g_pRLX->mm_heap->malloc(sizeof(GXDISPLAYMODEINFO)*(j+1));
	for (drv0=drv1, drv=gl_pDisplayModes, i=0;i<gl_nDisplayModeCount;i++, drv++)
	{
		if ((drv->dmBitsPerPel==(unsigned)bpp)||(bpp == -1))
		{
			drv0->lWidth = (u_int16_t)drv->dmPelsWidth;
			drv0->lHeight = (u_int16_t)drv->dmPelsHeight;
			drv0->BitsPerPixel = (u_int8_t)drv->dmBitsPerPel;
			drv0->mode = i;
			drv0++;
		}
	}
	drv0->BitsPerPixel = 0;
	return drv1;
}

static int GL_GetDesktopColorDepth(HWND hwnd)
{
	HDC hDC = ::GetDC(hwnd);
	SYS_ASSERT(hDC!=0);
	int bpp = ::GetDeviceCaps(hDC, BITSPIXEL);
	::ReleaseDC(hwnd, hDC);
	return bpp;
}

static GXDISPLAYMODEHANDLE RLXAPI SearchDisplayMode(int lx, int ly, int bpp)
{
    int i;
	u_int16_t MaxRefreshRate = 0;
	gl_bpp = GL_GetDesktopColorDepth(g_hWnd);
	int safeMode1 = 0, safeMode2 = 0;

	if (!(g_pRLX->Video.Config&RLXVIDEO_Windowed))
	{
		int ret = 0;
		DEVMODE *drv = gl_pDisplayModes;
		SYS_ASSERT(gl_nDisplayModeCount);
		for (i=0;i<gl_nDisplayModeCount;i++, drv++)
		{			
			if (
				((drv->dmBitsPerPel ==(u_int16_t)bpp)|| ((bpp == -1) && (bpp == gl_bpp)))
			&&	 (drv->dmPelsWidth  ==(u_int16_t)lx)
			&&	 (drv->dmPelsHeight ==(u_int16_t)ly)
			)
			{
				// get highest supported refresh rate
				if (drv->dmDisplayFrequency > MaxRefreshRate)
				{
					// not too high ...
					if ((ret && (drv->dmDisplayFrequency<=70))||(!ret))
					{
						MaxRefreshRate = (u_int16_t)drv->dmDisplayFrequency;
						ret = i + 1;
					}
				}
			}
		}
		return ret;
	}
	else
	{
		if (gl_lx!=lx)
		{
			gl_lx  = lx;
			gl_nCurrentDisplayModeGL = 0xffff;
		}

		if (gl_ly!=ly)
		{
			gl_ly  = ly;
			gl_nCurrentDisplayModeGL = 0xffff;
		}
		return 1;
	}
}

static void RLXAPI GetDisplayInfo(GXDISPLAYMODEHANDLE mode)
{
	if (g_pRLX->Video.Config&RLXVIDEO_Windowed)
	{
		g_pRLX->pfSetViewPort(&g_pRLX->pGX->View, gl_lx, gl_ly, gl_bpp);
	}
	else
	{
		DEVMODE *drv = gl_pDisplayModes + mode - 1;
		if (mode<0 || mode>=gl_nDisplayModeCount)
		{
			g_pRLX->Video.Config|=RLXVIDEO_Windowed;
			return GetDisplayInfo(SearchDisplayMode(640, 480, 16));
		}
		g_pRLX->pfSetViewPort(&g_pRLX->pGX->View, drv->dmPelsWidth, drv->dmPelsHeight, drv->dmBitsPerPel);
		gl_pCurrentDisplayMode = drv;
		gl_lx = drv->dmPelsWidth;
		gl_ly = drv->dmPelsHeight;
		gl_bpp = drv->dmBitsPerPel;
	}
	g_pRLX->pGX->gi = GI_OpenGL;
	
	SetPrimitiveSprites();
    g_pRLX->pGX->View.Flip = Flip;
    g_pRLX->pGX->gi = GI_OpenGL;
    g_pRLX->pGX->csp = CSP_OpenGL;
	GI_OpenGL.setGammaRamp = setGammaRamp;
    g_pRLX->pGX->csp_cfg.put.fonct = g_pRLX->pGX->csp.put;
    g_pRLX->pGX->csp_cfg.pset.fonct = g_pRLX->pGX->csp.pset;
    g_pRLX->pGX->csp_cfg.transp.fonct = g_pRLX->pGX->csp.Trsp50;
    g_pRLX->pGX->csp_cfg.op = g_pRLX->pGX->csp.put;
    return;
}

static int RLXAPI SetDisplayMode(GXDISPLAYMODEHANDLE mode)
{
	bool bWindowed = !!(g_pRLX->Video.Config&RLXVIDEO_Windowed);
	W32_WindowLayoutSetStyle(g_hWnd, bWindowed);
	if (!bWindowed)
	{
		DEVMODE *drv = gl_pDisplayModes + mode - 1;
		SYS_ASSERT(mode>=0);
		if (ChangeDisplaySettings(gl_pCurrentDisplayMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			bWindowed = true;
			W32_WindowLayoutSetStyle(g_hWnd, bWindowed);
		}
	}

	gl_nCurrentDisplayModeGL = mode;
	SetActiveWindow(g_hWnd);
    return 0;
}

static int RLXAPI CreateSurface(int numberOfSparePages)
{    
	int ret = GL_SetupPixelFormat(g_hWnd);
	if (ret)
		return ret;

	g_hGLRC = wglCreateContext(g_hDC);
	if (!g_hGLRC)
		return -4;

	if (!wglMakeCurrent(g_hDC, g_hGLRC))
		return -5;

	// Set window size
	RECT rect;
	W32_WindowLayoutSetSize(g_hWnd, &rect, gl_lx, gl_ly, !(g_pRLX->Video.Config&RLXVIDEO_Windowed), 0);
	ShowWindow(g_hWnd, SW_SHOW);
	SetForegroundWindow(g_hWnd);
	SetFocus(g_hWnd);

	wglGetProcAddresses();
	wglGetProcAddressesARB(g_hDC);
	GL_InstallExtensions();
	GL_ResetViewport();
	
	glClear(GL_COLOR_BUFFER_BIT);
	Flip();

	glClear(GL_COLOR_BUFFER_BIT);
	Flip();

	g_pRLX->pGX->Surfaces.maxSurface = numberOfSparePages;
    return 0; 
}

static void RLXAPI ReleaseSurfaces(void)
{
	wglMakeCurrent(g_hDC, g_hGLRC);

	if (g_hGLRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(g_hGLRC);
		g_hGLRC = 0;
	}

	g_pRLX->pGX->Surfaces.maxSurface = 0;
    return;
}

static int RLXAPI RegisterMode(int mode)
{
	g_pRLX->pGX->View.DisplayMode = (u_int16_t)mode;
	g_pRLX->pGX->Client->GetDisplayInfo(mode);
	return g_pRLX->pGX->Client->SetDisplayMode(mode);

}

static void RLXAPI Shutdown(void)
{
	if (!(g_pRLX->Video.Config&RLXVIDEO_Windowed))
		ChangeDisplaySettings(NULL, 0);

	if (gl_pDisplayModes)
	{
		delete[] gl_pDisplayModes;
		gl_pDisplayModes = 0;
	}
    return;
}

static int Open(void *hWnd)
{
	g_hWnd = (HWND) hWnd;
	gl_nCurrentDisplayModeGL = 0xffff;
	wglQueryDisplayModes(&gl_pDisplayModes, &gl_nDisplayModeCount);
    return 1;
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

_RLXEXPORTFUNC void RLXAPI GX_EntryPoint(struct RLXSYSTEM *p)
{
    g_pRLX = p;
	g_pRLX->pGX->Client = &GX_OpenGL;
    return;
}

_RLXEXPORTFUNC void RLXAPI V3X_EntryPoint(struct RLXSYSTEM *p)
{    
	GX_EntryPoint(p);
    g_pRLX->pV3X->Client = &V3X_OpenGL;
    return;
}
