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
#include <stdlib.h>
#include <stdio.h>
#include "_rlx32.h"
#include "sysresmx.h"
#include "systools.h"
/*****/
#include "gx_struc.h"
#include "gx_tools.h"
#include "gx_csp.h"
#include "gx_rgb.h"
/*****/
#include "v3x_priv.h"
#include "v3xdefs.h"
#include "v3x_1.h"
#include "v3x_2.h"
#include "v3xtrig.h"
#include "v3xmaps.h"
#include "v3xrend.h"
#include "fixops.h"
__extern_c
int V3XVECTOR_IsVisible(void *Scene, V3XVECTOR *start, V3XVECTOR *end, unsigned hint, void *who) ;
__end_extern_c
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  #define V3XMatrix_Transform(Mat, MObj)\
*
* Description :  
*
*/
#define V3XMatrix_Transform(Mat, MObj)\
{\
    V3XVECTOR VV;\
    V3XMatrix_Multiply3x3(Mat.Matrix, V3X.Camera.M.Matrix, MObj.Matrix);\
    V3XVector_Dif(&VV, &MObj.v.Pos, &V3X.Camera.M.v.Pos);\
    V3XVector_ApplyMatrix(Mat.v.Pos, VV, V3X.Camera.M.Matrix);\
    V3XVector_ApplyTransposeMatrix(V3X.Camera.transposed, VV, MObj.Matrix);\
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int32_t V3XMesh_Transform(V3XMESH *mesh)
*
* DESCRIPTION :
*
*/
static void V3XMesh_T2(V3XMESH *mesh, V3XMATRIX *Matrix)
{
    if ((mesh->flags&V3XMESH_HASDYNLIGHT)&&((mesh->flags&V3XMESH_PRESHADED)==0))
    {
        unsigned  sx, n;        
        V3XSCALAR *p;
        V3XLIGHT *lite = V3X.Light.light;
        // Initializing lights values
        n = (mesh->flags&V3XMESH_FLATSHADE) ? mesh->numFaces : mesh->numVerts;
        if ((V3X.Client->Capabilities&GXSPEC_RGBLIGHTING))
        {
            rgb32_t *sh = mesh->rgb;
            for (p=V3X.Buffer.shade;n!=0;p++, sh++, n--)
            *(rgb32_t*)p= mesh->flags&V3XMESH_HASSHADETABLE ? *sh : V3X.Light.ambiant;
        }
        else
        {
            V3XSCALAR *sh = mesh->shade;
            for (p=V3X.Buffer.shade;n!=0;p++, sh++, n--)
            *p = mesh->flags&V3XMESH_HASSHADETABLE ? *sh : mesh->selfIllumine;
        }        
        // Lights processing  
        for (sx=V3X.Light.numSource;sx!=0;lite++, sx--)
        {
            if ((lite->type!=V3XLIGHTTYPE_DIRECTIONAL)||
            (lite->type==V3XLIGHTTYPE_DIRECTIONAL) && (((V3X.Setup.flags&V3XOPTION_RAYTRACE)==0)||V3XVECTOR_IsVisible(NULL, &lite->pos, &mesh->matrix.v.Pos, 2, NULL)))
            if (!((lite->flags&V3XLIGHTCAPS_DYNAMIC)
            &&(mesh->flags&V3XMESH_EXCPTDYNLIGHT)))
            {
                V3XVECTOR temp1, v_lite3pt;
                V3XSCALAR  ds=CST_ZERO;
                V3XSCALAR  multiplier = lite->intensity;
                V3XSCALAR  invRange = CST_ZERO;
                V3XVector_Dif(&temp1, &lite->pos, &mesh->matrix.v.Pos);
                if (
                (lite->flags & V3XLIGHTCAPS_RANGE)&&((lite->flags&V3XLIGHTCAPS_INFINITE)==0)
                )
                {
                    invRange = xINV(lite->range);
                }
                // Setup light
                switch(lite->type) {
                    case V3XLIGHTTYPE_DIRECTIONAL:
                    {
                        lite->vector = temp1;
                        ds = V3XVector_Normalize(&lite->vector, &lite->vector);
                        V3XVector_ApplyTransposeMatrix(v_lite3pt, lite->vector, mesh->matrix.Matrix);
                    }
                    break;
                    case V3XLIGHTTYPE_OMNI:
                    break;
                    case V3XLIGHTTYPE_SPOT:
                    {
                        V3XVECTOR V2;
                        V3XVector_Dif(&V2, &lite->target, &lite->pos);
                        V3XVector_Normalize( &V2, &V2 );
                        V3XVector_ApplyTransposeMatrix(lite->vector, V2, mesh->matrix.Matrix);
                    }
                    break;
                    case V3XLIGHTTYPE_OMNI_Z:
                    lite->flags|= V3XLIGHTCAPS_RANGE;
                    break;
                    default:
                    break;
                }
                if (((lite->flags&V3XLIGHTCAPS_INFINITE)==0)
                && (lite->type!=V3XLIGHTTYPE_DIRECTIONAL))
                ds = V3XVector_Length(&temp1);
                if  ((lite->flags&V3XLIGHTCAPS_INFINITE)
                ||  ((lite->flags&V3XLIGHTCAPS_RANGE) && (ds <= lite->range + mesh->radius))
                ||  ((lite->flags&V3XLIGHTCAPS_RANGE)==0)
                )
                {
                    unsigned char  *flag0;
                    V3XVECTOR *a, *b, *e;
                    V3XSCALAR  *p;
                    V3XPOLY   *f;
                    unsigned   n;
                    b = V3X.Buffer.rot_vertex;
                    e = mesh->vertex;
                    p = V3X.Buffer.shade;
                    // Switch
                    if (mesh->flags&V3XMESH_FLATSHADE)
                    {
                        flag0 = NULL;
                        f = mesh->face;
                        a = mesh->normal_face;
                        n = mesh->numFaces;
                    }
                    else
                    {
                        flag0 = V3X.Buffer.flag;
                        f = NULL;
                        a = mesh->normal;
                        n = mesh->numVerts;
                    }
                    for(;n!=0; a++, b++, e++, p++, n-- )
                    {
                        if ((f&&f->visible)||(flag0 &&(!*flag0)))
                        {
                            // Value de determinate
                            V3XSCALAR inten = CST_ZERO;
                            V3XSCALAR factor = CST_ONE;
                            // Range fogging
                            if ((lite->flags&V3XLIGHTCAPS_INFINITE)==0)
                            {
                                if (lite->flags & V3XLIGHTCAPS_RANGE)
                                {
                                    V3XVECTOR vertex_world_pos;
                                    V3XSCALAR  dst_vertex_to_light;
                                    V3XVector_ApplyMatrixTrans(vertex_world_pos, *e, mesh->matrix.Matrix);
                                    dst_vertex_to_light = V3XVector_Distance(&vertex_world_pos, &lite->pos);
                                    factor = (dst_vertex_to_light<lite->range) ? (CST_ONE - MULF32(dst_vertex_to_light, invRange)) : CST_ZERO;
                                }
                                else
                                {
                                    V3XSCALAR dx = b->z - lite->range;
                                    factor = (dx<CST_ZERO) ? CST_ONE+MULF32(dx, lite->diminish) : CST_ZERO;
                                }
                            }
                            if (factor>CST_ZERO)
                            {
                                V3XVECTOR v_lite2pt;
                                switch( lite->type) {
                                    case V3XLIGHTTYPE_OMNI:
                                    {
                                        V3XVector_Dif(&lite->vector, e, &temp1);
										V3XVector_Normalize(&lite->vector, &lite->vector);
                                        V3XVector_ApplyTransposeMatrix(v_lite2pt, lite->vector, mesh->matrix.Matrix);
                                        inten = -V3XVector_DotProduct(&v_lite2pt, a);
                                    }
                                    break;
                                    case V3XLIGHTTYPE_SPOT:
                                    {
                                        V3XSCALAR at;
                                        V3XVector_Dif(&lite->vector, e, &temp1);
										V3XVector_Normalize(&lite->vector, &lite->vector);
                                        V3XVector_ApplyTransposeMatrix(v_lite2pt, lite->vector, mesh->matrix.Matrix);
                                        at = V3XVector_DotProduct(&lite->vector, &v_lite2pt);
                                        if (at<CST_ZERO) inten = CST_ZERO;
                                        else
                                        {
                                            inten = -V3XVector_DotProduct(&v_lite2pt, a);
                                            if (at<lite->falloff) inten*=.5f;
                                        }
                                    }
                                    break;
                                    case V3XLIGHTTYPE_DIRECTIONAL:
                                    inten = V3XVector_DotProduct(&v_lite3pt, a);
                                    break;
                                    case V3XLIGHTTYPE_OMNI_Z:
                                    inten = CST_ONE;
                                    break;
                                }
                                if (inten > CST_ZERO )
                                {
                                    inten *= multiplier * factor;
                                    if (inten > multiplier) inten = multiplier;
                                    if ((V3X.Client->Capabilities&GXSPEC_RGBLIGHTING))
                                    {
                                        rgb32_t *rgb = (rgb32_t*)p;
                                        unsigned int i = (unsigned int)inten;
                                        unsigned
                                        wr = rgb->r + xMUL8(i, lite->color.r), 
                                        wg = rgb->g + xMUL8(i, lite->color.g), 
                                        wb = rgb->b + xMUL8(i, lite->color.b);                                        
                                        rgb->r = wr<255 ? (u_int8_t)wr : (u_int8_t)255;
                                        rgb->g = wg<255 ? (u_int8_t)wg : (u_int8_t)255;
                                        rgb->b = wb<255 ? (u_int8_t)wb : (u_int8_t)255;
                                        rgb->a = 0;
                                    }
                                    else
                                    {
                                        (*p)+=inten;
                                    }
                                }
                            }
                        }
                        if (flag0) flag0++;
                        if (f)    f++;                            
                    } // End for 
                }
            }
        }
    }
    if (mesh->flags&V3XMESH_HASDYNUV)
    {
        int32_t n=mesh->numFaces, envmap = 0;
        V3XPOLY *f=mesh->face;
        for (;n!=0;f++, n--)
        {
            if (f->visible)
            {
                int32_t i = (((V3XMATERIAL*)f->Mat)->info.Environment);
                if (i)
                {
                    int32_t m=f->numEdges;
                    u_int32_t *p1=f->faceTab;
                    for (;m!=0;m--, p1++) V3X.Buffer.flag[*p1]|= (u_int8_t)((i&7)<<2);
                    envmap=1;
                }
            }
        }
        if (envmap)
        {
            V3XVECTOR *edges = mesh->normal;
            V3XUV     *map = V3X.Buffer.uv_vertex;
            V3XVECTOR w;
            u_int8_t *flag0=V3X.Buffer.flag;
            int i;
            for (i=mesh->numVerts;i!=0;map++, edges++, flag0++, i--)
            {
                int m= (*flag0);
                if (m>1)
                {
                    switch(m) {
                        case V3XENVMAPTYPE_CAMERA<<2: // Reflection 'normal' 
                        {
                            V3XVECTOR ww;
                            V3XVector_Dif(&ww, edges, &V3X.Light.HVector);
                            V3XVector_Normalize(&ww, &ww);
                            V3XVector_ApplyMatrix(w, ww, mesh->matrix.Matrix);
                            map->u = 0.5f + w.x * 0.5f;
                            map->v = 0.5f - w.y * 0.5f;
                        }
                        break;
                        case V3XENVMAPTYPE_LIGHT<<2:
                        V3XVector_ApplyMatrix(w, (*edges), Matrix->Matrix);
                        map->u = 0.5f + w.x * 0.5f;
                        map->v = 0.5f - w.y * 0.5f;
                        break;
                        case V3XENVMAPTYPE_PLANE<<2:// Reflection 'plan'
                        V3XVector_ApplyMatrix(w, (*edges), Matrix->Matrix);
                        map->u = mesh->uv[mesh->numVerts-i].u + w.x * 0.5f;
                        map->v = mesh->uv[mesh->numVerts-i].v - w.x * 0.5f;
                        break;
                        default:
                        break;
                    }
                }
                *flag0&=3;
            }
        }
    }
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : static void V3XMesh_T3(V3XMESH *mesh)
*
* DESCRIPTION : Phase 3 : copy the lightnen, textured, projected value in the polygone.
*
*/
static void V3XMesh_T3(V3XMESH *mesh)
{
    int32_t       n;
    V3XPOLY   *f;
    V3XVECTOR *a = V3X.Buffer.rot_vertex;
    V3XVECTOR *d = V3X.Buffer.prj_vertex;
    if (!V3X.Client->primitive)
    {
        for(  n = 0; n < mesh->numMaterial; n++)
        {
            V3XMATERIAL *Mat= mesh->material + n;
            Mat->lod = (mesh->flags&V3XMESH_LOWDETAIL) ? Mat->lod_far : Mat->lod_near;			
        }
    }
    else
    {
        for(  n = 0; n < mesh->numMaterial; n++)
        {
            V3XMATERIAL *Mat= mesh->material + n;
            Mat->render_clip = (mesh->flags&V3XMESH_LOWDETAIL)
              ? Mat->render_far : Mat->render_near;             
        }
    }
    for(  n = mesh->numFaces, f =mesh->face;  n!=0;  f++, n--)
    {
        V3XPOLY *fce = (V3X.Setup.flags&V3XOPTION_DUPPOLY) ? V3XPoly_Duplicate(f) : f;
        if (fce->visible)
        {
            V3XMATERIAL *Mat = (V3XMATERIAL*)fce->Mat;
            if ((mesh->flags&V3XMESH_HASDYNLIGHT)&&(Mat->info.Shade))
            {
                if (mesh->flags&V3XMESH_FLATSHADE)
                {
                    int j;
                    if (V3X.Client->Capabilities&GXSPEC_RGBLIGHTING)
                    {
                        rgb32_t *dest = fce->rgb;
                        *dest = V3X.Buffer.rgb[mesh->numFaces-n]; dest++;
                        for(j=1;j<fce->numEdges;j++, dest++)
                        {
                            *dest = fce->rgb[0];
                        }
                    }
                    else
                    {
                        for(j=0;j<fce->numEdges;j++)
                        {
                            fce->shade[j] = V3X.Buffer.shade[fce->faceTab[j]];
                            if (fce->shade[j]>255.f) 
								fce->shade[j]=255.f;
                        }
                    }
                }            
                else
                {
                    int j;
                    if (V3X.Client->Capabilities&GXSPEC_RGBLIGHTING)
                    {
                        u_int32_t *tab = fce->faceTab;
                        rgb32_t *dest = fce->rgb + 0;
                        for(j=0;j<fce->numEdges;j++, tab++, dest++)
                        {
                            *dest = V3X.Buffer.rgb[*tab];
                        }
                    }
                    else
                    {
                        for(j=0;j<fce->numEdges;j++)
                        {
                            fce->shade[j] = V3X.Buffer.shade[fce->faceTab[j]];
                            if (fce->shade[j]>255.f) fce->shade[j]=255.f;
                        }
                    }
                }    
            }   
            if ( Mat->info.Environment )
            {
                unsigned j;
                unsigned n = (Mat->info.Environment&V3XENVMAPTYPE_DOUBLE) ? 1 : 0;
                V3XUV *uv;
                uv = fce->uvTab[n];
                if (uv)
                {
                    for (j=0;j<fce->numEdges;j++, uv++)
                    {
                        unsigned ix = fce->faceTab[j];
                        V3XUV *uvR = V3X.Buffer.uv_vertex + ix;
                        *uv = *uvR;
                    }
                }
            }   
			if (Mat->info.Perspective && fce->ZTab )
            {
                unsigned j;
                u_int32_t *ix = fce->faceTab;                
                V3XWPTS  *po = fce->ZTab;                
                V3XUV    *uv = fce->uvTab[0];
				SYS_ASSERT(po);

                for(j=0;j<fce->numEdges;j++, po++, ix++, uv++)
                {
                    unsigned index = *ix;
                    po->oow = -V3X.Buffer.prj_vertex[index].z;
                    po->uow = uv->u * po->oow;
                    po->vow = uv->v * po->oow;
                }

                if ((V3X.Client->Capabilities&GXSPEC_HARDWARE)==0)
                {
                    float zmin = fce->ZTab[0].oow, zmax = zmin;
                    for(po = fce->ZTab, j = 0;j < fce->numEdges;j++, po++, ix++, uv++)
                    {
                        if (po->oow>zmax) zmax = po->oow;
                        if (po->oow<zmin) zmin = po->oow;
                    }
                    fce->visible+=( (zmin / zmax) > .90f);
                }    
            }
            {
                V3XSCALAR  zMax, zMin;
                V3XVECTOR *b = a + fce->faceTab[0];
                V3XPTS    *pd;
                u_int32_t     *p1;
                int32_t       m;
                zMax = zMin = b->z;
                for( p1 = fce->faceTab, pd = fce->dispTab, m = fce->numEdges;  m !=0;  p1 ++, pd ++, m  --)
                {
                    int32_t vX = *p1;
                    b = a + vX;
                    pd->x = d[vX].x;
                    pd->y = d[vX].y;
                    pd->z = b->z;
                    if (b->z<zMin) zMin = b->z;
                    if (b->z>zMax) zMax = b->z;
                }
                if ( zMax < V3X.Clip.Far ) fce->visible = 0;
                else
                if ( fce->visible )
                {    
                    if (fce->NeedToClip)
                    {
                        fce = V3XPoly_ZClipNear(fce);    
                    }     
                    if ((fce->visible)&&(zMin<V3X.Clip.Far))
                    {
                        fce = V3XPoly_ZClipFar(fce);
                    }
                    if (fce->visible)
                    {
                        switch (Mat->info.Sprite) {
                            case 2:
                            break;
                            case 1:
                            case 3:
                            {
                                V3XVECTOR    p;
                                p.x =  V3X.Buffer.prj_vertex[fce->faceTab[0]].x;
                                p.y =  V3X.Buffer.prj_vertex[fce->faceTab[0]].y;
                                p.z = -V3X.Buffer.rot_vertex[fce->faceTab[0]].z;
                                Mat->shift_size = 0;
                                V3XPoly_SpriteZoom(fce, &Mat->texture[0], &p, 
                                V3X.Buffer.prj_vertex[fce->faceTab[1]].x, 
                                V3X.Buffer.prj_vertex[fce->faceTab[1]].y, 
                                0);
                            }
                            default:
                            if (!(V3X.Client->Capabilities&GXSPEC_XYCLIPPING))
                            {
                                fce = V3XPoly_XYClipping(fce);
                            }
                            break;
                        }
                        if (fce->visible)
                        {
                            if ((mesh->flags&V3XMESH_NOZSORT)==0)
                            {
                                if (mesh->flags&V3XMESH_MINZSORT)
                                fce->distance = (zMin+zMin);
                                else
                                if (mesh->flags&V3XMESH_MAXZSORT)
                                fce->distance = (zMax+zMax);
                                else
                                fce->distance = (zMax+zMin);
                            }
                            if ((V3X.Client->Capabilities&(GXSPEC_ENABLEZBUFFER|GXSPEC_ENABLEWBUFFER))&&(!Mat->info.Transparency))
                            fce->distance =- fce->distance;
                            V3XPoly_QAddPipeline(fce);
                        }
                    }
                }
            }
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int32_t V3XMesh_Transform(V3XMESH *mesh)
*
* Description :  
*
*/
int32_t V3XMesh_Transform(V3XMESH *mesh)
{
    unsigned isVisible = 0;
    V3XMATRIX Matrix, MatrixS;
    V3XMatrix_Transform(Matrix, mesh->matrix);
    if (mesh->scale==CST_ONE)
    {
        MatrixS = Matrix;
    }
    else
    {
        V3XMATRIX MatrixOs, Ms;
        V3XMatrix_SetScale(Ms.Matrix, mesh->scale);
        V3XMatrix_Multiply3x3(MatrixOs.Matrix, Ms.Matrix, mesh->matrix.Matrix);
        V3XMatrix_Multiply3x3(MatrixS.Matrix, V3X.Camera.M.Matrix, MatrixOs.Matrix);
        MatrixS.v.Pos = Matrix.v.Pos;
    }
    if (mesh->flags&V3XMESH_FULLUPDATE)
    {
        int n;
        V3XPOLY *f;
        for (n=mesh->numFaces, f=mesh->face;n!=0;f++, n--)
        {
            f->visible=1;
        }
        sysMemZero(V3X.Buffer.flag, mesh->numVerts);
    }
    else
    {
        V3XVECTOR *b, *a=mesh->vertex;
        int n;
        V3XPOLY *f;
        // Mark visible vertices & faces
        sysMemSet(V3X.Buffer.flag, 1, mesh->numVerts);
        for (b=mesh->normal_face, n=mesh->numFaces, f=mesh->face;n!=0;f++, b++, n--)
        {
            if (((V3XMATERIAL*)f->Mat)->info.TwoSide) 
				f->visible=1;
            else
            {
                V3XVECTOR V2;
                V3XVECTOR *a0 = a+f->faceTab[0];
                V3XVector_Sum(&V2, a0, &V3X.Camera.transposed);
                f->visible = ((V3XVector_DotProduct(&V2, b)) <= CST_ZERO) ;
            }
            if (f->visible)
            {
                int m;
                u_int32_t *p1;
                for (m=f->numEdges, p1=f->faceTab;m!=0;m--, p1++)
                {
                    u_int32_t index = *p1;
                    V3X.Buffer.flag[index]=0;
                }
                isVisible = 1;
            }
        }
    }
    if ((!isVisible)&&(!(mesh->flags&V3XMESH_FULLUPDATE)))
    {  
        return 3;
    } 
    {
        V3XVECTOR *a, *b;
        V3XVECTOR *d;
        u_int8_t *flag0;
        int32_t n;
        V3XSCALAR zclip = V3X.Clip.Near;
        isVisible = 0;
        for (
        flag0 = V3X.Buffer.flag, 
        a = V3X.Buffer.rot_vertex, 
        d = V3X.Buffer.prj_vertex, 
        b = mesh->vertex, 
        n = mesh->numVerts;
        n    != 0;
        a++, b++, flag0++, d++, n--)
        {
            if (*flag0==0)
            {
                V3XVector_ApplyMatrixTrans(*a, *b, MatrixS.Matrix);
                if (a->z < zclip)
                {
                    V3XSCALAR sx;
                    (*d).z = xINV((*a).z);
                    sx = V3X.ViewPort.Focal * (*d).z;
                    (*d).y = V3X.ViewPort.center.y - MULF32((*a).y, sx);
                    (*d).x = V3X.ViewPort.center.x + MULF32((*a).x, sx * V3X.ViewPort.Ratio);
                    isVisible = 1;
                }
                else
                {
                    *flag0 = 2;
                }
            }
        }
    }
    if ((!isVisible)&&(!(mesh->flags&V3XMESH_FULLUPDATE)))
    {   
        return 1;
    }
    else
    {
        V3XVECTOR *a=V3X.Buffer.rot_vertex;
        V3XVECTOR *d=V3X.Buffer.prj_vertex;
        V3XPOLY *f=mesh->face;
        int32_t n=mesh->numFaces;
        isVisible = 0;
        for(;n!=0;f++, n--)
        {
            if (f->visible)
            {
                V3XMATERIAL *Mat=(V3XMATERIAL*)f->Mat;
                if (Mat->info.Sprite)
                {
                    int
                    n0 = f->faceTab[0], 
                    n1 = f->faceTab[1], 
                    n2 = f->faceTab[2];
                    V3XVECTOR LX, A, B;
                    if ((a[n0].z<CST_ZERO)&&(a[n2].z<CST_ZERO))
                    {
                        V3XVector_Dif(&LX, &mesh->vertex[n0], &mesh->vertex[n2]);
                        V3XVector_Avg(&B, &a[n2], &a[n0]);
                        A.x = (LX.x==CST_ZERO) ? fabs(LX.z) : fabs(LX.x);
                        A.y = (LX.y==CST_ZERO) ? fabs(LX.z) : fabs(LX.y);
                        A.z = B.z;
                        if (A.z>=CST_ZERO) f->visible = 0;
                        else
                        {
                            V3XVECTOR *s0 = d + n0;
                            V3XVECTOR *s1 = d + n1;
                            if (Mat->scale)
                            {
                                V3XSCALAR zoom = xSHRD((V3XSCALAR)Mat->scale, 7);
                                A.x = MULF32(A.x, zoom);
                                A.y = MULF32(A.y, zoom);
                            }
                            V3XVector_ProjectWithCenter(*s0, B);
                            V3XVector_ProjectWithoutCenter(*s1, A);
                            s0->x -= (s1->x)/2;
                            s0->y -= (s1->y)/2;
                            isVisible = 1;
                        }
                    } else f->visible = 0;
                }
                else
                if (f->visible)
                {
                    u_int32_t *p1;
                    int32_t m;
                    for(
                    f->NeedToClip = 0, 
                    p1 = f->faceTab, 
                    m = f->numEdges;
                    m != 0;
                    p1++, 
                    m--)
                    {
                        if (V3X.Buffer.flag[*p1]==2) f->NeedToClip++;
                    }
                    if (f->NeedToClip==f->numEdges)
                    {
                        f->visible = 0;
                    } else isVisible=1;
                }
            }
        }
    }
    if ((!isVisible)&&(!(mesh->flags&V3XMESH_FULLUPDATE))) 
    {  
        return 1;
    }
    V3XMesh_T2(mesh, &Matrix); // Lighting.
    V3XMesh_T3(mesh );          // Face building.    
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void space_pixel(V3XKEYANGLE *Ob_info, V3XVECTOR *b, V3XVECTOR *a)
*
* DESCRIPTION : resultat dans A (ne l'utiliser que par necessite, ou calculer
* la matrix, puis faire une PROD_VECT_MATRIX3x3
*
*/
void V3XMesh_RenderPoints(V3XMATRIX *Mat, V3XVECTOR *vertex_list, int32_t maxvertex, int color)
{
    int32_t i;
    V3XSCALAR *p;
    V3XMATRIX Matrix;
    V3XVECTOR *a;
    V3XVECTOR b;
    V3XMatrix_Transform(Matrix, (*Mat));
    p = Matrix.Matrix;
    for (a=vertex_list, i=maxvertex;i!=0;a++, i--)
    {
        V3XVector_ApplyMatrixTrans(b, (*a), p);
        if (b.z<0)
        {
            int32_t x, y;
            V3XVECTOR c;
            V3XVector_ProjectWithCenter(c, b);
            x=(int32_t)c.x;
            y=(int32_t)c.y;
            if ((x>=GX.View.xmin)&&(y>=GX.View.ymin)
            &&(x<=GX.View.xmax)&&(y<=GX.View.ymax))
            GX.gi.drawPixel(x, y, color);
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XVector_WorldPos(V3XMATRIX *Mat, V3XVECTOR *input, V3XVECTOR *result)
*
* DESCRIPTION :
*
*/
static V3XMATRIX m1 = {
    {CST_ONE, CST_ZERO , CST_ZERO, 
        CST_ZERO, CST_ONE, CST_ZERO, 
        CST_ZERO, CST_ZERO , CST_ONE, 
    CST_ZERO, CST_ZERO , CST_ZERO }
};
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XVector_WorldPos(V3XMATRIX *Mat, V3XVECTOR *input, V3XVECTOR *result)
*
* Description :  
*
*/
void V3XVector_WorldPos(V3XMATRIX *Mat, V3XVECTOR *input, V3XVECTOR *result)
{
    V3XMATRIX Matrix;
    if (!Mat) Mat = &m1;
    V3XMatrix_Transform(Matrix, (*Mat));
    V3XVector_ApplyMatrixTrans((*result), (*input), Matrix.Matrix);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3XVector_3Dto2D_pts(V3XMATRIX *Mat, V3XVECTOR *input, V3XVECTOR *result)
*
* DESCRIPTION :
*
*/
int V3XVector_3Dto2D_pts(V3XMATRIX *Mat, V3XVECTOR *input, V3XVECTOR *result)
{
    V3XVECTOR temp;
    int flag=0;
    V3XVector_WorldPos(Mat, input, &temp);
    V3XVector_ProjectWithCenterAndTest((*result), temp, flag);
    return flag==0;
}
