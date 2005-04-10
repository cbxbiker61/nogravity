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
#include "sysini.h"
#include "sysctrl.h"
#include "systools.h"
#include "_stub.h"
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
#include "gx_init.h"
#include "gx_rgb.h"
#include "gx_tools.h"
#include "gui_os.h"
//
#include "lt_struc.h"
#include "lt_data.h"
#include "lt_func.h"

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  SGScript *GetSifByName(char *s)
*
* DESCRIPTION :
*
*/
SGScript *GetSifByName(char *s)
{
	       
    SGScript *pScript, *f=NULL;
    int i;
    for (pScript=g_pPlayerInfo, i=0;(i<g_SGSettings.maxCase)&&(f==NULL);i++, pScript++)
    {
        if (strcmp(s, pScript->Basename)==0) 
			f=pScript;
    }
	SYS_ASSERT(f);
    return f;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_StageReadFile(char *fin)
*
* DESCRIPTION :
*
*/
void NG_StageReadFile(char *fin, int extrn)
{
    ConfigFile iniFile;
    int32_t val;
    u_int8_t val2;
    char *s;
    char tex2[64], fn[64];
    SYS_FILEHANDLE in;
    int i=0, iCase=0;
    SGScript *pScript = 0;
    if (extrn)
    {
        sprintf(fn, "%s", fin);
    }
    else
    {
        sprintf(fn, "%s\\cmx\\%s", ".", fin);
    }
    in = FIO_cur->fopen(fn, "rb");
	SYS_ASSERT(in);
    do
    {
		char tex[256];
		char tmp[1024];
		FIO_cur->fgets(tmp, 1025, in);
		if (strlen(tmp)<4)
			continue;

        sscanf(tmp, "%s %s", tex, tex2);
        if (strcmp(tex, "include")==0) 
			NG_StageReadFile(tex2, extrn);
        else
        if (strcmp(tex, "xinclude")==0) 
			NG_StageReadFile(tex2, 1);
        else i=1;
    } while(i!=1);
    FIO_cur->fclose(in);
    if ( ReadConfig(fn, &iniFile) == 0 )
    {
        if(SelectConfigClass("LiTHiUM FOE Informations", &iniFile))
        {
            iCase = 1+GetCF_long("LastCase", &iniFile);
			SYS_ASSERT(g_SGSettings.maxCase<MAX_PLAYER);
            pScript=g_pPlayerInfo+g_SGSettings.maxCase;
        }
                    
		if(SelectConfigClass("World", &iniFile))
		{
			g_SGObjects.World.Focal = GetCF_long("Focal", &iniFile);
			g_SGObjects.World.ZClip = (V3XSCALAR)GetCF_long("ZClip", &iniFile);
		}
		for (i=0;i<8;i++)
		{
			char tex[16];
			sprintf(tex, "NAV %d", i);
			if (SelectConfigClass(tex, &iniFile))
			{
				g_SGObjects.World.Nav[i].SunFlare = (u_int8_t)GetCF_bool("SunFlare", &iniFile);
				g_SGObjects.World.Nav[i].SpaceDebris = (u_int8_t)GetCF_bool("Debris", &iniFile);
				g_SGObjects.World.Nav[i].Skybox = (u_int8_t)GetCF_bool("Sky", &iniFile);
				g_SGObjects.World.Nav[i].Check=1;
			} else g_SGObjects.World.Nav[i].Check=0;
		}
		for (i=1;i<8;i++)
		{
			if (!g_SGObjects.World.Nav[i].Check) 
				g_SGObjects.World.Nav[i]=g_SGObjects.World.Nav[0];
		}
		if(SelectConfigClass("Scene", &iniFile))
		{
			g_SGObjects.World.Track = GetCF_long("Music", &iniFile);
			g_SGObjects.ParTime = GetCF_long("ParTime", &iniFile);
			sysStrnCpy(g_SGObjects.World.scene_name, GetCF_str2("Scene", &iniFile), 31);
			sysStrnCpy(g_SGObjects.World.path_name, GetCF_str2("Path", &iniFile), 31);
		}
		if (iCase>1)
		for (i=0;i<iCase;i++, pScript++)
		{
			char tex[256];
			g_SGSettings.maxCase++;
			sprintf(tex, "case %d", i);
			if(SelectConfigClass(tex, &iniFile))
			{
				s = GetCF_str("SameAs", &iniFile);
				if (s)
				{
					sysMemCpy(pScript, GetSifByName(s), sizeof(SGScript));
				} else
				{
					sysMemZero(pScript, sizeof(SGScript));
					pScript->Tactic = t_TAC_NONE;
					pScript->SpeedTurn = 4;
					pScript->Animation = t_FIXE;
				}
				s = GetCF_str("Basename", &iniFile);
				SYS_ASSERT(s);
				sysStrnCpy(pScript->Basename, s, 12);
				s = GetCF_str("Type", &iniFile);
				if (s)
				{
					if (strcmp(s, "t_SPECIAL")==0) pScript->Type=t_SPECIAL;
					else
					if (strcmp(s, "t_OBSTACLE")==0) pScript->Type=t_OBSTACLE;
					else
					if (strcmp(s, "t_ENEMY")==0) pScript->Type=t_ENEMY;
					else
					if (strcmp(s, "t_NEUTRAL")==0) pScript->Type=t_NEUTRAL;
					else
					if (strcmp(s, "t_PIRAT")==0) pScript->Type=t_PIRAT;
					else
					if (strcmp(s, "t_FRIEND")==0) pScript->Type=t_FRIEND;
					else
					if (strcmp(s, "t_DECOR")==0) pScript->Type=t_DECOR;
					else
					if (strcmp(s, "t_PLAYER")==0) pScript->Type=t_PLAYER;
					else
					if (strcmp(s, "t_ACCESSOIRE")==0) pScript->Type=t_ACCESSOIRE;
					else
					{
						SYS_ASSERT(0);
					}
				}
				s = GetCF_str("Realname", &iniFile);
				if (s) sysStrnCpy(pScript->Realname, s, 12);
				else
				sysStrnCpy(pScript->Realname, pScript->Basename, 12);
				s = GetCF_str("Movement", &iniFile);
				if (s)
				{
					if (strcmp(s, "TURN_AROUND_X")==0) pScript->Animation=t_TURN_AROUND_X;
					else
					if (strcmp(s, "TURN_AROUND_Y")==0) pScript->Animation=t_TURN_AROUND_Y;
					else
					if (strcmp(s, "TURN_AROUND_Z")==0) pScript->Animation=t_TURN_AROUND_Z;
					else
					if (strcmp(s, "TURN_RANDOM")==0) pScript->Animation=t_TURN_RANDOM;
					else
					if (strcmp(s, "FIXE")==0) pScript->Animation=t_FIXE;
					else
					if (strcmp(s, "GIANT")==0)
					{
						pScript->Animation=t_GIANT;
					}
				}
				s = GetCF_str("Tactic", &iniFile);
				if (s)
				{
					if (strcmp(s, "NONE")==0) pScript->Tactic=t_TAC_NONE;
					else
					if (strcmp(s, "RANDOM_PATROL")==0) pScript->Tactic=t_TAC_RANDOM_PATROL;
					else
					if (strcmp(s, "FIGHT_PATROL1")==0) pScript->Tactic=t_TAC_PATROL_FIGHT1;
					else
					if (strcmp(s, "ATTACK1")==0) pScript->Tactic=t_TAC_ATTACK1;
					else
					if (strcmp(s, "ATTACK0")==0) pScript->Tactic=t_TAC_ATTACK0;
					else
					if (strcmp(s, "TRACK")==0) pScript->Tactic=t_TAC_TRACK;
					else
					if (strcmp(s, "TRACKFREE")==0) pScript->Tactic=t_TAC_TRACKFREE;
					else
					if (strcmp(s, "TRACKFULL")==0) pScript->Tactic=t_TAC_TRACKFULL;
					else
					if (strcmp(s, "MINE")==0) pScript->Tactic=t_TAC_MINE;
					else
					if (strcmp(s, "DROID")==0) pScript->Tactic=t_TAC_DROID;
					else
					if (strcmp(s, "HIND")==0) pScript->Tactic=t_TAC_HIND;
				}
				s = GetCF_str("EndTrack", &iniFile);
				if (s)
				{
					if (strcmp(s, "WARP")==0) pScript->EndTrack=t_ET_WARP;
				}
				s = GetCF_str("AfterTrack", &iniFile);
				if (s)
				{
					if (strcmp(s, "ADD_AIM")==0) pScript->AfterTrack=t_AD_ADD_AIM;
					if (strcmp(s, "FAILED")==0)  pScript->AfterTrack=t_AD_FAILED;
					if (strcmp(s, "SUCCESS")==0)  pScript->AfterTrack=t_AD_SUCCESS;
					if (strcmp(s, "EXPLODE")==0)  pScript->AfterTrack=t_AD_EXPLODE;
				}
				s = GetCF_str("EventCode", &iniFile);
				if (s)
				{
					if (strcmp(s, "ENABLE")==0)      pScript->EventCode=t_EC_ENABLE;
					if (strcmp(s, "SHOOTAPPEAR")==0) pScript->EventCode=t_EC_APPEAR;
					if (strcmp(s, "SHIELDOFF")==0)   pScript->EventCode=t_EC_SHIELDOFF;
					if (strcmp(s, "SHIELDOFFDIE")==0) pScript->EventCode=t_EC_SHIELDOFFDIE;
					if (strcmp(s, "TRACK_TOGGLE")==0) pScript->EventCode=t_EC_TRACK_TOGGLE;
				}
				s = GetCF_str("EventType", &iniFile);
				if (s)
				{
					if (strcmp(s, "BYSHOOT")==0)      pScript->EventType=t_ET_SHOOT;
					if (strcmp(s, "BYDESTROY")==0)    pScript->EventType=t_ET_DESTROY;
				} else pScript->EventType=t_ET_SHOOT;
				s = GetCF_str("AfterDestroy", &iniFile);
				if (s)
				{
					if (strcmp(s, "ADD_AIM")==0)  pScript->AfterDestroy=t_AD_ADD_AIM;
					if (strcmp(s, "FAILED")==0)   pScript->AfterDestroy=t_AD_FAILED;
					if (strcmp(s, "SUCCESS")==0)  pScript->AfterDestroy=t_AD_SUCCESS;
				}
				s = GetCF_str("Collision", &iniFile);
				if (s)
				{
					if (strcmp(s, "NONE")==0) pScript->Collision=t_COL_NONE;
					else
					if (strcmp(s, "AUTOSPHERE")==0) pScript->Collision=t_COL_AUTOSPHERE;
					else
					if (strcmp(s, "AUTOSPHERE2")==0) pScript->Collision=t_COL_AUTOSPHERE2;
				}
				s = GetCF_str("CollisionStyle", &iniFile);
				if (s)
				{
					if (strcmp(s, "ADD_SCORE_HIDE")==0) pScript->CollisionStyle=t_CS_ADD_SCORE_HIDE;
					else
					if (strcmp(s, "ADD_SCORE")==0) pScript->CollisionStyle=t_CS_ADD_SCORE;
					else
					if (strcmp(s, "BUMP")==0) pScript->CollisionStyle=t_CS_BUMP;
					else
					if (strcmp(s, "BUMP_LOSE_SHIELD")==0) pScript->CollisionStyle=t_CS_BUMP_LOSE_SHIELD;
					else
					if (strcmp(s, "LOSE_SHIELD")==0) pScript->CollisionStyle=t_CS_LOSE_SHIELD;
					else
					if (strcmp(s, "WARP")==0) pScript->CollisionStyle=t_CS_WARP;
					else
					if (strcmp(s, "BONUS")==0) pScript->CollisionStyle=t_CS_BONUS;
					else
					if (strcmp(s, "NAV")==0) pScript->CollisionStyle=t_CS_NAV;
					else
					if (strcmp(s, "NAVWARP")==0) pScript->CollisionStyle=t_CS_NAVWARP;
				}
				s = GetCF_str("AppearMode", &iniFile);
				if (s) pScript->AppearMode = NG_FXGetByName(s);
				s = GetCF_str("Remplace", &iniFile);
				if (s) pScript->Replace = GetSifByName(s)-g_pPlayerInfo;
				s = GetCF_str("DebrisMode", &iniFile);
				if (s) pScript->DebrisMode = NG_FXGetByName(s);
				s = GetCF_str("DebrisMode2", &iniFile);
				if (s) pScript->DebrisMode2 = NG_FXGetByName(s);
				s = GetCF_str("ExplodeMode", &iniFile);
				if (s) pScript->ExplodeMode = NG_FXGetByName(s);
				s = GetCF_str("FinisHim", &iniFile);
				if (s) pScript->FinisHim = NG_FXGetByName(s);
				s = GetCF_str("StartSound", &iniFile);
				if (s) pScript->StartSound = NG_AudioGetByName(s);
				s = GetCF_str("SoundFinish", &iniFile);
				if (s) pScript->StartSound = NG_AudioGetByName(s);
				s = GetCF_str("Doppler", &iniFile);
				if (s) pScript->Doppler = NG_AudioGetByName(s);
				s = GetCF_str("SoundExplode", &iniFile);
				if (s) pScript->SoundExplode = NG_AudioGetByName(s);
				s = GetCF_str("SoundAppear", &iniFile);
				if (s) pScript->SoundAppear = NG_AudioGetByName(s);
				s = GetCF_str("SoundAmbiant", &iniFile);
				if (s) pScript->AmbiantSound = 0; // NG_AudioGetByName(s);
				if (GetCF_long2("Event", &iniFile, &val)) pScript->Event=(u_int8_t)val;
				if (GetCF_long2("AnimMode", &iniFile, &val)) pScript->AnimMode=(u_int8_t)val;
				if (GetCF_long2("Appear", &iniFile, &val)) pScript->Appear=val;
				if (GetCF_long2("Avary", &iniFile, &val)) pScript->Avary=(u_int8_t)val;
				if (GetCF_long2("SpeedTurn", &iniFile, &val)) pScript->SpeedTurn=(u_int8_t)val;
				if (GetCF_long2("ShootAxe", &iniFile, &val)) pScript->ShootAxe=(u_int8_t)val;
				if (GetCF_long2("Shield", &iniFile, &val)) pScript->Shield=val; else pScript->Shield=1;
				if (GetCF_long2("ShieldMax", &iniFile, &val)) pScript->ShieldMax=val;
				else pScript->ShieldMax=pScript->Shield;
				if (GetCF_long2("Weight", &iniFile, &val)) pScript->Poids=val;
				if (GetCF_long2("Sight", &iniFile, &val)) pScript->Sight=val;
				if (GetCF_long2("Scoring", &iniFile, &val)) pScript->Scoring=val;
				if (GetCF_long2("Code", &iniFile, &val)) pScript->Code=(u_int8_t)val;
				if (GetCF_long2("Wait", &iniFile, &val)) pScript->Wait=val;
				if (GetCF_long2("SpeedMax", &iniFile, &val)) pScript->fSpeedMax=(float)val;
				if (GetCF_long2("Attack", &iniFile, &val)) pScript->Attack=(u_int8_t)val;
				if (GetCF_long2("ColorRadar", &iniFile, &val)) pScript->ColorRadar=(u_int8_t)val;
				if (GetCF_long2("NeverShock", &iniFile, &val)) pScript->NeverShock=(u_int8_t)val;
				if (GetCF_long2("Stealth", &iniFile, &val))
				{
					pScript->Stealth = 1;
					pScript->Appear = val+sysRand(256);
					if (!pScript->Wait) pScript->Wait = pScript->Appear;
				}
				if (GetCF_bool2("Aim", &iniFile, &val2)) pScript->Mission=val2;
				if (GetCF_bool2("Protect", &iniFile, &val2)) pScript->Protect=val2;
				if (GetCF_bool2("NeverHide", &iniFile, &val2)) pScript->NeverHide=val2 ? 1 : 2;
				if (GetCF_bool2("BigExplode", &iniFile, &val2)) pScript->BigExplode=val2;
				if (GetCF_bool2("NoFogging", &iniFile, &val2)) pScript->NoFog=val2;
			} 
			else
			{
				SYS_ASSERT(0);
			}
		}
		DestroyConfig(&iniFile);
    }
    else
    {
        SYS_ASSERT(0);
    }
    return;
}
