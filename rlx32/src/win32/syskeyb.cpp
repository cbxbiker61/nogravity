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
#include "systools.h"
#include "sysctrl.h"
#include "win32/sys_di.h"
#include "win32/w32_dx.h"

static LPDIRECTINPUTDEVICE8 g_pKeyboard;
static HINSTANCE g_lpInstance;

static int g_bAcquired = 0;

static unsigned long Update(void *device)
{
	if (!g_pKeyboard)
		return -1;

	sysMemCpy(sKEY->steButtons, sKEY->rgbButtons, 256);
	HRESULT hr = g_pKeyboard->GetDeviceState(256, sKEY->rgbButtons);
	
	if (SYS_DXTRACE(hr))
	{	
		if (DIERR_INPUTLOST == hr)
		{
			SYS_DXTRACE(g_pKeyboard->Acquire());
		}
	}
	
    return 1;
}

static int Open(void *hwnd)
{
	if (SYS_DXTRACE(g_lpDI->CreateDevice(GUID_SysKeyboard, &g_pKeyboard, NULL)))
        return FALSE;

	if (SYS_DXTRACE(g_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
        return FALSE;

    if (SYS_DXTRACE(g_pKeyboard->SetCooperativeLevel((HWND)hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
        return FALSE;

    SYS_DXTRACE(g_pKeyboard->Acquire());

	sKEY->numControllers = 1;

	return TRUE;
}

static void Release(void)
{
    SYS_DXTRACE(g_pKeyboard->Unacquire());
    g_pKeyboard->Release();
    g_pKeyboard = NULL;
    return;
}

static char *NameScanCode(int scan)
{
    static char d[32];
    char *dest = d;
    *dest = 0;
    if (!GetKeyNameText(scan<<16, d, 32)||(d[0]==0))
    {
    	switch(scan) {
    	    case 0: sysStrCpy(dest, "<NONE>");break;
    	    case s_esc: sysStrCpy(dest, "ESC"); break;
    	    case s_ctrl+128:
    	    case s_ctrl: sysStrCpy(dest, "CTRL"); break;
    	    case s_alt:
    	    case s_alt+128:
    	    sysStrCpy(dest, "ALT"); break;
    	    case s_capslock: sysStrCpy(dest, "CAPSL"); break;
    	    case s_home: sysStrCpy(dest, "HOME"); break;
    	    case s_up: sysStrCpy(dest, "UP"); break;
    	    case s_pageup: sysStrCpy(dest, "PAGEUP"); break;
    	    case s_left: sysStrCpy(dest, "LEFT"); break;
    	    case s_right: sysStrCpy(dest, "RIGHT"); break;
    	    case s_end: sysStrCpy(dest, "END"); break;
    	    case s_down: sysStrCpy(dest, "DOWN"); break;
    	    case s_pagedown: sysStrCpy(dest, "PAGEDN"); break;
    	    case s_insert: sysStrCpy(dest, "INS"); break;
    	    case s_delete: sysStrCpy(dest, "DEL"); break;
    	    default:
    	    {
             	dest[0] = sKEY->charCode;
    	       	dest[1] = 0;
            }
             break;
       	}
    }
    return d;
}


_RLXEXPORTFUNC KEY_ClientDriver *KEY_SystemGetInterface_STD(void)
{
	static KEY_ClientDriver driver = {
		Open,
		Release,
		NameScanCode,
		Update
	};
	sKEY = &driver;

    return &driver;
}

