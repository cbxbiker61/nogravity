//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2004 - realtech VR

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

#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Directory.h>
#include <Path.h>
#include <Volume.h>
#include <VolumeRoster.h>

#include <stdio.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "sysini.h"
#include "systools.h"

STUB_Registry RLX={
 // System
 {
   RLXSYSTEM_Enable+RLXSYSTEM_Network,
   RLXOS_BEOS,
   FALSE,
   5,
   0,
   {
     NULL,
     NULL,
     NULL,
     "80386",
     "80486",
     "Pentium",
     "Pentium Pro",
     "Ix86"
   }
 },
 // Audio
 {
   RLXAUDIO_Enable+
   RLXAUDIO_Use16BIT+
   RLXAUDIO_UseSTEREO+
   RLXAUDIO_DetectHARDWARE+
   RLXAUDIO_EnableCDAudio,
   0,
   0, 8, 0,
   0x200,
   2, 32,
   {11025, 16537, 22050, 33075, 44100}
 },
 // Video
 {
   RLXVIDEO_Enable,
   RLXVIDEO_Primary,
   70,
   0,
   0
 },
 // 3D
 {
   0,
   RLX3D_Software,
   2, // 64x64
   {0, 0}
 },

 // Control
 {
   RLXCTRL_Enable+
   RLXCTRL_IntKeyboard,
   RLXCTRL_Mouse
 },
 // Joy
 {
   {
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
   }
 },
 // Network
 {
   0,
   {0, 0, 0}
 },
 // Dev
 {
   "Default",
   "RLX3",
   "Realtech",
   0,
   0,
 },
 // App
 {
   "",
   RLXAPP_IsRunning,
   0,
   1L<<20
 },
 "RLXreg.ini",
 "."
};

void STUB_RegistryReset(STUB_Registry *regs)
{
    *regs = RLX;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void STUB_RegistryRead(STUB_Registry *regs)
*
* DESCRIPTION :
*
*/

void STUB_RegistryRead(STUB_Registry *regs)
{
	char tex[256];
    ConfigFile       iniFile;

	sprintf(tex, "%s/%s", regs->IniPath, regs->IniFilename);
    if (  ReadConfig(tex, &iniFile) == 0 )
    {
        if (SelectConfigClass("System", &iniFile))
        {
            GetCF_hexa2("Config", &iniFile, (int32_t*)&regs->System.Config);
            GetCF_uchar2("Id", &iniFile, (uint8_t*)&regs->System.Id);
            GetCF_uchar2("Processor", &iniFile, (uint8_t*)&regs->System.Processor);
        }
        if (SelectConfigClass("Audio", &iniFile))
        {
            GetCF_hexa2("Config", &iniFile, (int32_t*)&regs->Audio.Config);
            GetCF_uchar2("WaveDeviceId", &iniFile, (uint8_t*)&regs->Audio.WaveDeviceId);
            GetCF_uchar2("MidiDeviceId", &iniFile, (uint8_t*)&regs->Audio.MidiDeviceId);
            GetCF_uchar2("SamplingRate", &iniFile, (uint8_t*)&regs->Audio.SamplingRate);
            GetCF_uchar2("ChannelToMix", &iniFile, (uint8_t*)&regs->Audio.ChannelToMix);
        }
        if (SelectConfigClass("Video", &iniFile))
        {
            GetCF_hexa2("Config", &iniFile, (int32_t*)&regs->Video.Config);
            GetCF_uchar2("Id", &iniFile, (uint8_t*)&regs->Video.Id);
        }
        if (SelectConfigClass("3D", &iniFile))
        {
            GetCF_hexa2("Config", &iniFile, (int32_t*)&regs->V3X.Config);
            GetCF_uchar2("Id", &iniFile, (uint8_t*)&regs->V3X.Id);
            GetCF_uchar2("TextureQuality", &iniFile, (uint8_t*)&regs->V3X.DefaultResize);
        }
        if (SelectConfigClass("Controller", &iniFile))
        {
            GetCF_hexa2("Config", &iniFile, (int32_t*)&regs->Control.Config);
            GetCF_uchar2("Id", &iniFile, (uint8_t*)&regs->Control.Id);
        }
        DestroyConfig(&iniFile);
    } else STUB_RegistryWrite(regs);
}

int STUB_RegistryWrite(STUB_Registry *regs)
{
    FILE *in;
	char tex[256];
    sprintf(tex, "%s/%s", regs->IniPath, regs->IniFilename);
    in = fopen(tex, "wt");
    if (in)
    {
         fprintf(in, "// RLX Interface settings v3.11\n");
         fprintf(in, "\n[System]\n");
         fprintf(in, "Processor=%d\n", (int)regs->System.Processor);
         fprintf(in, "Config=%x\n", (int)regs->System.Config);
         fprintf(in, "\n[Audio]\n");
         fprintf(in, "Config=%x\n", (int)regs->Audio.Config);
         fprintf(in, "WaveDeviceId=%d\n", (int)regs->Audio.WaveDeviceId);
         fprintf(in, "MidiDeviceId=%d\n", (int)regs->Audio.MidiDeviceId);
         fprintf(in, "SamplingRate=%d\n", (int)regs->Audio.SamplingRate);
         fprintf(in, "ChannelToMix=%d\n", (int)regs->Audio.ChannelToMix);
         fprintf(in, "\n[Video]\n");
         fprintf(in, "Config=%x\n", (int)regs->Video.Config);
         fprintf(in, "Id=%d\n", (int)regs->Video.Id);
         fprintf(in, "\n[3D]\n");
         fprintf(in, "Config=%x\n", (int)regs->V3X.Config);
         fprintf(in, "Id=%d\n", (int)regs->V3X.Id);
         fprintf(in, "TextureQuality=%d\n", (int)regs->V3X.DefaultResize);
         fprintf(in, "\n[Controller]\n");
         fprintf(in, "Config=%x\n",  (int)regs->Control.Config);
         fprintf(in, "Id=%d\n", (int)regs->Control.Id);

         fclose(in);
         return 1;
    }
    else return 0;
}

