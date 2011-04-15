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
#define LK_MAX        22
#define MUSIC_CDAUDIO  1
#define MUSIC_MODULE   2
#define MUSIC_WAVE     3

#define MAX_KEYS	  32
typedef struct
{
	uint8_t	key[MAX_KEYS];
    uint8_t	joy[MAX_KEYS];
	uint8_t	mou[MAX_KEYS];

    uint8_t	joyPOV[4];
	float		WorldUnit;
	int32_t		sp_inert, maxCase;
    uint32_t	player;
    int			ResolutionX, ResolutionY, ColorDepth;
    int			ComNumber, ComTime, ComFTime;
    uint8_t	VolDIG, VolFX, VolMusic;
    uint8_t	VisualsFx;
	uint8_t	Interlaced;
    uint8_t	ZBuffer;
	uint8_t	Flip;
	uint8_t	TexFiltering;
	uint8_t	TexPOT;
	uint8_t	VerticalSync;
	uint8_t    Multisampling;
	uint8_t	Stereo;
	uint8_t	Language;
    uint8_t	LangCode;
	uint8_t	LensFX;
    uint8_t	Palette;
    uint8_t	Dithering;
    uint8_t	Sky;
    uint8_t	Shade;
    uint8_t	Wide;
    uint8_t	Cheater;
    uint8_t	Difficulty;
	uint8_t	CamDisp;
    uint8_t	TexCompression;
    uint8_t	bClearView;
    uint8_t	showInf;
    uint8_t	ticker;
    uint8_t	cursor;
    uint8_t	DemoMode;
    uint8_t	SerialGame;
    uint8_t	Intro;
    uint8_t	DuoGame;
    uint8_t	NextMenu;
    uint8_t	GoToBrief;
    uint8_t	RecTime;
    uint8_t	Menu;
    uint8_t	DemoDelay;
    uint8_t	rearview;
    uint8_t	HalfSize;
    uint8_t	OS;
    uint8_t	AddOn;
	uint8_t	FlipYMouse;
    uint8_t	FrameSkip;
	uint8_t	Ctrl;
	uint8_t	MouseSensitivity;
	uint8_t    ComPlayer;
	uint8_t	DisplayIndex;
	uint8_t	AxisThrottle;
	uint8_t	AxisRoll;
	uint8_t	AltMouse;

}NG_GameSettings;

enum {CTRL_Mouse, CTRL_Keyb, CTRL_Joystick};

#ifdef __cpluspluis
extern "C" {
    #endif
	void   NG_SaveGameInfo(void);
	void   NG_SetGameInfo(void);
	void   NG_ReadGameConfig(void);
    extern NG_GameSettings g_SGSettings;
#ifdef __cpluspluis
}
#endif
