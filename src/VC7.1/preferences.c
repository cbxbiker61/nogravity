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
#include "_rlx32.h"
#include "_rlx.h"
#include "_stub.h"

int STUB_OsStartup(char *parms)
{
	HKEY hKey;
	int uSize;
	if (RegOpenKeyEx(HKEY_CURRENT_USER,
					 ("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"),
					 0,
					 KEY_QUERY_VALUE,
					 &hKey) == ERROR_SUCCESS)
	{
		DWORD type;
		RegQueryValueEx(hKey, "Personal", NULL, &type, (BYTE*)RLX.IniPath, &uSize);
		strcat(RLX.IniPath, "\\No Gravity Savegames\\");
		CreateDirectory(RLX.IniPath, NULL);
		RegCloseKey(hKey);
		return 0;
	}
    return 1;
}

int STUB_OsCustom(char *parms)
{
    return 1;
}


