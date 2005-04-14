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
#ifndef __V3XREND_H
#define __V3XREND_H


enum {
    V3XCSPFLAGS_CENTER = 0x1, 
    V3XCSPFLAGS_UPLEFT = 0x4, 
    V3XCSPFLAGS_CHANGESIZE = 0x10
};

enum {
    V3XCSPDRAW_CLIP = 0x1, 
    V3XCSPDRAW_INSTANCE = 0x2, 
    V3XCSPDRAW_INSTMATERIAL = 0x4
};

#define V3XPipeline_Reset() V3X.Buffer.MaxFaces = V3X.Buffer.MaxMat = V3X.Buffer.MaxClipped = 0;

// Rasterizer caps

enum {
      V3XTEXDWNOPTION_STATIC = 0x1, // Download a static texture      
      V3XTEXDWNOPTION_COLORKEY = 0x4, // Apply colorkeying
      V3XTEXDWNOPTION_TMU2 = 0x8, // Load in the second TMU
      V3XTEXDWNOPTION_3DSURFACE = 0x10, 
      V3XTEXDWNOPTION_REFRESH = 0x20, 
	  V3XTEXDWNOPTION_DYNAMIC = 0x1000, // Download a dynamic texture
};

enum {
      GXSPEC_ZBUFFER =0x1,   // Driver can z-buffering
      GXSPEC_OPACITYTRANSPARENT=0x2,   // Driver can draw opacity & transparent textured poly
      GXSPEC_SPRITEAREPOLY =0x4,   // Sprite are polygones (not scalable GXSPRITE)
      GXSPEC_XYCLIPPING =0x8,   // Driver can perform XY Clipping
      GXSPEC_UVNORMALIZED =0x10,  // UV coordinates must be normalized [0, 1]
      GXSPEC_RESIZABLEMAP =0x20,  // Textures can be resizable at any size
      GXSPEC_ALPHABLENDING_ADD =0x40,  // Driver can do additive blending
      GXSPEC_ALPHABLENDING_MID =0x80,  // Driver can do 50% blending
      GXSPEC_FOG =0x100, // Driver can do fog
      GXSPEC_ENABLECOMPRESSION =0x200, // Enable tex compression
      GXSPEC_HARDWAREBLENDING =0x400, // Driver can do color blending
      GXSPEC_FORCEHWPERSPECTIVE=0x800, // Force perspective correction   
      GXSPEC_ENABLEFILTERING =0x1000, // Enable bilinear filtering  
      GXSPEC_HARDWARE =0x2000, // Driver is an hardware
      GXSPEC_ENABLEPERSPECTIVE =0x4000, // Allow perspective correction
      GXSPEC_MULTITEXTURING =0x8000, // Driver can do multitexturing
      GXSPEC_ENABLEFOGGING =0x10000, // Fog enable
      GXSPEC_FULLHWSPRITE  =0x20000, // Driver is a dynamic DLLs
      GXSPEC_ENABLEZBUFFER =0x40000, // Z-Buffer enable
      GXSPEC_FORCEWIREFRAME =0x80000, // Force WireFrame polygons draw.
      GXSPEC_LOCKISNOTREQUIRED =0x100000, // Locking surface before BeginList is not required
      GXSPEC_ANTIALIASEDLINE =0x200000, // Enable lines aliasing
      GXSPEC_ENABLEDITHERING =0x400000, // Enable color dithering
      GXSPEC_RGBLIGHTING  =0x800000, // RGB lighting
      GXSPEC_WBUFFER	  =0x1000000, 
      GXSPEC_ENABLEWBUFFER =0x2000000, 
      GXSPEC_ENABLESTENCIL =0x4000000, 
      GXSPEC_DISABLEDUALTEX =0x8000000, 
      GXSPEC_BUMPMAPPING  =0x10000000, 
      GXSPEC_NONPOWOF2 =0x20000000, 
      GXSPEC_RESERVED30 =0x40000000

};

// Render global categories 
enum {
      V3XRCLASS_point,         
      V3XRCLASS_wired,         
      V3XRCLASS_flat, 
      V3XRCLASS_gouraud, 
      V3XRCLASS_transp, 
      V3XRCLASS_normal_mapping, 
      V3XRCLASS_opacity_mapping, 
      V3XRCLASS_transp_mapping, 
      V3XRCLASS_transp_flat, 
      V3XRCLASS_dualtex_mapping, 
      V3XRCLASS_blur_mapping, 
      V3XRCLASS_shadow, 
      V3XRCLASS_double_mapping, 
      V3XRCLASS_bitmap, 
      V3XRCLASS_bitmap_transp, 
      V3XRCLASS_bitmap_any, 
      V3XRCLASS_bump_mapping
};

// Render discrete categories

