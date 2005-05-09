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

Original pSrc: 1996 - Stephane Denis
Prepared for public release: 02/24/2004 - Stephane Denis, realtech VR
*/
//-------------------------------------------------------------------------

#include <windows.h>
#include <math.h>
#include <ddraw.h>
#include <d3d.h>
#include <stdio.h>
#include <stdlib.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "fixops.h"
#include "systools.h"
#include "sysresmx.h"

#include "gx_struc.h"
#include "gx_init.h"
#include "gx_tools.h"
#include "gx_csp.h"
#include "gx_rgb.h"
#include "v3x"

#include "win32/gx_dx.h"
#include "win32/w32_dx.h"
#include "v3xrend.h"
#include "d3ddrv.h"

void  (* CALLING_C old_drawAnyLine)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour);
#define FLOATtoDWORD(a) (*(DWORD*)&(a))

extern HWND g_hWnd;

typedef struct {
    int       index;
    GUID FAR *lpGUID;
}DX_Context;

static char szSwapPath[_MAX_PATH];

__extern_c
struct RLXSYSTEM *g_pRLX;
__end_extern_c

static GXCLIENTDRIVER GX_Direct3D;
extern GXCLIENTDRIVER GX_DDraw;


static unsigned             pipe_iRender;
static unsigned             pipe_iBlend;
static u_int8_t               *pipe_pTex[4];

static LPDIRECT3D3          g_lpD3D = NULL;
static LPDIRECT3DDEVICE3    g_lpDevice = NULL;
static LPDIRECTDRAWSURFACE4 lpddZBuffer = NULL;
static LPDIRECT3DVIEWPORT3  lpd3dViewport3 = NULL;

static D3DHARDRIVER         g_cDeviceInfo;
static u_int32_t                D3D_SpecularValue;
static D3DVALUE             D3D_FogFactor, D3D_ClipFactor;
static DDCOLORKEY           D3D_ColorKey;
static LPD3DTLVERTEX2       g_lpVertexBuffer = NULL;

