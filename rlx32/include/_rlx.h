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
#ifndef _RLXREGISTRY
#define _RLXREGISTRY

enum {
    RLXOS_DOS32  = 0x1,
    RLXOS_WIN32  = 0x2,
    RLXOS_BEOS = 0x4,
    RLXOS_MACOS  = 0x8,
    RLXOS_BEOSINTEL = 0x10,
    RLXOS_PSX = 0x20,
    RLXOS_LINUX  = 0x21
};
enum {
    RLXSYSTEM_Enable = 0x1,
    RLXSYSTEM_MultiThread = 0x2,
    RLXSYSTEM_Network  = 0x4,
    RLXSYSTEM_IntelMMX = 0x8,
    RLXSYSTEM_Amd3DNow = 0x10,
    RLXSYSTEM_IntelXMMX = 0x20
};

typedef struct {
    uint32_t  Config;
    uint8_t  Id, DietMode, Processor, Running;
    char  *CPU_List[8];
}RLX_RegisterSystem;

enum {
    RLXAUDIO_Enable = 0x1,
    RLXAUDIO_Use16BIT  = 0x2,
    RLXAUDIO_UseSTEREO = 0x4,
    RLXAUDIO_UseSURRND = 0x8,
    RLXAUDIO_UseBASS = 0x10,
    RLXAUDIO_FlipSTEREO = 0x20,
    RLXAUDIO_CacheONDISK = 0x40,
    RLXAUDIO_DetectHARDWARE = 0x80,
    RLXAUDIO_EnableCDAudio = 0x100,
    RLXAUDIO_Use3D = 0x200,
    RLXAUDIO_UseHwMixer = 0x400
};

typedef struct {
    uint32_t  Config;
    uint8_t  WaveDeviceId, MidiDeviceId, ChannelToMix, filler;
    uint16_t ioPort;
    uint32_t  SamplingRate, CacheSize;
    uint32_t  SR_to_Khz[8];
}RLX_RegisterAudio;

enum {
    RLXVIDEO_Enable = 0x1,
    RLXVIDEO_Use2DHARDWARE = 0x2,
    RLXVIDEO_UseLFB = 0x4,
    RLXVIDEO_WideDAC = 0x8,
    RLXVIDEO_Windowed = 0x10,
    RLXVIDEO_ConvertImg8bit = 0x20
};


enum {
    RLXVIDEO_VGA,
    RLXVIDEO_SVGA,
    RLXVIDEO_VESA12,
    RLXVIDEO_VESA2,
    RLXVIDEO_VESA2U,
    RLXVIDEO_3D
};

enum {
    RLXVIDEO_Primary,
    RLXVIDEO_Secondary
};

typedef struct {
    uint32_t  Config;
    uint8_t  Id;
    uint8_t  Refresh;
    uint8_t  Gamma;
    uint8_t  filler;
}RLX_RegisterVideo;

enum {
    RLX3D_none,
    RLX3D_Software,
    RLX3D_S3VIRGE,
    RLX3D_3DFX,
    RLX3D_DYNAMIC,
    RLX3D_DIRECT3D,
    RLX3D_OPENGL,
    RLX3D_METAL,
};
enum {
    RLX3D_Reserved = 0x1,
    RLX3D_NoPalettizedTex = 0x2,
    RLX3D_UseMultiTex = 0x4,
    RLX3D_UseAGP = 0x8,
    RLX3D_AutoDetect = 0x10,
    RLX3D_FakeHardware = 0x20,
    RLX3D_TexManager = 0x40,
    RLX3D_NoBusMastering = 0x80,
	RLX3D_FullQualityTex = 0x100
};
typedef struct {
    uint32_t  Config;
    uint8_t  Id;
    uint8_t  DefaultResize;
    uint8_t  filler[2];
}RLX_Register3D;

enum {
    RLXCTRL_Keyboard,
    RLXCTRL_Mouse,
    RLXCTRL_JoyAnalog,
    RLXCTRL_JoyPad,
    RLXCTRL_ThrustMaster,
    RLXCTRL_SWIFT,
    RLXCTRL_VirtualIO,
    RLXCTRL_SideWinder
};

