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
//-------------------------------------------------------------------------/

#ifndef __V3XMATHS_H
#define __V3XMATHS_H

#include "v3xtrig.h"

static inline V3XSCALAR V3XVector_DotProduct(const V3XVECTOR *a, const V3XVECTOR *b)
{
	return MULF32(a->x, b->x) + MULF32(a->y, b->y) + MULF32(a->z, b->z);
}

static inline V3XSCALAR V3XVector_LengthSq(const V3XVECTOR *a)
{
	return V3XVector_DotProduct(a, a);
}

static inline V3XSCALAR V3XVector_Length(const V3XVECTOR *a)
{
	return sqrtf(V3XVector_LengthSq(a));
}

static inline void V3XVector_CrossProduct(V3XVECTOR *r, const V3XVECTOR *a, const V3XVECTOR *b)
{
	r->x = MULF32(a->y, b->z) - MULF32 (a->z, b->y);
    r->y = MULF32(a->z, b->x) - MULF32 (a->x, b->z);
    r->z = MULF32(a->x, b->y) - MULF32 (a->y, b->x);
}

static inline void V3XVector_Set(V3XVECTOR *P, V3XSCALAR X, V3XSCALAR Y, V3XSCALAR Z)
{
    P->x = X;
	P->y = Y;
	P->z = Z;
}

static inline void  V3XMatrix_GetCol0(V3XVECTOR *P, const V3XMATRIX *M)
{
    P->x = M->Matrix[0];
    P->y = M->Matrix[3];
    P->z = M->Matrix[6];
}

static inline void  V3XMatrix_GetCol1(V3XVECTOR *P, const V3XMATRIX *M)
{
    P->x = M->Matrix[1];
    P->y = M->Matrix[4];
    P->z = M->Matrix[7];
}

static inline void  V3XMatrix_GetCol2(V3XVECTOR *P, const V3XMATRIX *M)
{
    P->x = M->Matrix[2];
    P->y = M->Matrix[5];
    P->z = M->Matrix[8];
}


#define V3XVector_Cpy(R, B) {\
    R.x = B.x; R.y = B.y; R.z = B.z;\
}

static inline void V3XVector_Neg(V3XVECTOR *r)
{
    r->x = -r->x;
    r->y = -r->y;
    r->z = -r->z;
}

static inline void V3XVector_Sum(V3XVECTOR *r, const V3XVECTOR* a, const V3XVECTOR *b)
{
    r->x = a->x + b->x;
    r->y = a->y + b->y;
    r->z = a->z + b->z;
}

static inline void V3XVector_Dif(V3XVECTOR *r, const V3XVECTOR* a, const V3XVECTOR *b)
{
    r->x = a->x - b->x;
    r->y = a->y - b->y;
    r->z = a->z - b->z;
}

static inline void V3XVector_Inc(V3XVECTOR *r, const V3XVECTOR* a)
{
    r->x += a->x ;
    r->y += a->y ;
    r->z += a->z ;
}

static inline void V3XVector_Dec(V3XVECTOR *r, const V3XVECTOR* a)
{
    r->x -= a->x ;
    r->y -= a->y ;
    r->z -= a->z ;
}

static inline void V3XVector_Avg(V3XVECTOR *r, const V3XVECTOR* a, const V3XVECTOR *b)
{
    r->x = (a->x + b->x)/2;
    r->y = (a->y + b->y)/2;
    r->z = (a->z + b->z)/2;
}


static inline void V3XVector_Scl(V3XVECTOR *r, const V3XVECTOR *a, V3XSCALAR k)
{
    r->x = MULF32(a->x, k);
    r->y = MULF32(a->y, k);
    r->z = MULF32(a->z, k);
}

static inline void V3XVector_Div(V3XVECTOR *r, V3XSCALAR k)
{
    V3XSCALAR xNor = DIVF32(CST_ONE, k);
    V3XVector_Scl(r, r, xNor);
}

static inline void V3XVector_Madd(V3XVECTOR *r, const V3XVECTOR *a, const V3XVECTOR *b, V3XSCALAR k)
{
    r->x = MULF32(a->x, k) + b->x;
    r->y = MULF32(a->y, k) + b->y;
    r->z = MULF32(a->z, k) + b->z;
}