static LPDIRECT3DMATERIAL3  g_lpBackgroundMaterial, g_lpLightingMaterial;
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void D3D_TextureSwapReset(void)
*
* DESCRIPTION :
*
*/
void D3D_TextureSwapReset(void)
{
    if (!szSwapPath[0])
    {
		static char szSwap[_MAX_PATH];
		GetTempPath(_MAX_PATH, szSwap);
		GetTempFileName(szSwap, "vxswp", 1, szSwapPath);
    }
    DeleteFile(szSwapPath);
    D3D_TextureSwapPut((u_int8_t*)szSwapPath, strlen(szSwapPath));
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  u_int32_t D3D_TextureSwapPut(u_int8_t *buffer, u_int32_t size)
*
* DESCRIPTION :
*
*/
u_int32_t D3D_TextureSwapPut(u_int8_t *buffer, u_int32_t size)
{
    FILE *in = fopen(szSwapPath, "a+b");
    u_int32_t offset=0;
	SYS_ASSERT(in);
    if (in)
    {
        offset = ftell(in);
        fwrite(buffer, sizeof(char), size, in);
		fclose(in);
    }    
    return offset;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void D3D_TextureSwapGet(u_int8_t *dest, u_int32_t offset)
*
* DESCRIPTION :
*
*/
void D3D_TextureSwapGet(u_int8_t *dest, u_int32_t offset, u_int32_t size)
{
    FILE *in=fopen(szSwapPath, "rb");
    if (in)
    {
        if (!fseek(in, offset, SEEK_SET))
        {
            fread(dest, sizeof(char), size, in);
        }
    }
    fclose(in);
    return;
}

static void ZbufferFree(void)
{
    if (lpddZBuffer)
    {
        lpddZBuffer->Release();
        lpddZBuffer = NULL;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void ZbufferAlloc(void)
*
* DESCRIPTION :
*
*/
static int ZbufferAlloc(void)
{
    DDSURFACEDESC2   ddsd;
    LPD3DDEVICEDESC lpd3dDeviceDesc = g_cDeviceInfo.fIsHardware ? &g_cDeviceInfo.d3dHWDeviceDesc : &g_cDeviceInfo.d3dSWDeviceDesc;
    if (lpddZBuffer) 
		return 0;

	sysMemZero(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | (g_cDeviceInfo.fIsHardware ? DDSCAPS_VIDEOMEMORY : DDSCAPS_SYSTEMMEMORY);
    ddsd.dwWidth = g_pRLX->pGX->View.lWidth;
    ddsd.dwHeight = g_pRLX->pGX->View.lHeight;
    ddsd.ddpfPixelFormat = g_cDeviceInfo.ddpfZBuffer;
    
    if (SYS_DXTRACE( g_lpDD->CreateSurface(&ddsd, &lpddZBuffer, NULL)))
    {
        g_pRLX->pV3X->Client->Capabilities&=~GXSPEC_ENABLEZBUFFER;		
        return 0;
    }

    if (SYS_DXTRACE(g_lpPrimarySurface->AddAttachedSurface(lpddZBuffer)))
    {
        ZbufferFree();
        g_pRLX->pV3X->Client->Capabilities&=~GXSPEC_ENABLEZBUFFER;
        return 0;
    }
    return 1;
}

static unsigned ZbufferClear(rgb24_t *color, V3XSCALAR z, void *bitmap)
{
    D3DRECT dummy;
    int b = g_pRLX->pGX->View.State&GX_STATE_LOCKED;
    DWORD   flags =  D3DCLEAR_ZBUFFER;
    sysMemZero(&dummy, sizeof(D3DRECT));
    dummy.x1 = g_pRLX->pGX->View.xmin;
    dummy.y1 = g_pRLX->pGX->View.ymin;
    dummy.x2 = g_pRLX->pGX->View.xmax+1;
    dummy.y2 = g_pRLX->pGX->View.ymax+1;
	if (b) g_pRLX->pGX->Client->Unlock();
    SYS_DXTRACE( lpd3dViewport3->Clear(1, &dummy, flags));
	if (b) g_pRLX->pGX->Client->Lock();
    UNUSED(color);
    UNUSED(z);
    UNUSED(bitmap);
    return 2;
}

static void SetLightColor()
{
	DWORD  hLmat;
	D3DMATERIAL material;
	sysMemZero(&material, sizeof(D3DMATERIAL));
	material.dwSize  = sizeof(D3DMATERIAL);

	material.power = 0.0f;
	material.hTexture = 0;
	material.dwRampSize = 16;

	RGB_Set(material.diffuse, (D3DVALUE)1.0f, (D3DVALUE)1.0f, (D3DVALUE)1.0f);
	RGB_Set(material.ambient, (D3DVALUE)1.0f, (D3DVALUE)1.0f, (D3DVALUE)1.0f);
	RGB_Set(material.specular, (D3DVALUE)1.0f, (D3DVALUE)1.0f, (D3DVALUE)1.0f);
	
	SYS_DXTRACE( g_lpD3D->CreateMaterial(&g_lpLightingMaterial, NULL));
	SYS_DXTRACE( g_lpLightingMaterial->SetMaterial(&material));
	SYS_DXTRACE( g_lpLightingMaterial->GetHandle(g_lpDevice, &hLmat));
}

static void SetBackgroundColor(rgb24_t *color)
{
	DWORD hBmat;
	D3DMATERIAL material;
	sysMemZero(&material, sizeof(D3DMATERIAL));    
	material.dwSize = sizeof(D3DMATERIAL);

	material.power = 1.0f;
    material.hTexture = 0;
    material.dwRampSize = 1;

    RGB_Set(material.diffuse, (D3DVALUE)color->r/255.f, (D3DVALUE)color->g/255.f, (D3DVALUE)color->b/255.f);
    RGB_Set(material.ambient, (D3DVALUE)color->r/255.f, (D3DVALUE)color->g/255.f, (D3DVALUE)color->b/255.f);
    RGB_Set(material.specular, (D3DVALUE)0.0f, (D3DVALUE)0.0f, (D3DVALUE)0.0f);

	SYS_DXTRACE(g_lpD3D->CreateMaterial(&g_lpBackgroundMaterial, NULL));  
	SYS_DXTRACE(g_lpBackgroundMaterial->SetMaterial(&material));
    SYS_DXTRACE(g_lpBackgroundMaterial->GetHandle(g_lpDevice, &hBmat));
    SYS_DXTRACE(lpd3dViewport3->SetBackground(hBmat));

    return;
}

static unsigned SetState(unsigned command, u_int32_t value)
{
    switch(command) {
        case V3XCMD_SETBACKGROUNDCOLOR:
        {
            rgb24_t *cl=(rgb24_t*)value;
            g_pRLX->pV3X->ViewPort.backgroundColor = RGB_MAKE(cl->r, cl->g, cl->b);
        }
        break;
        case V3XCMD_SETZBUFFERSTATE:
        {
            (value) ? g_pRLX->pV3X->Client->Capabilities|=GXSPEC_ENABLEZBUFFER : g_pRLX->pV3X->Client->Capabilities&=~GXSPEC_ENABLEZBUFFER;
        }
        return 1;
        case V3XCMD_SETWBUFFERSTATE:
        {
            if (g_pRLX->pV3X->Client->Capabilities&GXSPEC_WBUFFER)
            {
                (value) ? g_pRLX->pV3X->Client->Capabilities|=GXSPEC_ENABLEWBUFFER : g_pRLX->pV3X->Client->Capabilities&=~GXSPEC_ENABLEWBUFFER;
            }
        }
        return 1;
    }
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void Reset(void)
*
* DESCRIPTION : Reset hardware (obsolete)
*
*/
void Reset(void)
{
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : int  HardwareSetup(void)
*
* DESCRIPTION : Initialises Direct3D (-1 si probleme (pas de Direct3D, Pmode/W etc..)
*
*/
static long FAR PASCAL D3DAppDEVEnumDeviceCallback(
LPGUID          lpGUID, 
LPSTR           lpszDeviceDesc, 
LPSTR           lpszDeviceName, 
LPD3DDEVICEDESC lpd3dHWDeviceDesc, 
LPD3DDEVICEDESC lpd3dSWDeviceDesc, 
LPVOID          lpUserArg)
{
    int pref=0;
    BOOL            fIsHardware = (0 != lpd3dHWDeviceDesc->dcmColorModel);
    LPD3DDEVICEDESC lpd3dDeviceDesc = fIsHardware ? lpd3dHWDeviceDesc: lpd3dSWDeviceDesc;
    if (fIsHardware)
    {
        if (lpd3dDeviceDesc->dpcTriCaps.dwTextureCaps) 
			pref = 1;
    }
    else
    {
        if (D3DCOLOR_MONO != lpd3dDeviceDesc->dcmColorModel) 
			pref = 2;
    }
    if (pref)
    {
        g_cDeviceInfo.fIsHardware = fIsHardware;
        sysMemCpy(&g_cDeviceInfo.guidDevice, lpGUID, sizeof(GUID));
        sysMemCpy(&g_cDeviceInfo.d3dHWDeviceDesc, lpd3dHWDeviceDesc, sizeof(D3DDEVICEDESC));
        sysMemCpy(&g_cDeviceInfo.d3dSWDeviceDesc, lpd3dSWDeviceDesc, sizeof(D3DDEVICEDESC));
        sysStrnCpy(g_cDeviceInfo.szDeviceDesc, lpszDeviceDesc, 64);
        sysStrnCpy(g_cDeviceInfo.szDeviceName, lpszDeviceName, 64);
        if (g_cDeviceInfo.fIsHardware)
            return D3DENUMRET_CANCEL;
    }
    // Otherwise, keep looking.
    return D3DENUMRET_OK;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static BOOL FAR PASCAL D3DAppIDDEnumCallback(GUID FAR* lpGUID, LPSTR lpDriverDesc, LPSTR lpDriverName, LPVOID lpContext)
*
* Description :  
*
*/
static BOOL FAR PASCAL D3DAppIDDEnumCallback(GUID FAR* lpGUID, LPSTR lpDriverDesc, LPSTR lpDriverName, LPVOID lpContext)
{
    LPDIRECTDRAW lpDD = NULL;
    DDCAPS ddHwDesc, ddSwDesc;
    /*
    * A NULL GUID* indicates the DirectDraw HEL which we are not interested
    * in at the moment.
    */
    if (lpGUID)
    {
        /*
        * Create the DirectDraw device using this driver.  If it fails, 
        * just move on to the next driver.
        */
        if (SYS_DXTRACE(DirectDrawCreate(lpGUID, &lpDD, NULL)))
        {  
            if (lpDD) 
				lpDD->Release();
            return DDENUMRET_OK;
        }
        /*
        * Get the capabilities of this DirectDraw driver.  If it fails, 
        * just move on to the next driver.
        */
        sysMemZero(&ddHwDesc, sizeof(DDCAPS));
        sysMemZero(&ddSwDesc, sizeof(DDCAPS));
        ddHwDesc.dwSize = sizeof(DDCAPS);
        ddSwDesc.dwSize = sizeof(DDCAPS);
        if (SYS_DXTRACE(lpDD->GetCaps(&ddHwDesc, &ddSwDesc)))
        {
            lpDD->Release();
            return DDENUMRET_OK;
        }
        if (ddHwDesc.dwCaps & DDCAPS_3D)
        {
            sysStrnCpy(g_pRLX->pGX->Client->s_DrvName, lpDriverName, 63);
            g_cDeviceInfo.bIsPrimary = FALSE;
            g_lpDDraw = lpDD;
            return DDENUMRET_CANCEL;
        }        
        lpDD->Release();
        lpDD = NULL;
    }
    else
    {
        UNUSED(lpDriverDesc);
    }
    return DDENUMRET_OK;
}
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
        sysStrnCpy(g_pRLX->pGX->Client->s_DrvName, lpDriverName, 63);
    }
    UNUSED(lpDriverDesc);
    cx->index++;
    return TRUE;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int  HardwareSetup(void)
*
* Description :  
*
*/
static int HardwareSetup(void)
{
    unsigned n;

    if(g_lpDDraw) 
	  g_lpDDraw->Release();

    g_lpDDraw = NULL;

	if (g_pRLX->V3X.Config & RLX3D_AutoDetect )
        SYS_DXTRACE( DirectDrawEnumerate(D3DAppIDDEnumCallback, &g_lpDDraw));

	if (!g_lpDDraw)
    {
        DX_Context dx;
        dx.index = 0;
        dx.lpGUID = NULL;
        DirectDrawEnumerate((LPDDENUMCALLBACKA)DDEnumCallback, &dx);
        SYS_DXTRACE( DirectDrawCreate(dx.lpGUID, &g_lpDDraw, NULL));
    }
    if (DX_DrawSetup()==0) 
		return -2;

    if (SYS_DXTRACE(g_lpDD->QueryInterface(IID_IDirect3D3, (LPVOID*)&g_lpD3D))) 
		return -4;

	if (SYS_DXTRACE(g_lpD3D->EnumDevices(D3DAppDEVEnumDeviceCallback, NULL))) 
		return -5;
    else
    {
        LPD3DDEVICEDESC lpd3dDeviceDesc = g_cDeviceInfo.fIsHardware ? &g_cDeviceInfo.d3dHWDeviceDesc : &g_cDeviceInfo.d3dSWDeviceDesc;
        g_cDeviceInfo.desc = lpd3dDeviceDesc;        
        if (!g_cDeviceInfo.fIsHardware)
        {
            g_pRLX->V3X.Config|=RLX3D_FakeHardware;
            g_pRLX->pV3X->Client->Capabilities&=~(GXSPEC_ENABLEFILTERING|GXSPEC_HARDWAREBLENDING);
            g_pRLX->pV3X->Client->reduce = 0;
        }
        else
        {
            g_pRLX->pV3X->Client->Capabilities|=GXSPEC_ENABLEFILTERING;
            g_pRLX->pV3X->Client->Capabilities|=GXSPEC_OPACITYTRANSPARENT|GXSPEC_HARDWAREBLENDING;
        }
        if (lpd3dDeviceDesc->dpcTriCaps.dwDestBlendCaps&D3DPBLENDCAPS_ONE)
            g_pRLX->pV3X->Client->Capabilities|=GXSPEC_ALPHABLENDING_ADD;

		if (lpd3dDeviceDesc->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE)
            g_pRLX->pV3X->Client->Capabilities|=GXSPEC_ENABLEPERSPECTIVE;

		if ( lpd3dDeviceDesc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_WBUFFER )
            g_pRLX->pV3X->Client->Capabilities|=GXSPEC_WBUFFER;

		n = lpd3dDeviceDesc->dwMaxTextureWidth;		
		g_pRLX->pV3X->Client->texMaxSize = 0;
		while (n!=0)  {g_pRLX->pV3X->Client->texMaxSize++;n>>=1;}
		g_pRLX->pV3X->Client->texMaxSize--;

		if (lpd3dDeviceDesc->dwDeviceZBufferBitDepth)
            g_pRLX->pV3X->Client->Capabilities|=GXSPEC_ZBUFFER;
        
		if (g_pRLX->V3X.Config&RLX3D_UseMultiTex)
        {
            if (lpd3dDeviceDesc->wMaxSimultaneousTextures>=2)
                g_pRLX->pV3X->Client->Capabilities|= GXSPEC_MULTITEXTURING;
            else 
				g_pRLX->pV3X->Client->Capabilities&= ~GXSPEC_MULTITEXTURING;				
        }

		if (g_pRLX->V3X.Config&RLX3D_UseAGP)
        {
            if (lpd3dDeviceDesc->dwDevCaps&D3DDEVCAPS_TEXTURENONLOCALVIDMEM)
            {
                g_pRLX->pV3X->Client->reduce = 0;
            }
        }
        sprintf(g_pRLX->pV3X->Client->s_DrvName, "Direct3D 6/%s", 
			g_cDeviceInfo.szDeviceName);
    }
    // Vertex temporaires pour les rendus D3D
    n = 256;
    if (n < g_pRLX->pV3X->Ln.maxLines) 
		n = g_pRLX->pV3X->Ln.maxLines;
    g_lpVertexBuffer = (D3DTLVERTEX2*) g_pRLX->mm_std->malloc((n+2)*sizeof(D3DTLVERTEX2));
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void  HardwareShutdown(void)
*
* DESCRIPTION : Close Direct3D
*
*/
static void HardwareShutdown(void)
{
    if (g_lpVertexBuffer)
    {
        g_pRLX->mm_std->free(g_lpVertexBuffer);
        g_lpVertexBuffer = NULL;
    }
    ZbufferFree();
    if (g_lpD3D)
    {
        g_lpD3D->Release();
		if (g_lpDDraw)
		{
			g_lpDDraw->Release();
			g_lpDDraw = NULL;
		}
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void  FreeTexture(u_int8_t *id)
*
* DESCRIPTION : Free texture
*
*/
static void FreeTexture(void *id)
{
    V3XD3DHANDLE *handle = (V3XD3DHANDLE*)id;
    if (handle)
    {
        if (handle->lpTextureSurf) 
			handle->lpTextureSurf->Release();

        if (handle->lpTexture)     
			handle->lpTexture->Release();

        g_pRLX->mm_heap->free(handle);

    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static u_int8_t *RGB_ConvertToGrayScale( GXSPRITE *sp, int bp )
*
* DESCRIPTION :  
*
*/
static u_int8_t *RGB_ConvertToGrayScale( GXSPRITE *sp, int bp )
{
    unsigned size = sp->LX * sp->LY;
    u_int8_t *Buffer = (u_int8_t*)g_pRLX->mm_std->malloc(size), *dest = Buffer;
    if (!bp) bp = 1;
    size *= bp;
    switch(bp) {
        case 1:
        {
            u_int8_t *src = (u_int8_t*) sp->data;
            for ( ;size!=0; dest++, src++, size--)
            {
                rgb24_t *c = g_pRLX->pGX->ColorTable + (*src);
                *dest = (u_int8_t)RGB_ToGray(c->r, c->g, c->b);
            }
        }
        break;
        case 2:
        {
            u_int16_t *src = (u_int16_t*) sp->data;
            for (;size!=0; dest++, src++, size--)
            {
                rgb24_t c;
                g_pRLX->pfGetPixelFormat(&c, (u_int32_t)*src);
                *dest = (u_int8_t)RGB_ToGray(c.r, c.g, c.b);
            }
        }
        break;
        case 3:
        {
            rgb24_t *src = (rgb24_t*) sp->data;
            for ( ;size!=0; dest++, src++, size--)
            *dest = (u_int8_t)RGB_ToGray(src->r, src->g, src->b);
        }
        break;
        case 4:
        {
            rgb32_t *src = (rgb32_t*) sp->data;
            for ( ;size!=0; dest++, src++, size--)
            *dest = (u_int8_t)RGB_ToGray(src->r, src->g, src->b);
        }
        break;
    }
    return Buffer;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  LPDIRECTDRAWSURFACE4  D3D_CreateSurfaceFromSprite(GXSPRITE *sp, unsigned bpp)
*
* DESCRIPTION :  
*
*/
static LPDIRECTDRAWSURFACE4 D3D_CreateSurfaceFromSprite(const GXSPRITE *sp, unsigned bpp, D3D_TextureFormat *pF)
{
    DDSURFACEDESC2  ddsd;
    LPDIRECTDRAWSURFACE4 lpDDS = NULL;
    u_int8_t  *src_buf = sp->data;
    unsigned i;
    int  lx = sp->LX;
    int32_t bs = (bpp+1)>>3;
    int32_t bp = (pF->IndexBPP+1)>>3;

	sysMemZero(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags = DDSD_CAPS |  DDSD_HEIGHT |  DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE |  DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = sp->LX;
    ddsd.dwHeight  = sp->LY;
    ddsd.ddpfPixelFormat= pF->pfd;
    
	if (pF->IndexBPP!=4)
    {
        if (bpp==4) 
		{bpp=8;bs=1;}
    }

    if (pF->IndexBPP==4)
    {
        u_int8_t *dest = sp->data, *src;
        i = (sp->LX*sp->LY)>>1;
        src_buf = (u_int8_t*) g_pRLX->mm_std->malloc(i); src = src_buf;
        for (;i!=0;dest+=2, src++, i--) 
			*src = (u_int8_t)((dest[0]&15)+((dest[1]&15)<<4));
        lx>>=1;
    }
    else
    if (pF->IndexBPP==24) // True color texture
    {
        bp = 4;
        lx*= bs;
    }
    else
    {
        if (pF->IndexBPP==12)  // 4444 texture
        {
            bp = 2;			
        }
        // Hi-color texture
        if ((pF->IndexBPP>8)&&(pF->IndexBPP<=16))
        {
            u_int8_t c[8];
            c[0] = g_pRLX->pGX->View.ColorMask.RedMaskSize;
            c[1] = g_pRLX->pGX->View.ColorMask.RedFieldPosition;
            c[2] = g_pRLX->pGX->View.ColorMask.GreenMaskSize;
            c[3] = g_pRLX->pGX->View.ColorMask.GreenFieldPosition;
            c[4] = g_pRLX->pGX->View.ColorMask.BlueMaskSize;
            c[5] = g_pRLX->pGX->View.ColorMask.BlueFieldPosition;
            c[6] = g_pRLX->pGX->View.ColorMask.RsvdMaskSize;
            c[7] = g_pRLX->pGX->View.ColorMask.RsvdFieldPosition;
            g_pRLX->pGX->View.ColorMask.RedMaskSize = pF->RedMaskSize;
            g_pRLX->pGX->View.ColorMask.RedFieldPosition = pF->RedFieldPosition;
            g_pRLX->pGX->View.ColorMask.GreenMaskSize = pF->GreenMaskSize;
            g_pRLX->pGX->View.ColorMask.GreenFieldPosition = pF->GreenFieldPosition;
            g_pRLX->pGX->View.ColorMask.BlueMaskSize  = pF->BlueMaskSize;
            g_pRLX->pGX->View.ColorMask.BlueFieldPosition = pF->BlueFieldPosition;
            g_pRLX->pGX->View.ColorMask.RsvdMaskSize  = pF->RsvdMaskSize;
            g_pRLX->pGX->View.ColorMask.RsvdFieldPosition = pF->RsvdFieldPosition;
            if (bs!=bp)
            {
                src_buf = (u_int8_t*) g_pRLX->mm_std->malloc(sp->LX*sp->LY*bp);
                if (!src_buf)
                {
                    return NULL;
                }
                g_pRLX->pfSmartConverter(src_buf, g_pRLX->pGX->ColorTable, bp, sp->data, g_pRLX->pGX->ColorTable, bs, sp->LX*sp->LY);
                UNUSED(bpp);
            }
            else
            {
				
                if ((bs==2)&&(c[2] != g_pRLX->pGX->View.ColorMask.GreenMaskSize))
                {
                    unsigned szx = sp->LX*sp->LY;
                    u_int16_t *tb2 = (u_int16_t*)src_buf;
				
                    while(szx!=0)
                    {
                        unsigned char r, g, b;
                        r = (u_int8_t)(((tb2[0]>>c[1] ) & ((1<<c[0]) -1)) << (8-c[0] ));
                        g = (u_int8_t)(((tb2[0]>>c[3] ) & ((1<<c[2]) -1)) << (8-c[2] ));
                        b = (u_int8_t)(((tb2[0]>>c[5] ) & ((1<<c[4]) -1)) << (8-c[4] ));
						tb2[0] = (u_int16_t)(g_pRLX->pfSetPixelFormat(r, g, b));
                        szx--;tb2++;
                    }
                }
                src_buf=sp->data;
            }
            // Restore le format de pixel
            g_pRLX->pGX->View.ColorMask.RedMaskSize = c[0];
            g_pRLX->pGX->View.ColorMask.RedFieldPosition = c[1];
            g_pRLX->pGX->View.ColorMask.GreenMaskSize = c[2];
            g_pRLX->pGX->View.ColorMask.GreenFieldPosition = c[3];
            g_pRLX->pGX->View.ColorMask.BlueMaskSize  = c[4];
            g_pRLX->pGX->View.ColorMask.BlueFieldPosition = c[5];
            g_pRLX->pGX->View.ColorMask.RsvdMaskSize  = c[6];
            g_pRLX->pGX->View.ColorMask.RsvdFieldPosition = c[7];
        }
        lx*=bp;
    }
    if (SYS_DXTRACE(g_lpDD->CreateSurface(&ddsd, &lpDDS, NULL)))
		return NULL;

    sysMemZero(&ddsd, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    if (SYS_DXTRACE(lpDDS->Lock( NULL, &ddsd, 0, NULL))) 
		return NULL;

    for (i=0;i<ddsd.dwHeight;i++)
    {
        u_int8_t *ds=(u_int8_t*)ddsd.lpSurface+ddsd.lPitch*i;
        u_int8_t *sr=src_buf+i*lx;
        if ((bs!=bp)&&(bp>2))
        {
            int j;
            for (j=sp->LX;j!=0;j--, ds+=bp, sr+=bs)
            {
                ds[0]=sr[pF->RedFieldPosition ? 2 : 0];
                ds[1]=sr[1];
                ds[2]=sr[pF->RedFieldPosition ? 0 : 2];
            }
        }
        else sysMemCpy(ds, sr, lx);
    }
    if (SYS_DXTRACE(lpDDS->Unlock(NULL))) 
		return NULL;

    if (sp->data!=src_buf)  
		g_pRLX->mm_std->free(src_buf);
    
	if (pF->IndexBPP<=8)
    { // Create la palette source
        DWORD pcaps = 0;
        if (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
        {
            pcaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
        }
        if (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
        {
            pcaps = DDPCAPS_4BIT;
        }
        if (pcaps)
        {
            PALETTEENTRY  ppe[256];
            PALETTEENTRY  *p;
            rgb24_t     *pl = g_pRLX->pGX->ColorTable;
            int i;
            LPDIRECTDRAWPALETTE lpDstPalette = NULL;
            for (i=(pcaps==DDPCAPS_4BIT) ? 16 : 256, p=ppe;i!=0;p++, pl++, i--)
            {
                p->peFlags = PC_NOCOLLAPSE|PC_RESERVED;
                p->peRed = pl->r;
                p->peGreen = pl->g;
                p->peBlue = pl->b;
            }
            SYS_DXTRACE(g_lpDD->CreatePalette(DDPCAPS_INITIALIZE | pcaps, ppe, &lpDstPalette, NULL));
            SYS_DXTRACE(lpDDS->SetPalette(lpDstPalette));
            lpDstPalette->Release();
        }
    }
    return lpDDS;
}


/*------------------------------------------------------------------------
*
* PROTOTYPE  :  LPDIRECT3DTEXTURE2  D3D_UploadTextureFromSurface(V3XD3DHANDLE *handle, unsigned option)
*
* Description :
*
*/
LPDIRECT3DTEXTURE2  D3D_UploadTextureFromSurface(V3XD3DHANDLE *handle, unsigned option)
{
    DWORD               pcaps;
    DDSURFACEDESC2       ddsd;
    LPDIRECTDRAWSURFACE4 lpTextureSurf = NULL;
    LPDIRECT3DTEXTURE2  lpTexture = NULL;
    LPDIRECT3DTEXTURE2  lpSrcTexture = NULL;
    LPDIRECTDRAWSURFACE4 lpSrcTextureSurf = handle->lpTextureSurf;
    // Swap
    if (!handle->offsetSwap)
    {
        sysMemZero(&ddsd, sizeof(DDSURFACEDESC2));
        ddsd.dwSize = sizeof(DDSURFACEDESC2);
        SYS_DXTRACE(lpSrcTextureSurf->GetSurfaceDesc(&ddsd));
        // Format
        handle->offsetSwap = D3D_TextureSwapPut((u_int8_t*)&ddsd, ddsd.dwSize);
        // Palette
        D3D_TextureSwapPut((u_int8_t*)g_pRLX->pGX->ColorTable, 768);
        // Datas
        SYS_DXTRACE(lpSrcTextureSurf->Lock(NULL, &ddsd, DDLOCK_WAIT|DDLOCK_READONLY, NULL));
        D3D_TextureSwapPut((u_int8_t*)ddsd.lpSurface, ddsd.lPitch*ddsd.dwHeight);
        SYS_DXTRACE(lpSrcTextureSurf->Unlock(NULL));
    }
    if (SYS_DXTRACE(lpSrcTextureSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&lpSrcTexture))) 
		return NULL;

	ddsd.dwSize = sizeof(DDSURFACEDESC2);
    SYS_DXTRACE( lpSrcTextureSurf->GetSurfaceDesc(&ddsd));
    ddsd.dwFlags = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_ALLOCONLOAD;
    if (g_pRLX->V3X.Config&RLX3D_TexManager)
        ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_TEXTUREMANAGE;
    else
        ddsd.ddsCaps.dwCaps |= (!g_cDeviceInfo.fIsHardware) ? DDSCAPS_SYSTEMMEMORY : DDSCAPS_VIDEOMEMORY;

	if (option&V3XTEXDWNOPTION_DYNAMIC)
    {
        ddsd.ddsCaps.dwCaps  &= ~(DDSCAPS_SYSTEMMEMORY|DDSCAPS_VIDEOMEMORY);
        ddsd.ddsCaps.dwCaps2 &= ~(DDSCAPS2_HINTSTATIC|DDSCAPS2_OPAQUE);
        ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_HINTDYNAMIC;
    }

    if (SYS_DXTRACE(g_lpDD->CreateSurface(&ddsd, &lpTextureSurf, NULL))) 
		return NULL;

    pcaps = 0;
    if (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
        pcaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;

	if (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
        pcaps = DDPCAPS_4BIT;

	if (pcaps)
    {
        PALETTEENTRY        ppe[256];
        LPDIRECTDRAWPALETTE lpDstPalette = NULL;
        PALETTEENTRY       *p;
        rgb24_t          *pl = g_pRLX->pGX->ColorTable;
        int i;
        for (i=(pcaps==DDPCAPS_4BIT) ? 16 : 256, p=ppe;i!=0;p++, pl++, i--)
        {
            p->peFlags = PC_NOCOLLAPSE|PC_RESERVED;
            p->peRed = pl->r;
            p->peGreen = pl->g;
            p->peBlue = pl->b;
        }
        SYS_DXTRACE(g_lpDD->CreatePalette(pcaps, ppe, &lpDstPalette, NULL));
        SYS_DXTRACE(lpTextureSurf->SetPalette(lpDstPalette));
        lpDstPalette->Release();
    }
    // Convertit les surfaces en TEXTURE
    if (SYS_DXTRACE(lpTextureSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&lpTexture))) 
		return NULL;

    if (SYS_DXTRACE(lpTexture->Load(lpSrcTexture))) 
		return NULL;

	// Check
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
	SYS_DXTRACE(lpTextureSurf->GetSurfaceDesc(&ddsd));

    lpSrcTexture->Release();
    lpSrcTextureSurf->Release();

	handle->lpTextureSurf = lpTextureSurf;
    handle->lpTexture = lpTexture;
    handle->option = option;
	
    if ((option&V3XTEXDWNOPTION_DYNAMIC)&&(!handle->lpSurface3D))
    {
        int32_t lx = ddsd.dwWidth, ly = ddsd.dwHeight;
        sysMemZero(&ddsd, sizeof(DDSURFACEDESC2));
        ddsd.dwSize = sizeof(ddsd);
        ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        ddsd.dwWidth = lx;
        ddsd.dwHeight = ly;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE | (g_pRLX->V3X.Config&RLX3D_FakeHardware ? DDSCAPS_SYSTEMMEMORY : DDSCAPS_VIDEOMEMORY);
        SYS_DXTRACE(g_lpDD->CreateSurface(&ddsd, &handle->lpSurface3D, NULL));
    }
    return lpTexture;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int  D3DSelectTextureFormat(int prefBpp)
*
* DESCRIPTION :
*
*/
static int D3DSelectTextureFormat(int prefBpp, D3D_TextureFormat **pF)
{
    int i;
    D3D_TextureFormat *pTexFormat = g_cDeviceInfo.TextureFormat;
	*pF = NULL;
    for (i=g_cDeviceInfo.NumTextureFormats;(i!=0)&&(*pF==NULL);pTexFormat++, i--)
    {
        if (prefBpp==32)
        {
            if ((pTexFormat->RedMaskSize+pTexFormat->BlueMaskSize+pTexFormat->GreenMaskSize==24)&&(pTexFormat->bPalettized & 2))
            {
                *pF = pTexFormat;
            }
        }
        else
        if ((prefBpp>8)&&(pTexFormat->RedMaskSize+pTexFormat->BlueMaskSize+pTexFormat->GreenMaskSize==prefBpp))
        {            
			if (((prefBpp==12)&&(pTexFormat->bPalettized & 2))||(prefBpp!=12))
				*pF = pTexFormat;

        }
        else
        if ((prefBpp<=8)&&(pTexFormat->IndexBPP==prefBpp)&&(pTexFormat->bPalettized&1)&& (!(g_pRLX->V3X.Config&RLX3D_NoPalettizedTex)))
        {
            *pF = pTexFormat;
        }
    }
	return !!(*pF);
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int D3D_ChooseBestTexFormat(int bpp)
*
* DESCRIPTION :
*
*/
static int D3D_ChooseBestTexFormat(int bpp, D3D_TextureFormat **pF)
{
    if ((g_pRLX->V3X.Config&RLX3D_NoPalettizedTex)&&(bpp<=8))
        bpp = 16;

	if (g_pRLX->V3X.Config&RLX3D_FullQualityTex)
	{
		if (D3DSelectTextureFormat(bpp, pF)) 
			return bpp;
	}    
	switch(bpp) {
        case 4:
        if (D3DSelectTextureFormat(4, pF)) return 4;		
        case 8:
        if (D3DSelectTextureFormat(8, pF)) return 8;		
        case 15:
        if (D3DSelectTextureFormat(15, pF)) return 15;		
        case 16:
        if (D3DSelectTextureFormat(16, pF)) return 16;
        if (D3DSelectTextureFormat(15, pF)) return 15;
        case 24:
		if (D3DSelectTextureFormat(16, pF)) return 16;
        if (D3DSelectTextureFormat(24, pF)) return 24;        		
        if (D3DSelectTextureFormat(15, pF)) return 15;
        case 32:      
        if (D3DSelectTextureFormat(12, pF)) return 12; 
		if (D3DSelectTextureFormat(32, pF)) return 32;        
        break;		
    }
	if (D3DSelectTextureFormat(bpp, pF)) return bpp;
	
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  u_int8_t  *TextureUpload(GXSPRITE *sp, int bpp)
*
* DESCRIPTION :
*
*/
static void V3XAPI *UploadTexture(const GXSPRITE *sp, const rgb24_t *colorTable, int bpp, unsigned options)
{
	D3D_TextureFormat *pF = 0;
    V3XD3DHANDLE *handle = (V3XD3DHANDLE*) g_pRLX->mm_heap->malloc(sizeof(V3XD3DHANDLE));    
    if (options & V3XTEXDWNOPTION_DYNAMIC)
        D3D_ChooseBestTexFormat(g_pRLX->pGX->View.ColorMask.RedMaskSize+g_pRLX->pGX->View.ColorMask.GreenMaskSize+g_pRLX->pGX->View.ColorMask.BlueMaskSize, &pF);
    else
        D3D_ChooseBestTexFormat(bpp, &pF);

    handle->lpTextureSurf = D3D_CreateSurfaceFromSprite(sp, bpp, pF);
    if (!handle->lpTextureSurf) 
		return NULL;

	handle->lpTexture = D3D_UploadTextureFromSurface(handle, options);
    if (!handle->lpTexture)
    {
        FreeTexture(handle);
        return NULL;
    }
    return (u_int8_t*)handle;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void  RestoreTexture(V3XD3DHANDLE *handle)
*
* DESCRIPTION :
*
*/
static void RestoreTexture(V3XD3DHANDLE *handle, int fromDisk)
{
    DDSURFACEDESC2 ddsd;
    // Restore Texture surface et les effacent
    if (fromDisk)
    {
        if (handle->lpTextureSurf) handle->lpTextureSurf->Release();
        handle->lpTextureSurf = NULL;
    }
    if (handle->lpTexture)     
		handle->lpTexture->Release();
	handle->lpTexture = NULL;
    if (fromDisk&1)
    {
        D3D_TextureSwapGet((u_int8_t*)&ddsd, handle->offsetSwap, sizeof(DDSURFACEDESC2));
        SYS_DXTRACE( SYS_DXTRACE(g_lpDD->CreateSurface(&ddsd, &handle->lpTextureSurf, NULL)));
			return;
        D3D_TextureSwapGet((u_int8_t*)g_pRLX->pGX->ColorTable, handle->offsetSwap+sizeof(DDSURFACEDESC2), 768);
        SYS_DXTRACE(handle->lpTextureSurf->Lock(NULL, &ddsd, DDLOCK_WAIT|DDLOCK_WRITEONLY, NULL));
        D3D_TextureSwapGet((u_int8_t*)ddsd.lpSurface, handle->offsetSwap+sizeof(DDSURFACEDESC2)+768, ddsd.lPitch*ddsd.dwHeight);
        SYS_DXTRACE(handle->lpTextureSurf->Unlock(NULL));
    }
    // Reload la texture
    handle->lpTexture = D3D_UploadTextureFromSurface(handle, handle->option);
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void  RenderPoly(V3XPOLY *fce)
*
* DESCRIPTION :
*
*/


void V3XRGB_Composing(rgb32_t *dest, V3XPOLY *fce)
{
    V3XMATERIAL *Mat = (V3XMATERIAL*)fce->Mat;
    int i = ( Mat->info.Shade == 1 )  ? 1 : fce->numEdges;
    rgb32_t *cl = fce->rgb;
    u_int32_t ra = Mat->ambient.r>>2;
    u_int32_t rb = Mat->ambient.g>>2;
    u_int32_t rc = Mat->ambient.b>>2;
	if (!cl)
		return;

    for (;i!=0;dest++, cl++, i--)
    {
        u_int32_t
        r = ra + xMUL8(cl->r, Mat->diffuse.r), 
        g = rb + xMUL8(cl->g, Mat->diffuse.g), 
        b = rc + xMUL8(cl->b, Mat->diffuse.b) ;

		dest->r = r<Mat->specular.r ? (u_int8_t)r : Mat->specular.r;
        dest->g = g<Mat->specular.g ? (u_int8_t)g : Mat->specular.g;
        dest->b = b<Mat->specular.b ? (u_int8_t)b : Mat->specular.b;
		dest->a = cl->a;		
    }
	return;
}

static void RenderWPoly(V3XPOLY **fe, int count)
{
    SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD));
    SYS_DXTRACE( g_lpDevice->SetTexture( 0, NULL));
    for (;count!=0;count--, fe++)
    {
        V3XPOLY     *fce = *fe;
        V3XMATERIAL *pMat = (V3XMATERIAL*)fce->Mat;
        LPD3DTLVERTEX  v, v0 = (LPD3DTLVERTEX)g_lpVertexBuffer;		
        int i;
        rgb24_t c = pMat->diffuse;
        if (pMat->info.Shade) 
			V3XRGB_Composing(g_pRLX->pV3X->Buffer.rgb, fce);
        g_lpVertexBuffer->color = RGBA_MAKE(c.r, c.g, c.b, 255);
        for (v=v0, i=0;i<fce->numEdges;i++, v++)
        {
            V3XPTS *pt = fce->dispTab + i;
            v->sx = D3DVAL(  pt->x );
            v->sy = D3DVAL(  pt->y );
            v->sz = D3DVAL( -pt->z );
            switch(pMat->info.Shade) {
                case 2:
                {
                    rgb32_t *ce = g_pRLX->pV3X->Buffer.rgb + i;
                    v->color =  RGBA_MAKE(ce->r, ce->g, ce->b, pMat->info.AlphaComponent ? ce->a : pMat->alpha);
                }
                break;
                case 1:
                default:
                v->color = g_lpVertexBuffer->color;
                break;
            }
            v->sz *= D3D_ClipFactor;
        }
		*v = *v0;
        // Affiche le polygone avec Direct Primitive
        SYS_DXTRACE( g_lpDevice->DrawPrimitive(
        D3DPT_LINESTRIP, D3DFVF_TLVERTEX, (LPVOID)g_lpVertexBuffer, 
        1+fce->numEdges, D3DDP_DONOTCLIP|D3DDP_DONOTUPDATEEXTENTS|D3DDP_DONOTLIGHT));
    }
    return;
}

static void RenderPoly(V3XPOLY **fe, int count)
{
    LPDIRECT3DTEXTURE2 t1=NULL, t2=NULL, t3=NULL;
    for (;count!=0;count--, fe++)
    {
        V3XPOLY *fce = *fe;
        V3XMATERIAL *pMat = (V3XMATERIAL*)fce->Mat;
        LPD3DTLVERTEX2  v = g_lpVertexBuffer;
        D3DCOLOR spec;
        int i, flat=0, t=0;
        rgb24_t c = pMat->diffuse;		  
        if (pMat->Render==255) 
            pMat->render_clip(fce); 
        else
        {
            if (pMat->info.Sprite==2) 
				continue;
			// Shade composer
            if (pMat->info.Shade) 
				V3XRGB_Composing(g_pRLX->pV3X->Buffer.rgb, fce);

			// Texture Cache
            if (pMat->info.Texturized)
            {             
				if (pipe_pTex[0] !=(u_int8_t*)pMat->texture[0].handle)
                {
                    pipe_pTex[0] = (u_int8_t*)pMat->texture[0].handle;
					t|=1;
                    if (pipe_pTex[0])
                    {
                        V3XD3DHANDLE  *h = (V3XD3DHANDLE*)pipe_pTex[0];
                        if (h->option&V3XTEXDWNOPTION_REFRESH)
                        {
                            h->option&=~V3XTEXDWNOPTION_REFRESH;
                        }
                        else
                        {                            
                            if (SYS_DXTRACE(h->lpTextureSurf->IsLost())==DDERR_SURFACELOST)
								RestoreTexture(h, 1);
                        }
                        if ((pMat->info.Opacity)&&((h->option&V3XTEXDWNOPTION_COLORKEY)==0))
                        {
                            SYS_DXTRACE(h->lpTextureSurf->SetColorKey(DDCKEY_SRCBLT, &D3D_ColorKey));
                        }
                        t1 = h->lpTexture;
						if (t3) t|=4;
                    }
                    else
                    {
                        int s = (u_int8_t)fce->Mat + (u_int8_t)fce->dispTab;
                        c.r = (u_int8_t)xMUL8( s, pMat->diffuse.r );
                        c.g = (u_int8_t)xMUL8( s, pMat->diffuse.g );
                        c.b = (u_int8_t)xMUL8( s, pMat->diffuse.b );
                        t1 = NULL;
                    }                    
                }
                // Texture 2 (light map)
                if (g_pRLX->pV3X->Client->Capabilities&GXSPEC_MULTITEXTURING)
                {
                    if (pipe_pTex[1] !=(u_int8_t*)pMat->texture[1].handle)
                    {
                        pipe_pTex[1] = (u_int8_t*)pMat->texture[1].handle;
						t|=2;
                        if ((pipe_pTex[1])&&(pMat->info.Environment&V3XENVMAPTYPE_DOUBLE))
                        {
                            V3XD3DHANDLE  *h = (V3XD3DHANDLE*)pipe_pTex[1];                            
                            if (SYS_DXTRACE(h->lpTextureSurf->IsLost())==DDERR_SURFACELOST) 
								RestoreTexture(h, 1);
                            t2 = h->lpTexture;
                        }
                        else
                        {
                            t2 = NULL;                            
                        }
                    }
                }
				if ((t)&&(pipe_iRender==pMat->lod))
				{
					if (t&1) 
						SYS_DXTRACE(g_lpDevice->SetTexture( 0, t1 ));
					if (t&4)						
						SYS_DXTRACE(g_lpDevice->SetTexture( 1, t3 ));

					if (t&2) 
					{
						unsigned s = t&4 ? 2 : 1;
						SYS_DXTRACE(g_lpDevice->SetTexture( s, t2 ));
					}
				}
                
            }
			else
			{
				pipe_pTex[0] = NULL;
				t1 = NULL;
				if (pipe_iRender!=pMat->lod)	
					SYS_DXTRACE(g_lpDevice->SetTexture( 0, t1 ));
				t|=1;
			}
            // Render cache ID
            if (pipe_iRender!=pMat->lod)
            {
                DWORD ShadeModel;
                pipe_iRender = pMat->lod;
                // Shading mode
                switch(pMat->info.Shade) {
                    case 2:
                    case 3:  ShadeModel = D3DSHADE_GOURAUD; break;
                    default: ShadeModel = D3DSHADE_FLAT; break;
                }
                SYS_DXTRACE(g_lpDevice->SetRenderState( D3DRENDERSTATE_EDGEANTIALIAS, pMat->Render==V3XRCLASS_wired ? TRUE : FALSE));    
                SYS_DXTRACE(g_lpDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE, ShadeModel));
                
                // Blending mode
                if ((g_cDeviceInfo.desc->dpcTriCaps.dwShadeCaps&D3DPSHADECAPS_ALPHAFLATBLEND)==0)
                {
                    if ((g_cDeviceInfo.desc->dpcTriCaps.dwShadeCaps&D3DPSHADECAPS_ALPHAFLATSTIPPLED))
						pipe_iBlend = pMat->info.Transparency ? V3XBLENDMODE_STIPPLE : 0;
                    else 
						pipe_iBlend = 0;
                } else 
					pipe_iBlend =  pMat->info.Transparency;

                if (g_pRLX->pV3X->Client->Capabilities&(GXSPEC_ENABLEZBUFFER|GXSPEC_ENABLEWBUFFER))
                {
                    if ((pMat->info.Transparency)&&(!pMat->info.Environment))
						SYS_DXTRACE(g_lpDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE));
                    else
						SYS_DXTRACE(g_lpDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE));
                }                    
                switch(pipe_iBlend) {
                    case V3XBLENDMODE_ADD: // Additif
						SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE));
						SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE));
						SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE));
                    break;
                    case V3XBLENDMODE_ALPHA: // Alpha blending
						SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE));
						SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_ALPHAFUNC, TRUE));
						SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA));
						SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA));							break;
                    case V3XBLENDMODE_SUB: // Transparence negative
						SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE));
						SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO));
						SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR));
                    break;
                    case V3XBLENDMODE_STIPPLE:
						SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_STIPPLEDALPHA, TRUE));
                    break;
                    case V3XBLENDMODE_NONE: // Pas de transparence
                    default:
						SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE));
						SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_STIPPLEDALPHA, FALSE));
                    break;
                }      				

                // Set Direct X 6 Stage State				
                if (  pMat->info.Texturized )
                {                    
					unsigned stage = 0;
					SYS_DXTRACE(g_lpDevice->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, 
                    (
                    (pMat->info.Sprite) ||
                    (pMat->info.Perspective||(g_pRLX->pV3X->Client->Capabilities&GXSPEC_FORCEHWPERSPECTIVE)))
                    ? TRUE : FALSE));

                    SYS_DXTRACE(g_lpDevice->SetRenderState( D3DRENDERSTATE_COLORKEYENABLE, (pMat->info.Opacity) ? TRUE : FALSE));
					SYS_DXTRACE(g_lpDevice->SetTexture( 0, t1 ));
					SYS_DXTRACE(g_lpDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 ));
                    SYS_DXTRACE(g_lpDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE));
                    SYS_DXTRACE(g_lpDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE));                    
					SYS_DXTRACE(g_lpDevice->SetTextureStageState( 0, D3DTSS_COLOROP , D3DTOP_MODULATE));
                    switch(pipe_iBlend) 
					{
                        case V3XBLENDMODE_STIPPLE:
                        case V3XBLENDMODE_ALPHA:                        
						    SYS_DXTRACE(g_lpDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
	                        SYS_DXTRACE(g_lpDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE));
							SYS_DXTRACE(g_lpDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP , D3DTOP_MODULATE));
                        break;
                        case V3XBLENDMODE_ADD:
                        case V3XBLENDMODE_NONE:                        
							SYS_DXTRACE(g_lpDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
							SYS_DXTRACE(g_lpDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE));
							SYS_DXTRACE(g_lpDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE));
                        break;
                        default:
                        break;
                    }
						
					if ((g_pRLX->pV3X->Client->Capabilities&GXSPEC_DISABLEDUALTEX)==0)
					{
						if ((pipe_pTex[1])&&(pMat->info.Environment&V3XENVMAPTYPE_DOUBLE))					
						{
							stage++;
							SYS_DXTRACE(g_lpDevice->SetTexture( stage, t2 ));
							SYS_DXTRACE(g_lpDevice->SetTextureStageState( stage, D3DTSS_TEXCOORDINDEX, 1 ));                        
							SYS_DXTRACE(g_lpDevice->SetTextureStageState( stage, D3DTSS_COLORARG1, D3DTA_TEXTURE ));
							SYS_DXTRACE(g_lpDevice->SetTextureStageState( stage, D3DTSS_COLORARG2, D3DTA_CURRENT ));
							SYS_DXTRACE(g_lpDevice->SetTextureStageState( stage, D3DTSS_COLOROP, D3DTOP_ADD));
							SYS_DXTRACE(g_lpDevice->SetTextureStageState( stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
							SYS_DXTRACE(g_lpDevice->SetTextureStageState( stage, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE));
							SYS_DXTRACE(g_lpDevice->SetTextureStageState( stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE));						
						}			
					}
					
					if (stage)
					{
						DWORD dwPasses;
						if (SYS_DXTRACE(g_lpDevice->ValidateDevice(&dwPasses)))
						{					
							g_pRLX->pV3X->Client->Capabilities&=~GXSPEC_MULTITEXTURING;
							SYS_DXTRACE( g_lpDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE ));
							pipe_pTex[1] = NULL;
						} 	return;
					}
					else
					{
						SYS_DXTRACE(g_lpDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE ));
						SYS_DXTRACE(g_lpDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE ));
					}
					
                }
				else
				{
					SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, FALSE));
					if (!t1)
						SYS_DXTRACE( g_lpDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE ));
					if (!t2)
						SYS_DXTRACE( g_lpDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE ));
					if (!t3)
						SYS_DXTRACE( g_lpDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE ));
				}
            } 			

            if (pMat->Render == V3XRCLASS_wired )
            {
                LPD3DTLVERTEX  v = (LPD3DTLVERTEX)g_lpVertexBuffer;
                LPD3DTLVERTEX  v0 = v;
                V3XPTS *pt = fce->dispTab;
                v->color = RGBA_MAKE(c.r, c.g, c.b, pMat->alpha);
                for (i=0;i<fce->numEdges;i++, v++, pt++)
                {
                    v->sx = D3DVAL(  pt->x );
                    v->sy = D3DVAL(  pt->y );
                    v->sz = D3DVAL( -pt->z );
                    v->rhw = 1.f / v->sz;
                    v->sz *= D3D_ClipFactor;
                    v->specular = 0;
                    switch(pMat->info.Shade) {
                        case 2:
                        {
                            rgb32_t *ce = g_pRLX->pV3X->Buffer.rgb + i;
                            v->color = RGBA_MAKE(ce->r, ce->g, ce->b, pMat->info.AlphaComponent ? ce->a : pMat->alpha);
                        }
                        break;
                        case 1:
                        default:
                        v->color = v0->color;
                        break;
                    }            
                }
                SYS_DXTRACE( g_lpDevice->DrawPrimitive( D3DPT_LINESTRIP, D3DFVF_TLVERTEX, (LPVOID)v0, 
					fce->numEdges, D3DDP_DONOTCLIP|D3DDP_DONOTUPDATEEXTENTS|D3DDP_DONOTLIGHT));
				pipe_pTex[0] = NULL;
				pipe_pTex[1] = NULL;

            }
            else
            {
                if (!pMat->info.Shade)
                {
                    switch(pipe_iBlend){
                        case V3XBLENDMODE_NONE:
                        case V3XBLENDMODE_ADD:
                        case V3XBLENDMODE_SUB: g_lpVertexBuffer->color = RGBA_MAKE(c.r, c.g, c.b, 255); break;
                        case V3XBLENDMODE_ALPHA:
                        case V3XBLENDMODE_STIPPLE: g_lpVertexBuffer->color = RGBA_MAKE(c.r, c.g, c.b, pMat->alpha); break;
                    }
                }
                else
                {
                    rgb32_t *ce = g_pRLX->pV3X->Buffer.rgb + 0;
                    g_lpVertexBuffer->color = RGBA_MAKE(ce->r, ce->g, ce->b, pMat->info.AlphaComponent ? ce->a : pMat->alpha);
					
                }

                // Build ngone g_lpVertexBuffer list
                spec = (!pipe_iBlend) ? D3D_SpecularValue : 0;  
                for (v=g_lpVertexBuffer, i=0;i<fce->numEdges;i++, v++)
                {
                    V3XPTS *pt = fce->dispTab + i;
                    v->sx = D3DVAL(  pt->x );
                    v->sy = D3DVAL(  pt->y );
                    v->sz = D3DVAL( -pt->z );
                    v->rhw = pMat->info.Perspective ? fce->ZTab[i].oow : 1.f / v->sz;
                    v->specular = spec;

                    switch(pMat->info.Shade) {
                        case 2:
                        {
                            rgb32_t *ce = g_pRLX->pV3X->Buffer.rgb + i;
                            v->color =  RGBA_MAKE(ce->r, ce->g, ce->b, pMat->info.AlphaComponent ? ce->a : pMat->alpha);							
							v->specular = pMat->info.AlphaComponent ? spec :  RGBA_MAKE(
											xMUL8(ce->a, pMat->specular.r), 
											xMUL8(ce->a, pMat->specular.g), 
											xMUL8(ce->a, pMat->specular.b), 
											128);
							
                        }
                        break;
                        case 1:
                        default:
                        v->color = g_lpVertexBuffer->color;
                        break;
                    }                    
                    if (pMat->info.Texturized)
                    {
                        if (pMat->info.Perspective)
                        {
                            float cs = v->sz;
                            v->tu = fce->ZTab[i].uow * cs;
                            v->tv = fce->ZTab[i].vow * cs;
                        }
                        else
                        {
                            v->tu = D3DVAL(fce->uvTab[0][i].u);
                            v->tv = D3DVAL(fce->uvTab[0][i].v);
                            v->rhw = 1.f;
                        }
                        if (pMat->info.Environment&V3XENVMAPTYPE_DOUBLE) // Couche 2 du mapping
                        {
                            v->tu2 = fce->uvTab[1][i].u;
                            v->tv2 = fce->uvTab[1][i].v;
                        }
                    }
                    v->sz *= D3D_ClipFactor;
                }
                SYS_DXTRACE( g_lpDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX2, (LPVOID)g_lpVertexBuffer, 
                fce->numEdges, D3DDP_DONOTCLIP|D3DDP_DONOTUPDATEEXTENTS|D3DDP_DONOTLIGHT));
                
				if ((pMat->info.Environment&V3XENVMAPTYPE_DOUBLE)&&(!(g_pRLX->pV3X->Client->Capabilities&(GXSPEC_MULTITEXTURING|GXSPEC_DISABLEDUALTEX))))
                {					
                    g_lpDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1 );
                    SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE));
                    SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE));
                    SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE));                    
                    if ((pipe_pTex[0]!=(u_int8_t*)pMat->texture[1].handle))
                    {
                        V3XD3DHANDLE *h;
                        pipe_pTex[0] = (u_int8_t*)pMat->texture[1].handle;
                        h =  (V3XD3DHANDLE*)pipe_pTex[0];
                        if (h)
                        {
                            if (SYS_DXTRACE( h->lpTextureSurf->IsLost())==DDERR_SURFACELOST) 
								RestoreTexture(h, 1);
                            SYS_DXTRACE( g_lpDevice->SetTexture( 0, h->lpTexture));
                        }
                    }
                    for (v=g_lpVertexBuffer, i=0;i<fce->numEdges;i++, v++) 
						v->color = RGBA_MAKE(pMat->specular.r, pMat->specular.g, pMat->specular.b, pMat->alpha);		
					
					SYS_DXTRACE( g_lpDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX2, (LPVOID)g_lpVertexBuffer, 
                    fce->numEdges, D3DDP_DONOTCLIP|D3DDP_DONOTUPDATEEXTENTS|D3DDP_DONOTLIGHT));
                    SYS_DXTRACE( g_lpDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0));
                    pipe_iRender = -1;
                }
				
            }
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void  BeginList(void)
*
* DESCRIPTION : Start a polygon list
*
*/
static void  BeginList(void)
{
    if (g_pRLX->pGX->View.State&GX_STATE_SCENEBEGUN) return;
    int b = g_pRLX->pGX->View.State&GX_STATE_LOCKED;
    if (b)  
		if (b) g_pRLX->pGX->Client->Unlock();

    g_pRLX->pGX->View.State|=GX_STATE_SCENEBEGUN;
    SYS_DXTRACE( g_lpDevice->BeginScene());
    pipe_iRender = -1;
    pipe_pTex[0] = NULL;
    pipe_pTex[1] = NULL;
    if (g_pRLX->pV3X->Client->Capabilities&GXSPEC_ENABLEFOGGING)
    {
        SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_FOGENABLE, TRUE));
        SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_FOGCOLOR , g_pRLX->pV3X->ViewPort.fogColor));
        D3D_FogFactor = 255.f / (g_pRLX->pV3X->Light.fogDistance-g_pRLX->pV3X->Clip.Far);
    }
    if (g_pRLX->pV3X->Client->Capabilities&GXSPEC_ENABLEZBUFFER)
    {
        D3D_ClipFactor = -1.f / g_pRLX->pV3X->Clip.Far;
        SYS_DXTRACE( g_lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, 
        g_pRLX->pV3X->Client->Capabilities&GXSPEC_ENABLEWBUFFER ? D3DZB_USEW : D3DZB_TRUE ));
        SYS_DXTRACE( g_lpDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE));
        SYS_DXTRACE( g_lpDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL));
    }
    else D3D_ClipFactor = 1.f ;
	SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, 
    (g_pRLX->pV3X->Client->Capabilities&GXSPEC_ENABLEDITHERING) ? TRUE : FALSE));
    SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_CULLMODE , D3DCULL_NONE));
    int i=0, m = ((g_pRLX->pV3X->Client->Capabilities&GXSPEC_MULTITEXTURING) ? 2 : 1);
    for (;i<m;i++)
    {
        u_int32_t m = (g_pRLX->pV3X->Client->Capabilities&GXSPEC_ENABLEFILTERING) ?  D3DTFG_LINEAR : D3DTFG_POINT;
        SYS_DXTRACE( g_lpDevice->SetTexture( i , NULL));
        SYS_DXTRACE( g_lpDevice->SetTextureStageState( i, D3DTSS_MAGFILTER, m));
        SYS_DXTRACE( g_lpDevice->SetTextureStageState( i, D3DTSS_MINFILTER, m));
    }
    if (g_pRLX->pV3X->Client->Capabilities&GXSPEC_FORCEWIREFRAME)
    {
        SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE));
        SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE , D3DSHADE_GOURAUD));
        SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_FILLMODE , D3DFILL_SOLID));
        SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_STIPPLEDALPHA, FALSE));
    }
    else
    {
        SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE , TRUE));
        SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE , D3DSHADE_FLAT));
        SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_FILLMODE , D3DFILL_SOLID));
    }
    SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_EDGEANTIALIAS, FALSE));
    SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE));
    D3D_ColorKey.dwColorSpaceLowValue = g_pRLX->pGX->csp_cfg.colorKey;
    D3D_ColorKey.dwColorSpaceHighValue = g_pRLX->pGX->csp_cfg.colorKey;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void  EndList(void)
