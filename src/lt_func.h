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
#ifndef _LT_FUNC_HH
#define _LT_FUNC_HH

#include "console.h"
struct _fli_struct;
struct _v3x_ovi;
struct _v3xvector2;

__extern_c

void NG_PlayPresentsGame(void);
void NG_PlayGameOver(void);
void NG_PlayEndEpisode(int episode);
void NG_PlayTheEnd(void);
int NG_EndLevel(void);
void NG_PlayLoadingScreen(void);
void NG_DrawLoadingScreen(void);
void NG_PlayEndGame(void);
void NG_MenuCredits(void);
void NG_ExitMessage(int quit);
int NG_BriefingGame(void);
void NG_ReadMissionList(void);
void NG_ReleaseMissionList();
int NG_MainMenu(void);
void NG_CleanUp(void);
void SGMOU_MapKeyboard();
void SGJOY_MapKeyboard();
void NG_DrawFlash();
void NG_ResetKey();
void NG_SetGamma(float gamma);

void NG_RosterReset(void);
void NG_RosterLoad(void);
void NG_RosterReset(void);
void NG_RosterSaveSlot(int i);
void NG_RosterSave(void);

void NG_HighScoresSave(void);
void NG_HighScoresLoad(void);
void NG_HighScoresUpdate(void);

void NG_GameStart(void);
void NG_GameStop(void);

void NG_RenderView(void);
void NG_DrawHelpFile(GXSPRITEGROUP *font, int color2, int xz);    
void NG_InstallHandlers(void);
int NG_ColorToNAV(int x);
void NG_CreateDisplayList(void);

void NG_LoadGameData(void);
void NG_ReleaseGameData(void);
void NG_ChangeScreenMode(int newmode);
//
int NG_WaitForKeyPress(void);
int NG_SampleButtonJoystick();
int NG_SampleButtonMouse();
void NG_WaitForKeyWithDelay(int temps);

void CALLING_C NG_DrawCircle(struct _v3xvector2 *c, int32_t r, int32_t cx, int cote);
void NG_StageReadFile(char *fn, int extrn);
void NG_StretchFont(GXSPRITEGROUP *pSpriteGroup, int fx, int fy);
void NG_DrawTicker(void);
void NG_DrawBackgroundPic(char *szFilename, int TrackPlay, int mode);
void NG_CheckSystems(void);

// Starfield
void NG_LoadFonts();
void NG_ReleaseFonts();
void NG_StarfieldCreate(void);
void NG_StarfieldRelease(void);
int NG_FXGetByName(char *s);
void NG_FXLoadList();
void NG_FXDraw();

// Struct
void NG_AddFaceMoreObjects(void);
void NG_AddMoreLights(void);
V3XOVI *NG_GetFreeSphere(void);
void NG_DisplayWarp(void);

void NG_RenderDisplayBox(struct _button_item *but, V3XMATRIX *Mat, V3XOVI *OVI);
void NG_InitGameShip(void);
void NG_LevelUpdate(void);
void NG_InitAnimateStage(void);
GXSPRITEGROUP *NG_LoadMovie(char *file, int die);

// Waypoint
void NG_WeaponCreate(void);
int NG_WeaponFire(SGActor *Jj, int type, V3XOVI *target);
void NG_WeaponUpdate(void);
int NG_MomentumValue(int x, int mx);

// Nav
void NG_NAVReset(int reset);
void NG_NAVClear(void);

void NG_StretchFont(GXSPRITEGROUP *pSpriteGroup, int fx, int fy);
void NG_GamePlay(void);    
void NG_DrawVideoSubtitles(FLI_STRUCT *);
void NG_ReadLanguagePack(void);
void NG_SetLanguage();
int NG_ExecMainMenu(char **menu, int def, u_int32_t defColor, u_int8_t spacing);

