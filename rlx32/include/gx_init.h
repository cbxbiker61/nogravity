#pragma once
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
#ifndef _GX_INIT_HH
#define _GX_INIT_HH

struct RLXSYSTEM;
struct GXSYSTEM;
struct _gx_viewport;

#ifdef __BEOS__
	#ifdef V3X_EXPORT_DLL
	#define _V3XEXPORTFUNC __declspec(dllexport)
	#else
	#define _V3XEXPORTFUNC __declspec(dllimport)
	#endif
#else
	#define _V3XEXPORTFUNC
#endif

__extern_c

_RLXEXPORTFUNC	  void  RLXAPI  GX_KernelAlloc();
_RLXEXPORTFUNC    void  RLXAPI  GX_SetupViewport(struct _gx_viewport *pView, int x, int y, int bpp);
_RLXEXPORTFUNC    void  RLXAPI  GX_SetupDefaultViewport(int x, int y, int bbp);
_RLXEXPORTFUNC    void  RLXAPI  GX_EntryPoint(struct RLXSYSTEM *p);
_RLXEXPORTFUNC	  void  RLXAPI  GX_GetGraphicInterface(struct GXSYSTEM *p);

_V3XEXPORTFUNC    void  RLXAPI  V3X_EntryPoint(struct RLXSYSTEM *p);

__end_extern_c

#endif

