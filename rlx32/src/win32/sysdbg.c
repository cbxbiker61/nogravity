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

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include "_rlx32.h"
#include "_rlx.h"

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void SYS_Msg(char *fmt, ...)
*
* DESCRIPTION :
*
*/
void SYS_Msg(char *fmt, ...)
{
    char buffer[8192];
    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(buffer, fmt, argptr);
    va_end(argptr);
    MessageBox(GetActiveWindow(), buffer, "Warning", MB_OK);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void SYS_Debug(char *fmt, ...)
*
* DESCRIPTION :
*
*/
void SYS_Debug(char *fmt, ...)
{
    FILE *in;
    in = fopen("debug.txt", "a+t");
    if (in)
    {
        char buffer[8192];
        va_list argptr;
        va_start(argptr, fmt);
        vsprintf(buffer, fmt, argptr);
        fprintf(in, "%s", buffer);
		OutputDebugString(buffer);
        va_end(argptr);
        fclose(in);
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void SYS_Error(char *fmt, ...)
*
* DESCRIPTION :
*
*/
void SYS_Error(char *fmt, ...)
{
    char buffer[8192];
    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(buffer, fmt, argptr);
	strcat(buffer, "\n");
	OutputDebugString(buffer);
    va_end(argptr);
    MessageBox(GetActiveWindow(), buffer, "Error", MB_OK|MB_ICONERROR);
}