static inline void V3XVector_PolarSet(V3XVECTOR *a, const V3XVECTOR *b, V3XSCALAR r, int32_t al, int32_t bl)
{
    a->x = b->x + MULF32(MULF32(sin16(al), cos16(bl)), r);
    a->y = b->y + MULF32(MULF32(cos16(al), cos16(bl)), r);
    a->z = b->z + MULF32(sin16(bl), r);
}

#define V3XSCALAR_Blend(r, a, b, kA, kB)\
{\
    V3XSCALAR delt = DIVF32((kA), (kB));\
    (r) = (a) - MULF32((V3XSCALAR)((a) - (b)), delt);\
}

static inline void V3XVector_Slerp(V3XVECTOR *r, const V3XVECTOR *a, const V3XVECTOR *b, V3XSCALAR k)
{
    r->x = a->x - MULF32((V3XSCALAR)(a->x - b->x), k);
    r->y = a->y - MULF32((V3XSCALAR)(a->y - b->y), k);
    r->z = a->z - MULF32((V3XSCALAR)(a->z - b->z), k);
}

static inline void V3XVector_ProjectOnVector(V3XVECTOR *r, const V3XVECTOR *V, const V3XVECTOR* N)
{
    V3XVector_Madd(r, N, V, -V3XVector_DotProduct (V, N));
}

static inline V3XSCALAR V3XVector_DistanceSq(const V3XVECTOR *a, const V3XVECTOR *b)
{
    V3XVECTOR temp;
    V3XVector_Dif(&temp, a, b);
    return V3XVector_LengthSq(&temp);
}

static inline V3XSCALAR V3XVector_Distance(const V3XVECTOR *a, const V3XVECTOR *b)
{
    V3XVECTOR temp;
    V3XVector_Dif(&temp, a, b);
    return V3XVector_Length(&temp);
}

static inline V3XSCALAR V3XVector_Normalize(V3XVECTOR *a, const V3XVECTOR *b)
{
    V3XSCALAR N = V3XVector_LengthSq(b);
    if (N < CST_EPSILON)
    {
        if ((!b->x)&&(!b->y))
			V3XVector_Set(a, CST_ZERO, CST_ZERO, CST_ONE);
        else
        if ((!b->x)&&(!b->z))
			V3XVector_Set(a, CST_ZERO, CST_ONE, CST_ZERO);
        else
		V3XVector_Set(a, CST_ONE, CST_ZERO, CST_ZERO);
		return 1.f;
    }
    else
    {
        float n = rsqrtf(N);
        V3XVector_Scl(a, b, n);
		return N * n;
    }
    return N;
}


//**********************************************************************/
// Projection
//**********************************************************************/

#ifdef V3X_FLOATINGPOINT

#define V3XViewport_Setup(pCamera, View)\
{\
    V3X.ViewPort.center.x = (float)((View.xmax+View.xmin)>>1);\
    V3X.ViewPort.center.y = (float)((View.ymax+View.ymin)>>1); \
    V3X.ViewPort.Focal = (*pCamera).focal * (float)(View.ymin - View.ymax) /480.f;\
    V3X.ViewPort.Ratio = (float)View.lRatio*(1.f/16.f);\
}
#define V3XVector_ProjectWithCenter(R, A)\
{\
    V3XSCALAR sx;\
    (R).z = xINV((A).z); sx = V3X.ViewPort.Focal * (R).z;\
    (R).y = V3X.ViewPort.center.y - MULF32((A).y, sx);\
    (R).x = V3X.ViewPort.center.x + MULF32((A).x, sx*V3X.ViewPort.Ratio);\
}
#define V3XVector_ProjectWithoutCenter(R, A)\
{\
    V3XSCALAR sx;\
    (R).z = xINV((A).z); sx = V3X.ViewPort.Focal * (R).z;\
    (R).y = MULF32((A).y, sx);\
    (R).x = MULF32((A).x, sx * V3X.ViewPort.Ratio);\
}

#else

