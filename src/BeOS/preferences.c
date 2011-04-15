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
#include <Be.h>
#include <Directory.h>
#include <FindDirectory.h>
#include <Path.h>
#include <stdio.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "_stub.h"

int STUB_OsStartup(char *parms)
{
	BPath path;
	app_info info;
	// Change to current folder
	if (be_app->GetAppInfo(&info)>= B_NO_ERROR)
	{
		BEntry entry(&info.ref);
		if (entry.GetPath(&path)>=B_NO_ERROR)
		if (path.GetParent(&path)>=B_NO_ERROR)
		{
			chdir(path.Path());
		}
	}

    //if (find_directory((directory_which)B_USER_SETTINGS_DIRECTORY, &path, false, NULL) != B_NO_ERROR)
    path.SetTo("/boot/home/config/settings");

	sprintf(RLX.IniPath, "%s/Realtech/", path.Path());

    return 0;
}

int STUB_OsCustom(char *parms)
{
    return 1;
}

