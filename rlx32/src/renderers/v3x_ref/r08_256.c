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

#define DEFSHL      0
#define MAPWIDTH    8

#include "poly.hpp"
#include "rd_256.hpp"

void CALLING_C Rend8bit_C_glenzflat (V3XPOLY *fce)
{
    uint8_t vx=255;
    TRI_VAR

    uint8_t *Shader = (uint8_t*)g_MixTable[vx];
    MASTER_1
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
		MASTER_2
			INNER_LOOP
			    {
				*ptr = Shader[*ptr];
			    }
			}
		    FINAL
}

void CALLING_C Rend8bit_C_glenz (V3XPOLY *fce)
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
				*ptr = g_MixTable[SHRD(It)&255][*ptr];
				It += dIx;

			    }
			}
		    ADD_IT
		    FINAL
}
// =========================================================================
void CALLING_C Rend8bit_C_flat(V3XPOLY *fce)
{
    uint8_t vx;
    int32_t *shade=(int32_t*)fce->shade;
    TRI_VAR

    vx=(uint8_t)g_DiffuseTable[shade[0]];
    MASTER_1
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
		MASTER_2
			/*  Trace d'un points*/
			INNER_LOOP
			    {
				*ptr = vx;
			    }
			}
		    FINAL
    Pper=Pper;
}
// =========================================================================
void CALLING_C Rend8bit_C_gouraud(V3XPOLY *fce)
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
				*ptr = (uint8_t)g_DiffuseTable[SHRD(It)&255];
				It += dIx;

			    }
			}
		    ADD_IT
		    FINAL
    return;
}

// =========================================================================
void CALLING_C Rend8bit_C_map_norm_G(V3XPOLY *fce)
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
	TSWAP_IT
	TSWAP_MAP
		MASTER_2
			/* Trace d'une ligne */
			START_IT
			START_MAP
			/*  Trace d'un points*/
			INNER_LOOP
			    {
				*ptr = g_MixTable[SHRD(It)&255]
						  [texture->texture[MAPADR(U, V)]];
				It += dIx;
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_IT
		    ADD_MAP
		    FINAL
}

// ==================/ le rendu sacr‚ /===============================
void CALLING_C Rend8bit_C_map_normal(V3XPOLY *fce)
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
					*ptr = texture->texture[MAPADR(U, V)];
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
}

// =========================================================================
void CALLING_C Rend8bit_C_map_dualtex(V3XPOLY *fce)
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
			    ptr = off_ptr + x0;
			    for (x=XL>>1;x!=0;ptr+=2, x--) //2 fois moins
			    {
				ptr[0] = ptr[1] = texture->texture[MAPADR(U, V)];
				U  += dUx<<1;
				V  += dVx<<1;
			    }
			    // le point eventuellement restant
			    if (XL&1) ptr[0] = texture->texture[MAPADR(U, V)];
			}
		    ADD_MAP
		    FINAL
}

// =========================================================================
void CALLING_C Rend8bit_C_map_sprite(V3XPOLY *fce)
{
    uint8_t vx;
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
				vx = texture->texture[MAPADR(U, V)];
				if (vx) *ptr=vx;
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
}

// =========================================================================
void CALLING_C Rend8bit_C_map_norm_F(V3XPOLY *fce)
{
    uint8_t *Shader;
    int32_t *shade=(int32_t*)fce->shade;
    TRI_VAR
    VAR_MAP

    Shader = (uint8_t*)g_MixTable[shade[0]];

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
				*ptr = Shader[texture->texture[MAPADR(U, V)]];
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
}

// =========================================================================
void CALLING_C Rend8bit_C_tex_alpha(V3XPOLY *fce)
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
				*ptr = g_MixTable[texture->texture[MAPADR(U, V)]][*ptr];
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
}

// =========================================================================
void CALLING_C Rend8bit_C_map_sprite_F(V3XPOLY *fce)
{
    uint8_t *Shader;
    int32_t *shade=(int32_t*)fce->shade;
    uint8_t vx;
    TRI_VAR
    VAR_MAP

    Shader = (uint8_t*)g_MixTable[shade[0]];

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
				if (vx) *ptr=Shader[vx];
				U  += dUx;
				V  += dVx;
			    }
			}
		    ADD_MAP
		    FINAL
}

// =========================================================================
void CALLING_C Rend8bit_C_map_phong(V3XPOLY *fce)
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
				*ptr =
					g_MixTable[texture2->texture[MAPADR(U0, V0)]]
				[texture->texture[ MAPADR(U, V )]];
				U   += dUx;
				V   += dVx;
				U0  += dU0x;
				V0  += dV0x;

			    }
			}
		    ADD_MAP
		    ADD_MAP2
		    FINAL
}

