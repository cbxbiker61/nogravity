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

#define DEFSHL      2
#define MAPWIDTH    8

#include "poly.hpp"
#include "rd_256.hpp"
// =========================================================================
void CALLING_C Rend32bit_C_glenzflat (V3XPOLY *fce)
{
    TRI_VAR    
    MASTER_1
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
		MASTER_2
			INNER_LOOP
			    {
				   *(u_int32_t*)ptr = (u_int32_t)RGB_ALPHA50(g_DiffuseTable[GET_CONSTANT()], *(u_int32_t*)ptr);
			    }
			}
		    FINAL
   return;
}

void CALLING_C Rend32bit_C_glenz (V3XPOLY *fce)
{
    TRI_VAR
    VAR_IT
    
    MASTER_1
    CALC_IT
	CST_IT
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	TSWAP_IT
		MASTER_2
			/* Trace d'une ligne */
			START_IT
			/*  Trace d'un points*/
			INNER_LOOP
			    {
				  *(u_int32_t*)ptr = (u_int32_t)RGB_ALPHA50(g_DiffuseTable[GET_DIFFUSE(It)], *(u_int32_t*)ptr);
				  It += dIx;
			    }
			}
		    ADD_IT
		    FINAL

   return;
}
// =========================================================================
void CALLING_C Rend32bit_C_flat(V3XPOLY *fce)
{
    u_int32_t vx = g_DiffuseTable[GET_CONSTANT()];
    TRI_VAR    
    
    MASTER_1
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
		MASTER_2
			/*  Trace d'un points*/
			INNER_LOOP
			    {
				*(u_int32_t*)ptr = vx;
			    }
			}
		    FINAL
    Pper=Pper;
    return;
}
// =========================================================================
void CALLING_C Rend32bit_C_gouraud(V3XPOLY *fce)
{
    TRI_VAR
    VAR_IT
    
    MASTER_1
    CALC_IT
	CST_IT
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
		MASTER_2
			/* Trace d'une ligne */
			START_IT
			/*  Trace d'un points*/
			INNER_LOOP
			    {
				*(u_int32_t*)ptr = g_DiffuseTable[GET_DIFFUSE(It)];
				It += dIx;

			    }
			}
		    ADD_IT
		    FINAL
    return;
}

// =========================================================================
void CALLING_C Rend32bit_C_map_normal(V3XPOLY *fce)
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
				GFX_dword(ptr, texture->palette[texture->texture[MAPADR(U, V)]]);
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
    return;
}
// =========================================================================
// =========================================================================
void CALLING_C Rend32bit_C_map_dualtex(V3XPOLY *fce)
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
				GFX_dword(ptr, texture->palette[texture->texture[MAPADR(U, V)]]);
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
    return;
}
// =========================================================================

// =========================================================================
void CALLING_C Rend32bit_C_map_norm_G(V3XPOLY *fce)
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
					GFX_dword(ptr, g_MixTable[GET_DIFFUSE(It)][texture->texture[MAPADR(U, V)]]);
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
void CALLING_C Rend32bit_C_map_sprite(V3XPOLY *fce)
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
					u_int8_t vx = texture->texture[MAPADR(U, V)];
					if (vx) 
						GFX_dword(ptr, texture->palette[vx]);
					U  += dUx;
					V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
    return;
}

// =========================================================================
void CALLING_C Rend32bit_C_map_norm_F(V3XPOLY *fce)
{
	u_int32_t *palette = g_MixTable[GET_CONSTANT()];
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
					GFX_dword(ptr, palette[texture->texture[MAPADR(U, V)]]);
					U  += dUx;
					V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
    return;
}
// =========================================================================
void CALLING_C Rend32bit_C_tex_alpha(V3XPOLY *fce)
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
   		        GFX_dword(ptr, RGB_ALPHA50(texture->palette[texture->texture[MAPADR(U, V)]], *(u_int32_t*)ptr));
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
    return;
}
// =========================================================================
void CALLING_C Rend32bit_C_map_sprite_F(V3XPOLY *fce)
{
	u_int32_t *palette = g_MixTable[GET_CONSTANT()];
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
					u_int8_t vx = texture->texture[MAPADR(U, V)];
					if (vx)
					{
						GFX_dword(ptr, palette[vx]);
					}
					U  += dUx;
					V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
    return;
}
