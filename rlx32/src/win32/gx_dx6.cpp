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
#include <windows.h>
#include <winreg.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "_rlx32.h"
#include "_rlx.h"

#include "systools.h"
#include "sysctrl.h"
#include "sysresmx.h"
#include "gx_struc.h"
#include "gx_init.h"
#include "gx_tools.h"
#include "gx_rgb.h"
#include "gx_csp.h"
#include "win32/gx_dx.h"
#include "gx_init.h"
#include "win32/w32_dx.h"

#define GET_GX() g_pRLX->pGX

__extern_c
LPDIRECTDRAW					g_lpDDraw;
LPDIRECTDRAW4					g_lpDD;
LPDIRECTDRAWPALETTE				g_lpDDPalette;
LPDIRECTDRAWSURFACE4			g_lpPrimarySurface;
LPDIRECTDRAWSURFACE4			g_lpDDrawSurfaces[DXMAXVPAGE];
extern struct RLXSYSTEM *	g_pRLX;
__end_extern_c

HWND g_hWnd;
RECT g_cRect;

static rgb32_t					g_lpPalette[256];
static GXDISPLAYMODEINFO     *	g_lpDisplayModeInfo;
static uint8_t               *	g_lpBackBuffer;
static int						g_nDisplayModes, g_nPage;
static LPDIRECTDRAWCLIPPER		g_lpDDClipper;
static int						g_nCurrentDisplayMode=-1;
static int						g_lx, g_ly, g_bpp;
static HDC						g_hDC;

#define GetWindowStyle(h) GetWindowLong(h, GWL_STYLE)
#define GetWindowExStyle(h) GetWindowLong(h, GWL_EXSTYLE)

static void getWindowPixelFormat(void)
{
	PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR) /* size of this pfd */
	};
	g_hDC = GetDC(g_hWnd);
	DescribePixelFormat(g_hDC, GetPixelFormat(g_hDC), sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	GET_GX()->View.ColorMask.RedMaskSize = pfd.cRedBits;
    GET_GX()->View.ColorMask.GreenMaskSize = pfd.cGreenBits;
    GET_GX()->View.ColorMask.BlueMaskSize  = pfd.cBlueBits;
    GET_GX()->View.ColorMask.RsvdMaskSize  = pfd.cAlphaBits;
    GET_GX()->View.ColorMask.RedFieldPosition = pfd.cRedShift;
    GET_GX()->View.ColorMask.GreenFieldPosition = pfd.cGreenShift;
    GET_GX()->View.ColorMask.BlueFieldPosition = pfd.cBlueShift;
    GET_GX()->View.ColorMask.RsvdFieldPosition = pfd.cAlphaShift;

	ReleaseDC(g_hWnd, g_hDC);
}

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
}

static void Unlock(void)
{
    int i = GET_GX()->View.Flags&GX_CAPS_DRAWFRONTBACK ? 0 : 1;
	SYS_ASSERT((GET_GX()->View.State&GX_STATE_LOCKED));
	SYS_ASSERT(g_lpDDrawSurfaces[0]);

    if (GET_GX()->Surfaces.lpSurface[i]!=NULL)
    {
        SYS_DXTRACE(g_lpDDrawSurfaces[i]->Unlock(NULL));
        GET_GX()->Surfaces.lpSurface[i]= NULL;
		GET_GX()->View.lpBackBuffer = 0;
    }
	GET_GX()->View.State&=~GX_STATE_LOCKED;
}

static uint8_t *Lock(void)
{
    int i = GET_GX()->View.Flags&GX_CAPS_DRAWFRONTBACK ? 0 : 1 ;
    DDSURFACEDESC2 ddsd;
	SYS_ASSERT((GET_GX()->View.State&GX_STATE_LOCKED)==0);

    if (g_lpDDrawSurfaces[0])
    {
        sysMemZero(&ddsd, sizeof(DDSURFACEDESC2));
        ddsd.dwSize = sizeof(ddsd);
        if (GET_GX()->Surfaces.lpSurface[i]==NULL)
        {
            HRESULT res;
            while((res = g_lpDDrawSurfaces[i]->Lock(
				NULL,
				&ddsd,
				DDLOCK_WAIT|
				DDLOCK_SURFACEMEMORYPTR, NULL)) == DDERR_WASSTILLDRAWING)
            {
            }
            GET_GX()->Surfaces.lpSurface[i] = (res == DD_OK)
            ? (uint8_t *)ddsd.lpSurface
            : NULL;
            if (res==DD_OK)
				GET_GX()->View.State|=GX_STATE_LOCKED;
            else
				return NULL;
        }
		if (GET_GX()->View.Flags&GX_CAPS_BACKBUFFERINVIDEO)
        {
			GET_GX()->View.lPitch = ddsd.lPitch << ((GET_GX()->View.Flags&GX_CAPS_FBINTERLEAVED)!=0 ? 1 : 0);
            GET_GX()->View.lpBackBuffer = GET_GX()->Surfaces.lpSurface[i];
            GET_GX()->View.lpFrontBuffer = GET_GX()->Surfaces.lpSurface[i^1];
            if (!GET_GX()->View.lpFrontBuffer)
				GET_GX()->View.lpFrontBuffer = GET_GX()->View.lpBackBuffer;
        }
        else
        {
            GET_GX()->View.lpFrontBuffer = GET_GX()->Surfaces.lpSurface[i];
        }
    }
    GET_GX()->View.lpBackBuffer = g_lpBackBuffer ? g_lpBackBuffer : GET_GX()->Surfaces.lpSurface[i];
    return GET_GX()->View.lpBackBuffer;
}

