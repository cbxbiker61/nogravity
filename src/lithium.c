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
#include "sysini.h"
#include "sysctrl.h"
#include "gx_struc.h"
#include "gx_init.h"
#include "gx_tools.h"
#include "gx_rgb.h"
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
#include "gui_os.h"
#include "iss_defs.h"
#include "iss_fx.h"
//
#include "lt_struc.h"
#include "lt_data.h"
#include "lt_func.h"

#define APP_NAME "No Gravity"
#define STUB_SIG "application/x-vnd.Realtech-nogravity"

static char *Pname=APP_NAME, *Psig=STUB_SIG;
static u_int8_t *g_HeapBuffer;
static u_int32_t g_HeapSize = 0x800000;
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_CleanUp(void)
*
* Description :  
*
*/
void NG_CleanUp(void)
{
    NG_NetRelease();
    if (g_HeapBuffer) 
		free(g_HeapBuffer);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_SetGameInfo(void)
*
* DESCRIPTION :
*
*/
void NG_SetGameInfo(void)
{
    g_SGSettings.Menu = 1;
    g_SGSettings.RecTime = 15;
    g_SGSettings.DemoDelay = 30;
	g_SGSettings.VerticalSync = 1;
    g_SGSettings.WorldUnit = 1;
    g_SGSettings.showInf = 1;

	SETBITFIELD(g_SGSettings.TexFiltering, V3X.Client->Capabilities, GXSPEC_ENABLEFILTERING);

	V3X.Client->Capabilities|=GXSPEC_RGBLIGHTING;
	V3X.Client->Capabilities|=GXSPEC_ENABLEDITHERING;
	V3X.Setup.flags|=V3XOPTION_TRUECOLOR;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void NG_CheckSystems(void)
{
    GX.View.Flags|=GX_CAPS_VSYNC;
    switch(RLX.Control.Id){
        case RLXCTRL_Keyboard:
        g_SGSettings.Ctrl=CTRL_Keyb;
        break;
        case RLXCTRL_Mouse:
        break;
        case RLXCTRL_JoyAnalog:
        case RLXCTRL_JoyPad:
        case RLXCTRL_ThrustMaster:
        case RLXCTRL_SWIFT:
        case RLXCTRL_VirtualIO:
        break;
    }
    switch(RLX.V3X.Id) {
        case RLX3D_DIRECT3D:
        g_SGSettings.TexFiltering = 1;
        break;
        case RLX3D_3DFX:
        case RLX3D_OPENGL:
        g_SGSettings.TexFiltering = 1;
        g_SGSettings.TexFiltering = 1;
        case RLX3D_S3VIRGE:
        g_SGSettings.TexFiltering = 1;
        break;
    }
	if ((g_SGSettings.Ctrl >= CTRL_Joystick) && (!sJOY->numControllers))
		g_SGSettings.Ctrl = CTRL_Mouse;
	if ((g_SGSettings.Ctrl == CTRL_Mouse) && (!sMOU->numControllers))
		g_SGSettings.Ctrl = 0;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/

static 
int OnStealth(char *parms)
{
	if (!g_SGSettings.SerialGame)
	{
		g_pPlayer->mode^=STEALTHMODE;
		sysConPrint("stealth mode is %s",g_pPlayer->mode&STEALTHMODE ? "on" : "off");
	}
	UNUSED(parms);
	return 0;
}

static 
int OnGod(char *parms)
{
	if (!g_SGSettings.SerialGame)
	{
		g_pPlayer->mode^=GODMODE;
		sysConPrint("god mode is %s",g_pPlayer->mode&GODMODE ? "on" : "off");
	}
	UNUSED(parms);
	return 0;
}

static 
int OnTicker(char *parms)
{
	g_SGSettings.ticker^=1;
	sysConPrint("ticker is %s", g_SGSettings.ticker ? "on" : "off");
	UNUSED(parms);
	return 0;
}

static 
int OnCheat(char *parms)
{
	g_SGSettings.Cheater = 1 - g_SGSettings.Cheater; 
	NG_AudioPlaySound(NG_AudioGetByName("bonus")-1, 0);	
	sysConPrint("cheat mode is %s", g_SGSettings.Cheater ? "on" : "off");
	UNUSED(parms);
	return 0;
}

static 
int OnWin(char *parms)
{
	g_SGObjects.FinCode=GAMESTATE_WON;
	NG_AudioPlaySound(NG_AudioGetByName("bonus")-1, 0);	
	sysConPrint("Win");
	UNUSED(parms);
	return 0;
}

static 
int OnLose(char *parms)
{
	g_SGObjects.FinCode=GAMESTATE_FAILED;
	NG_AudioPlaySound(NG_AudioGetByName("bonus")-1, 0);	
	sysConPrint("Lose");
	UNUSED(parms);
	return 0;
}

void STUB_MainCode(void)
{
    int32_t MenuChoice=1;
	if (!FIO_wad)
		return;

    FIO_cur = &FIO_res;
    filewad_chdir(FIO_wad,"");
	V3XPlugIn_Add(1, V3XPlug_CollisionMove);

    // Game Data
#if defined  _DEBUG || defined __BEOS__
	RLX.Video.Config|=RLXVIDEO_Windowed;
#else
 	RLX.Video.Config&=~RLXVIDEO_Windowed;
#endif

#ifdef _DEBUG
    SYS_Debug("Create display mode list ...\n");
#endif

	NG_CreateDisplayList();
	{
		int mode = GX.Client->SearchDisplayMode(g_SGSettings.ResolutionX, g_SGSettings.ResolutionY, g_SGSettings.ColorDepth);
		GX.Client->GetDisplayInfo(mode);
		NG_ChangeScreenMode(mode);	
	}

	g_HeapBuffer = (u_int8_t*) malloc(g_HeapSize);
	if (!g_HeapBuffer)
		return;
	SYS_ASSERT(g_HeapBuffer);

#ifdef _DEBUG
    SYS_Debug("Allocate %x ...\n", g_HeapSize);
#endif

    MM_heap.heapalloc(g_HeapBuffer, g_HeapSize); 
    MM_heap.active = 0;

#ifdef _DEBUG
    SYS_Debug("Init console ...\n");
#endif

	// System
	sysConCreate();
	sysConPrint("Welcome to No Gravity.");
	sysConBindCmd("/cheat", OnCheat);
	sysConBindCmd("/lose", OnLose);
	sysConBindCmd("/win", OnWin);
	sysConBindCmd("/ticker", OnTicker);
	sysConBindCmd("god", OnGod);
	sysConBindCmd("stealth", OnStealth);

	sysConPrint("System:");
	   
	sysConPrint( "3D driver : ~%s~", V3X.Client->s_DrvName);
    sysConPrint( "Display driver : ~%s~.", GX.Client->s_DrvName);
    if ((V3XA.State & 1))
		sysConPrint("Sound driver : ~%s~.", V3XA.Client->s_DrvName);
    else
		sysConPrint("No sound.");

#ifdef _DEBUG
    SYS_Debug("Load preferences, languages and save games ...\n");
#endif

	NG_CheckSystems();
    NG_ReadLanguagePack();
    NG_RosterLoad();
    NG_HighScoresLoad();
    
	// Data
    NG_LoadGameData();
	V3X.ViewPort.minTextureVisibleRadius = 6.f/200.f;
	V3X.ViewPort.minVisibleRadius = 12.f/300.f;
    	
    MM_heap.reset();

    if (V3XA.State & 1)
    {
	    float doppler = 0.1f;
        float distance = 1.f;
        float rolloff = 0.0f;
        V3XA_REVERBPROPERTIES cfg;

#ifdef _DEBUG
	   SYS_Debug("Configure audio...\n");
#endif

		V3XA.Client->ChannelOpen(0, 16);
        cfg.environment = V3XA_ENVIRONMENT_ARENA;
        cfg.fVolume = 0.361f;
        cfg.fDecayTime_sec = 7.284f;
        cfg.fDamping  = 0.332f;
        V3XA.Client->ChannelSetEnvironment(-1, &cfg);
	    V3XA.Client->UserSetParms(NULL, NULL, &distance, &doppler, &rolloff);
    }

#ifdef _DEBUG
    SYS_Debug("Load network preferences...\n");
#endif
    NG_NetInitialize();
    
#ifndef _DEBUG
	NG_PlayPresentsGame();
#endif
    
    MM_heap.reset();    

#ifdef _DEBUG
    SYS_Debug("Run game loop...\n");
#endif

    do
    {
		if (STUB_TaskControl())
			break;
        
        if (g_SGSettings.Menu)
        {
            MM_heap.reset();
            FIO_cur = g_pGameIO;
            MenuChoice = NG_MainMenu();
            MM_heap.reset();

			if (STUB_TaskControl())
				break;
        }
#ifdef _DEBUG
	    SYS_Debug("Menu choice is %d...\n", MenuChoice);
#endif
        if (MenuChoice==4)
        {

        }
        else
        if (MenuChoice==3)
        {
            NG_MenuCredits();
        }
        else
        if (MenuChoice==1)
        {
            do
            {
                NG_GameStart();
                NG_GamePlay();
                NG_GameStop();
                MM_heap.reset();
				MenuChoice = 0;
                if (!g_SGSettings.DemoMode)
                {
                    switch(g_SGObjects.FinCode) 
					{
                        case GAMESTATE_WON:
							NG_EndLevel();
							MenuChoice=0;						
                        break;
                        case GAMESTATE_ABORT:
							MenuChoice=0;
                        break;
                        case GAMESTATE_RETRY:
							MenuChoice=1;
                        break;
						case GAMESTATE_QUIT:
							goto end;
						break;

                        default:
							if (!STUB_TaskControl())
							{
#if (SGTARGET ==NG_DEMO_VERSION)
								g_SGSettings.Menu=1;
#else
								NG_PlayGameOver();
								MenuChoice = g_SGSettings.Menu;
								g_SGSettings.Menu=1;
							}
#endif
                        break;
                    }
                }
                else
                {
#if (SGTARGET != NG_DEMO_VERSION)
                    if  (g_SGSettings.DemoMode==2)
                    {
                        if (g_SGGame.Demo==3) 
							NG_MenuCredits();

                        if (g_SGGame.Demo==0) 
							NG_PlayPresentsGame();
                    }
#endif
                    MenuChoice =0;
                }
                MM_heap.reset();
            }while(MenuChoice!=0);
            MenuChoice=1;
        } else MenuChoice=0;
    }while(MenuChoice!=0);

end:
#ifdef _DEBUG
    SYS_Debug("Exiting game loop...\n");
#endif
    MM_heap.reset();
    NG_PlayEndGame();
    MM_heap.reset();
	NG_ReleaseGameData();
    filewad_close(FIO_wad);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void STUB_Default(void)
*
* DESCRIPTION :
*
*/
void STUB_Default(void)
{
    RLX.Dev.ApplicationName = Pname;
    RLX.Dev.TeamSignature = Psig;
    RLX.App.Heap_Required = 0x580000;
    NG_ReadGameConfig();

	V3X.Buffer.MaxLight = 16;
    V3X.Buffer.MaxFacesDisplay = 5000;
    V3X.Buffer.MaxPointsPerMesh = 700;
    V3X.Buffer.MaxClippedFaces = 1280;
    V3X.Ln.maxLines = 1024;

	RLX.Audio.Config|=RLXAUDIO_Enable;
    RLX.Audio.Config|=RLXAUDIO_UseHwMixer;
    RLX.Audio.Config&=~RLXAUDIO_Use3D;
	V3XA.State|=1;

    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void STUB_ReadyToRun(void)
*
* DESCRIPTION :
*
*/

void STUB_ReadyToRun(void)
{
	char *resFile = "NOGRAVITY.RMX";
	
#ifdef _DEBUG
    SYS_Debug("Open file resource : %s\n", resFile);
#endif
	FIO_wad = filewad_open(resFile, 0);
	if (!FIO_wad)
	{
		SYS_Error("Couldn't find NOGRAVITY.RMX.\n\nGo to http://www.realtech-vr.com/nogravity/ \nand get the No Gravity Game Data package.");
		return ;
	}
	SYS_ASSERT(FIO_wad);

#if (SGTARGET ==NG_DEMO_VERSION)
    FIO_wad->mode |= SYS_WAD_STATUS_ENABLED;
#else
    FIO_wad->mode |= SYS_WAD_STATUS_ENABLED;
#endif

    g_pGameIO = &FIO_res;
    FIO_cur = g_pGameIO;
	NG_SetGameInfo();
	
    V3X.Setup.flags|=V3XOPTION_COLLISION;
    V3X.Setup.MaxExtentableObjet = 200;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void STUB_QuitRequested(void)
*
* DESCRIPTION :
*
*/
void STUB_QuitRequested(void)
{
	if (!FIO_wad)
		return;
    NG_HighScoresSave();
	NG_CleanUp();
    return;
}

GXDISPLAYMODEINFO *g_pDisplayMode;

static int EnumDisplayMode(char **ModeList)
{
	GXDISPLAYMODEINFO *p;
    
	int colorDepth = g_SGSettings.ColorDepth;
	int m_nValue = g_SGSettings.DisplayIndex;

    g_pDisplayMode = p = GX.Client->EnumDisplayList(g_SGSettings.ColorDepth);	
	
	if (g_pDisplayMode)
	{
		struct _gx_display_mode_info *current = g_pDisplayMode;		
		int n = 0;
		SYS_ASSERT(g_pDisplayMode);

		while (current->BitsPerPixel)
		{
			current ++;
			n++;		
		}
		
		if (n)	
		{
			struct _gx_display_mode_info *filtered = (struct _gx_display_mode_info*) MM_heap.malloc((n+1) * sizeof(struct _gx_display_mode_info));
			struct _gx_display_mode_info *filter = filtered;			

			int pref = -1;
			n = 0;
			current = g_pDisplayMode;
			while (current->BitsPerPixel)
			{
				if (!((current->BitsPerPixel <= 8) || (current->lWidth<640) || (current->lHeight<480)))
				{
					struct _gx_display_mode_info *f = filtered;
					while (f<filter)
					{
						if ((f->BitsPerPixel == current->BitsPerPixel)
							&& (f->lWidth == current->lWidth)
							&& (f->lHeight == current->lHeight))
						{
							break;
						}
						f++;
					}

					if (f==filter)
					{
						*filter = *current;
						
						if ((current->lWidth==g_SGSettings.ResolutionX)&&
							(current->lHeight==g_SGSettings.ResolutionY)&&(current->BitsPerPixel==colorDepth))
							pref = n;
						
						filter++;
						n++;
					}
				}
				current ++;
			}

			*filter = *current;
			MM_heap.free(g_pDisplayMode);
			g_pDisplayMode = filtered;
			g_SGSettings.DisplayIndex = m_nValue = pref;
		}
		{
			int i = 0;
			while(g_pDisplayMode[i].BitsPerPixel)
			{
				ModeList[i] = (char*)malloc(32);
				sprintf(ModeList[i], "%d x %d (%d)",
					g_pDisplayMode[i].lWidth,
					g_pDisplayMode[i].lHeight,
					g_pDisplayMode[i].BitsPerPixel

				);
				i++;
			}		

			return i;
		}
    }
	return 0;
	
}

void NG_CreateDisplayList(void)
{
    SGMenu *pMenu = g_pMenuVideo + 0;
    pMenu->max = EnumDisplayMode(pMenu->Val);
    pMenu->Val[pMenu->max] = NULL;
    pMenu->max--;
    return;
}

void NG_ChangeScreenMode(int mode)
{
	static int nCurrentMode;
	static int n3DSystem;
	int is3D = !!(GX.View.Flags&GX_CAPS_3DSYSTEM);
		
	if (RLX.Video.Config&RLXVIDEO_Windowed)
		mode = -1;

    if (g_SGSettings.Multisampling)
    {
	   GX.View.Multisampling = 1<<g_SGSettings.Multisampling;
	   GX.View.Flags|=GX_CAPS_MULTISAMPLING;
	   sysConPrint("Multisampling : %d", GX.View.Multisampling);
    }

	
	if (!(V3X.Client->Capabilities&GXSPEC_FULLHWSPRITE))
	{
		if (n3DSystem!=is3D)
		{
			n3DSystem = is3D;;
			nCurrentMode = -2;
		}
	}

    if ((mode!=nCurrentMode)||(!GX.Surfaces.maxSurface))
    {
		if (GX.Surfaces.maxSurface)
			GX.Client->ReleaseSurfaces();
        GX.Client->RegisterMode(mode);
        GX.Client->CreateSurface(2);

    }
    else
    {
        GX.Client->GetDisplayInfo(mode);
    }
	
	if (GX.View.BitsPerPixel<=8) 
		PAL_Black();

	V3XKernel_RenderClass();
    nCurrentMode = mode;
	sMOU->Hide();

    return;
}
