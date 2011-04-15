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
#include <stdio.h>
#include <string.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "_stub.h"
#include "systools.h"
#include "sysctrl.h"
#include "gx_init.h"
#include "iss_defs.h"
#include "sysini.h"
#include "_nginfo.h"

NG_GameSettings g_SGSettings;

void NG_GetIniFilename(char *tex)
{
	sprintf(tex, "%s/nogravity.ini", RLX.IniPath);
}

static char *YesNo[]={"no", "yes"};

char static *SaveKey(uint8_t *table)
{
	char tex[256];
    char *s=tex;
    int i;
    *s = 0;
    for (i=0;i<LK_MAX;i++)
    {
        char te[4];
        sprintf(te, "%2x ", table[i]);
        strcat(s, te);
    }
    return s;
}

static int ReadKey(char *s, uint8_t *table)
{
    int i, d;
    for (i=0, d=0;i<LK_MAX;i++, s+=3)
    {
        char te[4];
        sysStrnCpy(te, s, 3);
        if (strlen(te)>=2)
        {
            int j;
            sscanf(te, "%x", &j);
            if (j)
            {
                table[i]=j;
                d++;
            }
	    else
		table[i]=0;
        }
    }
    return d>=LK_MAX-1;
}

static void ResetKey(void)
{
    g_SGSettings.key[0] = s_rightshift;
    g_SGSettings.key[1] = s_numplus;
    g_SGSettings.key[2] = s_numminus;
    g_SGSettings.key[3] = s_tab;
    g_SGSettings.key[4] = s_backspace;
    g_SGSettings.key[5] = s_delete;
    g_SGSettings.key[6] = s_insert;
    g_SGSettings.key[7] = s_slash;
    g_SGSettings.key[8] = s_0;
    g_SGSettings.key[9] = s_alt;
    g_SGSettings.key[10] = s_return;
    g_SGSettings.key[11] = s_space;
    g_SGSettings.key[12] = s_equals;
    g_SGSettings.key[13] = s_minus;
    g_SGSettings.key[14] = s_a;
    g_SGSettings.key[15] = s_i;
    g_SGSettings.key[16] = s_r;
    g_SGSettings.key[17] = s_c;
    g_SGSettings.key[18] = s_up;
    g_SGSettings.key[19] = s_down;
    g_SGSettings.key[20] = s_right;
    g_SGSettings.key[21] = s_left;
    g_SGSettings.key[22] = s_t;

    g_SGSettings.joy[9] = 1;
    g_SGSettings.joy[0] = 2;
    g_SGSettings.joy[10] = 3;
    g_SGSettings.joy[8] = 4;
    g_SGSettings.joy[4] = 5;
    g_SGSettings.joy[11] = 6;
    g_SGSettings.joy[14] = 7;
    g_SGSettings.joy[16] = 8;

	g_SGSettings.mou[9] = 1;
    g_SGSettings.mou[0] = 2;
}

