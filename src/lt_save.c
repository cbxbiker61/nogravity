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
#include <time.h>
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

void NG_GetHscFilename(char *tex)
{
	sprintf(tex, "%s/nogravity.hsc", RLX.IniPath);
}

void NG_GetPtsFilename(char *tex)
{
	sprintf(tex, "%s/nogravity.pts", RLX.IniPath);
}

void NG_ReadLevelInfo(SGLevelItem *LI, ConfigFile *iniFile)
{
    int j;
    sysStrnCpy(LI->name, GetCF_str2("Name", iniFile), 30);
    for (j=0;j<15;j++)
    {
        char tx2[16];
        char *s;
        sprintf(tx2, "Line%d", j+1);
        s = GetCF_str(tx2, iniFile);
        if (s) sysStrnCpy(LI->tex[j], s, 30);
        else LI->tex[j][0]=0;
    }
    LI->WeaponMax=1;
    LI->Bonus = (uint16_t)GetCF_long("Bonus", iniFile);
    LI->Time = (uint16_t)GetCF_long("Time", iniFile);
    LI->Level = (uint16_t)GetCF_long("Level", iniFile);
}

void NG_ReadMissionData(char *name, SGEpisodeItem *EP)
{
    ConfigFile iniFile;
    if ( ReadConfig(name, &iniFile) == 0 )
    {
        int i;
        SGLevelItem *LI = EP->LI;
        for (i=0;i<10;i++, LI++)
        {
            char tx[16];
            sprintf(tx, "level_%d", i+1);
            if(SelectConfigClass(tx, &iniFile))
            {
                NG_ReadLevelInfo(LI, &iniFile);
                EP->numLevel = i+1;
            }
        }
        DestroyConfig(&iniFile);// sinon ca pine
    }
}

void NG_ReleaseMissionList()
{
	int k;
	for (k=0;k<MAX_EPISODE;k++)
    {
        SGEpisodeItem *EP = g_pGameItem->EI+k;
		MM_heap.free(EP->LI);
	}
	MM_heap.free(g_pGameItem->EI);
	MM_heap.free(g_pGameItem);
}

void NG_ReadMissionList(void)
{
    int k, pe=0;
	char tex[256];
    g_pGameItem = (SGGameItem*)MM_heap.malloc(sizeof(SGGameItem));
    g_pGameItem->EI = (SGEpisodeItem*)MM_heap.malloc((MAX_EPISODE+1) * sizeof(SGEpisodeItem));
	g_pGameItem->numEpisode = MAX_EPISODE;

	for (k=0;k<MAX_EPISODE;k++)
    {
        SGEpisodeItem *EP = g_pGameItem->EI+k;
        EP->LI = MM_CALLOC(10, SGLevelItem);
#ifdef LT_TRANSLATE
        sprintf(tex, ".\\e_level%d.cmx", k+1);
        if (file_exists(tex))
        {
            pe=1;
        }
        else
#endif
        sprintf(tex, ".\\cmx\\%c_level%d.cmx", g_SGSettings.LangCode, k+1);
        if (pe)
			FIO_cur=&FIO_std;
        NG_ReadMissionData(tex, EP);
        if (pe)
			FIO_cur=g_pGameIO;
    }
}

void NG_HighScoresReset(void)
{
    int i, j;
	sysConPrint("High scores reset");
    for (i=0;i<10;i++)
    {
        sprintf(g_pBestGames[i].name, "No Gravity");
        g_pBestGames[i].score=10000L*11-i*10000L;
        for (j=0;j<MAX_SAVE_GAMES;j++)
			g_pBestGames[i].level[j]=0;
        g_pBestGames[i].episode=0;
		g_pBestGames[i].last_time = time(0);

    }
}

