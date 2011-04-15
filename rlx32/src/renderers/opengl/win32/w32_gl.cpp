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

// Function helper
#define ISFULLSCREEN() (!(g_pRLX->Video.Config&RLXVIDEO_Windowed))
int GL_IsSupported(const char *extension);
GLboolean glewGetExtension (const char* name, GLubyte *p);
void SetPrimitiveSprites();

extern GXGRAPHICINTERFACE GI_OpenGL;
extern GXCLIENTDRIVER GX_OpenGL;
extern GXSPRITEINTERFACE CSP_OpenGL;
extern V3X_GXSystem V3X_OpenGL;

// Display mode

static DEVMODE			*		gl_pDisplayModes,
						*		gl_pCurrentDisplayMode;
static int32_t					gl_lx,
								gl_ly;
static int						gl_bpp,
								gl_nDisplayModeCount;
static GXDISPLAYMODEHANDLE		gl_nCurrentDisplayModeGL;

// Error management

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

/*
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
*/

// Window management

#define GetWindowStyle(h) GetWindowLong(h, GWL_STYLE)
#define GetWindowExStyle(h) GetWindowLong(h, GWL_EXSTYLE)

static DWORD old_Style;

static int GL_GetDesktopColorDepth(HWND hwnd)
{
	HDC hDC = ::GetDC(hwnd);
	SYS_ASSERT(hDC!=0);
	int bpp = ::GetDeviceCaps(hDC, BITSPIXEL);
	::ReleaseDC(hwnd, hDC);
	return bpp;
}

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
		SetWindowPos( hWnd, HWND_TOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, NULL);
	}

	if (pRect)
		*pRect = rc;
	return;
}

// Page flip
static void CALLING_C Flip(void)
{
    glFinish();
    SwapBuffers(g_hDC);
    return;
}

// lock framebuffer
static uint8_t RLXAPI *Lock(void)
{
    return NULL;
}

// unlock frame buffer
static void RLXAPI Unlock(void)
{
    return;
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
    BOOL ret = SetDeviceGammaRamp(g_hDC, RamdacTable );
	return;
}