void NG_SaveGameInfo(void)
{
	char tex[256];
#ifndef RLX_IOREADONLY
    SYS_FILEHANDLE in;
    char *Ctr[]={"Mouse", "Keyboard", "Joystick"};
	NG_GetIniFilename(tex);
    in = fopen(tex, "wt");
    if ( in  )
    {
        fprintf(in, "[Game]\n");
#ifdef LT_DEVELOP
        fprintf(in, "DebugSphere=%s\n", YesNo[g_SGSettings.DebSphere]);
        fprintf(in, "Cheat=%s\n", YesNo[g_SGSettings.Cheater]);
#endif
        fprintf(in, "Language=%d\n", (int)g_SGSettings.Language);
        fprintf(in, "Difficulty=%d\n", g_SGSettings.Difficulty);
        fprintf(in, "OS=%d\n", g_SGSettings.OS);
        fprintf(in, "\n[Video]\n");
        fprintf(in, "Dithering=%s\n", YesNo[ g_SGSettings.Dithering ]);
        fprintf(in, "TexFiltering=%s\n", YesNo[ g_SGSettings.TexFiltering ]);
		fprintf(in, "TexPOT=%s\n", YesNo[ g_SGSettings.TexPOT ]);
		fprintf(in, "TexCompression=%s\n", YesNo[ g_SGSettings.TexCompression ]);
        fprintf(in, "Sky=%d\n", (int)g_SGSettings.Sky );
        fprintf(in, "Shading=%d\n", (int)2 );
        fprintf(in, "Gamma=%d\n", (int)RLX.Video.Gamma);

        fprintf(in, "LensFX=%d\n", (int)g_SGSettings.LensFX);
        fprintf(in, "VisualsFx=%d\n", (int)g_SGSettings.VisualsFx);

        fprintf(in, "Zbuffer=%s\n", YesNo[g_SGSettings.ZBuffer]);
		fprintf(in, "ResolutionX=%d\n", (int)g_SGSettings.ResolutionX);
		fprintf(in, "ResolutionY=%d\n", (int)g_SGSettings.ResolutionY);
        fprintf(in, "ColorDepth=%d\n", g_SGSettings.ColorDepth);
        fprintf(in, "FrameSkip=%d\n", g_SGSettings.FrameSkip);
		fprintf(in, "Multisampling=%d\n", g_SGSettings.Multisampling);
		fprintf(in, "VSync=%d\n", g_SGSettings.VerticalSync);
        fprintf(in, "\n[Sound]\n");

        fprintf(in, "VolFX=%f\n", (float)g_SGSettings.VolFX);
        fprintf(in, "VolDIG=%f\n", (float)g_SGSettings.VolDIG);
        fprintf(in, "VolMusic=%f\n", (float)g_SGSettings.VolMusic);

        fprintf(in, "\n[Control]\n");
        fprintf(in, "Controller=%s\n", Ctr[g_SGSettings.Ctrl]);
        fprintf(in, "Speed=%d\n", (int)g_SGSettings.MouseSensitivity);
        fprintf(in, "FlipY=%s\n", YesNo[g_SGSettings.FlipYMouse]);
        fprintf(in, "Key=%s\n", SaveKey(g_SGSettings.key));
        fprintf(in, "Joy=%s\n", SaveKey(g_SGSettings.joy));
		fprintf(in, "Mou=%s\n", SaveKey(g_SGSettings.mou));
		fprintf(in, "AltMouse=%d\n", g_SGSettings.AltMouse);
		fprintf(in, "JoyThrottleAxis=%d\n", g_SGSettings.AxisThrottle);
		fprintf(in, "JoyRollAxis=%d\n", g_SGSettings.AxisRoll);
		if (RLX.Joy.Config != RLXCTRL_Uncalibrated)
		fprintf(in, "JoyCalibration2=%d %d %d %d %d %d %d %d",
					RLX.Joy.J[0].MinX,
					RLX.Joy.J[0].MaxX,
					RLX.Joy.J[0].MinY,
					RLX.Joy.J[0].MaxY,
					RLX.Joy.J[0].MinZ,
					RLX.Joy.J[0].MaxZ,
					RLX.Joy.J[0].MinR,
					RLX.Joy.J[0].MaxR
					);

        fclose(in);
    }
#endif
}

