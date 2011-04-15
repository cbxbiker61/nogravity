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
#include "gx_struc.h"
#include "gx_rgb.h"
#include "v3xdefs.h"
#include "v3x_1.h"
#include "v3x_2.h"
#include "v3xrend.h"
#include "v3xtrig.h"
#include "v3xcoll.h"

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XCL *V3XCL_New(int numItem )
*
* DESCRIPTION :
*
*/
V3XCL *V3XCL_New(int numItem )
{
    V3XCL *Cs = (V3XCL*)MM_heap.malloc(sizeof(V3XCL));
    Cs->numItem = numItem ;
    Cs->item = V3X_CALLOC(Cs->numItem+1, V3XCL_ITEM);
    return Cs;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XCL_Release(V3XCL *Cs)
*
* DESCRIPTION :
*
*/
void V3XCL_Release(V3XCL *Cs)
{
    MM_heap.free(Cs->item);
    MM_heap.free(Cs);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XCL_findGlobalSphere(V3XMESH *obj, V3XCL_SPHERE *Cs)
*
* DESCRIPTION :
*
*/
void V3XCL_findGlobalSphere(V3XMESH *obj, V3XCL_SPHERE *Cs)
{
    int32_t j;
    V3XVECTOR *caller=obj->vertex;
    V3XVECTOR xmin, xmax, ymin, ymax, zmin, zmax, d, span, dia1, dia2, cen;
    V3XSCALAR  rad, rad_sq, maxspan, old_to_p_sq, old_to_p, old_to_new;
    // Pass 1
    xmin=ymin=zmin=xmax=ymax=zmax=*caller;
    for (j=obj->numVerts;j!=0;caller++, j--)
    {
        if (caller->x<xmin.x) xmin = *caller;
        if (caller->x>xmax.x) xmax = *caller;
        if (caller->y<ymin.y) ymin = *caller;
        if (caller->y>ymax.y) ymax = *caller;
        if (caller->z<zmin.z) zmin = *caller;
        if (caller->z>zmax.z) zmax = *caller;
    }
    V3XVector_Dif(&d, &xmax, &xmin); span.x = V3XVector_LengthSq(&d);
    V3XVector_Dif(&d, &ymax, &ymin); span.y = V3XVector_LengthSq(&d);
    V3XVector_Dif(&d, &zmax, &zmin); span.z = V3XVector_LengthSq(&d);
    maxspan = span.x;
    dia1 = xmin;
    dia2 = xmax;
    if (span.y>maxspan)
    {
        maxspan = span.y;
        dia1 = ymin;
        dia2 = ymax;
    }
    if (span.z>maxspan)
    {
        dia1 = zmin;
        dia2 = zmax;
    }
    V3XVector_Avg(&cen, &dia1, &dia2);
    V3XVector_Dif(&d, &dia2, &cen);
    rad_sq = V3XVector_LengthSq(&d);
    rad = sqrtf(rad_sq);
    /* PASS 2 */
    for(caller=obj->vertex, j=obj->numVerts;j!=0;caller++, j--)
    {
        V3XVector_Dif(&d, caller, &cen);
        old_to_p_sq = V3XVector_LengthSq(&d);
        if (old_to_p_sq>rad_sq)
        {
            old_to_p = sqrtf(old_to_p_sq);
            rad = xSHRD(rad+old_to_p, 1);
            old_to_new = old_to_p - rad;
            cen.x = (rad*cen.x+old_to_new*caller->x)/old_to_p;
            cen.y = (rad*cen.y+old_to_new*caller->y)/old_to_p;
            cen.z = (rad*cen.z+old_to_new*caller->z)/old_to_p;
        }
    }
    Cs->_center = cen;
    Cs->radius = rad*obj->scale;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XCL_MESH *V3XCL_GenerateFromMesh(V3XCL_MESH *mc, V3XMESH *mesh)
*
* DESCRIPTION :
*
*/
#define TESSLATE_CMESH

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XCL_MESH *V3XCL_GenerateFromMesh(V3XCL_MESH *mc, V3XMESH *mesh)
*
* Description :
*
*/
V3XCL_MESH *V3XCL_GenerateFromMesh(V3XCL_MESH *mc, V3XMESH *mesh)
{
    int i;
    V3XPOLY    *fm = mesh->face;
    V3XVECTOR  *nm = mesh->normal_face;
    V3XCL_FACE *fc;
#ifdef TESSLATE_CMESH
    for (mc->numFaces = 0, i=0;i<mesh->numFaces;fm++, i++)
    {
        mc->numFaces+=fm->numEdges-2;
    }
    mc->face = (V3XCL_FACE*)MM_heap.malloc(mc->numFaces * sizeof(V3XCL_FACE));
    for (fc=mc->face, fm=mesh->face, i=0;i<mesh->numFaces;i++, fm++, nm++)
    {
        int nbFan = fm->numEdges - 2;
        int j, k;
        for (j=0;j<nbFan;j++, fc++)
        {
            fc->numEdges = 3;
            fc->edges = (V3XVECTOR*)MM_heap.malloc((uint32_t)fc->numEdges*sizeof(V3XVECTOR));
            fc->sectorID = 0;
            fc->offset = 0;
            fc->normal = *nm;
            for (k=0;k<3;k++)
            {
				unsigned edge = (k+j*2)%fm->numEdges;
				unsigned index = fm->faceTab[edge];
                fc->edges[k] = mesh->vertex[index];
            }
        }
    }
#else
    if (!mc->numFaces) mc->numFaces = mesh->numFaces;
    if (!mc->face)     mc->face = (V3XCL_FACE*)MM_heap.malloc(mc->numFaces * sizeof(V3XCL_FACE));
    for (fc=mc->face, fm=mesh->face, i=0;i<mesh->numFaces;i++, fm++, fc++, nm++)
    {
        int k;
        fc->numEdges = fm->numEdges;
        fc->edges = (V3XVECTOR*)MM_heap.malloc((uint32_t)fc->numEdges*sizeof(V3XVECTOR));
        fc->sectorID = 0;
        fc->offset = 0;
        fc->normal = *nm;
        for (k=0;k<fc->numEdges;k++)
        {
            fc->edges[k] = mesh->vertex[fm->faceTab[k]];
        }
    }
#endif
    return mc;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XCL *V3XCL_NewFromMesh(V3XMESH *obj)
*
* DESCRIPTION : Create a V3XCL for dummies objects (cameras, lights, dummies).
*
*/
V3XCL *V3XCL_NewFromDummy(V3XSCALAR radius)
{
    V3XCL *Cs = V3XCL_New(1);
    Cs->global.radius = radius;
    V3XVector_Set(&Cs->global._center, CST_ZERO, CST_ZERO, CST_ZERO);
    Cs->item[0].sh = Cs->global;
    Cs->item[0].type = V3XCTYPE_SPHERE;
    return Cs;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XCL *V3XCL_NewFromMesh(V3XMESH *obj, int mode)
*
* DESCRIPTION :  Create an V3XCL from a mesh.
*
*/
V3XCL *V3XCL_NewFromMesh(V3XMESH *obj, int mode)
{
    V3XCL *Cs = V3XCL_New(1);
    V3XCL_findGlobalSphere(obj, &Cs->global);
    switch (mode) {
        case V3XCTYPE_SPHERE:
        Cs->item[0].sh = Cs->global;
        Cs->item[0].type = V3XCTYPE_SPHERE;
        break;
        default:
        break;
    }
    return Cs;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XCL_XForm(V3XCL *Cs)
*
* DESCRIPTION : XForming object.
*
*/
void V3XCL_Xform(V3XCL *Cs)
{
    int i;
    V3XCL_ITEM *a;
    if (Cs->mesh_ref)
    {
        V3XVector_ApplyMatrixTrans(Cs->global.center,
        Cs->global._center,
        Cs->mesh_ref->matrix.Matrix);
    }
    for (i=Cs->numItem, a=Cs->item;i!=0;i--, a++)
    {
        if (Cs->mesh_ref)
        {
            switch(a->type) {
                case V3XCTYPE_SPHERE:
                a->box.max = a->sh.center;
                V3XVector_ApplyMatrixTrans(a->sh.center, a->sh._center, Cs->mesh_ref->matrix.Matrix);
                break;
                case V3XCTYPE_AXISBOX:
                V3XVector_ApplyMatrixTrans(a->box.center, a->box._center, Cs->mesh_ref->matrix.Matrix);
                break;
            }
        }
        else
        {
            switch(a->type) {
                case V3XCTYPE_SPHERE:
                a->sh.center = a->sh._center;
                break;
                case V3XCTYPE_AXISBOX:
                a->box.center = a->box._center; //?
                break;
            }
        }
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XCL_XformNoRef(V3XCL *Cs, V3XVECTOR *pos)
*
* Description :
*
*/
void V3XCL_XformNoRef(V3XCL *Cs, V3XVECTOR *pos)
{
    int i;
    V3XCL_ITEM *a;
    Cs->global.center = *pos;
    for (i=Cs->numItem, a=Cs->item;i!=0;i--, a++)
    {
        switch(a->type) {
            case V3XCTYPE_SPHERE:
            a->box.max = a->sh.center;
            a->sh.center = *pos;
            break;
            case V3XCTYPE_AXISBOX:
            a->box.center = *pos;
            break;
        }
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int V3XCL_Test_SphSph(V3XCL *a, V3XCL *b, V3XCL_SPHERE *aa, V3XCL_SPHERE*bb)
*
* DESCRIPTION :
*
*/
static int V3XCL_Test_SphSph(V3XCL *a, V3XCL *b, V3XCL_SPHERE *aa, V3XCL_SPHERE *bb, int update)
{
    int m = 0, kk, inv=((a->flags&V3XCMODE_INVERTCONDITION) || (b->flags&V3XCMODE_INVERTCONDITION)) ? 1 : 0;
    V3XCL *c=NULL;
    V3XSCALAR d, r;
    V3XVECTOR k1;
    V3XVECTOR *Pa, *Pb;
    Pa = &aa->center;
    Pb = &bb->center;
    V3XVector_Dif(&k1, Pb, Pa);
    d = V3XVector_Normalize(&k1, &k1);
    r = aa->radius + bb->radius;
	if (r<CST_EPSILON)
		r=CST_EPSILON;
    if ( (r>=d) ^ inv)
    {
        if (!update) return 1;
        m = 1;
        if (((a->weight<b->weight)))
        {
            c = a;
            kk= -1;
        }
        else
        {
            c = b;
            kk = 1;
        }
        if ((a->flags&V3XCMODE_REFLEX )&&(b->flags&V3XCMODE_REFLEX ))
        {
            V3XSCALAR f = r - d;
            if (((inv)&&(kk==-1))||(kk<0)) f = -f;
            V3XVector_Madd(&c->velocity, &k1, &c->velocity, f);
        }
    }
    return m;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void v3x_BumpBoxCompute(V3XCL *a, V3XVECTOR *mini, V3XVECTOR *maxi, V3XVECTOR *Min2, V3XVECTOR *Max2, int inv)
*
* DESCRIPTION :
*
*/
static void v3x_BumpBoxCompute(V3XCL *a, V3XVECTOR *mini, V3XVECTOR *maxi, V3XVECTOR *Min2, V3XVECTOR *Max2, int inv)
{
    V3XVECTOR cas1, cas2, p;
    V3XVector_Dif(&cas1, (maxi), (Min2));
    V3XVector_Dif(&cas2, (mini), (Max2));
    p.x = (fabs(cas1.x)<fabs(cas2.x)) ? xSHRD(cas2.x, 3) : xSHRD(cas1.x, 3);
    p.y = (fabs(cas1.y)<fabs(cas2.y)) ? xSHRD(cas2.y, 3) : xSHRD(cas1.y, 3);
    p.z = (fabs(cas1.z)<fabs(cas2.z)) ? xSHRD(cas2.z, 3) : xSHRD(cas1.z, 3);
    if (inv)
    {
        V3XVector_Dec(&a->velocity, &p);
    }
    else
    {
        V3XVector_Inc(&a->velocity, &p);
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int V3XCL_Test_BoxBox(V3XCL *a, V3XCL *b, V3XCOL_Box *m1, V3XCOL_Box *m2)
*
* DESCRIPTION :
*
*/
static int V3XCL_Test_BoxBox(V3XCL *a, V3XCL *b, V3XCL_BOX *m1, V3XCL_BOX *m2)
{
    int c=0, m=0, inv=((a->flags&V3XCMODE_INVERTCONDITION) || (b->flags&V3XCMODE_INVERTCONDITION));
    V3XVECTOR mini, Min2, maxi, Max2;
    V3XVector_Sum(&mini, &m1->min, &m1->center);
    V3XVector_Sum(&maxi, &m1->max, &m1->center);
    V3XVector_Sum(&Min2, &m2->min, &m2->center);
    V3XVector_Sum(&Max2, &m2->max, &m2->center);
    if ((maxi.x < Min2.x) || (Max2.x< mini.x)) c = 0;
    else
    {
        if ((maxi.y < Min2.y) || (Max2.y < mini.y)) c = 1;
        else
        {
            if ((maxi.z < Min2.z) || (Max2.z < mini.z)) c = 2;
            else  c = 3;
        }
    }
    if (c==3)
    {
        m = 1;
        if (a->weight<b->weight)
        {
            if (a->flags&V3XCMODE_REFLEX) v3x_BumpBoxCompute(a, &mini, &maxi, &Min2, &Max2, inv);
        }
        else
        {
            if (b->flags&V3XCMODE_INVERTCONDITION) v3x_BumpBoxCompute(b, &Min2, &Max2, &mini, &maxi, inv);
        }
    }
    return m^inv;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int V3XCL_Test_BoxSph(V3XCL *a, V3XCL *b, V3XCL_BOX *m1, V3XCL_SPHERE *m2)
*
* DESCRIPTION :
*
*/
static int V3XCL_Test_BoxSph(V3XCL *a, V3XCL *b, V3XCL_BOX *m1, V3XCL_SPHERE *m2)
{
    int cd, m=0, inv=((a->flags&V3XCMODE_INVERTCONDITION) || (b->flags&V3XCMODE_INVERTCONDITION));
    V3XVECTOR r;
    V3XVECTOR mini, maxi, Max2, Min2;
    V3XVector_Sum(&mini, &m1->min, &m1->center);
    V3XVector_Sum(&maxi, &m1->max, &m1->center);
    r.x = r.y = r.z = m2->radius;
    V3XVector_Sum(&Max2, &m2->center, &r);
    V3XVector_Dif(&Min2, &m2->center, &r);
    if ((maxi.x < Min2.x) || (Max2.x< mini.x)) cd = 0;
    else
    {
        if ((maxi.y < Min2.y) || (Max2.y < mini.y)) cd = 1;
        else
        {
            if ((maxi.z < Min2.z) || (Max2.z < mini.z)) cd = 2;
            else  cd = 3;
        }
    }
    if (cd==3)
    {
        m=1;
        if (a->weight<b->weight)
        {
            v3x_BumpBoxCompute(a, &mini, &maxi, &Min2, &Max2, inv);
        }
        else
        {
            v3x_BumpBoxCompute(b, &Min2, &Max2, &mini, &maxi, inv);
        }
    }
    return m^inv;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3XCL_Test(V3XCL *a, V3XCL *b)
*
* DESCRIPTION :
*
*/
int V3XCL_Test(V3XCL *a, V3XCL *b)
{
    int i, mm=0;
    V3XCL_ITEM *aa, *bb;
    for (i=a->numItem, aa=a->item;i!=0;i--, aa++)
    {
        int j;
        int m;
        for (j=b->numItem, bb=b->item;j!=0;j--, bb++)
        {
            m = 0;
            switch(aa->type) {
                case V3XCTYPE_SPHERE:
                {
                    switch(bb->type) {
                        case V3XCTYPE_SPHERE:
                        m = V3XCL_Test_SphSph(a, b, &aa->sh, &bb->sh, 1);
                        break;
                        case V3XCTYPE_AXISBOX:
                        m = V3XCL_Test_BoxSph(b, a, &bb->box, &aa->sh);
                        break;
                    }
                }
                break;
                case V3XCTYPE_AXISBOX:
                {
                    switch(bb->type) {
                        case V3XCTYPE_AXISBOX:
                        m = V3XCL_Test_BoxBox(a, b, &aa->box, &bb->box);
                        break;
                        case V3XCTYPE_SPHERE:
                        m = V3XCL_Test_BoxSph(a, b, &aa->box, &bb->sh);
                        break;
                    }
                }
                break;
            }
            if (m)
            {
                a->last_hit = bb;
                b->last_hit = aa;
                mm = 1;//return 1;
            }
        }
    }
    return mm;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3X_DrawSphere(V3XMATRIX *mat, V3XVECTOR *center, V3XSCALAR r, uint32_t cl)
*
* DESCRIPTION :
*
*/
void V3X_DrawSphere(V3XMATRIX *mat, V3XVECTOR *center, V3XSCALAR r, uint32_t cl)
{
	char __temp[2048];
    int32_t det = 8, det1=4096L/det;
    int32_t al, bl;
    V3XVECTOR *b=(V3XVECTOR*)__temp, *a=b;
    for (bl=0;bl<4096*2;bl+=det1/2)
    {
        for (al=0;al<2048;al+=det1/2, a++)
        {
            V3XVector_PolarSet(a, center, r, al, bl);
        }
    }
    V3XMesh_RenderPoints(mat, b, det*det*2, cl);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XBBox_Draw(V3XVECTOR *mini, V3XVECTOR *maxi, uint32_t cl)
*
* DESCRIPTION :
*
*/
void V3XBBox_Draw(V3XVECTOR *mini, V3XVECTOR *maxi, uint32_t cl)
{
	char __temp[2048];
    V3XVECTOR *b=(V3XVECTOR*)__temp;
    V3XVector_Set(&b[0], mini->x, mini->y, mini->z);
    V3XVector_Set(&b[1], mini->x, maxi->y, mini->z);
    V3XVector_Set(&b[2], maxi->x, maxi->y, mini->z);
    V3XVector_Set(&b[3], maxi->x, mini->y, mini->z);
    V3XVector_Set(&b[4], mini->x, mini->y, maxi->z);
    V3XVector_Set(&b[5], mini->x, maxi->y, maxi->z);
    V3XVector_Set(&b[6], maxi->x, maxi->y, maxi->z);
    V3XVector_Set(&b[7], maxi->x, mini->y, maxi->z);
    GX_ClippedLine3D((b+ 8-8), (b+ 9-8), cl);
    GX_ClippedLine3D((b+ 9-8), (b+10-8), cl);
    GX_ClippedLine3D((b+10-8), (b+11-8), cl);
    GX_ClippedLine3D((b+11-8), (b+ 8-8), cl);
    GX_ClippedLine3D((b+12-8), (b+13-8), cl);
    GX_ClippedLine3D((b+13-8), (b+14-8), cl);
    GX_ClippedLine3D((b+14-8), (b+15-8), cl);
    GX_ClippedLine3D((b+15-8), (b+12-8), cl);
    GX_ClippedLine3D((b+ 8-8), (b+12-8), cl);
    GX_ClippedLine3D((b+ 9-8), (b+13-8), cl);
    GX_ClippedLine3D((b+10-8), (b+14-8), cl);
    GX_ClippedLine3D((b+11-8), (b+15-8), cl);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XCL_Draw(V3XCL *cs)
*
* DESCRIPTION :  Dessine les objets de collisions.
*
*/
void V3XCL_Draw(V3XCL *cs)
{
    V3XCL_ITEM *aa=cs->item;
    uint32_t i;
    for (i=0;i<cs->numItem ;i++, aa++)
    {
        uint32_t cl = RGB_PixelFormat(0xff, 0xff, 0x80);
        switch(aa->type) {
            case V3XCTYPE_SPHERE:
			{
				V3X_DrawSphere(&cs->mesh_ref->matrix, &aa->sh._center, aa->sh.radius, cl);
			}
            break;
            case V3XCTYPE_AXISBOX:
            {
                V3XVECTOR mini, maxi;
                V3XVector_Sum(&mini, &aa->box.min, &aa->box.center);
                V3XVector_Sum(&maxi, &aa->box.max, &aa->box.center);
                V3XBBox_Draw(&mini, &maxi, cl);
            }
            break;
        }
    }
}