void NG_HighScoresSave(void)
{
	SYS_FILEHANDLE in;
	char tex[256];
    int mode = FIO_wad->mode;
    FIO_wad->mode &= SYS_WAD_STATUS_ENABLED;
    NG_GetHscFilename(tex);
    in = FIO_std.fopen(tex, "wb");
    if (in)
    {
		int ver = sizeof(SGPlayerSave);
#ifdef __BIG_ENDIAN__
		BSWAP32((uint32_t*)&ver, 1);
#endif
		FIO_std.fwrite(&ver, sizeof(int), 1, in);
#ifdef __BIG_ENDIAN__
        int i;
        for (i=0;i<10;i++)
        {
            BSWAP32((uint32_t*)&g_pBestGames[i].episode, 5);
            BSWAP16((uint16_t*)&g_pBestGames[i].active, 2);
        }
#endif
        FIO_std.fwrite( g_pBestGames, sizeof(SGPlayerSave), MAX_SAVE_GAMES, in);
#ifdef __BIG_ENDIAN__
        for (i=0;i<10;i++)
        {
            BSWAP32((uint32_t*)&g_pBestGames[i].episode, 5);
            BSWAP16((uint16_t*)&g_pBestGames[i].active, 2);
        }
#endif
        FIO_std.fclose(in);
    }
    FIO_wad->mode = mode;
}

void NG_HighScoresLoad(void)
{
    SYS_FILEHANDLE in;
	char tex[256];
    int mode = FIO_wad->mode;
    FIO_wad->mode &= ~SYS_WAD_STATUS_ENABLED;
    NG_GetHscFilename(tex);
    in = FIO_std.fopen(tex, "rb");
    if (in)
    {
		int ver;
		FIO_std.fread(&ver, 1, sizeof(int), in);
#ifdef __BIG_ENDIAN__
		BSWAP32((uint32_t*)&ver, 1);
#endif
		if (ver!=sizeof(SGPlayerSave))
		{
			sysConPrint("%s wrong version", tex);
			NG_HighScoresReset();
			NG_HighScoresSave();
		}
		else
		{
			FIO_std.fread( g_pBestGames, sizeof(SGPlayerSave), MAX_SAVE_GAMES, in);
#ifdef __BIG_ENDIAN__
			{
				int i;
				for (i=0;i<10;i++)
				{
					BSWAP32((uint32_t*)&g_pBestGames[i].episode, 5);
					BSWAP16((uint16_t*)&g_pBestGames[i].active, 2);
				}
			}
#endif
		}
        FIO_std.fclose(in);
    }
    else
    {
        NG_HighScoresReset();
        NG_HighScoresSave();
		sysConPrint("%s not found", tex);

    }
    FIO_wad->mode = mode;
}

void NG_HighScoresUpdate(void)
{
    int i, j, f=0;
    for (i=0;(i<MAX_SAVE_GAMES)&&(f==0);i++)
    {
        if (g_pCurrentGame->score>g_pBestGames[i].score)
        {
            for (j=MAX_SAVE_GAMES-1;j>i;j--)
				g_pBestGames[j]=g_pBestGames[j-1];
            g_pBestGames[i]=g_pSaveGames[g_SGSettings.player];
            f=1;
        }
    }
}
//

void NG_RosterSaveSlot(int i)
{
    sysMemZero(g_pSaveGames+i, sizeof(SGPlayerSave));
    sprintf(g_pSaveGames[i].name, "No Gravity");
    g_pSaveGames[i].ship = 1;
}

void NG_RosterSave(void)
{
#ifndef RLX_IOREADONLY
	char tex[256];
    SYS_FILEHANDLE in;
    int mode = FIO_wad->mode;
    FIO_wad->mode &= ~SYS_WAD_STATUS_ENABLED;
    NG_GetPtsFilename(tex);
    in = FIO_std.fopen(tex, "wb");
    if (in)
    {
		int ver = sizeof(SGPlayerSave);
#ifdef __BIG_ENDIAN__
		BSWAP32((uint32_t*)&ver, 1);
#endif
		FIO_std.fwrite(&ver, 1, sizeof(int), in);

#ifdef __BIG_ENDIAN__
        int i;
        for (i=0;i<MAX_SAVE_GAMES;i++)
        {
            BSWAP32((uint32_t*)&g_pSaveGames[i].episode, 5);
            BSWAP16((uint16_t*)&g_pSaveGames[i].active, 2);
        }
#endif
        FIO_std.fwrite( g_pSaveGames, sizeof(SGPlayerSave), MAX_SAVE_GAMES, in);
#ifdef __BIG_ENDIAN__
        for (i=0;i<MAX_SAVE_GAMES;i++)
        {
			BSWAP32((uint32_t*)&g_pSaveGames[i].episode, 5);
            BSWAP16((uint16_t*)&g_pSaveGames[i].active, 2);
        }
#endif
		FIO_std.fclose(in);
    }

    FIO_wad->mode = mode;
#endif
}

