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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "_rlx32.h"
#include "_rlx.h"
#include "systools.h"
#include "sysresmx.h"

static int file_size(SYS_FILEHANDLE stream)
{
    int32_t curpos, length;
    curpos = FIO_std.ftell(stream);
    FIO_std.fseek(stream, 0L, SEEK_END);
    length = FIO_std.ftell(stream);
    FIO_std.fseek(stream, curpos, SEEK_SET);
    return length;
}

SYS_FILEIO FIO_std = { fopen, fclose, fseek, fread, fgetc, fwrite, ftell, feof, fgets, file_size, file_exists };

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/

int	GX_GetDesktopSize(int *lx, int *ly, int *bpp) // Get desktop resolution
{
    RECT rcWork;
	HWND hwnd = GetDesktopWindow();
    HDC hDC = GetDC(hwnd);
 	SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWork, 0 );
    *lx = rcWork.right - rcWork.left;
    *ly = rcWork.bottom - rcWork.top;    
 	*bpp = GetDeviceCaps(hDC, BITSPIXEL);
	ReleaseDC(hwnd, hDC);
	return 0;
}

