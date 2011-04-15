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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

Original Source: 1996 - Stephane Denis
Prepared for public release: 02/24/2004 - Stephane Denis, realtech VR
*/
//-------------------------------------------------------------------------
#include "lt_conf.h"

#if (SGTARGET ==NG_DEMO_VERSION)
	#define MAX_EPISODE 1
#else
    #define MAX_EPISODE 8
#endif

#define MAX_GAME_EPISODE 5

#define MAX_SAVE_GAMES 10
#define MAX_NETWORK_PLAYER 10
#define MAX_POWERUP 6
#define MAX_PLAYER 64
#define MAX_WEAPONS 64
#define MAX_SAMPLE 80
#define MAX_TEXT_MENUS 200
#define MAX_ENEMYS 256
#define MAX_COM_DELAY 256
#define MAX_LINES_README_TXT 450

#define MAX_EXPLOSIONS 256
#define MAX_DEBRIS 32
#define MAX_BLASTS 16

#define REFRESH_FNF 7
#define CURVE_PREC 32
#define RADARMAX 8000 * g_SGSettings.WorldUnit

#include "_nginfo.h"
#include "lt_cmx.h"

enum MODE_PLAYER
{
	GODMODE = 0x1,
	STEALTHMODE = 0x2,
	DOOMEDMODE = 0x4,
	TURBOMODE = 0x8,
	FROZENMODE = 0x10
};

enum NGNET
{
	SGNET_HASMOVED = 0x1, // Notifie deplacement
	SGNET_HASTURN = 0x2, // Notifie rotation
	SGNET_HASPAUSED = 0x4, // Notifie pause game
	SGNET_HASRESUMED = 0x8, // Notifie resume game
	SGNET_HASLEFT = 0x10, // Notifie quitt‚ le jeu
	SGNET_HASDIE = 0x20, // Notifie la mort
	SGNET_HASWARP = 0x40, // Notifie a warp‚
	SGNET_NEWSHOOT = 0x80, // Notifie le tir
	SGNET_COMMUNICATE = 0x100, // Message
	SGNET_CHAT = 0x200, // Chat
	SGNET_HASWON = 0x400, // Notifie a warp‚
	SGNET_SHIELDCHANGE= 0x800,
	SGNET_HASRESPAWN = 0x1000, // Notifie la mort
	SGNET_UPDATEAIM = 0x2000, // Notifie le nbre de aim
	SGNET_SHIP = 0x4000,
	SGNET_MODECHANGE = 1L<<12,
	SGNET_OTHERSIDE = 1L<<13
};

typedef struct _sg_PowerUp {
	uint8_t				val, MAX_val;
	short				Time_val;
}SGPowerUp;


typedef struct _sg_effect
{
	V3XOVI			*	OVI;
	V3XOVI			*	Sphere;
	GXSPRITEGROUP	*	Sprite;
	V3XVECTOR		*	pos;

	int32_t			Step[2];
	int32_t			Nloop;
	int32_t			LastFrame;
	V3XSCALAR			Size;

	V3XVECTOR			keep;
    V3XVECTOR           cent;
    V3XVECTOR           pos2D[2];
    V3XVECTOR           size2D[2];
	V3XPOLY				fce;
	V3XMATERIAL			material;
	V3XVECTORL			angle;
	V3XSCALAR			radius;
	int32_t             FadeStart;

	int				TagFx;
	int				FxCode;
	int				type;
	int				Dispo;

	int				useKeep;
	int				frame;
	int				PlaySample;
	int				Light;

}SGEffect;

// ***************** SGActor *****************************

typedef struct _sg_desiredmotion{
	V3XVECTOR		vel;
	V3XVECTOR		aim;
	V3XVECTOR		OldAim;
	V3XVECTOR		NewAim;
	V3XSCALAR		Dist;
	V3XSCALAR		k;
	short			Ang, Reg, Id, Pad;
}SGDesiredMotion;

typedef struct _sg_actor{
	SGScript		pInf;
	SGDesiredMotion	Mv;
	V3XOVI		*	OVI,
				*	OVItarget;
	SGEffect	*	pEffect;
}SGActor;

typedef struct _sg_control{
	V3XSCALAR		x, y, z, inert;
}SGControl;

typedef struct _sg_netdata{
	uint32_t			but;
	V3XMATRIX		Mat;
}SGNetData;

typedef struct _sg_weapon_info{
	int				cur,
					max,
					reload,
					lockTime;
	unsigned		loadTime;
}SGWeaponInfo;

typedef struct _sg_ship{
	int				defShoot;
	int				maxWeapons;
	char		*	name;
	int				code;
	SGWeaponInfo	wea[6];
}SGShip;

typedef struct _sg_player{
	SGActor			J;
	V3XKEYEULER *	Rot;
	V3XMATRIX	*	Mat;
	SGShip		*	Si;
	SGControl		Mv;
	SGNetData		Mx;
	SGPowerUp		Art[8];

	float			fBooster;
	uint32_t			Notify;
	int32_t			NetRetry;

	unsigned char	mode;
	signed char		CurArt;
	uint8_t			dispatched;
	unsigned char	reserved;

}SGPlayer;

