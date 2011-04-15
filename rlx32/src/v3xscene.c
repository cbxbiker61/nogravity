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
#include <string.h>
#include <stdlib.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "sysresmx.h"
#include "systools.h"
#include "gx_struc.h"
#include "gx_csp.h"
#include "gx_flc.h"
#include "gx_tools.h"
#include "gx_init.h"
#include "gx_rgb.h"
#include "v3xdefs.h"
#include "v3x_1.h"
#include "v3x_2.h"
#include "v3xsort.h"
#include "v3xtrig.h"
#include "v3xcoll.h"
#include "v3xmaps.h"
#include "v3xrend.h"
#include "v3xscene.h"
#include "v3x_priv.h"
#include "systime.h"
#include "fixops.h"
enum {
    V3XCLIPPED_VIEW=1,
    V3XCLIPPED_Z =2,
    V3XCLIPPED_SIDE=4
};
static V3XSCENE *__scene;
static V3XOVI *__ovi;
/*
static void V3X_Euler2Matrix(V3XMESH *Ob)
{
    V3XMATRIX *M = &Ob->matrix;
    V3XKEYEULER *key = &Ob->Tk.info;
    V3XMatrix_SetIdentity(M->Matrix);
    V3XMatrix_Rotate_X_Local((int)key->angle.x, M->Matrix);
    V3XMatrix_Rotate_Y_Local((int)key->angle.y, M->Matrix);
    V3XMatrix_Rotate_Z_Local((int)key->angle.z, M->Matrix);
    M->v.Pos = key->pos;
    return;
}
*/

