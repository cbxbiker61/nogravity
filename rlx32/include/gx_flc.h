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

#ifndef __GX_FLC_H
#define __GX_FLC_H

// Load format
enum {
    FLI_DIRECTFROMDISK = 0x0,
    FLI_USEMEMORY  = 0x1,
    FLI_LZWPACKED  = 0x2,
    FLI_EXPANDED = 0x4};

// Play status
enum {
    FLX_ISPLAYING  = 0x1,
    FLX_LOOPANIMATION = 0x2,
    FLX_DECOMPRESSFRAME = 0x4,
    FLX_RESETPALETTE = 0x8,
    FLX_INSurfaces = 0x10,
    FLX_DOUBLEBUFFERING = 0x20,
    FLX_FORMARD1FRAME = 0x40,
    FLX_EXPANDED = 0x80};

// FLC Header
typedef struct{
    int32_t        size;
    uint16_t      type;
    uint16_t      Chunks;
    uint16_t      width;
    uint16_t      height;
    uint16_t      bits_a_pixel;
    short       flags;
    int32_t        speed;
    short       reserved;
    int32_t        created;
    int32_t        creator;
    int32_t        updated;
    int32_t        updater;
    short       aspectx;
    short       aspecty;
    uint8_t       reserved2[38];
    int32_t        frame1;
    int32_t        frame2;
    char        reserved3[40];
}FLC_FileHeader;  // FIXME : pas tres align‚

typedef union {
    FLC_FileHeader Struct;
    char        Raw[128];
}FLC_Header;

// FLC Handle
typedef struct _fli_struct {
    SYS_FILEHANDLE       fli_stream;
    FLC_Header  Header;

    uint8_t      *fileBuffer;
    uint8_t      *decompBuffer;

    GXSPRITE      bitmap;
    uintptr_t       start;
    uintptr_t       start_buf;
    uint32_t       LastTime;
    uint16_t      CurrentFrame;
    uint16_t      MaximumFrame;
    uint8_t       Flags;
    uint8_t       ReadMode;
    uint8_t       filler[2];
	GXSPRITE     *frames;
	rgb24_t	    ColorTable[256];

}FLI_STRUCT;


// Functions
__extern_c
_RLXEXPORTFUNC    FLI_STRUCT RLXAPI *FLI_Open(SYS_FILEHANDLE in, int mode);
_RLXEXPORTFUNC    void       RLXAPI  FLI_Close(FLI_STRUCT *f);
_RLXEXPORTFUNC    void       RLXAPI  FLI_Unpack(FLI_STRUCT *f);
_RLXEXPORTFUNC    void       RLXAPI  FLI_Rewind(FLI_STRUCT *f);
_RLXEXPORTFUNC    void       RLXAPI  FLI_SetPalette(FLI_STRUCT *fli);
_RLXEXPORTFUNC    void       RLXAPI  FLI_Draw(FLI_STRUCT *a, int xoff, int yoff);
_RLXEXPORTFUNC    GXSPRITEGROUP       RLXAPI *FLI_LoadToSpriteGroup(const char *filename, int diet);

__end_extern_c

#endif

