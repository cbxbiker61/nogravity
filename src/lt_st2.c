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
#include <string.h>
#include <stdio.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "_stub.h"
#include "systools.h"
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

#include "v3x_1.h"
#include "v3xtrig.h"
#include "sysini.h"
#include "sysctrl.h"
#include "sysnetw.h"
#include "gx_init.h"
#include "gx_rgb.h"
#include "gx_tools.h"
#include "v3xrend.h"
#include "gui_os.h"
#include "iss_defs.h"
//
#include "lt_struc.h"
#include "lt_data.h"
#include "lt_func.h"

static V3XPROFILER g_Profiler;
static V3XBUFFER g_TmpBuffer;

static void V3XScene_PushList(void)
{
    g_TmpBuffer = V3X.Buffer;
    V3X.Buffer.RenderedFaces   += g_TmpBuffer.MaxFaces;
    V3X.Buffer.ClippedFaces    += g_TmpBuffer.MaxClipped;
    V3X.Buffer.Mat             += g_TmpBuffer.MaxMat;
    V3X.Buffer.MaxFacesDisplay -= g_TmpBuffer.MaxFaces;
    V3X.Buffer.MaxClippedFaces -= g_TmpBuffer.MaxClipped;
    V3X.Buffer.MaxTmpMaterials     -= g_TmpBuffer.MaxMat;
    V3X.Buffer.MaxFaces = V3X.Buffer.MaxClipped = V3X.Buffer.MaxMat = 0;
    return;
}

static void V3XScene_PopList(void)
{
    V3X.Buffer = g_TmpBuffer;
    return;
}

static void v3x_RenderOneObject(V3XSCENE *Scene, V3XOVI *OVI)
{
    V3XOVI **OVIf = OVI->child;
    while(*OVIf!=NULL)
    {
        v3x_RenderOneObject(Scene, *OVIf);
        (**OVIf).state|=V3XSTATE_MATRIXUPDATE;
        OVIf++;
    }
    if (((OVI->state&V3XSTATE_HIDDEN)==0)) V3XScene_ObjectBuild(OVI, 0);
    return;
}

void NG_RenderDisplayBox(RW_Button *but, V3XMATRIX *Mat, V3XOVI *OVI)
{
    GXVIEWPORT CamFen;
    int m = V3X.Camera.matrix_Method;
    unsigned flags= V3X.Setup.flags, fl;
    V3XMATRIX Cam = V3X.Camera.M;
    CamFen = GX.View;
    GX.View.xmin = but->X;
    GX.View.ymin = but->Y;
    GX.View.xmax = GX.View.xmin + but->LX;
    GX.View.ymax = GX.View.ymin + but->LY;
    V3X.Camera.matrix_Method =  V3XMATRIX_Custom;
    V3X.Setup.flags         &= ~(V3XOPTION_COLLISION);
    V3X.Setup.flags         |=   V3XOPTION_DUPPOLY;
    V3X.Camera.M  =  *Mat;
    V3XScene_PushList();
    fl = g_SGGame.Scene->Layer.bg.flags;
    g_SGGame.Scene->Layer.bg.flags |= V3XBG_COLOR;
    V3XViewport_Setup(&V3X.Camera, GX.View);
    g_SGSettings.bClearView = 1;
    if (OVI)
    {
        V3XScene_Viewport_Build(g_SGGame.Scene, NULL);
        v3x_RenderOneObject(g_SGGame.Scene, OVI);
    }
    else 
		V3XScene_Viewport_Build(g_SGGame.Scene, &GX.View);
    V3XScene_Viewport_Clear(g_SGGame.Scene);
    V3XScene_Viewport_Render(g_SGGame.Scene);
    V3XScene_PopList();
    g_SGGame.Scene->Layer.bg.flags = fl;
    V3X.Setup.flags  = flags;
    g_SGSettings.bClearView = 0;
    GX.View = CamFen;
    V3X.Camera.M = Cam;
    V3X.Camera.matrix_Method = m;
    V3XViewport_Setup(&V3X.Camera, GX.View);
    return;
}

void NG_DrawTicker(void)
{
    V3XPROFILER *pf=&g_Profiler;
	char tex[256];
    pf->poly+=V3X.Buffer.MaxFaces;
    pf->frame++;
    if (timer_ms()-pf->time>2000)
    {
        pf->pps = pf->poly/2;
        pf->fps = pf->frame/2;
        pf->poly = pf->frame = 0;
        pf->time = timer_ms();
    }
    sprintf(tex, "%d fps %d pps",(int) pf->fps, (int)pf->pps);
	CSP_Color(g_SGGame.CI_WHITE);
    CSP_WriteCenterText(tex, GX.View.ymax-20, g_pSmallFont);
    return;
}

void NG_HudDisplayRear(void)
{
    RW_Button *b = g_pGameBoard->item+VCfg_win_rear;
    V3XMATRIX Cam = V3X.Camera.M;
    V3XVector_Neg(&Cam.v.K);
    V3XVector_CrossProduct(&Cam.v.I, &Cam.v.J, &Cam.v.K);
    NG_RenderDisplayBox(b, &Cam, NULL);
    return;
}

int NG_HudDisplayLocked(V3XOVI *OVI)
{
    V3XMATRIX Cam;
    RW_Button *but = g_pGameBoard->item+VCfg_win_camis;
    if (g_SGGame.CameraMode)
       return 0;

    if (g_SGGame.Missile)
    {
        Cam = g_mtxMissile;
        g_SGGame.Missile->state |= V3XSTATE_HIDDEN;
        OVI = NULL;
    }
    else
    {
        V3XVECTOR retrait = {CST_ZERO, CST_ZERO, CST_ZERO};
        Cam = V3X.Camera.M;
        retrait.z = (OVI->ORI->global_rayon*2);
        V3XVector_PolarSet(&Cam.v.Pos, &OVI->mesh->matrix.v.Pos, retrait.z, g_SGGame.Count*16, g_SGGame.Count*8);
        V3XVector_Dif(&retrait, &Cam.v.Pos, &OVI->mesh->matrix.v.Pos);
        V3XMatrix_BuildFromVector(&Cam, &retrait, 0);
    }
    // (Cam.Matrix);
    NG_RenderDisplayBox(but, &Cam, OVI);
    if (g_SGGame.Missile)
        g_SGGame.Missile->state &= ~V3XSTATE_HIDDEN;
    return 0;
}

void NG_HudDisplayCamera(void)
{
    int p = 0;
    if (g_SGSettings.rearview) 
		p|=1;

    if (g_pLockTarget)
    {
        if (((!g_SGSettings.Stereo)&&(GX.View.ymax>100))||(g_SGGame.Missile))
			p|=2;
    }

    g_SGLockMode.dmode&=~(8+16+32);

    if (p)
    {
        if (p&2)
        {
            if ((g_pLockTarget->state&V3XSTATE_HIDDEN)==0)
            {
                g_SGSettings.bClearView = 1;
                if (NG_HudDisplayLocked(g_pLockTarget))
                {
                    p|=8;
                    g_SGLockMode.dmode|=8;
                }
                g_SGLockMode.dmode|=16;
                p|=4;
            }
        }
        if (p&1)
        {
            g_SGSettings.bClearView = 1;
            NG_HudDisplayRear();
            g_SGLockMode.dmode|=32;
            p|=4;
        }
    }
    g_SGSettings.bClearView = 0;
    return;
}
