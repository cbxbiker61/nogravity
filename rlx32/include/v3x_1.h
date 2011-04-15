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
#ifndef __V3X_1H
#define __V3X_1H

__extern_c
    // Render mesh
_RLXEXPORTFUNC    void     RLXAPI  V3XMESH_SetFixedDistance(V3XMESH *mesh, V3XSCALAR z);
_RLXEXPORTFUNC    int32_t  RLXAPI  V3XMesh_Transform(V3XMESH *Ob);
_RLXEXPORTFUNC    void     RLXAPI  V3XMesh_RenderPoints(V3XMATRIX *Mat, V3XVECTOR *vertex_list, int32_t maxvertex, int color);
_RLXEXPORTFUNC    int32_t     RLXAPI  V3XLight_Transform(V3XLIGHT *light, unsigned option);

    // Matrix operators
_RLXEXPORTFUNC    void     RLXAPI V3XMatrix_MeshTransform(V3XCAMERA *Cam);
_RLXEXPORTFUNC    void     RLXAPI V3XMatrix_BuildFromVector(V3XMATRIX *M0, V3XVECTOR *vect, int roll);
_RLXEXPORTFUNC    void     RLXAPI V3XMatrix_BuildFromVectorUP(V3XMATRIX *M0, V3XVECTOR *v, V3XVECTOR *Source);
_RLXEXPORTFUNC    void     RLXAPI V3XMatrix_BuildFromNVector(V3XMATRIX *M0, V3XVECTOR *v, int roll);

    // Vector operator
_RLXEXPORTFUNC    int      RLXAPI V3XVector_Project(V3XVECTOR *result, V3XVECTOR *input);
_RLXEXPORTFUNC    V3XSCALAR RLXAPI V3XVector_DistanceToPlane(V3XVECTOR *Point, V3XVECTOR *PlanePoint, V3XVECTOR *PlaneNormal);
_RLXEXPORTFUNC    V3XSCALAR RLXAPI V3XVector_ProjectPointLine(V3XVECTOR *res, V3XVECTOR *u, V3XVECTOR *e0, V3XVECTOR *e1);
_RLXEXPORTFUNC    int      RLXAPI V3XVector_IntersectPlaneSegmentEx(V3XVECTOR * isect, V3XVECTOR * start, V3XVECTOR * end, V3XVECTOR *normal, V3XSCALAR d, V3XSCALAR* pr);
_RLXEXPORTFUNC    int      RLXAPI V3XVector_IntersectPlaneSegment(V3XVECTOR * isect, V3XVECTOR * start, V3XVECTOR * end, V3XVECTOR *normal, V3XVECTOR *point);
_RLXEXPORTFUNC    V3XSCALAR RLXAPI V3XVector_IntersectPointPlane(V3XVECTOR *isect, V3XVECTOR *Point, V3XVECTOR *PlanePoint, V3XVECTOR *PlaneNormal);
_RLXEXPORTFUNC    int      RLXAPI V3XVector_IntersectSphereSegment(V3XSCALAR radius, V3XVECTOR *center, V3XVECTOR *start, V3XVECTOR *end);
_RLXEXPORTFUNC    int      RLXAPI V3XVector_InPoly(V3XVECTOR * isect, int numVerts, V3XVECTOR *vertex, V3XVECTOR *normal);
   // Bounding boxes
_RLXEXPORTFUNC    void     RLXAPI V3XBBox_Compute(V3XVECTOR *mini, V3XVECTOR *maxi, int numVerts, V3XVECTOR *vertex);
_RLXEXPORTFUNC    int      RLXAPI V3XBBox_Inside(V3XVECTOR *vertex, V3XVECTOR *mini, V3XVECTOR *maxi);
_RLXEXPORTFUNC    void     RLXAPI V3XBBox_Draw(V3XVECTOR *mini, V3XVECTOR *maxi, uint32_t cl);

   // Transformation
_RLXEXPORTFUNC    void     RLXAPI V3XVector_WorldPos(V3XMATRIX *Mat, V3XVECTOR *input, V3XVECTOR *result);
_RLXEXPORTFUNC    int      RLXAPI V3XVector_TransformProject_pts(V3XVECTOR *input, V3XVECTOR *result);
_RLXEXPORTFUNC    int      RLXAPI V3XVector_3Dto2D_pts(V3XMATRIX *Mat, V3XVECTOR *input, V3XVECTOR *result);
    // Primitive graphique
_RLXEXPORTFUNC    void     RLXAPI V3X_DrawSphere(V3XMATRIX *mat, V3XVECTOR *centre, V3XSCALAR r, uint32_t cl);
_RLXEXPORTFUNC    void     RLXAPI V3X_DrawBox(V3XVECTOR *Min1, V3XVECTOR *Max1, uint32_t cl);

__end_extern_c

#endif

