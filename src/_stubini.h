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

#define __STUB_RLX
#define __STUB_CDROM
#define __STUB_INPUT
#define __STUB_GX
#define __STUB_SOUND
#define __STUB_RES
#define __STUB_V3X
#define __STUB_QUIET

#define APP_NAME "No Gravity"
#define STUB_SIG "application/x-vnd.Realtech-nogravity"
#if (defined INITGUID && defined WIN32)
#include <dplay.h>
     DEFINE_GUID(STUB_GUID, 0xdbd83641, 0xe44, 0x11d1, 0xa3, 0x9, 0x0, 0xa0, 0x24, 0x5a, 0x36, 0x78);
#endif

