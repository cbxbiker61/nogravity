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
#include "sysctrl.h"
#include "systools.h"
#include "beos/stub.h"

static int Open(void *hwnd)			// Return non-zero if error
{
	sMOU->numControllers = 1;
	sMOU->numButtons = 2;
	sMOU->numAxes = 2;
	return 0;
}

static void Release(void)
{
	return;
}

static void Show()
{
    return;
}

static void Hide()
{
    return;
}

static void SetPosition(u_int32_t x, u_int32_t y)
{
	sMOU->x = x;
	sMOU->y = y;
	return;
}

static unsigned long CALLING_STD Update(void *device)
{
   BWindow *w = sysApplication::GetInstance()->m_hWnd;
   BView *v = sysApplication::GetInstance()->m_hView;
   if (w->Lock())
   {
       if (w->IsActive())
       {
            BPoint point;
            uint32 button;
	 		v->GetMouse(&point, &button);
  		    sMOU->lX = (int)point.x - sMOU->x;
		    sMOU->lY = (int)point.y - sMOU->y;
	  	 	sMOU->x = (int)point.x;
	   		sMOU->y = (int)point.y;
	   		sysMemCpy(sMOU->steButtons, sMOU->rgbButtons, sizeof(sMOU->steButtons));
	 	 	sMOU->rgbButtons[0] =  button&1;
	 	    sMOU->rgbButtons[1] = (button>>1)&1;
	  	    sMOU->rgbButtons[2] = (button>>2)&1;
       }
   }
   
   return 0;
}

static MSE_ClientDriver MouseClientBeOS =
{
	Open,
	Release,
	Show,
	Hide,
	SetPosition,
	Update
};

_RLXEXPORTFUNC MSE_ClientDriver *MSE_SystemGetInterface_STD(void)
{
    sMOU = &MouseClientBeOS;
	return sMOU;
}
