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

#include <windows.h>
#include <dinput.h>
#include <stdio.h>
#include <stdlib.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "systools.h"
#include "sysctrl.h"
#include "win32/sys_di.h"
#include "win32/w32_dx.h"

static LPDIRECTINPUTDEVICE8		g_pMouse;
static DIMOUSESTATE2				sState;

static void SetPosition(u_int32_t x, u_int32_t y)
{
    SetCursorPos(x, y);
    return;
}

static void Show(void)
{
    ShowCursor(TRUE);
    return;
}

static void Hide(void)
{
    ShowCursor(FALSE);
    return;
}
static int Open(void *hwnd) 			// Return non-zero if error
{
	if (SYS_DXTRACE(g_lpDI->CreateDevice(GUID_SysMouse, &g_pMouse, NULL)))
       return -2;

	if (SYS_DXTRACE(g_pMouse->SetDataFormat(&c_dfDIMouse2)))
	{
		return -3;
	}
	if (SYS_DXTRACE(g_pMouse->SetCooperativeLevel((HWND)hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
	{
		return -4;
	}
	SYS_DXTRACE(g_pMouse->Acquire());


	DIDEVCAPS caps;
	caps.dwSize = sizeof(caps);
	g_pMouse->GetCapabilities(&caps);
		
	if (!SYS_DXTRACE(g_pMouse->GetCapabilities(&caps)))
	{
		sMOU->numButtons = caps.dwButtons;
		sMOU->numAxes = caps.dwAxes;
	}
	sMOU->numControllers = 1;
	return 0;
}

static void Release(void)
{
	if (g_pMouse)
	{
		g_pMouse->Unacquire();
		g_pMouse->Release();
		g_pMouse = NULL;
	}
}


static unsigned long Update(void *device)
{
	if (!g_pMouse)
		return 0;

	HRESULT hr = g_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &sState);

	if (SYS_DXTRACE(hr))
	{
		if (DIERR_INPUTLOST == hr)
		{
			SYS_DXTRACE(g_pMouse->Acquire());
		}
	}

	sMOU->lX = sState.lX;
	sMOU->lY = sState.lY;
	sMOU->lZ = sState.lZ;

	sysMemCpy(sMOU->steButtons, sMOU->rgbButtons, sMOU->numButtons);
	sysMemCpy(sMOU->rgbButtons, sState.rgbButtons, sMOU->numButtons);

	POINT point;
	GetCursorPos(&point);

	if (point.x < 0) 
		point.x = 0;

	if (point.y < 0)
		point.y = 0;

	sMOU->x = point.x;
	sMOU->y = point.y;

	return 0;
}


_RLXEXPORTFUNC MSE_ClientDriver *MSE_SystemGetInterface_STD(void)
{
	static MSE_ClientDriver driver = {
		Open, 
		Release,
		Show,
		Hide, 
		SetPosition,
		Update
	};
	sMOU = &driver;
    return &driver;
}


