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

#define DEFSHL      1
#define MAPWIDTH    7

#include "poly.hpp"
#include "rd_128.hpp"

// =========================================================================
void CALLING_C Rend16bit_CS_map_bump_F (V3XPOLY *fce)
{
   fce=fce;
   return;
}

// =========================================================================
void CALLING_C Rend16bit_CS_map_normal(V3XPOLY *fce)
{
    TRI_VAR
    VAR_MAP
    
    MASTER_1
    CALC_MAP
	CST_MAP
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	TSWAP_MAP
		MASTER_2
			/* Trace d'une ligne */
			START_MAP



			/*  Trace d'un points*/
			INNER_LOOP
			    {
				GFX_word(ptr, texture->palette[texture->texture[MAPADR(U, V)]]);
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
    return;
}
// =========================================================================
void CALLING_C Rend16bit_CS_map_dualtex(V3XPOLY *fce)
{
    TRI_VAR
    VAR_MAP
    
    MASTER_1
    CALC_MAP
	CST_MAP
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	TSWAP_MAP
		MASTER_2
			/* Trace d'une ligne */
			START_MAP



			/*  Trace d'une ligne*/
			if (XL>=0)
			{
			    int32_t x;
			    ptr = off_ptr + x0*2;
			    for (x=(XL+1)>>1;x!=0;ptr+=4, x--) //2 fois moins
			    {
				*(u_int16_t*)ptr =
				*(u_int16_t*)(ptr+2) = (u_int16_t)texture->palette[texture->texture[MAPADR(U, V)]];
				U  += dUx<<1;
				V  += dVx<<1;
			    }
			    // le point eventuellement restant
			    if (!(XL&1)) *(u_int16_t*)ptr = (u_int16_t)texture->palette[texture->texture[MAPADR(U, V)]];
			}
		    ADD_MAP
		    FINAL
    return;
}
// =========================================================================
void CALLING_C Rend16bit_CS_map_norm_G(V3XPOLY *fce)
{
    TRI_VAR
    VAR_IT
    VAR_MAP
    
    
    MASTER_1
    CALC_IT
    CALC_MAP
	CST_IT
	CST_MAP
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	TSWAP_MAP
	TSWAP_IT
		MASTER_2
			/* Trace d'une ligne */
			START_IT
			START_MAP


			/*  Trace d'un points*/
			INNER_LOOP
			    {
				GFX_word(ptr, g_DiffuseTable[
					g_MixTable[SHRD(It)&255]
					[texture->texture[MAPADR(U, V)]]]);
				It += dIx;
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_IT
		    ADD_MAP
		    FINAL
    return;
}

// =========================================================================
void CALLING_C Rend16bit_CS_map_sprite(V3XPOLY *fce)
{
    u_int16_t vx;
    TRI_VAR
    VAR_MAP
    
    MASTER_1
    CALC_MAP
	CST_MAP
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	TSWAP_MAP
		MASTER_2
			/* Trace d'une ligne */
			START_MAP



			/*  Trace d'un points*/
			INNER_LOOP
			    {
				vx = (u_int16_t)texture->palette[texture->texture[MAPADR(U, V)]];
				if (vx) GFX_word(ptr, vx);
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
    return;
}

// =========================================================================
void CALLING_C Rend16bit_CS_map_norm_F(V3XPOLY *fce)
{
    u_int8_t *Shader;
    int32_t *shade=(int32_t*)fce->shade;
    TRI_VAR
    VAR_MAP    
    
    Shader = (u_int8_t*)g_MixTable[shade[0]];
    MASTER_1
    CALC_MAP
	CST_MAP
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	TSWAP_MAP
		MASTER_2
			/* Trace d'une ligne */
			START_MAP



			/*  Trace d'un points*/
			INNER_LOOP
			    {
					GFX_word(ptr, g_DiffuseTable[Shader[texture->texture[MAPADR(U, V)]]]);
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
    return;
}
// =========================================================================
void CALLING_C Rend16bit_CS_tex_alpha(V3XPOLY *fce)
{
    TRI_VAR
    VAR_MAP
    
    MASTER_1
    CALC_MAP
	CST_MAP
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
		MASTER_2
			/* Trace d'une ligne */
			START_MAP


			/*  Trace d'un points*/

			INNER_LOOP
			    {
				GFX_word(ptr, RGB_ALPHA50(texture->palette[texture->texture[MAPADR(U, V)]], *ptr));
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
    return;
}
// =========================================================================
void CALLING_C Rend16bit_CS_map_sprite_F(V3XPOLY *fce)
{
    u_int8_t *Shader;
    int32_t *shade=(int32_t*)fce->shade;
    u_int8_t vx;
    TRI_VAR
    VAR_MAP
    
    
    Shader = (u_int8_t*)g_MixTable[(shade[0]+shade[1]+shade[2])/3];
    MASTER_1
    CALC_MAP
	CST_MAP
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	TSWAP_MAP
		MASTER_2
			/* Trace d'une ligne */
			START_MAP



			/*  Trace d'un points*/
			INNER_LOOP
			    {
					vx = texture->texture[MAPADR(U, V)];
				if (vx) GFX_word(ptr, texture->palette[Shader[vx]]);
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
    return;
}
// =========================================================================
void CALLING_C Rend16bit_CS_map_phong(V3XPOLY *fce)
{
    TRI_VAR
    VAR_MAP
    VAR_MAP2
    
    
    MASTER_1
    CALC_MAP
    CALC_MAP2
	CST_MAP
	CST_MAP2
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	TSWAP_MAP
	TSWAP_MAP2
		MASTER_2
			/* Trace d'une ligne */
			START_MAP
			START_MAP2




			/*  Trace d'un points*/
			INNER_LOOP
			    {
				GFX_word(ptr, g_DiffuseTable[
					       g_MixTable
							   [texture->texture [MAPADR(U, V)]]
							   [texture2->texture[MAPADR(U0, V0)]]]);

				U  += dUx;
				V  += dVx;
				U0 += dU0x;
				V0 += dV0x;

			    }
			}
		    ADD_MAP
		    ADD_MAP2
		    FINAL
    return;
}