SYS_THREAD static g_cThreadCollision;

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RLXAPI V3XPlug_CollisionMove(const void *data)
*
* DESCRIPTION :
*
*/
void RLXAPI V3XPlug_CollisionMove(const void *data)
{
    V3XOVI *OVI = (V3XOVI *)data;
    V3XORI    *ORI = OVI->ORI;
	if (ORI)
	{
    V3XCL      *OCs = ORI->Cs;
    if ((OCs)&&(V3X.Setup.flags&V3XOPTION_COLLISION ))
    {
        if (!(OCs->flags&V3XCMODE_DONTMOVE))
        {
            V3XVECTOR *p = &OCs->velocity;
            if (p->x||p->y||p->z)
            {
                V3XVECTOR *pos = &OVI->Tk->vinfo.pos;
                V3XVector_Madd(pos, p, pos, OCs->reaction);
                V3XVector_Scl(p, p, CST_ONE-OCs->reaction);
                OVI->state|=V3XSTATE_MATRIXUPDATE;
            }
        }
    }
	}
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void V3XScene_ClearViewport(V3XSCENE *Scene)
*
* DESCRIPTION :  Pre processing du rendu
*
*/
void V3XScene_Viewport_Clear(V3XSCENE *Scene)
{
    V3XLAYER *layer = &Scene->Layer;
    if (layer->bg.flags&V3XBG_CALCINDEX)
    {
        layer->bg.index_color = RGB_PixelFormat(layer->bg.BG_color.r, layer->bg.BG_color.g, layer->bg.BG_color.b);
        layer->bg.flags&=~V3XBG_CALCINDEX;
    }
    // ZBuffer
    if (V3X.Client->Capabilities&(GXSPEC_ENABLEZBUFFER|GXSPEC_ENABLEWBUFFER))
    if (!V3X.Client->ZbufferClear((rgb24_t*)&layer->bg.BG_color, 1., NULL)) return;
    // Arriere plan
    switch(layer->bg.flags&15) {
        case V3XBG_BLACK:
        GX.gi.clearBackBuffer();
        return;
        case V3XBG_IMG:
        if (layer->bg.bitmap.handle)
        {
            layer->bg._bitmap.x = (V3XSCALAR)GX.View.xmin;
            layer->bg._bitmap.y = (V3XSCALAR)GX.View.ymin;
            layer->bg._bitmap.x2 = (V3XSCALAR)GX.View.xmax+1;
            layer->bg._bitmap.y2 = (V3XSCALAR)GX.View.ymax+1;
            layer->bg._bitmap.z = V3X.Clip.Far;
            V3X_CSP_Prepare(&layer->bg.bitmap, 8);
        }
        return;
        case V3XBG_GRAD:
        {
            rgb32_t *s = &layer->bg.UP_color;
            rgb32_t *e = &layer->bg.DW_color;
            uint32_t l = GX.View.xmax-GX.View.xmin;
            int32_t  b = GX.View.ymax-GX.View.ymin, y;
            for (y=GX.View.ymin;y<GX.View.ymax;y++)
            {
                rgb24_t d;
                int32_t a = y - GX.View.ymin;
                int32_t c = VDIV(a, b);
                d.r = (uint8_t)((int32_t)s->r + (int32_t)VMUL(e->r - s->r, c));
                d.g = (uint8_t)((int32_t)s->g + (int32_t)VMUL(e->g - s->g, c));
                d.b = (uint8_t)((int32_t)s->b + (int32_t)VMUL(e->b - s->b, c));
                GX.gi.drawHorizontalLine(y, GX.View.xmin, l, RGB_PixelFormatEx(&d));
            }
        }
        break;
        case V3XBG_COLOR:
        GX.gi.drawFilledRect(GX.View.xmin, GX.View.ymin, GX.View.xmax, GX.View.ymax, layer->bg.index_color);
        break;
        case V3XBG_NONE:
        if (V3X.Client->Capabilities&GXSPEC_FORCEWIREFRAME)
        GX.gi.drawFilledRect(GX.View.xmin, GX.View.ymin, GX.View.xmax, GX.View.ymax, layer->bg.index_color);
        return;
        case V3XBG_SIDE:
        break;
        default:
        return;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void v3x_render_camera(V3XKEY *s)
*
* DESCRIPTION :  Build camera matrix.
*
*/
static void v3x_render_camera(V3XKEY *s)
{
    switch(V3X.Camera.matrix_Method) {
        case V3XMATRIX_Euler:
        case V3XMATRIX_Vector:
        if (s) V3X.Camera.Tk = *s;
        V3XMatrix_MeshTransform(&V3X.Camera);
        break;
    }
    if (V3X.Camera.flags&1)
    {
        V3XMATRIX Mat, M1;
        M1 = V3X.Camera.M;
        V3XMatrix_Rot_XYZ(Mat.Matrix, V3X.Camera.Tk.info.angle);
        V3XVector_Set(&Mat.v.Pos, 0, 0, 0);
        // (Mat.Matrix);
        V3XMatrix_Multiply3x3(V3X.Camera.M.Matrix, Mat.Matrix, M1.Matrix);
        // (V3X.Camera.M.Matrix);
    }
    V3XVector_Avg(&V3X.Light.HVector, &V3X.Light.light[0].vector, &V3X.Camera.transposed);
	if (V3XVector_Length(&V3X.Light.HVector)<CST_EPSILON)
		V3XVector_Set(&V3X.Light.HVector, 0,0,1);
	else
		V3XVector_Normalize(&V3X.Light.HVector, &V3X.Light.HVector);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : static void RLXAPI V3XScene_LightBuild(V3XSCENE *Scene, V3XOVI *OVI)
*
* DESCRIPTION :  Add light in the lightner pipeline. OVI must be a light node.
*
*/
static void RLXAPI V3XScene_LightBuild(V3XSCENE *Scene, V3XOVI *OVI)
{
    unsigned tm = 0;
    V3XLIGHT *slight = OVI->light;
    if (slight->status&2) tm = V3X.Time.ms;
    if (V3X.Light.numSource<V3X.Buffer.MaxLight)
    {
        V3XSPRITE *lt;
        // Blinking lights
        if ((slight->status&2) && (tm>slight->Timer))
        {
            if (slight->status&1)
            {
                slight->Timer = tm + slight->TimeOn;
                slight->status&=~1;
            }
            else
            {
                slight->Timer = tm + slight->TimeOff;
                slight->status|=1;
            }
        }
        if (slight->range==CST_ZERO) slight->range = V3X.Light.fogDistance;
        if (slight->diminish==0)  slight->diminish=1.f/2000.f;
        if ((slight->status&1)==0)
        {
            V3XLIGHT *light = V3X.Light.light + V3X.Light.numSource;
            V3X.Light.numSource++;
            *light = *OVI->light;
            lt = (V3XSPRITE *)light->flare;
            if (lt)
            {
                unsigned flags = 0;
                if (!light->flaresize) light->flaresize = 128;
                if ( light->flags & V3XLIGHTCAPS_REALLF)        flags|=1;
                if ((light->flags & V3XLIGHTCAPS_FIXEDSIZE)==0) flags|=2;
                if (!V3X_CSP_Set3D(&lt->sp, &light->pos, light->flaresize, flags))
                {
                    V3XMATERIAL *mat = &lt->_sp.mat;
                    int spi = 32;
                    V3XOVI *who = NULL;
                    int b = light->flags & V3XLIGHTCAPS_REALLF ? V3XVECTOR_IsVisible(Scene, &V3X.Camera.M.v.Pos, &light->pos, 1, &who) : 1;
                    if (b)
                    {
                        spi = 64;
                    mat->alpha = (mat->alpha<255-spi) ? mat->alpha + spi : 255;                           }
                    else
                    {
                        spi = 16;
                        mat->alpha = (mat->alpha>spi) ? mat->alpha - spi : 0;
                    }
                    mat->diffuse.r = (uint8_t)xMUL8(mat->alpha, light->color.r);
                    mat->diffuse.g = (uint8_t)xMUL8(mat->alpha, light->color.g);
                    mat->diffuse.b = (uint8_t)xMUL8(mat->alpha, light->color.b);
                    V3X_CSP_Draw(&lt->sp, V3XCSPDRAW_CLIP|V3XCSPDRAW_INSTANCE);
                }
            }
        }
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3XViewport_Config(void)
*
* DESCRIPTION :  Config viewport
*
*/
static void V3XViewport_Config(void)
{
    V3XSCALAR focaley = V3X.ViewPort.Focal;
    V3XSCALAR focalex = V3X.ViewPort.Focal * V3X.ViewPort.Ratio;
    V3XSCALAR window_Largeur = (V3XSCALAR)((GX.View.xmax-GX.View.xmin+1)>>1);
    V3XSCALAR window_Hauteur = (V3XSCALAR)((GX.View.ymax-GX.View.ymin+1)>>1);
    V3XVector_Set(&V3X.Clip.normalLeft , - focalex,   CST_ZERO, - window_Largeur );
    V3XVector_Normalize(&V3X.Clip.normalLeft, &V3X.Clip.normalLeft);
    V3XVector_Set(&V3X.Clip.normalRight, focalex,   CST_ZERO, - window_Largeur );
    V3XVector_Normalize(&V3X.Clip.normalRight, &V3X.Clip.normalRight);
    V3XVector_Set(&V3X.Clip.normalTop  ,   CST_ZERO, - focaley, - window_Hauteur );
    V3XVector_Normalize(&V3X.Clip.normalTop, &V3X.Clip.normalTop);
    V3XVector_Set(&V3X.Clip.normalBottom,   CST_ZERO, focaley, - window_Hauteur );
    V3XVector_Normalize(&V3X.Clip.normalBottom, &V3X.Clip.normalBottom);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : static unsigned V3XMesh_Cull(V3XOVI* OVI)
*
* DESCRIPTION :  Determines if an object is visible or not
*
*/
static unsigned V3XMesh_Cull(V3XOVI* OVI)
{
    unsigned flags=0;
    V3XORI    *ORI = OVI->ORI;
    V3XSCALAR radius = ORI->global_rayon * OVI->mesh->scale * 1.1f;
    V3XVECTOR c;
    if (radius==CST_ZERO) return 0;
    // Projection du centre de la sphere englobante (resultat dans c)
    V3XVector_WorldPos(&OVI->node->matrix, &ORI->global_center, &c);
    // Clip pyramide
    c.z = -c.z;
    OVI->distance = c.z;
    OVI->mesh->flags&=~V3XMESH_LOWDETAIL;
    if (c.z) // Culling and light processing;
    {
        V3XSCALAR ray2 = (V3X.ViewPort.Focal*radius)/c.z;
        if ((OVI->mesh->flags&V3XMESH_LODNEVER)==0)
        if (fabs(ray2)<V3X.ViewPort.minTextureVisibleRadius * (float)OVI->mesh->numFaces)
        {
            OVI->mesh->flags|=V3XMESH_LOWDETAIL;
        }
        if (fabs(ray2)<V3X.ViewPort.minVisibleRadius * (float)OVI->mesh->numFaces)
        {
            OVI->state|=V3XSTATE_CULLEDOFR;
            return 1;
        }
    }
    if (OVI->state&V3XSTATE_CULLNEVER)
		return 0;

    V3XCLIP_SIDE(&V3X.Clip.normalLeft);
    V3XCLIP_SIDE(&V3X.Clip.normalRight);
    V3XCLIP_SIDE(&V3X.Clip.normalTop);
    V3XCLIP_SIDE(&V3X.Clip.normalBottom);
    {
        // Clip Z Near
        if ( c.z > -V3X.Clip.Near)
        {
            if (c.z - radius <= -V3X.Clip.Near)
            {
                flags |= V3XCLIPPED_Z;
            }
        }
        else
        {
            flags |= (c.z + radius > -V3X.Clip.Near) ? V3XCLIPPED_Z : V3XCLIPPED_VIEW;
        }
        // Clip Z Far
        {
            if ( c.z < -V3X.Clip.Far)
            {
                if (c.z + radius > -V3X.Clip.Far)
                {
                    flags |= V3XCLIPPED_Z;
                }
            }
            else
            {
                flags|= (c.z - radius < -V3X.Clip.Far) ? V3XCLIPPED_Z : V3XCLIPPED_VIEW;
            }
        }
    }
    return flags&V3XCLIPPED_VIEW;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XScene_ObjectBuild(V3XOVI *OVI, int spec)
*
* DESCRIPTION : Convert a mesh to projected polygones and add them in the polys-pipeline.
*
*/
void V3XScene_ObjectBuild(V3XOVI *OVI, int spec)
{
    V3XORI    *ORI = OVI->ORI;
    if (ORI)
    switch(ORI->type) {
        case V3XOBJ_MESH:
        if (((spec&1)==0)&&(ORI->mesh)) V3XCache_Mesh(ORI->mesh);
        if (
        (OVI->state&V3XSTATE_BSPCULLED)
        || V3XMesh_Cull(OVI)
		|| (!ORI->mesh)
        )
        {
            V3X.Buffer.ObjFailed++;
            OVI->state|=V3XSTATE_CULLED;
        }
        else
        {
            __ovi = OVI;
            spec = V3XMesh_Transform(OVI->mesh);
            V3X.Buffer.ObjFailed+=spec;
            __ovi = NULL;
        }
        break;
        default:
        V3X.Buffer.ObjFailed++;
        break;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : int32_t static V3x_TestAllCollision(V3XSCENE *Scene)
*
* DESCRIPTION : Test des collisions
*
*/
int32_t static V3x_TestAllCollision(V3XSCENE *Scene)
{
    V3XORI    *ORI, *ORI2;
    V3XOVI **OOVI, **OOVI2;
    V3XOVI *OVI, *OVI2;
    V3XCL *OCs, *OC2s;
    unsigned int i, j;
    // Calcul All

    if (V3X.Buffer.MaxObj>1)
    for (OOVI=(V3XOVI **)V3X.Buffer.OVI, i=0;i<V3X.Buffer.MaxObj-1;i++, OOVI++)
    {
        OVI = *OOVI;
        ORI = OVI->ORI;
        if (OVI->state&V3XSTATE_PRSCOLLISION)
        {
            OCs = ORI->Cs;
            // Test Collision
            OCs->mesh_ref = ORI->type == V3XOBJ_CAMERA ? NULL : OVI->mesh;

			{
				V3XCL_Xform(OCs);
			}
            for (OOVI2=OOVI+1, j=i+1;j<V3X.Buffer.MaxObj;j++, OOVI2++)
            {
                OVI2 = *OOVI2;
                ORI2 = OVI2->ORI;
                OC2s = ORI2->Cs;
                if (OVI2->state&V3XSTATE_PRSCOLLISION)
                if (OVI2->parent!=OVI)
                if (OVI->parent!=OVI2)
                if (((OCs->flags&V3XCMODE_DONOTTESTSAMEID)==0)
                ||  ((OCs->flags&V3XCMODE_DONOTTESTSAMEID)
                && (OCs->ID!=OC2s->ID)))
                {
                    OC2s->mesh_ref = ORI->type == V3XOBJ_CAMERA ? NULL : OVI2->mesh;
                    //if (OVI2->state&V3XSTATE_INSTANCED)
					{
						V3XCL_Xform(OC2s);
					}
                    //if ((!OCs->hitCount)&&(!OC2s->hitCount))
                    if (V3XCL_Test(OCs, OC2s)) // Retourne le numero de la sphere
                    {
                        /*
                        V3XSCALAR p1=OCs->weight, p2=OC2s->weight;
                        if (p1<p2) p2 = p1;
                        if (!p2) p2=p1=CST_ONE;
                        */

                        OC2s->hitID = (uint16_t)i;
                        OCs->hitID = (uint16_t)j;
                        OCs->hitCount++;
                        OC2s->hitCount++;
                        OVI2->collisionList = OVI;
                        OVI->collisionList = OVI2;
                    }
                }
            }
            OVI->state|=V3XSTATE_PROCESSED;
        }
    }

	g_cThreadCollision.nStatus = 1;
    UNUSED(Scene);
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XScene_MatrixBuild(V3XOVI *OVI)
*
* Description :   Build matrices objects and hiearchical process;
*
*/
void V3XScene_MatrixBuild(V3XOVI *OVI)
{
    V3XMATRIX M;
    V3XOVI *OVIp, *OVIt;
    V3XORI    *ORI;
    V3XCL      *OCs;
    V3XMESH           *obj;
    V3XLIGHT       *light;
    ORI = OVI->ORI;
    if (!ORI) return;
    if ((OVI->state&V3XSTATE_PROCESSED)||(!ORI))
    {
        return;
    }
	SYS_ASSERT(V3X.Buffer.OVI);
    if (V3X.Buffer.MaxObj<V3X.Buffer.MaxSceneNodes)
		V3X.Buffer.OVI[V3X.Buffer.MaxObj++]=(uint8_t*)OVI;
    OVI->state |= V3XSTATE_PROCESSED;
    obj = OVI->mesh;
    light = OVI->light;
    OCs = ORI->Cs;
    OVIp = OVI->parent;
    while(!((OVIp==NULL)||(OVI->state&V3XSTATE_MATRIXUPDATE)))
    {
        if (OVIp->state&V3XSTATE_MATRIXUPDATE)
			OVI->state|=V3XSTATE_MATRIXUPDATE;
        OVIp = OVIp->parent;
    }
    if (OVI->state&V3XSTATE_MATRIXUPDATE)
    switch(ORI->type) {
        case V3XOBJ_MESH:
        case V3XOBJ_DUMMY:
        case V3XOBJ_PORTAL:
        {
            if (OVI->state&V3XSTATE_GIANT)
            {
                V3XMatrix_SetIdentity(obj->matrix.Matrix);
                obj->matrix.v.Pos = V3X.Camera.M.v.Pos;
                obj->Tk.info.pos = obj->matrix.v.Pos;
            }
            else
            switch(OVI->matrix_Method) {
                case V3XMATRIX_Quaternion:
                v3x_EvalMatrixRotation_q(obj);
                break;
                case V3XMATRIX_Vector:
                v3x_EvalMatrixRotation2(obj);
                break;
                case V3XMATRIX_Vector2:
                v3x_EvalMatrixRotation3(obj);
                break;
                case V3XMATRIX_Custom:
                break;
                case V3XMATRIX_Euler:
                //V3X_Euler2Matrix(obj);
                v3x_EvalMatrixEuler(obj);
                break;
                default:
                V3XMatrix_SetIdentity(obj->matrix.Matrix);
                break;
            }
            // (obj->matrix.Matrix);
        }
        break;
        case V3XOBJ_LIGHT:
        if (!OVIp)
        {
            light->pos = light->Tk.vinfo.pos;
            light->target = light->Tk.vinfo.target;
        }
        break;
        case V3XOBJ_CAMERA:
        v3x_render_camera(OVI->Tk);
        break;
    }
    // Hiearchy
    OVIp = OVI->parent;
    OVIt = OVI->target;
    if ((OVIp)&&(OVI->state&V3XSTATE_MATRIXUPDATE))
    {
        V3XNODE *objP = OVIp->node;
        if ((OVIp->state&V3XSTATE_PROCESSED)==0) V3XScene_MatrixBuild(OVIp);
        switch(ORI->type) {
            case V3XOBJ_MESH: // le child est un object
            case V3XOBJ_DUMMY:
            case V3XOBJ_PORTAL:
            switch(OVIp->ORI->type) {
                case V3XOBJ_MESH:   // Hiearchie niveau
                case V3XOBJ_DUMMY:
                case V3XOBJ_PORTAL:
                M = obj->matrix;
                V3XMatrix_Multiply3x4(obj->matrix, objP->matrix, M);
                break;
                case V3XOBJ_CAMERA: // Hiearchie Simple sur camera/lumiere
                V3XVector_Sum(&obj->matrix.v.Pos, &obj->Tk.vinfo.pos, &objP->Tk.vinfo.pos);
                break;
                case V3XOBJ_LIGHT:  // Les lights ne peuvent pas etres Parent
                break;
            }
            // (obj->matrix.Matrix);
            break;
            case V3XOBJ_LIGHT:
            {
                V3XKEYCAMERA *s = &OVI->Tk->vinfo;
                if (OVIp->state&V3XSTATE_GIANT)
                {
                    V3XVector_Sum(&OVI->light->pos, &V3X.Camera.M.v.Pos, &s->pos);
                    V3XVector_Sum(&OVI->light->target, &V3X.Camera.M.v.Pos, &s->target);
                }
                else
                {
                    V3XMATRIX *mat = NULL;
                    if (OVIp->ORI->type==V3XOBJ_CAMERA)
                    {
                        mat = &V3X.Camera.M;
                    }
                    else
                    {
                        if (OVIp->mesh)
                        mat = &OVIp->mesh->matrix;
                    }
                    if (mat)
                    {
                        // voir pour si le pere est la camera
                        V3XVector_ApplyMatrixTrans(OVI->light->pos, s->pos, mat->Matrix);
                        if (OVI->light->type==V3XLIGHTTYPE_SPOT)
                        V3XVector_ApplyMatrixTrans(OVI->light->target, s->target, mat->Matrix);
                    }
                }
            }
            break;
            case V3XOBJ_CAMERA:
            switch(OVIp->ORI->type) {
                case V3XOBJ_MESH:
                case V3XOBJ_DUMMY:
                {
                    V3XVECTOR a = OVI->Tk->vinfo.pos;
                    V3XVector_ApplyMatrixTrans( V3X.Camera.Tk.vinfo.pos, a, objP->matrix.Matrix);
                    v3x_render_camera(NULL);
                }
                break;
            }
            break;
        }
    }
    // Target matrices building;
    if ((OVIt)&&(OVI->state&V3XSTATE_MATRIXUPDATE))
    {
        V3XNODE *objT = OVIt->node;
        if (((OVIt->state&V3XSTATE_PROCESSED)==0)&&(OVIt!=OVIp)) V3XScene_MatrixBuild(OVIt);
        switch(ORI->type) {
            case V3XOBJ_MESH:
            case V3XOBJ_DUMMY:
            switch(OVIt->ORI->type) {
                case V3XOBJ_MESH:
                case V3XOBJ_DUMMY:
                case V3XOBJ_CAMERA:
                case V3XOBJ_LIGHT:
                {
                    obj->Tk.vinfo.target = objT->matrix.v.Pos;
                    v3x_EvalMatrixRotation2(obj);
                }
            }
            break;
            case V3XOBJ_LIGHT:
            switch(OVIt->ORI->type) {
                case V3XOBJ_MESH:
                case V3XOBJ_DUMMY:
                {
                    V3XVECTOR b = OVI->Tk->vinfo.target;
                    V3XVector_ApplyMatrixTrans( OVI->light->target , b, objT->matrix.Matrix);
                }
            }
            break;
            case V3XOBJ_CAMERA:
            switch(OVIt->ORI->type) {
                case V3XOBJ_MESH:
                case V3XOBJ_DUMMY:
                {
                    V3XVECTOR b = OVI->Tk->vinfo.target;
                    V3XVector_ApplyMatrixTrans( V3X.Camera.Tk.vinfo.target, b, objT->matrix.Matrix);
                    v3x_render_camera(NULL);
                }
                break;
            }
            break;
        }
    }
    if (
    (V3X.Setup.flags&V3XOPTION_COLLISION)&&
	((OVI->state&V3XSTATE_HIDDEN)==0)
    )
    {
        OVI->collisionList = NULL;
        if (OCs)
        {
            OVI->state   |= V3XSTATE_PRSCOLLISION;

            OCs->hitCount = 0;
            OCs->hitID = 0;
            OCs->last_hit = NULL;
            if (ORI->type == V3XOBJ_CAMERA)
            {
                OCs->mesh_ref = NULL;
                V3XCL_XformNoRef(OCs, &OVI->Tk->vinfo.pos);
            }
            else
            {
                OCs->mesh_ref = obj;
                V3XCL_Xform(OCs);
            }
        }
    }
    OVI->state &= ~(V3XSTATE_MATRIXUPDATE|V3XSTATE_TOPROCESS);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XScene_Viewport_Render(V3XSCENE *Scene)
*
* DESCRIPTION :  Pipeline rasterizer call; All primitives are sent to renderer from here;
*
*/
void V3XScene_Viewport_Render(V3XSCENE *Scene)
{
    int a = GX.View.State&GX_STATE_SCENEBEGUN;
    GXVIEWPORT Old;
    if (Scene->Layer.bg.flags&V3XBG_STEREO)
    {
        Old = GX.View;
        GX.View.lPitch*=2;
        GX.View.ymax = (GX.View.ymax/2);
        GX.View.ymin = (GX.View.ymin/2);
    }
    if (!a) V3X.Client->BeginList();
    V3X.Setup.pre_render();
    if (V3X.Buffer.MaxFaces)
    {
        V3X.Client->Render();
    }
    V3X.Setup.post_render();
    if (V3X.Ln.nbLines)
    {
        V3X.Client->DrawPrimitives(
        V3X.Ln.lineBuffer, NULL, 0,
        V3X.Ln.nbLines,
        V3XRCLASS_wired|256,
        V3X.Ln.lineColor);
    }
    if (!a) V3X.Client->EndList();

    V3X.Ln.nbLines = 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : static void V3x_render_buildGeometry(V3XSCENE *Scene, GXVIEWPORT *fen, int spec)
*
* DESCRIPTION : Transforms the mesh pipeline to polys pipeline;
*
*/
static void V3x_render_buildGeometry(V3XSCENE *Scene, GXVIEWPORT *fen, int spec)
{
    GXVIEWPORT Old = GX.View;
    V3XVIEWPORT OldV = V3X.ViewPort;
    unsigned int i;
    if (fen) GX.View = *fen;
    V3X.Buffer.ObjFailed = 0;
    V3X.Buffer.MaxFaces = 0;
    V3X.Buffer.MaxClipped = 0;
    V3X.Buffer.MaxMat  = 0;
    V3X.Light.numSource = 0;
    if (Scene->Layer.bg.bitmap.handle)
    {
        V3X_CSP_Draw(&Scene->Layer.bg.bitmap, 0);
    }

    if (fen)
    {
        V3XOVI *OVI=Scene->OVI;
        // render light list.
        for (i=0;i<Scene->numOVI;i++, OVI++)
        {
            if (OVI->ORI)
            if (OVI->ORI->type==V3XOBJ_LIGHT)
            if ((OVI->state&(V3XSTATE_HIDDENDISPLAY|V3XSTATE_HIDDEN))==0)
            V3XScene_LightBuild(Scene, OVI);
        }
        V3X.Setup.add_lights();
        // render mesh list.
        for (i=0;i<V3X.Buffer.MaxObj;i++)
        {
            OVI=(V3XOVI*)V3X.Buffer.OVI[i];
            if ((OVI->state&V3XSTATE_HIDDENDISPLAY)==0)
            {
                V3XScene_ObjectBuild(OVI, spec);
            }
        }
    }
    V3X.Setup.add_poly();
    if (V3X.Buffer.MaxFaces>1)
    {
        v3xpoly_SortByDistance(V3X.Buffer.RenderedFaces, V3X.Buffer.RenderedFaces+V3X.Buffer.MaxFaces-1);
        if (V3X.Client->Capabilities&(GXSPEC_ENABLEZBUFFER|GXSPEC_ENABLEWBUFFER))
        {
            //v3xpoly_SortByTexture(V3X.Buffer.RenderedFaces, V3X.Buffer.RenderedFaces+V3X.Buffer.MaxFaces-1);
            v3xpoly_SortByID(V3X.Buffer.RenderedFaces, V3X.Buffer.RenderedFaces+V3X.Buffer.MaxFaces-1);
        }
    }
    GX.View  = Old;
    V3X.ViewPort = OldV;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void v3x_GetGlobalMatrix(V3XOVI *OVI, V3XMATRIX *Mat)
*
* DESCRIPTION :
*
*/
static void v3x_GetGlobalMatrix(V3XOVI *OVI, V3XMATRIX *Mat)
{
    V3XMESH  *obj = OVI->mesh;
    switch(OVI->matrix_Method) {
        case V3XMATRIX_Quaternion:
        v3x_EvalMatrixRotation_q(obj);
        break;
        case V3XMATRIX_Vector:
        v3x_EvalMatrixRotation2(obj);
        break;
        case V3XMATRIX_Vector2:
        v3x_EvalMatrixRotation3(obj);
        break;
        case V3XMATRIX_Euler:
        v3x_EvalMatrixEuler(obj);
        break;
    }
    // (obj->matrix.Matrix);
    if (OVI->parent)
    {
        V3XMESH *objP = OVI->parent->mesh;
        v3x_GetGlobalMatrix(OVI->parent, &objP->matrix);
        V3XMatrix_Multiply3x4(*Mat, objP->matrix, obj->matrix);
    } else *Mat = obj->matrix;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : static int v3x_qsort_functionZ( const void *a, const void *b)
*
* DESCRIPTION :  Preprocessing sort.
*
*/
#define SGN2(a, b)    ((a==b) ? 0 : ((a>b) ? 1 : -1))
static int v3x_qsort_functionZ( const void *a, const void *b)
{
    V3XOVI **aa=(V3XOVI**)a, **bb=(V3XOVI**)b;
    return SGN2((**bb).distance, (**aa).distance);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XScene_Viewport_Build(V3XSCENE *Scene, GXVIEWPORT *ViewPrt)
*
* DESCRIPTION :  Build scene.
*
*/
void V3XScene_Viewport_Build(V3XSCENE *Scene, GXVIEWPORT *ViewPrt)
{
    unsigned int i;
    GXVIEWPORT    Old = GX.View;
    V3XOVI *OVI;
    V3XORI    *ORI;
    V3X.Buffer.MaxObj  = 0;
    __scene = Scene;
    // Animated materials
    if (ViewPrt)
    {
        GX.View = *ViewPrt;
        V3XResources_Animated(&V3X.Cache);
    }
    V3XViewport_Config();
    // Traitement des objets
    for (i=Scene->numOVI, OVI=Scene->OVI;i!=0;OVI++, i--)
    {
        ORI = OVI->ORI;
        OVI->state &= V3XSTATE_RESETMASK;
        if (ORI)
        {
            if ((V3X.Setup.flags&V3XOPTION_PORTALCULL)&&(OVI->subDefinition&V3XSUBDEF_PSECTOR))
            {
                OVI->state|=V3XSTATE_BSPCULLED;
            }
            switch(ORI->type) {
                case V3XOBJ_MESH:
                case V3XOBJ_DUMMY:
                OVI->state |= (OVI->state&V3XSTATE_HIDDEN) ? 0 : V3XSTATE_TOPROCESS;
                break;
                case V3XOBJ_NONE:
                OVI->distance = -10;
                break;
                case V3XOBJ_LIGHT:
                case V3XOBJ_CAMERA:
                OVI->state |= (OVI->state&V3XSTATE_HIDDEN) ? 0 : V3XSTATE_TOPROCESS;
                OVI->distance = CST_MAX;
                break;
                default:
                OVI->state |= V3XSTATE_TOPROCESS;
                OVI->distance = CST_MAX;
                break;
            }
        }
    }
    // Recurse object
    for (i=Scene->numOVI, OVI=Scene->OVI;i!=0;OVI++, i--)
    {
        if (OVI->state&V3XSTATE_TOPROCESS)
        {
            if ((OVI->ORI)&&(OVI->parent)&&(OVI->parent->state&V3XSTATE_MATRIXUPDATE))
            OVI->state|=V3XSTATE_MATRIXUPDATE|V3XSTATE_TOPROCESS;
        }
    }
    // Calcul des matrices
    for (i=Scene->numOVI, OVI=Scene->OVI;i!=0;OVI++, i--)
    {
        if (OVI->state&V3XSTATE_TOPROCESS)
        {
            V3XScene_MatrixBuild(OVI);
        }
        else
        {
            V3X.Buffer.ObjFailed++;
        }
    }
    for (i=Scene->numOVI, OVI=Scene->OVI;i!=0;OVI++, i--)
    {
        unsigned j;
        for (j=0;j<V3X.Plugin.maxPlug;j++)  V3X.Plugin.plug[j].call((void*)OVI);
    }
    // ?
    qsort(V3X.Buffer.OVI, V3X.Buffer.MaxObj, sizeof(V3XOVI*), v3x_qsort_functionZ);
    // Lance le test de collision
	V3x_TestAllCollision(Scene);
    // Render Draw ... pendant ce temps, on construit la geometrie
    V3x_render_buildGeometry(Scene, ViewPrt, 0);
    if (V3X.Setup.flags&V3XOPTION_COLLISION)
    {
        if ((V3X.Setup.flags&V3XOPTION_COLLISIONDRAW)&&(ViewPrt))
        for (i=0;i<V3X.Buffer.MaxObj;i++)
        {
            OVI = *(((V3XOVI**)V3X.Buffer.OVI)+i);
            if ((OVI->state&V3XSTATE_DISPLAYCOL)&&(OVI->state&V3XSTATE_PRSCOLLISION))
            {
                V3XCL_Draw(OVI->ORI->Cs);
            }
        }
    }
    GX.View=Old;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XCL_MESH_Optimize(V3XCL_MESH *mesh, V3XSCENE *scene, V3XVECTOR *pos, unsigned mode)
*
* Description :  Optimize collision mesh
*
*/
void V3XCL_MESH_Optimize(V3XCL_MESH *mesh, V3XSCENE *scene, V3XVECTOR *pos, unsigned mode)
{
    // Trés simple avec le portal, on traite que le secteur actuel.
    if (mode&V3XCL_MESHOPT_PORTAL)
    {
        unsigned i;
        V3XOVI *OVI = scene->OVI;
        for (i=0;i<mesh->maxsectors;OVI++, i++)
        {
            if ((OVI->state&V3XSTATE_CULLED)||
            (OVI->state&V3XSTATE_BSPCULLED))
            mesh->sectorList[i] = 0x1;
            else mesh->sectorList[i] = 0;
        }
    }
    // On ne traite pas les objets non visible.
    if (mode&V3XCL_MESHOPT_HIDDENOBJ)
    {
        unsigned i;
        V3XOVI *OVI = scene->OVI;
        for (i=0;i<mesh->maxsectors;OVI++, i++)
        {
            if (OVI->state&V3XSTATE_CULLED)
            {
                mesh->sectorList[i]=0x1;
            }
            else mesh->sectorList[i] = 0;
        }
    }
    UNUSED(pos);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3XVECTOR_IsVisible(V3XSCENE *Scene, V3XVECTOR *start, V3XVECTOR *end, unsigned hint)
*
* Description :
*
*/
int V3XVECTOR_IsVisible(V3XSCENE *Scene, V3XVECTOR *start, V3XVECTOR *end, unsigned hint, V3XOVI **who_hide)
{
    int i;
    V3XOVI *OVI;
    if (!Scene) Scene = __scene;
    for (OVI=Scene->OVI, i=0;i<Scene->numOVI;i++, OVI++)
    {
        if (OVI->ORI)
        {
            V3XORI *ORI = OVI->ORI;
            if ((ORI->type == V3XOBJ_MESH)&&(OVI!=__ovi))
            {
                if ((OVI->state&(V3XSTATE_HIDDENDISPLAY|V3XSTATE_HIDDEN|V3XSTATE_GIANT))==0)
                if ( ORI->global_rayon>=CST_EPSILON)
                if ( ((hint&1)==0)|| ((hint&1)&&(!(OVI->state&V3XSTATE_CULLED))))
                {
                    int j;
                    V3XMESH *mesh = OVI->mesh;
                    V3XPOLY *f = mesh->face;
                    V3XVECTOR center, isect;
                    // La droite coupe la sphere englobante
                    V3XVector_ApplyMatrixTrans(center, ORI->global_center, mesh->matrix.Matrix);
					if ((!__ovi)||(ORI->global_rayon>__ovi->ORI->global_rayon))
                    if (V3XVector_IntersectSphereSegment(ORI->global_rayon, &center, start, end))
                    {
                        if (hint&2)
                        {
                            int ret = 0;
                            for (j=0;j<mesh->numMaterial;j++)if (!mesh->material[j].info.Transparency) ret=1;
                        }
                        for (j=0;j<mesh->numFaces;j++, f++)
                        {
                            V3XVECTOR roty[4];
                            V3XVECTOR *normal = mesh->normal_face + j;
                            V3XMATERIAL *mat = (V3XMATERIAL*)f->Mat;
                            if ((f->matIndex<0xffff)||(!mat->info.Transparency))
                            {
                                V3XVECTOR norm;
                                V3XVector_ApplyMatrixTrans(roty[0], mesh->vertex[f->faceTab[0]], mesh->matrix.Matrix);
                                V3XVector_ApplyMatrix(norm, *normal, mesh->matrix.Matrix);
                                if (V3XVector_IntersectPlaneSegment(&isect, start, end, &norm, roty+0))
                                {
                                    V3XSCALAR de = V3XVector_DistanceSq(&isect, end);
                                    if (de>0.0001f)
                                    {
                                        int k;
                                        for (k=1;k<f->numEdges;k++)
                                        {
                                            V3XVector_ApplyMatrixTrans(roty[k],
                                            mesh->vertex[f->faceTab[k]],
                                            mesh->matrix.Matrix);
                                        }
                                        // La droite coupe le plan
                                        if (V3XVector_InPoly(&isect, f->numEdges, roty, &norm))
                                        {
                                            // isect est sur le polygone. Abort
                                            if (who_hide) *who_hide = OVI;
                                            return 0;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 1;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMESH_SetFixedDistance(V3XMESH *mesh, V3XSCALAR z)
*
* Description :
*
*/
void V3XMESH_SetFixedDistance(V3XMESH *mesh, V3XSCALAR z)
{
    int i;
    V3XPOLY *fce = mesh->face;
    mesh->flags|=V3XMESH_NOZSORT;
    for (i=0;i<mesh->numFaces;i++, fce++)  fce->distance = z;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XOVI_DisplayObject(V3XOVI *OVI, int mode)
*
* Description :
*
*/
void V3XOVI_DisplayObject(V3XOVI *OVI, int mode)
{
    V3XOVI **oo = OVI->child;

    if (mode&V3XOVID_HIDE)
    OVI->state|=mode&V3XOVID_FULL?V3XSTATE_HIDDEN:V3XSTATE_HIDDENDISPLAY;
    else
    OVI->state&=~(V3XSTATE_HIDDEN|V3XSTATE_HIDDENDISPLAY);
    if (!oo) return;
    while (*oo!=NULL)
    {
        V3XOVI *o = *oo;
        if (mode)
        o->state|=(mode&V3XOVID_FULL?V3XSTATE_HIDDEN:V3XSTATE_HIDDENDISPLAY);
        else
        o->state&=~(V3XSTATE_HIDDEN|V3XSTATE_HIDDENDISPLAY);
        oo++;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XOVI *V3XOVI_ChildGetByName(V3XOVI *parent, char *name)
*
* DESCRIPTION :
*
*/
V3XOVI *V3XOVI_ChildGetByName(V3XOVI *parent, const char *name)
{
    V3XOVI **oo = parent->child;
    while (*oo!=NULL)
    {
        V3XOVI *o = *oo;
        V3XORI *ORI = o->ORI;
        if (sysStriCmp(ORI->name, name)==0) return o;
        oo++;
    }
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XCL_NewFromGroupMesh(V3XOVI *parent, int mode)
*
* Description :
*
*/
void V3XCL_NewFromGroupMesh(V3XOVI *parent, int mode)
{
    V3XOVI **oo = parent->child;
    V3XORI *pORI = parent->ORI;
    while (*oo!=NULL)
    {
        V3XOVI *o = *oo;
        if (o)
        {
            V3XORI *ORI = o->ORI;
            if (ORI->type==V3XOBJ_MESH)
            {
                ORI->Cs = V3XCL_NewFromMesh(o->mesh, mode);
                if (pORI->Cs)
                {
                    ORI->Cs->ID = pORI->Cs->ID;
                }
            }
            oo++;
        }
    }
    if (!parent->ORI->Cs)
    parent->ORI->Cs = V3XCL_NewFromMesh(parent->mesh, mode);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XScene_Viewport_BuildOVI(V3XSCENE *Scene, V3XOVI *OVI, GXVIEWPORT *ViewPrt)
*
* Description :
*
*/
void V3XScene_Viewport_BuildOVI(V3XSCENE *Scene, V3XOVI *OVI, GXVIEWPORT *ViewPrt)
{
    GXVIEWPORT prevView = GX.View;
    V3XOVI **OVIf = OVI->child;
    if (ViewPrt)  GX.View = *ViewPrt;
    while(*OVIf!=NULL)
    {
        V3XScene_Viewport_BuildOVI(Scene, *OVIf, NULL);
        (**OVIf).state|=V3XSTATE_MATRIXUPDATE;
        OVIf++;
    }
    if (((OVI->state&V3XSTATE_HIDDEN)==0)) V3XScene_ObjectBuild(OVI, 0);
    GX.View = prevView;
}

