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

#include <stdio.h>
#include <stdlib.h>
#include "_rlx32.h"
#include "v3xdefs.h"
#include "v3xrend.h"
#include "v3xproto.h"
#include "gx_struc.h"
#include "gx_rgb.h"

typedef struct{
int32_t S, PS, CRT, E, PE;
}PasTriangle;

typedef struct{
float S, PS, CRT, E, PE;
}PasTriangle2;

#define SWAP(x, y) {int32_t Z=x;x=y;y=Z;}
#define TRI_VAR \
PasTriangle Xg; \
int32_t n0=0, n1=1, n2=2, somCount=1, k, y0, yF, superDY, Pper;\
int32_t DY, y, x0;\
int32_t XL, swappy;\
V3XPTS *pt = fce->dispTab;\
u_int8_t *ptr, *off_ptr;

/**/
#define MASTER_1 \
MapAgain: \
k = 1; \
while (k != 0)\
{ \
if (pt[n1].y < pt[n0].y) SWAP (n0, n1)\
else\
if (pt[n2].y < pt[n1].y) SWAP (n1, n2)\
else k = 0;\
} \
swappy = (pt[n1].x<pt[n2].x);\
y0 = pt[n0].y; \
yF = pt[n2].y; \
superDY = yF - y0;\
if (!superDY) goto NextOne; \
XL = pt[n1].y - pt[n0].y; \
Pper = VDIV(XL, superDY);\
Xg.S = SHLD(pt[n0].x);\
Xg.PS = VDIV(pt[n2].x - pt[n0].x, superDY);\
Xg.CRT = SHL16(pt[n1].x - (pt[n0].x + VMUL(Xg.PS, XL)));\
if (!Xg.CRT) goto NextOne;

/**/


/**/
#define MASTER_2 \
while (y0 < yF)\
{\
DY = pt[n1].y - pt[n0].y;\
if (DY == 0) goto MapNextTriangle;\
Xg.PE = VDIV(pt[n1].x - pt[n0].x, DY) ;\
Xg.E = SHLD(pt[n0].x);\
XL = y0 + DY;\

/**/
#define MASTER_3
/**/
#define MASTER_4 \
if (DY > 0)\
{\
off_ptr = g_pRLX->pGX->View.ptr + y0*g_pRLX->pGX->View.BytesPerLine;\
for (y=DY;y!=0;y--)\
{\


/**/
#define TRI_BORD \
if (!swappy) \
{ \
x0 = SHRD(Xg.S);\
XL = SHRD(Xg.E) - x0;\
}\
else\
{\
x0 = SHRD(Xg.E);\
XL = SHRD(Xg.S) - x0;
/**/
#define CLIP_BORD\
}
/**/
#define INNER_LOOP \
if (XL>0) \
{\
int32_t x;\
ptr = off_ptr + x0;\
for (x=XL;x!=0;ptr++, x--)
/**/
#define FINAL \
Xg.S += Xg.PS;\
Xg.E += Xg.PE;\
of
f_ptr+= g_pRLX->pGX->View.BytesPerLine;\
} \
MapNextTriangle: \
n0 = n1; \
n1 = n2; \
y0 += DY; \
}\
}\
NextOne:\
if (somCount+2<fce->numEdges) \
{\
somCount++; n0 = 0; n1 = somCount; n2 = somCount+1; goto MapAgain;\
}\

