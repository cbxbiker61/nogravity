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

#include <stdio.h>

#include "_rlx32.h"
#include "_rlx.h"
#include "sysini.h"
#include "systools.h"

STUB_Registry RLX={
 // System
 {
   // TODO: Understand what all these options mean and if the right ones are chosen.
   RLXSYSTEM_Enable, 
   RLXOS_LINUX,
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
   RLXAUDIO_UseSTEREO,
   0, 0, 8, 0,
   0,
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
   RLX3D_OPENGL,
   2,
   {0, 0}
 }, 

 // Control
 {
   RLXCTRL_Enable+
   RLXCTRL_IntKeyboard,
   RLXCTRL_JoyAnalog+
   RLXCTRL_Mouse,
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
   "application/x-vnd",
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
 "rlxreg.ini",
 ""
};
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void STUB_RegistryReset(STUB_Registry *regs)
*
* DESCRIPTION :
*
*/
void STUB_RegistryReset(STUB_Registry *regs)
{
    *regs = RLX;
    return;
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
  ConfigFile iniFile;

  sprintf(tex, "%s/%s", regs->IniPath, regs->IniFilename);
    if (  ReadConfig(tex, &iniFile) == 0 )
    {
        if (SelectConfigClass("System", &iniFile))
        {
            GetCF_hexa2("Config", &iniFile, (int32_t*)&regs->System.Config);
            GetCF_uchar2("Id", &iniFile, (u_int8_t*)&regs->System.Id);
            GetCF_uchar2("Processor", &iniFile, (u_int8_t*)&regs->System.Processor);
        }
        if (SelectConfigClass("Audio", &iniFile))
        {
            GetCF_hexa2("Config", &iniFile, (int32_t*)&regs->Audio.Config);
            GetCF_uchar2("WaveDeviceId", &iniFile, (u_int8_t*)&regs->Audio.WaveDeviceId);
            GetCF_uchar2("MidiDeviceId", &iniFile, (u_int8_t*)&regs->Audio.MidiDeviceId);
            GetCF_uchar2("SamplingRate", &iniFile, (u_int8_t*)&regs->Audio.SamplingRate);
            GetCF_uchar2("ChannelToMix", &iniFile, (u_int8_t*)&regs->Audio.ChannelToMix);
        }
        if (SelectConfigClass("Video", &iniFile))
        {
            GetCF_hexa2("Config", &iniFile, (int32_t*)&regs->Video.Config);
            GetCF_uchar2("Id", &iniFile, (u_int8_t*)&regs->Video.Id);
        }
        if (SelectConfigClass("3D", &iniFile))
        {
            GetCF_hexa2("Config", &iniFile, (int32_t*)&regs->V3X.Config);
            GetCF_uchar2("Id", &iniFile, (u_int8_t*)&regs->V3X.Id);
            GetCF_uchar2("TextureQuality", &iniFile, (u_int8_t*)&regs->V3X.DefaultResize);
        }
        if (SelectConfigClass("Controller", &iniFile))
        {
            GetCF_hexa2("Config", &iniFile, (int32_t*)&regs->Control.Config);
            GetCF_uchar2("Id", &iniFile, (u_int8_t*)&regs->Control.Id);
        }
        DestroyConfig(&iniFile);
    } else STUB_RegistryWrite(regs);
    return;
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
         fprintf(in, "Processor=%d\n", regs->System.Processor);
         fprintf(in, "Config=%x\n",    regs->System.Config);
         fprintf(in, "\n[Audio]\n");
         fprintf(in, "Config=%x\n",    regs->Audio.Config);
         fprintf(in, "WaveDeviceId=%d\n", regs->Audio.WaveDeviceId);
         fprintf(in, "MidiDeviceId=%d\n", regs->Audio.MidiDeviceId);
         fprintf(in, "SamplingRate=%d\n", regs->Audio.SamplingRate);
         fprintf(in, "ChannelToMix=%d\n", regs->Audio.ChannelToMix);
         fprintf(in, "\n[Video]\n");
         fprintf(in, "Config=%x\n",    regs->Video.Config);
         fprintf(in, "Id=%d\n",        regs->Video.Id);
         fprintf(in, "\n[3D]\n");
         fprintf(in, "Config=%x\n",     regs->V3X.Config);
         fprintf(in, "Id=%d\n",        regs->V3X.Id);
         fprintf(in, "TextureQuality=%d\n", regs->V3X.DefaultResize);
         fprintf(in, "\n[Controller]\n");
         fprintf(in, "Config=%x\n",    regs->Control.Config);
         fprintf(in, "Id=%d\n",        regs->Control.Id);

         fclose(in);
         return 1;
    }
    else return 0;
}