// AI
void NG_AIRandomAim(V3XVECTOR *a, V3XVECTOR *cent, V3XSCALAR r);
void NG_AIRandomAimEx(V3XVECTOR *a, V3XVECTOR *cent, V3XSCALAR r, int32_t al, int32_t bl);
V3XOVI *NG_AILocateNearestEnemy(V3XVECTOR *pos, V3XVECTOR *dist, V3XSCALAR *di, int mode);
V3XOVI *NG_AILocateNearestTarget(V3XVECTOR *pos, V3XVECTOR *dist, V3XSCALAR *di);
void NG_AISetTacticMode(void);

// HUD
void NG_HudLockTargetOff(void);
void NG_HudLockTargetReset(void);
void NG_HudLockTargetOn(void);
void NG_HudDisplayCamera(void);
int NG_HudDisplayLocked(struct _v3x_ovi *OVI);
void NG_HudDisplayRear(void);
GXSPRITEGROUP *NG_LoadSpriteGroup(char *filename, int resize);
void NG_LoadBackground(char *file, GXSPRITE *sp);
void NG_ChangeGameDetail(void);

// FX
void NG_FXUpdateEx(void);
SGEffect *NG_FXNew(V3XVECTOR *pos, int type, int lop, SGScript *pInf, int kp, V3XOVI *OVI);
void NG_FXBlast(SGScript *pInf, V3XOVI *OVI);
void NG_FXDebris(SGScript *pInf, V3XOVI *OVI);
void NG_FXAvary(V3XOVI *mOVI, SGScript *pInf);
V3XOVI *NG_SetSpherePos(V3XOVI *OVI, V3XMATRIX *Mat, V3XSCALAR scale, int status);
void NG_FXSetSceneShading(V3XSCENE *Scene, int limit);    
void NG_FXNoTextureScene(V3XSCENE *Scene);
void NG_FXChangeRenderObject(V3XMESH *obj, int nouv);
void NG_FXFlare(void);
void NG_FXImpact(int power);    
void NG_FXLoadData();
void NG_FXReleaseData();
void NG_FXCreate();
void NG_FXUpdate();
void NG_FXRelease();

// Record
void NG_ReplayStart(void);
int NG_ReplayLoad(void);
void NG_ReplaySave(void);
void NG_ReplayRelease(void);

// Audio
int  NG_AudioGetByName(char *s);
void NG_AudioPlayWave(char *wave);
void NG_AudioStreamWave(char *wave);
void NG_AudioBeep(int code);
void NG_AudioPlayWarp(void);
void NG_AudioPlayTrack(int ms);
void NG_AudioLoadWave(void);
void NG_AudioLoadSong(char *filename);
void NG_AudioStopTrack(void);
int  NG_AudioPlaySound(int number, float pan);
void NG_AudioStreamStart(char *wave);
void NG_AudioLoadList(void);
void NG_AudioStreamWave(char *wave);
void NG_AudioKickSound(int chan, int number, float pan);
void NG_AudioSay(char *voicename);
int NG_Audio3DUpdate(V3XVECTOR *v, V3XVECTOR *speed, int chan, int number);
int NG_Audio3DPlay(int number, V3XVECTOR *v, V3XVECTOR *speed);
void NG_AudioSetFreq(int chan, int freq);
void NG_AudioStopSound(int chan);
void NG_AudioSetMusicVolume(void);
void NG_AudioPlayMusic(void);
void NG_AudioStopMusic(void);
void NG_AudioPauseMusic(void);
void NG_AudioResumeMusic(void);

// Network
void NG_NetInitialize(void);
void NG_NetRelease(void);
void NG_MenuMessage(char *tex);
int NG_NetSelectMode(void);
void NG_NetDisplay(int force);
void NG_NetRender();
void NG_NetDispatch(void);
u_int32_t NG_NetDispatchPlayer(unsigned playerId, u_int8_t *buffer);
unsigned NG_NetTranslate(SGPlayer *pInf, u_int8_t *be);

__end_extern_c

#endif 

