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
#ifndef __GX_RGB_H
#define __GX_RGB_H

// Color format macros
#define RGB_PixelFormat(r, g, b) \
(( (  (r)>>(8-GX.View.ColorMask.RedMaskSize  )) << GX.View.ColorMask.RedFieldPosition   )\
| ( ((g)>>(8-GX.View.ColorMask.GreenMaskSize)) << GX.View.ColorMask.GreenFieldPosition )\
| ( ((b)>>(8-GX.View.ColorMask.BlueMaskSize )) << GX.View.ColorMask.BlueFieldPosition  ))

#define RGBA_PixelFormat(r, g, b, a) \
(( (  (r)>>(8-GX.View.ColorMask.RedMaskSize  )) << GX.View.ColorMask.RedFieldPosition   )\
| ( ((g)>>(8-GX.View.ColorMask.GreenMaskSize)) << GX.View.ColorMask.GreenFieldPosition )\
| ( ((b)>>(8-GX.View.ColorMask.BlueMaskSize )) << GX.View.ColorMask.BlueFieldPosition  )\
| ( ((a)>>(8-GX.View.ColorMask.RsvdMaskSize )) << GX.View.ColorMask.RsvdFieldPosition  ))\

#define RGB_332(x) (  ((unsigned)(x).r>>5) + (((unsigned)(x).g>>5)<<3) + (((unsigned)(x).b>>6)<<6)  )
#define RGB_ToGray(r, g, b)    ((((unsigned)(b)*29L)+((unsigned)(g)*150L)+((unsigned)(r)*77L))>>8)
#define RGB_Make32bit(r, g, b, a) (unsigned)(b)+((unsigned)(g)<<8)+((unsigned)(r)<<16)+((unsigned)(a)<<24)
#define RGB_Alpha50(a, b)        ((((a)&GX.View.RGB_Magic)+((b)&GX.View.RGB_Magic))>>1)
#define RGB_Specular(a, ext)   ((unsigned)a.ext+(unsigned)GX.AmbientColor.ext>255 ? 255 : a.ext+GX.AmbientColor.ext)
#define RGB_Set(pal, xr, xg, xb) { (pal).r=(uint8_t)(xr); (pal).g=(uint8_t)(xg);  (pal).b=(uint8_t)(xb); }
#define RGB32_Set(pal, xr, xg, xb, xa) { (pal).r=(uint8_t)(xr); (pal).g=(uint8_t)(xg);  (pal).b=(uint8_t)(xb); (pal).a=(uint8_t)(xa);}

// 48 bit color structure
typedef struct{
    unsigned rouge;
    unsigned vert;
    unsigned bleu;
} rgb48_t;

// Mix method for 8bit blending table
enum {
    REALRGBMIX_Alpha, // Alpha blending
    REALRGBMIX_Mul,  // Mul blending
    REALRGBMIX_Add,  // Additive blending
    REALRGBMIX_Sub,  // Substractive blending
    REALRGBMIX_Neg,  // Inverse blending
    REALCOLOR_RgbReal
};
// 8bit fade method
enum {
    FADE_Black,      // Fade from/to black color
    FADE_White         // Fade from/to with color
};


// Functions

__extern_c

    // Pixel format
_RLXEXPORTFUNC    void    RLXAPI  RGB_GetPixelFormat(rgb24_t *rgb, uint32_t c);    //
_RLXEXPORTFUNC    unsigned   RLXAPI  RGB_SetPixelFormat(int r, int g, int b);    //
_RLXEXPORTFUNC    void    RLXAPI  RGB_SetAlphaBit(uint16_t *mp, int32_t size);

    // Operation Palette
_RLXEXPORTFUNC    void    RLXAPI  PAL_fadeChannel(rgb24_t *pf, int st, int fi, int start, int fin, rgb48_t *coul, int revrse);
_RLXEXPORTFUNC    void    RLXAPI  PAL_cycle(      rgb24_t *palfade, int start, int fin);
_RLXEXPORTFUNC    void    RLXAPI  PAL_fade(       rgb24_t *palfade, int pal_start, int pal_fin, int start, int fin, int echelle, int revrse);
_RLXEXPORTFUNC    void    RLXAPI  PAL_fading(     rgb24_t *palfade, int start, int fin, int echelle, int revrse);

    // Load color Table
_RLXEXPORTFUNC    void    RLXAPI  ACT_LoadFn(rgb24_t *pal, char *filename2);
_RLXEXPORTFUNC    void    RLXAPI  ACT_LoadFp(rgb24_t *pal, SYS_FILEHANDLE in);

    // Color converters
_RLXEXPORTFUNC    uint32_t   RLXAPI  RGB_convert(int c, rgb24_t *palette);
_RLXEXPORTFUNC    void    RLXAPI  RGBA_to_RGB(rgb32_t *tab, unsigned size);
_RLXEXPORTFUNC    uint32_t   RLXAPI  RGB_PixelFormatEx(rgb24_t *p);
_RLXEXPORTFUNC    uint8_t   RLXAPI *RGB_SmartConverter(void *tgt, rgb24_t *target_pal, int target_bpp,
      void *source, const rgb24_t *source_pal, int source_bpp, uint32_t size);
_RLXEXPORTFUNC    uint32_t  RLXAPI   RGB_findNearestColor(const rgb24_t *col, const rgb24_t *pal);

    // RGB filtering
_RLXEXPORTFUNC    void    RLXAPI  CSP_SmoothRGB(GXSPRITE *sp, int tbpp, int sbpp);
_RLXEXPORTFUNC    void    RLXAPI  CSP_AntiAliasRGB(GXSPRITE *sp, int tbpp, int sbpp);
_RLXEXPORTFUNC    void    RLXAPI  CSP_DarkRGB(GXSPRITE *sp, int alpha, int tbpp, int sbpp);
_RLXEXPORTFUNC    void    RLXAPI  CSP_Remap8bit(GXSPRITE *sp, rgb24_t *oldpal, rgb24_t *newpal);
_RLXEXPORTFUNC    void    RLXAPI  PAL_SetRedCyanPalette(void);
_RLXEXPORTFUNC    void    RLXAPI  RGB_AntiAlias(GXSPRITE *sp);
_RLXEXPORTFUNC    void    RLXAPI  RGB_Build332ColorTable(rgb24_t *lut);

    // 8bit blend palette
_RLXEXPORTFUNC    uint8_t   RLXAPI *REALCOLOR_Compute(rgb24_t *pal, rgb24_t *clr, int mode, int alpha, int quantize);
_RLXEXPORTFUNC    void    RLXAPI  REALCOLOR_Reduce(uint8_t **real, int factor);
_RLXEXPORTFUNC    int     RLXAPI  REALCOLOR_Simply(uint8_t **real);
_RLXEXPORTFUNC    void    RLXAPI  REALCOLOR_Free(uint8_t **real);
_RLXEXPORTFUNC    uint8_t   RLXAPI **REALCOLOR_LoadFn(const char *xpal);

    // Fast macro for color fading.
#define GX_FadeDownPalette(typ)     PAL_fading(GX.ColorTable, 32, 0, 32, typ)
#define PAL_FadeUp(typ)       PAL_fading(GX.ColorTable, 0, 32, 32, typ)
#define PAL_Black()           PAL_fading(GX.ColorTable, 0, 0, 32, FADE_Black)
#define PAL_Full()            PAL_fading(GX.ColorTable, 32, 32, 32, FADE_Black)

#define RASTER_CMDRESET       0xFF
#define RASTER_CMDBORDER      0x31
#define RASTER_CMDBACK        0x00

__end_extern_c

#endif

