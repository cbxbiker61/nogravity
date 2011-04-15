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
#define DX5_BLEND
#define V3XRCLASS_WIRED2 1024
#define D3DMAX_TMU  2

typedef struct {
    DDPIXELFORMAT	pfd;
    uint8_t	bPalettized, IndexBPP;
    uint8_t	RedMaskSize, RedFieldPosition;
    uint8_t	BlueMaskSize, BlueFieldPosition;
    uint8_t	GreenMaskSize, GreenFieldPosition;
    uint8_t	RsvdMaskSize, RsvdFieldPosition;
}D3D_TextureFormat;

typedef struct {
    char	szDeviceDesc[64];
    char	szDeviceName[64];
    GUID	guidDevice;
    D3DDEVICEDESC	d3dSWDeviceDesc;
    D3DDEVICEDESC	d3dHWDeviceDesc;
    DDPIXELFORMAT	ddpfZBuffer;
    LPD3DDEVICEDESC	desc;
    BOOL	fIsHardware;
    BOOL	bIsPrimary;
    int	NumTextureFormats;
    D3D_TextureFormat		TextureFormat[63];
}D3DHARDRIVER;

typedef struct {
    LPDIRECT3DTEXTURE2	lpTexture;
    LPDIRECTDRAWSURFACE4	lpTextureSurf;
    LPDIRECTDRAWSURFACE4	lpSurface3D;
    uint32_t	offsetSwap;
    unsigned	option;
}V3XD3DHANDLE;

typedef struct _D3DTLVERTEX2 {
    union {
        D3DVALUE    sx;             /* Screen coordinates */
        D3DVALUE    dvSX;
    };
    union {
        D3DVALUE    sy;
        D3DVALUE    dvSY;
    };
    union {
        D3DVALUE    sz;
        D3DVALUE    dvSZ;
    };
    union {
        D3DVALUE    rhw;            /* Reciprocal of homogeneous w */
        D3DVALUE    dvRHW;
    };
    union {
        D3DCOLOR    color;          /* Vertex color */
	D3DCOLOR    dcColor;
    };
   union {
        D3DCOLOR    specular;       /* Specular component of vertex */
        D3DCOLOR    dcSpecular;
    };
    union {
        D3DVALUE    tu;             /* Texture coordinates */
        D3DVALUE    dvTU;
    };
    union {
        D3DVALUE    tv;
        D3DVALUE    dvTV;
    };
    union {
        D3DVALUE    tu2;            /* Texture coordinates 2 */
        D3DVALUE    dvTU2;
    };
    union {
        D3DVALUE    tv2;
        D3DVALUE    dvTV2;
    };
} D3DTLVERTEX2, *LPD3DTLVERTEX2;
/*
* Direct3D interfaces
*/
typedef struct {
    D3DVALUE    x;
    D3DVALUE    y;
    D3DVALUE    z;
    D3DVALUE    oow;
    D3DCOLOR    color;
    D3DCOLOR    specular;
    D3DVALUE    u1;
    D3DVALUE    v1;
    D3DVALUE    u2;
    D3DVALUE    v2;
}D3D_VERTEX2;

__extern_c

 _RLXEXPORTFUNC void RLXAPI V3XDRV_Load_D3D(void);
 _RLXEXPORTFUNC void  D3D_TextureSwapReset(void);
 _RLXEXPORTFUNC uint32_t D3D_TextureSwapPut(uint8_t *buffer, uint32_t size);
 _RLXEXPORTFUNC void  D3D_TextureSwapGet(uint8_t *dest, uint32_t offset, uint32_t size);

#define D3DFVF_TLVERTEX2 ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | (2<<D3DFVF_TEXCOUNT_SHIFT))

__end_extern_c

extern  V3X_GXSystem   V3X_Direct3D;