#define V3XViewport_Setup(pCamera, View)\
{\
    V3X.ViewPort.center.x = (View.xmax+View.xmin)>>1;\
    V3X.ViewPort.center.y = (View.ymax+View.ymin)>>1; \
    V3X.ViewPort.Focal = xSHLD((*pCamera).focal*(View.ymin-View.ymax), 16);\
    V3X.ViewPort.Ratio = View.lRatio;\
}
#define V3XVector_ProjectWithCenter(R, A)\
{\
    V3XSCALAR sx = V3X.ViewPort.Focal / (A).z;\
    (R).y = V3X.ViewPort.center.y - VMUL((A).y, sx);\
    (R).x = V3X.ViewPort.center.x + VMUL((A).x, xSHRD(sx*GX.View.lRatio, 4));\
}
#define V3XVector_ProjectWithoutCenter(R, A)\
{\
    V3XSCALAR sx = V3X.ViewPort.Focal / (A).z;\
    (R).y = MULF32((A).y, sx);\
    (R).x = MULF32((A).x, xSHRD(sx*GX.View.lRatio, 4));\
}

#endif

#define V3XVector_ProjectWithCenterAndTest(R, A, flag)\
if ((A).z<CST_ZERO) \
{\
    V3XVector_ProjectWithCenter(R, A);\
} else (flag) = 2;


#define V3XMatrix_Transpose(R, A)\
{\
    R[0] = A[0]; R[3]=A[1]; R[6]=A[2];\
    R[1] = A[3]; R[4]=A[4]; R[7]=A[5];\
    R[2] = A[6]; R[5]=A[7]; R[8]=A[8];\
}
#define V3XMatrix_SetScale(M, scale)\
{\
    M[0] = scale  ;M[1] = CST_ZERO ;M[2] = CST_ZERO;\
    M[3] = CST_ZERO ;M[4] = scale  ;M[5] = CST_ZERO;\
    M[6] = CST_ZERO ;M[7] = CST_ZERO ;M[8] = scale;\
}

#define V3XMatrix_SetIdentity(M) V3XMatrix_SetScale(M, CST_ONE)

#define V3X_PROD_VECT_x9MATRIX(w, v, M)\
{\
    *(w+0) =  MULF32(M[0], *(v+0)) + MULF32(M[1], *(v+3)) + MULF32(M[2], *(v+6));\
    *(w+3) =  MULF32(M[3], *(v+0)) + MULF32(M[4], *(v+3)) + MULF32(M[5], *(v+6));\
    *(w+6) =  MULF32(M[6], *(v+0)) + MULF32(M[7], *(v+3)) + MULF32(M[8], *(v+6));\
}

#define V3XMatrix_Multiply3x3(R, A, B) \
{ \
    V3X_PROD_VECT_x9MATRIX(R , B , A); \
    V3X_PROD_VECT_x9MATRIX(R+1, B+1, A); \
    V3X_PROD_VECT_x9MATRIX(R+2, B+2, A); \
}

#define V3XMatrix_Multiply3x4(R, A, B) \
{ \
    V3XMatrix_Multiply3x3((R).Matrix, (A).Matrix, (B).Matrix);\
    V3XVector_ApplyMatrixTrans((R).v.Pos, (B).v.Pos, (A).Matrix); \
}

#define V3XVector_ApplyMatrix(R, A, M)\
{\
    (R).x =  MULF32(M[0], (A).x) + MULF32(M[1], (A).y) + MULF32(M[2], (A).z);\
    (R).y =  MULF32(M[3], (A).x) + MULF32(M[4], (A).y) + MULF32(M[5], (A).z);\
    (R).z =  MULF32(M[6], (A).x) + MULF32(M[7], (A).y) + MULF32(M[8], (A).z);\
}

#define V3XVector_ApplyMatrixTrans(R, A, M)\
{\
    (R).x =  MULF32(M[0], (A).x) + MULF32(M[1], (A).y) + MULF32(M[2], (A).z) + M[ 9];\
    (R).y =  MULF32(M[3], (A).x) + MULF32(M[4], (A).y) + MULF32(M[5], (A).z) + M[10];\
    (R).z =  MULF32(M[6], (A).x) + MULF32(M[7], (A).y) + MULF32(M[8], (A).z) + M[11];\
}

