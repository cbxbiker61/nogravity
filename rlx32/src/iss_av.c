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
#include "_rlx32.h"
#include "systools.h"
#include "systime.h"
#include "sysresmx.h"
#include "gx_csp.h"
#include "gx_flc.h"
#include "iss_av.h"
#include "iss_defs.h"
static int FLZ_time, FLZ_rate, FLZ_mode, FLZ_SkipFrame;
static V3XA_STREAM FLZ_hnd;
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int32_t FLZ_AVCheck(void *data)
*
* DESCRIPTION :
*
*/
int32_t FLZ_AVCheck(void *data)
{
    FLI_STRUCT *fli = (FLI_STRUCT*)data;
    u_int32_t temps = timer_ms();
    if ( ((V3XA.State & 1)) || (temps>=(u_int32_t)(FLZ_time+fli->Header.Struct.speed)) )
    {
		FLI_Unpack(fli);
		FLZ_time = temps;
		return !FLZ_SkipFrame;
    }
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void FLZ_AVStop(FLI_STRUCT *fli)
*
* DESCRIPTION :
*
*/
void FLZ_AVStop(void *fli)
{
    V3XA.Client->StreamSetVolume(FLZ_hnd, 0);
    if ((V3XA.State & 1)) 
		V3XA.Client->StreamRelease(FLZ_hnd);
    FLI_Close((FLI_STRUCT *)fli);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  FLI_STRUCT *FLZ_AVInit(char *filename)
*
* DESCRIPTION :
*
*/
void *FLZ_AVInit(char *filename)
{
    SYS_FILEHANDLE in = FIO_gzip.fopen(filename, "rb");
    FLI_STRUCT  *fli;
    if (!in ) return NULL;
    fli = FLI_Open(in, FLI_LZWPACKED);
    if (!fli) return NULL;
    // fli->Decode = FLZ_AVUpdate;
    FLZ_mode = 0;
    FLZ_time = 0;
    FLZ_rate = 22050;
    return fli;
}
