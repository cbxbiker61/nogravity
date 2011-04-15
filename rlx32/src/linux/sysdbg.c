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
#include "_rlx32.h"
#include "_rlx.h"

void SYS_Msg(const char *fmt, ...)
{
    char buffer[4096];
    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, argptr);
    printf("%s\n", buffer);
    va_end(argptr);
}

void SYS_Debug(const char *fmt, ...)
{
	char buffer[2048];
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, argptr);
	printf( "%s", buffer);
	va_end(argptr);
}

void SYS_Error(const char *fmt, ...)
{
    char buffer[2048];
    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, argptr);
    printf("%s\n", buffer);
    va_end(argptr);
    exit(-1);
}

