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
#include <math.h>
#include <windows.h>
#include <dinput.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "sysctrl.h"
#include "systools.h"
#include "win32/w32_dx.h"
#include "win32/sys_di.h"

static LPDIRECTINPUTDEVICE8    g_pJoystick;
static LPDIRECTINPUTEFFECT     g_pEffect[2];
static DIJOYSTATE2             g_sJOY;

static int acquired;

static int CALLBACK EnumFFJoysticksCallback(LPCDIDEVICEINSTANCE pinst, LPVOID lpvContext)
{
    LPDIRECTINPUTDEVICE8 lpDevice;
    UNUSED(lpvContext);
    if (SYS_DXTRACE(g_lpDI->CreateDevice(pinst->guidInstance, &lpDevice, NULL)))
        return DIENUM_CONTINUE;
    g_pJoystick = lpDevice;
    return DIENUM_STOP;
}

static int Open(void *hwnd, int bForceFeedback)
{
    SYS_DXTRACE(g_lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumFFJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY | (bForceFeedback ? DIEDFL_FORCEFEEDBACK : 0)));

    if (g_pJoystick == NULL)
    {
        return FALSE;
    }
    
    if (SYS_DXTRACE(g_pJoystick->SetDataFormat(&c_dfDIJoystick2)))
    {
        return FALSE;
    }
    
    if (SYS_DXTRACE(g_pJoystick->SetCooperativeLevel((HWND)hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND)))
    {
        return FALSE;
    }
    
    if (bForceFeedback)
    {
        DIPROPDWORD dipdw;
        dipdw.diph.dwSize  = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = FALSE;
        return !SYS_DXTRACE(g_pJoystick->SetProperty(DIPROP_AUTOCENTER, &dipdw.diph));
    }

	DIDEVCAPS caps;
	caps.dwSize = sizeof(caps);
	g_pJoystick->GetCapabilities(&caps);
		
	if (!SYS_DXTRACE(g_pJoystick->GetCapabilities(&caps)))
	{
		sJOY->numButtons = caps.dwButtons;
		sJOY->numAxes = caps.dwAxes;
		sJOY->numPOVs = caps.dwPOVs;
	}
	sJOY->numControllers = 1;
	SYS_DXTRACE(g_pJoystick->Acquire());

    return TRUE;
}

static void Release(void)
{
    if (g_pEffect[0])
    {
        g_pEffect[0]->Release();
        g_pEffect[0] = NULL;
    }
    if (g_pJoystick)
    {
        SYS_DXTRACE(g_pJoystick->Unacquire());
        g_pJoystick->Release();
        g_pJoystick = NULL;
    }
    return;
}

_RLXEXPORTFUNC int Acquire(void)
{
    if (g_pJoystick)
    {
        SYS_DXTRACE(g_pJoystick->Acquire());
        acquired = 1;
    }
    if (g_pEffect[0])
        g_pEffect[0]->Start(1, 0);
    return 1;
}

_RLXEXPORTFUNC int Unacquire(void)
{
    if (g_pJoystick)
    {
        g_pJoystick->Unacquire();
        acquired = 0;
    }
    if (g_pEffect[0]) 
        g_pEffect[0]->Stop();
    return 1;
}

static unsigned long Update(void *device)
{
	// Poll the device to read the current state
	if (!g_pJoystick)
		return -1;

    HRESULT hr = g_pJoystick->Poll(); 
    if(FAILED(hr))  
    {
        // DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = SYS_DXTRACE(g_pJoystick->Acquire());
        while( hr == DIERR_INPUTLOST ) 
            hr = SYS_DXTRACE(g_pJoystick->Acquire());

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return S_OK; 
    }

    // Get the input's device state
    if( FAILED( hr = SYS_DXTRACE(g_pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &g_sJOY ) ) ) )
        return hr; // The device should have been acquired during the Poll()

    sJOY->lX = g_sJOY.lX;
	sJOY->lY = g_sJOY.lY;
	sJOY->lZ = g_sJOY.lZ;
	sJOY->lRx = g_sJOY.lRx;
	sJOY->lRy = g_sJOY.lRy;
	sJOY->lRz = g_sJOY.lRz;

// TODO:  lRx = lZ and lRy = lRx with PS2 pad

	sJOY->rglSlider[0] = sJOY->rglSlider[0];
	sJOY->rglSlider[1] = sJOY->rglSlider[1];	
	sysMemCpy(sJOY->rgdwPOV, g_sJOY.rgdwPOV, 4 * 4);
	sysMemCpy(sJOY->steButtons, sJOY->rgbButtons, 128);
	sysMemCpy(sJOY->rgbButtons, g_sJOY.rgbButtons, 128);

#if DIRECTINPUT_VERSION >=0x0700
	sJOY->lVX = g_sJOY.lVX;
	sJOY->lVY = g_sJOY.lVY;
	sJOY->lVZ = g_sJOY.lVZ;
	sJOY->lVRx = g_sJOY.lVRx;
	sJOY->lVRy = g_sJOY.lVRy;
	sJOY->lVRz = g_sJOY.lVRz;
	sJOY->rglVSlider[0] = g_sJOY.rglVSlider[0];
	sJOY->rglVSlider[1] = g_sJOY.rglVSlider[1];
	sJOY->lAX = g_sJOY.lAX;
	sJOY->lAY = g_sJOY.lAY;
	sJOY->lAZ = g_sJOY.lAZ;
	sJOY->lARx = g_sJOY.lARx;
	sJOY->lARy = g_sJOY.lARy;
	sJOY->lARz = g_sJOY.lARz;
	sJOY->rglASlider[0] = g_sJOY.rglASlider[0];
	sJOY->rglASlider[1] = g_sJOY.rglASlider[1];
	sJOY->lFX = g_sJOY.lFX;
	sJOY->lFY = g_sJOY.lFY;
	sJOY->lFZ = g_sJOY.lFZ;
	sJOY->lFRx = g_sJOY.lFRx;
	sJOY->lFRy = g_sJOY.lFRy;
	sJOY->lFRz = g_sJOY.lFRz;
	sJOY->rglFSlider[0] = g_sJOY.rglFSlider[0];
	sJOY->rglFSlider[1] = g_sJOY.rglFSlider[1];
#endif
	return 0;

}

static JOY_ClientDriver MsJoy = {
    Open,
    Release,
    Update
};


JOY_ClientDriver *JOY_SystemGetInterface_STD(void)
{
    return &MsJoy;
}
