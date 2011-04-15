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

#ifndef __GX_TOOLS_H
#define __GX_TOOLS_H

__extern_c

    // Image operation
_RLXEXPORTFUNC    void RLXAPI     IMG_stretch(uint8_t *old_buf, uint8_t *new_buf, int old_ly, int new_ly, int old_lx, int new_lx, int bytes);
_RLXEXPORTFUNC    void RLXAPI     IMG_pan(uint8_t *old_buf, uint8_t *new_buf, int old_ly, int new_ly, int old_lx, int new_lx, int bytes);
_RLXEXPORTFUNC    void RLXAPI     IMG_rotate_image(uint8_t *old, uint8_t *tmp, int lx, int ly);
_RLXEXPORTFUNC    void RLXAPI     IMG_LoadFnSprite(char *file, GXSPRITE *sp, int options);
_RLXEXPORTFUNC    void RLXAPI     IMG_StretchPoint(GXSPRITE *texture, GXSPRITE *sp3, int bp, int pitch);
_RLXEXPORTFUNC    void RLXAPI     IMG_StretchBilinear(GXSPRITE *_new, GXSPRITE *_old, int bp, int strides);

    // Image loading
_RLXEXPORTFUNC    int  RLXAPI     IMG_LoadFn(const char *filename, GXSPRITE *sp);
_RLXEXPORTFUNC    int  RLXAPI     IMG_LoadFp(const char *filename, SYS_FILEHANDLE fp, GXSPRITE *sp);
_RLXEXPORTFUNC    void RLXAPI     IMG_DumpScreen(char *prefix);

    // Addition Primitives
_RLXEXPORTFUNC    void CALLING_C  B_Line_8bit(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t colour);
_RLXEXPORTFUNC    void CALLING_C  A_Line_8bit(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t colour);
_RLXEXPORTFUNC    void RLXAPI     GX_filledCircle(int x, int y, int radius, unsigned colour);
_RLXEXPORTFUNC    void RLXAPI     GX_DrawBoxEffect3D(int32_t x, int32_t y, int32_t lx, int32_t ly);

__end_extern_c

#endif

