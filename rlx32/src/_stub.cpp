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
#define STUB_VERSION "1.98"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WINDOWS
#define INITGUID
#include <dplay.h>
DEFINE_GUID(STUB_GUID, 0xdbd83641, 0xe44, 0x11d1, 0xa3, 0x9, 0x0, 0xa0, 0x24, 0x5a, 0x36, 0x78);

#endif
#include "_rlx32.h"
#include "_rlx.h"

#include "_stub.h"

#include "systools.h"
#include "sysctrl.h"
#include "sysnetw.h"
#include "sysresmx.h"

#include "gx_struc.h"
#include "gx_init.h"
#include "gx_rgb.h"
#include "iss_defs.h"
#include "v3xdefs.h"
#include "v3xtrig.h"
#include "v3x_2.h"

void STUB_Down(void)
{
 #ifdef _DEBUG
     SYS_Debug("Release input devices..\n");
 #endif
  	if (sKEY)
		sKEY->Release();

	if (sJOY)
		sJOY->Release();

	if (sMOU)
	    sMOU->Release();

 #ifdef _DEBUG
     SYS_Debug("Release audio device..\n");
 #endif
 	V3XA.Client->Release();
 	
 #ifdef _DEBUG
     SYS_Debug("Release 3d engine..\n");
 #endif
	V3X.Client->Shutdown();
	V3XKernel_Release();

 #ifdef _DEBUG
     SYS_Debug("Release 2d engine..\n");
 #endif

    if (GX.Client)
		GX.Client->Shutdown();

    return;
}

static int STUB_CheckAudio(void *hwnd)
{
	V3XA_EntryPoint(&RLX);
 #ifdef _DEBUG
     SYS_Debug("init audio..\n");
 #endif
	V3XA.Client->Enum();
    if (!V3XA.Client->Detect())
    {
        if (!V3XA.Client->Initialize(hwnd))
			V3XA.State|= 1;
		else
			V3XA.State &= ~1;

    }
    else
		V3XA.State&=~1;
    return V3XA.State & 1;
}

static int STUB_CheckVideo(void *hwnd)
{
 #ifdef _DEBUG
     SYS_Debug("init video.. (%s)\n", GX.Client->s_DrvName);
 #endif
    SYS_ASSERT(GX.Client);
    return GX.Client->Open(hwnd);
}

static void STUB_CheckControl(void *hwnd)
{
 #ifdef _DEBUG
     SYS_Debug("init input..\n");
 #endif
    sKEY = KEY_SystemGetInterface_STD();
    sKEY->Open(hwnd);
    
    sMOU = MSE_SystemGetInterface_STD();
    sMOU->Open(hwnd);
    
    sJOY = JOY_SystemGetInterface_STD();
    sJOY->Open(hwnd, 0);

	RLX.Control.mouse = sMOU;
	RLX.Control.joystick = sJOY;
	RLX.Control.keyboard = sKEY;

	return;
}


static void STUB_Check3D(void *hwnd)
{
 #ifdef _DEBUG
     SYS_Debug("init 3d engine..\n");
 #endif
	GX_KernelAlloc();
    TRG_Generate();
	V3X_EntryPoint(&RLX);
    SYS_ASSERT(V3X.Client);
    V3XKernel_Alloc();
}

static void STUB_CheckSystem(void *hwnd)
{                       
 #ifdef _DEBUG
     SYS_Debug("init system engine..\n");
 #endif
 	RLX.mm_heap = &MM_heap;
	RLX.mm_std = &MM_std;
	RLX.pfGetPixelFormat = RGB_GetPixelFormat;
	RLX.pfSetPixelFormat = RGB_SetPixelFormat;
	RLX.pfSmartConverter = RGB_SmartConverter;
	RLX.pfSetViewPort = GX_SetupViewport;
	RLX.pGX = &GX;
	RLX.pV3X = &V3X;
    sysInitFS();

#ifdef WIN32
	sNET = NET_GetDirectPlayInterface(hwnd, &STUB_GUID);
	SYS_ASSERT(sNET);
#endif
	
    return;
}

void STUB_CheckUp(void *hwnd)
{
#ifdef __STUB_RLX
    if ((RLX.App.Config&RLXAPP_DefaultRegs)==0)
        STUB_RegistryRead(&RLX);
#endif

	STUB_CheckSystem(hwnd);
    STUB_CheckControl(hwnd);
    STUB_CheckAudio(hwnd);
    STUB_Check3D(hwnd);
    STUB_CheckVideo(hwnd);

#ifdef __STUB_NET
#ifdef WIN32
	sNET = NET_GetDirectPlayInterface();
#endif
#endif


    return;
}