typedef struct _sg_weapon {
	SGScript		pInf;
	V3XOVI		*	OVI,
				*	TargetLock;
	SGActor		*	Attacker;

	V3XVECTOR		vel,
					Target;

	float			fTime;
	float			Delay;

	int				Available;
	int				Camera;
	int				Freeze;
	int				TagFX;

}SGWeapon;

typedef struct _sg_game{
	V3XSCENE	*	Scene;

	int				numWeapons;
	SGWeapon	*	pWea;

	int				numEffects;
	SGEffect	*	pExpl;

	int				numPlayer;
	SGPlayer	*	pPlayer;

	int				numEnemies;
	SGActor		*	pEnemy;

	int				CameraMode;

	int				MaxAim[8],
					MaxProtect[8];

	int32_t			Count,
					oldTimer,
					Shock;

	float			RadarRange,
					DeathDist;

	V3XMATERIAL	*	pReactorMaterial;
	SYS_WAD		*	AddOnResource;
	char		**	PlayersName;
	V3XOVI		*	Missile;

	uint32_t			CI_COL2, CI_BLACK, CI_YELLOW, CI_RED, CI_WHITE, CI_GREEN, CI_BLUE, CI_BLUELIGHT;
	uint32_t			pColorRadar[8];
	uint32_t			pDisplayModes[32];

	short int		LockMAX, FlashAlpha;
	uint8_t			RadarMode, ComMode, MaxDetect[2];

	char			LangCode[4];

	uint8_t			WarpOk;
	uint8_t			IdPlayer, IsHost, Demo, Session;

	uint8_t			mySession;
	rgb24_t			FlashColor;
}SGGameStruct;

#define HIND_MAXPLAYER 16

enum {
	HIND_MODE_PHASE_0,
	HIND_MODE_PHASE_1,
	HIND_MODE_PHASE_2,
};

typedef struct _sg_AI
{
	V3XOVI		*	OVI[HIND_MAXPLAYER];
	V3XOVI		*	Target;
	int				mode,
					nHind;
	float			fTime;
}SGAI;


typedef struct _sg_environment{
	 char			SunFlare,
					SpaceDebris,
					Skybox,
					Check;
}SGENVIRONMENT;

typedef struct _sg_world{
	 char			scene_name[32];
	 char			path_name[32];
	 SGENVIRONMENT	Nav[8];
	 char			FinCode;
	 int32_t			Focal;
	 V3XSCALAR		ZClip;
	 int32_t			FogFactor, Track;
	 int32_t			Time;
	 int32_t			WarpX, WarpY, WarpZ;
	 int32_t			LimitX, LimitY, LimitZ;
}SGWORLD;


typedef struct _sg_mission
{
	SGWORLD				World;
	SGENVIRONMENT	*	Nav;
	V3XOVI			*	Sky,
					*	Ship,
					*	Laser,
					*	NavCam,
					*	Shield,
					*	Cam;
	V3XVECTOR		*	defTarget[2];
	int32_t				Time, ParTime;
	uint8_t				IsTimed, quit, MaxNAV, NAV, CallMode[2];
	char				FinCode;
}SGMISSION;

typedef struct {
   uint8_t			*	buffer;
   uint32_t				length;
}SGRecordBuffer;

typedef struct
{
	uint16_t				startlevel;
	uint16_t				version;
	uint32_t				maxstep;
	uint32_t				step;
	SGNetData		*	pos;
#if (SGTARGET ==NG_FULL_VERSION)
	SGRecordBuffer *	rec;
#endif
}SGRecordReplay;

typedef struct {
	uint8_t				behind,
						led,
						radar,
						win_radar,
						sp_radar,
						win_rear,
						sp_rear,
						win_camis,
						sp_camis,
						win_radar2,
						sp_radar2;
}SGHudDisplay;

enum NG_GAMESTATE{
	GAMESTATE_PLAY,
	GAMESTATE_DEAD,
	GAMESTATE_WON,
	GAMESTATE_FAIL,
	GAMESTATE_FAILED,
	GAMESTATE_QUIT,
	GAMESTATE_PAUSE,
	GAMESTATE_HELP,
	GAMESTATE_ABORT,
	GAMESTATE_RETRY
};

enum NG_FX {
	FX_CUSTOM,
	FX_EXPLODE_1,
	FX_IMPACT,
	FX_SHIELD_1,
	FX_DEBRIS,
	FX_FLARE,
	FX_FINISH,
	FX_WARPIN,
	FX_SMOKE,
	FX_SMOKE2
};

enum NG_PowerUp {
	 ART_NONE=0,
	 ART_CLOAK,
	 ART_SUPERPOWER,
	 ART_STEALTH,
	 ART_POWER,
	 ART_INVICIBLITY,
	 ART_MEGABANG
};

typedef struct
{
	float		fAlarmTime;
	int			Engine,
				SoundEngine,
				Alarm,
				SoundAlarm;
}SGFXAudioTable;

typedef struct {
	char	*	szTitle;
	uint8_t	*	ptr;
	int			min, max;
	char	*	Val[32];
}SGMenu;

