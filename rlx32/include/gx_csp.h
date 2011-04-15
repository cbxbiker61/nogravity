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
#ifndef _GX_CSP_H
#define _GX_CSP_H

// Download GXSPRITE method
enum {
    CSPLOAD_POSTERIZE = 0x1,
    CSPLOAD_SURFACE = 0x2,
    CSPLOAD_HWSPRITE = 0x4,
    CSPLOAD_FORCE8 = 0x8
};

// Font text write
enum {
    CSPM_CHARSET_32,  // 32 characters
    CSPM_CHARSET_128, // 128 characters
    CSPM_CHARSET_255  // 255+ characters
};

enum {
   DD_TTYPE_BOLD = 0x1,
   DD_TTYPE_ITALIC = 0x2,
   DD_TTYPE_UNDERLINE = 0x4,
   DD_TTYPE_STRIKEOUT = 0x8,
   DD_TTYPE_HDCOPENED = 0x10,
   DD_TTYPE_HDCCLOSE = 0x20,
   DD_TTYPE_GETWIDTH = 0x80
};

// Get Area definition (SPC file item structures)
typedef struct{
    short   a, b, c, d;
}GXSPRITEFORMAT;

// Sprite group definition
typedef struct {
    GXSPRITE           *item;        // single GXSPRITE
    short int         maxItem;     // numbers of GXSPRITE in group
    uint16_t            speed;       // internal
    uint8_t             HSpacing;    // Horizontal spacing
    uint8_t             VSpacing;    // Vertical spacing
    uint8_t             Caps;        // Capabilities
    uint8_t             filler;
}GXSPRITEGROUP;

// Sprite function pointer
typedef void (* CALLING_C CSP_STDFUNCTION)(int32_t x, int32_t y, GXSPRITE *sp);
typedef void (* CALLING_C CSP_STRFUNCTION)(GXSPRITE *sp, int32_t x, int32_t y, int32_t lx, int32_t ly);

typedef union {
	CSP_STDFUNCTION fonct;
	CSP_STRFUNCTION zoomf;
}CSP_FUNCTION;

typedef struct {
    uint32_t             flags;      // Flags
    uint32_t             color;      // Color replaced by white
    uint32_t             colorKey;   // Colorkeying (transparency color)
    uint32_t             colorInv;   // inversed color
    uint32_t             alpha;      // Alpha component
    void             *table;      // Table
    void CALLING_C  (*op)(int32_t x, int32_t y, GXSPRITE *sp); // Default operators
    CSP_FUNCTION      put, pset, transp;  // Main operators
    char              ext[4];     // Extension for bitmap GXSPRITE file
}CSP_Config;

// Sprite class
typedef struct {
    uint32_t           caps;
    // Normal GXSPRITE drawer
    void CALLING_C  (*put)(      int32_t x, int32_t y, GXSPRITE *sp);
    void CALLING_C  (*pset)(     int32_t x, int32_t y, GXSPRITE *sp);
    void CALLING_C  (*flipAxisX)(int32_t x, int32_t y, GXSPRITE *sp);

    // Blended GXSPRITE drawer (in the current depth color)
    void CALLING_C  (*Trsp50)(   int32_t x, int32_t y, GXSPRITE *sp);
    void CALLING_C  (*TrspADD)(  int32_t x, int32_t y, GXSPRITE *sp);
    void CALLING_C  (*TrspSUB)(  int32_t x, int32_t y, GXSPRITE *sp);
    void CALLING_C  (*TrspALPHA)(int32_t x, int32_t y, GXSPRITE *sp);

    // Scalable GXSPRITE drawer
    void CALLING_C  (*zoom_pset)(   GXSPRITE *sp, int32_t x, int32_t y, int32_t new_lx, int32_t new_ly);
    void CALLING_C  (*zoom_put)(      GXSPRITE *sp, int32_t x, int32_t y, int32_t new_lx, int32_t new_ly);
    void CALLING_C  (*zoom_Trsp50)( GXSPRITE *sp, int32_t x, int32_t y, int32_t new_lx, int32_t new_ly);
    void CALLING_C  (*zoom_TrspADD)( GXSPRITE *sp, int32_t x, int32_t y, int32_t new_lx, int32_t new_ly);
    void CALLING_C  (*zoom_TrspSUB)( GXSPRITE *sp, int32_t x, int32_t y, int32_t new_lx, int32_t new_ly);
    void CALLING_C  (*zoom_TrspALPHA)( GXSPRITE *sp, int32_t x, int32_t y, int32_t new_lx, int32_t new_ly);

}GXSPRITEINTERFACE;

