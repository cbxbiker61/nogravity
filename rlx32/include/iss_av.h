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
#ifndef __ISS_AVINCLUDED
#define __ISS_AVINCLUDED

typedef struct _RMovie {
    void *reserved;
}RMOVIE_HANDLE;

__extern_c

_RLXEXPORTFUNC int32_t   FLZ_AVCheck(void *data);
_RLXEXPORTFUNC void   FLZ_AVStop(void *fli);
_RLXEXPORTFUNC void  *FLZ_AVInit(char *filename);

_RLXEXPORTFUNC int  RMovie_New(char *filename, RMOVIE_HANDLE *config);
_RLXEXPORTFUNC int  RMovie_Release(RMOVIE_HANDLE *handle);
_RLXEXPORTFUNC int  RMovie_Start(RMOVIE_HANDLE *handle, int doLoop);
_RLXEXPORTFUNC int  RMovie_Stop(RMOVIE_HANDLE *handle);
_RLXEXPORTFUNC int  RMovie_Poll(RMOVIE_HANDLE *handle, int command);

__end_extern_c

#endif
