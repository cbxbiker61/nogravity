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

#define VAR_MAPZ \
V3XWPTS *ZTab = fce->ZTab; \
V3XTEXTURESW *textureZ = (V3XTEXTURESW*)fce->Mat->texture[0].handle; \
PasTriangle2 Vgz, Ugz, Wgz; \
float Uz, Vz, Wz, dVxz, pVxz, dUxz, pUxz, dWxz, pWxz;

#define CALC_MAPZ \
{\
float eXLz, \
Perz = (float)Pper/65536.f, \
sup = 1.f/(float)superDY;\
Ugz.S = ZTab[n0].uow;\
eXLz = ZTab[n2].uow - ZTab[n0].uow;\
Ugz.PS = eXLz*sup;\
Ugz.CRT = (ZTab[n1].uow - (ZTab[n0].uow + (Perz* eXLz)));\
\
Vgz.S = ZTab[n0].vow;\
eXLz = ZTab[n2].vow - ZTab[n0].vow; \
Vgz.PS = eXLz*sup;\
Vgz.CRT = (ZTab[n1].vow - (ZTab[n0].vow + (Perz* eXLz)));\
\
Wgz.S = ZTab[n0].oow;\
eXLz = ZTab[n2].oow - ZTab[n0].oow; \
Wgz.PS = eXLz*sup;\
Wgz.CRT = (ZTab[n1].oow - (ZTab[n0].oow + (Perz* eXLz)));}

/**/
#define CST_MAPZ \
{\
float eXLz = 65536.f/(float)Xg.CRT;\
dVxz = Vgz.CRT*eXLz;\
dUxz = Ugz.CRT*eXLz;\
dWxz = Wgz.CRT*eXLz;\
eXLz = (float)(1<<PIXAR);\
pVxz = dVxz*eXLz;\
pUxz = dUxz*eXLz;\
pWxz = dWxz*eXLz;}

/**/
#define START_MAPZ {\
Uz = Ugz.S;\
Vz = Vgz.S;\
Wz = Wgz.S;}


#define ADD_MAPZ {\
Ugz.S += Ugz.PS; \
Vgz.S += Vgz.PS; \
Wgz.S += Wgz.PS; }


#define MAP_Z_CALC { \
float Xz = 65536.f/Wz; \
u1 = (int32_t)(Uz*Xz); \
v1 = (int32_t)(Vz*Xz); }


#define CORRZ_LOOP \
if (XL>0)\
{ \
float divXz;\
int32_t u0, v0, u1, v1; \
int32_t deX, deY, i, j; \
ptr = off_ptr + (x0<<DEFSHL);\
MAP_Z_CALC \
Wz += pWxz;\
Uz += pUxz;\
Vz += pVxz;\
divXz = 65536.f/Wz;\
for (i=(1+(XL>>PIXAR));i!=0;XL-=(1<<PIXAR), i--)\
{\
u0 = u1; \
v0 = v1; \
u1 = (int32_t)(Uz*divXz); \
v1 = (int32_t)(Vz*divXz); \
Uz += pUxz; \
Vz += pVxz; \
Wz += pWxz; \
divXz = 65536.f/Wz; \
deX = (u1-u0)>>PIXAR; \
deY = (v1-v0)>>PIXAR; \
for (j=XL>(1<<PIXAR)?(1<<PIXAR):XL;j!=0;ptr+=DEFSIZE, j--)


