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
#ifndef __V3XMAPS_H
#define __V3XMAPS_H

#include "gx_csp.h"

// Texture & material cache system

enum {
   V3XMAT_LOADIMAGE = 0x1, 
   V3XMAT_LOADRGB = 0x2
};

// Advanced 2D sprites system

typedef struct _v3xspriteinfo
{
    unsigned    flags;	   // Material
    rgb32_t  *	color;                  // RGB Color
    V3XSCALAR    x, y, z;            // Screen positions, Z, Capabilities (see below)
    V3XSCALAR    ROTcos, ROTsin;
    union {
       V3XSCALAR    x2;
       int32_t       lx;
    };
    union {
       V3XSCALAR    y2;
       int32_t       ly;
    };    // Angle and size
    V3XPOLY     poly;                   // Pipeline polygone
    V3XMATERIAL mat;
}V3XSPRITEINFO;

typedef struct _v3xsprite
{
    V3XSPRITEINFO	_sp;
    GXSPRITE		sp;
}V3XSPRITE;

typedef struct _v3xspritegroup
{
    GXSPRITEGROUP    spg;
    V3XSPRITEINFO  * item;
    V3XMATERIAL		 mat;
}V3XSPRITEGROUP;

enum {
    V3XRESOURCETYPE_TEXTURE = 0x1,
    V3XRESOURCETYPE_STREAM = 0x2,
    V3XRESOURCETYPE_MATERIAL = 0x3, 
    V3XRESOURCETYPE_TEXTURE2 = 0x4
};


__extern_c

// Texture cache
_RLXEXPORTFUNC    void           RLXAPI    V3XResources_Reset(V3XRESOURCE *bm);
_RLXEXPORTFUNC    V3XRESOURCE_ITEM *  RLXAPI    V3XResources_Put(V3XRESOURCE *bm, const char *filename, void *data, int type);
_RLXEXPORTFUNC    void			 RLXAPI    V3XResources_Purge(V3XRESOURCE *bm, int purge);
_RLXEXPORTFUNC    void           RLXAPI   *V3XResources_Get(V3XRESOURCE *bm, const char *filename, int type);
_RLXEXPORTFUNC    int            RLXAPI    V3XResources_Del(V3XRESOURCE *bm, const char *filename);
_RLXEXPORTFUNC    void           RLXAPI    V3XResources_Animated(V3XRESOURCE *bm);

// Dynamic materials
_RLXEXPORTFUNC    int	         RLXAPI    V3XCache_Material(V3XMATERIAL *Mat, int option);
_RLXEXPORTFUNC    void           RLXAPI    V3XCache_Mesh(V3XMESH *Obj);

// Material upload mechanism
_RLXEXPORTFUNC    V3XMATERIAL    RLXAPI   *V3XMaterials_GetFn(char *filename, int *maxtexture, unsigned flags);
_RLXEXPORTFUNC    V3XMATERIAL    RLXAPI   *V3XMaterials_GetFp(SYS_FILEHANDLE in, int mt);
_RLXEXPORTFUNC    void           RLXAPI    V3XMaterials_LoadFromMesh(V3XMESH *Obj);

// Low level operators
_RLXEXPORTFUNC    V3XMATERIAL    RLXAPI   *V3XMaterials_SearchByName(V3XMATERIAL *mat, char *name, int maxmaterial);

_RLXEXPORTFUNC    void           RLXAPI    V3XMaterial_LoadTextures(V3XMATERIAL *Mat);
_RLXEXPORTFUNC    void           RLXAPI    V3XMaterial_Release(V3XMATERIAL *map, V3XMESH *Obj);
_RLXEXPORTFUNC    void           RLXAPI    V3XMaterial_Register(V3XMATERIAL *mat);
_RLXEXPORTFUNC    V3XMATERIAL    RLXAPI   *V3XMaterial_NewFromPipeline(void);

// V3X'98 GXSPRITE upload methods (singled GXSPRITE)
_RLXEXPORTFUNC    void           RLXAPI    V3X_CSP_GetFn(char *filename, GXSPRITE *item, int load);
_RLXEXPORTFUNC    void           RLXAPI    V3X_CSP_Prepare(GXSPRITE *sp, int flags);
_RLXEXPORTFUNC    int            RLXAPI    V3X_CSP_Set3D(GXSPRITE *item, V3XVECTOR *pos, V3XSCALAR s, unsigned mode);
_RLXEXPORTFUNC    int            RLXAPI    V3X_CSP_Draw(GXSPRITE *sp, int clip);
_RLXEXPORTFUNC    void           RLXAPI    V3X_CSP_Unload(GXSPRITE *item);
_RLXEXPORTFUNC    void           RLXAPI    V3X_CSP_Upload(GXSPRITE *item, int bpp);
_RLXEXPORTFUNC    void           RLXAPI    V3X_CSP_Release(GXSPRITE *sp);

_RLXEXPORTFUNC    void           RLXAPI    V3X_CSP_Initialize(GXSPRITE *item, V3XMATERIAL *mat);

__end_extern_c

#endif