static void CopyPage()
{
    int32_t   ecx = g_ly>>1,
    edx = GET_GX()->View.lPitch;
    uint8_t *edi = GET_GX()->Surfaces.lpSurface[1] + (g_nPage ? edx : 0);
    uint8_t *esi = g_lpBackBuffer;
	int lPitch = GET_GX()->View.lPitch;
    if (GET_GX()->View.Flags&GX_CAPS_FBINTERLEAVESWAP)
    {
        if (g_nPage) ecx--;
        g_nPage^=1;
    }
    for (;ecx!=0;ecx--)
    {
        sysMemCpy(edi, esi, edx);
        esi+=edx;
        edi+=lPitch<<1;
    }
}

static void RestoreSurfaces(void)
{
    HRESULT hr;
    unsigned i;
    if (g_lpDD)
    for (i=0;i<=GET_GX()->Surfaces.maxSurface;i++)
    {
        if (g_lpDDrawSurfaces[i])
        {
            hr = g_lpDDrawSurfaces[i]->Restore();
            if (hr!=DD_OK)
            {
                switch(hr) {
                    case DDERR_WRONGMODE:
                    g_lpDD->RestoreDisplayMode();
                    break;
                    default:
                    break;
                }
            }
        }
    }
    if (g_pRLX->Video.Config&RLXVIDEO_Windowed)
    {
        ShowWindow(g_hWnd, SW_NORMAL);
        UpdateWindow(g_hWnd);
    }
}

