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
#define MAPWIDTH    8
#define PIXAR       4

#include "poly.hpp"
#include "rdz_256.hpp"
#include "rd_256.hpp"

// =========================================================================
void CALLING_C Rend16bit_Fix_map_normal(V3XPOLY *fce)
{
    TRI_VAR
    VAR_MAPZ
    
    MASTER_1
    CALC_MAPZ
	CST_MAPZ
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	TSWAP_Z
		MASTER_2
			/* Trace d'une ligne */
			START_MAPZ
			/*  Trace d'un points*/
			     CORRZ_LOOP
			     {
			       // Ici l'inner loop normal
			       GFX_word(ptr, textureZ->palette[textureZ->texture[MAPADR(u0, v0)]]);
			       u0+=deX;
			       v0+=deY;
			     }
			  }
			}
		    ADD_MAPZ
		    FINAL
    return;
}
// =========================================================================
void CALLING_C Rend16bit_Fix_map_norm_F(V3XPOLY *fce)
{
    u_int16_t *Shader;
    int32_t *shade=(int32_t*)fce->shade;
    TRI_VAR
    VAR_MAPZ
    
    Shader = (u_int16_t*)g_MixTable[shade[0]];
   
    MASTER_1
    CALC_MAPZ
	CST_MAPZ
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	TSWAP_Z
		MASTER_2
		/* Trace d'une ligne */
			START_MAPZ


			/*  Trace d'un points*/
			CORRZ_LOOP
			     {
			       // Ici l'inner loop normal
			       GFX_word(ptr, Shader[textureZ->texture[MAPADR(u0, v0)]]);
			       u0+=deX;
			       v0+=deY;
			     }
			  }
			}


		    ADD_MAPZ
		    FINAL
    return;
}
// =========================================================================
void CALLING_C Rend16bit_Fix_map_sprite(V3XPOLY *fce)
{
    TRI_VAR
    VAR_MAPZ
    
    MASTER_1
    CALC_MAPZ

	CST_MAPZ
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	TSWAP_Z
		MASTER_2
			/* Trace d'une ligne */
			START_MAPZ


			/*  Trace d'un points*/
			     CORRZ_LOOP
			     {
					 u_int8_t al= textureZ->texture[MAPADR(u0, v0)];
			       if (al) GFX_word(ptr, textureZ->palette[al]);
			       u0+=deX;
			       v0+=deY;
			     }
			  }
			}


		    ADD_MAPZ
		    FINAL
    return;
}
// =========================================================================
void CALLING_C Rend16bit_Fix_map_sprite_F(V3XPOLY *fce)
{
    u_int16_t *Shader;
    int32_t *shade=(int32_t*)fce->shade;
    TRI_VAR
    VAR_MAPZ
    
    Shader = (u_int16_t*)g_MixTable[shade[0]];
    MASTER_1
    CALC_MAPZ
	CST_MAPZ
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	TSWAP_Z
		MASTER_2
			/* Trace d'une ligne */
			START_MAPZ


			/*  Trace d'un points*/
			     CORRZ_LOOP
			     {
					 u_int8_t al= textureZ->texture[MAPADR(u0, v0)];
			       if (al) GFX_word(ptr, Shader[al]);
			       u0+=deX;
			       v0+=deY;
			     }
			  }
			}


		    ADD_MAPZ
		    FINAL
    return;
}

// =========================================================================
void CALLING_C Rend16bit_Fix_tex_alpha(V3XPOLY *fce)
{
    TRI_VAR
    VAR_MAPZ
    
    MASTER_1
    CALC_MAPZ
	CST_MAPZ
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	TSWAP_Z
		MASTER_2
			/* Trace d'une ligne */
			START_MAPZ


			/*  Trace d'un points*/
			     CORRZ_LOOP
			     {
			       GFX_word(ptr, RGB_ALPHA50(textureZ->palette[textureZ->texture[MAPADR(u0, v0)]], *(u_int16_t*)ptr));

			       u0+=deX;
			       v0+=deY;
			     }
			  }
			}


		    ADD_MAPZ
		    FINAL
    return;
}
// =========================================================================
void CALLING_C Rend16bit_Fix_map_norm_G(V3XPOLY *fce)
{
    TRI_VAR
    VAR_IT
    VAR_MAPZ
    
    MASTER_1
	   CALC_MAPZ
	   CALC_IT
	   CST_MAPZ
	   CST_IT
	/* ==================== BOUCLE PRINCIPALE ============================== */
	MASTER_L
	    TSWAP_Z
	    TSWAP_IT
		MASTER_2
			/* Trace d'une ligne */

			START_MAPZ
			START_IT
			{
			/*  Trace d'un points*/
			     CORRZ_LOOP
			     {
			       // Ici l'inner loop normal
					 GFX_word(ptr, g_MixTable[SHRD(It)&255][textureZ->texture[MAPADR(u0, v0)]]);
			       u0+=deX;
			       v0+=deY;
			       It += dIx;
			     }
			   }
			 }
		       }
		    ADD_MAPZ
		    ADD_IT
		    FINAL
    return;
}