static int GL_SetupPixelFormat(HWND hWnd)
{
    PIXELFORMATDESCRIPTOR pfd;
    int pixfmt = -1;
	int retVal = 0;
	int colorIndexMode	= 0;
	int stereo          = 0;
	int dib				= 0;
	int bpp				= gl_bpp;
	int depth			= 16;
	int stencil			= 0;
	int dbl				= 1;
	int acc				= 1;
	int fs              = ISFULLSCREEN();
	int multisample     = g_pRLX->pGX->View.Flags & GX_CAPS_MULTISAMPLING ? g_pRLX->pGX->View.Multisampling : 0;
    HDC hDCFront;

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
	wcTemp.lpszClassName = "fakewindow";

	if (!RegisterClass(&wcTemp))
	{
		return -1;
	}

#ifdef _DEBUG
	SYS_Debug("...registered fake window class");
#endif

	hWndTemp = CreateWindowEx(0,
                              wcTemp.lpszClassName,
							  wcTemp.lpszClassName,
							  WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1,
							  0, 0, wcTemp.hInstance, NULL);

	if (!hWndTemp)
	{
		return -1;
	}

	hDCTemp = GetDC(hWndTemp);

	if (!hDCTemp)
	{
		return -1;
	}

	pixfmt = ChoosePixelFormat( hDCTemp, &pfd );
	if (!pixfmt)
	{
       return -1;
	}

	SetPixelFormat(hDCTemp, pixfmt, &pfd);
	g_hGLRC = wglCreateContext(hDCTemp);
	if (!g_hGLRC)
	{
		return -1;
	}

	if (!wglMakeCurrent(hDCTemp, g_hGLRC))
	{
		return -1;
	}

	wglGetProcAddresses();
	wglGetProcAddressesARB(hDCTemp);

	// "WGL_ARB_extensions_string"
	if (wglGetExtensionsStringARB && wglIsExtensionSupportedARB(hDCTemp, "WGL_ARB_pixel_format"))
	{
		while(1)
		{
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

			if (multisample && wglIsExtensionSupportedARB(hDCTemp, "WGL_ARB_multisample"))
			{
				*piAttrib = WGL_SAMPLE_BUFFERS_ARB; piAttrib++;
				*piAttrib = GL_TRUE; piAttrib++;
				*piAttrib = WGL_SAMPLES_ARB; piAttrib++;
				*piAttrib = multisample; piAttrib++;
			}
			else
			{
				multisample = 0;
				g_pRLX->pGX->View.Flags &=~ GX_CAPS_MULTISAMPLING;
			}

			*piAttrib = 0; piAttrib++;
			*piAttrib = 0; piAttrib++;

			GLuint numFormats = 0;

			if ((wglChoosePixelFormatARB(hDCTemp,
											iattrib,
											fattrib,
											1,
											&pixfmt,
											&numFormats) == GL_TRUE) && (numFormats>=1))
			{
				break;
			}
			else
			{
#ifdef _DEBUG
				SYS_Debug("...failed to set multisampling to %d\n", multisample);
#endif
				pixfmt = -1;
				if (multisample)
				{
					multisample/=2;
			    }
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

	g_hDC = hDCFront = GetDC(hWnd);

	if (!g_hDC)
	{
		return -1;
	}

	if (pixfmt == -1)
	{
		// Using old method
		pixfmt = ChoosePixelFormatEx(hDCFront, &bpp, &depth, &stencil, &dbl, &acc, &dib);
	}

	retVal = SetPixelFormat(g_hDC, pixfmt, &pfd);
	if (retVal == FALSE)
	{
		return -2;
	}

	SYS_ASSERT(retVal == TRUE);
	DescribePixelFormat(g_hDC, pixfmt, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	g_pRLX->pGX->View.ColorMask.RedMaskSize	= pfd.cRedBits;
	g_pRLX->pGX->View.ColorMask.GreenMaskSize = pfd.cGreenBits;
	g_pRLX->pGX->View.ColorMask.BlueMaskSize = pfd.cBlueBit
	g_pRLX->pGX->View.ColorMask.RsvdMaskSize = pfd.cAlphaBits;
	g_pRLX->pGX->View.ColorMask.RedFieldPosition = pfd.cRedShift;
	g_pRLX->pGX->View.ColorMask.GreenFieldPosition = pfd.cGreenShift;
	g_pRLX->pGX->View.ColorMask.BlueFieldPosition = pfd.cBlueShift;
	g_pRLX->pGX->View.ColorMask.RsvdFieldPosition = pfd.cAlphaShift;
	g_pRLX->pV3X->ViewPort.zDepth = pfd.cDepthBits;

	return pixfmt;
}

// Enumerate display modes
static GXDISPLAYMODEINFO RLXAPI *EnumDisplayList(int bpp)
{
	DEVMODE *drv;
	int j=0, i;
	GXDISPLAYMODEINFO *pRes, *p;
	SYS_ASSERT(gl_pDisplayModes);
	SYS_ASSERT(gl_nDisplayModeCount);

	for (drv = gl_pDisplayModes, i=0;i<gl_nDisplayModeCount;i++, drv++)
	{
		if ((drv->dmBitsPerPel==(unsigned)bpp) || (bpp == -1))
			j++;
	}

	pRes = (GXDISPLAYMODEINFO*)g_pRLX->mm_heap->malloc(sizeof(GXDISPLAYMODEINFO)*(j+1));
	for (p=pRes, drv=gl_pDisplayModes, i=0;i<gl_nDisplayModeCount;i++, drv++)
	{
		if ((drv->dmBitsPerPel==(unsigned)bpp)||(bpp == -1))
		{
			p->lWidth = (uint16_t)drv->dmPelsWidth;
			p->lHeight = (uint16_t)drv->dmPelsHeight;
			p->BitsPerPixel = (uint8_t)drv->dmBitsPerPel;
			p->mode = i;
			p++;
		}
	}
	p->BitsPerPixel = 0;
	return pRes;
}

// Search display modes
static GXDISPLAYMODEHANDLE RLXAPI SearchDisplayMode(int lx, int ly, int bpp)
{
    int i;
	uint16_t MaxRefreshRate = 0;
	gl_bpp = GL_GetDesktopColorDepth(g_hWnd);
	int safeMode1 = 0, safeMode2 = 0;

	if (ISFULLSCREEN())
	{
		int ret = 0;
		DEVMODE *drv = gl_pDisplayModes;
		SYS_ASSERT(gl_nDisplayModeCount);
		for (i=0;i<gl_nDisplayModeCount;i++, drv++)
		{
			if (
				((drv->dmBitsPerPel ==(uint16_t)bpp)|| ((bpp == -1) && (bpp == gl_bpp)))
			&&	 (drv->dmPelsWidth  ==(uint16_t)lx)
			&&	 (drv->dmPelsHeight ==(uint16_t)ly)
			)
			{
				// get highest supported refresh rate
				if (drv->dmDisplayFrequency > MaxRefreshRate)
				{
					// not too high ...
					if ((ret && (drv->dmDisplayFrequency<=70))||(!ret))
					{
						MaxRefreshRate = (uint16_t)drv->dmDisplayFrequency;
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

// Get display mode info
static void RLXAPI GetDisplayInfo(GXDISPLAYMODEHANDLE mode)
{
	if (!ISFULLSCREEN())
	{
		g_pRLX->pfSetViewPort(&g_pRLX->pGX->View, gl_lx, gl_ly, gl_bpp);
	}
	else
	{
		DEVMODE *drv = gl_pDisplayModes + mode - 1;
		if (mode<0 || mode>=gl_nDisplayModeCount)
		{
			g_pRLX->Video.Config|=RLXVIDEO_Windowed;
			return GetDisplayInfo(SearchDisplayMode(640, 480, GL_GetDesktopColorDepth(g_hWnd)));
		}
		g_pRLX->pfSetViewPort(&g_pRLX->pGX->View, drv->dmPelsWidth, drv->dmPelsHeight, drv->dmBitsPerPel);
		gl_pCurrentDisplayMode = drv;
		gl_lx = drv->dmPelsWidth;
		gl_ly = drv->dmPelsHeight;
		gl_bpp = drv->dmBitsPerPel;
	}

	g_pRLX->pGX->gi = GI_OpenGL;

	GL_FakeViewPort();
	SetPrimitiveSprites();

    g_pRLX->pGX->gi = GI_OpenGL;
    g_pRLX->pGX->csp = CSP_OpenGL;
    g_pRLX->pGX->View.Flip = Flip;

	GI_OpenGL.setGammaRamp = setGammaRamp;
    g_pRLX->pGX->csp_cfg.put.fonct = g_pRLX->pGX->csp.put;
    g_pRLX->pGX->csp_cfg.pset.fonct = g_pRLX->pGX->csp.pset;
    g_pRLX->pGX->csp_cfg.transp.fonct = g_pRLX->pGX->csp.Trsp50;
    g_pRLX->pGX->csp_cfg.op = g_pRLX->pGX->csp.put;
}

// Set display mode
static int RLXAPI SetDisplayMode(GXDISPLAYMODEHANDLE mode)
{
	bool bWindowed = !ISFULLSCREEN();
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

// Create surface
static int RLXAPI CreateSurface(int BackBufferCount)
{
	int pixfmt = GL_SetupPixelFormat(g_hWnd);
	if (pixfmt < 0)
	{
		return -1;
	}

#ifdef _DEBUG
	SYS_Debug("...PIXELFORMAT %d selected\n", pixfmt);
#endif

	g_hGLRC = wglCreateContext(g_hDC);
	if (!g_hGLRC)
	{
		return -4;
	}

#ifdef _DEBUG
	SYS_Debug("...creating GL context: succeeded");
#endif

	if (!wglMakeCurrent(g_hDC, g_hGLRC))
	{
		return -5;
	}

#ifdef _DEBUG
	SYS_Debug("...making context current: succeeded");
#endif

	// Set window size
	RECT rect;
	W32_WindowLayoutSetSize(g_hWnd, &rect, gl_lx, gl_ly, ISFULLSCREEN(), 0);
	ShowWindow(g_hWnd, SW_SHOW);
	SetForegroundWindow(g_hWnd);
	SetFocus(g_hWnd);

	// Reset engine
	wglGetProcAddresses();
	wglGetProcAddressesARB(g_hDC);
	GL_InstallExtensions();
	GL_ResetViewport();

	if (wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT(g_pRLX->pGX->View.Flags&GX_CAPS_VSYNC);
	}

#ifdef _DEBUG
	SYS_Debug("...%s", glGetString(GL_VENDOR));
	SYS_Debug("...%s", glGetString(GL_VERSION));
	SYS_Debug("...%s", glGetString(GL_RENDERER));
#endif

	// Enable multisampling
	if (g_pRLX->pGX->View.Flags & GX_CAPS_MULTISAMPLING)
	{
		if (GL_IsSupported("GL_NV_multisample_filter_hint"))
		{
			glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_FASTEST);
		}
		glEnable(GL_MULTISAMPLE_ARB);
	}

	// Clear buffers
	for (int i=0;i<2;i++)
	{
	    glClearColor(0.5,0.5,0.5,0.5);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		Flip();
	}
    glClearColor(0,0,0,0);

	g_pRLX->pGX->Surfaces.maxSurface = BackBufferCount;

    return 0;
}

// Release surfaces
static void RLXAPI ReleaseSurfaces(void)
{
    if (g_hDC)
    {
	    ReleaseDC(g_hWnd, g_hDC);
	    g_hDC = 0;
    }

	if (g_hGLRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(g_hGLRC);
		g_hGLRC = 0;
	}

	g_pRLX->pGX->Surfaces.maxSurface = 0;
}

// Register display mode
static int RLXAPI RegisterMode(int mode)
{
	g_pRLX->pGX->View.DisplayMode = (uint16_t)mode;
	g_pRLX->pGX->Client->GetDisplayInfo(mode);
	return g_pRLX->pGX->Client->SetDisplayMode(mode);
}

// Shutdown
static void RLXAPI Shutdown(void)
{
	if (ISFULLSCREEN())
	{
		ChangeDisplaySettings(NULL, 0);
	}

	if (gl_pDisplayModes)
	{
		delete[] gl_pDisplayModes;
		gl_pDisplayModes = 0;
	}
}

// Open engine
static int Open(void *hWnd)
{
	g_hWnd = (HWND) hWnd;
	gl_nCurrentDisplayModeGL = 0xffff;
	wglQueryDisplayModes(&gl_pDisplayModes, &gl_nDisplayModeCount);
    return 1;
}

// Unused
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
}

_RLXEXPORTFUNC void RLXAPI V3X_EntryPoint(struct RLXSYSTEM *p)
{
	GX_EntryPoint(p);
    g_pRLX->pV3X->Client = &V3X_OpenGL;
}

