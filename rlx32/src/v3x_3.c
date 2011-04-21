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
#include <stdlib.h>
#include <stdio.h>
#include "_rlx32.h"
#include "sysresmx.h"
#include "systools.h"
/*****/
#include "gx_struc.h"
#include "gx_tools.h"
#include "gx_csp.h"
/*****/
#include "fixops.h"
#include "v3x_priv.h"
#include "v3xdefs.h"
#include "v3x_1.h"
#include "v3x_2.h"
#include "v3xtrig.h"
#include "v3xmaps.h"
#include "v3xrend.h"

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void V3XMatrix_Rotate_X(int32_t Theta, V3XSCALAR *Matrice)
{
    V3XSCALAR sin, cos;
    V3XSCALAR M[9];
    sin = sin16(Theta);
    cos = cos16(Theta);
    M[3]=MULF32(cos, Matrice[3])-MULF32(sin, Matrice[6]);
    M[4]=MULF32(cos, Matrice[4])-MULF32(sin, Matrice[7]);
    M[5]=MULF32(cos, Matrice[5])-MULF32(sin, Matrice[8]);
    M[6]=MULF32(sin, Matrice[3])+MULF32(cos, Matrice[6]);
    M[7]=MULF32(sin, Matrice[4])+MULF32(cos, Matrice[7]);
    M[8]=MULF32(sin, Matrice[5])+MULF32(cos, Matrice[8]);
    Matrice[3]=M[3];
    Matrice[4]=M[4];
    Matrice[5]=M[5];
    Matrice[6]=M[6];
    Matrice[7]=M[7];
    Matrice[8]=M[8];
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMatrix_BuildFromVectorUP(V3XMATRIX *M0, V3XVECTOR *v, V3XVECTOR *Source)
*
* Description :
*
*/
void V3XMatrix_BuildFromVectorUP(V3XMATRIX *M0, V3XVECTOR *v, V3XVECTOR *Source)
{
    V3XVector_Normalize(&M0->v.K, v);
    V3XVector_ProjectOnVector(&M0->v.J, Source, &M0->v.K);
    V3XVector_Normalize (&M0->v.J, &M0->v.J);
    V3XVector_CrossProduct(&M0->v.I, &M0->v.J, &M0->v.K);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMatrix_BuildFromVector(V3XMATRIX *M0, V3XVECTOR *v, int roll)
*
* DESCRIPTION :
*
*/
void V3XMatrix_BuildFromVector(V3XMATRIX *M0, V3XVECTOR *v, int roll)
{
    V3XVECTOR Source;
    V3XVector_Normalize(&M0->v.K, v);
    Source.x = CST_ZERO;
    Source.y = CST_ZERO;
    Source.z = CST_ONE;
    V3XVector_ProjectOnVector(&M0->v.J, &Source, &M0->v.K);
	if (V3XVector_LengthSq(&M0->v.J)<CST_EPSILON)
		V3XVector_Set(&M0->v.J, 0, 1, 0);
	else
		V3XVector_Normalize (&M0->v.J, &M0->v.J);
    V3XVector_CrossProduct(&M0->v.I,& M0->v.J,& M0->v.K);
    if (roll)
		V3XMatrix_Rotate_Z(roll, M0->Matrix);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMatrix_BuildFromNVector(V3XMATRIX *M0, V3XVECTOR *v, int roll)
*
* DESCRIPTION :
*
*/
void V3XMatrix_BuildFromNVector(V3XMATRIX *M0, V3XVECTOR *v, int roll)
{
    V3XVECTOR Source;
    V3XVector_Normalize (&M0->v.K, v);
    Source.x = CST_ZERO;
    Source.y = CST_ZERO;
    Source.z = CST_ONE;
    V3XVector_ProjectOnVector(&M0->v.J, &Source, &M0->v.K);
	if (V3XVector_LengthSq(&M0->v.J)>CST_EPSILON)
		V3XVector_Normalize (&M0->v.J, &M0->v.J) ;
	else
		V3XVector_Set(&M0->v.J, 0, 1, 0);
    V3XVector_CrossProduct(&M0->v.I, &M0->v.J, &M0->v.K);
    if (roll) V3XMatrix_Rotate_Z(roll, M0->Matrix);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XSCALAR V3XVector_DistanceToPlane(V3XVECTOR *Point, V3XVECTOR *PlanePoint, V3XVECTOR *PlaneNormal)
*
* DESCRIPTION :
*
*/
V3XSCALAR V3XVector_DistanceToPlane(V3XVECTOR *Point, V3XVECTOR *PlanePoint, V3XVECTOR *PlaneNormal)
{
    V3XVECTOR point_to_plane;
    V3XVector_Dif( &point_to_plane, (Point), (PlanePoint));
    return V3XVector_DotProduct((PlaneNormal), &point_to_plane);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XVector_IntersectPointPlane(V3XVECTOR *isect, V3XVECTOR *Point, V3XVECTOR *PlanePoint, V3XVECTOR *PlaneNormal)
*
* DESCRIPTION :
*
*/
V3XSCALAR V3XVector_IntersectPointPlane(V3XVECTOR *isect, V3XVECTOR *Point, V3XVECTOR *PlanePoint, V3XVECTOR *PlaneNormal)
{
    V3XSCALAR k = V3XVector_DistanceToPlane(Point, PlanePoint, PlaneNormal);
    V3XVector_Madd(isect, PlaneNormal, Point, k);
    return k;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3XVector_TransformProject_pts(V3XVECTOR *input, V3XVECTOR *result)
*
* DESCRIPTION :
*
*/

#define V3X_TRANSFORM_CAMERA(R, A, Cam)\
{\
    V3XVECTOR VV;\
    V3XVector_Dif(&VV, A, &Cam.M.v.Pos);\
    V3XVector_ApplyMatrix((R), VV, Cam.M.Matrix);\
}

int V3XVector_TransformProject_pts(V3XVECTOR *input, V3XVECTOR *result)
{
    uint8_t flag=0;
    V3XVECTOR temp;
    V3X_TRANSFORM_CAMERA(temp, input, V3X.Camera);
    V3XVector_ProjectWithCenterAndTest((*result), temp, flag);
    result->z = temp.z;
    return flag==0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3XVector_Project(V3XVECTOR *result, V3XVECTOR *input)
*
* DESCRIPTION :
*
*/
int V3XVector_Project(V3XVECTOR *result, V3XVECTOR *input)
{
    V3XVector_ProjectWithoutCenter((*result), (*input));
    return 1;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XSCALAR V3XVector_ProjectPointLine(V3XVECTOR *res, V3XVECTOR *v, V3XVECTOR *e0, V3XVECTOR *e1)
*
* DESCRIPTION :
*
*/
int V3XVector_IntersectPlaneSegmentEx(V3XVECTOR * isect, V3XVECTOR * start, V3XVECTOR * end, V3XVECTOR *normal, V3XSCALAR d, V3XSCALAR* pr)
{
    V3XVECTOR dv;
    V3XSCALAR r, num, denom;
    V3XVector_Dif(&dv, end, start);
    denom = V3XVector_DotProduct(normal, &dv);
    if (fabs(denom) < CST_EPSILON)
    {
        return 0; // la droite est parrallele au plan
    }
    num = -V3XVector_DotProduct(normal, start) - d;
    r =  DIVF32(num, denom);
    if (pr) *pr = r;
    V3XVector_Madd(isect, &dv, start, r);
    if (r < CST_ZERO || r > CST_ONE) return 0;
    return 1;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3XVector_IntersectPlaneSegment(V3XVECTOR * isect, V3XVECTOR * start, V3XVECTOR * end, V3XVECTOR *normal, V3XVECTOR *point)
*
* Description :
*
*/
int V3XVector_IntersectPlaneSegment(V3XVECTOR * isect, V3XVECTOR * start, V3XVECTOR * end, V3XVECTOR *normal, V3XVECTOR *point)
{
    V3XSCALAR d = -V3XVector_DotProduct(normal, point);
    int r =V3XVector_IntersectPlaneSegmentEx(isect, start, end, normal, d, NULL);
    return r;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XSCALAR V3XVector_ProjectPointLine(V3XVECTOR *res, V3XVECTOR *u, V3XVECTOR *e0, V3XVECTOR *e1)
*
* Description :
*
*/
V3XSCALAR V3XVector_ProjectPointLine(V3XVECTOR *res, V3XVECTOR *u, V3XVECTOR *e0, V3XVECTOR *e1)
{
    V3XVECTOR v, w;
    V3XSCALAR vl, wl;
    V3XVector_Dif(&v, u, e0);
    V3XVector_Dif(&w, e1, e0);
    wl = V3XVector_Normalize(&w, &w);
	vl = V3XVector_Normalize(&v, &v);

    if (wl>CST_EPSILON)
	{
		if ((wl>CST_EPSILON)&&(vl>CST_EPSILON))
		{
			V3XSCALAR cosa = V3XVector_DotProduct(&w, &v);
			if (fabs(cosa)<=CST_ONE)
			{
				V3XSCALAR sina = sqrtf(1 - cosa * cosa);
				cosa*=vl;
				V3XVector_Madd(res, &v, e0, cosa);
				return vl * sina;
			}
			else
			{
				*res = *u;
				return CST_ZERO;
			}
		}
		else
		{
			*res = *e0;
			return vl;
		}
	}
	return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3X_IntersectSphereSegment(V3XSCALAR radius, V3XVECTOR *center, V3XVECTOR *a, V3XVECTOR *b)
*
* DESCRIPTION :
*
*/
int V3XVector_IntersectSphereSegment(V3XSCALAR radius, V3XVECTOR *center, V3XVECTOR *a, V3XVECTOR *b)
{
    V3XVECTOR res;
    V3XSCALAR  dist = V3XVector_ProjectPointLine(&res, center, a, b);
    return (dist<=radius); //&&(dist!=CST_ZERO));
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMatrix_Rotate_Y(int32_t Theta, V3XSCALAR *Matrice)
*
* DESCRIPTION :
*
*/
void V3XMatrix_Rotate_Y(int32_t Theta, V3XSCALAR *Matrice)
{
    V3XSCALAR sin, cos;
    V3XSCALAR M[9];
    sin = (V3XSCALAR)sin16(Theta);
    cos = (V3XSCALAR)cos16(Theta);
    M[0]=MULF32(cos, Matrice[0])-MULF32(sin, Matrice[6]);
    M[1]=MULF32(cos, Matrice[1])-MULF32(sin, Matrice[7]);
    M[2]=MULF32(cos, Matrice[2])-MULF32(sin, Matrice[8]);
    M[6]=MULF32(sin, Matrice[0])+MULF32(cos, Matrice[6]);
    M[7]=MULF32(sin, Matrice[1])+MULF32(cos, Matrice[7]);
    M[8]=MULF32(sin, Matrice[2])+MULF32(cos, Matrice[8]);
    Matrice[0]=M[0];    Matrice[1]=M[1];    Matrice[2]=M[2];
    Matrice[6]=M[6];    Matrice[7]=M[7];    Matrice[8]=M[8];
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMatrix_Rotate_Z(int32_t Theta, V3XSCALAR *Matrice)
*
* DESCRIPTION :
*
*/
void V3XMatrix_Rotate_Z(int32_t Theta, V3XSCALAR *Matrice)
{
    V3XSCALAR sin, cos;
    V3XSCALAR M[9];
    sin=(V3XSCALAR)sin16(Theta);
    cos=(V3XSCALAR)cos16(Theta);
    M[0]=MULF32(cos, Matrice[0])-MULF32(sin, Matrice[3]);
    M[1]=MULF32(cos, Matrice[1])-MULF32(sin, Matrice[4]);
    M[2]=MULF32(cos, Matrice[2])-MULF32(sin, Matrice[5]);
    M[3]=MULF32(sin, Matrice[0])+MULF32(cos, Matrice[3]);
    M[4]=MULF32(sin, Matrice[1])+MULF32(cos, Matrice[4]);
    M[5]=MULF32(sin, Matrice[2])+MULF32(cos, Matrice[5]);
    Matrice[0]=M[0];    Matrice[1]=M[1];    Matrice[2]=M[2];
    Matrice[3]=M[3];    Matrice[4]=M[4];    Matrice[5]=M[5];
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMatrix_Rotate_X_Local(int32_t Theta, V3XSCALAR *Matrice)
*
* DESCRIPTION :
*
*/
void V3XMatrix_Rotate_X_Local(int32_t Theta, V3XSCALAR *Matrice)
{
    V3XSCALAR sin, cos;
    V3XSCALAR M[9];
    sin=(V3XSCALAR)sin16(Theta);
    cos=(V3XSCALAR)cos16(Theta);
    M[1]=MULF32(cos, Matrice[1])-MULF32(sin, Matrice[2]);
    M[4]=MULF32(cos, Matrice[4])-MULF32(sin, Matrice[5]);
    M[7]=MULF32(cos, Matrice[7])-MULF32(sin, Matrice[8]);
    M[2]=MULF32(sin, Matrice[1])+MULF32(cos, Matrice[2]);
    M[5]=MULF32(sin, Matrice[4])+MULF32(cos, Matrice[5]);
    M[8]=MULF32(sin, Matrice[7])+MULF32(cos, Matrice[8]);
    Matrice[1]=M[1];    Matrice[4]=M[4];    Matrice[7]=M[7];
    Matrice[2]=M[2];    Matrice[5]=M[5];    Matrice[8]=M[8];
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMatrix_Rotate_Y_Local(int32_t Theta, V3XSCALAR *Matrice)
*
* DESCRIPTION :
*
*/
void V3XMatrix_Rotate_Y_Local(int32_t Theta, V3XSCALAR *Matrice)
{
    V3XSCALAR sin, cos;
    V3XSCALAR M[9];
    sin=(V3XSCALAR)sin16(Theta);
    cos=(V3XSCALAR)cos16(Theta);
    M[0]=MULF32(cos, Matrice[0])-MULF32(sin, Matrice[2]);
    M[3]=MULF32(cos, Matrice[3])-MULF32(sin, Matrice[5]);
    M[6]=MULF32(cos, Matrice[6])-MULF32(sin, Matrice[8]);
    M[2]=MULF32(sin, Matrice[0])+MULF32(cos, Matrice[2]);
    M[5]=MULF32(sin, Matrice[3])+MULF32(cos, Matrice[5]);
    M[8]=MULF32(sin, Matrice[6])+MULF32(cos, Matrice[8]);
    Matrice[0]=M[0];    Matrice[3]=M[3];
    Matrice[6]=M[6];    Matrice[2]=M[2];
    Matrice[5]=M[5];    Matrice[8]=M[8];
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMatrix_Rotate_Z_Local(int32_t Theta, V3XSCALAR *Matrice)
*
* DESCRIPTION :
*
*/
void V3XMatrix_Rotate_Z_Local(int32_t Theta, V3XSCALAR *Matrice)
{
    V3XSCALAR sin, cos;
    V3XSCALAR M[9];
    sin=(V3XSCALAR)sin16(Theta);
    cos=(V3XSCALAR)cos16(Theta);
    M[0]=MULF32(cos, Matrice[0])-MULF32(sin, Matrice[1]);
    M[3]=MULF32(cos, Matrice[3])-MULF32(sin, Matrice[4]);
    M[6]=MULF32(cos, Matrice[6])-MULF32(sin, Matrice[7]);
    M[1]=MULF32(sin, Matrice[0])+MULF32(cos, Matrice[1]);
    M[4]=MULF32(sin, Matrice[3])+MULF32(cos, Matrice[4]);
    M[7]=MULF32(sin, Matrice[6])+MULF32(cos, Matrice[7]);
    Matrice[0]=M[0];    Matrice[3]=M[3];
    Matrice[6]=M[6];    Matrice[1]=M[1];
    Matrice[4]=M[4];    Matrice[7]=M[7];
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XSCALAR V3XVector_AudioInfo(V3XVECTOR *v, V3XSCALAR *panning, V3XSCALAR *volume, V3XSCALAR maxDist)
*
* DESCRIPTION :
*
*/
V3XSCALAR V3XVector_AudioInfo(V3XVECTOR *v, V3XSCALAR *panning, V3XSCALAR *volume, V3XSCALAR maxDist)
{
    V3XSCALAR    distance;
    V3XVECTOR   VV;
    V3XVector_Dif(&VV, v, &V3X.Camera.M.v.Pos);
    distance = V3XVector_Normalize(&VV, &VV);
    *panning = V3XVector_DotProduct(&VV, &V3X.Camera.M.v.I);
    *volume = (distance<maxDist) ? CST_ONE : maxDist / distance;
    return distance;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  Determine la bounding box
*
* DESCRIPTION :
*
*/
void V3XBBox_Compute(V3XVECTOR *mini, V3XVECTOR *maxi, int numVerts, V3XVECTOR *vertex)
{
    int i;
    *mini = *maxi = *vertex;
    for (i=0;i<numVerts;i++, vertex++)
    {
        if (vertex->x>maxi->x) maxi->x = vertex->x;
        if (vertex->y>maxi->y) maxi->y = vertex->y;
        if (vertex->z>maxi->z) maxi->z = vertex->z;
        if (vertex->x<mini->x) mini->x = vertex->x;
        if (vertex->y<mini->y) mini->y = vertex->y;
        if (vertex->z<mini->z) mini->z = vertex->z;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
int V3XBBox_Inside(V3XVECTOR *vertex, V3XVECTOR *mini, V3XVECTOR *maxi)
{
    if (vertex->x>maxi->x) return 0;
    if (vertex->y>maxi->y) return 0;
    if (vertex->z>maxi->z) return 0;
    if (vertex->x<mini->x) return 0;
    if (vertex->y<mini->y) return 0;
    if (vertex->z<mini->z) return 0;
    return 1;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3X_IsV3XVECTORInPoly(V3XVECTOR * isect, int numVerts, V3XVECTOR *vertex)
*
* DESCRIPTION :  Est ce que le V3XVECTOR {isect} est dans le triangle  ?
*
*/
#define SUM_ANGLE
#define xacos(v) (V3XSCALAR)acos((double)(v))

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3XVector_InPoly(V3XVECTOR * isect, int numVerts, V3XVECTOR *vertex, V3XVECTOR *normal)
*
* Description :
*
*/
int V3XVector_InPoly(V3XVECTOR * isect, int numVerts, V3XVECTOR *vertex, V3XVECTOR *normal)
{
    V3XVECTOR mini, maxi;
    V3XVECTOR *st, *fin;
    V3XSCALAR sum = CST_ZERO;
    #ifdef SUM_ANGLE
    V3XSCALAR sumA = CST_ZERO;
    #endif
    int i;
    // phase 1 : bounding box de la face
    V3XBBox_Compute(&mini, &maxi, numVerts, vertex);
    // phase 2 : Si il est pas dans la bounding box, laisse tomber.
    //           (ca evite de faire des normalisation plus tard.)
    if (!V3XBBox_Inside(isect, &mini, &maxi))
    {
        return 0;
    }
    /* phase 3 : determine si le point dans le polygone
    */
    {
        st = vertex + numVerts - 1;
        fin = vertex + 0;
        for (i=0;i<numVerts;i++)
        {
            V3XVECTOR a, b;
            V3XSCALAR  c;
            V3XVector_Dif(&a, st, isect); c = V3XVector_Normalize(&a, &a); if (c<CST_EPSILON) return 1;
            V3XVector_Dif(&b, fin, isect); c = V3XVector_Normalize(&b, &b); if (c<CST_EPSILON) return 1;
            c = V3XVector_DotProduct(&a, &b);
            sum += c;
#ifdef SUM_ANGLE
            c = xacos(c); // Ca me fait chier ca.
            sumA +=c;
#endif
            st = fin;
            fin++;
        }
        /*
        if ((numVerts==3)&&(sum <-.9f*CST_ONE)) return 1;
        else
        */
        if (numVerts>=3)
        {
            if (sum<CST_ZERO) sum=-sum;
            if (sumA>350.f*M_PI180) return 1; // 360ø pour un n-gones … + de 3 cot‚s.
        }
    }
    UNUSED(normal);
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void CalculMatrixCamera(V3XREPERE *Cam)
*
* DESCRIPTION : Calcul la matrice de rotation d'un objet
*
*/
void V3XMatrix_MeshTransform(V3XCAMERA *Cam)
{
    V3XVECTOR Dir;
    switch(Cam->matrix_Method) {
        /* Calcul de la matrice par base orthornorme reel */
        case V3XMATRIX_Vector:
        V3XVector_Dif(&Dir, &Cam->Tk.vinfo.pos, &Cam->Tk.vinfo.target);
        V3XMatrix_BuildFromVector(&Cam->M, &Dir, (int)Cam->Tk.vinfo.roll);
        V3XVector_Cpy(&Cam->M.v.Pos, &Cam->Tk.vinfo.pos);
        // (Cam->M.Matrix);
        break;
        /* Calcul de la matrice avec les angles */
        case V3XMATRIX_Euler:
        V3XMatrix_Rot_XYZ(Cam->M.Matrix, Cam->Tk.info.angle);
        V3XVector_Cpy(&Cam->M.v.Pos, &Cam->Tk.vinfo.pos);
        // (Cam->M.Matrix);
        break;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  Fonctions cliposaures
*
* DESCRIPTION :
*
*/
#define LCLIPVALUE(r, a, b, t) {(r) = (a)+(V3XSCALAR)MULF32((b)-(a), t);}
#define LCLIPVALUE2(r, a, b, t) {(r) = (uint8_t)((a)+(V3XSCALAR)MULF32((b)-(a), t));}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3X_FaceCopy(V3XPOLY *clip, V3XPOLY *polygon)
*
* DESCRIPTION :
*
*/
static void V3X_FaceCopy(V3XPOLY *clip, V3XPOLY *polygon)
{
    V3XMATERIAL *Mat = (V3XMATERIAL*)polygon->Mat;
    // Transfer information from polygon structure to clipped polygon structure
    clip->Mat = polygon->Mat;
    clip->distance = polygon->distance;
    sysMemCpy(&clip->numEdges, &polygon->numEdges, 4);
    sysMemCpy(clip->dispTab, polygon->dispTab, polygon->numEdges*sizeof(V3XPTS));
    if (Mat)
    {
        if (Mat->info.Texturized)
        {
            sysMemCpy(clip->uvTab[0], polygon->uvTab[0], polygon->numEdges*sizeof(V3XUV));
            if (Mat->info.Perspective)
            {
                sysMemCpy(clip->ZTab  , polygon->ZTab, polygon->numEdges*sizeof(V3XWPTS));
            }
            if (Mat->info.Environment&V3XENVMAPTYPE_DOUBLE)
            {
                sysMemCpy(clip->uvTab[1], polygon->uvTab[1], polygon->numEdges*sizeof(V3XUV));
            }
        }
        if (Mat->info.Shade)
        {
            sysMemCpy(clip->shade , polygon->shade , polygon->numEdges*sizeof(V3XSCALAR));
        }
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static V3XPOLY *V3XPoly_Duplicate(V3XPOLY *clip)
*
* DESCRIPTION :
*
*/
V3XPOLY *V3XPoly_Duplicate(V3XPOLY *clip)
{
    V3XPOLY *nouv = V3XPoly_QAlloc();
    if (V3XPoly_QExhausted())
    {
        clip->visible = 0;
        return clip;
    }
    else
    {
        V3X.Buffer.MaxClipped++;
        V3X_FaceCopy(nouv, clip);
        if (clip->faceTab)
        {
            if (nouv->faceTab) sysMemCpy(nouv->faceTab, clip->faceTab, clip->numEdges*sizeof(int32_t));
            else nouv->faceTab = clip->faceTab;
        }
        return nouv;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void v3x_facedataCopy(V3XPOLY *clip, V3XPOLY *polygon, int32_t cp, int32_t v1)
*
* DESCRIPTION :
*
*/
static void v3x_facedataCopy(V3XPOLY *clip, V3XPOLY *polygon, int32_t cp, int32_t v1)
{
    V3XMATERIAL *Mat = (V3XMATERIAL*)polygon->Mat;
    clip->dispTab[ cp ] = polygon->dispTab[v1];
    if (!Mat) return;
    if (Mat->info.Texturized)
    {
        clip->uvTab[0][ cp ] = polygon->uvTab[0][v1] ;
        if (Mat->info.Perspective)
        {
            clip->ZTab[ cp ] = polygon->ZTab[ v1 ];
        }
        if (Mat->info.Environment&V3XENVMAPTYPE_DOUBLE)
        {
            clip->uvTab[1][ cp ] = polygon->uvTab[1][v1] ;
        }
    }
    if (Mat->info.Shade==2)
    {
        sysMemCpy(clip->shade+ cp, polygon->shade + v1, sizeof(V3XSCALAR));
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void v3x_facedataClip(V3XSCALAR t, V3XPOLY *clip, V3XPOLY *polygon, int32_t cp, int32_t v1, int32_t v2, V3XSCALAR z)
*
* DESCRIPTION :
*
*/
static void v3x_facedataClip(V3XSCALAR t, V3XPOLY *clip, V3XPOLY *polygon, int32_t cp, int32_t v1, int32_t v2, V3XSCALAR z)
{
    V3XMATERIAL *Mat = (V3XMATERIAL*)polygon->Mat;
    if (Mat)
    {
        if (Mat->info.Texturized)
        {
            LCLIPVALUE( clip->uvTab[0][ cp ].u , polygon->uvTab[0][v1].u, polygon->uvTab[0][v2].u, t);
            LCLIPVALUE( clip->uvTab[0][ cp ].v , polygon->uvTab[0][v1].v, polygon->uvTab[0][v2].v, t);
            if (Mat->info.Environment&V3XENVMAPTYPE_DOUBLE)
            {
                LCLIPVALUE( clip->uvTab[1][ cp ].u , polygon->uvTab[1][v1].u, polygon->uvTab[1][v2].u, t);
                LCLIPVALUE( clip->uvTab[1][ cp ].v , polygon->uvTab[1][v1].v, polygon->uvTab[1][v2].v, t);
            }
            if (Mat->info.Perspective)
            {
                if (z)
                {
                    clip->ZTab[cp].oow = -CST_ONE/z;
                    clip->ZTab[cp].uow = clip->uvTab[0][cp].u * clip->ZTab[cp].oow;
                    clip->ZTab[cp].vow = clip->uvTab[0][cp].v * clip->ZTab[cp].oow;
                }
                else
                {
                    LCLIPVALUE( clip->ZTab[ cp ].uow , polygon->ZTab[v1].uow, polygon->ZTab[v2].uow, t);
                    LCLIPVALUE( clip->ZTab[ cp ].vow , polygon->ZTab[v1].vow, polygon->ZTab[v2].vow, t);
                    LCLIPVALUE( clip->ZTab[ cp ].oow , polygon->ZTab[v1].oow, polygon->ZTab[v2].oow, t);
                }
            }
        }
        if (Mat->info.Shade==2)
        {
            if (V3X.Client->Capabilities&GXSPEC_RGBLIGHTING)
            {
                rgb32_t *c1= clip->rgb + cp;
                rgb32_t *c2= polygon->rgb + v1;
                rgb32_t *c3= polygon->rgb + v2;
                LCLIPVALUE2( c1->r, (V3XSCALAR)c2->r, (V3XSCALAR)c3->r, t);
                LCLIPVALUE2( c1->g, (V3XSCALAR)c2->g, (V3XSCALAR)c3->g, t);
                LCLIPVALUE2( c1->b, (V3XSCALAR)c2->b, (V3XSCALAR)c3->b, t);
                LCLIPVALUE2( c1->a, (V3XSCALAR)c2->a, (V3XSCALAR)c3->a, t);
            }
            else
            LCLIPVALUE( clip->shade[ cp ], polygon->shade[v1], polygon->shade[v2], t);
        }
    }
    if (!z)
    {
        V3XSCALAR z1=DIVF32(CST_ONE, polygon->dispTab[v1].z);
        V3XSCALAR z2=DIVF32(CST_ONE, polygon->dispTab[v2].z);
        V3XSCALAR zf;
        LCLIPVALUE( zf , z1, z2, t);
        clip->dispTab[ cp ].z = CST_ONE/zf;
        //LCLIPVALUE( clip->dispTab[ cp ].z , polygon->dispTab[v1].z, polygon->dispTab[v2].z, t);
    } else  clip->dispTab[ cp ].z = z;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void V3XPoly_ZClip(V3XPOLY *clip)
*
* DESCRIPTION :  Clip against front of viewport (SutherLand/Hodgman)
*
*/
V3XPOLY *V3XPoly_ZClipNear(V3XPOLY *polygon)
{
    // Create pointer to numVerts of clipped polygon
    //  structure:
    V3XPOLY *clip;
    V3XVECTOR C;
    // Clip against front of window view volume:
    int cp = 0; // Point to current vertex of clipped polygon
    // Initialize pointer to last vertex:
    unsigned v2, ne=polygon->numEdges, v1=ne-1;
    // Test Maxclipped faces
    C.z =  V3X.Clip.Near;
    if ((v1>V3X.Buffer.MaxEdges)||(V3XPoly_QExhausted()))
    {
        polygon->visible = 0;
        return polygon;
    }
    clip = V3XPoly_QAlloc();
    // Transfer information from polygon structure to clipped polygon structure
    V3X_FaceCopy(clip, polygon);
    // Loop through all edges of polygon
    for (v2=0; v2<ne; v2++)
    {
        V3XVECTOR *rv1;
        V3XVECTOR *rv2;
        V3XVECTOR *pv2;
        rv1 = V3X.Buffer.rot_vertex+polygon->faceTab[v1];
        rv2 = V3X.Buffer.rot_vertex+polygon->faceTab[v2];
        pv2 = V3X.Buffer.prj_vertex+polygon->faceTab[v2];
        // Categorize edges by type:
        if ((rv1->z < C.z) && (rv2->z < C.z))
        {
            // Entirely inside front
            v3x_facedataCopy(clip, polygon, cp, v2);
            clip->dispTab[cp  ].x = pv2->x;
            clip->dispTab[cp  ].y = pv2->y;
            clip->dispTab[cp  ].z = rv2->z;
            cp++;
        }
        else
        if ((rv1->z >= C.z) && (rv2->z >= C.z))
        {
            // Edge is entirely past front, so do nothing
        }
        else
        if ((rv1->z < C.z) && (rv2->z >= C.z))
        {
            // Edge is leaving view volume
            V3XSCALAR t = DIVF32(C.z - rv1->z, rv2->z - rv1->z);
            LCLIPVALUE( C.x, rv1->x, rv2->x, t);
            LCLIPVALUE( C.y, rv1->y, rv2->y, t);
            V3XVector_ProjectWithCenter(clip->dispTab[cp], C);
            v3x_facedataClip(t, clip, polygon, cp, v1, v2, C.z);
            cp++;
        }
        else
        {
            // Line is entering view volume
            V3XSCALAR t = DIVF32(C.z - rv2->z, rv1->z - rv2->z);
            LCLIPVALUE( C.x, rv2->x, rv1->x, t);
            LCLIPVALUE( C.y, rv2->y, rv1->y, t);
            V3XVector_ProjectWithCenter(clip->dispTab[cp], C);
            v3x_facedataClip(t, clip, polygon, cp, v2, v1, C.z);
            cp++;
            v3x_facedataCopy(clip, polygon, cp, v2);
            clip->dispTab[cp].x = pv2->x;
            clip->dispTab[cp].y = pv2->y;
            clip->dispTab[cp].z = rv2->z;
            cp++;
        }
        v1=v2; // Advance to next vertex
    }
    // Put number of numVerts in clipped polygon structure:
    clip->numEdges = (uint8_t)cp;
    if (cp<2) clip->visible = 0;
    else V3X.Buffer.MaxClipped ++;
    return clip;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XPOLY static *V3XPoly_ZClipFar(V3XPOLY *polygon)
*
* DESCRIPTION :
*
*/
V3XPOLY *V3XPoly_ZClipFar(V3XPOLY *polygon)
{
    // Create pointer to numVerts of clipped polygon
    //  structure:
    V3XPOLY *clip;
    V3XVECTOR C;
    // Clip against front of window view volume:
    unsigned cp = 0; // Point to current vertex of clipped polygon
    // Initialize pointer to last vertex:
    unsigned v2, v1=polygon->numEdges-1;
    // Test Maxclipped faces
    C.z =  V3X.Clip.Far;
    if ((v1>V3X.Buffer.MaxEdges)||(V3XPoly_QExhausted()))
    {
        polygon->visible = 0;
        return polygon;
    }
    clip = V3XPoly_QAlloc();
    // Transfer information from polygon structure to clipped polygon structure
    V3X_FaceCopy(clip, polygon);
    // Loop through all edges of polygon
    for (v2=0; v2<polygon->numEdges; v2++)
    {
        V3XVECTOR *rv1=V3X.Buffer.rot_vertex+polygon->faceTab[v1];
        V3XVECTOR *rv2=V3X.Buffer.rot_vertex+polygon->faceTab[v2];
        V3XVECTOR *pv2=V3X.Buffer.prj_vertex+polygon->faceTab[v2];
        // Categorize edges by type:
        if ((rv1->z > C.z) && (rv2->z > C.z))
        {
            // Entirely inside front
            v3x_facedataCopy(clip, polygon, cp, v2);
            clip->dispTab[cp  ].x = pv2->x;
            clip->dispTab[cp  ].y = pv2->y;
            clip->dispTab[cp  ].z = rv2->z;
            cp++;
        }
        else
        if ((rv1->z <= C.z) && (rv2->z <= C.z))
        {
            // Edge is entirely past front, so do nothing
        }
        else
        if ((rv1->z >= C.z) && (rv2->z < C.z))
        {
            // Edge is leaving view volume
            V3XSCALAR t = DIVF32(C.z - rv1->z, rv2->z - rv1->z);
            LCLIPVALUE( C.x, rv1->x, rv2->x, t);
            LCLIPVALUE( C.y, rv1->y, rv2->y, t);
            V3XVector_ProjectWithCenter(clip->dispTab[cp], C);
            v3x_facedataClip(t, clip, polygon, cp, v1, v2, C.z);
            cp++;
        }
        else
        {
            // Line is entering view volume
            V3XSCALAR t = DIVF32(C.z - rv2->z, rv1->z - rv2->z);
            LCLIPVALUE( C.x, rv2->x, rv1->x, t);
            LCLIPVALUE( C.y, rv2->y, rv1->y, t);
            V3XVector_ProjectWithCenter(clip->dispTab[cp], C);
            v3x_facedataClip(t, clip, polygon, cp, v2, v1, C.z);
            cp++;
            v3x_facedataCopy(clip, polygon, cp, v2);
            clip->dispTab[cp].x = pv2->x;
            clip->dispTab[cp].y = pv2->y;
            clip->dispTab[cp].z = rv2->z;
            cp++;
        }
        v1=v2; // Advance to next vertex
    }
    // Put number of numVerts in clipped polygon structure:
    clip->numEdges = (uint8_t)cp;
    if (cp<2) clip->visible = 0;
    else V3X.Buffer.MaxClipped ++;
    return clip;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void V3XPoly_XYClipping(V3XPOLY *clip)
*
* DESCRIPTION :  Clip against sides of viewport (SutherLand/Hodgman)
*
*/
V3XPOLY *V3XPoly_XYClipping(V3XPOLY *clip)
{
    V3XPOLY *nouv;
    V3XPOLY *final = V3XPoly_QTemp();
    V3XPTS  *Cdisp = clip->dispTab;
    V3XPTS  *Fdisp, *p1, *p2;
    V3XSCALAR Theta;
    int cp, v2, v1;
    V3XSCALAR xmax=(V3XSCALAR)(GX.View.xmax+1), ymax=(V3XSCALAR)(GX.View.ymax+1);
    V3XSCALAR xmin=(V3XSCALAR)(GX.View.xmin), ymin=(V3XSCALAR)(GX.View.ymin);
    // Minimal Test
    for (cp=0, v2=0;v2<clip->numEdges; v2++)
    {
        p2 = Cdisp+v2;
        if
        ((p2->x<xmin)
        ||(p2->x>xmax)
        ||(p2->y<ymin)
        ||(p2->y>ymax))
        cp++;
    }
    if (!cp) return clip;
    nouv = V3XPoly_QAlloc();
    clip->visible = 0;
    if ((clip->numEdges>=V3X.Buffer.MaxEdges)|| (V3XPoly_QExhausted()))
    {
        return clip;
    }
    V3X_FaceCopy(nouv, clip);
    clip = nouv;
    Cdisp = clip->dispTab;
    Fdisp = final->dispTab;
    final->Mat = clip->Mat;
    clip->visible = 0;
    {
        /*****************************************************
        *                                                    *
        *                    X M I N                         *
        *           Clip against left edge of viewport:      *
        *                                                    *
        ******************************************************/
        // Initialize pointer to last vertex:
        v1 = clip->numEdges-1;
        cp = 0;
        for (v2=0; v2<clip->numEdges; v2++)
        {
            p1 = Cdisp+v1;
            p2 = Cdisp+v2;
            // Categorize edges by type:
            if ((p1->x >= xmin)
            &&  (p2->x >= xmin))
            {
                // Edge isn't off left side of viewport
                v3x_facedataCopy(final, clip, cp, v2);
                cp++;
            }
            else
            if ((p1->x < xmin)
            &&  (p2->x < xmin))
            {
                // Edge is entirely off left side of viewport,
                //  so don't do anything
            }
            else
            if ((p1->x >= xmin)
            &&  (p2->x <  xmin))
            {
                // Edge is leaving viewport
                V3XSCALAR  lx=  p2->x - p1->x;
                V3XSCALAR  l =  xmin - p1->x;
                V3XSCALAR  m = DIVF32(p2->y-p1->y, lx);
                Fdisp[cp].x = xmin;
                Fdisp[cp].y = p1->y + MULF32(m, l);
                Theta  = DIVF32(l, lx);
                v3x_facedataClip(Theta, final, clip, cp, v1, v2, CST_ZERO);
                cp++;
            }
            else
            {
                // Edge is entering viewport
                V3XSCALAR  lx = p1->x-p2->x;
                V3XSCALAR  l = xmin - p2->x;
                V3XSCALAR  m = DIVF32(p1->y-p2->y, lx);
                Fdisp[cp].x = xmin;
                Fdisp[cp].y = p2->y + MULF32(m, l);
                Theta = DIVF32(l, lx);
                v3x_facedataClip(Theta, final, clip, cp, v2, v1, CST_ZERO);
                cp++;
                v3x_facedataCopy(final, clip, cp, v2);
                cp++;
            }
            v1=v2;
        }
        clip->numEdges = (uint8_t)cp;
        if (cp<2) return clip;
        //*****************************************************
        //*                                                    *
        //*                   X M A X                          *
        //*      Clip against right edge of viewport:   *
        //*                                                    *
        //******************************************************
        //
        cp = 0;
        // Initialize pointer to last vertex:
        v1=clip->numEdges-1;
        for (v2=0; v2<clip->numEdges; v2++)
        {
            p1 = Fdisp+v1;
            p2 = Fdisp+v2;
            // Categorize edges by type:
            if ((p1->x <= xmax)
            &&  (p2->x <= xmax)) {
                // Edge isn't off right side of viewport
                v3x_facedataCopy(clip, final, cp, v2);
                cp++;
            }
            else
            if ((p1->x >  xmax)
            &&  (p2->x >  xmax)){
                // Edge is entirely off right side of viewport,
                //  so do nothing
            }
            else
            if ((p1->x <= xmax)
            &&  (p2->x >  xmax))
            {
                // Edge if leaving viewport
                V3XSCALAR lx = p2->x - p1->x;
                V3XSCALAR l = xmax - p1->x;
                V3XSCALAR m = DIVF32(p2->y-p1->y, lx);
                Cdisp[cp].x = xmax ;
                Cdisp[cp].y = p1->y + MULF32(m, l);
                Theta  = DIVF32(l, lx);
                v3x_facedataClip(Theta, clip, final, cp, v1, v2, CST_ZERO);
                cp++;
            }
            else
            {
                // Edge is entering viewport
                V3XSCALAR lx = p1->x - p2->x;
                V3XSCALAR l = xmax - p2->x;
                V3XSCALAR m = DIVF32(p1->y-p2->y, lx);
                Cdisp[cp].x = xmax ;
                Cdisp[cp].y = p2->y + MULF32(m, l);
                Theta = DIVF32(l, lx);
                v3x_facedataClip(Theta, clip, final, cp, v2, v1, CST_ZERO);
                cp++;
                v3x_facedataCopy(clip, final, cp, v2);
                cp++;
            }
            v1=v2;
        }
        clip->numEdges = (uint8_t)cp;
        if (cp<2) return clip;
    }
    {
        //****************************************************
        //*                                                   *
        //*                   Y M I N                         *
        //*        Clip against upper edge of viewport:*
        //*                                                   *
        //*****************************************************
        cp = 0;
        // Initialize pointer to last vertex:
        v1=clip->numEdges-1;
        for (v2=0; v2<clip->numEdges; v2++)
        {
            p1 = Cdisp+v1;
            p2 = Cdisp+v2;
            // Categorize edges by type:
            if ((p1->y >= ymin)
            &&  (p2->y >= ymin))
            {
                // Edge is not off top off viewport
                v3x_facedataCopy(final, clip, cp, v2);
                cp++;
            }
            else
            if ((p1->y < ymin)
            &&  (p2->y < ymin))
            {
                // Edge is entirely off top of viewport,
                //  so don't do anything
            }
            else
            if ((p1->y >= ymin)
            &&  (p2->y <  ymin))
            {
                V3XSCALAR ly= p2->y-p1->y;
                V3XSCALAR l = ymin  -p1->y;
                // Edge is leaving viewport
                V3XSCALAR m = DIVF32(p2->x-p1->x, ly);
                Fdisp[cp].x = p1->x + MULF32(l, m );
                Fdisp[cp].y = ymin;
                Theta = DIVF32(l, ly);
                v3x_facedataClip(Theta, final, clip, cp, v1, v2, CST_ZERO);
                cp++;
            }
            else
            {
                V3XSCALAR ly= p1->y-p2->y;
                V3XSCALAR l = ymin-p2->y;
                // Edge is entering viewport
                V3XSCALAR m = DIVF32(p1->x-p2->x, ly);
                Fdisp[cp].x = p2->x + MULF32(l, m);
                Fdisp[cp].y = ymin;
                Theta = DIVF32(l, ly);
                v3x_facedataClip(Theta, final, clip, cp, v2, v1, CST_ZERO);
                cp++;
                // Edge is not off top off viewport
                v3x_facedataCopy(final, clip, cp, v2);
                cp++;
            }
            v1=v2;
        }
        clip->numEdges = (uint8_t)cp;
        if (cp<2) return clip;
        //****************************************************
        //*                                                   *
        //*                   Y M A X                         *
        //*         Clip against lower edge of viewport:      *
        //*                                                   *
        //*****************************************************
        cp = 0;
        // Initialize pointer to last vertex:
        v1=clip->numEdges-1;
        for (v2=0; v2<clip->numEdges; v2++)
        {
            p1 = Fdisp+v1;
            p2 = Fdisp+v2;
            // Categorize edges by type:
            if ((p1->y <= ymax)
            &&  (p2->y <= ymax))
            {
                // Edge is not off bottom of viewport
                v3x_facedataCopy(clip, final, cp, v2);
                cp++;
            }
            else
            if ((p1->y > ymax)
            &&  (p2->y > ymax))
            {
                // Edge is entirely off bottom of viewport,
                //  so don't do anything
            }
            else
            if ((p1->y <= ymax)
            &&  (p2->y >  ymax))
            {
                // Edge is leaving viewport
                V3XSCALAR ly = p2->y-p1->y;
                V3XSCALAR l = ymax - p1->y;
                V3XSCALAR m = DIVF32(p2->x-p1->x, ly);
                Cdisp[cp].x = p1->x + MULF32(l, m);
                Cdisp[cp].y = ymax;
                Theta = DIVF32(l, ly);
                v3x_facedataClip(Theta, clip, final, cp, v1, v2, CST_ZERO);
                cp++;
            }
            else
            {
                // Edge is entering viewport
                V3XSCALAR ly =  p1->y-p2->y;
                V3XSCALAR l =  ymax - p2->y;
                V3XSCALAR m = DIVF32(p1->x - p2->x, ly);
                Cdisp[cp].x = p2->x + MULF32(l, m);
                Cdisp[cp].y = ymax ;
                Theta = DIVF32(l, ly);
                v3x_facedataClip(Theta, clip, final, cp, v2, v1, CST_ZERO);
                cp++;
                // Edge is not off top off viewport
                v3x_facedataCopy(clip, final, cp, v2);
                cp++;
            }
            v1=v2;
        }
        clip->numEdges = (uint8_t)cp;
        if (cp<2) return clip;
    }
    V3X.Buffer.MaxClipped++;
    clip->visible=1;
    return clip;
}

