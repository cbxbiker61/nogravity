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
#include "_rlx.h"

extern struct RLXSYSTEM *g_pRLX;
#define RGB_ALPHA50(a, b)        ((((a)&g_pRLX->pGX->View.RGB_Magic)+((b)&g_pRLX->pGX->View.RGB_Magic))>>1)

typedef struct
{		
	int		bpp;
	u_int32_t   palette[256];
	u_int8_t * texture;
	
}V3XTEXTURESW;

#ifdef cplusplus
extern "C"{
#endif
    // PolyPrimitives References(C) - 8bit
 
    void CALLING_C Rend8bit_C_flat(V3XPOLY *fce);
    void CALLING_C Rend8bit_C_glenzflat(V3XPOLY *fce);
    void CALLING_C Rend8bit_C_gouraud(V3XPOLY *fce);
    void CALLING_C Rend8bit_C_glenz(V3XPOLY *fce);
    void CALLING_C Rend8bit_C_map_normal(V3XPOLY *fce);
    void CALLING_C Rend8bit_C_map_dualtex(V3XPOLY *fce);
    void CALLING_C Rend8bit_C_map_sprite(V3XPOLY *fce);
    void CALLING_C Rend8bit_C_tex_alpha(V3XPOLY *fce);
    void CALLING_C Rend8bit_C_map_norm_F(V3XPOLY *fce);
    void CALLING_C Rend8bit_C_map_norm_G(V3XPOLY *fce);
    void CALLING_C Rend8bit_C_map_sprite_F(V3XPOLY *fce);
    void CALLING_C Rend8bit_C_map_phong(V3XPOLY *fce);

    void CALLING_C Rend8bit_Fix_map_normal(V3XPOLY *fce);
    void CALLING_C Rend8bit_Fix_map_norm_F(V3XPOLY *fce);
    void CALLING_C Rend8bit_Fix_map_norm_G(V3XPOLY *fce);
    void CALLING_C Rend8bit_Fix_map_sprite(V3XPOLY *fce);
    void CALLING_C Rend8bit_Fix_map_sprite_F(V3XPOLY *fce);
    void CALLING_C Rend8bit_Fix_tex_alpha(V3XPOLY *fce); 
  

 // PolyPrimitives References(C) - 16bit
    void CALLING_C Rend16bit_C_flat(V3XPOLY *fce);
    void CALLING_C Rend16bit_C_glenzflat(V3XPOLY *fce);
    void CALLING_C Rend16bit_C_gouraud(V3XPOLY *fce);
    void CALLING_C Rend16bit_C_glenz(V3XPOLY *fce);
    void CALLING_C Rend16bit_C_map_normal(V3XPOLY *fce);
    void CALLING_C Rend16bit_C_map_dualtex(V3XPOLY *fce);
    void CALLING_C Rend16bit_C_map_sprite(V3XPOLY *fce);
    void CALLING_C Rend16bit_C_tex_alpha(V3XPOLY *fce);
    void CALLING_C Rend16bit_C_map_norm_F(V3XPOLY *fce);
    void CALLING_C Rend16bit_C_map_norm_G(V3XPOLY *fce);
    void CALLING_C Rend16bit_C_map_sprite_F(V3XPOLY *fce);
    void CALLING_C Rend16bit_C_map_bump_F(V3XPOLY *fce);
    void CALLING_C Rend16bit_C_map_phong(V3XPOLY *fce);

    void CALLING_C Rend16bit_Fix_map_normal(V3XPOLY *fce);
    void CALLING_C Rend16bit_Fix_map_norm_F(V3XPOLY *fce);
    void CALLING_C Rend16bit_Fix_map_norm_G(V3XPOLY *fce);
    void CALLING_C Rend16bit_Fix_map_sprite(V3XPOLY *fce);
    void CALLING_C Rend16bit_Fix_map_sprite_F(V3XPOLY *fce);
    void CALLING_C Rend16bit_Fix_tex_alpha(V3XPOLY *fce);


  // PolyPrimitives References(C) - 32bit
    void CALLING_C Rend32bit_C_flat(V3XPOLY *fce);
    void CALLING_C Rend32bit_C_glenzflat(V3XPOLY *fce);
    void CALLING_C Rend32bit_C_gouraud(V3XPOLY *fce);
    void CALLING_C Rend32bit_C_glenz(V3XPOLY *fce);

    void CALLING_C Rend32bit_C_map_normal(V3XPOLY *fce);
    void CALLING_C Rend32bit_C_map_dualtex(V3XPOLY *fce);
    void CALLING_C Rend32bit_C_map_sprite(V3XPOLY *fce);
    void CALLING_C Rend32bit_C_tex_alpha(V3XPOLY *fce);
    void CALLING_C Rend32bit_C_map_norm_F(V3XPOLY *fce);
    void CALLING_C Rend32bit_C_map_norm_G(V3XPOLY *fce);
    void CALLING_C Rend32bit_C_map_sprite_F(V3XPOLY *fce);
    void CALLING_C Rend32bit_C_map_phong(V3XPOLY *fce);

    void CALLING_C Rend32bit_Fix_map_normal(V3XPOLY *fce);
    void CALLING_C Rend32bit_Fix_map_norm_F(V3XPOLY *fce);
    void CALLING_C Rend32bit_Fix_map_norm_G(V3XPOLY *fce);
    void CALLING_C Rend32bit_Fix_map_sprite(V3XPOLY *fce);
    void CALLING_C Rend32bit_Fix_map_sprite_F(V3XPOLY *fce);
    void CALLING_C Rend32bit_Fix_tex_alpha(V3XPOLY *fce);

    // Nearly obsolete version
    void CALLING_C Rend8bit_CS_map_normal(V3XPOLY *fce);
    void CALLING_C Rend8bit_CS_map_dualtex(V3XPOLY *fce);
    void CALLING_C Rend8bit_CS_map_sprite(V3XPOLY *fce);
    void CALLING_C Rend8bit_CS_tex_alpha(V3XPOLY *fce);
    void CALLING_C Rend8bit_CS_map_norm_F(V3XPOLY *fce);
    void CALLING_C Rend8bit_CS_map_norm_G(V3XPOLY *fce);
    void CALLING_C Rend8bit_CS_map_sprite_F(V3XPOLY *fce);
    void CALLING_C Rend8bit_CS_map_bump_F(V3XPOLY *fce);
    void CALLING_C Rend8bit_CS_map_phong(V3XPOLY *fce);

    void CALLING_C Rend16bit_CS_map_normal(V3XPOLY *fce);
    void CALLING_C Rend16bit_CS_map_dualtex(V3XPOLY *fce);
    void CALLING_C Rend16bit_CS_map_sprite(V3XPOLY *fce);
    void CALLING_C Rend16bit_CS_tex_alpha(V3XPOLY *fce);
    void CALLING_C Rend16bit_CS_map_norm_F(V3XPOLY *fce);
    void CALLING_C Rend16bit_CS_map_norm_G(V3XPOLY *fce);
    void CALLING_C Rend16bit_CS_map_sprite_F(V3XPOLY *fce);
    void CALLING_C Rend16bit_CS_map_bump_F(V3XPOLY *fce);
    void CALLING_C Rend16bit_CS_map_phong(V3XPOLY *fce);

#ifdef cplusplus
}
#endif