typedef struct {
	uint32_t		time_start, time_end;
	uint32_t		killed_nmy, total_nmy;
	uint32_t		killed_amy, total_amy;
	uint32_t		bonus, total_bonus;
	uint32_t		shooted, hitted, newlevel;
	int32_t		frag;
}SGGameStat;

typedef struct {
	V3XVECTOR *	Stars;
	V3XVECTOR *	Prj[4];
	V3XVECTOR *	Sky;
	uint32_t		maxStars;
	V3XSCALAR	Zclip;
	uint32_t		Frame;
	uint32_t		LastTime;
	uint32_t		maxStars2;
	uint32_t		Mx;
}SGStarfield;

typedef struct {
	char		EpisodeName[32];
	char		LevelName[10][32];
}SGEpisode;

typedef struct {
	char		name[32];
	char		tex[16][32];
	uint16_t		WeaponMax, Bonus, Time, Level;
}SGLevelItem;

typedef struct {
	int			numLevel;
	char		filename[32];
	char		name[32];
	SGLevelItem *LI;
}SGEpisodeItem;

typedef struct {
	int			numEpisode;
	SGEpisodeItem *EI;
}SGGameItem;

enum {
	CAMERA_SHIP=5,
	CAMERA_NAV=7,
	CAMERA_DEATH=8,
	CAMERA_NEWNAV=10,
	CAMERA_START=12,
	CAMERA_MISSILE=14
};

typedef struct {
	char		name[32];
	uint8_t		level[16]; // Level done

	int32_t		episode;
	uint32_t		score;
	uint32_t		life;
	uint32_t		rank;
	time_t		last_time;

	uint16_t		active;
	uint16_t		ship;
}SGPlayerSave;

enum VCfg{
	VCfg_behind=5, VCfg_led=12, VCfg_radar=4, VCfg_sp_radar=14, VCfg_sp_rear=5, VCfg_sp_camis=6,
	VCfg_win_radar=36, VCfg_win_rear=28, VCfg_win_camis=29, VCfg_win_radar2,
	VCfg_radar2
};

enum COM_ORDER
{
	ORDER_Status,
	ORDER_AttackTarget,
	ORDER_Protect,
	ORDER_StayAlert,
	ORDER_Attack,
	ORDER_TargetWeakest,
	ORDER_TargetStrongest,
	ORDER_Retreat
};

enum COM_COMMAND
{
	COM_ShipAttacked=1,
	COM_HelpMe,
	COM_Complete,
	COM_FindWarp,
	COM_FriendDead,
	COM_Failed,
	COM_TimeShort,
	COM_NearBy,
	COM_TooFar,
	COM_EscortOk,
	COM_Status,
	COM_Bonus=15,
	COM_Weapon=21,
	COM_Request=27,
	COM_Chat=33
};

typedef struct {
	uint16_t track, mode;
	char filename[16];
}SGMusic;

enum Ms_Enum
{
	Ms_MENU1, Ms_FINAL, Ms_OVERTURE, Ms_INTRO, Ms_CREDITS, Ms_FAILED, Ms_SUCCESS
};

typedef struct {
	 int32_t Xtitle, Ytitle, item2X, item1X, YZoneMin, YZoneSize, Width2, captionX, captionY, XZoneMin, YZoneMax;
	 GXSPRITEGROUP *Font;
	 char *name;
	 SGMenu *pMenu;
	 char **menu;
	 int def;
	 uint32_t defColor;
	 uint8_t HSpacing;
	 uint8_t Help;
	 uint8_t mode;
	 uint8_t pad;
}SGMenuPosition;

typedef struct {
    char  szText[32];
    uint32_t color;
    uint32_t dist;
    uint32_t dmode, shield;
    int32_t x, y, x0, y0;
}LockInfo_Struct;

#define LK_THROTTLE g_SGSettings.key[0]
#define LK_SPEEDPLUS g_SGSettings.key[1]
#define LK_SPEEDLESS g_SGSettings.key[2]
#define LK_THRUST g_SGSettings.key[3]
#define LK_ANTITHRUST g_SGSettings.key[4]
#define LK_ROLLLEFT g_SGSettings.key[5]
#define LK_ROLLRIGHT g_SGSettings.key[6]
#define LK_STOP g_SGSettings.key[7]

#define LK_NEXTWEAPON g_SGSettings.key[8]
#define LK_FIRE g_SGSettings.key[9]
#define LK_LOCK g_SGSettings.key[10]
#define LK_ITEM g_SGSettings.key[11]
#define LK_NEXTITEM g_SGSettings.key[12]
#define LK_PREVITEM g_SGSettings.key[13]

#define LK_NEXTNAV g_SGSettings.key[14]
#define LK_HUD g_SGSettings.key[15]
#define LK_RADAR g_SGSettings.key[16]
#define LK_COM g_SGSettings.key[17]
#define LK_UP g_SGSettings.key[18]
#define LK_DOWN g_SGSettings.key[19]
#define LK_RIGHT g_SGSettings.key[20]
#define LK_LEFT g_SGSettings.key[21]
#define LK_TALK g_SGSettings.key[22]

