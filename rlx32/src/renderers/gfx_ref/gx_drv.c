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

#include <stdio.h>
#include "_rlx32.h"
#include "systools.h"
#include "sysresmx.h"
#include "_rlx.h"
#include "gx_struc.h"
#include "gx_init.h"
#include "gx_rgb.h"
#include "gx_tools.h"
#include "gx_csp.h"
#include "gx_c.h"

#include "_rlx.h"

extern struct RLXSYSTEM *g_pRLX;

static void CALLING_C setPalette(u_int32_t a, u_int32_t b, void * pal)
{
    a=a;
    b=b;
    pal=pal;
    return;
}

static void CALLING_C blit(u_int32_t dest, u_int32_t src)
{ 
     u_int8_t *a = g_pRLX->pGX->Surfaces.lpSurface[dest];
     u_int8_t *b = g_pRLX->pGX->Surfaces.lpSurface[src];
     sysMemCpy(a, b, g_pRLX->pGX->View.lSurfaceSize);
     return;    
}

static void CALLING_C waitDrawing(void)
{
	 return;    
}

static void CALLING_C clearBackBuffer(void)
{
     sysMemZero(g_pRLX->pGX->View.lpBackBuffer, g_pRLX->pGX->View.lSurfaceSize);
	 return;
}

static void CALLING_C clearVideo(void)
{
     sysMemZero(g_pRLX->pGX->View.lpBackBuffer, g_pRLX->pGX->View.lSurfaceSize);
	 return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : GX_GetGraphicInterface
*
* DESCRIPTION :
*
*/
void RLXAPI GX_GetGraphicInterface(struct GXSYSTEM	*pGX)
{
    switch(pGX->View.BitsPerPixel) {
		case 15:
		case 16:			
			GX_GetSpriteInterfaceRef16(pGX, &pGX->csp);
			GX_GetGraphicInterfaceRef16(pGX, &pGX->gi);
		break;
		case 8:
			GX_GetSpriteInterfaceRef8(pGX, &pGX->csp);
			GX_GetGraphicInterfaceRef8(pGX, &pGX->gi);
		break;
		case 24:
		case 32:
			GX_GetSpriteInterfaceRef32(pGX, &pGX->csp);
			GX_GetGraphicInterfaceRef32(pGX, &pGX->gi);
		break;
		default:
		SYS_ASSERT(0);
		break;
    }

	pGX->gi.blit = blit;
	pGX->gi.clearBackBuffer = clearBackBuffer;
	pGX->gi.clearVideo = clearVideo;
	pGX->gi.setPalette = setPalette;
	pGX->gi.waitDrawing = waitDrawing;

    pGX->csp_cfg.put.fonct = pGX->csp.put;
    pGX->csp_cfg.pset.fonct = pGX->csp.pset;
    pGX->csp_cfg.transp.fonct = pGX->csp.Trsp50;
    pGX->csp_cfg.op = pGX->csp.put;    
	sysStrCpy(pGX->csp_cfg.ext, "png");

	pGX->View.RGB_Magic =
          (((1L<<pGX->View.ColorMask.RedMaskSize  )-2L) << pGX->View.ColorMask.RedFieldPosition   )
        | (((1L<<pGX->View.ColorMask.GreenMaskSize)-2L) << pGX->View.ColorMask.GreenFieldPosition )
        | (((1L<<pGX->View.ColorMask.BlueMaskSize )-2L) << pGX->View.ColorMask.BlueFieldPosition  );

    return;
}
