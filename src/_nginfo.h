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
	u_int8_t	key[MAX_KEYS];
    u_int8_t	joy[MAX_KEYS];
	u_int8_t	mou[MAX_KEYS];

    u_int8_t	joyPOV[4];
	float		WorldUnit;
	int32_t		sp_inert, maxCase;
    u_int32_t	player;    
    int			ResolutionX, ResolutionY, ColorDepth;
    int			ComNumber, ComTime, ComFTime;
    u_int8_t	VolDIG, VolFX, VolMusic;
    u_int8_t 	VisualsFx;
	u_int8_t 	Interlaced;
    u_int8_t 	ZBuffer;
	u_int8_t 	Flip;
	u_int8_t 	TexFiltering;
	u_int8_t	TexPOT;
	u_int8_t 	VerticalSync;
	u_int8_t    Multisampling;
	u_int8_t 	Stereo;
	u_int8_t 	Language;
    u_int8_t 	LangCode;    
	u_int8_t 	LensFX;
    u_int8_t 	Palette;    
    u_int8_t 	Dithering;
    u_int8_t 	Sky;   
    u_int8_t 	Shade;
    u_int8_t 	Wide;
    u_int8_t 	Cheater;        
    u_int8_t 	Difficulty;
	u_int8_t 	CamDisp;
    u_int8_t 	TexCompression;
    u_int8_t 	bClearView;    
    u_int8_t 	showInf;    
    u_int8_t 	ticker;
    u_int8_t 	cursor;
    u_int8_t 	DemoMode;
    u_int8_t 	SerialGame;
    u_int8_t 	Intro;
    u_int8_t 	DuoGame;
    u_int8_t 	NextMenu;    
    u_int8_t 	GoToBrief;
    u_int8_t 	RecTime;
    u_int8_t 	Menu;
    u_int8_t 	DemoDelay;
    u_int8_t 	rearview;
    u_int8_t 	HalfSize;   
    u_int8_t 	OS;
    u_int8_t 	AddOn;    
	u_int8_t 	FlipYMouse;
    u_int8_t 	FrameSkip;
	u_int8_t 	Ctrl;
	u_int8_t	MouseSensitivity;
	u_int8_t    ComPlayer;
	u_int8_t	DisplayIndex;
	u_int8_t	AxisThrottle;
	u_int8_t	AxisRoll;
	u_int8_t	AltMouse;

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
