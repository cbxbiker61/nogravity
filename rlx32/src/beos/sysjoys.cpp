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

#include <device/Joystick.h>
#include <stdlib.h>
#include <stdio.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "sysctrl.h"
#include "systools.h"

class CController : public BJoystick 
{
public:
	int16_t		*	Axis;
	u_int8_t	*	Hats;
	int32_t			InitCheck;
};

static class CController *g_pJoystick;      

static unsigned long Update(void *pdevice)
{
	if (!g_pJoystick->InitCheck)
		return 0;

	unsigned i;
	sysMemCpy(sJOY->steButtons, sJOY->rgbButtons, sizeof(sJOY->steButtons));
	g_pJoystick->Update();
	g_pJoystick->GetAxisValues(g_pJoystick->Axis);
	g_pJoystick->GetHatValues(g_pJoystick->Hats);

	for (int *Axis = &sJOY->lX, i=0;i<g_pJoystick->CountAxes();i++)
		Axis[i] = 32768 + (int32_t)g_pJoystick->Axis[i];

	for (i=0;i<(unsigned)g_pJoystick->CountHats();i++)
		sJOY->rgdwPOV[i] = g_pJoystick->Hats[i] ? (g_pJoystick->Hats[i] - 1) * 4500 : -1;

	for (uint32 Buttons = g_pJoystick->ButtonValues(), i=0;i<(unsigned)sJOY->numButtons;i++)
		sJOY->rgbButtons[i] = (Buttons >> i) & 1 ? 0x80 : 0;

	return 0;
}

static int DeviceStart(void)
{
	char name[B_OS_NAME_LENGTH];

	if (!g_pJoystick->CountDevices())
	{
		g_pJoystick->InitCheck = 0;
		return -1;
	}

	g_pJoystick->GetDeviceName(0, name);
	if (g_pJoystick->Open(name, true)==B_ERROR)
	{
    	g_pJoystick->InitCheck = 0;
		return -2;
	}
	else
	{
		g_pJoystick->EnterEnhancedMode();
		g_pJoystick->EnableCalibration(true);
    	g_pJoystick->InitCheck = 1;
	}
	return 0;
}

static int Open(void *hwnd, int bForceFeedback)
{
	g_pJoystick = new CController();
	g_pJoystick->Axis = (int16*)MM_std.malloc(sizeof(int16)*(1+g_pJoystick->CountAxes()));
	g_pJoystick->Hats = (uint8*)MM_std.malloc(sizeof(uint8)*(1+g_pJoystick->CountHats()));
	sJOY->device = g_pJoystick;
	sJOY->numControllers = g_pJoystick->CountDevices();	
	sJOY->numAxes = g_pJoystick->CountAxes();
	sJOY->numPOVs = g_pJoystick->CountHats();

#ifdef _DEBUG
	SYS_Debug("Joystick count : %d\n", sJOY->numControllers); 
#endif
	
	int i;
	for (i=0;i<sJOY->numControllers;i++)
	{
		 char outName[ B_OS_NAME_LENGTH];
		 g_pJoystick->GetDeviceName(i, outName);
#ifdef _DEBUG
		 SYS_Debug("Device %s : \n", outName);
#endif
	}

    if (!sJOY->numControllers)
	{
		// Leak CController
		return 0;
	}


	return DeviceStart();
}

static void Release(void)
{

	MM_std.free(g_pJoystick->Axis);
	MM_std.free(g_pJoystick->Hats);
	return;
}

static JOY_ClientDriver JoystickClientBeOS = {
	Open,
	Release,
	Update
};

JOY_ClientDriver *JOY_SystemGetInterface_STD(void)
{
    sJOY = &JoystickClientBeOS;
	return sJOY;
}
