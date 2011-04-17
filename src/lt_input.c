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
#define NG_EXPORT
#include <stdlib.h>
#include <stdio.h>
#include "_rlx32.h"
#include "sysctrl.h"
#include "systools.h"
#include "gx_init.h"
#include "gx_rgb.h"
#include "sysresmx.h"
#include "gx_struc.h"
#include "gx_csp.h"
#include "gx_flc.h"
#include "v3xdefs.h"
#include "v3x_2.h"
#include "v3xcoll.h"
#include "v3xrend.h"
#include "v3xmaps.h"
#include "v3xscene.h"

#include "gui_os.h"
#include "iss_defs.h"
#include "iss_fx.h"
//
#include "lt_struc.h"
#include "lt_data.h"
#include "lt_func.h"
#include "_stub.h"

#ifndef __amigaos__
#define USE_MUTEX
extern SYS_MUTEX m_Mutex;
#endif

int NG_WaitForKeyPress(void)
{
    int a = 0;

#ifdef USE_MUTEX
	mutex_lock(&m_Mutex);
#endif

    V3XA.Client->Poll(0);
    V3XAStream_PollAll();

#ifdef USE_MUTEX
	mutex_unlock(&m_Mutex);
#endif

	sKEY->Update(0);
	sJOY->Update(0);
	sMOU->Update(0);

	if (sKEY_IsClicked(s_esc) || sJOY_IsClicked(1) || sMOU_IsClicked(1))
		a|=2;

    if (sKEY_IsHeld(s_return) || sJOY_IsClicked(0) || sMOU_IsClicked(0))
		a|=1;

    return a;
}

void NG_WaitForKeyWithDelay(int dt)
{
    uint32_t t = timer_sec() + dt;

	while(STUB_TaskControl() == 0)
    {
		if (NG_WaitForKeyPress())
			break;

		if (timer_sec()>=t)
			break;
    }
}

uint8_t SGJOY_ButtonKeys[128];
char SGPOV_ButtonKeys[128];
char SGMOU_ButtonKeys[128];

int NG_SampleButtonJoystick()
{
    int singleButton = 0;
	int i;
    for (i=0;i<sJOY->numButtons;i++)
    {
		if (sJOY_IsClicked(i))
            singleButton = i+1;
    }
    return singleButton;
}

int NG_SampleButtonMouse()
{
    int singleButton = 0;
	int i;
	for (i=0;i<sMOU->numButtons;i++)
    {
        if (sMOU_IsClicked(i))
            singleButton = i+1;
    }
    return singleButton;
}

#define deadZone     16382L
#define isjoyright  (sJOY->lX>32767L+deadZone)
#define isjoyleft   (sJOY->lX<32767L-deadZone)
#define isjoyup     (sJOY->lY<32767L-deadZone)
#define isjoydown   (sJOY->lY>32767L+deadZone)

#define deadZone2    10382L
#define isjoyrup    (sJOY->throttle>32767L+deadZone2)
#define isjoyrdown  (sJOY->throttle<32767L-deadZone2)
#define isjoyzup    (sJOY->z<32767L-deadZone2)
#define isjoyzdown  (sJOY->z>32767L+deadZone2)

void SGJOY_ReadAxis(int *lpAxisX, int *lpAxisY, int *lpAxisRoll, int *lpAxisThrottle, int *status)
{
	int lZ = 0, lRz = 0;
	*status = 0;


	*lpAxisX = sJOY->lX - 32768;
	if (abs(*lpAxisX)>deadZone2)
	{
		*status|=1;
		if (*lpAxisX < 0)
			(*lpAxisX) += deadZone2;
		else
			(*lpAxisX) -= deadZone2;
	}
	else
		*lpAxisX = 0;

	*lpAxisY = sJOY->lY - 32768;
	if (abs(*lpAxisY)>deadZone2)
	{
		*status|=2;
		if (*lpAxisY < 0)
			(*lpAxisY) += deadZone2;
		else
			(*lpAxisY) -= deadZone2;
	}
	else
		*lpAxisY = 0;

	switch(g_SGSettings.AxisRoll)
	{
		case 0:	lZ = 0; break;
		case 1:	lZ = sJOY->lZ; break;
		case 2:	lZ = sJOY->lRx; break;
		case 3:	lZ = sJOY->lRy; break;
		case 4:	lZ = sJOY->lRz; break;
	}

	switch(g_SGSettings.AxisThrottle)
	{
		case 0:	lRz = 0; break;
		case 1:	lRz = sJOY->lZ; break;
		case 2:	lRz = sJOY->lRx; break;
		case 3:	lRz = sJOY->lRy; break;
		case 4:	lRz = sJOY->lRz; break;
	}

	if (g_SGSettings.AxisRoll)
	{
		*lpAxisRoll = lZ - 32768;
		*status|=4;
	}

	if (g_SGSettings.AxisThrottle)
	{
		*lpAxisThrottle = 65535 - lRz;
		*status|=8;
	}
}

void SGJOY_MapKeyboard()
{
    // Remap buttons
    int i;
    uint8_t *buttonKeys = SGJOY_ButtonKeys;
    for (i=0;i<sJOY->numButtons;i++)
    {
       uint8_t b = SGJOY_ButtonKeys[i];
       uint8_t v = sJOY_IsHeld(i)?0x80:0;
	   if (!sKEY_IsHeld(b))
			SKEY_SET_BIT(sKEY->rgbButtons, b, v);
        buttonKeys++;
    }
    SKEY_SET_BIT(sKEY->rgbButtons, LK_UP, isjoyup);
    SKEY_SET_BIT(sKEY->rgbButtons, LK_DOWN, isjoydown);
    SKEY_SET_BIT(sKEY->rgbButtons, LK_LEFT, isjoyleft);
    SKEY_SET_BIT(sKEY->rgbButtons, LK_RIGHT, isjoyright);
}

void SGMOU_MapKeyboard()
{
    int i;
	for (i=0;i<sMOU->numButtons;i++)
    {
        uint8_t b = SGMOU_ButtonKeys[i];
		uint8_t v = sMOU_IsHeld(i) ? 0x80 : 0;
	    if (!sKEY_IsHeld(b))
		{
			SKEY_SET_BIT(sKEY->rgbButtons, b, v);
		}
    }
}

void NG_ResetKey(void)
{
    int i;
    if (!sJOY)
		return;

    sysMemZero(SGJOY_ButtonKeys, 32);

	for (i=0;i<LK_MAX;i++)
    {
        if (g_SGSettings.joy[i])
        {
            SGJOY_ButtonKeys[g_SGSettings.joy[i]-1] = g_SGSettings.key[i];
        }
    }

	sysMemZero(SGMOU_ButtonKeys, 32);

	for (i=0;i<LK_MAX;i++)
    {
		if (g_SGSettings.mou[i])
        {
            SGMOU_ButtonKeys[g_SGSettings.mou[i]-1] = g_SGSettings.key[i];
        }
    }

    for (i=0;i<4;i++)
    {
        if (g_SGSettings.joyPOV[i])
	        SGPOV_ButtonKeys[ g_SGSettings.joyPOV[i]-1] = g_SGSettings.key[i];
    }
}