void NG_RosterLoad(void)
{
    SYS_FILEHANDLE in;
	char tex[256];
    int mode = FIO_wad->mode;
    FIO_wad->mode &= ~SYS_WAD_STATUS_ENABLED;
    NG_GetPtsFilename(tex);
    in=FIO_std.fopen(tex, "rb");
    if (in)
    {
		int ver;
		FIO_std.fread(&ver, 1, sizeof(int), in);
#ifdef __BIG_ENDIAN__
		BSWAP32((uint32_t*)&ver, 1);
#endif
		if (ver!=sizeof(SGPlayerSave))
		{
			NG_RosterReset();
			NG_RosterSave();
			sysConPrint("%s wrong version", tex);
		}
		else
		{
			FIO_std.fread( g_pSaveGames, sizeof(SGPlayerSave), MAX_SAVE_GAMES, in);

#ifdef __BIG_ENDIAN__
			{
				int i;
				for (i=0;i<MAX_SAVE_GAMES;i++)
				{
					BSWAP32((uint32_t*)&g_pSaveGames[i].episode, 5);
					BSWAP16((uint16_t*)&g_pSaveGames[i].active, 2);
				}
			}
#endif
		}
        FIO_std.fclose(in);
    }
    else
    {
        NG_RosterReset();
        NG_RosterSave();
		sysConPrint("%s not found.", tex);
	}
    FIO_wad->mode = mode;
}

void NG_RosterReset(void)
{
    int i;
    for (i=0;i<MAX_SAVE_GAMES;i++)
		NG_RosterSaveSlot(i);
}

void NG_ReplayStart(void)
{
#ifdef LT_DEV
    int k;
    g_pRecordData.version = 3;
    g_pRecordData.maxstep = g_SGSettings.RecTime*100;
    g_pRecordData.startlevel = (uint16_t)g_SGGame.Level;
    g_pRecordData.step  = 0;
    g_pRecordData.pos = (SGNetData*)MM_heap.malloc(sizeof(SGNetData)*g_pRecordData.maxstep);
    g_pRecordData.rec = (SGRecordBuffer*)MM_std.malloc(sizeof(SGRecordBuffer)*g_pRecordData.maxstep);
    for (k=0;k<g_pRecordData.maxstep;k++)
    {
        SGRecordBuffer *rec = g_pRecordData.rec + k;
        rec->buffer = (uint8_t*) MM_std.malloc(2096);
        rec->length = 0;
    }
#endif
}