struct _sys_fileio;

__extern_c
    // Sprite
_RLXEXPORTFUNC    GXSPRITE RLXAPI *CSP_GetFn(const char *filename, unsigned option);
_RLXEXPORTFUNC    void   RLXAPI  CSP_Get(int32_t xx, int32_t yy, GXSPRITE *capt, uint8_t *buf, int32_t width, int32_t byte);
_RLXEXPORTFUNC    void   RLXAPI  CSP_CaptureFrom(int32_t xx, int32_t yy, GXSPRITE *capt);
_RLXEXPORTFUNC    void   RLXAPI  CSP_Resize(GXSPRITE *sp, int lx, int ly, int bpp);
_RLXEXPORTFUNC    void   RLXAPI  CSP_DrawRect(int32_t x, int32_t y, int32_t lx, int32_t ly, GXSPRITE *sp);

    // Sprite familly
_RLXEXPORTFUNC    void   RLXAPI  CSPG_Release(GXSPRITEGROUP *pSpriteGroup);
_RLXEXPORTFUNC    GXSPRITEGROUP   RLXAPI *CSPG_GetFn(char *filename, struct _sys_fileio *f, unsigned option);

_RLXEXPORTFUNC    int32_t   RLXAPI  CSPG_TxLen(const char *texte, const GXSPRITEGROUP *Fonte);
_RLXEXPORTFUNC    int32_t   RLXAPI  CSPG_TxLenS(const char *texte, int32_t fx, GXSPRITEGROUP *Fonte);

    // Graphic Text writer
_RLXEXPORTFUNC    void   RLXAPI  CSP_DrawText(const char *texte, int32_t xx, int32_t yy, const GXSPRITEGROUP *Fonte, CSP_FUNCTION sp);
_RLXEXPORTFUNC    void   RLXAPI  CSP_DrawTextC(const char *str, int x, int y, int attr1, int attr2, const GXSPRITEGROUP *Fonte, CSP_FUNCTION spz);
_RLXEXPORTFUNC    void   RLXAPI  CSP_ZoomText(const char *texte, int32_t xx, int32_t yy, int32_t fx, int32_t fy, const GXSPRITEGROUP *Fonte, CSP_FUNCTION func);
    //
__end_extern_c

#define CSP_Color(c)     GX.csp_cfg.color = c
#define CSP_ColorKey(c)  GX.csp_cfg.colorKey = c
#define CSP_Alpha(c)  GX.csp_cfg.alpha = c

#define CSP_ZoomCenterText(texte, yy, factor, Fonte) CSP_ZoomText(texte, GX.View.xmin+(((GX.View.xmax-GX.View.xmin)-CSPG_TxLenS(texte, factor, Fonte))>>1), yy, factor, Fonte)
#define CSP_DrawCenterText(texte, yy, Fonte, sp)     CSP_DrawText(texte, GX.View.xmin+(((GX.View.xmax-GX.View.xmin)-CSPG_TxLen(texte, Fonte))>>1), yy, Fonte, sp)
#define CSP_DrawCenterCText(texte, yy, Fonte, sp)    CSP_DrawTextC(texte, GX.View.xmin+(((GX.View.xmax-GX.View.xmin)-CSPG_TxLen(texte, Fonte))>>1), yy, Fonte, sp)
#define CSP_WriteText(texte, xx, yy, Fonte)          CSP_DrawText(texte, xx, yy, Fonte, GX.csp_cfg.put)
#define CSP_WriteCenterText(texte, yy, Fonte)       CSP_DrawCenterText(texte, yy, Fonte, GX.csp_cfg.put)
#endif

