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
#define NG_EXPORT extern
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "_stub.h"
#include "sysresmx.h"
#include "systools.h"
#include "sysini.h"
#include "sysctrl.h"
#include "sysnetw.h"
#include "gx_tools.h"
#include "gx_init.h"
#include "gx_rgb.h"
#include "gx_csp.h"
#include "sysresmx.h"
#include "gx_struc.h"
#include "gx_csp.h"
#include "gx_flc.h"
#include "v3xdefs.h"
#include "v3x_2.h"
#include "v3xcoll.h"
#include "v3xrend.h"
#include "v3xmaps.h"
#include "v3xscene.h"

#include "v3xrend.h"
#include "iss_defs.h"
#include "iss_fx.h"
#include "iss_av.h"
#include "gui_os.h"
//
#include "lt_struc.h"
#include "lt_data.h"
#include "lt_func.h"

extern GXSPRITE g_csPicture;

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void NG_FadeInBackground()
{
	int i;
	int s = (V3X.Client->Capabilities&GXSPEC_HARDWARE) ? 4 : 8;
	for (i=0;i<255;i+=s)
	{
		int k = i == 256-s? 255 : i;
		GX.Client->Lock();
		if (i == 0)
			GX.gi.clearBackBuffer();
		CSP_Color((GX.View.BytePerPixel > 1 ? RGBA_PixelFormat(k, k, k, 0) : 255));
		GX.csp.zoom_pset(&g_csPicture,0,0,GX.View.lWidth, GX.View.lHeight);
		GX.Client->Unlock();
		GX.View.Flip();
	}
}

void NG_SetGamma(float gamma)
{
	rgb24_t ramp[256];

	int i;

	/* 0.0 gamma is all black */
	if ( gamma <= 0.0f ) {
		for ( i=0; i<256; ++i ) {
			ramp[i].r = ramp[i].g = ramp[i].b = 0;
		}
		return;
	} else
	/* 1.0 gamma is identity */
	if ( gamma == 1.0f ) {
		for ( i=0; i<256; ++i ) {
			ramp[i].r = ramp[i].g = ramp[i].b = (uint8_t)i;
		}
		return;
	} else
	/* Calculate a real gamma ramp */
	{
		int value;
		gamma = 1.0f / gamma;
		for ( i=0; i<256; ++i ) {
			value = (int)(pow(i/256.f, gamma)*255.0f+0.5f);
			if ( value > 255 ) {
				value = 255;
			}
			ramp[i].r = ramp[i].g = ramp[i].b =(uint8_t)value;
		}
	}
	SYS_ASSERT(GX.gi.setGammaRamp);
	if (GX.gi.setGammaRamp)
		GX.gi.setGammaRamp(ramp);
}

void NG_FadeOutBackground()
{
	int i;
	int s = (V3X.Client->Capabilities&GXSPEC_HARDWARE) ? 4 : 8;
	for (i=0;i<255;i+=s)
	{
		int k = i == 256-s? 0 : 255 - i;
		GX.Client->Lock();
		CSP_Color((GX.View.BytePerPixel > 1 ? RGBA_PixelFormat(k, k, k, 0) : 255));
		GX.csp.zoom_pset(&g_csPicture,0,0,GX.View.lWidth, GX.View.lHeight);
		GX.Client->Unlock();
		GX.View.Flip();
	}
}

void NG_DrawBackgroundPic(char *szFilename, int TrackPlay, int mode)
{
    GX.View.Flags = GX_CAPS_VSYNC|GX_CAPS_BACKBUFFERINVIDEO;
    mode&=7;

	// NG_ChangeScreenMode(GX.Client->SearchDisplayMode(640, 480, g_SGSettings.ColorDepth));
    NG_InstallHandlers();

	SYS_ASSERT(g_csPicture.data == 0);
    NG_LoadBackground(szFilename, &g_csPicture);
	SYS_ASSERT(g_csPicture.data);
    if (TrackPlay>=0)
    {
        NG_AudioPlayTrack(TrackPlay);
    }
}

void NG_DrawLoadingScreen(void)
{
	GX.csp.zoom_pset(&g_csPicture,0,0,GX.View.lWidth, GX.View.lHeight);
	CSP_WriteCenterText(
		g_SGSettings.DemoMode
		? "Demo"
		: g_pGameItem->EI[g_pCurrentGame->episode].LI[g_pCurrentGame->level[g_pCurrentGame->episode]].name,
		GX.View.ymax-26, g_pFontMenuSml);
}

