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
#ifndef __V3XTYPEDRV
#define __V3XTYPEDRV

#include <math.h>

// Value range

typedef float V3XSCALAR;

#define CST_ONE     1.f
#define CST_ZERO      0.f
#define CST_MAX       3.4e38f
#define CST_EPSILON   0.00001f

// Definitions for multiplications of value
#define MULF32(x, y)   ((x)*(y))
#define MULF32_SQR(x)    ((x)*(x))
#define DIVF32(x, y)   ((x)/(y))
#define xINV(x )     (1.f/(x))

// Shifting (* or / par 2^n)
#define xSHLD(x, k) ( (x)*(     (float)(1L<<(k)) )   )
#define xSHRD(x, k) ( (x)*(1.f/((float)(1L<<(k))) )   )

// Cast
#define xFLOATtoINT(x) (int)(x)
#define xINTtoFLOAT(x) (V3XSCALAR)(x)

#define xCASTTOREAL(a, b){\
	(a).x = (float)(b).x;\
	(a).y = (float)(b).y;\
	(a).z = (float)(b).z;}

#define xCASTTOFIX(a, b){\
	(a).x = (int32_t)(b).x;\
	(a).y = (int32_t)(b).y;\
	(a).z = (int32_t)(b).z;}

#define xADJUSTFIX(x, fact)    xSHLD(x, 16-(fact))

// Mathematic functions
#define cos16(x)          TRG_cos((int32_t)(x))
#define sin16(x)          TRG_sin((int32_t)(x))
#define sqrtf(x)          (V3XSCALAR)sqrt((double)(x))
#define rsqrtf(x)		  1.f / (V3XSCALAR)sqrt((double)(x))
#define fabs(x)			  (((x)>=CST_ZERO) ? (x) : -(x))
#endif