#define V3XVector_ApplyTransposeMatrix(R, A, M)\
{\
    (R).x =  MULF32(M[0], (A).x) + MULF32(M[3], (A).y) + MULF32(M[6], (A).z);\
    (R).y =  MULF32(M[1], (A).x) + MULF32(M[4], (A).y) + MULF32(M[7], (A).z);\
    (R).z =  MULF32(M[2], (A).x) + MULF32(M[5], (A).y) + MULF32(M[8], (A).z);\
}

#define V3XVector_ApplyTransposeMatrixTrans(R, A, M)\
{\
    (R).x =  MULF32(M[0], (A).x) + MULF32(M[3], (A).y) + MULF32(M[6], (A).z) - M[ 9];\
    (R).y =  MULF32(M[1], (A).x) + MULF32(M[4], (A).y) + MULF32(M[7], (A).z) - M[10];\
    (R).z =  MULF32(M[2], (A).x) + MULF32(M[5], (A).y) + MULF32(M[8], (A).z) - M[11];\
}

#define V3XMatrix_Rot_XYZ(M, rot)\
{ \
    V3XSCALAR cx =  cos16(rot.y), cy=cos16(rot.z), cz=cos16(rot.x), \
	     sx =  sin16(rot.y), sy=sin16(rot.z), sz=sin16(rot.x);\
    \
    M[0] =  MULF32(cy, cx) ;\
    M[1] =  MULF32(cy, sx) ;\
    M[2] =  -sy;\
    \
    M[3] =  MULF32(sz, MULF32(sy, cx))-MULF32(cz, sx);\
    M[4] =  MULF32(sx, MULF32(sy, sz))+MULF32(cz, cx);\
    M[5] =  MULF32(cy, sz);\
    \
    M[6] =  MULF32(cz, MULF32(sy, cx))+MULF32(sz, sx);\
    M[7] =  MULF32(cz, MULF32(sy, sx))-MULF32(sz, cx);\
    M[8] =  MULF32(cy, cz);\
}

#define V3XMatrix_Rot_X(M, angle)\
{\
    V3XSCALAR c = cos16(angle), s = sin16(angle);\
    M[0] =  CST_ONE; M[1] =  CST_ZERO; M[2] =  CST_ZERO; \
    M[3] =  CST_ZERO;     M[4] =  c; M[5] = -s; \
    M[6] =  CST_ZERO;     M[7] =  s; M[8] =  c; \
}

#define V3XMatrix_Rot_Y(M, angle)\
{\
    V3XSCALAR c = cos16(angle), s=sin16(angle);\
    M[0] =  c;     M[1] =  CST_ZERO;     M[2] =  s; \
    M[3] =  CST_ZERO; M[4] =  CST_ONE;    M[5] =  CST_ZERO; \
    M[6] = -s;     M[7] =  CST_ZERO;     M[8] =  c; \
}

#define V3XMatrix_Rot_Z(M, angle)\
{\
    V3XSCALAR c = cos16(angle), s = sin16(angle);\
    M[0] =  c; M[1] = -s; M[2] =  CST_ZERO; \
    M[3] =  s; M[4] =  c; M[5] =  CST_ZERO; \
    M[6] =  CST_ZERO; M[7] =  CST_ZERO; M[8] =  CST_ONE; \
}

__extern_c
_RLXEXPORTFUNC    void    RLXAPI  V3XMatrix_Rotate_X(int32_t Theta, V3XSCALAR *Matrice);
_RLXEXPORTFUNC    void    RLXAPI  V3XMatrix_Rotate_Y(int32_t Theta, V3XSCALAR *Matrice);
_RLXEXPORTFUNC    void    RLXAPI  V3XMatrix_Rotate_Z(int32_t Theta, V3XSCALAR *Matrice);
_RLXEXPORTFUNC    void    RLXAPI  V3XMatrix_Rotate_X_Local(int32_t Theta, V3XSCALAR *Matrice);
_RLXEXPORTFUNC    void    RLXAPI  V3XMatrix_Rotate_Y_Local(int32_t Theta, V3XSCALAR *Matrice);
_RLXEXPORTFUNC    void    RLXAPI  V3XMatrix_Rotate_Z_Local(int32_t Theta, V3XSCALAR *Matrice);
__end_extern_c

#endif

