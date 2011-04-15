#pragma once
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
typedef struct _sg_script
{
	char Basename[12];
	char Realname[12];
	int32_t Shield, ShieldMax, Scoring, Sight, Poids;
	int32_t Wait, ShootOk, Appear, AfterDeath;
	float fSpeed, fFrozen, fSpeedMax;

	int
	SifCode , AfterDestroy, ExplodeMode, SoundExplode,
	FinisHim , Type , DebrisMode2, AmbiantChannel,
	Collision , Tactic , Attack , Animation,
	AmbiantSound, StartSound , Doppler , CollisionStyle,
	Mission , NeverHide , ColorRadar, Doppling,
	Locked , Protect , LockTime , ShootAxe,
	Code , Avary , AppearMode, SoundAppear,
	Stealth , AfterTrack , Event , EventCode,
	BigExplode , EventType , DebrisMode, AnimMode,
	Replace , NeverShock , ColorNav , EndTrack,
	PlayerNearby, NoFog , ModeX , SpeedTurn ;
}SGScript;
/*
0:rien
+1:Morph
+2:Track
+4:Loop
+8:Off
+16:Reverse
+32:PingPong
*/
enum {m_MORPH=1, m_TRACK=2, m_LOOP=4, m_OFF=8, m_REVERSE=16, m_PINGPONG=32};
enum {t_ET_SHOOT=1, t_ET_DESTROY, t_ET_SIGHT};
enum {t_EC_ENABLE=1, t_EC_APPEAR, t_EC_SHIELDOFF, t_EC_TRACK_TOGGLE, t_EC_TRACKON,
      t_EC_TRACKOFF, t_EC_MORPHLOOP, t_EC_MORPHON, t_EC_MORPHOFF, t_EC_SHIELDOFFDIE};
enum {t_ET_WARP=1};
enum {idNORMAL=0, idESCAPE=1, idATTACK=2, idSTANDBY=4};
enum {
    t_ENEMY=0, t_FRIEND, t_SPECIAL, t_PIRAT, t_PLAYER, t_OBSTACLE, t_DECOR, t_ACCESSOIRE, t_NEUTRAL,
    t_FIXE=16, t_TURN_RANDOM, t_TURN_AROUND_X, t_TURN_AROUND_Y, t_TURN_AROUND_Z, t_GIANT, t_TRACK, t_MPLAYER,
    t_TAC_NONE=32,
    t_TAC_RANDOM_PATROL,
    t_TAC_PATROL_FIGHT1,
    t_TAC_ATTACK1,
    t_TAC_TRACK,
    t_TAC_ATTACK0,
    t_TAC_MINE,
    t_TAC_TRACKSTOP,
    t_TAC_TRACKFREE,
    t_TAC_DROID,
    t_TAC_TRACKFULL,
    t_TAC_HIND,
    t_COL_NONE=48, t_COL_AUTOSPHERE, t_COL_AUTOSPHERE2,
    t_CS_BUMP=64, t_CS_BUMP_LOSE_SHIELD, t_CS_LOSE_SHIELD,
    t_CS_ADD_SCORE, t_CS_WARP, t_CS_NAV, t_CS_NAVWARP,
    t_CS_ADD_SCORE_HIDE, t_CS_BONUS,
    t_AD_ADD_AIM, t_AD_FAILED, t_AD_EXPLODE, t_AD_SUCCESS
};

