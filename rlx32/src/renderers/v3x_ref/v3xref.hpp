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

static V3X_GXNonTexPrimitives GFX3Dg_8bit = {
    0, 
    Rend8bit_C_flat, 
    Rend8bit_C_flat, 
    Rend8bit_C_gouraud, 
    Rend8bit_C_glenz, 
    Rend8bit_C_glenzflat
};

static V3X_GXTexPrimitives GFX3D_8bit= {
    0, 
    Rend8bit_C_map_normal, 
    Rend8bit_C_map_normal, 
    Rend8bit_C_map_dualtex, 
    Rend8bit_C_map_sprite, 
    Rend8bit_C_tex_alpha, 
    Rend8bit_C_map_norm_F, 
    Rend8bit_C_map_norm_G, 
    Rend8bit_C_map_sprite_F

};
static V3X_GXTexPrimitives GFX3Df_8bit= {
    0, 
    Rend8bit_Fix_map_normal, 
    Rend8bit_C_map_normal, 
    Rend8bit_C_map_dualtex, 
    Rend8bit_Fix_map_sprite, 
    Rend8bit_Fix_tex_alpha, 
    Rend8bit_Fix_map_norm_F, 
    Rend8bit_Fix_map_norm_G, 
    Rend8bit_Fix_map_sprite_F
};

static V3X_GXTexPrimitives GFX3Ds_8bit= {
    0, 
    Rend8bit_CS_map_normal, 
    Rend8bit_CS_map_normal, 
    Rend8bit_CS_map_dualtex, 
    Rend8bit_CS_map_sprite, 
    Rend8bit_CS_tex_alpha, 
    Rend8bit_CS_map_norm_F, 
    Rend8bit_CS_map_norm_G,
    Rend8bit_CS_map_sprite_F
};
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static V3X_GXNonTexPrimitives Prim16_NonTex = {
    0, 
    Rend16bit_C_flat, 
    Rend16bit_C_flat, 
    Rend16bit_C_gouraud, 
    Rend16bit_C_glenz, 
    Rend16bit_C_glenzflat
};

static V3X_GXTexPrimitives Prim16_Linear256x256x8b= {
    0, 
    Rend16bit_C_map_normal, 
    Rend16bit_C_map_normal, 
    Rend16bit_C_map_dualtex, 
    Rend16bit_C_map_sprite, 
    Rend16bit_C_tex_alpha, 
	Rend16bit_C_tex_alpha, 
    Rend16bit_C_map_norm_F, 
    Rend16bit_C_map_norm_G, 
    Rend16bit_C_map_sprite_F
};
static V3X_GXTexPrimitives Prim16_Corrected256x256x8b= {
    0, 
    Rend16bit_Fix_map_normal, 
    Rend16bit_C_map_normal, 
    Rend16bit_C_map_dualtex,
    Rend16bit_Fix_map_sprite,
    Rend16bit_Fix_tex_alpha, 
	Rend16bit_Fix_tex_alpha, 
    Rend16bit_Fix_map_norm_F, 
    Rend16bit_Fix_map_norm_G, 
    Rend16bit_Fix_map_sprite_F
};
static V3X_GXTexPrimitives Prim16_Linear128x128x8b= {
    0, 
    Rend16bit_CS_map_normal, 
    Rend16bit_CS_map_normal, 
    Rend16bit_CS_map_dualtex, 
    Rend16bit_CS_map_sprite, 
    Rend16bit_CS_tex_alpha, 
	Rend16bit_CS_tex_alpha, 
    Rend16bit_CS_map_norm_F, 
    Rend16bit_CS_map_norm_G, 
    Rend16bit_CS_map_sprite_F

};
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static V3X_GXNonTexPrimitives Prim32_NonTex = {
    0, 
    Rend32bit_C_flat, 
    Rend32bit_C_flat, 
    Rend32bit_C_gouraud, 
    Rend32bit_C_glenz, 
    Rend32bit_C_glenzflat
};


static V3X_GXTexPrimitives Prim32_Linear256x256x8b= {
    0, 
    Rend32bit_C_map_normal,  // tex
    Rend32bit_C_map_normal,  // tex_rough
    Rend16bit_C_map_dualtex, // tex_2pass
    Rend32bit_C_map_sprite,  // tex_opa 
	Rend32bit_C_tex_alpha,   // tex_trsp 
    Rend32bit_C_tex_alpha,   // tex_trsp Add
    Rend32bit_C_map_norm_F,  // flat_tex
    Rend32bit_C_map_norm_G,  // gouraud_tex 
    Rend32bit_C_map_sprite_F // flat_opa_tex

};
static V3X_GXTexPrimitives Prim32_Corrected256x256x8b= {
    0, 
    Rend32bit_Fix_map_normal, // tex
    Rend32bit_Fix_map_normal, // tex_rough  
    Rend32bit_C_map_dualtex,  // tex_2pass
    Rend32bit_Fix_map_sprite, // tex_opa
    Rend32bit_Fix_tex_alpha,  // tex_trsp
	Rend32bit_Fix_tex_alpha,  // tex_trsp_add
    Rend32bit_Fix_map_norm_F, // flat_tex 
    Rend32bit_Fix_map_norm_G, // gouraud_tex 
    Rend32bit_Fix_map_sprite_F // flat_opa_tex

};
static V3X_GXTexPrimitives Prim32_Linear128x128x8b= {
    0, 
    Rend16bit_CS_map_normal,   // tex
    Rend16bit_CS_map_normal,   // tex_rough
    Rend16bit_CS_map_dualtex,  // tex_2pass
    Rend16bit_CS_map_sprite,   // tex_opa
    Rend16bit_CS_tex_alpha,    // tex_trsp
	Rend16bit_CS_tex_alpha,    // tex_trsp
    Rend16bit_CS_map_norm_F,   // flat_tex
    Rend16bit_CS_map_norm_G,   // gouraud_tex
    Rend16bit_CS_map_sprite_F  // flat_opa_tex
};

