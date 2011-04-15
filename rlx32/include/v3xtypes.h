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

#ifndef __V3X_TYPES_H
#define __V3X_TYPES_H
//**********************************************************************/
//  Value format
//**********************************************************************/
#ifndef V3X_FLOATINGPOINT
  #ifndef V3X_INTSHORT
     #ifndef V3X_INTLONG
        #define V3X_FLOATINGPOINT
     #endif
  #endif
#endif

#if defined V3X_FLOATINGPOINT
   #include "v3xfloat.h"

#elif defined V3X_INTSHORT
   #include "v3xshort.h"

#elif defined V3X_INTLONG
   #include "v3xlong.h"
#endif

// Rasterizer call driver format
#define V3XAPI

typedef struct _v3xluv {
    int u, v;
}V3XlUV;             // 2D vector (integer format)

typedef struct _v3xlpts{
    int x, y, z, w;
}V3XlPTS;            // 4D vector (integer format)

typedef struct _v3xlvector{
    int x, y, z;
}V3XVECTORL;         // 3D vector (integer format)

typedef struct _v3xvector2{
    V3XSCALAR x, y;
}V3XVECTOR2;          // 2D vector

typedef struct _v3xvector{
    V3XSCALAR x, y, z;
}V3XVECTOR;          // 3D vector (standard)

typedef struct _v3xquaternion{
    V3XSCALAR x, y, z, w;
}V3XQUATERNION;      // Quaternion

typedef V3XQUATERNION V3XPTS;
		     // Projected vector (same struc as quaternion)
typedef struct _v3xwpts{
    V3XSCALAR uow, vow, oow;
}V3XWPTS;            // Homogenous coordinates

typedef struct _v3xuv{
    V3XSCALAR u, v;
}V3XUV;              // Texture coordinates

typedef struct _v3xrange{
    V3XSCALAR min, max;
}V3XRANGE;           // Range

typedef struct _v3xaxis{
    V3XVECTOR I, J, K;      // 3-axis
    V3XVECTOR Pos;        // Position
    V3XVECTOR reserved;   // resrved
}V3XAXIS;

typedef union _v3xmatrix{
    V3XSCALAR  Matrix[16]; // 4 * 3 + 3 reserved
    V3XAXIS   v;
}V3XMATRIX;
#endif