static void CALLING_C Flip(void)
{
    HRESULT hr;
    // Flip the surfaces
    if ((GET_GX()->View.Flags&GX_CAPS_BACKBUFFERINVIDEO)==0)
    {
        if (g_lpBackBuffer)
        {
            Lock();
            CopyPage();
            Unlock();
        }
        else
        {
            HRESULT hr;
            RECT rc;
            SetRect(&rc, 0, 0, g_lx, g_ly);
            hr = g_lpDDrawSurfaces[0]->Blt(&rc, g_lpDDrawSurfaces[1], NULL, DDBLT_WAIT, NULL);
            return;
        }
    }
    while( 1 )
    {
        hr = g_lpDDrawSurfaces[0]->Flip(NULL, DDFLIP_WAIT);
        if( hr == DD_OK )
        {
            break;
        }
        else
        {
            if( hr == DDERR_SURFACELOST )
            {
                RestoreSurfaces();
            }
            if( hr != DDERR_WASSTILLDRAWING )
            {
                break;
            }
        }
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
typedef struct {
    RGBQUAD palette[256];
} DIB_Palette;

typedef struct {
 //   BITMAPV4HEADER   bmi4;
	BITMAPINFOHEADER bmi;
    DIB_Palette      pal;
} DIB_Bitmap;

static DIB_Bitmap DIBinf;

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C DIB_setPalette(uint32_t a, uint32_t b, void * pal)
*
* DESCRIPTION :
*
*/
static void CALLING_C DIB_setPalette(uint32_t a, uint32_t b, void * pal)
{
    unsigned int i;
    rgb24_t *palette = (rgb24_t*)pal                + a;
    RGBQUAD   *palx = (RGBQUAD  *)DIBinf.pal.palette + a;
    for (i=0;i<b;i++, palette++, palx++)
    {
        palx->rgbRed = palette->r;
        palx->rgbGreen = palette->g;
        palx->rgbBlue = palette->b;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void DIB_Blit(uint8_t* output, int32_t resx, int32_t resy, int32_t bpp)
*
* DESCRIPTION :
*
*/
static void DIB_Blit(uint8_t* output, int32_t resx, int32_t resy, int32_t bpp)
{
    HDC handle = GetDC(g_hWnd);
    sysMemZero(&DIBinf.bmi, sizeof(BITMAPINFOHEADER));
    DIBinf.bmi.biSize = sizeof(BITMAPINFOHEADER);
    DIBinf.bmi.biWidth =  resx;
    DIBinf.bmi.biHeight =  -resy;
    DIBinf.bmi.biPlanes = 1;
    DIBinf.bmi.biBitCount = bpp==16 ? 15 : (uint8_t)bpp;
    DIBinf.bmi.biCompression = BI_RGB;
    DIBinf.bmi.biXPelsPerMeter = 1;
    DIBinf.bmi.biYPelsPerMeter = 1;
    if ((handle)&&(output))
    {
        int ret = StretchDIBits(
			  handle,
			  0, 0, resx, resy,
			  0, 0, resx, resy,
        (CONST void *)output,
        (CONST BITMAPINFO *)(&DIBinf.bmi),
        GET_GX()->View.BytePerPixel==1 ? DIB_RGB_COLORS : 0,
		SRCCOPY );
		SYS_ASSERT(ret == GDI_ERROR);
        ReleaseDC(g_hWnd, handle);
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C DX_WinBlt(void)
*
* DESCRIPTION :
*
*/
static void CALLING_C DX_WinBlt(void)
{
    uint8_t *v = Lock();
    Unlock();
    DIB_Blit(v, g_lx, g_ly, g_bpp);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C DX_WinDBlt(void)
*
* DESCRIPTION :
*
*/
static void CALLING_C DX_WinDBlt(void)
{
    HRESULT hr;
    RECT    rc;
    GetWindowRect(g_hWnd, &rc);
    hr = g_lpDDrawSurfaces[0]->Blt(&rc, g_lpDDrawSurfaces[1], NULL, DDBLT_WAIT, NULL);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void DX_MaskToVESA(uint32_t mask, uint8_t *pos, uint8_t *size)
*
* DESCRIPTION :
*
*/
static void DX_MaskToVESA(uint32_t mask, uint8_t *pos, uint8_t *size)
{
    *pos=0;
    while (!(  ((mask&1)==1) || (mask==0)    ))
    {
        (*pos)++;
        mask>>=1;
    }
    *size=0;
    while (mask!=0)
    {
        (*size)++;
        mask>>=1;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : GXDISPLAYMODEINFO *EnumDisplayList(int bpp)
*
* DESCRIPTION : Liste des modes disponibles
*
*/
HRESULT CALLBACK GetVideoCallback(LPDDSURFACEDESC pSurf, void *ptr)
{
    GXDISPLAYMODEINFO *pstParam=g_lpDisplayModeInfo + g_nDisplayModes;
    pstParam->lWidth = (uint16_t) pSurf->dwWidth;
    pstParam->lHeight = (uint16_t) pSurf->dwHeight;
    pstParam->BitsPerPixel = (uint8_t)  pSurf->ddpfPixelFormat.dwRGBBitCount;
    g_nDisplayModes+= (g_nDisplayModes < DXMAXLISTABLE-1);
    pstParam->mode = (uint16_t) g_nDisplayModes;
    UNUSED(ptr);
    return DDENUMRET_OK;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : int DX_Install(void)
*
* DESCRIPTION : Installation de Direct Draw
*
*/
int DX_DrawSetup(void)
{
    HRESULT hr;
    GXDISPLAYMODEINFO *p;
    g_nDisplayModes = 0;
    p = g_lpDisplayModeInfo = (GXDISPLAYMODEINFO*) g_pRLX->mm_heap->malloc(DXMAXLISTABLE * sizeof(GXDISPLAYMODEINFO));
    if (g_lpDDraw->EnumDisplayModes(0, NULL, NULL, GetVideoCallback)!=DD_OK ) return 0;
    g_lpDisplayModeInfo = p;
    g_lpDisplayModeInfo[g_nDisplayModes].mode = 0;

	if (g_nDisplayModes == 0)
		return 0;

    if (g_pRLX->V3X.Id == 0)
		return 1;
    {
        DDCAPS ddcaps;
		if (!g_lpDD)
			hr = g_lpDDraw->QueryInterface(IID_IDirectDraw4, (LPVOID *)&g_lpDD);
        if (hr!=DD_OK)
			return 0;
        sysMemZero(&ddcaps, sizeof(DDSCAPS));
        ddcaps.dwSize = sizeof( ddcaps );
        hr = g_lpDD->GetCaps( &ddcaps, NULL );
        GET_GX()->View.lVideoSize = ddcaps.dwVidMemTotal;
    }
    return 1;
    // Set Window
}

typedef struct {
    int       index;
    GUID FAR *lpGUID;
}DX_Context;

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static BOOL WINAPI DDEnumCallback(GUID FAR *lpGUID, char const * lpDriverDesc, char const * lpDriverName, LPVOID lpContext)
*
* Description :
*
*/
static BOOL WINAPI DDEnumCallback(GUID FAR *lpGUID, char const * lpDriverDesc, char const * lpDriverName, LPVOID lpContext)
{
    DX_Context *cx = (DX_Context*)lpContext;
    if (cx->index == g_pRLX->Video.Id)
    {
        cx->lpGUID = lpGUID;
        sysStrnCpy(GET_GX()->Client->s_DrvName, lpDriverName, 64);
    }
    UNUSED(lpDriverDesc);
    cx->index++;
    return TRUE;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int DX_Install(int nCmdShow)
*
* Description :
*
*/
int DX_Install(int nCmdShow)
{
    DX_Context dx;
    HRESULT    hr;
    dx.index = 0;
    dx.lpGUID = NULL;
    DirectDrawEnumerate((LPDDENUMCALLBACKA)DDEnumCallback, &dx);
	if (!g_lpDDraw)
    hr = DirectDrawCreate(dx.lpGUID, &g_lpDDraw, NULL);
    if (hr!=DD_OK) return 0;
    UNUSED(nCmdShow);
    return DX_DrawSetup();
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  GXDISPLAYMODEINFO *EnumDisplayList(int bpp)
*
* DESCRIPTION :
*
*/
GXDISPLAYMODEINFO *EnumDisplayList(int bpp)
{
    GXDISPLAYMODEINFO *lst = (GXDISPLAYMODEINFO*)g_pRLX->mm_heap->malloc(g_nDisplayModes * sizeof(GXDISPLAYMODEINFO)), *ls, *l;
    ls=g_lpDisplayModeInfo;
    l=lst;
    while (ls->mode!=0)
    {
        if (ls->BitsPerPixel==bpp)
        {
            *l = *ls;
            l++;
        }
        ls++;
    }
    return lst;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void SearchDisplayMode(int lx, int ly, int bpp)
*
* DESCRIPTION : Recherche un mode graphique
*
*/
static int DX_GetDesktopColorDepth(HWND hwnd)
{
	HDC hDC = GetDC(hwnd);
	int bpp = GetDeviceCaps(hDC, BITSPIXEL);
	ReleaseDC(hwnd, hDC);
	return bpp;
}

static int SearchDisplayMode(int lx, int ly, int bpp)
{
    GXDISPLAYMODEINFO *ls = g_lpDisplayModeInfo;

    if (g_pRLX->Video.Config&RLXVIDEO_Windowed)
    {
		g_lx = lx;
		g_ly = ly;
		g_bpp = DX_GetDesktopColorDepth(g_hWnd);
        return 1;
    }
	else
	while (ls->mode!=0)
    {
        if ((ls->lWidth == lx)
        && (ls->lHeight == ly)
        && (ls->BitsPerPixel == bpp))
			return ls->mode;
        ls++;
    }
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void SetDisplayMode(int mode)
*
* DESCRIPTION :
*
*/
static int SetDisplayMode(int mode)
{
	if (g_pRLX->Video.Config&RLXVIDEO_Windowed)
	{
		SYS_DXTRACE(g_lpDDraw->SetCooperativeLevel(g_hWnd, DDSCL_NORMAL));
		W32_WindowLayoutSetSize(g_hWnd, &g_cRect, g_lx, g_ly, false, false);
		W32_WindowLayoutSetStyle(g_hWnd, true);
		ShowWindow(g_hWnd, SW_NORMAL);

	}
	else
	{
		SYS_DXTRACE(g_lpDDraw->SetCooperativeLevel(g_hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT));
		W32_WindowLayoutSetSize(g_hWnd, &g_cRect, g_lx, g_ly, true, false);
		W32_WindowLayoutSetStyle(g_hWnd, false);
		ShowWindow(g_hWnd, SW_NORMAL);
		SYS_DXTRACE(g_lpDD->SetDisplayMode(g_lx, g_ly, g_bpp, 0, 0));
	}
    g_nCurrentDisplayMode = mode;
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void ReleaseSurfaces(void)
*
* DESCRIPTION :
*
*/
static void ReleaseSurfaces(void)
{
    int i;
    if (g_lpDDPalette)
    {
        g_lpDDPalette->Release();
        g_lpDDPalette = NULL;
    }

	if (g_lpBackBuffer)
    {
        g_pRLX->mm_std->free(g_lpBackBuffer);
        g_lpBackBuffer = NULL;
    }

    if (g_lpDDClipper)
    {
        g_lpDDClipper->Release();
        g_lpDDClipper = NULL;

    }

	i = DXMAXVPAGE-1;
	do
    {
		if (g_lpDDrawSurfaces[i])
		{
			g_lpDDrawSurfaces[i]->Unlock(NULL);
			g_lpDDrawSurfaces[i]->Release();
			g_lpDDrawSurfaces[i] = NULL;
			GET_GX()->Surfaces.lpSurface[i] = NULL;
		}
		i--;
	}while(i>=0);

    GET_GX()->Surfaces.maxSurface = 0;
    GET_GX()->View.State&=~GX_STATE_LOCKED;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void Shutdown(void)
*
* DESCRIPTION :
*
*/
static void Shutdown(void)
{
    SYS_ASSERT(g_lpDDrawSurfaces[0]);
	SYS_ASSERT(g_lpDD);
    if (g_lpDD)
	{
		g_lpDD->SetCooperativeLevel(g_hWnd, DDSCL_NORMAL);
		if (0<g_lpDD->Release())
			return;
	}
    if (g_lpDDraw)
	{
		if (0<g_lpDDraw->Release())
			return;
	}
    g_lpDDraw = NULL;
    g_lpDD = NULL;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C ClearBackBuffer(void)
*
* DESCRIPTION :
*
*/
static void CALLING_C ClearBackBuffer(void)
{
    int i, b=GET_GX()->View.State&GX_STATE_LOCKED;
    DDBLTFX ddbltfx;
    RECT RSource;
    /* Remplit la structure standard du rectangle */
    RSource.left = 0;
    RSource.top = 0;
    RSource.right = g_lx;
    RSource.bottom = g_ly;
    /* Réalise l'effacement */
    ddbltfx.dwSize = sizeof(ddbltfx);
    ddbltfx.dwFillColor = 0x0;
    if (b)
		Unlock();
    for (i=0;i<2;i++)
    {
        g_lpDDrawSurfaces[1]->Blt(&RSource, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
        if ((g_pRLX->Video.Config&RLXVIDEO_Windowed)==0)
			g_lpDDrawSurfaces[0]->Flip(NULL, DDFLIP_WAIT);
    }
    if (b)
		Lock();
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CreateSurface(int npages)
*
* DESCRIPTION :
*
*/
static int CreateSurface(int npages)
{
    unsigned i, doMask = 0;
    HRESULT         hr;
    DDSURFACEDESC2  ddsd;
    DDSCAPS2        ddscaps;
    DDPIXELFORMAT   ddPix;

	SYS_ASSERT(GET_GX()->Surfaces.maxSurface == 0);
    GET_GX()->Surfaces.maxSurface = npages;

	// Create Primary
    sysMemZero(&ddsd, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	ddsd.dwWidth = g_lx;
	ddsd.dwHeight = g_ly;
	if ((g_pRLX->Video.Config&RLXVIDEO_Windowed)==0)
    {
        ddsd.dwBackBufferCount = GET_GX()->View.Flags&GX_CAPS_FBTRIPLEBUFFERING ? 2 : 1;
        ddsd.dwFlags |= DDSD_BACKBUFFERCOUNT;
        ddsd.ddsCaps.dwCaps   |= DDSCAPS_COMPLEX|DDSCAPS_FLIP;
        if ((g_pRLX->V3X.Id==RLX3D_DIRECT3D)&&(GET_GX()->View.Flags&GX_CAPS_3DSYSTEM))
			ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE | (g_pRLX->V3X.Config&RLX3D_FakeHardware ? DDSCAPS_SYSTEMMEMORY : DDSCAPS_VIDEOMEMORY);
    }
    else
		GET_GX()->View.Flags&=~GX_CAPS_BACKBUFFERINVIDEO;

	SYS_ASSERT(g_lpDDrawSurfaces[0] == 0);
	SYS_DXTRACE(g_lpDD->CreateSurface(&ddsd, g_lpDDrawSurfaces+0, NULL));

    if ((g_pRLX->Video.Config&RLXVIDEO_Windowed)==0)
    {
        sysMemZero(&ddscaps, sizeof(DDSCAPS));
        ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
        SYS_DXTRACE(g_lpDDrawSurfaces[0]->GetAttachedSurface(&ddscaps, g_lpDDrawSurfaces+1));
        i = 1;
        sysMemZero(&ddscaps, sizeof(DDSCAPS));
        hr = g_lpDDrawSurfaces[0]->GetCaps(&ddscaps);
        if (!(GET_GX()->View.Flags&GX_CAPS_BACKBUFFERINVIDEO))
        {
            if (GET_GX()->View.Flags&GX_CAPS_FBINTERLEAVED)
            {
                g_lpBackBuffer = (uint8_t*) g_pRLX->mm_std->malloc(GET_GX()->View.lSurfaceSize);
				SYS_ASSERT(g_lpBackBuffer);
            }
            else
				i = 0;
        }
    }
    else
		i = 0;

    // Create offscreens
    sysMemZero(&ddsd, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.dwWidth = g_lx;
	ddsd.dwHeight = g_ly;

	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    if ((g_pRLX->V3X.Id==RLX3D_DIRECT3D)&&(GET_GX()->View.Flags&GX_CAPS_3DSYSTEM))
        ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE | (g_pRLX->V3X.Config&RLX3D_FakeHardware ? DDSCAPS_SYSTEMMEMORY : DDSCAPS_VIDEOMEMORY);
    else
    if ((GET_GX()->View.Flags&GX_CAPS_BACKBUFFERINVIDEO)==0)
        ddsd.ddsCaps.dwCaps|=DDSCAPS_SYSTEMMEMORY;

	for (;i<GET_GX()->Surfaces.maxSurface;i++)
	{
		GET_GX()->Surfaces.flags[i+1] = 2;
        SYS_DXTRACE(g_lpDD->CreateSurface( &ddsd, g_lpDDrawSurfaces+i+1, NULL));
	}

	// Get Pixel Format
    sysMemZero(&ddPix, sizeof(DDPIXELFORMAT));
    ddPix.dwSize = sizeof(DDPIXELFORMAT);
    ddPix.dwFlags = DDPF_RGB;
    g_lpDDrawSurfaces[0]->GetPixelFormat(&ddPix);

    // Clipper mode fenetre
    if (g_pRLX->Video.Config&RLXVIDEO_Windowed)
    {
        SYS_DXTRACE(g_lpDD->CreateClipper(0UL, &g_lpDDClipper, NULL));
        SYS_DXTRACE(g_lpDDClipper->SetHWnd(0, g_hWnd));
        SYS_DXTRACE(g_lpDDrawSurfaces[0]->SetClipper(g_lpDDClipper));
        GET_GX()->Client->Lock = Lock;
		GET_GX()->Client->Unlock = Unlock;

        if ((GET_GX()->View.Flags&GX_CAPS_3DSYSTEM)==0)
		{
			if ((GET_GX()->View.BitsPerPixel==16)||(GET_GX()->View.BitsPerPixel==15))
			{
				GET_GX()->View.ColorMask.BlueMaskSize = 5;
				GET_GX()->View.ColorMask.GreenMaskSize = 6;
				GET_GX()->View.ColorMask.RedMaskSize = 5;
				GET_GX()->View.ColorMask.RsvdMaskSize = 0;
				GET_GX()->View.ColorMask.BlueFieldPosition = 0;
				GET_GX()->View.ColorMask.GreenFieldPosition = 5;
				GET_GX()->View.ColorMask.RedFieldPosition = 11;
				GET_GX()->View.ColorMask.RsvdFieldPosition = 16;
				doMask = 1;
			}
		}
    }
	if (!doMask)
	{
		if (GET_GX()->View.BitsPerPixel>8)
		{
			DX_MaskToVESA(ddPix.dwRBitMask, &GET_GX()->View.ColorMask.RedFieldPosition, &GET_GX()->View.ColorMask.RedMaskSize);
			DX_MaskToVESA(ddPix.dwGBitMask, &GET_GX()->View.ColorMask.GreenFieldPosition, &GET_GX()->View.ColorMask.GreenMaskSize);
			DX_MaskToVESA(ddPix.dwBBitMask, &GET_GX()->View.ColorMask.BlueFieldPosition, &GET_GX()->View.ColorMask.BlueMaskSize);
			DX_MaskToVESA(ddPix.dwRGBAlphaBitMask, &GET_GX()->View.ColorMask.RsvdFieldPosition, &GET_GX()->View.ColorMask.RsvdMaskSize);
		}
		else
		{
			if (g_lpDDPalette)
				g_lpDDPalette->Release();
			g_lpDD->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256, (tagPALETTEENTRY *)g_lpPalette, &g_lpDDPalette, NULL);
			g_lpDDrawSurfaces[0]->SetPalette(g_lpDDPalette);
		}
	}

	if (GET_GX()->View.ColorMask.GreenMaskSize == 8)
	{
		GET_GX()->View.ColorMask.RsvdFieldPosition = 24;
		GET_GX()->View.ColorMask.RsvdMaskSize = 8;
	}

	GET_GX()->View.RGB_Magic =
          (((1L<<GET_GX()->View.ColorMask.RedMaskSize  )-2L) << GET_GX()->View.ColorMask.RedFieldPosition   )
        | (((1L<<GET_GX()->View.ColorMask.GreenMaskSize)-2L) << GET_GX()->View.ColorMask.GreenFieldPosition )
        | (((1L<<GET_GX()->View.ColorMask.BlueMaskSize )-2L) << GET_GX()->View.ColorMask.BlueFieldPosition  );

	g_lpPrimarySurface = g_lpDDrawSurfaces[1];
    GET_GX()->View.State &= ~GX_STATE_BACKBUFFERPAGE;
    ClearBackBuffer();
    Lock();
    Unlock();
	SYS_ASSERT(GET_GX()->View.lPitch);

    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION : Fonctions Graphiques
*
*/
static void CALLING_C DX_Idle(void)
{
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C Surfaces_putPage(void *src)
*
* DESCRIPTION :
*
*/
void CALLING_C Surfaces_putPage(void *src)
{
    UNUSED(src);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C Surfaces_getPage(void *src)
*
* DESCRIPTION :
*
*/
void CALLING_C Surfaces_getPage(void *src)
{
    UNUSED(src);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C DX_filledRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
*
* DESCRIPTION :
*
*/
static void CALLING_C DX_filledRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
{
    IDirectDrawSurface4 *Source = g_lpDDrawSurfaces[GET_GX()->View.Flags&GX_CAPS_DRAWFRONTBACK?0:1];
    DDBLTFX ddbltfx;
    RECT RSource;
    int b = GET_GX()->View.State&GX_STATE_LOCKED;
    if (!Source)
		return;
    /* Remplit la structure standard du rectangle */
    RSource.left = x1;
    RSource.top = y1<<((GET_GX()->View.Flags&GX_CAPS_FBINTERLEAVED)!=0 ? 1 : 0);
    RSource.right = 1+x2;
    RSource.bottom = (1+y2)<<((GET_GX()->View.Flags&GX_CAPS_FBINTERLEAVED)!=0 ? 1 : 0);
    /* Réalise l'effacement */
    ddbltfx.dwSize = sizeof(ddbltfx);
    ddbltfx.dwFillColor = color;
    // Blit
    if (b) Unlock();
    Source->Blt(&RSource, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
    if (b) Lock();
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C DX_Clear(void)
*
* DESCRIPTION :
*
*/
static void CALLING_C DX_Clear(void)
{
    IDirectDrawSurface4 *Source = g_lpDDrawSurfaces[1];
    DDBLTFX ddbltfx;
    RECT RSource;
    int b = GET_GX()->View.State&GX_STATE_LOCKED;
    if(!Source) return;
    /* Remplit la structure standard du rectangle */
    RSource.left = 0;
    RSource.top = 0;
    RSource.right = g_lx;
    RSource.bottom = g_ly;
    /* Réalise l'effacement */
    ddbltfx.dwSize = sizeof(ddbltfx);
    ddbltfx.dwFillColor = 0x0;
    if (b) Unlock();
    Source->Blt(&RSource, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
    if (b) Lock();
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C DX_Blit(uint32_t dest, uint32_t src)
*
* DESCRIPTION :
*
*/
static void CALLING_C DX_Blit(uint32_t dest, uint32_t src)
{
    int ps = (src ==0) ? 1 : src ;
    int pd = (dest==0) ? 1 : dest;
    HRESULT hr;
    IDirectDrawSurface4 *Destination=g_lpDDrawSurfaces[pd],
    *Source =g_lpDDrawSurfaces[ps];
    RECT RSource, RDest;
    int b = GET_GX()->View.State&GX_STATE_LOCKED;
    if((!Source)||(!Destination))
		return;
    /* Constitue les rectangles sources et destination */
    RSource.left = 0;
    RSource.top = 0;
    RSource.right = g_lx;;
    RSource.bottom = g_ly;
    RDest = RSource;
    // Blit
    if (b)
		Unlock();
    hr = Destination->Blt(&RSource, Source, &RSource, FALSE, NULL);
    // Fix 3DFx, copie manuel
    if (hr == DDERR_NOBLTHW)
    {
        DDSURFACEDESC2 ddsd1;
        DDSURFACEDESC2 ddsd2;
        unsigned lx;
        sysMemZero(&ddsd1, sizeof(DDSURFACEDESC2));
        ddsd1.dwSize = sizeof(ddsd1);
        sysMemZero(&ddsd2, sizeof(DDSURFACEDESC2));
        ddsd2.dwSize = sizeof(ddsd2);
        hr = Source->Lock( NULL, &ddsd1, DDLOCK_READONLY  | DDLOCK_WAIT, NULL);
        hr = Destination->Lock(  NULL, &ddsd2, DDLOCK_WRITEONLY | DDLOCK_WAIT, NULL);
        lx = min(ddsd1.lPitch, ddsd2.lPitch);
        if (lx)
        {
            uint8_t *a, *b;
            int i;
            for (a=(uint8_t*)ddsd2.lpSurface,
            b=(uint8_t*)ddsd1.lpSurface,
            i=RSource.top;
            i<RSource.bottom;
            i++,
            a+=ddsd2.lPitch,
            b+=ddsd1.lPitch) sysMemCpy(a, b, lx);
        }
        Source->Unlock(NULL);
        Destination->Unlock(NULL);
    }
    if (b) Lock();
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void CALLING_C DX_SoftClear(void)
*
* DESCRIPTION :
*
*/
static void CALLING_C DX_SoftClear(void)
{
    sysMemZero(GET_GX()->View.lpBackBuffer, GET_GX()->View.lSurfaceSize);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
__extern_c
void V3XRef_HardwareRegister(int bpp);
__end_extern_c

static int RegisterMode(int mode)
{

    GET_GX()->View.DisplayMode = (uint16_t)mode;
    GET_GX()->Client->GetDisplayInfo(mode);
	V3XRef_HardwareRegister(GET_GX()->View.BytePerPixel);
    return GET_GX()->Client->SetDisplayMode(mode);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void UploadSprite(GXSPRITE *sp)
*
* DESCRIPTION :
*
*/
static void UploadSprite(GXSPRITE *sp, rgb24_t *colorTable, int bpp)
{
	GXSPRITESW *p = (GXSPRITESW*) g_pRLX->mm_heap->malloc(sizeof(GXSPRITESW));
	int i;
	if (bpp == 3)
	{
		int BytePerPixel = GET_GX()->View.BytePerPixel;
		uint8_t * src_buf = (uint8_t*)g_pRLX->mm_std->malloc(sp->LX * sp->LY * BytePerPixel);
		g_pRLX->pfSmartConverter(src_buf, NULL, BytePerPixel,
								 sp->data, colorTable, bpp, sp->LX*sp->LY);
		g_pRLX->mm_heap->free(sp->data);
		sp->data = src_buf;
		bpp = BytePerPixel;
	}
	else
	if (bpp == 1)
	{
		for (i=0;i<256;i++)
			p->palette[i] = g_pRLX->pfSetPixelFormat(colorTable[i].r, colorTable[i].g, colorTable[i].b);
	}
	else
	{
		SYS_ASSERT(bpp);
	}
	sp->handle = p;
	p->bpp = bpp;
    UNUSED(sp);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void ReleaseSprite(GXSPRITE *sp)
*
* DESCRIPTION :
*
*/
static void ReleaseSprite(GXSPRITE *sp)
{
	GXSPRITESW *p = (GXSPRITESW*) sp->handle;
	g_pRLX->mm_heap->free(p);
	sp->data = NULL;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  unsigned UpdateSprite(GXSPRITE *sp, const uint8_t *bitmap, const rgb24_t *colorTable)
*
* DESCRIPTION :
*
*/
static unsigned UpdateSprite(GXSPRITE *sp, const uint8_t *bitmap, const rgb24_t *colorTable)
{
	GXSPRITESW *p = (GXSPRITESW*) sp->handle;
	int i;
	for (i=0;i<256;i++)
		p->palette[i] = g_pRLX->pfSetPixelFormat(colorTable[i].r, colorTable[i].g, colorTable[i].b);
	sysMemCpy(sp->data, bitmap, sp->LX * sp->LY);
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void GX_SetPrimitive(void)
*
* DESCRIPTION :
*
*/
static void CALLING_C DX_setPalette(uint32_t firstReg, uint32_t numRegs, void *paletteData)
{
    PALETTEENTRY    pe[256];
    PALETTEENTRY   *p;
    rgb24_t      *pl=(rgb24_t*)paletteData;
    unsigned        i;
    if (!g_lpDDPalette)
		return;
    for (i=numRegs, p=pe;i!=0;p++, pl++, i--)
    {
        p->peFlags = PC_NOCOLLAPSE|PC_RESERVED;
        p->peRed = pl->r;
        p->peGreen = pl->g;
        p->peBlue = pl->b;
    }
    if ((GET_GX()->View.Flags&GX_CAPS_VSYNC)&&(!(g_pRLX->Video.Config&RLXVIDEO_Windowed)))
		SYS_DXTRACE(g_lpDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL));
    SYS_DXTRACE(g_lpDDPalette->SetEntries(0, firstReg, numRegs, (PALETTEENTRY*)pe));
}

static void SetPrimitive(void)
{
    GX_GetGraphicInterface(GET_GX());
    GET_GX()->gi.clearBackBuffer = DX_SoftClear;
    GET_GX()->gi.clearVideo = ClearBackBuffer;
    GET_GX()->gi.blit = DX_Blit;
    GET_GX()->gi.waitDrawing = DX_Idle;
    if (GET_GX()->View.BitsPerPixel==8)
		GET_GX()->gi.setPalette = (g_pRLX->Video.Config&RLXVIDEO_Windowed) ? DIB_setPalette : DX_setPalette;

    if ((GET_GX()->View.Flags&GX_CAPS_BACKBUFFERINVIDEO)&&((g_pRLX->Video.Config&RLXVIDEO_Windowed)==0))
    {
        GET_GX()->gi.clearBackBuffer = DX_Clear;
        GET_GX()->gi.blit = DX_Blit;
        if ((GET_GX()->View.Flags&GX_CAPS_FBINTERLEAVED)==0)
			GET_GX()->gi.drawFilledRect = DX_filledRect;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void GetDisplayInfo(int mode)
*
* DESCRIPTION :
*
*/
static void GetDisplayInfo(int mode)
{
    if (g_pRLX->Video.Config&RLXVIDEO_Windowed)
    {
		g_pRLX->pfSetViewPort(&g_pRLX->pGX->View, g_lx, g_ly, g_bpp);
		SetPrimitive();
		if (g_pRLX->V3X.Id)
		{
			GET_GX()->View.Flip = DX_WinDBlt;
		}

    }
    else
    {
		GXDISPLAYMODEINFO *ls = g_lpDisplayModeInfo;
		while (ls->mode!=0)
		{
			if (ls->mode == mode)
				break;
			ls++;
		}
		SYS_ASSERT(ls->mode == mode);
		g_pRLX->pfSetViewPort(&g_pRLX->pGX->View, ls->lWidth, ls->lHeight, ls->BitsPerPixel);
		SetPrimitive();
		if (g_pRLX->V3X.Id)
			GET_GX()->View.Flip = Flip;
		g_lx = ls->lWidth;
		g_ly = ls->lHeight;
		g_bpp = ls->BitsPerPixel;
    }
}

static unsigned NotifyEvent(enum GX_EVENT_MODE mode, int x, int y)
{
    UNUSED(mode);
    UNUSED(x);
    UNUSED(y);
    return mode;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int Open(void *hwnd)
*
* Description :
*
*/
static int Open(void *hwnd)
{
	g_hWnd = (HWND)hwnd;

	if (g_pRLX->V3X.Id == 0)
		return 1;

    if (!DX_Install(0))
		return 0;

    return 1;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : GXCLIENTDRIVER DX_SubSystem
*
* DESCRIPTION :
*
*/
GXCLIENTDRIVER GX_DDraw = {
    Lock,
    Unlock,
    EnumDisplayList,
    GetDisplayInfo,
    SetDisplayMode,
    SearchDisplayMode,
    CreateSurface,
    ReleaseSurfaces,
    UploadSprite,
    ReleaseSprite,
    UpdateSprite,
    RegisterMode,
    Shutdown,
    Open,
    NotifyEvent,
	"Direct3D"
};

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void GET_GX()->ClientEntryPoint(void)
*
* DESCRIPTION :
*
*/
void GX_EntryPoint(struct RLXSYSTEM *p)
{
	g_pRLX = p;
	GET_GX()->Client = &GX_DDraw;
}

