//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2005 - realtech VR

This file is part of No Gravity 1.9

No Gravity is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by cthe Free Software Foundation; either version 2
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
#include "_rlx32.h"
#include "_stub.h"
#include "_rlx.h"

#include "systools.h"
#include "sysresmx.h"
#include "systime.h"
#include "sysnetw.h"
#include "sysctrl.h"
#include "gx_struc.h"
#include "gx_csp.h"
#include "gx_flc.h"
#include "gx_cdc.h"
#include "gx_tools.h"
#include "v3xdefs.h"
#include "v3xtrig.h"
#include "v3x_1.h"
#include "v3x_2.h"
#include "v3xcoll.h"
#include "v3xrend.h"
#include "v3xmaps.h"
#include "v3xscene.h"

#include "iss_defs.h"

//
#include "gui_os.h"
#include "gx_rgb.h"
//             
#include "lt_struc.h"
#include "lt_data.h"
#include "lt_func.h"

typedef struct {
    char Code[32];
    int Car, isType;
}CheatStruct;

static CheatStruct g_SGInput;
static SGMenu *g_pComFunction;
static int  g_nWaitAnswer;
static char *g_szLockText[]={"Lock", "Locking...", NULL};
LockInfo_Struct g_SGLockMode;

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_UpdateCamera(void)
*
* DESCRIPTION :
*
*/
static void NG_UpdateCamera(void)
{
    V3XVECTOR v2={CST_ZERO, (V3XSCALAR)-128, (V3XSCALAR)128}, v3={(V3XSCALAR)-128, (V3XSCALAR)-128, (V3XSCALAR)1200}, v4={(V3XSCALAR)-128, (V3XSCALAR)-128, (V3XSCALAR)-4000};
    V3XVECTOR d[]={{(V3XSCALAR)1200, CST_ZERO, CST_ZERO},
        {CST_ZERO, CST_ZERO, (V3XSCALAR)1200},
        {(V3XSCALAR)-1200, CST_ZERO, CST_ZERO}, 
        {CST_ZERO, CST_ZERO, (V3XSCALAR)-2}, 
    {CST_ZERO, CST_ZERO, (V3XSCALAR)-600}};
    V3XVECTOR b, vTgt, vPos, vDV;
    V3XOVI *mySHIP =  g_pPlayer->J.OVI;
    V3XSCALAR sp = 2;
    
	if (g_SGObjects.NavCam)  
		mySHIP->state|=V3XSTATE_MATRIXUPDATE;

	mySHIP->state&=~V3XSTATE_HIDDENDISPLAY;

	g_SGObjects.Cam->state|=V3XSTATE_MATRIXUPDATE;

	if( g_SGObjects.Sky )
		g_SGObjects.Sky->state|=V3XSTATE_MATRIXUPDATE|V3XSTATE_GIANT;

	V3XVector_Set(&vPos, 0,0,0);

    switch(g_SGGame.CameraMode) {
        case 0:
			V3X.Camera.matrix_Method = V3XMATRIX_Custom;
			g_pCamera->pos = g_pPlayer->Mat->v.Pos;
			g_pCamera->roll = 0;
			V3X.Camera.M.v.Pos = g_pPlayer->Mat->v.Pos;
			V3XMatrix_Transpose(V3X.Camera.M.Matrix, g_pPlayer->Mat->Matrix);
			// (V3X.Camera.M.Matrix);
			// (g_pPlayer->Mat->Matrix);
			mySHIP->state|=V3XSTATE_HIDDENDISPLAY;
        break;
        case 2:
        case 3:
        case 4:
        case 5:
			sp = 6;
			g_pCamera->roll = 0;
			V3X.Camera.matrix_Method = V3XMATRIX_Vector;
			V3XVector_ApplyMatrix(b, d[g_SGGame.CameraMode-2], g_pPlayer->Mat->Matrix);
			vTgt = g_pPlayer->Mat->v.Pos;
			V3XVector_Dif(&vPos, &vTgt, &b);
        break;
        case 1:
			V3X.Camera.matrix_Method = V3XMATRIX_Custom;
			vTgt = g_pPlayer->Mat->v.Pos;
			g_pCamera->roll = 0;
			V3XMatrix_Transpose(V3X.Camera.M.Matrix, g_pPlayer->Mat->Matrix);
			V3XVector_ApplyMatrix(b, d[4], g_pPlayer->Mat->Matrix);
			V3XVector_Dif(&vPos, &g_pPlayer->Mat->v.Pos, &b);
			V3X.Camera.M.v.Pos = g_pCamera->pos;
			// (V3X.Camera.M.Matrix);
			// (g_pPlayer->Mat->Matrix);
			sp = 8;
        break;
        case CAMERA_NAV:
			/*
			V3X.Camera.matrix_Method = V3XMATRIX_byTarget;
			g_pCamera->roll = 0;
			vTgt = g_pPlayer->Mat->v.Pos;
			if (g_SGObjects.NavCam)
			{
				V3XVector_Cpy(Pos, (V3XSCALAR)g_SGObjects.NavCam->Tk->vinfo.pos);
				g_SGObjects.NavCam->state|=V3XOCAPS_OUTOFFRUSTRUM;
			}
			mySHIP->state&=~V3XOCAPS_OUTOFFRUSTRUM;
			*/
#define GRID (4096L)
			V3X.Camera.matrix_Method = V3XMATRIX_Vector;
			g_pCamera->roll = 0;
			vTgt = g_pPlayer->Mat->v.Pos;
			{
				V3XSCALAR *t1=&vTgt.x, *t2=&vPos.x, *t3=&g_pCamera->pos.x;
				int i;
				for (i=0;i<3;i++, t1++, t2++)
				{
					int32_t p =(int32_t)(*t1);
					int32_t p0=p&  (GRID-1L),
					p1=p&(~(GRID-1L));
					if (p0>(GRID>>1)) p1+=GRID;
					*t2 = ((g_SGGame.Count&511)==0) ? (int32_t)p1 : t3[i];
				}
			}
        break;
        case CAMERA_MISSILE:
        /*if (g_SGGame.Missile)
        {
            V3X.Camera.M = g_mtxMissile;
            V3X.Camera.matrix_Method = V3XMATRIX_Custom;
            g_pCamera->roll = 0;
            // (V3X.Camera.M.Matrix);
            return;
        }
        else
        */
        {
            g_SGGame.CameraMode = 0;
        }
        break;
        case CAMERA_DEATH:
			// (g_pPlayer->Mat->Matrix);
			NG_FXBlast(&g_pPlayer->J.pInf, mySHIP);
			V3X.Camera.matrix_Method = V3XMATRIX_Vector;
			g_pCamera->roll = 0;
			g_SGGame.CameraMode++;
			g_SGGame.DeathDist = 512;
        case CAMERA_DEATH+1:
			vTgt = g_pPlayer->Mat->v.Pos;
			NG_AIRandomAimEx(&vPos, &vTgt, (V3XSCALAR)(g_SGGame.DeathDist+=16), (int32_t)(g_SGGame.Count*32), (int32_t)(g_SGGame.Count*4));
			if (g_SGGame.DeathDist>4048)
				g_SGGame.DeathDist=4048;
			g_pCamera->pos = vPos;
			g_pCamera->target = vTgt;
        goto CAM_END;
        case CAMERA_START:
			sp = 9;
			V3X.Camera.matrix_Method = V3XMATRIX_Vector;
			g_pCamera->roll += (g_cTimer.iCounter>>15);
			V3XVector_ApplyMatrix(b, v2, g_pPlayer->Mat->Matrix);
			vTgt = g_pPlayer->Mat->v.Pos;
			V3XVector_Dif(&vPos, &vTgt, &b);
			if (g_SGGame.Count==2)
				NG_AudioKickSound(g_cFXTable.Engine, g_cFXTable.SoundEngine, 0);
        break;
        case CAMERA_NEWNAV:
			V3XMatrix_SetIdentity(g_pPlayer->Mat->Matrix);
			V3XVector_ApplyMatrix(b, v3, g_pPlayer->Mat->Matrix);
			vTgt = g_pPlayer->Mat->v.Pos;
			V3XVector_Dif(&g_pCamera->pos, &vTgt, &b);
			g_pCamera->target = vTgt;
			g_SGGame.CameraMode = CAMERA_NEWNAV+1;
			g_pPlayer->J.pInf.Doppling = NG_Audio3DPlay(g_pPlayer->J.pInf.Doppler-1, &vTgt, &g_pPlayer->J.Mv.vel);
			g_pPlayer->J.pInf.fSpeed = g_pPlayer->J.pInf.fSpeedMax;

        case CAMERA_NEWNAV+1:
			g_pCamera->roll += g_cTimer.iCounter>>13;
			V3X.Camera.matrix_Method = V3XMATRIX_Vector;
			V3XVector_ApplyMatrix(b, v4, g_pPlayer->Mat->Matrix);
			vTgt = g_pPlayer->Mat->v.Pos;
			g_pCamera->target = vTgt;
			V3XVector_Dif(&vPos, &vTgt, &b);
			sp = 10;
			NG_Audio3DUpdate(&vTgt, &g_pPlayer->J.Mv.vel, g_pPlayer->J.pInf.Doppling, g_pPlayer->J.pInf.Doppler-1);
        break;
        default:
        g_SGGame.CameraMode = CAMERA_SHIP;
        break;
    }

    if (g_SGGame.CameraMode>0)
    {
        V3XSCALAR di;
		float k = 1.f / sp;

        V3XVector_Dif(&vDV, &vPos, &g_pCamera->pos);
		V3XVector_Scl(&vDV, &vDV, k);
        di = V3XVector_LengthSq(&vDV);
        if ((di<=(V3XSCALAR)128)||((g_SGGame.CameraMode==5)&&(di<=(V3XSCALAR)2048)))
        {
            if (g_SGGame.CameraMode==CAMERA_START)
                g_SGGame.CameraMode=CAMERA_SHIP;

            if (g_SGGame.CameraMode==CAMERA_SHIP)
                g_SGGame.CameraMode=0;

            if (g_SGGame.CameraMode==CAMERA_NEWNAV+1)
            {
                g_SGGame.CameraMode=CAMERA_SHIP;
                g_SGGame.Count=0;
                g_SGObjects.NAV++;
                if (g_SGObjects.NAV>=g_SGObjects.MaxNAV)
					g_SGObjects.NAV=0;
                NG_NAVReset(1);
                V3XVector_Cpy(g_pPlayer->Mat->v.Pos, V3X.Camera.Tk.info.pos);
                V3XVector_Cpy(g_pCamera->pos, V3X.Camera.Tk.info.pos);
                goto CAM_END;
            }
        }
        V3XVector_Inc(&g_pCamera->pos, &vDV);
		//
        V3XVector_Dif(&vDV, &vTgt, &g_pCamera->target);
		V3XVector_Scl(&vDV, &vDV, k);
        V3XVector_Inc(&g_pCamera->target, &vDV);
        if (g_SGObjects.FinCode!=GAMESTATE_DEAD)
            NG_Audio3DUpdate(&g_pPlayer->Mat->v.Pos, &g_pPlayer->J.Mv.vel, g_cFXTable.Engine, g_cFXTable.SoundEngine);

        if (V3XA.State & 1)
		{			
			float distance = 1.f;
			float rolloff = 0.0f;      
			float doppler = 1.0f;
            V3XA.Client->UserSetParms(&V3X.Camera.M, &g_pPlayer->J.Mv.vel, &distance, &doppler, &rolloff);
		}
    }
    CAM_END:
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void PauseSoundGame(void)
*
* DESCRIPTION :
*
*/
void PauseSoundGame(void)
{
    NG_AudioPauseMusic();
    V3XA.Client->ChannelFlushAll(1);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void ResumeSoundGame(void)
*
* DESCRIPTION :
*
*/
void ResumeSoundGame(void)
{
    V3XA.Client->ChannelFlushAll(1);
    NG_AudioResumeMusic();
    if( g_cFXTable.Alarm)
		g_cFXTable.Alarm = NG_AudioPlaySound(g_cFXTable.SoundAlarm, 0);
    NG_AudioKickSound(g_cFXTable.Engine, g_cFXTable.SoundEngine, 0);
    g_cFXTable.Engine = V3XA.Client->ChannelGetFree(NULL);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_RenderView(void)
*
* DESCRIPTION :
*
*/
void NG_RenderView(void)
{
    if (g_SGSettings.bClearView==2)
	{
		GX.csp_cfg.alpha = 128;
		GX.gi.drawFilledRect(0, 0, GX.View.xmax, GX.View.ymax, 0);
	}
    else
    {
        V3XScene_Viewport_Clear(g_SGGame.Scene);
        V3XScene_Viewport_Render(g_SGGame.Scene);
    }
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void ShowHelpBack(void)
*
* Description :
*
*/
int ShowHelpBack(void)
{
    timer_Reset(&g_cTimer);
    NG_RenderView();
	GX.csp_cfg.alpha = 128;
    GX.gi.drawMeshedRect(0, 0, GX.View.xmax, GX.View.ymax, 0);
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_MissionSummary(int sty)
*
* DESCRIPTION :
*
*/
void NG_MissionSummary(int sty)
{
    GXSPRITEGROUP *pFont = 1  ? g_pspDispFont : g_pspCat;
    int quit = 0;
    SGLevelItem *Level = &g_pGameItem->EI[g_pCurrentGame->episode].LI[g_pCurrentGame->level[g_pCurrentGame->episode]];
    do
    {
        int i, y;

        sKEY->Update(0);
		sJOY->Update(0);
        quit = sysConIsActive() ? 0 : sKEY_IsClicked(s_esc)||sKEY_IsClicked(s_return);

        // LOCK
        GX.Client->Lock();
        {
			g_SGSettings.bClearView = 1;
            ShowHelpBack();
            CSP_WriteCenterText(Level->name, 0, g_pspDispFont);
            for (y=sty, i=0;i<16;i++, y+=pFont->item[0].LY+2)
                CSP_WriteCenterText(Level->tex[i], y, pFont);
        }

        // UNLOCK
         if (sysConIsActive())
			sysConRender();
        GX.Client->Unlock();
        GX.View.Flip();

    } while(!quit);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void ShowHelp(void)
*
* DESCRIPTION :
*
*/
void ShowHelp(void)
{
    u_int32_t tim = timer_sec();
    int32_t p=MM_heap.push();
    PauseSoundGame();
    NG_DrawHelpFile(g_pspDispFont, g_SGGame.CI_BLUELIGHT, 10);
    MM_heap.pop(p);
    ResumeSoundGame();
    g_cGameStat.time_start+=timer_sec()-tim;
    timer_Update(&g_cTimer);
    timer_Update(&g_cTimer);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void DrawShadedTexte(char *texte, int ye, GXSPRITEGROUP *g_pFont, u_int32_t color)
*
* DESCRIPTION :
*
*/
static void DrawShadedTexte(char *texte, int ye, GXSPRITEGROUP *pFont, u_int32_t color)
{
    int32_t xe=GX.View.xmin+(((GX.View.xmax-GX.View.xmin)-CSPG_TxLen(texte, pFont))>>1);
    if (GX.View.xmax > 320)
    {
        CSP_Color(0x0);
        CSP_DrawText(texte, xe+1, ye+1, pFont, GX.csp_cfg.put);
    }
    CSP_Color(color);
    CSP_DrawText(texte, xe, ye, pFont, GX.csp_cfg.put);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int NG_DisplayDropMenu(char **menu)
*
* DESCRIPTION :
*
*/
static int NG_DisplayDropMenu(char **menu)
{
    GXSPRITEGROUP *pFont = g_pspDispFont;
    GXSPRITE panel;
    int i, ch=0, y, sz=array_size((const char**)menu)-1, ly=pFont->item[0].LY+2, esc=0;
    int stx, sty;
    int32_t p;
    p = MM_heap.push();
    if (GX.View.xmax>400)
    {
		IMG_LoadFn(".\\int\\panel.png", &panel);
		GX.Client->UploadSprite(&panel, GX.ColorTable, 1);
        sty = (GX.View.ymax-panel.LY)/2;
        stx = (GX.View.xmax-panel.LX)/2;
    }
    else
    {
        stx = 0;
        sty = (GX.View.ymax - sz*ly)/2;
        panel.data = NULL;
    }
    do
    {
        int dy = 0;
		if (STUB_TaskControl())
		{
			esc = 1;
			break;
		}

		sKEY->Update(0);
		sJOY->Update(0);
		sysConHandleInput();

		if (!sysConIsActive())
		{
			if (sKEY_IsClicked(s_return)) 
				esc=2;

			dy = (sKEY_IsClicked(s_up)-sKEY_IsClicked(s_down));
			if (dy) 
				NG_AudioPlaySound(NG_AudioGetByName("bloup")-1, 0);
		}       

        // LOCK
        GX.Client->Lock();
        g_SGSettings.bClearView = 1;
        ShowHelpBack();
        CSP_Color(g_SGGame.CI_WHITE);
        y = sty;
        if (panel.data)
        {
            GX.csp.put(stx, y, &panel);
			y+=20;
        }	
        DrawShadedTexte(menu[0], y, pFont, g_SGGame.CI_WHITE);
		y+= ly*2;
        ch -= dy;

		if (ch<0)
			ch= sz - 1;

		if (ch>=sz)
			ch= 0;

		for (i=0;i<sz;y+=ly, i++)
        {
            DrawShadedTexte(menu[i+1], y, pFont, (ch!=i) ? g_SGGame.CI_BLUELIGHT
            : g_SGGame.CI_WHITE);
        }

        if (sysConIsActive())
			sysConRender();

        // UNLOCK
        GX.Client->Unlock();
        GX.View.Flip();

    }while(!esc);

    NG_AudioPlaySound(NG_AudioGetByName("door")-1, 0);
	GX.Client->ReleaseSprite(&panel);
	
    MM_heap.pop(p);
    return esc==2 ? ch : 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int NG_QuitGame(void)
*
* DESCRIPTION :
*
*/
int NG_QuitGame(void)
{
    u_int32_t tim=timer_sec(), ret=0;
    char *PauseMenu[]={g_szGmT[165], 
        g_szGmT[166], 
        g_szGmT[168], 
        g_szGmT[167],
        g_szGmT[45], 
        g_szGmT[52], 
    NULL};
    PauseSoundGame();
    if (g_SGSettings.DemoMode)
    {
        g_SGObjects.FinCode = GAMESTATE_ABORT;
        ret = 1;
    }
    else
    switch(NG_DisplayDropMenu(PauseMenu))
	{
        case 0:
			g_SGObjects.FinCode = 0;
			ret=0;
        break;
        case 1:
			NG_MissionSummary(GX.View.ymax/3);
			g_SGObjects.FinCode = 0;
			ret=0;
        break;
        case 2:
			g_SGObjects.FinCode = GAMESTATE_RETRY;
			ret=2;
        break;
        case 3:
			g_SGObjects.FinCode = GAMESTATE_ABORT;
			ret=1;
        break;
        case 4:
			g_SGObjects.FinCode = GAMESTATE_QUIT;
			ret=2;
        break;
    }
    ResumeSoundGame();
    g_cGameStat.time_start+= timer_sec() - tim;
    timer_Update(&g_cTimer);
    timer_Update(&g_cTimer);
    return ret;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_UpdatePowerUps(void)
*
* DESCRIPTION :
*
*/
V3XOVI *NG_GetFreeSphere(void)
{
    V3XOVI *OVI = g_SGObjects.Shield;
    if (!OVI) 
		return NULL;
    return OVI;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XOVI *NG_SetSpherePos(V3XOVI *OVI, V3XMATRIX *Mat, V3XSCALAR scale, int status)
*
* DESCRIPTION :
*
*/
V3XOVI *NG_SetSpherePos(V3XOVI *OVI, V3XMATRIX *Mat, V3XSCALAR scale, int status)
{
    OVI->mesh->matrix = *Mat;
    OVI->mesh->scale = scale;
    OVI->state |= V3XSTATE_MATRIXUPDATE;
    SETBITFIELD(status, OVI->state, V3XSTATE_CULLNEVER);
    SETBITFIELD(status==0, OVI->state, V3XSTATE_HIDDEN);
    OVI->collisionList = NULL;
    OVI->matrix_Method = V3XMATRIX_Custom;
    return OVI;
}
static int warp, warps=1;
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void SetSphereColor(int status)
*
* DESCRIPTION :
*
*/
static void SetSphereColor(int status, V3XSCALAR z)
{
    if (V3X.Client->Capabilities&GXSPEC_HARDWARE)
    {
        V3XOVI *OVI = NG_SetSpherePos(g_SGObjects.Shield, g_pPlayer->Mat, z, status);
        if (status==1)
        {
            V3XMATERIAL *Mat = OVI->mesh->material+0;
            Mat->diffuse = g_SGGame.FlashColor;
            Mat->info.Transparency = V3XBLENDMODE_ALPHA;
            Mat->alpha = warp;
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_UpdateLights(void)
*
* DESCRIPTION :  
*
*/
static void NG_UpdateLights(void)
{
    V3XOVI *OVI = g_SGGame.Scene->OVI;
    int i = g_SGGame.Scene->numOVI;
    int t = max(1, g_cTimer.iCounter>>16);

    warp += warps * t;

    if (warp<0)
    {
    	warp=0;
        warps= 2;
    }
    if (warp>255)
    {
    	warp=255;
        warps=-2;
    }

    for (;i!=0;OVI++, i--)
    {
        V3XORI *ORI = OVI->ORI;
        if ((ORI->type == V3XOBJ_MESH)&&(OVI->state&V3XSTATE_HIDDEN)==0)
        {
            V3XMESH  *mesh = OVI->mesh;
            V3XMATERIAL *Mat = mesh->material;
            int j;
            for (j=0;j<mesh->numMaterial;j++, Mat++)
            {
                if (sysStriCmp(Mat->mat_name, "LIGHT2")==0)
                {
                    Mat->diffuse.r -= (u_int8_t)t;
                    Mat->diffuse.g -= t;
                    Mat->diffuse.b -= t;
                }
                if (sysStriCmp(Mat->mat_name, "WARP")==0)
                {
                    Mat->diffuse.r = warp;
                    Mat->diffuse.g = warp;
                    Mat->diffuse.b = warp;
                }
            }
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_UpdatePowerUps(void)
*
* DESCRIPTION :
*
*/
static void NG_UpdatePowerUps(void)
{
    int i;
    for (i=1;i<=MAX_POWERUP;i++)
    {
        if (g_pPlayer->Art[i].Time_val>0)
        {
            SetSphereColor(2, 1.);
            switch(i) {
                case ART_STEALTH:
                RGB_Set(g_SGGame.FlashColor, 0, 0, 255);
                g_SGGame.FlashAlpha=8;
                break;
                case ART_INVICIBLITY:
                RGB_Set(g_SGGame.FlashColor, 255, 0, 0);
                g_SGGame.FlashAlpha=8;
                break;
            }
            g_pPlayer->Art[i].Time_val-=(short)g_cTimer.iCounter>>16;
            if (g_pPlayer->Art[i].Time_val<=0)
            {
                SetSphereColor(FALSE, 1.);
                g_pPlayer->Art[i].Time_val=0;
                switch(i) {
                    case ART_STEALTH:
                    g_pPlayer->mode^=STEALTHMODE;
                    g_pPlayer->Notify+=SGNET_MODECHANGE;
                    sysConPrint("stealth mode depleated");
                    break;
                    case ART_INVICIBLITY:
                    g_pPlayer->mode^=GODMODE;
                    g_pPlayer->Notify+=SGNET_MODECHANGE;
                    sysConPrint("inviciblity mode depleated");
                    break;
                }
            }
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void PlayPowerUp(void)
*
* DESCRIPTION :
*
*/
static void PlayPowerUp(void)
{
    if (g_pPlayer->Art[g_pPlayer->CurArt].val>0)
    {
        switch(g_pPlayer->CurArt) {
            case ART_POWER:
            if (g_pPlayer->J.pInf.Shield<g_pPlayer->J.pInf.ShieldMax)
            {
                g_pPlayer->J.pInf.Shield+=(g_pPlayer->J.pInf.ShieldMax/5);
                if (g_pPlayer->J.pInf.Shield>=g_pPlayer->J.pInf.ShieldMax)
                g_pPlayer->J.pInf.Shield=g_pPlayer->J.pInf.ShieldMax;
                g_pPlayer->Art[g_pPlayer->CurArt].val--;
                NG_AudioPlaySound(NG_AudioGetByName("bloup")-1, 0);
                sysConPrint("%s used power up", g_pCurrentGame->name);
            }
            break;
            case ART_SUPERPOWER:
            if (g_pPlayer->J.pInf.Shield<g_pPlayer->J.pInf.ShieldMax)
            {
                g_pPlayer->J.pInf.Shield=g_pPlayer->J.pInf.ShieldMax;
                g_pPlayer->Art[g_pPlayer->CurArt].val--;
                NG_AudioPlaySound(NG_AudioGetByName("bloup")-1, 0);
                g_pPlayer->Notify+=SGNET_SHIELDCHANGE;
                sysConPrint("%s used super power up", g_pCurrentGame->name);
            }
            break;
            case ART_STEALTH:
            g_pPlayer->Art[g_pPlayer->CurArt].Time_val = 1968;
            g_pPlayer->Art[g_pPlayer->CurArt].val--;
            NG_AudioPlaySound(NG_AudioGetByName("shield")-1, 0);
            g_SGGame.FlashAlpha=32;
            g_pPlayer->mode|=STEALTHMODE;
            g_pPlayer->Notify+=SGNET_MODECHANGE;
            RGB_Set(g_SGGame.FlashColor, 0, 0, 255);
            SetSphereColor(1, 1.);
            sysConPrint("%s used stealth mode", g_pCurrentGame->name);
            break;
            case ART_INVICIBLITY:
            g_pPlayer->Art[g_pPlayer->CurArt].Time_val = 1968;
            g_pPlayer->Art[g_pPlayer->CurArt].val--;
            NG_AudioPlaySound(NG_AudioGetByName("shield")-1, 0);
            g_pPlayer->mode|=GODMODE;
            g_pPlayer->Notify+=SGNET_MODECHANGE;
            RGB_Set(g_SGGame.FlashColor, 255, 0, 0);
            SetSphereColor(1, 1.5);
            sysConPrint("%s used invicility", g_pCurrentGame->name);
            break;
            case ART_CLOAK:
            NG_NAVReset(1);
            g_pPlayer->Art[g_pPlayer->CurArt].val--;
            g_SGGame.FlashAlpha=32;
            NG_AudioPlaySound(NG_AudioGetByName("warp")-1, 0);
            g_pPlayer->Rot->pos = g_SGObjects.NavCam->Tk->vinfo.pos;
            g_pPlayer->Art[g_pPlayer->CurArt].Time_val = 50;
            RGB_Set(g_SGGame.FlashColor, 255, 255, 255);
            SetSphereColor(1, 1.);
            sysConPrint("%s used cloak device", g_pCurrentGame->name);
            break;
            case ART_MEGABANG:
            {
                V3XSCALAR k;
                g_SGGame.FlashAlpha =  32;
                k = g_pPlayer->J.OVI->ORI->global_rayon;
                g_pPlayer->J.OVI->ORI->global_rayon = 2048L;
                NG_FXAvary(g_pPlayer->J.OVI, &g_pPlayer->J.pInf);
                NG_FXBlast(&g_pPlayer->J.pInf, g_pPlayer->J.OVI);
                g_pPlayer->J.OVI->ORI->global_rayon = k;
                g_pPlayer->Art[g_pPlayer->CurArt].val--;
                NG_AudioPlaySound(NG_AudioGetByName("shield")-1, 0);
                g_pPlayer->Art[g_pPlayer->CurArt].Time_val = 50;
                RGB_Set(g_SGGame.FlashColor, 0, 255, 0);
                SetSphereColor(1, 5.);
                sysConPrint("%s used mega bomb", g_pCurrentGame->name);
            }
            break;
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int AddPowerUp(int code)
*
* DESCRIPTION :
*
*/
static int AddPowerUp(int code)
{
    int c=-1;
    if ((code>=8)&&(code<16))
    {
        c=code-8;
        g_cShip.wea[c].cur+=8;
    }
    if (code>=16)
    {
        c=code-16;
        g_cShip.wea[c].cur+=16;
    }
    if (c>=0)
    {
        if (g_cShip.wea[c].cur>=g_cShip.wea[c].max)
			g_cShip.wea[c].cur =g_cShip.wea[c].max;
        else 
			NG_AudioBeep(2);
        g_SGSettings.ComNumber = COM_Weapon + c -1;
        g_SGSettings.ComTime = MAX_COM_DELAY;
        return 1;
    }
    if (g_pPlayer->Art[code].val==0)
        g_pPlayer->CurArt = (char)code;
    if (g_pPlayer->Art[code].val<g_pPlayer->Art[code].MAX_val)
    {
        g_SGSettings.ComNumber = COM_Bonus+code-1;
        g_SGSettings.ComTime = MAX_COM_DELAY;
        g_pPlayer->Art[code].val++;
        NG_AudioBeep(2);
        g_SGGame.FlashColor.r=255;
		g_SGGame.FlashColor.g=0;
		g_SGGame.FlashColor.b=0;
		g_SGGame.FlashAlpha=16;
        c = 1;
    }else c=0;
    return c;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void WaitAnswer(void)
*
* DESCRIPTION :
*
*/
static void NG_UpdateCom(void)
{
    int c;
    SGActor *J;
    if (g_nWaitAnswer>0)
    {
        g_nWaitAnswer-=g_cTimer.iCounter>>16;
        if (g_nWaitAnswer<=0)
        {
            if (g_pLockTarget)
            {
                J = (SGActor*)g_pLockTarget->data;
                if (J->pInf.Type == t_FRIEND)
                {
					char tex[256];
                    c = 0;
                    if (J->pInf.Shield>0)
                       c=3;
                    if (J->pInf.Shield>(J->pInf.ShieldMax>>2)) 
                       c=2;
                    if (J->pInf.Shield>(J->pInf.ShieldMax>>1)) 
                       c=1;
                    if (J->pInf.Shield==J->pInf.ShieldMax)     
                       c=0;
                    g_SGSettings.ComTime = MAX_COM_DELAY/2;
                    g_SGSettings.ComNumber = COM_Status+c;
                    sprintf(tex, "rep0%d", c+1);
					NG_AudioSay(tex);
                    sysConPrint(tex);
                    NG_AudioBeep(3);
                }
            }
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void setFormation(int code)
*
* DESCRIPTION :
*
*/
static void setFormation(int code)
{
	char tex[256];
    sprintf(tex, "com0%d", code+1); 
    NG_AudioSay(tex);
    sysConPrint(tex);
    
    if ((code)&&(!(g_pPlayer->mode&DOOMEDMODE)))
    {
        g_SGObjects.CallMode[t_FRIEND] = (u_int8_t)code;
    }
    else
    {
        g_nWaitAnswer=128;
    }
    if (g_SGSettings.SerialGame)
    {
        g_pPlayer->Notify+=SGNET_COMMUNICATE;
        g_SGSettings.ComNumber = (u_int8_t)(COM_Request+code);
        g_SGSettings.ComTime = MAX_COM_DELAY;
        g_SGGame.ComMode = 0;
    }
    NG_AISetTacticMode();
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int32_t static NG_ControlGame(void)
*
* DESCRIPTION :
*
*/
static void NG_ReadInput(int *dx, int *dy, int *InMax)
{
    sKEY->Update(0);
	sJOY->Update(0);

	sysConHandleInput();

    if (sysConIsActive())
       return;

    if (!g_SGInput.isType)
    {

        if (sKEY_IsClicked(LK_TALK))
			g_SGInput.isType = 1;

		if (sKEY_IsClicked(LK_HUD))
		{
			g_SGSettings.showInf^=(sKEY_IsHeld(s_leftshift) ? 2 : 1);
			NG_AudioBeep(1);
		}

		if ((g_SGObjects.FinCode==GAMESTATE_DEAD)||(g_SGSettings.DemoMode>=2))
        {
            if (sKEY_IsClicked(s_esc))
            {
                g_SGObjects.quit = 1;
            }
        }
        else
        {
            switch(g_SGSettings.Ctrl) 
			{
                case CTRL_Joystick: // Joystick Control
					*InMax= (int)(16.f*g_cTimer.fCounter);
                switch(RLX.Control.Id)
				{
                    case RLXCTRL_JoyPad:
						if (sJOY)
						{
							sJOY->Update(0);
							SGJOY_MapKeyboard();
							sKEY->scanCode = 255;
							if (sKEY_IsHeld(LK_LEFT ))
								*dx=-1;
							if (sKEY_IsHeld(LK_RIGHT))
								*dx= 1;
							if (sKEY_IsHeld(LK_UP)) 
								*dy=-1;
							if (sKEY_IsHeld(LK_DOWN))
								*dy= 1;
							g_pPlayer->Mv.x -= (float)(3*(*dx)) * g_cTimer.fCounter;
							g_pPlayer->Mv.y -= (float)(3*(*dy)) * g_cTimer.fCounter;
						}
                    break;
                    case RLXCTRL_ThrustMaster:
                    case RLXCTRL_JoyAnalog:
                    case RLXCTRL_SideWinder:
                    default:
						if (sJOY)
						{
							int x, y, z;
							sJOY->Update(0);
							SGJOY_MapKeyboard();
							if (sJOY->lRz)
								g_pPlayer->Mv.inert = (float)((65535-sJOY->lRz)*g_pPlayer->J.pInf.fSpeedMax)/65000.f;

							#define deadZone 10000
							x = sJOY->lX - 32768;
							if (abs(x)<deadZone)
							{
								g_pPlayer->Mv.x *= 0.99f;
							}
							else
							{
								x+=(x<0?deadZone:-deadZone);
								g_pPlayer->Mv.x = - ((V3XSCALAR)x * g_cTimer.fCounter)/(512.f);
							}

							y = sJOY->lY - 32768;
							if (abs(y)<deadZone)
								g_pPlayer->Mv.y *= 0.99f;
							else
							{
								y+=(y<0?deadZone:-deadZone);
								g_pPlayer->Mv.y = - ((V3XSCALAR)y * g_cTimer.fCounter)/(512.f);
							}

							z = sJOY->lZ - 32768;
							if (abs(z)<deadZone)
							{
								z = 0;
								g_pPlayer->Mv.z *= 0.99f;
							}
							else
							{
								z+=(z<0?deadZone:-deadZone);
								g_pPlayer->Mv.z = - ((V3XSCALAR)z * g_cTimer.fCounter)/(2048.f);
							}						
							
							
						}
                    break;
                }
                sKEY->scanCode=1;
                break;
                case CTRL_Keyb:  // Keyboard Control
				{
					if (sKEY_IsHeld(LK_RIGHT))
						*dx =  1;
					if (sKEY_IsHeld(LK_LEFT))
						*dx = -1;
					if (sKEY_IsHeld(LK_DOWN))
						*dy =  1;
					if (sKEY_IsHeld(LK_UP))  
						*dy = -1;
					if (*dx) 
						g_pPlayer->Mv.x -= (float)(3*(*dx))*g_cTimer.fCounter;
					if (*dy) 
						g_pPlayer->Mv.y -= (float)(3*(*dy))*g_cTimer.fCounter;
					*InMax = (int)(16.f*g_cTimer.fCounter);
				}
                break;				
                case CTRL_Mouse:
                {
                    V3XSCALAR ra = 2.f*g_cTimer.fCounter*(float)(g_SGSettings.Mickey+16), s1;
                    sMOU->Update(0);
					SGMOU_MapKeyboard();
                    s1 = DIVF32((float)sMOU->x, (float)GX.View.xmax) - 0.5f;
                    g_pPlayer->Mv.x =  - s1 * ra;
                    s1 = DIVF32((float)sMOU->y, (float)GX.View.ymax) - 0.5f;
                    g_pPlayer->Mv.y =  s1 * ra;
                }
                *InMax = (int)(16.f*g_cTimer.fCounter);
                break;
            }

            {
                if (sKEY_IsHeld(LK_FIRE))
					g_pPlayer->Mx.but|=1;

                if (sKEY_IsHeld(LK_THROTTLE))  
					g_pPlayer->Mx.but|=2;

                if (sKEY_IsClicked(LK_ITEM))
					g_pPlayer->Mx.but|=8;

                if (sKEY_IsClicked(LK_NEXTWEAPON)||(sMOU->lZ))
                {
                    int s = sMOU->lZ ? sMOU->lZ/abs(sMOU->lZ) : 1;
                    g_pPlayer->J.pInf.Attack+=s;

                    if(g_pPlayer->J.pInf.Attack>6) 
						g_pPlayer->J.pInf.Attack=1;

                    if(g_pPlayer->J.pInf.Attack<1)
						g_pPlayer->J.pInf.Attack=6;

                    g_pPlayer->Mx.but|=1L<<(g_pPlayer->J.pInf.Attack+5);
                }
                // Rotation
				if (sKEY_IsHeld(LK_ANTITHRUST)) 
					g_pPlayer->Mx.but|=(1L<<30);

                if (sKEY_IsHeld(LK_SPEEDLESS))
					g_pPlayer->Mv.inert-=g_cTimer.fCounter/2;

                if (sKEY_IsHeld(LK_SPEEDPLUS))  
					g_pPlayer->Mv.inert+=g_cTimer.fCounter/2;

                if (sKEY_IsHeld(LK_STOP))
					g_pPlayer->Mv.inert=g_pPlayer->J.pInf.fSpeedMax;

                if (sKEY_IsHeld(LK_ROLLRIGHT))  
					g_pPlayer->Mv.z-=4*g_cTimer.fCounter;

                if (sKEY_IsHeld(LK_ROLLLEFT))
					g_pPlayer->Mv.z+=4*g_cTimer.fCounter;

                if (sKEY_IsClicked(LK_RADAR))
                {
                    g_SGGame.RadarMode^=1;
                    NG_AudioBeep(1);
                }
				else
                if (sKEY_IsClicked(s_esc))   
				{
					g_pPlayer->Mx.but|=(1L<<20);
				}
				else
                if (sKEY_IsClicked(LK_LOCK)||sKEY_IsClicked(s_winapp))
					g_pPlayer->Mx.but|=4;
				else
                if (sKEY_IsClicked(LK_THRUST) ||sKEY_IsClicked(s_winright))
					g_pPlayer->Mx.but|=16;
				else
                if (sKEY_IsClicked(LK_NEXTNAV)||sKEY_IsClicked(s_winleft))   
					g_pPlayer->Mx.but|=32;
				else
                if (sKEY_IsClicked(s_1)) 
					g_pPlayer->Mx.but|=1L<<(6+(g_SGGame.ComMode ? 15 : 0));
				else
                if (sKEY_IsClicked(s_2)) 
					g_pPlayer->Mx.but|=1L<<(7+(g_SGGame.ComMode ? 15 : 0));
				else
                if (sKEY_IsClicked(s_3)) 
					g_pPlayer->Mx.but|=1L<<(8+(g_SGGame.ComMode ? 15 : 0));
				else
                if (sKEY_IsClicked(s_4))
					g_pPlayer->Mx.but|=1L<<(9+(g_SGGame.ComMode ? 15 : 0));
				else
                if (sKEY_IsClicked(s_5))
					g_pPlayer->Mx.but|=1L<<(10+(g_SGGame.ComMode ? 15 : 0));
				else
                if (sKEY_IsClicked(s_6)) 
					g_pPlayer->Mx.but|=1L<<(11+(g_SGGame.ComMode ? 15 : 0));
				else			
                if (sKEY_IsClicked(LK_COM))
					g_SGGame.ComMode^=1;
				else
                if (sKEY_IsClicked(LK_PREVITEM))
					g_pPlayer->Mx.but|=1L<<(12);
				else
                if (sKEY_IsClicked(LK_NEXTITEM))
					g_pPlayer->Mx.but|=1L<<(13);
				else
                if (g_SGGame.ComMode)
                {
                    if (sKEY_IsClicked(s_7))             
						g_pPlayer->Mx.but|=1L<<27;
					else
                    if (sKEY_IsClicked(s_8))
						g_pPlayer->Mx.but|=1L<<28;
                }
            }
        }


		/*
        if (0)
        {
            V3XVECTOR v;
            v.x = CST_ZERO;
            v.y = CST_ZERO;

            if (g_pPlayer->Mx.but&1)
            {
                int amp=(g_pPlayer->J.pInf.Attack<<10);
                if (g_pPlayer->J.pInf.Attack&1) amp=-amp;
                v.x+=(V3XSCALAR)amp;
            }

            if (g_SGGame.Shock)
            {
                int amp = (g_SGGame.Shock);
                v.x+=(V3XSCALAR)((sysRand(amp)-(amp/2))<<8);
                v.y+=(V3XSCALAR)((sysRand(amp)-(amp/2))<<8);
            }
            if (g_pPlayer->fBooster)
            {
                v.y+=g_pPlayer->fBooster * 256;
            }

            // Speed
            {
                V3XVECTOR v2=g_pPlayer->J.OVI->ORI->Cs->velocity;
                v.x+=(-v2.z)*(V3XSCALAR)100;
                v.y+=(-v2.y)*(V3XSCALAR)100;
            }
            if (v.y>(V3XSCALAR)32767L) 
				v.y=(V3XSCALAR)32767L;

            if (v.x>(V3XSCALAR)32767L) 
				v.x=(V3XSCALAR)32767L;

            if (v.y<(V3XSCALAR)(-32767L)) 
				v.y=(V3XSCALAR)(-32767L);

            if (v.x<(V3XSCALAR)(-32767L)) 
				v.x=(V3XSCALAR)(-32767L);

            sJOY->FB_ApplyForce((int)v.x, (int)v.y);
        }
		*/

        if (sKEY_IsClicked(s_f1)) 
			g_pPlayer->Mx.but|=(1L<<18);
		else
        if (sKEY_IsClicked(s_f2)) 
			g_pPlayer->Mx.but|=(1L<<14);
		else
        if (sKEY_IsClicked(s_f3)) 
			g_pPlayer->Mx.but|=(1L<<15);
		else
        if (sKEY_IsClicked(s_f4))
			g_pPlayer->Mx.but|=(1L<<16);
		else
        if (sKEY_IsClicked(s_f5)) 
			g_pPlayer->Mx.but|=(1L<<17);
		else
        if (sKEY_IsClicked(s_f6)) 
			g_pPlayer->Mx.but|=(1L<<19);
		else
        if ((sKEY_IsClicked(s_f7))&&(g_SGGame.Missile)) 
			g_pPlayer->Mx.but|=(1L<<29);
    }
    else
    {
		int oV = sKEY->charCode;
        if ((oV==8)&&(g_SGInput.Car))
        {
            g_SGInput.Car--;
            g_SGInput.Code[g_SGInput.Car]=0;
        }
        else
        if (oV==13)
        {
            g_pPlayer->Notify+= SGNET_CHAT;
            g_szCOM[COM_Chat] = g_SGInput.Code;
            g_SGInput.isType = 0;
            sysConPrint("%s: %s", g_pCurrentGame->name, g_SGInput.Code);
        }
        else
        {
            if ((oV>=32) && (oV<128)&&(g_SGInput.Car<31))
            {
                g_SGInput.Code[g_SGInput.Car]=(char)oV;
                g_SGInput.Code[g_SGInput.Car+1]=0;
                g_SGInput.Car++;
            }
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int32_t static NG_ControlGame(void)
*
* DESCRIPTION :
*
*/
static int g_nOldTimer, g_bSaveIT;

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int32_t static NG_ControlGame(void)
*
* DESCRIPTION :
*
*/
static void Demo_RecordA(void)
{
    SGActor *J=&g_pPlayer->J;
    g_bSaveIT = 0;
    if (g_SGSettings.DemoMode<=1)
    {
        V3XVector_Cpy(g_pPlayer->Mat->v.Pos, g_pPlayer->Rot->pos);
        if (g_pPlayer->Mv.inert<1)               
			g_pPlayer->Mv.inert=1;
        if (g_pPlayer->Mv.inert>J->pInf.fSpeedMax)
			g_pPlayer->Mv.inert=J->pInf.fSpeedMax;
    }
    switch(g_SGSettings.DemoMode)
	{
        case 1:
        if (g_pRecordData.step<g_pRecordData.maxstep)
        if (g_SGGame.CameraMode!=CAMERA_NEWNAV+1)
        {
            int t = timer_ms();
            if (t>g_nOldTimer)
            {
                g_pPlayer->Mx.Mat = *(g_pPlayer->Mat);
                g_pRecordData.pos[g_pRecordData.step]=g_pPlayer->Mx;
                g_nOldTimer += (1000/30);
                g_bSaveIT=1;
            }
        }
        case 2:
        case 4:
        if (g_pRecordData.step<g_pRecordData.maxstep)
        if (g_SGGame.CameraMode!=CAMERA_NEWNAV+1)
        {
            int t = timer_ms();
            if (t>g_nOldTimer)
            {
                g_pPlayer->Mx = g_pRecordData.pos[g_pRecordData.step];
                *(g_pPlayer->Mat) = g_pPlayer->Mx.Mat;
                g_pPlayer->Rot->pos = g_pPlayer->Mat->v.Pos;
                g_bSaveIT=1;
                g_nOldTimer += (1000/30);
            }
        }
        else g_pPlayer->Mx.but|=(1L<<20);
        break;
    }
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_RecordReplayAction(void)
*
* Description :  
*
*/
static void NG_RecordReplayAction(void)
{
#if (SGTARGET ==NG_FULL_VERSION)
    SGRecordBuffer  *rec;
#endif
    g_pPlayer->Mx.but = 0;
    if (g_pRecordData.step>=g_pRecordData.maxstep)
    {
        g_SGObjects.quit=1;
        return;
    }
#if (SGTARGET ==NG_FULL_VERSION)
    if (g_bSaveIT)
    switch(g_SGSettings.DemoMode) {
        case 1:
        rec = g_pRecordData.rec + g_pRecordData.step;
        rec->length = NG_NetTranslate(g_pPlayer, rec->buffer);
        g_pRecordData.step++;
        break;
        case 2:
        case 4:
        rec = g_pRecordData.rec + g_pRecordData.step;
        NG_NetDispatchPlayer(0, rec->buffer);
        g_pRecordData.step++;
        break;
    }
#else
    if (g_bSaveIT)
    switch(g_SGSettings.DemoMode) {
        case 2:
        case 4:
        g_pRecordData.step++;
        break;
    }
    #endif
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int32_t static NG_ControlGame(void)
*
* DESCRIPTION :
*
*/
int32_t static NG_ControlGame(void)
{
    int dx=0, dy=0, InMax=0;
    V3XSCALAR oV;
    V3XVECTOR b;
    V3XOVI *OVI;
    SGActor *J=&g_pPlayer->J, *JJ;
    g_pPlayer->Notify = 0;
    NG_ReadInput(&dx, &dy, &InMax);
    Demo_RecordA();
    if (g_SGGame.CameraMode<CAMERA_NEWNAV)
    {
        if (g_pPlayer->Mv.x||g_pPlayer->Mv.y||g_pPlayer->Mv.z) g_pPlayer->Notify+=SGNET_HASTURN;
        g_pPlayer->Notify+=SGNET_HASMOVED;
        if (g_pPlayer->Mx.but)
        {
            if (g_pPlayer->Mx.but&1)
            {
                if (!g_pPlayer->J.pInf.ShootOk)
                {
                    if (g_cShip.wea[g_pPlayer->J.pInf.Attack-1].cur>0)
                    {
                        if (NG_WeaponFire(&g_pPlayer->J, g_pPlayer->J.pInf.Attack, g_pLockTarget))
                        {
                            g_cShip.wea[g_pPlayer->J.pInf.Attack-1].cur--;
                            g_cGameStat.shooted++;
                            g_pPlayer->Notify+=SGNET_NEWSHOOT;
                        }
                    }
                    else
                    {
                        g_pPlayer->J.pInf.Attack++;
                        if (g_pPlayer->J.pInf.Attack>6) 
							g_pPlayer->J.pInf.Attack=1;
                        g_pPlayer->Mx.but|=(1L<<(5+g_pPlayer->J.pInf.Attack));
                    }
                }
            }
            if (g_pPlayer->Mx.but&(1L<<2))
            {
                NG_AudioBeep(3);
                NG_HudLockTargetOn();
            }
            if (g_pPlayer->Mx.but&(1L<<3))
            {
                NG_AudioBeep(1);
                PlayPowerUp();
            }
            if (g_pPlayer->Mx.but&(1L<<4))
            {
                if (!g_pPlayer->fBooster)
                {
                    g_pPlayer->fBooster=256;
                    NG_AudioPlaySound(NG_AudioGetByName("turbo")-1, 0);
                }
            }
            if (g_pPlayer->Mx.but>16) /* TOUCHES RAREMENT UTILISE */
            {
                if (g_pPlayer->Mx.but&(1L<<5))
                {
                    if ((g_SGGame.MaxAim[g_SGObjects.NAV])&&(!g_SGSettings.DemoMode))
                    {
                        g_SGSettings.ComNumber = COM_NearBy;
                        g_SGSettings.ComTime = MAX_COM_DELAY;
                    }
                    else
                    {
                        OVI = NG_AILocateNearestEnemy(&g_pPlayer->Mat->v.Pos, &b, &oV, t_SPECIAL);
                        if (OVI)
                        {
                            if (((oV<5000L)||(g_SGSettings.DemoMode))&&(g_SGObjects.NAV<g_SGObjects.MaxNAV-1))
                            {
                                g_SGSettings.ComNumber = 0;
                                g_SGSettings.ComTime=MAX_COM_DELAY;
                                g_SGGame.CameraMode = CAMERA_NEWNAV;
                                g_pPlayer->Notify+=SGNET_HASWARP;
                            }
                            else
                            {
                                g_SGSettings.ComNumber = COM_TooFar;
                                g_SGSettings.ComTime=MAX_COM_DELAY;
                            }
                        }
                    }
                    if (g_SGSettings.Cheater)
                    if (g_SGObjects.NAV<g_SGObjects.MaxNAV) g_SGGame.CameraMode=CAMERA_NEWNAV;
                    NG_DisplayWarp();
                }
                if (g_pPlayer->Mx.but&(1L<<6))     {g_pPlayer->J.pInf.Attack=1;NG_AudioBeep(2);NG_HudLockTargetReset();g_SGGame.LockMAX = g_pShip[g_pCurrentGame->ship].wea[0].lockTime;}
                if (g_pPlayer->Mx.but&(1L<<7))     {g_pPlayer->J.pInf.Attack=2;NG_AudioBeep(2);NG_HudLockTargetReset();g_SGGame.LockMAX = g_pShip[g_pCurrentGame->ship].wea[1].lockTime;}
                if (g_pPlayer->Mx.but&(1L<<8))     {g_pPlayer->J.pInf.Attack=3;NG_AudioBeep(2);NG_HudLockTargetReset();g_SGGame.LockMAX = g_pShip[g_pCurrentGame->ship].wea[2].lockTime;}
                if (g_pPlayer->Mx.but&(1L<<9))     {g_pPlayer->J.pInf.Attack=4;NG_AudioBeep(2);NG_HudLockTargetReset();g_SGGame.LockMAX = g_pShip[g_pCurrentGame->ship].wea[3].lockTime;}
                if (g_pPlayer->Mx.but&(1L<<10))    {g_pPlayer->J.pInf.Attack=5;NG_AudioBeep(2);NG_HudLockTargetReset();g_SGGame.LockMAX = g_pShip[g_pCurrentGame->ship].wea[4].lockTime;}
                if (g_pPlayer->Mx.but&(1L<<11))    {g_pPlayer->J.pInf.Attack=6;NG_AudioBeep(2);NG_HudLockTargetReset();g_SGGame.LockMAX = g_pShip[g_pCurrentGame->ship].wea[5].lockTime;}
                if (g_pPlayer->Mx.but&(1L<<12))
                {
                    NG_AudioBeep(1);
                    g_pPlayer->CurArt--;
                    if (g_pPlayer->CurArt<1) g_pPlayer->CurArt = MAX_POWERUP;
                }
                if (g_pPlayer->Mx.but&(1L<<13))
                {
                    NG_AudioBeep(1);
                    g_pPlayer->CurArt++;
                    if (g_pPlayer->CurArt>MAX_POWERUP)   g_pPlayer->CurArt=1;
                }
                if (g_SGObjects.FinCode!=GAMESTATE_DEAD)
                {
                    if (g_pPlayer->Mx.but&(1L<<14))    {if (g_SGGame.CameraMode) {g_SGGame.CameraMode = 0;NG_AudioBeep(3);}}
                    if (g_pPlayer->Mx.but&(1L<<15))    {g_SGGame.CameraMode ++;NG_AudioBeep(3);if (g_SGGame.CameraMode>4) g_SGGame.CameraMode=1;}
                    if (g_pPlayer->Mx.but&(1L<<16))    {g_SGGame.CameraMode --;NG_AudioBeep(3);if (g_SGGame.CameraMode<1) g_SGGame.CameraMode=4;}
                    if (g_pPlayer->Mx.but&(1L<<17))    {g_SGSettings.rearview^=1;}
                    if (g_pPlayer->Mx.but&(1L<<18))    g_SGObjects.FinCode=GAMESTATE_HELP;
                    if (g_pPlayer->Mx.but&(1L<<19))    {g_SGGame.CameraMode = CAMERA_NAV;NG_AudioBeep(3);}
                    if (g_pPlayer->Mx.but&(1L<<29))    {g_SGGame.CameraMode = CAMERA_MISSILE;NG_AudioBeep(3);}
                    if (g_pPlayer->Mx.but&(1L<<21))    setFormation(0);
                    if (g_pPlayer->Mx.but&(1L<<22))    setFormation(1);
                    if (g_pPlayer->Mx.but&(1L<<23))    setFormation(2);
                    if (g_pPlayer->Mx.but&(1L<<24))    setFormation(3);
                    if (g_pPlayer->Mx.but&(1L<<25))    setFormation(4);
                    if (g_pPlayer->Mx.but&(1L<<26))    setFormation(5);
                    if (g_pPlayer->Mx.but&(1L<<27))    setFormation(6);
                    if (g_pPlayer->Mx.but&(1L<<28))    setFormation(7);
                }
                if (g_pPlayer->Mx.but&(1L<<20))    
                   g_SGObjects.FinCode=GAMESTATE_QUIT;
                if (g_pPlayer->Mx.but&(1L<<30))
                {
                    if (!g_pPlayer->fBooster)
                    {
                        g_pPlayer->fBooster=-128;
                        NG_AudioPlaySound(NG_AudioGetByName("turbo")-1, 0);
                    }
                }
            }
        }
        if (g_pPlayer->J.pInf.fFrozen<=0)
        {
            if (g_pPlayer->Mv.x)
            {
                g_pPlayer->Mv.x=(float)NG_MomentumValue((int32_t)g_pPlayer->Mv.x, InMax);
                V3XMatrix_Rotate_Y_Local((int32_t)g_pPlayer->Mv.x, g_pPlayer->Mat->Matrix);
            }
            if (g_pPlayer->Mv.y)
            {
                g_pPlayer->Mv.y=(float)NG_MomentumValue((int)g_pPlayer->Mv.y, InMax);
                V3XMatrix_Rotate_X_Local (g_SGSettings.FlipYMouse ? -(int32_t)g_pPlayer->Mv.y : (int32_t)g_pPlayer->Mv.y, g_pPlayer->Mat->Matrix);
            }
            if ((g_pPlayer->Mv.z)||(g_pPlayer->Mv.x))
            {
				g_pPlayer->Mv.z=(float)NG_MomentumValue((int32_t)g_pPlayer->Mv.z, InMax);
				V3XMatrix_Rotate_Z_Local((int32_t)(g_pPlayer->Mv.z-(g_pPlayer->Mv.x*.7)), g_pPlayer->Mat->Matrix);
            }
            if (!(g_SGGame.Count&63))
            {
                V3XVector_Normalize(&g_pPlayer->Mat->v.I, &g_pPlayer->Mat->v.I);
                V3XVector_Normalize(&g_pPlayer->Mat->v.J, &g_pPlayer->Mat->v.J);
                V3XVector_Normalize(&g_pPlayer->Mat->v.K, &g_pPlayer->Mat->v.K);
            }
            {
                int i;
                for (i=0;i<6;i++)
                {
					if (g_cShip.wea[i].reload)
                    if (V3X.Time.ms > g_cShip.wea[i].loadTime)
					{
						g_cShip.wea[i].loadTime = V3X.Time.ms + g_cShip.wea[i].reload*100;
						if (g_cShip.wea[i].cur<g_cShip.wea[i].max)
							g_cShip.wea[i].cur++;
					}
                }
            }

            oV = J->pInf.fSpeed;
            if (!g_pPlayer->fBooster)
            {
                if (g_pPlayer->Mx.but&2)
                {
                    J->pInf.fSpeed+= (J->pInf.fSpeed< J->pInf.fSpeedMax) ? g_cTimer.fCounter / 2 : 0;
                    if (J->pInf.fSpeed> J->pInf.fSpeedMax)
						J->pInf.fSpeed= J->pInf.fSpeedMax;
                }
                else
                {
                    J->pInf.fSpeed-= g_cTimer.fCounter * 2;
                    if (J->pInf.fSpeed < g_pPlayer->Mv.inert)
						J->pInf.fSpeed= g_pPlayer->Mv.inert;
                }
            }
            else
            {
                J->pInf.fSpeed+=g_pPlayer->fBooster;
                if (g_pPlayer->fBooster<0)
                {
                    if (J->pInf.fSpeed<  -J->pInf.fSpeedMax)
						J->pInf.fSpeed= -J->pInf.fSpeedMax;

                    g_pPlayer->fBooster+= g_cTimer.fCounter;

                    if (g_pPlayer->fBooster>0)
						g_pPlayer->fBooster=0;
                }
                if (g_pPlayer->fBooster>0)
                {
                    if (J->pInf.fSpeed>2*J->pInf.fSpeedMax)
						J->pInf.fSpeed=J->pInf.fSpeedMax*2;

                    g_pPlayer->fBooster-= g_cTimer.fCounter;

                    if (g_pPlayer->fBooster<0) 
						g_pPlayer->fBooster=0;
                }
            }
			
            if ((g_SGGame.pReactorMaterial)&&(!g_SGSettings.SerialGame))
            {
                V3XMATERIAL *Mat = g_SGGame.pReactorMaterial;
				int scale  = (u_int8_t)(J->pInf.fSpeed<0 ? 1 : (J->pInf.fSpeed * 256)/(J->pInf.fSpeedMax+1));
                Mat->diffuse.r = 
                Mat->diffuse.g = 
                Mat->diffuse.b = (u_int8_t)scale;
            }
			

            b.x = b.y = CST_ZERO; b.z = -J->pInf.fSpeed * g_cTimer.fCounter * 2.f;
            V3XVector_ApplyMatrix(J->Mv.vel, b, g_pPlayer->Mat->Matrix);
            V3XVector_Inc(&g_pPlayer->Rot->pos, &J->Mv.vel);
            if ((g_SGGame.CameraMode==0)&&(g_ubSampleUsed[g_cFXTable.Engine]!=g_cFXTable.SoundEngine))
            {
                //NG_AudioKickSound(g_cFXTable.Engine, g_cFXTable.SoundEngine, 0);
                oV = 65535;
            }
            if (oV!=J->pInf.fSpeed)
            {
               NG_AudioSetFreq(g_cFXTable.Engine, (int32_t)(44100+J->pInf.fSpeed*256));
            }
        }
    }
    OVI = g_pPlayer->J.OVI->collisionList;
    if (OVI)
    {
        g_pPlayer->J.OVI->collisionList = NULL;
        JJ = (SGActor*) OVI->data;
        switch(JJ->pInf.Type)
		{
            case t_SPECIAL:
            if ((g_SGObjects.NAV==g_SGObjects.MaxNAV-1)&&(JJ->pInf.ColorRadar!=255))
            {
                sysConPrint("Completed");
				g_SGObjects.FinCode = GAMESTATE_WON;
			}
            break;
            case t_OBSTACLE:
            case t_ENEMY:
            case t_PIRAT:
            case t_DECOR:
            case t_FRIEND:
            switch(JJ->pInf.CollisionStyle)
			{
                case t_CS_NAV:
                case t_CS_NAVWARP:
				{
					g_SGObjects.NAV=JJ->pInf.Code;
					if (g_SGObjects.NAV>=g_SGObjects.MaxNAV) g_SGObjects.NAV=g_SGObjects.MaxNAV-1;
					if (JJ->pInf.CollisionStyle==t_CS_NAVWARP) g_pPlayer->Notify+=SGNET_HASWARP;
					g_pPlayer->Notify+=SGNET_UPDATEAIM;
					NG_NAVReset(JJ->pInf.CollisionStyle==t_CS_NAVWARP);
				}
                break;
                case t_CS_BONUS:
                if (AddPowerUp(JJ->pInf.Code))
                {
                    JJ->pInf.Wait = 1;
                    g_cGameStat.bonus++;
                    sysConPrint("Pickup a bonus");
                }
                break;
                case t_CS_WARP:
				{
					JJ->pInf.Wait = 1;
					g_cGameStat.newlevel = JJ->pInf.Code-1;
				}
                break;
                case t_CS_ADD_SCORE_HIDE:
                {
                     JJ->pInf.Wait = 1;
                }
                case t_CS_ADD_SCORE:
				{
					g_SGGame.MaxAim[g_SGObjects.NAV]-=JJ->pInf.Mission;
					g_pPlayer->Notify+=SGNET_UPDATEAIM;
					if (JJ->pInf.Mission) NG_DisplayWarp();
					JJ->pInf.Mission = 0;
					if (JJ->pInf.Scoring)
					{
						g_SGGame.FlashAlpha=16;
						J->pInf.Scoring+=JJ->pInf.Scoring;
						JJ->pInf.Scoring = 0;
					}
					break;
				}
                case t_CS_BUMP_LOSE_SHIELD:
                if ((!(g_pPlayer->mode&GODMODE))&&(g_SGObjects.FinCode!=GAMESTATE_DEAD))
                {
                    J->pInf.Shield--;
                    JJ->pInf.Shield--;
                    NG_AudioPlaySound(NG_AudioGetByName("flag_st")-1, 0);
                    g_SGGame.FlashAlpha=16;
                    NG_FXImpact(4);
                    g_pPlayer->Notify+=SGNET_SHIELDCHANGE;
                }
                case t_CS_BUMP:
                {
                	J->pInf.fSpeed*=0.5f;
                }
                break;
                case t_CS_LOSE_SHIELD:
				{
					J->pInf.Shield-=2;
					NG_AudioPlaySound(NG_AudioGetByName("flag_st")-1, 0);
					g_SGGame.FlashColor.r=255;g_SGGame.FlashColor.g=0;g_SGGame.FlashColor.b=0;
					g_SGGame.FlashAlpha=16;
					g_pPlayer->Notify+=SGNET_SHIELDCHANGE;
				}
                break;
            }
        }
    }
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_DisplayWarp(void)
*
* DESCRIPTION :
*
*/
void NG_DisplayWarp(void)
{
    int i;
    V3XSCENE *Scene = g_SGGame.Scene;
    V3XOVI *OVI;
    V3XORI    *ORI;
    SGActor *J;
    if (g_SGGame.MaxAim[g_SGObjects.NAV]<=0)
    {
        if (!g_SGGame.WarpOk)
        {
            g_SGGame.WarpOk = 1;
            if (g_SGObjects.NAV<g_SGObjects.MaxNAV-1)
            {
                g_SGSettings.ComNumber = COM_Complete;
                g_SGSettings.ComTime = MAX_COM_DELAY;
                if (g_SGObjects.NAV)
                {
                    NG_AudioSay("ins02");
                    NG_AudioBeep(3);
                    sysConPrint("Sector completed");
                }
            }
            else
            {
                int n=0;
                g_SGSettings.ComNumber = COM_FindWarp;
                g_SGSettings.ComTime = MAX_COM_DELAY;
                if (g_SGObjects.NAV)
                {
                    NG_AudioSay("ins04");
                    sysConPrint("Mission completed");
                    NG_AudioBeep(3);
                }
                for (i=Scene->numOVI, OVI=Scene->OVI;(i!=0)&&(n!=1);OVI++, i--)
                {
                    ORI = OVI->ORI;
                    if (ORI->type==V3XOBJ_MESH )
                    {
                        J = (SGActor*)OVI->data;
                        if (J)
                        {
                            if (strstr(J->pInf.Basename, "warp"))
                            {
                                J->pInf.Wait = 0;
                                J->pInf.Shield = 100000;
                                n = 1;
                                OVI->state&=~V3XSTATE_HIDDEN;
                            }
                        }
                    }
                }
				SYS_ASSERT(n);
            }
        }
    }
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void NG_StarfieldUpdate(void)
{
    unsigned i;
    V3XSCALAR spid = g_pPlayer->J.pInf.fSpeed / ((RLX.V3X.Id>RLX3D_Software) ? 8 : 1);
    V3XSCALAR zm = DIVF32((V3XSCALAR)16, Starfield.Zclip);
    V3XVECTOR *v;
    int32_t c = (int32_t)(255 * g_pPlayer->J.pInf.fSpeed / g_pPlayer->J.pInf.fSpeedMax);
    u_int32_t s;
    if (c<0) c=-c;
    s = RGB_PixelFormat(c, c, c);
    if ((g_SGGame.Count&128)==0)
    {
        int32_t ct = (Starfield.maxStars/2);
        Starfield.Mx = min(Starfield.maxStars, (unsigned)(ct + (ct * 65536.f / g_cTimer.fCounter)));
    }
    XCHG(Starfield.Prj[1], Starfield.Prj[0], V3XVECTOR *);
    for (v=Starfield.Stars, i=0;i<Starfield.Mx;v++, i++)
    {
        V3XVECTOR W;
        V3XVECTOR *p1 = &Starfield.Prj[0][i];
        V3XVECTOR *p2 = &Starfield.Prj[1][i];
        V3XVECTOR p3;
        int Rs=0, ToM=0;
        V3XVector_TransformProject_pts(v, &W);
        while ((ToM<16)&&((W.z>=CST_ZERO)||(W.z<-Starfield.Zclip)))
        {
            NG_AIRandomAim(v, &V3X.Camera.M.v.Pos, Starfield.Zclip);
            V3XVector_TransformProject_pts(v, &W);
            Rs=1;
            ToM++;
        }
        {
            p1->x = W.x,
            p1->y = W.y;
            p1->z = W.z;
            if (Rs)
            {
                p2->x = W.x;
                p2->y = W.y;
                p2->z = W.z;
            }
            {
                V3XSCALAR x=p2->x-p1->x;
                V3XSCALAR y=p2->y-p1->y;
                V3XSCALAR z=p2->z-p1->z;
                V3XSCALAR d=MULF32_SQR(x)+MULF32_SQR(y);
                if (d>MULF32_SQR(spid))
                {
                    d = DIVF32(MULF32(spid, CST_ONE), sqrtf(d));
                    p2->x = p1->x+MULF32(x, d);
                    p2->y = p1->y+MULF32(y, d);
                    p2->z = p1->z+MULF32(z, d);
                }
                if ((RLX.V3X.Id>RLX3D_Software))
                {
                    p3.x = ( p1->x + p2->x )/2;
                    p3.y = ( p1->y + p2->y )/2;
                    p3.z = ( p1->z + p2->z )/2;
                    V3X.Ln.lineBuffer[V3X.Ln.nbLines + 0] = *p1;
                    V3X.Ln.lineBuffer[V3X.Ln.nbLines + 1] =  p3;
                    if (
                    V2XVector_Clip((V3XVECTOR2*)(V3X.Ln.lineBuffer + V3X.Ln.nbLines + 0),
                    (V3XVECTOR2*)(V3X.Ln.lineBuffer + V3X.Ln.nbLines + 1)))
                    {
                        RGB32_Set(V3X.Ln.lineColor[V3X.Ln.nbLines+0], 0, 0, 0, 0);
                        RGB32_Set(V3X.Ln.lineColor[V3X.Ln.nbLines+1], 255, 255, 255, 255);
                        V3X.Ln.nbLines+=2;
                    }
                    V3X.Ln.lineBuffer[V3X.Ln.nbLines+0] =  p3;
                    V3X.Ln.lineBuffer[V3X.Ln.nbLines+1] = *p2;
                    if (
                    V2XVector_Clip((V3XVECTOR2*)(V3X.Ln.lineBuffer + V3X.Ln.nbLines + 0),
                    (V3XVECTOR2*)(V3X.Ln.lineBuffer + V3X.Ln.nbLines + 1)))
                    {
                        RGB32_Set(V3X.Ln.lineColor[V3X.Ln.nbLines+0], 255, 255, 255, 255);
                        RGB32_Set(V3X.Ln.lineColor[V3X.Ln.nbLines+1], 0, 0, 0, 0);
                        V3X.Ln.nbLines+=2;
                    }
                }
                else
                {
                    GX_ClippedLine((V3XVECTOR2*)p1, (V3XVECTOR2*)p2, GX.View.BytePerPixel == 1 ? (u_int32_t)(95 - MULF32(zm, W.z+Starfield.Zclip)) : s);
                }
            }
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int DrawBoxTarg(V3XVECTOR *p, int cx, V3XORI *ORI, SGScript *pInf)
*
* DESCRIPTION :
*
*/
static int DrawBoxTarg(V3XVECTOR *p, u_int32_t cx, V3XORI *ORI, SGScript *pInf)
{
    V3XVECTOR a;
    V3XVECTOR2 i0, j0, k0, l0;
    V3XVECTOR2 i1, j1, k1;
    V3XVECTOR2 i2;
    V3XSCALAR Diam = ORI->global_rayon;
    V3XSCALAR sx;
    int32_t  disp=0, inf;
    V3XVECTOR s;
    if (Diam>(V3XSCALAR)512)
		Diam=(V3XSCALAR)512;

    if (V3XVector_TransformProject_pts(p, &a))
    {
        sx = V3X.ViewPort.Focal / a.z;
        s.y = MULF32(Diam, sx);
        s.x = s.y*V3X.ViewPort.Ratio;
        i0.x= a.x-s.x; i0.y=a.y-s.y;
        j0.x= i0.x;    j0.y=a.y+s.y;
        k0.x= a.x+s.x; k0.y=j0.y;
        l0.x= k0.x; l0.y=i0.y;
        s.x = ((V3XSCALAR)3*s.x)/(V3XSCALAR)4;
        s.y = ((V3XSCALAR)3*s.y)/(V3XSCALAR)4;
        i1.x= a.x - s.x;
        i1.y= a.y - s.y;
        j1.y= a.y + s.y;
        k1.x= a.x + s.x;
        // Relock (pas de line en hrrdware)
        if (pInf->Locked==0)
        {
            i2.y=i0.y;i2.x=i1.x;
            disp+=GX_ClippedLine(&i0, &i2, cx);
            i2.y=i1.y;i2.x=i0.x;
            disp+=GX_ClippedLine(&i0, &i2, cx);
            i2.y=j0.y;i2.x=i1.x;
            disp+=GX_ClippedLine(&j0, &i2, cx);
            i2.y=j1.y;i2.x=j0.x;
            disp+=GX_ClippedLine(&j0, &i2, cx);
            i2.y=k0.y;i2.x=k1.x;
            disp+=GX_ClippedLine(&k0, &i2, cx);
            i2.y=j1.y;i2.x=k0.x;
            disp+=GX_ClippedLine(&k0, &i2, cx);
            i2.y=l0.y;i2.x=k1.x;
            disp+=GX_ClippedLine(&l0, &i2, cx);
            i2.y=i1.y;i2.x=l0.x;
            disp+=GX_ClippedLine(&l0, &i2, cx);
        }
        else
        {
            disp+=GX_ClippedLine(&i0, &j0, cx);
            disp+=GX_ClippedLine(&j0, &k0, cx);
            disp+=GX_ClippedLine(&k0, &l0, cx);
            disp+=GX_ClippedLine(&l0, &i0, cx);
            j0.x=a.x-V3X.ViewPort.center.x;
            j0.y=a.y-V3X.ViewPort.center.y;
            sx = V3X.ViewPort.center.x/4;
            g_szLockText[2]= ((pInf->Type==t_PLAYER)&&(g_SGSettings.SerialGame)) ? pInf->Realname : NULL;
            inf = 2;
            if (g_SGGame.LockMAX)
            {
                if ((j0.x*j0.x+j0.y*j0.y)<sx*sx)
                {
                    pInf->Locked=2;
                    inf=1;
                }
                else
                    pInf->Locked=1;
                if (pInf->Locked==2)
                {
                    int32_t oy, ox;
                    oy=ox=pInf->LockTime;
                    oy+=1+(g_cTimer.iCounter>>15);
                    if (oy>g_SGGame.LockMAX)
                    {
                        oy=g_SGGame.LockMAX;
                        if (ox<g_SGGame.LockMAX)
                        {
                            g_SGLockMode.dmode|=256;
                        }
                        inf=0;
                    }
                    if ((oy>=g_SGGame.LockMAX)&&g_SGSettings.cursor)
                    {
                       ox=g_SGGame.CI_WHITE;
                    }
                    else
                       ox=cx;
                    NG_DrawCircle((V3XVECTOR2*)&a, (int32_t)((oy*s.y)/g_SGGame.LockMAX), ox, 4);
                    pInf->LockTime = (u_int8_t)oy;
                    disp = -1;
                }
            }
            if (disp)
            {
                V3XVector_Dif(&a, &g_pLockTarget->mesh->matrix.v.Pos, &V3X.Camera.M.v.Pos);
                sysStrnCpy(g_SGLockMode.szText, g_szLockText[inf] ? g_szLockText[inf] : " ", 31);
                g_SGLockMode.dmode   |= inf ? 2 : 1;
                g_SGLockMode.color = cx;
                g_SGLockMode.x = (int32_t)i0.x;
                g_SGLockMode.y = (int32_t)i0.y;
                g_SGLockMode.x0 = (int32_t)l0.x;
                g_SGLockMode.y0 = (int32_t)l0.y;
                g_SGLockMode.dist = ((u_int32_t)(V3XVector_Length(&a) / 8));
                if (g_pCurrentGame->ship==0)
                {
                    g_SGLockMode.shield = (pInf->Shield*100)/pInf->ShieldMax;
                }
            }
        }
    }
    if (disp>=0)
    {
        pInf->LockTime=(disp!=0);
        if (disp==0)
        {
            if (pInf->Locked==2) pInf->Locked=1;
        }
    }
    return pInf->Locked;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void AddingFaceMoreDisp(void)
*
* DESCRIPTION :  
*
*/
static void AddingFaceMoreDisp(void)
{
    V3XSCENE *Scene = g_SGGame.Scene;
    V3XOVI *OVI;
    V3XORI    *ORI;
    SGActor *J;
    SGScript *pInf;
    int i;

    if (g_SGSettings.bClearView)
    {
        g_SGSettings.bClearView=0;
        return;
    }

    if (g_SGObjects.Nav->SpaceDebris )
    {
        NG_StarfieldUpdate();
    }
    for (i=Scene->numOVI, OVI=Scene->OVI;i!=0;OVI++, i--)
    {
        ORI = OVI->ORI;
        if (OVI->data)
        {
            J = (SGActor*)OVI->data;
            pInf = &J->pInf;
            if ( pInf->ModeX )
            {
                switch(pInf->ColorRadar) {
                    case 0:
                    case 255:
                    break;
                    default:
                    DrawBoxTarg(&OVI->mesh->matrix.v.Pos, g_SGGame.pColorRadar[pInf->ColorRadar], ORI, pInf);
                    break;
                }
            }
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_AISetTacticMode(void)
*
* DESCRIPTION :
*
*/
void NG_AISetTacticMode(void)
{
    switch(g_SGObjects.CallMode[t_FRIEND]) {
        case ORDER_Attack:
        case ORDER_StayAlert:
        case ORDER_Retreat:
        g_SGObjects.defTarget[t_ENEMY] = &g_SGObjects.NavCam->mesh->matrix.v.Pos; // ami
        break;
        case ORDER_TargetWeakest:
        case ORDER_TargetStrongest:
        g_SGObjects.defTarget[t_ENEMY] = &g_pPlayer->Mat->v.Pos;
        break;
        case ORDER_AttackTarget:
        case ORDER_Protect:
        if (g_pLockTarget )  g_SGObjects.defTarget[t_ENEMY] = &g_pLockTarget->mesh->matrix.v.Pos;
        else g_SGObjects.defTarget[t_ENEMY] = &g_pPlayer->Mat->v.Pos;
        break;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_InitAnimateStage(void)
*
* DESCRIPTION :
*
*/
void NG_InitAnimateStage(void)
{
    int i;
    V3XSCENE *Scene = g_SGGame.Scene;
    V3XOVI *OVI, *OVIp;
    V3XTRI *TRI;
    SGActor *J;
    SGScript *pInf;
    for (i=Scene->numOVI, OVI=Scene->OVI;i!=0;OVI++, i--)
    {
        if (OVI->data)
        {
            J = (SGActor*)OVI->data;
            J->pEffect = NULL;

            J->OVItarget = NULL;
            pInf = &J->pInf;
            if ((pInf->Wait)||(pInf->Appear))
				OVI->state |= V3XSTATE_HIDDEN;

            if (pInf->Stealth) 
				OVI->state|=V3XSTATE_HIDDENDISPLAY;

            pInf->fSpeed = pInf->fSpeedMax / 2;

            TRI = OVI->TVI->TRI;
			
            switch(pInf->Animation)
			{
                case t_GIANT:
                break;
                case t_TURN_AROUND_X:
                case t_TURN_AROUND_Y:
                case t_TURN_AROUND_Z:
                case t_TURN_RANDOM:
					OVI->matrix_Method = V3XMATRIX_Euler; // Calcul avec des angles
                break;
                default: // Quaternion
                break;
            }
            switch(pInf->Tactic) {
                case t_TAC_RANDOM_PATROL:
                case t_TAC_PATROL_FIGHT1:
                case t_TAC_TRACK:
                case t_TAC_HIND:
                case t_TAC_ATTACK1:
					OVI->matrix_Method = V3XMATRIX_Vector2; // Calcul avec system Base				
                break;
            }

			if ((J->pInf.Type != t_ACCESSOIRE) && (OVI->matrix_Method == V3XMATRIX_Vector2))
			{
				J->Mv.k = -1;
				J->Mv.Ang = i*128;

				if (pInf->Tactic == t_TAC_TRACK)
				{
					J->Mv.aim = TRI->keys[1].info.pos;
					V3XVector_Dif(&J->Mv.OldAim, &TRI->keys[1].info.pos, &OVI->Tk->info.pos);
					V3XVector_Normalize(&J->Mv.OldAim, &J->Mv.OldAim);
				}
				else
				{
					J->Mv.aim = OVI->Tk->info.pos;
					J->Mv.OldAim = OVI->mesh->matrix.v.K;
				}
				OVI->Tk->dinfo.vect = J->Mv.NewAim = J->Mv.OldAim;
			}


            if (pInf->Replace)
            {
                char *g=strstr(OVI->ORI->name, "_");
                if (g)
                {
					char tex[256];
                    sprintf(tex, "%s%s", g_pPlayerInfo[pInf->Replace].Basename, g);
                    OVIp = V3XScene_OVI_GetByName(g_SGGame.Scene, tex);
					SYS_ASSERT(OVIp);
                    if (OVIp)
                        OVIp->state |= V3XSTATE_HIDDEN;
                }
            }
        }
    }
    V3X.Setup.post_render = AddingFaceMoreDisp;
    V3X.Setup.add_poly = NG_AddFaceMoreObjects;
	V3X.Setup.add_lights = NG_AddMoreLights;
    g_SGObjects.defTarget[t_FRIEND] = &g_pPlayer->Mat->v.Pos; // enemi
    NG_AISetTacticMode();
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static void Line3D(V3XSCALAR x, V3XSCALAR y, V3XSCALAR z, V3XSCALAR x0, V3XSCALAR y0, V3XSCALAR z0, int c, V3XSCALAR *mat)
{
    V3XVECTOR A, C;
    V3XVECTOR pA, pB;
    C.x=x;C.y=y; C.z=z;
    V3XVector_ApplyTransposeMatrix(A, C, mat); A.z-=g_SGGame.RadarRange*3;
    if (A.z<CST_ZERO) V3XVector_ProjectWithCenter(pA, A);
    C.x=x0;C.y=y0; C.z=z0;
    V3XVector_ApplyTransposeMatrix(A, C, mat); A.z-=g_SGGame.RadarRange*3;
    if (A.z<CST_ZERO) V3XVector_ProjectWithCenter(pB, A);
    GX.gi.drawAnyLine((int32_t)pA.x, (int32_t)pA.y, (int32_t)pB.x, (int32_t)pB.y, c);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void Display_Radar(void)
*
* DESCRIPTION :
*
*/
static void Display_Radar(void)
{
    int32_t i, Rad, g=g_pCurrentGame->ship==2;
    V3XSCALAR z0;
    V3XVECTOR dir, di2, nearest;
    V3XVECTOR2 pt2, pt3;
    V3XVECTOR pt;
    RW_Button *but=g_pGameBoard->item + VCfg_win_radar;
    SGActor *Data;
    V3XSCENE *Scene = g_SGGame.Scene;
    V3XOVI *OVI;
    GXVIEWPORT oldView = GX.View;
    g_SGGame.MaxDetect[0] = g_SGGame.MaxDetect[1] = 0;

	V3XVector_Set(&nearest, 0,0, g_SGGame.RadarRange);
    if (!g_SGGame.RadarMode)
    {
        if (g)
        {
            but = g_pGameBoard->item + 38;
            pt3.x = (V3XSCALAR)((but->LX + but->X-g_pspHud2->item[1].LX)/2);
            pt3.y = (V3XSCALAR)((but->LY + but->Y-g_pspHud2->item[1].LY)/2);
        }
        but = g_pGameBoard->item + VCfg_win_radar;
        GX.View.xmin = but->X;
        GX.View.xmax = GX.View.xmin + but->LX;
        GX.View.ymin = but->Y;
        GX.View.ymax = GX.View.ymin + but->LY;
        pt2.x = (V3XSCALAR)((GX.View.xmax+GX.View.xmin-g_pspHud2->item[1].LX)/2);
        pt2.y = (V3XSCALAR)((GX.View.ymax+GX.View.ymin-g_pspHud2->item[1].LY)/2);
        z0 = (V3XSCALAR)(but->LX/2);
    }
    else
    {
        V3XSCALAR x0=CST_ZERO, y0=CST_ZERO, r;

        z0 = (V3XSCALAR)g_SGGame.RadarRange;
        r = z0;
        Line3D(CST_ZERO, CST_ZERO, -z0, CST_ZERO, CST_ZERO, z0, g_SGGame.CI_BLUE, g_pPlayer->Mat->Matrix);
        Line3D(CST_ZERO, -z0, CST_ZERO, CST_ZERO, z0, CST_ZERO, g_SGGame.CI_BLUE, g_pPlayer->Mat->Matrix);
        Line3D(-z0, CST_ZERO, CST_ZERO, z0, CST_ZERO, CST_ZERO, g_SGGame.CI_BLUE, g_pPlayer->Mat->Matrix);
        for (i=0;i<=32;i++)
        {
            V3XSCALAR x1=MULF32(r, TRG_sin(i<<7));
            V3XSCALAR y1=MULF32(r, TRG_cos(i<<7));
            if (i)
            {
                Line3D(x1, y1, 0, x0, y0, 0, g_SGGame.CI_BLUE, g_pPlayer->Mat->Matrix);
                Line3D(x1, 0, y1, x0, 0, y0, g_SGGame.CI_BLUE, g_pPlayer->Mat->Matrix);
                Line3D(0, x1, y1, 0, x0, y0, g_SGGame.CI_BLUE, g_pPlayer->Mat->Matrix);
            }
            x0 = x1;
            y0 = y1;
        }
    }
    z0 = g_pPlayer->Mat->v.Pos.z;
    for (i=Scene->numOVI, OVI=Scene->OVI;i!=0;OVI++, i--)
    {
        Data = (SGActor*)OVI->data;
        if (Data)
        {
            if (
            (OVI==g_SGObjects.NavCam)
            ||
            (
            (   (OVI->state&V3XSTATE_HIDDEN )==0)
            &&(   (OVI->state&V3XSTATE_CULLEDOFR )==0)
            &&(   (OVI->state&V3XSTATE_HIDDENDISPLAY )==0)
            )
            )
            {
                if (Data->pInf.ColorRadar>0)
                {
				//	float scale = (Data->pInf.Locked ? 1.25 : 1);
                    V3XVector_Dif(&dir, &OVI->mesh->matrix.v.Pos, &g_pPlayer->Rot->pos);
                    if (Data->pInf.ColorRadar==255)
                        Rad = 5;
                    else 
						Rad = ((Data->pInf.ColorRadar-1)%5)+1;
                    if (g_SGGame.RadarMode)
                    {
                        V3XVector_ApplyTransposeMatrix(di2, dir, g_pPlayer->Mat->Matrix);
                        CSP_Color(g_SGGame.CI_WHITE);
                        if ((fabs(di2.z)<nearest.z)||(Data->pInf.Locked))
                        {
                            nearest = di2;
                        }
                        di2.z-=g_SGGame.RadarRange*4;
                        if (di2.z<CST_ZERO)
                        {
                            V3XVector_ProjectWithCenter(pt, di2);
                            GX.csp.put((int32_t)pt.x, (int32_t)pt.y, g_pspHud2->item + Rad);
                            g_SGGame.MaxDetect[0]++;
                        }
                    }
                    else
                    {
                        di2.z = V3XVector_DotProduct(&dir, &V3X.Camera.M.v.K);
                        if (di2.z<=CST_ZERO)
                        {
                            CSP_Color(g_SGGame.CI_WHITE);
							if (V3XVector_Length(&dir)>CST_EPSILON)
								V3XVector_Normalize(&dir, &dir);
							else
								V3XVector_Set(&dir, 0, 0, 1);

                            di2.x = V3XVector_DotProduct(&dir, &V3X.Camera.M.v.I);
                            di2.y = V3XVector_DotProduct(&dir, &V3X.Camera.M.v.J);
							
                            pt.x = pt2.x + MULF32(di2.x, (but->LX/2)-4);
                            pt.y = pt2.y - MULF32(di2.y, (but->LY/2)-4);
                            if ((pt.x>0)&&(pt.y>0))
                                GX.csp.put((int32_t)pt.x, (int32_t)pt.y, g_pspHud2->item + Rad);
                            g_SGGame.MaxDetect[0]++;
                        }
                        if (g)
                        {
                            but = g_pGameBoard->item + 38;
                            if( di2.z>CST_ZERO)
                            {
                                CSP_Color(g_SGGame.CI_WHITE);
                                V3XVector_Normalize(&dir, &dir);
                                di2.x = V3XVector_DotProduct(&dir, &V3X.Camera.M.v.I);
                                di2.y = V3XVector_DotProduct(&dir, &V3X.Camera.M.v.J);
                                pt.x = pt3.x + MULF32(di2.x, (but->LX/2)-4);
                                pt.y = pt3.y - MULF32(di2.y, (but->LY/2)-4);
                                if ((pt.x>0)&&(pt.y>0))
                                    GX.csp.put((int32_t)pt.x, (int32_t)pt.y, g_pspHud2->item + Rad);
                                g_SGGame.MaxDetect[1]++;
                                but=g_pGameBoard->item + VCfg_win_radar;
                            }
                        }
                    }
                }
            }
        }
    }
    g_pPlayer->Mat->v.Pos.z = z0;
    CSP_Color(g_SGGame.CI_WHITE);
    if (g_SGGame.RadarMode)
    {
        GXSPRITE *sp;
        if (MULF32_SQR(nearest.x)+MULF32_SQR(nearest.y)> MULF32_SQR((V3XSCALAR)1024))
        {
            if (fabs(nearest.x)>fabs(nearest.y))
            {
                if (nearest.x<CST_ZERO) sp = g_pspHud2->item + 16;     // gauche
                else                 sp = g_pspHud2->item + 18;     // droit
            }
            else
            {
                if (nearest.y>CST_ZERO) sp = g_pspHud2->item + 17;     // haut
                else                 sp = g_pspHud2->item + 15;     // bas
            }
            GX.csp.put(((int32_t)V3X.ViewPort.center.x-(sp->LX/2)),
            ((int32_t)V3X.ViewPort.center.y-(sp->LY/2)), sp);
        }
    }
    GX.View = oldView;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void info_weapon(int val, int defc, RW_Button *but, int l)
*
* DESCRIPTION :
*
*/
#define COLOR_BLUE 0xff
static void info_weapon(int val, int defc, RW_Button *but, int l)
{
	char tex[256];
    sprintf(tex, "%02d~ %d", val, l);
    CSP_DrawTextC(tex, but->X, but->Y, defc, COLOR_BLUE, g_pspCat, GX.csp_cfg.put);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static void HotPowerBar(int x, int y, int val, int max)
{
    GXSPRITE *sp=g_pspHud2->item+12;
    int i, m=(val*12)/max, lx=1 ? sp->LX-4 : sp->LX-3;
    if (m>12) m=12;
    for (i=0;i<m;i++, x+=lx) GX.csp.put(x, y, sp);
    return ;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void Display_HUD(void)
*
* DESCRIPTION :
*
*/
static void Display_HUD(void)
{
    int32_t x, m;
    GXSPRITE *sp;
    RW_Button *but;
	int bAllowTrsp = 1; // (GX.View.Flags&GX_CAPS_BACKBUFFERINVIDEO)==0;
    SGScript *pInf = &g_pPlayer->J.pInf;

    if (g_SGLockMode.dmode&32)
    {
        RW_Button *b2=g_pGameBoard->item + VCfg_sp_rear;
        GX.csp.put(b2->X, b2->Y, g_pspHud->item + VCfg_sp_rear);

		if (bAllowTrsp)
          GX.csp.TrspADD(g_pGameBoard->item[34].X, g_pGameBoard->item[34].Y, g_pspHud2->item + 13);
    }
    
    if (g_SGLockMode.dmode&31)
    {
        CSP_Color(g_SGGame.CI_WHITE);
        GX.csp.put(g_pGameBoard->item[VCfg_sp_camis].X, g_pGameBoard->item[VCfg_sp_camis].Y, g_pspHud->item+VCfg_sp_camis);
        
		if (bAllowTrsp)
    		GX.csp.TrspADD(g_pGameBoard->item[35].X, g_pGameBoard->item[35].Y, g_pspHud2->item +13);
        
		if (g_SGLockMode.dmode&3)
        {
			char tex[256];
            g_SGLockMode.dmode&=~3;
            CSP_Color(g_SGLockMode.color);
            CSP_WriteText(g_SGLockMode.szText, g_SGLockMode.x, g_SGLockMode.y-g_pspCat->item[0].LY-1, g_pspCat);
            sprintf(tex, "%d m", (int)(g_SGLockMode.dist / 10));
            CSP_WriteText(tex, g_SGLockMode.x0+2, g_SGLockMode.y0, g_pspCat);
            // Bar energy
            if (g_pCurrentGame->ship==0)
            {
                CSP_Color(g_SGLockMode.shield<25 ? g_SGGame.CI_RED : g_SGGame.CI_GREEN);
                sprintf(tex, "%d %%", (int)g_SGLockMode.shield);
                CSP_WriteText(tex, g_SGLockMode.x0+2, g_SGLockMode.y0+g_pspCat->item[0].LY+1, g_pspCat);
            }
        }
    }
    if (g_SGSettings.showInf&1) 
		m=g_pGameBoard->numItem;
	else
		m=1;
    for (sp=g_pspHud->item, x=0;x<m;x++, sp++)
    {
		char tex[256];
        int xxx;
        CSP_Color(g_SGGame.CI_WHITE);
        but = g_pGameBoard->item + x;
        xxx = but->X+g_SGGame.Shock;
        switch(x){
            case 0: // g_pspHud
            if (!g_SGGame.RadarMode)
            {
				char tex[256];
                V3XVECTOR dx;
                V3XVector_Dif(&dx, &g_SGObjects.NavCam->Tk->vinfo.pos, &V3X.Camera.M.v.Pos);
                GX.csp.put(xxx, but->Y, sp);
                sprintf(tex, "%02d", g_SGGame.MaxAim[g_SGObjects.NAV]);
                CSP_WriteText(tex, g_pGameBoard->item[24].X, g_pGameBoard->item[24].Y, g_pspCat);
                sprintf(tex, "%06d", (int)(V3XVector_Length(&dx)));
                CSP_WriteText(tex, g_pGameBoard->item[25].X, g_pGameBoard->item[25].Y, g_pspCat);
                sprintf(tex, "%03d", (int)g_pPlayer->Mv.x);
                CSP_WriteText(tex, g_pGameBoard->item[26].X, g_pGameBoard->item[26].Y, g_pspCat);
                sprintf(tex, "%03d", (int)g_pPlayer->Mv.y);
                CSP_WriteText(tex, g_pGameBoard->item[27].X, g_pGameBoard->item[27].Y, g_pspCat);
            }
            break;
            case 1: // SGWeapon
            GX.csp.put(xxx, but->Y, sp);
            break;
            case 21: // This bonus
            GX.csp.pset(xxx, but->Y, g_pspHud2->item + 5 + g_pPlayer->CurArt);
            break;
            case 11: // Shield
            sprintf(tex, "%03d", (int)(pInf->Shield*100/pInf->ShieldMax));
            CSP_WriteText(tex, xxx, but->Y, g_pspCat);
            break;
            case 9: //  Speed
            sprintf(tex, "%03d", (int)(pInf->fSpeed*100/pInf->fSpeedMax));
            CSP_WriteText(tex, xxx, but->Y, g_pspCat);
            break;
            case 12: //  Time Game
			sprintf(tex, "%d%c%02d", (int)g_SGObjects.Time/60, 
				V3X.Time.ms%1000<500 ? ':' : ' ', (int)g_SGObjects.Time%60);
            if (g_SGObjects.IsTimed)
            {
                GXSPRITEGROUP *Pig = g_pspMsg;
                int o, ox=0;
				int l = strlen(tex);
                for (o=0;o<l;o++)
                {
					GXSPRITE *osp;				
					osp = tex[o] == ':' || tex[o] == ' ' ? g_pFontMenuLrg->item+39: Pig->item + tex[o]-'0'+2;
                    ox += osp->LX;
                }
                ox = (GX.View.xmax-ox)/2;
                for (o=0;o<l;o++)
                {					
					GXSPRITE *osp = tex[o] == ':' || tex[o] == ' ' ?  g_pFontMenuLrg->item+39: Pig->item + tex[o]-'0'+2;
					if (tex[o]!=' ')
                    GX.csp.TrspADD(ox, GX.View.ymax/4, osp);
                    ox += osp->LX;
                }
                if ((g_SGObjects.Time<30)&&(g_SGGame.Count&4)) 
					CSP_Color(g_SGGame.CI_YELLOW);

                if ((g_SGObjects.Time<10)&&(g_SGGame.Count&4)) 
					CSP_Color(g_SGGame.CI_RED);
            }
            CSP_WriteText(tex, xxx, but->Y, g_pspCat);
            break;
            case 22: // Val
            sprintf(tex, "%02d", g_pPlayer->Art[g_pPlayer->CurArt].val);
            CSP_WriteText(tex, xxx, but->Y, g_pspCat);
            break;
            case 23: // WeaTime
            sprintf(tex, "%02d", g_pPlayer->Art[g_pPlayer->CurArt].Time_val>>5);
            CSP_WriteText(tex, xxx, but->Y, g_pspCat);
            break;
            case 20: // Aim
            /*if (!g_SGGame.RadarMode)
            {
                sprintf(tex, "%02d", g_SGGame.MaxAim[g_SGObjects.NAV]);
                CSP_WriteText(tex, xxx, but->Y, g_pspCat);
            }
            */
            break;
            case 19: // Nav
            //sprintf(tex, "%02d", g_SGObjects.NAV);
            //CSP_WriteText(tex, xxx, but->Y, g_pspCat);
            break;
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            info_weapon(g_cShip.wea[x-13].cur, pInf->Attack==x-12 ? g_SGGame.CI_YELLOW : g_SGGame.CI_WHITE, but, x-12);
            break;
            case 2: // Speed
            GX.csp.put(xxx, but->Y, sp);
            break;
            case 8:
            HotPowerBar(xxx, but->Y, (int)pInf->fSpeed, (int)pInf->fSpeedMax);
            break;
            case 3: // Shield
            GX.csp.put(xxx, but->Y, sp);
            break;
            case 10:
            HotPowerBar(xxx, but->Y, pInf->Shield, pInf->ShieldMax);
            break;
            case 4: // Radar
            if (!g_SGGame.RadarMode)
            GX.csp.put(xxx, but->Y, sp);
            break;
            case 36:
            case 37:
            case 38:
            case 40:
            {
                int act=0;
                switch(g_pCurrentGame->ship){
                    case 0:
                    switch(x) {
                        case 36: act=2; break;
                        case 37: act=1; break;
                        case 38: act=3; break;
                    }
                    break;
                    case 1:
                    switch(x) {
                        case 36: act=2; break;
                        case 37: act=3; break;
                    }
                    break;
                    case 2:
                    switch(x) {
                        case 36:
                        case 38: act=2; break;
                        case 37: act=4; break;
                        case 40: act=3; break;
                    }
                    break;
                }
                switch(act) {
                    case 1: // Decor
                    break;
                    case 2: // glenz
                    if (!g_SGGame.RadarMode)
                    {
						CSP_Color((GX.View.BytePerPixel > 1 ? RGBA_PixelFormat(255, 255, 255, 0) : 255));
                        GX.csp_cfg.table = g_SGGame.Scene->Layer.lt.additive.table;
                        GX.csp.TrspADD(xxx, but->Y, g_pspHud2->item + 14);
                    }
                    break;
                    case 3: // Tir
                    {
                        int cc;
                        but->X = (short)((GX.View.xmax - 6*(but->LX-1))>>1);
                        for (cc=0;cc<6;cc++, xxx+=but->LX-1)
                        {
                            GX.csp.pset(xxx, but->Y, g_pspHud2->item + 19 + (cc<<1) + (pInf->Attack!=cc+1) );
                        }
                    }
                    break;
                    case 4: // g_pspHud
                    if (!g_SGGame.RadarMode)
                    GX.csp.put(xxx, but->Y, g_pspHud->item + 8);
                    break;
                }
            }
            break;
            case 7: // ComInfo
            if (g_SGGame.ComMode)
				GX.csp.put(xxx, but->Y, sp);
            break;
            case 30:
            if (g_SGGame.ComMode)
            {
                int y=but->Y, i;
                for (i=0;i<8;i++, y+=g_pspCat->item[0].LY+1)
                {
                    CSP_Color((i!=g_SGObjects.CallMode[t_FRIEND] ? g_SGGame.CI_WHITE : g_SGGame.CI_YELLOW));
                    CSP_WriteText(g_szOrder[i], xxx, y, g_pspCat);
                }
            }
            break;
            case 33:
            sprintf(tex, "%02d", g_SGGame.MaxDetect[0]);
            if (!g_SGGame.RadarMode)
				CSP_WriteText(tex, xxx, but->Y, g_pspCat);
            break;
            case 39:
            sprintf(tex, "%02d", g_SGGame.MaxDetect[1]);
            if (!g_SGGame.RadarMode)
				CSP_WriteText(tex, xxx, but->Y, g_pspCat);
            break;
        }
    }
	CSP_Color(g_SGGame.CI_WHITE);
    if ((g_SGGame.MaxAim[g_SGObjects.NAV]==0)&&(g_SGObjects.NAV<g_SGObjects.MaxNAV-1))
    {
        V3XSCALAR oV;
        V3XVECTOR b;
        if (NG_AILocateNearestEnemy(&g_pPlayer->Mat->v.Pos, &b, &oV, t_SPECIAL))
		{
			if (oV<5000)
			{
				sp = g_pspMsg->item + 1;
				GX.csp.TrspADD((GX.View.xmax-sp->LX)/2, GX.View.ymax-sp->LY*2, sp);
			}
		}
    }
	CSP_Color(g_SGGame.CI_WHITE);
    if ((g_cFXTable.Alarm)&(g_SGSettings.cursor))
    {
        sp = g_pspMsg->item + 0;
        GX.csp.TrspADD((GX.View.xmax-sp->LX)/2, 6, sp);
    }    
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_DrawOverlay(void)
*
* DESCRIPTION :
*
*/
static void NG_DrawHUD()
{
	 int dy = g_pspDispFont->item[0].LY+2, y=(g_pCurrentGame->ship!=0) ? (GX.View.ymax>>3) : GX.View.ymax - (GX.View.ymax>>2);

	 CSP_Color(g_SGGame.CI_WHITE);

    if ((g_SGSettings.Ctrl==CTRL_Mouse)&&(g_SGSettings.DemoMode<2)&&(g_SGGame.CameraMode<CAMERA_NAV))
		GX.csp.put(sMOU->x, sMOU->y, g_pspHud2->item + 0);

	NG_HudDisplayCamera();

    if (g_SGInput.isType)
    {
		char tex[256];
        sprintf(tex, ":%s%c", g_SGInput.Code, g_SGSettings.cursor ? '_' : ' ');
        CSP_WriteCenterText( tex, GX.View.ymax/3, g_pspCat );
    }

    if (g_SGSettings.ComTime==MAX_COM_DELAY)
    {
        if (g_SGObjects.FinCode==GAMESTATE_FAIL)
			g_SGObjects.FinCode=GAMESTATE_FAILED;

		if (g_pComFunction)
			sysConPrint("%s: %s", g_pComFunction->szTitle, g_pComFunction->Val[*(g_pComFunction->ptr)]);
    }

    if (g_SGSettings.ComFTime)
    {
        g_SGSettings.ComFTime--;
        CSP_WriteCenterText(g_pComFunction->szTitle, y+=dy, g_pspDispFont);
        CSP_WriteCenterText(g_pComFunction->Val[*(g_pComFunction->ptr)], y+=dy, g_pspDispFont);
    }

    if (g_SGSettings.ComTime)
    {
        char *orx = g_szCOM[g_SGSettings.ComNumber];
        g_SGSettings.ComTime--;
        if ((g_SGSettings.ComNumber==COM_Chat)&&(g_SGSettings.ComPlayer))
        {
			char tex[256];
            sprintf(tex, "(%s):%s", g_SGGame.PlayersName[g_SGSettings.ComPlayer-1], orx);
            CSP_Color(g_SGGame.pColorRadar[g_SGSettings.ComPlayer%5]);
            orx = tex;
        }
		CSP_Color(g_SGGame.CI_WHITE);
        CSP_WriteCenterText(orx, y+=dy, g_pspDispFont);        
    }
    else
    {
        if(g_SGObjects.FinCode==GAMESTATE_FAIL) 
           g_SGObjects.FinCode=GAMESTATE_FAILED;
    }

    if ((g_SGSettings.DemoMode)&&(g_SGSettings.cursor))
    {
        if (g_SGSettings.DemoMode>=2)
            CSP_WriteCenterText("Demo", y+=dy, g_pspDispFont);

        if (g_SGSettings.DemoMode==1)
        {
			char tex[256];
            sprintf(tex, "REC: %ld", (g_pRecordData.step*100L)/g_pRecordData.maxstep);
            CSP_WriteCenterText(tex, y+=dy, g_pspDispFont);
        }
    }
    if (g_SGObjects.IsTimed)
    {
        if (g_SGObjects.Time==10)
        {
            g_SGSettings.ComNumber=COM_TimeShort;g_SGSettings.ComTime=MAX_COM_DELAY;
        }
        if (timer_sec() - g_SGGame.oldTimer)
        {
            g_SGObjects.Time--;
            g_SGGame.oldTimer = timer_sec();
            if (g_SGObjects.Time==0)
            {
                g_SGSettings.ComNumber=COM_Failed;g_SGSettings.ComTime=MAX_COM_DELAY;
                g_SGObjects.FinCode=GAMESTATE_FAIL;
            }
        }
    }
    switch (g_SGGame.CameraMode) 
	{
        case 0:
        if (g_SGSettings.showInf&1)
        {
            if (g_SGGame.MaxAim[g_SGObjects.NAV]==0)
            {
                if (g_SGObjects.NAV == g_SGObjects.MaxNAV-1)
					CSP_WriteCenterText("Mission Complete", y+=dy, g_pspDispFont);
                else
					CSP_WriteCenterText(g_szGmT[39], y+=dy, g_pspDispFont);
            }
        }

		Display_HUD();

        if ((g_SGSettings.showInf&1)||((!(g_SGSettings.showInf&1))&&(g_SGGame.RadarMode)))
			Display_Radar();

        if (g_pPlayer->mode & TURBOMODE)
			CSP_WriteCenterText("Turbo", y+=dy, g_pspDispFont);

        break;
        default:
        if (g_SGSettings.cursor)
			CSP_WriteCenterText(g_szCamera[g_SGGame.CameraMode], y+=dy, g_pspDispFont);
        break;
    }
}

void NG_DrawFlash()
{
    if (g_SGGame.FlashAlpha)
    {
        if (g_SGGame.FlashAlpha>0)
            g_SGGame.FlashAlpha--;

        if (g_SGGame.FlashAlpha<0)
            g_SGGame.FlashAlpha++;

        GX.csp_cfg.alpha = min(255, abs(g_SGGame.FlashAlpha*8));
       	CSP_Color(RGB_PixelFormat(g_SGGame.FlashColor.r, g_SGGame.FlashColor.g, g_SGGame.FlashColor.b));
		if (V3X.Client->Capabilities&GXSPEC_HARDWARE)
		{
      		GX.gi.drawShadedRect(0, 0, GX.View.lWidth-1, GX.View.lHeight-1, NULL);
  		}
  		else
  		{
  			// TODO ?
  		}
    }
}

void NG_DrawOverlay(void)
{
   	NG_FXDraw();

    if (g_SGSettings.LensFX)
		NG_FXFlare();
		
    if (g_SGSettings.ticker)
		NG_DrawTicker();

    if (!(g_SGSettings.showInf&2))
	    NG_DrawHUD();

    if (g_SGGame.Shock)
    {
        V3X.ViewPort.center.x+=g_SGGame.Shock;
        g_SGGame.Shock=(-g_SGGame.Shock)>>1;
    }

    NG_DrawFlash();

    return;
}

static void NG_UpdateAlarmAndLock(void)
{
    if (g_pPlayer->J.pInf.Shield<(g_pPlayer->J.pInf.ShieldMax/4)) // /2
    {
        if (!g_cFXTable.Alarm)
        {
            g_cFXTable.Alarm = NG_AudioPlaySound(g_cFXTable.SoundAlarm, 0);
            g_cFXTable.fAlarmTime = 255;
        }
        else
        {
            if (g_cFXTable.fAlarmTime>0)
            {
                g_cFXTable.fAlarmTime-=g_cTimer.fCounter;
                if (g_cFXTable.fAlarmTime<=0)
                {
                    NG_AudioStopSound(g_cFXTable.Alarm);
                }
            }
        }
    }
    else
    {
        if (g_cFXTable.Alarm)
        {
            NG_AudioStopSound(g_cFXTable.Alarm);
            g_cFXTable.Alarm = 0;
        }
    }
    if (g_SGLockMode.dmode&256)
    {
        NG_AudioBeep(3);
        g_SGLockMode.dmode&=~256;
    }

}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_GamePlay(void)
*
* DESCRIPTION :
*
*/
void NG_GamePlay(void)
{
    NG_ResetKey();

    V3XScene_Viewport_Build(g_SGGame.Scene, &GX.View);

	g_cGameStat.time_start = timer_sec();
    V3X.Time.ms = g_nOldTimer = timer_ms();
  
    timer_Update(&g_cTimer);

    g_SGGame.FlashAlpha=32;
    g_SGGame.FlashColor.r=0;
    g_SGGame.FlashColor.g=0;
    g_SGGame.FlashColor.b=0;
    
    sysConPrint(g_pGameItem->EI[g_pCurrentGame->episode].LI[g_pCurrentGame->level[g_pCurrentGame->episode]].name);

	do
    {
		if (STUB_TaskControl())
			break;

        // LOCK
		GX.Client->Lock();
        g_SGSettings.bClearView = 0;
        
        // Render scene
        V3XScene_Viewport_Clear(g_SGGame.Scene);
        V3XScene_Viewport_Render(g_SGGame.Scene);

		NG_DrawOverlay();
    	if (sysConIsActive())
          	sysConRender();
		// UNLOCK
		GX.Client->Unlock();

        // Process pass
        NG_ControlGame();
        NG_UpdateCamera();
		NG_FXUpdate();
        NG_FXUpdateEx();
        NG_UpdatePowerUps();
        NG_LevelUpdate();
        NG_UpdateLights();
        NG_UpdateAlarmAndLock();
		NG_UpdateCom();
        V3XScene_Viewport_Build(g_SGGame.Scene, &GX.View);
        V3XViewport_Setup(&V3X.Camera, GX.View);
        NG_WeaponUpdate();
        if (V3XA.State & 1)
            V3XA.Client->Render();

        switch(g_SGObjects.FinCode) 
		{
            case GAMESTATE_DEAD:
            if ((g_SGSettings.SerialGame)&&(g_SGObjects.quit))
            {
                NG_AudioStopSound(g_cFXTable.Alarm );
                NG_InitGameShip();
                if (g_SGObjects.NavCam)
					V3XVector_Cpy(g_pPlayer->Rot->pos, g_SGObjects.NavCam->Tk->vinfo.pos);
                g_pPlayer->Notify += SGNET_HASRESPAWN|SGNET_HASMOVED|SGNET_SHIELDCHANGE;
                g_SGObjects.FinCode = 0;
                g_SGObjects.quit = 0;
                g_SGGame.FlashAlpha = -32;
                g_SGGame.CameraMode = 0;
                g_pPlayer->J.pInf.Shield = g_pPlayer->J.pInf.ShieldMax;
                g_pPlayer->J.pInf.ShootOk = 0;
                g_cGameStat.frag-=1;
            }
            break;
            case GAMESTATE_ABORT:
            case GAMESTATE_RETRY:
            case GAMESTATE_FAILED:
            case GAMESTATE_WON:
				g_SGObjects.quit=1;
            break;
        }

        if (g_SGObjects.FinCode==GAMESTATE_QUIT)
        {
            if(NG_QuitGame())
				g_SGObjects.quit = 1;
        }

        if (g_SGObjects.FinCode==GAMESTATE_HELP)
        {
            if (g_SGSettings.DemoMode) 
				g_SGObjects.quit=1;
            else
				ShowHelp();
            g_SGObjects.FinCode = 0;
        }

		if (!g_SGObjects.quit)
		{
			if (g_SGSettings.SerialGame)
			{
				NG_NetTranslate(g_pPlayer, NULL);
				NG_NetDispatch();
			}
			if (g_SGSettings.DemoMode)
				NG_RecordReplayAction();
			else  
				g_pPlayer->Mx.but = 0;

			
			V3X.Time.ms = timer_ms();

			g_SGGame.Count++;
			g_SGSettings.cursor = ((g_SGGame.Count&15) == 0) ? 2 : ((g_SGGame.Count&15)<8);

			GX.View.Flip();
		}

    }while(g_SGObjects.quit==0);

	if (g_cFXTable.Engine<255)
		NG_AudioStopSound(g_cFXTable.Engine);
    timer_Stop(&g_cTimer);
    g_cGameStat.time_end = timer_sec();

    if ((g_SGObjects.FinCode==GAMESTATE_WON)&&(!g_SGSettings.DemoMode))
    {
        g_pCurrentGame->score = g_pPlayer->J.pInf.Scoring;
        g_pPlayer->Notify+=SGNET_HASWON;
#if (SGTARGET != NG_DEMO_VERSION)
        NG_AudioPlayWarp();
#endif
    }
    else
    {
        if (g_SGObjects.FinCode==GAMESTATE_DEAD)
            NG_WaitForKeyWithDelay(2);

        g_pPlayer->Notify+=SGNET_HASLEFT;
    }

    if (g_SGSettings.DemoMode)
		g_SGObjects.FinCode=GAMESTATE_DEAD;

    if (g_SGSettings.SerialGame)
    {
		sysConPrint("Quit session");
        NG_NetTranslate(g_pPlayer, NULL);
        if (!g_SGGame.IsHost)
			sNET->QuitSession();
        else
			sNET->CloseSession();
    }
    return;
}
