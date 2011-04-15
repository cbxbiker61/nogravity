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
#include "sysctrl.h"
#include "systools.h"
#include "gx_init.h"
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

#include "gui_os.h"
#include "iss_defs.h"
#include "iss_fx.h"
//
#include "lt_struc.h"
#include "lt_data.h"
#include "lt_func.h"

char *g_szWeaponNames[]={"", g_szGmT[1], g_szGmT[2], g_szGmT[3], g_szGmT[4], g_szGmT[5], g_szGmT[6], g_szGmT[6]};
char *g_szPowerUpNames[]={" ", g_szGmT[7], g_szGmT[8], g_szGmT[9], g_szGmT[10], g_szGmT[11], g_szGmT[12], NULL};
char *g_szCOM[]={
    " ",
    g_szGmT[15],
    g_szGmT[16],
    g_szGmT[17],
    g_szGmT[18],
    g_szGmT[19],
    g_szGmT[20],
    g_szGmT[21],
    g_szGmT[117],
    g_szGmT[23],
    g_szGmT[110],
    g_szGmT[131],
    g_szGmT[132],
    g_szGmT[133],
    g_szGmT[134],
    g_szGmT[7],
    g_szGmT[8],
    g_szGmT[9],
    g_szGmT[10],
    g_szGmT[11],
    g_szGmT[12],
    g_szGmT[1],
    g_szGmT[2],
    g_szGmT[3],
    g_szGmT[4],
    g_szGmT[5],
    g_szGmT[6],
    g_szGmT[135],
    g_szGmT[136],
    g_szGmT[137],
    g_szGmT[138],
    g_szGmT[139],
    g_szGmT[140],
    g_szGmT[141],
NULL};

char *g_szOrder[]={
    g_szGmT[134],
    g_szGmT[135],
    g_szGmT[136],
    g_szGmT[137],
    g_szGmT[138],
    g_szGmT[139],
    g_szGmT[140],
    g_szGmT[141],
NULL};

char *g_szCamera[]=
{
    " ",
    g_szGmT[32],
    g_szGmT[29],
    g_szGmT[31],
    g_szGmT[30],
    g_szGmT[33],
    g_szGmT[143],
    g_szGmT[33],
    g_szGmT[34],
    g_szGmT[34],
    g_szGmT[36],
    g_szGmT[36],
    g_szGmT[35],
" "};

SGShip g_cShip, g_pShip[3]=
{
    {
        1, 6, "viper", 0,
        {
            {50, 60, 15, 0},
            {40, 60, 31, 0},
            {30, 60, 63, 0},
            { 0, 30, 0, 10},
            {30, 60, 0, 0},
            { 0, 30, 0, 40}
        }
    }
   ,
    {
        1, 6, "raiden", 1,
        {
            {30, 60, 63, 0},
            {50, 60, 31, 0},
            {40, 60, 31, 0},
            { 5, 30, 0, 30},
            {30, 60, 0, 0},
            {20, 30, 0, 80}
        }
    }
   ,
    {
        2, 6, "raptor", 2,
        {
            {20, 60, 0, 0},
            {30, 60, 0, 0},
            {50, 60, 63, 0},
            {30, 30, 0, 60},
            {30, 60, 0, 0},
            {40, 40, 0, 100}
        }
    }
};

     SGMenuPosition g_SGMenuPos;
     SGGameStruct g_SGGame;
     SGHudDisplay g_SGHudNormal, *g_SGHudCurrent;
	 GXSPRITE g_csPicture;
     GXSPRITEGROUP *g_pspHud, *g_pspCat, *g_pspHud2, *g_pFont, *g_pSmallFont;
     GXSPRITEGROUP *g_pspFX[32], *g_pFontMenuLrg, *g_pFontMenuSml;
     GXSPRITEGROUP *g_pspFlares, *g_pspFlares2, *g_pspDispFont, *g_pspMsg;
     char **g_pszAnimList;
     V3XKEYCAMERA *g_pCamera;
     uint8_t g_ubSampleUsed[32];
     SGFXAudioTable g_cFXTable;
     V3XMATRIX g_mtxMissile;
     SGPlayer g_pPlayers[2], *g_pPlayer;
     SGMISSION g_SGObjects;
     RW_Interface *g_pGameBoard;
     SGStarfield Starfield;
     SGRecordReplay g_pRecordData;
     V3XOVI *g_pLockTarget;
     SGScript *g_pPlayerInfo;
     SGGameItem *g_pGameItem;
     SGGameStat g_cGameStat;
     SGPlayerSave g_pSaveGames[MAX_SAVE_GAMES], g_pBestGames[MAX_SAVE_GAMES], *g_pCurrentGame;
     SYS_WAD *g_pWadVoice;
     SYS_FILEIO *g_pGameIO;
     SGMusic g_pMusicInfo[32];
     SGAI g_cAI;
	 char g_szGmT[MAX_TEXT_MENUS][32];
     char *g_szMenuSessions[16];
	 SYS_TIMER g_cTimer;