*
* DESCRIPTION : End a polygon list
*
*/
static void EndList(void)
{
    if (g_pRLX->pGX->View.State&GX_STATE_SCENEBEGUN) 
		SYS_DXTRACE( g_lpDevice->EndScene());
    g_pRLX->pGX->View.State&=~GX_STATE_SCENEBEGUN;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void  RenderDisplay(void)
*
* DESCRIPTION : Render a list of polygons
*
*/
static void RenderDisplay(void)
{
    int n=g_pRLX->pV3X->Buffer.MaxFaces;
    V3XPOLY **f=g_pRLX->pV3X->Buffer.RenderedFaces;
    if (g_pRLX->pV3X->Client->Capabilities&GXSPEC_FORCEWIREFRAME)
		RenderWPoly(f, n);
    else
		RenderPoly(f, n);
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int  RLXAPI TextureModify(GXSPRITE *ptr, u_int8_t *newBuffer)
*
* DESCRIPTION :
*
*/
static int V3XAPI TextureModify(GXSPRITE *sp, u_int8_t *bitmap, const rgb24_t *colorTable)
{
    DDSURFACEDESC2 ddsd1;
    RECT RSource;
    unsigned     lx, lPitch = sp->LX  * 2;
    V3XD3DHANDLE *handle = (V3XD3DHANDLE *)sp->handle;
    LPDIRECTDRAWSURFACE4 pDst = handle->lpTextureSurf;
    RSource.left = 0;
    RSource.top = 0;
    RSource.right = sp->LX;
    RSource.bottom = sp->LY;
    // Lock texture
    sysMemZero(&ddsd1, sizeof(DDSURFACEDESC2));
    ddsd1.dwSize = sizeof(ddsd1);
    SYS_DXTRACE( pDst->Lock(  NULL, &ddsd1, DDLOCK_WRITEONLY | DDLOCK_WAIT, NULL));
    lx = min((unsigned)ddsd1.lPitch, lPitch);
    // Copy line per line
    if (lx)
    {
        u_int8_t *a, *b;
        int i;
        for (a = (u_int8_t*)bitmap, 
        b = (u_int8_t*)ddsd1.lpSurface, 
        i = RSource.top;
        i < RSource.bottom;
        i++, 
        a+= lPitch, 
        b+= ddsd1.lPitch)
        {
            WORD *pSrc = (WORD*)b;
            WORD *pDst = (WORD*)a;
            sysMemCpy(pDst, pSrc, lx);
        }
    }
    SYS_DXTRACE(pDst->Unlock(NULL));
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void  RLXAPI DrawPrimitives(V3XVECTOR *vertexes, u_int16_t *indexTab, unsigned NumIndexes, unsigned NumVertexes, int mode, rgb32_t *cl)
*
* Description :
*
*/
static void  RLXAPI DrawPrimitives(V3XVECTOR *vertexes, u_int16_t *indexTab, unsigned NumIndexes, unsigned NumVertexes, int mode, rgb32_t *cl)
{
    LPD3DTLVERTEX v = (LPD3DTLVERTEX)g_lpVertexBuffer;
    unsigned i = 0;
    if (pipe_iRender!= V3XRCLASS_WIRED2)
    {
        pipe_iRender = V3XRCLASS_WIRED2;
        SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_EDGEANTIALIAS , FALSE));
        SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE));
        SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID));
        SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE , D3DSHADE_GOURAUD));
        if (mode&256)
        {
            SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE));
            SYS_DXTRACE( g_lpDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE));
        }
        SYS_DXTRACE( g_lpDevice->SetTexture( 0 , NULL));
    }
    mode&=255;
    for (;i<NumVertexes;i++, v++, vertexes++, cl++)
    {
        v->color = RGBA_MAKE(cl->r, cl->g, cl->b, cl->a);
        v->sx  = vertexes->x;
        v->sy  = vertexes->y;
        v->rhw = 1.f;
        v->sz  = -vertexes->z*D3D_ClipFactor;
        v->specular = RGB_MAKE(0, 0, 0);
    }
    if (indexTab)
    SYS_DXTRACE( g_lpDevice->DrawIndexedPrimitive( mode==V3XRCLASS_point ? D3DPT_POINTLIST : D3DPT_LINELIST, 
		D3DFVF_TLVERTEX, (LPVOID)g_lpVertexBuffer, NumVertexes, indexTab, NumIndexes, 0));
    else
    SYS_DXTRACE( g_lpDevice->DrawPrimitive( mode==V3XRCLASS_point ? D3DPT_POINTLIST : D3DPT_LINELIST, 
		D3DFVF_TLVERTEX, (LPVOID)g_lpVertexBuffer, NumVertexes, 0));
    return;
}

