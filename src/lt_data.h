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

#include "systime.h"
// VARS
#ifdef __cplusplus
extern "C" 
{
#endif

    extern SGMenuPosition g_SGMenuPos;
    extern SGGameStruct g_SGGame;
    extern SGHudDisplay g_SGHudNormal, *g_SGHudCurrent;
	extern GXSPRITE g_csPicture;
    extern GXSPRITEGROUP *g_pspHud, *g_pspCat, *g_pspHud2, *g_pFont, *g_pSmallFont;
    extern GXSPRITEGROUP *g_pspFX[32], *g_pFontMenuLrg, *g_pFontMenuSml;
    extern GXSPRITEGROUP *g_pspFlares, *g_pspFlares2, *g_pspDispFont, *g_pspMsg;
    extern char **g_pszAnimList;
    extern V3XKEYCAMERA *g_pCamera;
    extern u_int8_t g_ubSampleUsed[32];
    extern SGFXAudioTable g_cFXTable;
    extern V3XMATRIX g_mtxMissile;
    extern SGPlayer g_pPlayers[2], *g_pPlayer;
    extern SGMISSION g_SGObjects;
    extern RW_Interface *g_pGameBoard;
    extern SGStarfield Starfield;
    extern SGRecordReplay g_pRecordData;
    extern V3XOVI *g_pLockTarget;
    extern SGScript *g_pPlayerInfo;
    extern SGGameItem *g_pGameItem;
	extern GXDISPLAYMODEINFO *g_pDisplayMode;
    extern SGGameStat g_cGameStat;
    extern SGPlayerSave g_pSaveGames[10], g_pBestGames[10], *g_pCurrentGame;
    extern SYS_WAD *g_pWadVoice;
    extern SYS_FILEIO *g_pGameIO;    
    extern SGMusic g_pMusicInfo[32];
    extern SGAI g_cAI;
	extern char g_szGmT[MAX_TEXT_MENUS][32];
    extern char *g_szMenuSessions[16];
	extern SYS_TIMER g_cTimer;

    extern char *g_szWeaponNames[], *g_szPowerUpNames[], *g_szCOM[], *g_szOrder[], *g_szCamera[];
    extern char *g_szCOM[];
    extern SGShip g_pShip[3], g_cShip;
    extern SGMenu g_pMenuVideo[];
	extern LockInfo_Struct g_SGLockMode;

#ifdef __cplusplus
}
#endif
