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

#define TSWAP_MAP \
Ug.S = SHLD(texcoord0[n0].u)>>1;\
Ug.PS = VDIV((texcoord0[n1].u>>1) - (texcoord0[n0].u>>1), DY);\
Vg.S = SHLD(texcoord0[n0].v)>>1;\
Vg.PS = VDIV((texcoord0[n1].v>>1) - (texcoord0[n0].v>>1), DY);\

#define TSWAP_MAP2 \
U0g.S = SHLD(texcoord1[n0].u)>>1;\
U0g.PS = VDIV((texcoord1[n1].u>>1) - (texcoord1[n0].u>>1), DY);\
V0g.S = SHLD(texcoord1[n0].v)>>1;\
V0g.PS = VDIV((texcoord1[n1].v>>1) - (texcoord1[n0].v>>1), DY);\

#define CALC_IT \
Ig.S = SHLD(shade[n0]);\
XL = shade[n2] - shade[n0]; \
Ig.PS = VDIV(XL, superDY);\
Ig.CRT = SHL16(shade[n1] - (shade[n0] + VMUL(Pper, XL)));

/**/
#define CALC_MAP \
Ug.S = SHLD(texcoord0[n0].u)>>1;\
XL = (texcoord0[n2].u - texcoord0[n0].u)>>1;\
Ug.PS = VDIV(XL, superDY);\
Ug.CRT = SHL16((texcoord0[n1].u>>1) - ((texcoord0[n0].u>>1) + VMUL(Pper, XL)));\
Vg.S = SHLD(texcoord0[n0].v)>>1;\
XL = (texcoord0[n2].v - texcoord0[n0].v)>>1; \
Vg.PS = VDIV(XL, superDY);\
Vg.CRT = SHL16((texcoord0[n1].v>>1) - ((texcoord0[n0].v>>1) + VMUL(Pper, XL)));

/**/
#define CALC_MAP2 \
U0g.S = SHLD(texcoord0[n0].u)>>1;\
XL = (texcoord0[n2].u - texcoord0[n0].u)>>1;\
U0g.PS = VDIV(XL, superDY);\
U0g.CRT = SHL16((texcoord0[n1].u>>1) - ((texcoord0[n0].u>>1) + VMUL(Pper, XL)));\
V0g.S = SHLD(texcoord0[n0].v)>>1;\
XL = (texcoord0[n2].v - texcoord0[n0].v)>>1; \
V0g.PS = VDIV(XL, superDY);\
V0g.CRT = SHL16((texcoord0[n1].v>>1) - ((texcoord0[n0].v>>1) + VMUL(Pper, XL)));


/**/
#define CST_IT \
dIx = VDIV(Ig.CRT, Xg.CRT);
/**/
#define CST_MAP \
dVx = VDIV(Vg.CRT, Xg.CRT);\
dUx = VDIV(Ug.CRT, Xg.CRT);
/**/
#define CST_MAP2 \
dV0x = VDIV(V0g.CRT, Xg.CRT);\
dU0x = VDIV(U0g.CRT, Xg.CRT);


/**/
#define START_IT It = Ig.S;
/**/
#define START_MAP U = Ug.S;\
V = Vg.S;
/**/
#define START_MAP2 U0 = U0g.S;\
V0 = V0g.S;

/**/
#define ADD_IT Ig.S += Ig.PS;
/**/
#define ADD_MAP Ug.S += Ug.PS; \
Vg.S += Vg.PS;
/**/
#define ADD_MAP2 U0g.S += U0g.PS; \
V0g.S += V0g.PS;

#define VAR_IT int32_t *shade = (int32_t*)fce->shade; \
PasTriangle Ig;\
int32_t dIx, It;

#define VAR_MAP V3XlUV *texcoord0 = (V3XlUV*)fce->uvTab[0]; \
V3XTEXTURESW *texture = (V3XTEXTURESW*)fce->Mat->texture[0].handle; \
PasTriangle Vg, Ug; \
int32_t U, V, dVx, dUx;

#define VAR_MAP2 V3XlUV *texcoord1 = (V3XlUV*)fce->uvTab[1]; \
V3XTEXTURESW *texture2 = (V3XTEXTURESW*)fce->Mat->texture[1].handle; \
PasTriangle V0g, U0g; \
int32_t U0, V0, dV0x, dU0x;