enum {
    RLXCTRL_Enable = 0x1,
    RLXCTRL_IntKeyboard = 0x2,
	RLXCTRL_Uncalibrated = 0x4
};

struct _RClientDriver_Mouse;
struct _RClientDriver_Joystick;
struct _RClientDriver_Keybrd;

typedef struct {
    uint32_t Config;
    uint8_t Id;
	uint8_t Device;
    uint8_t filler[2];

	struct _RClientDriver_Mouse *mouse;
	struct _RClientDriver_Joystick *joystick;
	struct _RClientDriver_Keybrd *keyboard;

}RLX_RegisterController;

typedef struct {
    int32_t MinX, MinY;
    int32_t MaxX, MaxY;
    int32_t MinZ, MaxZ;
    int32_t MinR, MaxR;
}RLX_RegisterJoystick;

typedef struct {
    RLX_RegisterJoystick J[2];
	uint32_t Config;
}RLX_RegisterJoystickCal;

typedef struct {
    uint8_t  Id;
    uint8_t  filler[3];
}RLX_RegisterNetwork;

typedef struct {
    char   *ApplicationName;
    char   *TeamSignature;
    char   *Developper;
    uint8_t   ReleaseVersion;
    uint8_t   Status;
}RLX_RegisterDevelopper;

enum {
    RLXAPP_IsRunning  = 0x1,
    RLXAPP_IsPaused = 0x2,
    RLXAPP_Signal = 0x4,
    RLXAPP_DefaultRegs = 0x8
};

typedef struct {
    char    UserName[16];
    uint32_t   Config;
    uint32_t   Heap_Start;
    uint32_t   Heap_Required;
}RLX_RegisterApplication;


struct _gx_rgb24;
struct _gx_viewport;

typedef void			(*RLXAPI PFGXSETUPVIEWPORT)(struct _gx_viewport *pView, int width, int height, int bitsPerPixel); // setup viewport size
typedef uint8_t		*	(*RLXAPI PFRGB_SMARTCONVERTER)(void *dst, struct _gx_rgb24 *dst_pal, int dst_bpp, void *src, const struct _gx_rgb24 *src_pal, int src_bpp, uint32_t size);
typedef void			(*RLXAPI PFRGB_GETPIXELFORMAT)(struct _gx_rgb24 *rgb, uint32_t c);
typedef unsigned		(*RLXAPI PFRGB_SETPIXELFORMAT)(int r, int g, int b);

struct _sys_memory;

typedef struct RLXSYSTEM{
    RLX_RegisterSystem      System;
    RLX_RegisterAudio       Audio;
    RLX_RegisterVideo       Video;
    RLX_Register3D          V3X;
    RLX_RegisterController  Control;
    RLX_RegisterJoystickCal Joy;
    RLX_RegisterNetwork     Network;
    RLX_RegisterDevelopper  Dev;
    RLX_RegisterApplication App;
    char					IniPath[256];
    char					IniFilename[12];
	PFGXSETUPVIEWPORT		pfSetViewPort;			// Set Viewport callback
	PFRGB_SMARTCONVERTER	pfSmartConverter;		// Color conversion callback
	PFRGB_GETPIXELFORMAT	pfGetPixelFormat;		// Get Pixel Format callback
	PFRGB_SETPIXELFORMAT	pfSetPixelFormat;		// Set Pixel Format calbback
	struct _sys_memory	*	mm_heap;
	struct _sys_memory	*	mm_std;
	struct GXSYSTEM		*	pGX;
	struct V3XSYSTEM	*	pV3X;
	void				*	pApplication; // BEOS specific
}STUB_Registry;

__extern_c
    _RLXEXPORTFUNC char  *SYSREG_GetString(char *value, void *Class, char *Group, char *Key);
    _RLXEXPORTFUNC int    SYSREG_SetString(char *value, void *Class, char *Group, char *Key);
    _RLXEXPORTFUNC void   STUB_RegistryReset(STUB_Registry *regs);
    _RLXEXPORTFUNC void   STUB_RegistryRead(STUB_Registry *regs);
    _RLXEXPORTFUNC int    STUB_RegistryWrite(STUB_Registry *regs);
    _RLXEXPORTDATA extern STUB_Registry RLX;
__end_extern_c

#endif