void NG_PlayLoadingScreen(void)
{
    // int32_t p = MM_heap.push();
	int i;
	NG_DrawBackgroundPic(".\\menu\\load.png", -1, 0);

	for (i=0;i<256;i+=4)
	{
		int j = i == 255-4 ? 255 : i;
		GX.Client->Lock();
			CSP_Color(RGB_PixelFormat(j,j,j));
			NG_DrawLoadingScreen();

			GX.Client->Unlock();
		GX.View.Flip();
	}

    //MM_heap.pop(p);
}

void NG_PlayGameOver(void)
{
#if (SGTARGET != NG_DEMO_VERSION)

    int32_t delta, esc=0, g_uiTrackTime ;
    MM_heap.push();
    NG_DrawBackgroundPic(".\\menu\\failed.png", Ms_FAILED, 0);
	NG_FadeInBackground();
    g_SGSettings.Menu = 0;
    NG_AudioSay("ins05");
	g_uiTrackTime = timer_sec();

    while (((delta=timer_sec()-g_uiTrackTime)<20)&&(!esc))
    {
		char tex[256];
        sKEY->Update(0);
		sJOY->Update(0);
        sprintf(tex, "%02d", (int)(19-delta));

		GX.Client->Lock();
			CSP_Color(RGB_PixelFormat(255,255,255));
			GX.csp.zoom_pset(&g_csPicture,0,0,GX.View.lWidth, GX.View.lHeight);
			CSP_WriteCenterText(g_szGmT[145], (2*GX.View.ymax/3), g_pFontMenuSml);
			CSP_DrawCenterText(tex, (3*GX.View.ymax)/4, g_pFontMenuLrg, GX.csp_cfg.put);
			GX.Client->Unlock();
        GX.View.Flip();

        if (sKEY_IsHeld(s_esc)||sKEY_IsHeld(s_n))
        {
            esc=1;
            g_SGSettings.Menu=0;
        }

		if (sKEY_IsHeld(s_space)||sKEY_IsHeld(s_y)||sKEY_IsHeld(s_j)||sKEY_IsHeld(s_o))
        {
            esc=1;
			g_SGSettings.Menu=1;
        }
    }

	NG_FadeOutBackground();
    GX.Client->ReleaseSprite(&g_csPicture);
    NG_AudioPlaySound(NG_AudioGetByName("explode2")-1, 0);
    NG_AudioStopMusic();
    MM_heap.pop(-1);
#endif
}

void NG_PlayEndGame(void)
{
#if (SGTARGET ==NG_DEMO_VERSION)
    MM_heap.push();
    NG_DrawBackgroundPic("\\menu\\preview.png", -1, 1);
	NG_FadeInBackground();
    NG_WaitForKeyWithDelay(4);
    NG_FadeOutBackground();
	GX.Client->ReleaseSprite(&g_csPicture);
    MM_heap.pop(-1);
#endif
}

void NG_PlayPresentsGame(void)
{
    MM_heap.reset();
    NG_DrawBackgroundPic(".\\menu\\realtech.png", Ms_INTRO, 1);
    NG_FadeInBackground();
	NG_WaitForKeyWithDelay(2);
	NG_FadeOutBackground();
	GX.Client->ReleaseSprite(&g_csPicture);

    MM_heap.reset();
    NG_DrawBackgroundPic(".\\menu\\lithium.png", -1, 1);
    NG_FadeInBackground();
	NG_WaitForKeyWithDelay(4);
	NG_FadeOutBackground();
	GX.Client->ReleaseSprite(&g_csPicture);

    NG_AudioStopMusic();

#if (SGTARGET ==NG_FULL_VERSION)
    MM_heap.reset();
#endif
}

void NG_PlayEndEpisode(int episode)
{
}

void NG_PlayTheEnd(void)
{
	MM_heap.reset();
	MM_heap.push();
	NG_DrawBackgroundPic("\\menu\\theend.png", -1, 1);
	NG_FadeInBackground();
	NG_WaitForKeyWithDelay(30);
	NG_FadeOutBackground();
	GX.Client->ReleaseSprite(&g_csPicture);
	MM_heap.pop(-1);
}