int NG_ReplayLoad(void)
{
	char tex[256];
	SGRecordReplayDisk rrd;
	SYS_FILEIO *pIO = FIO_cur;
    SYS_FILEHANDLE in;
    int old = 0, number =
#if (SGTARGET != NG_DEMO_VERSION)
    g_SGGame.Demo;
#else
    0;
    #endif

#ifdef LT_DEV
    sprintf(tex, "c:\\rt\\record0.dem");
    filewad_chdir("");
    FIO_wad->mode &=~SYS_WAD_STATUS_ENABLED;
#else
    sprintf(tex, ".\\menu\\record%d.dem", number);
#endif
    pIO = &FIO_res;
    if (!FIO_cur->exists(tex))
    {
        sprintf(tex, ".\\menu\\demo%d.dem", number);
		SYS_ASSERT(FIO_cur->exists(tex));
        pIO = &FIO_gzip;
        old = 1;
    }
    else
		g_pRecordData.version = 3;
    g_SGGame.Demo++;
	if (g_SGGame.Demo>5)
		g_SGGame.Demo = 0;
    in = pIO->fopen(tex, "rb");
	SYS_ASSERT(in);

	sysConPrint("Load replay game %s", tex);
	pIO->fread(&rrd, sizeof(rrd), 1, in);
	RecordReplayDiskToMem(&rrd, &g_pRecordData);

#ifdef __BIG_ENDIAN__
	BSWAP16((uint16_t*)&g_pRecordData.startlevel, 2);
    BSWAP32((uint32_t*)&g_pRecordData.maxstep, 2);
#endif
    g_pRecordData.pos = (SGNetData*)MM_heap.malloc(sizeof(SGNetData)*g_pRecordData.step);
    pIO->fread(g_pRecordData.pos, sizeof(SGNetData), g_pRecordData.step, in);

#ifdef __BIG_ENDIAN__
    BSWAP32((uint32_t*)g_pRecordData.pos, (g_pRecordData.step*sizeof(SGNetData))>>2);
#endif

#if (SGTARGET ==NG_FULL_VERSION)
	if (g_pRecordData.version == 3)
	{
		unsigned k;
		SGRecordBuffer *p;
		g_pRecordData.rec = (SGRecordBuffer*)MM_std.malloc(sizeof(SGRecordBuffer)*g_pRecordData.step);

		for ( p = g_pRecordData.rec, k = g_pRecordData.step; k; ++p, --k )
		{
			SGRecordBufferDisk rbd;
			pIO->fread(&rbd, sizeof(rbd), 1, in);
			RecordBufferDiskToMem(&rbd, p);
#ifdef __BIG_ENDIAN__
			BSWAP32(&p->length, 1);
#endif
		}

		for ( p = g_pRecordData.rec, k = g_pRecordData.step; k; ++p, --k )
		{
			p->buffer = (uint8_t *)MM_std.malloc(p->length);
			pIO->fread(p->buffer, sizeof(uint8_t), p->length, in);
		}
	}
#endif
    pIO->fclose(in);
    g_pRecordData.maxstep = g_pRecordData.step;
    g_pRecordData.step = 0;
    g_SGObjects.IsTimed = 0;
    FIO_wad->mode |=SYS_WAD_STATUS_ENABLED;
    return g_pRecordData.startlevel;
}

void NG_ReplaySave(void)
{
#ifdef LT_DEV
#ifndef RLX_IOREADONLY
    SYS_FILEHANDLE in;
    int res=FIO_wad->mode;
    FIO_wad->mode &= ~SYS_WAD_STATUS_ENABLED;
    FIO_cur = &FIO_res;
    sprintf(tex, "c:\\rt\\record0.dem");
    in = FIO_cur->fopen(tex, "wb");
    if (!in)
        return;
    g_pRecordData.version = 3;
    FIO_cur->fwrite(&g_pRecordData, sizeof(SGRecordReplay), 1, in);
    FIO_cur->fwrite( g_pRecordData.pos, sizeof(SGNetData), g_pRecordData.step, in);
    if (g_pRecordData.version == 3)
    {
        int k;
        FIO_cur->fwrite(g_pRecordData.rec, sizeof(SGRecordBuffer), g_pRecordData.step, in);
        for (k=0;k<g_pRecordData.step;k++)
        {
            SGRecordBuffer *rec = g_pRecordData.rec + k;
            FIO_cur->fwrite(rec->buffer, sizeof(char), rec->length, in);
        }
    }
    FIO_cur->fclose(in);
    FIO_cur = g_pGameIO;
    FIO_wad->mode = res;
#endif
#endif
}

void NG_ReplayRelease(void)
{
#if (SGTARGET ==NG_FULL_VERSION)
    if (g_pRecordData.version == 3)
    {
        unsigned k;
        for (k=0;k<g_pRecordData.maxstep;k++)
        {
            SGRecordBuffer *rec = g_pRecordData.rec + k;
            MM_std.free(rec->buffer);
        }
        MM_std.free(g_pRecordData.rec);
    }
#endif
    MM_heap.free(g_pRecordData.pos);
}

