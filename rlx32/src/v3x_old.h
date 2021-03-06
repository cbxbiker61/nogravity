#pragma once
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

typedef struct _v3x_ori97
{
	uint32_t	mesh;
	uint32_t	morph;
	uint32_t	Cs;
	uint32_t	data;
	V3XSCALAR	global_rayon;
	uint8_t		Type, matrix_Method, index_Color, Flags;
	uint16_t	index_Parent;
	uint8_t		pad[2];
	char		name[16];
	V3XVECTOR	global_pivot;
	char		filler[4+4];
} V3XORI97;

typedef struct _v3x_ovi97
{
	uint32_t	mesh;
	uint32_t	data;
	uint32_t	Tk;
	uint32_t	ORI;
	uint32_t	TVI;
	uint32_t	Parent;
	uint32_t	Child;
	uint32_t	collisionList;
	V3XSCALAR	distance;
	uint16_t	index_OVI, index_ORI, index_TVI;
	uint8_t		Flags_MatrixToUpdate, Flags_MatrixDone;
	uint8_t		Flags_DisplaySphere;
	uint8_t		filler1[3];
	uint8_t		Hide_Never, Hide_ByDisplay, Hide_ByZ, Hide_ByFreeze;
	uint8_t		filler[12];
} V3XOVI97;

typedef struct _v3x_layer97
{
	char		background_name[16];
	uint8_t		filler5[32];
	char		ColorTable_name[16];
	uint8_t		filler6[32+80];
	rgb32_t		SolidColor;
	uint16_t	filler1, length_time, length_max, filler2;
	uint8_t		filler3[2];
	uint8_t		Version, Type;
	rgb24_t		FogColor;
	uint8_t		FogActivate;
	uint8_t		filler4[32];
} V3XLAYER97; //228 octets

typedef struct _v3x_chunk
{
	uint32_t	ckId; // Chunk ID
	uint32_t	ckSize; // Chunk Size
} v3xCHUNK;

#define MAKEID(d, c, b, a) (((int32_t)(a)<<24) | ((int32_t)(b)<<16) | ((int32_t)(c)<<8) | ((int32_t)(d)) )
#define ID_LYLT MAKEID('V','L','L','T')
#define ID_LYZB MAKEID('V','L','Z','B')
#define ID_LYFG MAKEID('V','L','F','G')
#define ID_LYTM MAKEID('V','L','T','M')
#define ID_LYBG MAKEID('V','L','B','G')

