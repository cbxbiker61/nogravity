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
#include <setjmp.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "_stub.h"
#include "sysresmx.h"
#include "systools.h"
#include "systools.h"

static int file_size(SYS_FILEHANDLE stream)
{
    int32_t curpos, length;
    curpos = FIO_std.ftell(stream);
    FIO_std.fseek(stream, 0L, SEEK_END);
    length = FIO_std.ftell(stream);
    FIO_std.fseek(stream, curpos, SEEK_SET);
    return length;
}

SYS_FILEIO FIO_std= {fopen, fclose, fseek, fread, fgetc, fwrite, ftell, feof, fgets, file_size, file_exists};

char *file_searchpath(const char *name)
{
    if (file_exists((char*)name))
		return (char*)name;
    return NULL;
}

