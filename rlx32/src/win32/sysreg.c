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
#include <stdio.h>
#include <windows.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "sysini.h"
#include "systools.h"

STUB_Registry RLX={
 // System
 {
   RLXSYSTEM_Enable+RLXSYSTEM_Network, 
   RLXOS_WIN32, 
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
 "rlxreg.ini", 
 "."
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
    ConfigFile   iniFile;
    u_int32_t  UserNameLength = 16;
    if (!GetComputerName(RLX.App.UserName, &UserNameLength))
		sysStrCpy(RLX.App.UserName, "New user");

    if (ReadConfig(regs->IniFilename, &iniFile)==1)
    {
		char tex[256];
		sprintf(tex, "%s\\%s", regs->IniPath, regs->IniFilename);
		if (ReadConfig(tex, &iniFile)!=0) 
			return;
    }
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
    }
    return;
}

char *SYSREG_GetString(char *value, void *Class, char *Group, char *Key)
{
    HKEY  hKey = NULL;
    LPSTR String = NULL;
    u_int8_t  val[256];
    if (RegOpenKeyEx(Class, Group, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
		DWORD valtype;
		DWORD valsize = sizeof(val);
		int32_t  res = RegQueryValueEx(hKey, Key, 0, &valtype, val, &valsize);
		if (res == ERROR_SUCCESS) String = (char*)val;
		RegCloseKey(hKey);
    }
    if (String)
    {
       sysStrCpy(value, String);
       return value;
    }
    else return NULL;
}

int SYSREG_SetString(char *value, void *Class, char *Group, char *Key)
{
    HKEY  hKey = NULL;
    LONG err;
    RegCreateKey(Class, Group, &hKey);
    err = RegSetValueEx(hKey, Key, 0, REG_SZ, (u_int8_t*)value, strlen(value)+1);
	RegCloseKey(hKey);
    return err==ERROR_SUCCESS;
}