enum {
      V3XID_BLACK = 1, 
      V3XID_POINT = 2, 
      V3XID_LINE = 3, 
      V3XID_FLAT = 4, 
      V3XID_GOURAUD = 5, 
      V3XID_TEX  = 16, 
      V3XID_TEX_FLAT = 16+2, 
      V3XID_TEX_GOURAUD = 16+3, 
      V3XID_TEXDOUBLE  = 16+5, 
      V3XID_TEXBUMP = 16+6, 
      V3XID_OPA_TEX = 32+8, 
      V3XID_OPA_TEX_FLAT = 32+8+8, 
      V3XID_OPA_TEX_GOURAUD = 32+8+16, 
      V3XID_SPRITE = 64+0, 
      V3XID_OPA_SPRITE = 64+32, 
	  V3XID_T_LINE			= 128, 
      V3XID_T_FLAT = 128+4, 
      V3XID_T_GOURAUD  = 128+8, 
      V3XID_T_TEX = 128+16, 
      V3XID_T_TEX_FLAT = 128+16+8, 
      V3XID_T_TEX_GOURAUD = 128+16+16, 
      V3XID_T_OPA_TEX = 128+32+8, 
      V3XID_T_OPA_TEX_FLAT = 128+32+16, 
      V3XID_T_OPA_TEX_GOURAUD = 128+32+24, 

      V3XID_T_SPRITE = 128+64, 
      V3XID_T_SPRITE_ADD = 128+64+1, 
      V3XID_T_SPRITE_SUB = 128+64+2, 
      V3XID_T_SPRITE_CST = 128+64+3
};

enum {
      V3XMAT_NOSMOOTH =  1, 
      V3XMAT_NORESIZE =  2
};


enum {
	 V3XCMD_CMPNEVER = 1, 
	 V3XCMD_CMPLESS     , 
	 V3XCMD_CMPEQUAL    , 
	 V3XCMD_CMPLESSEQUAL, 
	 V3XCMD_CMPGREATER  , 
	 V3XCMD_CMPNOTEQUAL , 
	 V3XCMD_CMPGREATEREQUAL, 
	 V3XCMD_CMPALWAYS
};

enum {
	 V3XCMD_SOPKEEP = 1, 
	 V3XCMD_SOPZERO     , 
	 V3XCMD_SOPREPLACE  , 
	 V3XCMD_SOPINCRSAT  , 
	 V3XCMD_SOPDECRSAT  , 
	 V3XCMD_SOPINCR     , 
	 V3XCMD_SOPDECR      
};


enum {
     V3XCMD_SETFOGCOLOR  = 0x10, 
     V3XCMD_SETFOGSTATE  = 0x20, 
     V3XCMD_SETZBUFFERSTATE = 0x30, 
     V3XCMD_SETZBUFFERCOMP = 0x40, 
     V3XCMD_SETWBUFFERSTATE = 0x50, 
     V3XCMD_SETBACKGROUNDCOLOR= 0x80, 
     V3XCMD_SETSTENCILMODE = 0x100, 
     V3XCMD_SETSTENCILOP = 0x200, 
     V3XCMD_SETSTENCILFUNC = 0x400

};

__extern_c


    // Render objects
_RLXEXPORTFUNC    void RLXAPI           V3XMesh_SetRender(V3XMESH *obj);

    // Additional Primitives
_RLXEXPORTFUNC    void CALLING_C        V3XRENDER_Null(V3XPOLY *fce);
_RLXEXPORTFUNC    void CALLING_C        V3XRENDER_Point(V3XPOLY *fce);
_RLXEXPORTFUNC    void CALLING_C        V3XRENDER_Wired(V3XPOLY *fce);
_RLXEXPORTFUNC    void CALLING_C        V3XRENDER_Color(V3XPOLY *fce);
_RLXEXPORTFUNC    void CALLING_C        V3XRENDER_SpriteAny(V3XPOLY *fce);


    // Primitive
_RLXEXPORTFUNC    int  RLXAPI           V2XVector_Clip( V3XVECTOR2 *a, V3XVECTOR2 *b );
_RLXEXPORTFUNC    int  RLXAPI           GX_ClipLine(V3XVECTOR *aa, V3XVECTOR *bb, V3XVECTOR *a, V3XVECTOR *b);
_RLXEXPORTFUNC    int  RLXAPI           GX_ClippedLine(V3XVECTOR2 *a, V3XVECTOR2 *b, u_int32_t cc);
_RLXEXPORTFUNC    void RLXAPI           GX_ClippedLine3D(V3XVECTOR *a, V3XVECTOR *b, u_int32_t c);

    // RGB calculations
_RLXEXPORTFUNC    void                  V3XRGB_Composing(rgb32_t *dest, V3XPOLY *fce);

    // Sprite render
_RLXEXPORTFUNC    void CALLING_C        V3XCSP_3DSprite(GXSPRITE *sp, int32_t x, int32_t y, int32_t new_lx, int32_t new_ly);
_RLXEXPORTFUNC    void CALLING_C        V3XCSP_3DSprite_Alpha(GXSPRITE *sp, int32_t x, int32_t y, int32_t new_lx, int32_t new_ly);

 #ifdef _GX_CSP_H
_RLXEXPORTDATA    extern CSP_FUNCTION   V3X_CSPset, V3X_CSPset_zoom;
 #endif
#define V3X_CSP_RegisterSprite(f, _f) (f).handle=&(_f);
__end_extern_c

#endif
