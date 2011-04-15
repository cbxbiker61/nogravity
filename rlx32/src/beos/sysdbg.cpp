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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <interface/Alert.h>
#include <Application.h>
#include "_rlx32.h"
#include "_rlx.h"

void SYS_Debug(const char *fmt, ...)
{
    FILE *fp = fopen("/boot/debug.txt", "a+t");
	char buffer[8192];

    va_list argptr;
    va_start(argptr, fmt);
	vsprintf(buffer, fmt, argptr);

	if (fp)
        fprintf(fp, "%s\n", buffer);

	printf("%s\n", buffer);

    va_end(argptr);

	if (fp)
		fclose(fp);
}

void SYS_Msg(const char *fmt, ...)
{
    char buffer[8192];
    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(buffer, fmt, argptr);
    va_end(argptr);
    SYS_Debug(buffer);

	BAlert *box;
	if (*buffer == '*')
	{
		box = new BAlert("",
							buffer+1,
							"Abort",
							"Retry",
							"Ignore",
							B_WIDTH_FROM_WIDEST,
							B_STOP_ALERT);

		if (box)
		{
			switch(	box->Go() )
			{
				case 0:
					exit(-1);
				break;
				case 1:
					exit(-1);
				break;
			}
		}
	}
	else
	{
		box = new BAlert("",
			*buffer == '!' ? buffer + 1 : buffer,
			"Ok",
			NULL,
			NULL,
			B_WIDTH_FROM_WIDEST,
			( *buffer == '!' ? B_WARNING_ALERT : B_INFO_ALERT)

		);
		if (box)
		{
			box->Go();
		}
	}
}

void SYS_Error(const char *fmt, ...)
{
	char tmp[1024];
    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(tmp, fmt, argptr);
    va_end(argptr);
    SYS_Debug(tmp);
	SYS_Msg("*%s\nPress Retry to debug application\n",tmp);
}

