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
#ifndef __GX_CDC_H
#define __GX_CDC_H

typedef struct  {
    char        manufacturer;
    char        version;
    char        encoding;
    char        bits_per_pixel;
    short int   xmin, ymin;
    short int   xmax, ymax;
    short int   hres, vres;
    char        palette[48];
    char        reserved;
    char        colour_planes;
    short int   bytes_per_line;
    short int   palette_type;
    char        filler[58];
}PCX_Header;

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

typedef struct tagBITMAPFILEHEADER {
    short       bfType;
    short       bfSize_L;
    short       bfSize_H;
    short       bfReserved;
    short       bfReserved2;
    short       bfOffBits_L;
    short       bfOffBits_H;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    u_int32_t       biSize;
    int32_t        biWidth;
    int32_t        biHeight;
    short       biPlanes;
    short       biBitCount;
    u_int32_t       biCompression;
    u_int32_t       biSizeImage;
    int32_t        biXPelsPerMeter;
    int32_t        biYPelsPerMeter;
    u_int32_t       biClrUsed;
    u_int32_t       biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagTGAMAPINFOHEADER {
    u_int8_t       ID_Length;
    u_int8_t       cmap_type; // 0: no palette, 1: has palette
    u_int8_t       image_type;
    u_int8_t       cmap_offset_H;
    u_int8_t       cmap_offset_L;
    u_int8_t       cmap_lenght_H;
    u_int8_t       cmap_lenght_L;
    u_int8_t       cmap_depth;
    u_int16_t      image_X;
    u_int16_t      image_Y;
    u_int16_t      image_width;
    u_int16_t      image_height;
    u_int8_t       image_depth;
    u_int8_t       image_descr;
}TGAMAPINFOHEADER;

typedef struct {
    char          sig[6];
    u_int16_t        screenwidth, screendepth;
    unsigned char flags, background, aspect;
}GIF_TagInfo;

typedef struct {
    u_int16_t        left, top, width, depth;
    unsigned char flags;
}GIF_Header;

typedef struct _3dm_header {
    char        nom[4];
    int32_t        version;
    int32_t        handle;
    int32_t        size;
    int32_t        type;
    struct      _3dm_header *prev;
    struct      _3dm_header *next;
    u_int32_t       palette[256];
}C3DM_Header;

typedef struct {
    short       w, h;
    short       x, y;
    u_int8_t       nPlanes;
    u_int8_t       masking;
    u_int8_t       compression;
    u_int8_t       pad1;
    short       transparentColor;
    u_int8_t       xAspect, yAspect;
    short       pageWidth, pageHeight;
} IFFHeader;

enum {
    mskNone, 
    mskHasMask, 
    mskHasTransparentColor, 
    mskLasso
};
enum {
    cmpNone, 
    cmpByteRun1
};

typedef struct {
    u_int8_t *destx, *savedestx;
    u_int8_t *dest;
}LBM_IFFTagInfo;

__extern_c

u_int8_t *PNG_unpack(SYS_FILEHANDLE fp, u_int32_t *lx, u_int32_t *ly, int *bpp, rgb24_t *ColorTable, int options);
void   PNG_pack(SYS_FILEHANDLE fp, u_int8_t *buf, int32_t lx, int32_t ly, int32_t bpp, rgb24_t *ColorTable);
u_int8_t *JPG_unpack(SYS_FILEHANDLE fp, u_int32_t *lx, u_int32_t *ly, int *bpp, rgb24_t *ColorTable, int options);
void   JPG_pack(SYS_FILEHANDLE fp, u_int8_t *buf, int32_t lx, int32_t ly, int32_t bpp, rgb24_t *ColorTable);

__end_extern_c

typedef struct {
    char      *ext;
    u_int8_t   RLXAPI *(*unpack)(SYS_FILEHANDLE in, u_int32_t *lx, u_int32_t *ly, int *bpp, rgb24_t *table, int options);
    void    (* RLXAPI pack)(SYS_FILEHANDLE fp, u_int8_t *buf, int32_t lx, int32_t ly, int32_t bpp, rgb24_t *ColorTable);
}IMG_Codec;

__extern_c

_RLXEXPORTDATA  extern SYS_FILEHANDLE (*iff_fopen)(char const *, char const *);
_RLXEXPORTDATA  extern IMG_Codec IMG_FormList[];

__end_extern_c

#endif