V3X_GXSystem V3X_Direct3D = {
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
    BeginList, 
    EndList, 
    DrawPrimitives, 
    "V3X/Direct 3D-6(tm)", 
    256+10, 
    GXSPEC_FOG+
    GXSPEC_SPRITEAREPOLY+
    GXSPEC_HARDWARE+
    GXSPEC_UVNORMALIZED+
    GXSPEC_RESIZABLEMAP, 
    0x000000, 
    0
};
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  HRESULT CALLBACK D3DAppFORMEnumDeviceCallback(LPDDSURFACEDESC2 lpDDSD, LPVOID lpContext)
*
* DESCRIPTION :
*
*/
void  DX_MaskToMAP(u_int32_t mask, u_int8_t *pos, u_int8_t *size)
{
    *pos=0;
    while (!(  ((mask&1)==1) || (mask==0) ))
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
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  HRESULT CALLBACK D3DAppFORMEnumDeviceCallback(LPDDSURFACEDESC2 lpDDSD, LPVOID lpContext)
*
* DESCRIPTION :
*
*/
HRESULT CALLBACK D3DAppFORMEnumDeviceCallback(LPDDPIXELFORMAT lpPfD, LPVOID lpContext)
{
    D3D_TextureFormat *pTexFormat=g_cDeviceInfo.TextureFormat+g_cDeviceInfo.NumTextureFormats;
    /*
    * Record the DDSURFACEDESC2 of this texture format
    */
    sysMemZero( pTexFormat , sizeof(D3D_TextureFormat));
    pTexFormat->pfd = *lpPfD;
    // Texture 8bit
    if (lpPfD->dwFlags & DDPF_PALETTEINDEXED8)
    {
        pTexFormat->bPalettized |= 1;
        pTexFormat->IndexBPP = 8;
    }
    // Texture 4bit
    else
    if (lpPfD->dwFlags & DDPF_PALETTEINDEXED4)
    {
        pTexFormat->bPalettized |= 1;
        pTexFormat->IndexBPP = 4;
    }
    // Texture RGB
    else
    {
        pTexFormat->IndexBPP = 0;
        DX_MaskToMAP(lpPfD->dwRBitMask, &pTexFormat->RedFieldPosition , &pTexFormat->RedMaskSize);
        DX_MaskToMAP(lpPfD->dwGBitMask, &pTexFormat->GreenFieldPosition, &pTexFormat->GreenMaskSize);
        DX_MaskToMAP(lpPfD->dwBBitMask, &pTexFormat->BlueFieldPosition, &pTexFormat->BlueMaskSize);
        DX_MaskToMAP(lpPfD->dwRGBAlphaBitMask, &pTexFormat->RsvdFieldPosition, &pTexFormat->RsvdMaskSize);
        pTexFormat->IndexBPP = (u_int8_t)((pTexFormat->RedMaskSize+pTexFormat->GreenMaskSize+pTexFormat->BlueMaskSize));
    }
    if (lpPfD->dwFlags & DDPF_ALPHAPIXELS)
        pTexFormat->bPalettized|=2;

	if (lpPfD->dwBumpBitCount)    
        pTexFormat->bPalettized|=4;

	g_cDeviceInfo.NumTextureFormats++;
    UNUSED(lpContext);
    return DDENUMRET_OK;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static HRESULT WINAPI D3DAppZBUFnumDeviceCallback( DDPIXELFORMAT* pddpf, VOID* pddpfDesired )
*
* DESCRIPTION :  
*
*/
static HRESULT WINAPI D3DAppZBUFnumDeviceCallback( DDPIXELFORMAT* pddpf, VOID* pddpfDesired )
{
    if ( pddpf->dwFlags & DDPF_ZBUFFER )
    {
        if    (
        (( g_pRLX->pV3X->Client->Capabilities & GXSPEC_ENABLESTENCIL ) && ( pddpf->dwFlags & DDPF_STENCILBUFFER ) )
        || (( g_pRLX->pV3X->Client->Capabilities & GXSPEC_ENABLESTENCIL ) == 0)
 )
        {        
            sysMemCpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT));			
            if (pddpf->dwZBufferBitDepth==g_pRLX->pV3X->ViewPort.zDepth)			
            return D3DENUMRET_CANCEL;
        }
    }    
    return D3DENUMRET_OK;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void  D3D_CheckTexture(void)
*
* DESCRIPTION :
*
*/
void D3D_CheckTexture(void)
{
    int StartFormat = -1;
    g_cDeviceInfo.NumTextureFormats = 0;
    SYS_DXTRACE( g_lpDevice->EnumTextureFormats(D3DAppFORMEnumDeviceCallback, (LPVOID)&StartFormat));
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void  D3D_SetViewPort(void)
*
* DESCRIPTION :
*
*/
void D3D_SetViewPort(void)
{
    D3DVIEWPORT2 viewData2;
    sysMemZero(&viewData2, sizeof(D3DVIEWPORT2));
    viewData2.dwSize = sizeof(D3DVIEWPORT2);
    viewData2.dwX = g_pRLX->pGX->View.xmin;
    viewData2.dwY = g_pRLX->pGX->View.ymin;
    viewData2.dwWidth = g_pRLX->pGX->View.xmax - g_pRLX->pGX->View.xmin + 1;
    viewData2.dwHeight = g_pRLX->pGX->View.ymax - g_pRLX->pGX->View.ymin + 1;
    viewData2.dvClipX = (float)viewData2.dwX;
    viewData2.dvClipY = (float)viewData2.dwY;
    viewData2.dvClipWidth = (float)viewData2.dwWidth;
    viewData2.dvClipHeight = (float)viewData2.dwHeight;
    viewData2.dvMinZ  = 0.f;
    viewData2.dvMaxZ  = 1.f;
    
	SYS_DXTRACE( g_lpD3D->CreateViewport(&lpd3dViewport3, NULL));
    SYS_DXTRACE( g_lpDevice->AddViewport(lpd3dViewport3));
    SYS_DXTRACE( lpd3dViewport3->SetViewport2(&viewData2));
    SYS_DXTRACE( g_lpDevice->SetCurrentViewport(lpd3dViewport3));
    
    SetBackgroundColor((rgb24_t*)&g_pRLX->pV3X->ViewPort.backgroundColor);
	SetLightColor();
    
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C D3D_clearVideo(int32_t x1, int32_t x2, int32_t y1, int32_t y2)
*
* DESCRIPTION :
*
*/
static void CALLING_C clearSurfaces(void)
{
    D3DRECT rc;
    int b = g_pRLX->pGX->View.State&GX_STATE_LOCKED;    
    rc.x1 = g_pRLX->pGX->View.xmin;
    rc.y1 = g_pRLX->pGX->View.ymin;
    rc.x2 = g_pRLX->pGX->View.xmax+1;
    rc.y2 = g_pRLX->pGX->View.ymax+1;

	if (b) g_pRLX->pGX->Client->Unlock();
    SYS_DXTRACE( lpd3dViewport3->Clear2( 1, &rc, D3DCLEAR_TARGET, g_pRLX->pV3X->ViewPort.backgroundColor, 0, 0));
    if (b) g_pRLX->pGX->Client->Lock();
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C D3D_DrawFilledRect(int32_t x1, int32_t x2, int32_t y1, int32_t y2, u_int32_t _color)
*
* DESCRIPTION :
*
*/
static void CALLING_C drawFilledRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t _color)
{
    D3DRECT rc;
	int b = g_pRLX->pGX->View.State&GX_STATE_LOCKED;    
    DWORD color;
    rgb24_t c;    
    rc.x1 = x1;
    rc.y1 = y1;
    rc.x2 = x2+1;
    rc.y2 = y2+1;
    g_pRLX->pfGetPixelFormat((rgb24_t*)&c, _color);  color = RGBA_MAKE(c.r, c.g, c.b, 0);

	if (b) g_pRLX->pGX->Client->Unlock();
    SYS_DXTRACE(lpd3dViewport3->Clear2( 1, &rc, D3DCLEAR_TARGET, color, 0.f, 0));
    if (b) g_pRLX->pGX->Client->Lock();
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void  CALLING_C D3D_drawAnyLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
*
* DESCRIPTION :
*
*/
void  CALLING_C D3D_drawAnyLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, u_int32_t colour)
{
    if (g_pRLX->pGX->View.State&GX_STATE_SCENEBEGUN)
    {
        if (g_pRLX->pV3X->Ln.nbLines<g_pRLX->pV3X->Ln.maxLines)
        {
            V3XVECTOR *v = g_pRLX->pV3X->Ln.lineBuffer + g_pRLX->pV3X->Ln.nbLines;
            rgb32_t *c = g_pRLX->pV3X->Ln.lineColor  + g_pRLX->pV3X->Ln.nbLines;
            v[0].x = (V3XSCALAR)x1;   v[1].x = (V3XSCALAR)x2;
            v[0].y = (V3XSCALAR)y1;   v[1].y = (V3XSCALAR)y2;
            v[0].z = v[1].z = g_pRLX->pV3X->Clip.Near;
            g_pRLX->pfGetPixelFormat((rgb24_t *)c, colour);
            c[1] = c[0];
            g_pRLX->pV3X->Ln.nbLines+=2;
        }
    }
    else
    {
        if (g_pRLX->pGX->View.State&GX_STATE_LOCKED) 
			old_drawAnyLine(x1, y1, x2, y2, colour);
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void  CALLING_C setPalette(u_int32_t a, u_int32_t b, void * pal)
*
* DESCRIPTION :
*
*/
void  CALLING_C setPalette(u_int32_t a, u_int32_t b, void * pal)
{
    rgb24_t D3D_SpecularColor;
    int c = RGB_ToGray((int32_t)g_pRLX->pGX->AmbientColor.r,  (int32_t)g_pRLX->pGX->AmbientColor.g,  (int32_t)g_pRLX->pGX->AmbientColor.b);
    c-=128; 
	if (c<0) c=0;
    D3D_SpecularColor.r = D3D_SpecularColor.g = D3D_SpecularColor.b = (u_int8_t)c;
    D3D_SpecularValue = RGB_MAKE(D3D_SpecularColor.r, D3D_SpecularColor.g, D3D_SpecularColor.b);
    UNUSED(a);
    UNUSED(b);
    UNUSED(pal);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void  CreateSurface(int pages)
*
* DESCRIPTION :
*
*/

static int CreateSurface(int pages)
{
	g_pRLX->pGX->Client->ReleaseSurfaces();
    g_pRLX->pGX->View.Flags|=GX_CAPS_BACKBUFFERINVIDEO;	
	GX_DDraw.CreateSurface(pages);
    if (g_pRLX->pGX->View.Flags&GX_CAPS_3DSYSTEM)
    {
        if ( g_pRLX->pV3X->Client->Capabilities&GXSPEC_ENABLEZBUFFER)
        {
            SYS_DXTRACE(g_lpD3D->EnumZBufferFormats(g_cDeviceInfo.guidDevice, D3DAppZBUFnumDeviceCallback, (LPVOID)&g_cDeviceInfo.ddpfZBuffer));
			g_pRLX->pV3X->ViewPort.zDepth = (u_int8_t)g_cDeviceInfo.ddpfZBuffer.dwZBufferBitDepth;
            ZbufferAlloc(); 
			
        }
        // Create Devices (on passle back buffer)
        SYS_DXTRACE(g_lpD3D->CreateDevice(g_cDeviceInfo.guidDevice, g_lpPrimarySurface, &g_lpDevice, NULL));
        SYS_DXTRACE(g_lpDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE));
        SYS_DXTRACE(g_lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE));
        D3D_CheckTexture();
        D3D_SetViewPort();
        g_pRLX->pGX->gi.clearBackBuffer = clearSurfaces;
        g_pRLX->pGX->gi.setPalette = setPalette;
        g_pRLX->pGX->gi.drawFilledRect = drawFilledRect;
        old_drawAnyLine = g_pRLX->pGX->gi.drawAnyLine;
        g_pRLX->pGX->gi.drawAnyLine = D3D_drawAnyLine;
        D3D_TextureSwapReset();
        SYS_DXTRACE( g_lpDevice->SetRenderTarget(g_lpPrimarySurface, 0));
    }
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void  ReleaseSurfaces(void)
*
* DESCRIPTION :
*
*/
static void ReleaseSurfaces(void)
{
    if (g_pRLX->pGX->View.Flags&GX_CAPS_3DSYSTEM)
    {
        D3D_TextureSwapReset();
        if (g_lpDevice)
        {
            g_lpBackgroundMaterial->Release();
            g_lpLightingMaterial->Release();
            lpd3dViewport3->Release();
            g_lpDevice->Release();
        }
        ZbufferFree();
        GX_DDraw.ReleaseSurfaces();
        g_lpDevice = NULL;
        lpd3dViewport3 = NULL;
    }
    else  
		GX_DDraw.ReleaseSurfaces();
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  _RLXEXPORTFUNC  void RLXAPI V3XDRV_Load_D3D(void)
*
* DESCRIPTION :  
*
*/
static int RLXAPI Open(void *hWnd)
{
	SYS_ASSERT(hWnd);
	g_hWnd = (HWND)hWnd;
	SYS_ASSERT(IsWindow(g_hWnd));    
    return 1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  _RLXEXPORTFUNC void RLXAPI V3XDRV_Load_D3D(void)
*
* Description :  
*
*/
void RLXAPI V3X_EntryPoint(struct RLXSYSTEM *p)
{
	GX_EntryPoint(p);
	
	g_pRLX->pGX->Client = &GX_Direct3D;
    g_pRLX->pV3X->Client = &V3X_Direct3D;	

    GX_Direct3D = GX_DDraw;
    GX_Direct3D.Open = Open;
    GX_Direct3D.CreateSurface = CreateSurface;
    GX_Direct3D.ReleaseSurfaces = ReleaseSurfaces;

    return;
}

void GX_GetGraphicInterface(struct GXSYSTEM *p)
{
	return;
}