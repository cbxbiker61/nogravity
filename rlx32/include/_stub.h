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
#ifndef __STUB_H
#define __STUB_H

__extern_c
	_RLXEXPORTFUNC    int    RLXAPI STUB_OsCustom(char *parms);
	_RLXEXPORTFUNC    void   RLXAPI STUB_Default(void);
	_RLXEXPORTFUNC    void   RLXAPI STUB_Warning(char *, ...);
	_RLXEXPORTFUNC    int    RLXAPI STUB_TaskControl(void);
	_RLXEXPORTFUNC    void   RLXAPI STUB_QuitRequested(void);
	_RLXEXPORTFUNC    void   RLXAPI STUB_ReadyToRun(void);
	_RLXEXPORTFUNC    void   RLXAPI STUB_CheckUp(void *);
	_RLXEXPORTFUNC    void   RLXAPI STUB_StartUp(void);
	_RLXEXPORTFUNC    void   RLXAPI STUB_MainCode(void);
	_RLXEXPORTFUNC    void   RLXAPI STUB_Initialize(void);
	_RLXEXPORTFUNC    int    RLXAPI STUB_OsStartup(char *parms);	
	_RLXEXPORTFUNC    void   RLXAPI STUB_Down(void);
__end_extern_c

#endif