void NG_ReadGameConfig(void)
{
	char tex[256];

#ifndef RLX_IOREADONLY
    ConfigFile  iniFile;
    char *s;
    NG_GetIniFilename(tex);
    ResetKey();

	g_SGSettings.ResolutionX = 640;
	g_SGSettings.ResolutionY = 480;

    if ( ReadConfig(tex, &iniFile) == 0 )
    {
        if(SelectConfigClass("Game", &iniFile))
        {
            g_SGSettings.Intro = (uint8_t)GetCF_bool("Intro", &iniFile);
            g_SGSettings.Language = (uint8_t)GetCF_long("Language", &iniFile);
            g_SGSettings.Difficulty = (uint8_t)GetCF_long("Difficulty", &iniFile);
            g_SGSettings.OS = (uint8_t)GetCF_long("OS", &iniFile);
            if (!g_SGSettings.OS)
				g_SGSettings.OS=66;
        }

        if(SelectConfigClass("Video", &iniFile))
        {
            g_SGSettings.ResolutionX = (uint16_t)GetCF_long("ResolutionX", &iniFile);
			g_SGSettings.ResolutionY = (uint16_t)GetCF_long("ResolutionY", &iniFile);
			g_SGSettings.ColorDepth = (uint8_t)GetCF_long("ColorDepth", &iniFile);

            g_SGSettings.Dithering = (uint8_t)GetCF_bool("Dithering", &iniFile);
            g_SGSettings.TexFiltering = (uint8_t)GetCF_bool("TexFiltering", &iniFile);
			g_SGSettings.TexPOT = (uint8_t)GetCF_bool("TexPOT", &iniFile);
            g_SGSettings.VisualsFx = (uint8_t)GetCF_long("VisualsFx", &iniFile);
            g_SGSettings.LensFX = (uint8_t)GetCF_bool("LensFX", &iniFile);
            g_SGSettings.Sky = (uint8_t)GetCF_long("Sky", &iniFile);
            RLX.Video.Gamma = (uint8_t)GetCF_long("Gamma", &iniFile);
            g_SGSettings.ZBuffer = (uint8_t)GetCF_bool("ZBuffer", &iniFile);
            g_SGSettings.FrameSkip = (uint8_t)GetCF_long("FrameSkip", &iniFile);

			if (g_SGSettings.ResolutionX<=511)
			{
				g_SGSettings.ResolutionX = 640;
				g_SGSettings.ResolutionY = 480;
			}
			g_SGSettings.Multisampling = (uint8_t)GetCF_long("Multisampling", &iniFile);
			g_SGSettings.VerticalSync = (uint8_t)GetCF_long("VSync", &iniFile);
        }

        if(SelectConfigClass("Sound", &iniFile))
        {
            g_SGSettings.VolDIG = (uint8_t)GetCF_long("VolDIG", &iniFile);
            g_SGSettings.VolFX = (uint8_t)GetCF_long("VolFX", &iniFile);
            g_SGSettings.VolMusic = (uint8_t)GetCF_long("VolMusic", &iniFile);
        }

        if(SelectConfigClass("Control", &iniFile))
        {
            g_SGSettings.MouseSensitivity = (uint8_t)GetCF_long("Speed", &iniFile);
            g_SGSettings.FlipYMouse = (uint8_t)GetCF_bool("FlipY", &iniFile);
            s = GetCF_str("Controller", &iniFile);
            if (s)
            {
                if (strcmp(s, "Keyboard")==0)
					g_SGSettings.Ctrl = CTRL_Keyb;
                else
                if (strcmp(s, "Joystick")==0)
					g_SGSettings.Ctrl = CTRL_Joystick;
                else g_SGSettings.Ctrl = CTRL_Mouse;
            }
            s = GetCF_str("Key", &iniFile);
            if (s) ReadKey(s, g_SGSettings.key);
            s = GetCF_str("Joy", &iniFile);
            if (s) ReadKey(s, g_SGSettings.joy);
			s = GetCF_str("Mou", &iniFile);
            if (s) ReadKey(s, g_SGSettings.mou);
			s = GetCF_str("JoyCalibration2", &iniFile);
			if (s)
			{
				sscanf(s, "%d %d %d %d %d %d %d %d", &RLX.Joy.J[0].MinX, &RLX.Joy.J[0].MaxX, &RLX.Joy.J[0].MinY, &RLX.Joy.J[0].MaxY,
				&RLX.Joy.J[0].MinZ, &RLX.Joy.J[0].MaxZ, &RLX.Joy.J[0].MinR, &RLX.Joy.J[0].MaxR);
			}
			else
			{
#if defined __APPLE__ || defined __BEOS__
				RLX.Joy.Config = RLXCTRL_Uncalibrated;
#endif
			}
			s = GetCF_str("AltMouse", &iniFile);
            if (s) g_SGSettings.AltMouse = GetCF_long("AltMouse", &iniFile);
			else g_SGSettings.AltMouse = 0;
			s = GetCF_str("JoyThrottleAxis", &iniFile);
            if (s) g_SGSettings.AxisThrottle = GetCF_long("JoyThrottleAxis", &iniFile);
			else g_SGSettings.AxisThrottle = 0;
			s = GetCF_str("JoyRollAxis", &iniFile);
            if (s) g_SGSettings.AxisRoll = GetCF_long("JoyRollAxis", &iniFile);
			else g_SGSettings.AxisRoll = 0;

        }
        DestroyConfig(&iniFile);
    }
    else
#endif
    {
        g_SGSettings.Difficulty = 1;
        g_SGSettings.DemoMode = 0;
        g_SGSettings.Intro = 0;
        g_SGSettings.MouseSensitivity = 4+8;
        g_SGSettings.VisualsFx = 4;
        g_SGSettings.VolDIG = 100;
        g_SGSettings.VolMusic = 100;
        g_SGSettings.VolFX = 100;
        g_SGSettings.VisualsFx = 3;
        g_SGSettings.Language = 0;
        g_SGSettings.Ctrl = CTRL_Mouse;
        g_SGSettings.OS = 0;
		RLX.V3X.Id = RLX3D_DIRECT3D;
    }

    if (g_SGSettings.OS!=RLX.System.Id)
    {
        RLX.Video.Gamma = 1;
        g_SGSettings.Sky = 1;
        g_SGSettings.Dithering = 1;
        g_SGSettings.LensFX = 1;
		g_SGSettings.ResolutionX = 640;
		g_SGSettings.ResolutionY = 480;
		g_SGSettings.ColorDepth = 32;

        switch(RLX.V3X.Id)
		{
            case RLX3D_3DFX:
            case RLX3D_DIRECT3D:
            case RLX3D_OPENGL:
		        g_SGSettings.TexFiltering = 1;
			    g_SGSettings.VisualsFx = 4;
            break;
            case RLX3D_S3VIRGE:
            g_SGSettings.TexFiltering = 1;
            g_SGSettings.VisualsFx = 2;
            g_SGSettings.showInf = 0;
            break;
            default:
            g_SGSettings.VisualsFx = 3;
            g_SGSettings.TexFiltering = 0;
            break;
        }
        g_SGSettings.OS = RLX.System.Id;
        NG_SaveGameInfo();
    }
}

