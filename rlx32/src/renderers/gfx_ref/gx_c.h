#pragma once
//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2004 - realtech VR

This file is part of Space Girl 1.9

Space Girl is free software; you can redistribute it and/or
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

__extern_c

void GX_GetSpriteInterfaceRef32(struct GXSYSTEM *pGX, GXSPRITEINTERFACE *p);
void GX_GetSpriteInterfaceRef16(struct GXSYSTEM *pGX, GXSPRITEINTERFACE *p);
void GX_GetSpriteInterfaceRef8(struct GXSYSTEM *pGX, GXSPRITEINTERFACE *p);

void GX_GetGraphicInterfaceRef32(struct GXSYSTEM *pGX, GXGRAPHICINTERFACE *p);
void GX_GetGraphicInterfaceRef16(struct GXSYSTEM *pGX, GXGRAPHICINTERFACE *p);
void GX_GetGraphicInterfaceRef8(struct GXSYSTEM *pGX, GXGRAPHICINTERFACE *p);

__end_extern_c

