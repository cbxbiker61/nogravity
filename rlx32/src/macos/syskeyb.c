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
#include <ctype.h>
#include <stdlib.h>
#include "_rlx32.h"
#include "systools.h"
#include "sysctrl.h"

static const uint16_t KEYB_US[] =
{
		'a','s','d','f','g','h','z','x','c' ,'v','ý' ,'ý','q','w','e','r',
		'y','t','1','2','3','4','6','5','=' ,'9','7' ,'-','8','0',']','o',
		'u','[','i','p', 13,'l','j','"','k' ,'{','\\',',','/','n','m','.',
		't',' ','~', 8 ,'ý', 27,'ý','ý','/' ,'ý','ý' ,'ý','ý','ý','ý','ý',
		'ý','.','ý','*','ý','+','ý','ý','ý' ,'ý','ý' ,'/',13 ,'ý','-','ý',
		'ý','/','-','ý','ý','ý','-','5','ý' ,'ý','ý' ,'ý','ý','0','ý','ý',
		'ý','ý','ý','ý','ý','ý','ý','ý','ý' ,'ý','ý' ,'ý','ý','ý','ý','ý',
		'ý','ý','0','ý','ý','ý','ý','ý','ý' ,'ý','ý' ,'ý','ý','ý','ý','ý',
		'ý'};


static unsigned long Update(void * device)
{
 	int i;
	sysMemCpy(sKEY->steButtons, sKEY->rgbButtons, sizeof(sKEY->steButtons));
#if TARGET_API_MAC_CARBON
	GetKeys((SInt32 *)sKEY->rgbButtons);
#else
	GetKeys((UInt32 *)sKEY->rgbButtons);
#endif
	for (i=0;i<16*8;i++)
	{
		if (sKEY_IsHeld(i))
      		sKEY->scanCode = i;
    }
    return 1;
}

static int Open(void *hwnd)
{
	return TRUE;
}

static void Release(void)
{
    return;
}

static char *NameScanCode(int scan)
{
	static char d[32];
	char *dest = d;
	*dest = 0;
	switch(scan)
	{
		 case 0:			sysStrCpy(dest,"<NONE>");break;
		 case s_esc:		sysStrCpy(dest,"ESC"); break;
		 case s_ctrl: 		sysStrCpy(dest,"CTRL"); break;
		 case s_tab:		sysStrCpy(dest,"TAB"); break;
		 case s_alt:		sysStrCpy(dest,"ALT"); break;
		 case s_capslock: 	sysStrCpy(dest,"CAPSL"); break;
		 case s_home: 		sysStrCpy(dest,"HOME"); break;
		 case s_end:		sysStrCpy(dest,"END"); break;
		 case s_space:		sysStrCpy(dest,"SPACE"); break;
		 case s_backspace:	sysStrCpy(dest,"BACKSPC"); break;
		 case s_leftshift:	sysStrCpy(dest,"SHIFT"); break;
		 case s_up:			sysStrCpy(dest,"UP"); break;
		 case s_left: 		sysStrCpy(dest,"LEFT"); break;
		 case s_right:		sysStrCpy(dest,"RIGHT"); break;
		 case s_down: 		sysStrCpy(dest,"DOWN"); break;
		 case s_numup:		sysStrCpy(dest,"NUMUP"); break;
		 case s_numleft:	sysStrCpy(dest,"NUMLEFT"); break;
		 case s_numright: 	sysStrCpy(dest,"NUMRIGHT"); break;
		 case s_numdown:	sysStrCpy(dest,"NUMDOWN"); break;
		 case s_pagedown: 	sysStrCpy(dest,"PAGEDN"); break;
		 case s_pageup:		sysStrCpy(dest,"PAGEUP"); break;
		 case s_numpagedown:sysStrCpy(dest,"NUMPAGEDN"); break;
		 case s_numpageup:	sysStrCpy(dest,"NUMPAGEUP"); break;
		 case s_insert:		sysStrCpy(dest,"INS"); break;
		 case s_delete:		sysStrCpy(dest,"DEL"); break;
		 case s_numinsert:	sysStrCpy(dest,"NUMINS"); break;
		 case s_numdelete:	sysStrCpy(dest,"NUMDEL"); break;

		 default:
		 {
			 dest[0] = KEYB_US[scan];
			 dest[1] = 0;
		 }
		 break;
  }
  return d;
}

static KEY_ClientDriver KeybClientMac = {
    Open,
    Release,
    NameScanCode,
    Update
};

_RLXEXPORTFUNC KEY_ClientDriver *KEY_SystemGetInterface_STD(void)
{
    return &KeybClientMac;
}

