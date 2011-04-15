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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "_rlx32.h"
#include "sysresmx.h"
#include "systools.h"
#include "gx_struc.h"
#include "_rlx.h"
#include "gx_csp.h"
#include "gx_tools.h"
#include "gx_rgb.h"
#include "v3xdefs.h"
#include "v3x_1.h"
#include "v3x_2.h"
#include "v3xmaps.h"
#include "v3xrend.h"

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
union _v3x_2dclipInfo
{
    struct  {
        unsigned code0 : 1;
        unsigned code1 : 1;
        unsigned code2 : 1;
        unsigned code3 : 1;
    }
    ocs;
    int outcodes;
};

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C V3XRENDER_Wired(V3XPOLY *fce)
*
* DESCRIPTION :
*
*/
void CALLING_C V3XRENDER_Wired(V3XPOLY *fce)
{
    int32_t *s=(int32_t*)fce->shade;
    V3XMATERIAL *mat = (V3XMATERIAL*)fce->Mat;
    uint32_t cl = mat ? RGB_PixelFormat(mat->diffuse.r, mat->diffuse.g, mat->diffuse.b) : (GX.View.BytePerPixel > 1 ? RGBA_PixelFormat(255, 255, 255, 0) : 255);
    V3XlPTS *pt=(V3XlPTS*)fce->dispTab;
    int i, j=fce->numEdges-1;
	return;

#ifdef CLIPPING
    for (i=0;i<fce->numEdges;i++, s++)
    {
        GX_ClippedLine( pt+i, pt+j, ((V3XMATERIAL*)fce->Mat)->ColorTable[(int32_t)*s]);
        j=i;
    }
#else
    for (i=0;i<fce->numEdges;i++, s++)
    {
        GX.gi.drawAnyLine(pt[i].x, pt[i].y, pt[j].x, pt[j].y, cl);
        j=i;
    }
#endif
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C V3XRENDER_point_shading(V3XPOLY *fce)
*
* DESCRIPTION :
*
*/
void CALLING_C V3XRENDER_Point(V3XPOLY *fce)
{
    int32_t *s=(int32_t*)fce->shade;
    int i;
    V3XlPTS *pt=(V3XlPTS*)fce->dispTab;
    for (i=fce->numEdges;i!=0;pt++, s++, i--)
    {
#ifdef CLIPPING
        if ((pt->x>=GX.View.xmin)
        &&  (pt->y>=GX.View.ymin)
        &&  (pt->x<=GX.View.xmax)
        &&  (pt->y<=GX.View.ymax))
#endif
        //GX.psetPixel(pt->x, pt->y, ((V3XMATERIAL*)fce->Mat)->ColorTable[(int32_t)*s]);
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void Out_SetCodes(union _v3x_2dclipInfo *u, V3XSCALAR x, V3XSCALAR y)
*
* DESCRIPTION :
*
*/
static void Out_SetCodes(union _v3x_2dclipInfo *u, V3XSCALAR x, V3XSCALAR y)
{
    u->outcodes =0;
    u->ocs.code0=(x<(V3XSCALAR)GX.View.xmin);
    u->ocs.code1=(y<(V3XSCALAR)GX.View.ymin);
    u->ocs.code2=(x>(V3XSCALAR)GX.View.xmax);
    u->ocs.code3=(y>(V3XSCALAR)GX.View.ymax);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int V2XVector_Clip( V3XVECTOR2 *a, V3XVECTOR2 *b )
*
* DESCRIPTION :
*
*/
int V2XVector_Clip( V3XVECTOR2 *a, V3XVECTOR2 *b )
{
    union _v3x_2dclipInfo ocu1, ocu2;
    int Inside;
    int Outside;
    Out_SetCodes(&ocu1, a->x, a->y);
    Out_SetCodes(&ocu2, b->x, b->y);
    Inside = ((ocu1.outcodes | ocu2.outcodes)==0);
    Outside = ((ocu1.outcodes & ocu2.outcodes)!=0);
    while(!Outside && !Inside)
    {
        if (ocu1.outcodes==0)
        {
            XCHG(a->x, b->x, V3XSCALAR);
            XCHG(a->y, b->y, V3XSCALAR);
            XCHG(ocu1, ocu2, union _v3x_2dclipInfo);
        }
        if (ocu1.ocs.code0)
        {
            a->y+=(V3XSCALAR)(b->y-a->y)*((V3XSCALAR)GX.View.xmin-a->x)/(b->x-a->x);
            a->x=(V3XSCALAR)GX.View.xmin;
        }
        else if(ocu1.ocs.code1)
        {
            a->x+=(V3XSCALAR)(b->x-a->x)*((V3XSCALAR)GX.View.ymin-a->y)/(b->y-a->y);
            a->y=(V3XSCALAR)GX.View.ymin;
        }
        else if(ocu1.ocs.code2)
        {
            a->y+=(V3XSCALAR)(b->y-a->y)*((V3XSCALAR)GX.View.xmax-a->x)/(b->x-a->x);
            a->x=(V3XSCALAR)GX.View.xmax;
        }
        else if(ocu1.ocs.code3)
        {
            a->x+=(V3XSCALAR)(b->x-a->x)*((V3XSCALAR)GX.View.ymax-a->y)/(b->y-a->y);
            a->y=(V3XSCALAR)GX.View.ymax;
        }
        Out_SetCodes(&ocu1, a->x, a->y);
        Inside =((ocu1.outcodes | ocu2.outcodes)==0);
        Outside=((ocu1.outcodes & ocu2.outcodes)!=0);
    }
    return Inside;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  #define LCLIPVALUE(r, a, b, t) {(r) = (a)+(V3XSCALAR)MULF32((b)-(a), t);}
*
* Description :
*
*/
#define LCLIPVALUE(r, a, b, t) {(r) = (a)+(V3XSCALAR)MULF32((b)-(a), t);}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3XSegment_ClipNear(V3XVECTOR *a)
*
* Description :
*
*/
int V3XSegment_ClipNear(V3XVECTOR *a)
{
    V3XSCALAR t;
    V3XVECTOR C;
    if (a[0].z>=V3X.Clip.Near)
    {
        XCHG(a[0], a[1], V3XVECTOR);
    }
    // a:bon, b: pas bon
    C.z = V3X.Clip.Near;
    t = DIVF32( C.z - a[0].z, a[1].z - a[0].z);
    LCLIPVALUE( C.x, a[0].x, a[1].x, t);
    LCLIPVALUE( C.y, a[0].y, a[1].y, t);
    a[1] = C;
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
int GX_ClipLine(V3XVECTOR *aa, V3XVECTOR *bb, V3XVECTOR *a, V3XVECTOR *b)
{
    int g=0;
    V3XVector_WorldPos(NULL, a, V3X.Buffer.rot_vertex+0);
    if (V3X.Buffer.rot_vertex[0].z<V3X.Clip.Near)
    {
        V3XVector_ProjectWithCenter(V3X.Buffer.prj_vertex[0], V3X.Buffer.rot_vertex[0]);
        g++;
    }
    V3XVector_WorldPos(NULL, b, V3X.Buffer.rot_vertex+1);
    if (V3X.Buffer.rot_vertex[1].z<V3X.Clip.Near)
    {
        V3XVector_ProjectWithCenter(V3X.Buffer.prj_vertex[1], V3X.Buffer.rot_vertex[1]);
        g++;
    }
    if (g==0) return 0;
    else
    if (g==1)
    {
        V3XSegment_ClipNear(V3X.Buffer.rot_vertex);
        V3XVector_ProjectWithCenter(V3X.Buffer.prj_vertex[0], V3X.Buffer.rot_vertex[0]);
        V3XVector_ProjectWithCenter(V3X.Buffer.prj_vertex[1], V3X.Buffer.rot_vertex[1]);
    }
    *aa = *(V3XVECTOR*)V3X.Buffer.prj_vertex;
    *bb = *(V3XVECTOR*)(V3X.Buffer.prj_vertex+1);
    return V2XVector_Clip((V3XVECTOR2*)aa, (V3XVECTOR2*)bb);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int GX_ClippedLine(V3XVECTOR2 *a, V3XVECTOR2 *b, uint32_t cc)
*
* Description :
*
*/
int GX_ClippedLine(V3XVECTOR2 *a, V3XVECTOR2 *b, uint32_t cc)
{
    V3XVECTOR2 A=*a, B=*b;
    int r = V2XVector_Clip(&A, &B);
    if ( r )
	{
		GX.gi.drawAnyLine((int32_t)A.x, (int32_t)A.y, (int32_t)B.x, (int32_t)B.y, cc);
	}
    return r;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void GX_ClippedLine3D(V3XVECTOR *a, V3XVECTOR *b, uint32_t cl)
*
* Description :
*
*/
void GX_ClippedLine3D(V3XVECTOR *a, V3XVECTOR *b, uint32_t cl)
{
    V3XVECTOR d[2];
    if (GX_ClipLine(d+0, d+1, a, b))
    {
        if (!cl) cl = (GX.View.BytePerPixel > 1 ? RGBA_PixelFormat(255, 255, 255, 0) : 255);
        GX.gi.drawAnyLine((int32_t)d[0].x, (int32_t)d[0].y, (int32_t)d[1].x, (int32_t)d[1].y, cl);
        /*
        rgb32_t c[2];
        RGB_GetPixelFormat((rgb24_t*)(c+0), cl); c[0].a = 255;
        c[1] = c[0];
        V3X.Client->DrawPrimitives(d, 0, 0, 2, V3XRCLASS_wired, c);
        */
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C V3XRENDER_Nulll(V3XPOLY *fce)
*
* DESCRIPTION :
*
*/
void CALLING_C V3XRENDER_Null(V3XPOLY *fce)
{
    fce=fce;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C V3XRENDER_SpriteAny(V3XPOLY *fce)
*
* DESCRIPTION :
*
*/
void CALLING_C V3XRENDER_SpriteAny(V3XPOLY *fce)
{
    V3XMATERIAL *Mat = (V3XMATERIAL*)fce->Mat;
    GXSPRITE  *sp = &Mat->texture[0];
    V3XlPTS *pt = (V3XlPTS*)fce->dispTab;

    unsigned bStretch;
	int lx, ly;

	if (V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY)
	{
		lx = pt[2].x - pt[0].x;
		ly = pt[2].y - pt[0].y;
	}
	else
	{
		lx = pt[1].x;
		ly = pt[1].y;
	}

	bStretch = (lx!=(int32_t)sp->LX) || (ly!=(int32_t)sp->LY);

    CSP_Color(RGB_PixelFormat(Mat->diffuse.r, Mat->diffuse.g, Mat->diffuse.b));
	SYS_ASSERT(sp->handle);

    switch(Mat->info.Transparency)
	{
        case V3XBLENDMODE_ADD:
			if (!bStretch)
				GX.csp.TrspADD(pt[0].x, pt[0].y, sp);
			else
				GX.csp.zoom_TrspADD( sp, pt[0].x, pt[0].y, lx, ly);

		break;
        case V3XBLENDMODE_SUB:
			if (!bStretch)
				GX.csp.TrspSUB(pt[0].x, pt[0].y, sp);
			else
				GX.csp.zoom_TrspSUB(  sp, pt[0].x, pt[0].y, lx, ly);

		break;
        case V3XBLENDMODE_ALPHA:
			GX.csp_cfg.alpha = Mat->alpha;
			if (!bStretch)
				GX.csp.TrspALPHA(pt[0].x, pt[0].y, sp);
			else
				GX.csp.zoom_TrspALPHA(  sp, pt[0].x, pt[0].y, lx, ly);

		break;
        default:
			if (!bStretch)
				GX.csp.put(pt[0].x, pt[0].y, sp);
			else
				GX.csp.zoom_put( sp, pt[0].x, pt[0].y, lx, ly);

		break;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMaterial_Register(V3XMESH *obj)
*
* DESCRIPTION :
*
*/
void V3XMaterial_Register(V3XMATERIAL *mat)
{
    V3X_GXTexPrimitives *G=NULL, *Gr=NULL, *Gc=NULL;
    V3X_GXNonTexPrimitives *Gs=NULL;

    int k = mat->info.Shade & 3;
    mat->render_clip = mat->render_near = mat->render_far = V3XRENDER_Wired;
    mat->RenderID = V3XID_LINE;

    if (V3X.Client->primitive)
    {
        Gs = V3X.Client->primitive->std;
        Gr = V3X.Client->primitive->Linear256x256x8b;
        Gc = V3X.Client->primitive->Corrected256x256x8b;
        // Rendu 'Corrige'
        if ((mat->info.Perspective)&&((GX.View.BitsPerPixel!=32)&&(RLX.V3X.Id!=1)))
        {
            G = V3X.Client->primitive->Corrected256x256x8b;
        }
        else
        {
            G = Gr;
            mat->info.Perspective = 0;
        }
        if (mat->shift_size)
        {
            G = V3X.Client->primitive->Linear128x128x8b;
            Gr = G;
            Gc = V3X.Client->primitive->Linear128x128x8b;
        }
    }

    if ((mat->Render == V3XRCLASS_transp_mapping) && (mat->info.Opacity))
    mat->Render = V3XRCLASS_opacity_mapping;
    switch(mat->Render) {
        case V3XRCLASS_wired:
        if (G)
        {
            mat->render_near = V3XRENDER_Wired;
            mat->render_far = V3XRENDER_Wired;
        }
        mat->RenderID = mat->info.Transparency ? V3XID_T_LINE + mat->info.Transparency : V3XID_LINE;
        break;
        case V3XRCLASS_flat:
        case V3XRCLASS_transp_flat:
        if (mat->info.Transparency)
        {
            if (G)
            {
                mat->render_near = Gs->const_trsp;
                mat->render_far = Gs->flat;
            }
            mat->RenderID = V3XID_T_FLAT;
        }
        else
        {
            if (G)
            {
                mat->render_near = Gs->flat;
                mat->render_far = Gs->flat;
            }
            mat->RenderID = V3XID_FLAT;
        }
        break;
        case V3XRCLASS_gouraud:
        case V3XRCLASS_transp:
        if (mat->info.Transparency)
        {
            if (G)
            {
                mat->render_near = Gs->gouraud_trsp;
                mat->render_far = Gs->flat;
            }
            mat->RenderID = V3XID_T_GOURAUD;
        }
        else
        {
            if (G)
            {
                mat->render_near = Gs->gouraud;
                mat->render_far = Gs->flat;
            }
            mat->RenderID = V3XID_GOURAUD;
        }
        break;
        case V3XRCLASS_dualtex_mapping:
        if (G)
        {
            mat->render_near = G->tex_2pass;
            mat->render_far = G->tex;
        }
        mat->RenderID = V3XID_TEXDOUBLE;
        break;
        case V3XRCLASS_bump_mapping:
        case V3XRCLASS_normal_mapping:
        switch(k) {
            case 3:
            if (G)
            {
                mat->render_near = G->gouraud_tex;
                mat->render_far = Gr->flat_tex;
            }
            mat->RenderID = V3XID_TEX_GOURAUD;
            break;
            case 2:
            if (G)
            {
                mat->render_near = G->gouraud_tex;
                mat->render_far = mat->info.Perspective ? Gr->gouraud_tex : Gr->flat_tex;
            }
            mat->RenderID = V3XID_TEX_GOURAUD;
            break;
            case 1:
            if (G)
            {
                mat->render_near = G->flat_tex;
                mat->render_far = Gr->flat_tex;
            }
            mat->RenderID = V3XID_TEX_FLAT;
            break;
            default:
            if (G)
            {
                mat->render_near = G->tex;
                mat->render_far = mat->info.Perspective ? Gr->tex : G->tex_rough; // selon guen
            }
            mat->RenderID = V3XID_TEX;
            break;
        }
        break;
        case V3XRCLASS_opacity_mapping:
        mat->info.Opacity = 1;
        switch(k) {
            case 1:
            if (G)
            {
                mat->render_near= G->flat_opacity_tex;
                mat->render_far = mat->info.Perspective ? Gr->flat_opacity_tex : G->flat_tex;
            }
            mat->RenderID = mat->info.Transparency ? V3XID_T_OPA_TEX_FLAT+mat->info.Transparency : V3XID_OPA_TEX_FLAT;
            break;
            case 3:
            case 2:
            if (G)
            {
                mat->render_near= G->flat_opacity_tex;
                mat->render_far = mat->info.Perspective ? Gr->flat_opacity_tex : G->flat_tex;
            }
            mat->RenderID = mat->info.Transparency ? V3XID_T_OPA_TEX_GOURAUD+mat->info.Transparency : V3XID_OPA_TEX_GOURAUD;
            break;
            default:
            if (G)
            {
                mat->render_near = G->tex_opacity;
                mat->render_far = mat->info.Perspective ? Gr->tex_opacity : G->flat_tex;
            }
            mat->RenderID = mat->info.Transparency ? V3XID_T_OPA_TEX+mat->info.Transparency : V3XID_OPA_TEX;
            break;
        }
        break;
        case V3XRCLASS_transp_mapping:
        if (!mat->info.Transparency)  mat->info.Transparency = V3XBLENDMODE_ALPHA;
        mat->RenderID = V3XID_T_TEX;
        if (mat->info.Shade==1) mat->RenderID = V3XID_T_TEX_FLAT;
        else
        if (mat->info.Shade==2) mat->RenderID = V3XID_T_TEX_GOURAUD;
        mat->RenderID += mat->info.Transparency;
        if (G)
        {
            mat->render_near = mat->info.Transparency==V3XBLENDMODE_ADD ? G->tex_trspAdd : G->tex_trsp;
            if (mat->info.Transparency==V3XBLENDMODE_ADD)
            {
                mat->render_far = mat->info.Perspective ? Gr->tex_trspAdd : G->tex;
            }
            else
            mat->render_far = mat->info.Perspective ? Gr->tex_trsp : G->tex;
        }
        mat->RenderID = V3XID_T_TEX;
        if (mat->info.Shade==1) mat->RenderID = V3XID_T_TEX_FLAT;
        if (mat->info.Shade>=2) mat->RenderID = V3XID_T_TEX_GOURAUD;
        break;
        case V3XRCLASS_bitmap:
        if (G)
        {
            mat->render_near = V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY ? G->tex: V3XRENDER_SpriteAny;
        }
        mat->info.Opacity = 1;
        mat->info.Sprite = V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY ? 1 : 2;
        mat->RenderID = V3XID_SPRITE;
        mat->info.Transparency = V3XBLENDMODE_NONE;
        break;
        case V3XRCLASS_bitmap_transp:
        mat->RenderID = (uint8_t)(V3XID_T_SPRITE + mat->info.Transparency);
        if (G)
        {
			if (V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY)
			{
				mat->render_near = mat->info.Transparency==V3XBLENDMODE_ADD ? G->tex_trspAdd : G->tex_trsp;
			}
			else mat->render_near = V3XRENDER_SpriteAny;
            mat->render_far = mat->render_near;
        }
        mat->info.Sprite = V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY ? 1 : 2;
        if (mat->info.Transparency == V3XBLENDMODE_NONE) mat->info.Transparency = V3XBLENDMODE_ADD;
        break;
        case V3XRCLASS_bitmap_any:
        mat->RenderID = (uint8_t)(V3XID_T_SPRITE+mat->info.Transparency);
        if (G)
        {
			if (V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY)
			{
				mat->render_near = mat->info.Transparency==V3XBLENDMODE_ADD ? G->tex_trspAdd : G->tex_trsp;
			}
			else
            mat->render_near = V3XRENDER_SpriteAny;
            mat->render_far = mat->render_near;
        }
        mat->info.Sprite = V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY ? 1 : 2;
        break;
        // Obsolte class
        case V3XRCLASS_blur_mapping:
        if (G)
        {
            mat->render_near = G->tex_trsp;
            mat->render_far = Gs->flat;
        }
        mat->RenderID = V3XID_T_TEX;
        break;
        case 0:
        default:
        break;
    }
    if (!mat->info.Texturized)
		mat->info.Perspective = 0;
    if (!G)
    {
        mat->lod_near = mat->lod;
        mat->lod_far = mat->lod;
        if (mat->info.Environment)
        {
            mat->info_far.Environment = 0;
        }
        else
        {
            //mat->info_far.Texturized = 0;
        }
        mat->info_far.Perspective = 0;
        mat->info_far.Shade = mat->info.Shade==2 ? 1 : mat->info.Shade;
    }
    else
    {
        mat->render_clip =  mat->render_near;
    }
    mat->RenderID_near = mat->RenderID;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMesh_SetRender(V3XMESH *obj)
*
* DESCRIPTION :
*
*/
void V3XMesh_SetRender(V3XMESH *obj)
{
    int i;
    V3XPOLY *fce;
    V3XMATERIAL *mat=obj->material;
    obj->flags|=V3XMESH_FLATSHADE;
    for (i=obj->numMaterial;i!=0;i--, mat++)
    {
        V3XMaterial_Register(mat);
        if (obj->flags&V3XMESH_NOZSHADING) mat->render_far = mat->render_near;
        if (mat->info.Shade>1) obj->flags&=~V3XMESH_FLATSHADE;
    }
    for (fce=obj->face, i=0;i<obj->numFaces;i++, fce++)
    {
        mat =(V3XMATERIAL*)fce->Mat;
		{
			if (mat->info.Shade)
			{
				if (!fce->shade)
				{
					mat->info.Shade = 0;
					obj->flags&=~V3XMESH_HASDYNLIGHT;
				}
				else obj->flags|=V3XMESH_HASDYNLIGHT;
			}
			if (mat->info.Perspective) if (!fce->uvTab) mat->info.Perspective = 0;
			if (mat->info.Environment) obj->flags|=V3XMESH_HASDYNUV;
		}

    }
    if ((obj->flags&V3XMESH_HASDYNLIGHT)&&(obj->flags&V3XMESH_PRESHADED))
    obj->flags&=~V3XMESH_HASDYNLIGHT;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3XRender_Draw3DSprite(int32_t x, int32_t y, GXSPRITE *sp, int32_t newlx, int32_t newly, int32_t option, int32_t alpha)
*
* DESCRIPTION :
*
*/
static void V3XRender_Draw3DSprite(int32_t x, int32_t y, GXSPRITE *sp, int32_t newlx, int32_t newly, int32_t option, int32_t alpha)
{
    V3XPOLY *fce, *fi;
    V3XVECTOR  p;
    V3XMATERIAL *Mat;
    if (!sp->handle)
		return;

    if (V3X.Buffer.MaxClipped>=V3X.Buffer.MaxClippedFaces )
		return;

    if (V3X.Buffer.MaxMat>=V3X.Buffer.MaxTmpMaterials)
		return;

    Mat = V3XMaterial_QAlloc();
    fce = V3XPoly_QAlloc();

    fce->Mat = Mat;
    sysMemZero(fce->Mat, sizeof(V3XMATERIAL));
    RGB_GetPixelFormat(&Mat->specular, GX.csp_cfg.color); Mat->diffuse = Mat->specular;
    Mat->info.Texturized = 1;
    Mat->info.Opacity = 1;
    Mat->info.Sprite  = 1;
    Mat->info.Transparency = option&2 ? ((V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? V3XBLENDMODE_ADD : V3XBLENDMODE_ALPHA) : 0;
    Mat->alpha  = (uint8_t)alpha;
    Mat->RenderID = (uint8_t)(V3XID_T_SPRITE + Mat->info.Transparency);
    sprintf(Mat->mat_name, "Sp.%dx%d", (int)sp->LX, (int)sp->LY);
    Mat->texture[0]  = *sp;

    p.x = (V3XSCALAR)x;
    p.y = (V3XSCALAR)y;
    p.z = V3X.Clip.Near;
    V3XPoly_SpriteZoom(fce, &Mat->texture[0], &p, (V3XSCALAR)newlx, (V3XSCALAR)newly, 0);
    if (option&1)
    {
        V3X.Buffer.MaxClipped++;
        fi = V3XPoly_XYClipping(fce);
    }
    else fi = fce;
    if ((fi)&&(fi->visible))
    {
        if (V3X.Setup.flags&V3XOPTION_DUPPOLY)
        {
            V3XPoly_QAddPipeline(fi);
            V3X.Buffer.MaxClipped++;
            V3X.Buffer.MaxMat++;
        }
        else
        {
            V3X.Client->RenderPoly(&fi, 1);
        }
    }
}

void CALLING_C V3XCSP_3DSprite(GXSPRITE *sp, int32_t x, int32_t y, int32_t new_lx, int32_t new_ly)
{
    V3XRender_Draw3DSprite(x, y, sp, new_lx, new_ly, !(V3X.Client->Capabilities&GXSPEC_XYCLIPPING), 128);
}

void CALLING_C V3XCSP_3DSprite_Alpha(GXSPRITE *sp, int32_t x, int32_t y, int32_t new_lx, int32_t new_ly)
{
    V3XRender_Draw3DSprite(x, y, sp, new_lx, new_ly, !(V3X.Client->Capabilities&GXSPEC_XYCLIPPING)+2, GX.csp_cfg.alpha);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3X_CSP_Initialize(V3XSPRITEINFO *item, V3XMATERIAL *mat, GXSPRITEFORMAT *spl)
*
* DESCRIPTION :
*
*/
void RLXAPI V3X_CSP_Initialize(GXSPRITE *sp, V3XMATERIAL *mat)
{
    V3XSPRITEINFO   *item = (V3XSPRITEINFO*)sp->handle;
    V3XMATERIAL *mt = &item->mat;
    V3XPOLY     *f = &item->poly;
    // Allocation du polygone
    f->Mat = mt;
    f->numEdges = 4;
    f->visible = 1;
    f->distance = 0;
    f->faceTab = NULL;
	f->uvTab = NULL;
    f->dispTab = V3X_CALLOC(f->numEdges, V3XPTS);
	if ((V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY))
	{
		f->uvTab = V3X_CALLOC(1, V3XUV*);
		f->uvTab[0] = V3X_CALLOC(f->numEdges, V3XUV);
	}
    *mt = *mat;
    sp->LX = mt->texture[0].LX;
    sp->LY = mt->texture[0].LY;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RLXAPI V3X_CSP_Clone(GXSPRITE *sp, GXSPRITE *sps, V3XMATERIAL *mat, unsigned mode)
*
* Description :
*
*/
void RLXAPI V3X_CSP_Clone(GXSPRITE *sp, GXSPRITE *sps, V3XMATERIAL *mat, unsigned mode)
{
    V3XSPRITEINFO   *item = (V3XSPRITEINFO*)sp->handle;
    V3XMATERIAL *mt = &item->mat;
    V3XPOLY     *f = &item->poly;
    V3XSPRITEINFO   *sitem = (V3XSPRITEINFO*)sps->handle;
    V3XPOLY     *sf = &sitem->poly;
    // Allocation du polygone
    f->Mat = mt;
    f->numEdges = 4;
    f->visible = 1;
    f->distance = 0;
    f->faceTab = NULL;
    f->dispTab = V3X_CALLOC(f->numEdges, V3XPTS);
	f->uvTab = sf->uvTab;

    *mt = *mat;
    sp->LX = mt->texture[0].LX;
    sp->LY = mt->texture[0].LY;
    UNUSED(mode);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RLXAPI V3X_CSP_Release(V3XSPRITEINFO *item)
*
* DESCRIPTION :
*
*/
void RLXAPI V3X_CSP_Unload(GXSPRITE *sp)
{
    V3XSPRITEINFO  *item = (V3XSPRITEINFO*)sp->handle;
    V3XPOLY     *f = &item->poly;
    V3XMATERIAL *mt=&item->mat;
    MM_heap.free(f->dispTab);
	if (f->uvTab)
	{
		if (V3XResources_Del(&V3X.Cache, mt->tex_name))
			V3X.Client->TextureFree(&mt->texture[0].handle);
		MM_heap.free(f->uvTab[0]);
		MM_heap.free(f->uvTab);
	}
	else MM_heap.free(sp->data);
    sysMemZero(mt, sizeof(V3XMATERIAL));
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RLXAPI V3X_CSP_GetFn(char *filename, V3XSPRITEINFO *item)
*
* DESCRIPTION :
*
*/
static void V3X_CSP_Default(char *filename, V3XMATERIAL *mat, int load)
{
    sysMemZero(mat, sizeof(V3XMATERIAL));
    RGB_Set(mat->diffuse, 255, 255, 255);
    mat->info.Sprite  = (V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY) ? 1 : 2;
    mat->info.Texturized = (V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY) ? 1 : 0;
    mat->info.Transparency = 0;
    mat->Render = V3XRCLASS_bitmap_any;
    sprintf(mat->mat_name, "*%s*", file_name(filename));
    sysStrCpy(mat->tex_name, file_name(filename));
    mat->info.Shade = 0;
    V3XMaterial_Register( mat );
    if (load)
    {
		V3XMaterial_LoadTextures( mat );
    }
    mat->info.Environment = 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RLXAPI V3X_CSP_GetFn(char *filename, GXSPRITE *sp)
*
* DESCRIPTION :
*
*/
void RLXAPI V3X_CSP_GetFn(char *filename, GXSPRITE *sp, int load)
{
    V3XSPRITEINFO *item = (V3XSPRITEINFO*)sp->handle;
    V3XPOLY *f = &item->poly;
    V3XMATERIAL *mat = &item->mat;
	{
		V3X_CSP_Default(filename, mat, load&1);
		V3X_CSP_Initialize(sp, mat);
		if ((V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY))
		{
			V3XSCALAR u = 1;
			V3XUV   *uv;
			uv = f->uvTab[0];
			uv[0].u = load&2 ? 1.f : 0.f; uv[0].v = load&2 ? 1.f : 0.f;
			uv[1].u = uv[0].u;   uv[1].v =  u;
			uv[2].u = u;     uv[2].v = uv[1].v;
			uv[3].u = uv[2].u;   uv[3].v = uv[0].v;
		}
		else
		{
			sp->data = mat->texture[0].data;
		}
	}
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3X_CSP_Prepare(V3XSPRITEINFO *sp, int flags)
*
* DESCRIPTION :
*
*/
void V3X_CSP_Prepare(GXSPRITE *item, int flags)
{
    V3XSPRITEINFO *sp = (V3XSPRITEINFO*)item->handle;
    int32_t lx, ly;
    if (sp)
    {
        V3XPTS *pos = sp->poly.dispTab;
        if ((flags&0x10)==0)
        {
            lx = (int32_t)item->LX;
            ly = (int32_t)item->LY;
        }
        else
        {
            lx = sp->lx;
            ly = sp->ly;
        }
        if ((flags&0x20)&&(sp->poly.uvTab))
        {
            V3XUV  *uv = sp->poly.uvTab[0];
            uv[1].v = 1;
            uv[2].v = uv[1].v;
            return;
        }
        if (flags&1)
        {
            int lx2 = lx>>1;
            int ly2 = ly>>1;
            int lx1 = lx2 + (lx&1);
            int ly1 = ly2 + (ly&1);
            V3XVector_Set((V3XVECTOR*)&pos[0], sp->x - lx2, sp->y - ly2, sp->z);
            V3XVector_Set((V3XVECTOR*)&pos[1], sp->x - lx2, sp->y + ly1, sp->z);
            V3XVector_Set((V3XVECTOR*)&pos[2], sp->x + lx1, sp->y + ly1, sp->z);
            V3XVector_Set((V3XVECTOR*)&pos[3], sp->x + lx1, sp->y - ly2, sp->z);
        }
        if (flags&2)
        {
            V3XPTS c_ulx, c_vly;
            int lx2 = lx>>1;
            int ly2 = ly>>1;
            c_ulx.x = MULF32(lx2, sp->ROTcos) * V3X.ViewPort.Ratio;
            c_ulx.y = MULF32(ly2, sp->ROTsin);
            c_vly.x = MULF32(lx2, -sp->ROTsin) * V3X.ViewPort.Ratio;
            c_vly.y = MULF32(ly2, sp->ROTcos);
            // Premier point
            pos[0].x = (V3XSCALAR)sp->x + (- c_ulx.x + c_vly.x)/2;
            pos[0].y = (V3XSCALAR)sp->y + (- c_ulx.y + c_vly.y)/2;
            // Autres données par la translation
            pos[1].x = pos[0].x - c_vly.x;
            pos[1].y = pos[0].y - c_vly.y;
            pos[2].x = pos[1].x + c_ulx.x;
            pos[2].y = pos[1].y + c_ulx.y;
            pos[3].x = pos[0].x + c_ulx.x;
            pos[3].y = pos[0].y + c_ulx.y;
            pos[0].z = pos[1].z = pos[2].z = pos[3].z = (V3XSCALAR)sp->z;
        }
        if (flags&4)
        {
            V3XVector_Set((V3XVECTOR*)&pos[0], sp->x       , sp->y   , sp->z);
            V3XVector_Set((V3XVECTOR*)&pos[1], sp->x       , sp->y+ly, sp->z);
            V3XVector_Set((V3XVECTOR*)&pos[2], sp->x+lx, sp->y+ly, sp->z);
            V3XVector_Set((V3XVECTOR*)&pos[3], sp->x+lx, sp->y   , sp->z);
        }
        if (flags&8)
        {
            V3XVector_Set((V3XVECTOR*)&pos[0], sp->x , sp->y, sp->z);
            V3XVector_Set((V3XVECTOR*)&pos[1], sp->x , sp->y2, sp->z);
            V3XVector_Set((V3XVECTOR*)&pos[2], sp->x2, sp->y2, sp->z);
            V3XVector_Set((V3XVECTOR*)&pos[3], sp->x2, sp->y, sp->z);
        }
        // Format to 2D GXSPRITE (size in second).
        if ((V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY)==0)
        {
            V3XVector_Dif((V3XVECTOR*)&pos[1], (V3XVECTOR*)&pos[2], (V3XVECTOR*)&pos[0]);
        }
        sp->poly.distance = (V3XSCALAR)(2*sp->z);
        UNUSED(flags);
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3X_CSP_Set3D(GXSPRITE *item, V3XVECTOR *pos, V3XSCALAR s, unsigned option)
*
* Description :
*
*/
int V3X_CSP_Set3D(GXSPRITE *item, V3XVECTOR *pos, V3XSCALAR s, unsigned option)
{
    int WillDraw = 0;
    V3XSPRITEINFO *sp = (V3XSPRITEINFO*)item->handle;
    V3XVECTOR c;
    V3XVector_WorldPos(NULL, pos, &c);
    V3XVector_ProjectWithCenterAndTest((*sp), c, WillDraw);
    if (!WillDraw)
    {
        V3XSCALAR rap;
        GXSPRITE *bit = &sp->mat.texture[0];
        if (bit->LX!=bit->LY) rap = (float)bit->LY/(float)bit->LX; else rap = 1.f;
        if (option&2)
        {
            V3XSCALAR sx = V3X.ViewPort.Focal * sp->z;
            item->LY = (uint32_t)MULF32(s*rap, sx);
            item->LX = (uint32_t)MULF32(s, sx*V3X.ViewPort.Ratio);
        }
        else
        {
            item->LX = (uint32_t) (s * V3X.ViewPort.Ratio);
            item->LY = (uint32_t) (s * rap);
        }
        sp->z = option&1 ? V3X.Clip.Near : c.z;
        V3X_CSP_Prepare(item, option&4 ? 2 : 1);
    }
    return WillDraw;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C V3X_CSP_pset(int32_t x, int32_t y, GXSPRITE *item)
*
* DESCRIPTION :
*
*/
static void CALLING_C V3X_CSP_pset(int32_t x, int32_t y, GXSPRITE *item)
{
    V3XSPRITEINFO *sp = (V3XSPRITEINFO*)item->handle;
    V3XMATERIAL *mat = (V3XMATERIAL*)sp->poly.Mat;
    V3XPTS *pos = sp->poly.dispTab;
    sp->x =  (float)x;
    sp->y =  (float)y;
    sp->z =  0;//V3X.Clip.Near;
    sp->poly.distance = (2*sp->z);
    RGB_GetPixelFormat(&mat->diffuse, GX.csp_cfg.color);
    mat->alpha = (uint8_t)GX.csp_cfg.alpha;
    mat->specular = mat->diffuse;
    mat->info.Transparency = RLX.V3X.Id>1 ? V3XBLENDMODE_ALPHA : V3XBLENDMODE_ADD;
    V3XVector_Set((V3XVECTOR*)&pos[0], sp->x         , sp->y           , sp->z);
    V3XVector_Set((V3XVECTOR*)&pos[1], sp->x         , sp->y+item->LY  , sp->z);
    V3XVector_Set((V3XVECTOR*)&pos[2], sp->x+item->LX, sp->y+item->LY  , sp->z);
    V3XVector_Set((V3XVECTOR*)&pos[3], sp->x+item->LX, sp->y           , sp->z);
    V3X_CSP_Draw(item, V3XCSPDRAW_INSTANCE|V3XCSPDRAW_INSTMATERIAL);
}

static void CALLING_C V3X_CSP_pset_zoom(GXSPRITE *item, int32_t x, int32_t y, int32_t lx, int32_t ly)
{
    V3XSPRITEINFO *sp = (V3XSPRITEINFO*)item->handle;
    V3XMATERIAL *mat = (V3XMATERIAL*)sp->poly.Mat;
    V3XPTS *pos = sp->poly.dispTab;
    sp->x =  (float)x;
    sp->y =  (float)y;
    sp->z =  0;//V3X.Clip.Near;
    sp->poly.distance = (2*sp->z);
    RGB_GetPixelFormat(&mat->diffuse, GX.csp_cfg.color);
    mat->alpha = (uint8_t)GX.csp_cfg.alpha;
    mat->specular = mat->diffuse;
    mat->info.Transparency = RLX.V3X.Id>1 ? V3XBLENDMODE_ALPHA : V3XBLENDMODE_ADD;
    V3XVector_Set((V3XVECTOR*)&pos[0], sp->x   , sp->y     , sp->z);
    V3XVector_Set((V3XVECTOR*)&pos[1], sp->x   , sp->y+ly  , sp->z);
    V3XVector_Set((V3XVECTOR*)&pos[2], sp->x+lx, sp->y+ly  , sp->z);
    V3XVector_Set((V3XVECTOR*)&pos[3], sp->x+lx, sp->y      , sp->z);
    V3X_CSP_Draw(item, V3XCSPDRAW_INSTANCE|V3XCSPDRAW_INSTMATERIAL);
}

CSP_FUNCTION V3X_CSPset={V3X_CSP_pset};
CSP_FUNCTION V3X_CSPset_zoom={(CSP_STDFUNCTION)V3X_CSP_pset_zoom};

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3X_CSP_Draw(V3XSPRITEINFO *sp, int clip)
*
* DESCRIPTION :
*
*/
int V3X_CSP_Draw(GXSPRITE *item, int clip)
{
    V3XSPRITEINFO *sp = (V3XSPRITEINFO*)item->handle;
    V3XPOLY *fce = &sp->poly;
    V3XPOLY *fi = fce;
    if (!item->LX) return 0;
    fi->visible = 1;
    if (V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY)
    {
        fi = (clip&V3XCSPDRAW_CLIP)
        &&(!(V3X.Client->Capabilities&GXSPEC_XYCLIPPING))
        ? V3XPoly_XYClipping(fce) : fce;
    }
    if ((fi)&&(!fi->visible)) return 0;
    if ((clip&V3XCSPDRAW_INSTANCE)&&(fi==fce)) fi = V3XPoly_Duplicate(fi);
    if (fi)
    {
        if (fi->visible)
        {
            if (clip&V3XCSPDRAW_INSTMATERIAL)
            {
                if (V3X.Buffer.MaxMat < V3X.Buffer.MaxTmpMaterials)
                {
                    V3XMATERIAL *Mat = V3XMaterial_QAlloc();
                    *Mat = *(V3XMATERIAL*)fi->Mat;
                    fi->Mat = Mat;
                    V3X.Buffer.MaxMat++;
                }
            }
            if (V3X.Buffer.MaxFaces<V3X.Buffer.MaxFacesDisplay)
            V3X.Buffer.RenderedFaces[V3X.Buffer.MaxFaces++] = fi;
        }
    }else return 0;
    return fi->visible;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void v3x_SpriteBuildLUT(GXSPRITE *sp)
*
* DESCRIPTION :  Chargement de GXSPRITE 2D en memoire 3D
*
*/
static void v3x_SpriteBuildLUT(GXSPRITE *sp)
{
    int i;
    rgb24_t *pal = GX.ColorTable;
    uint32_t *c;
    sp->handle = (uint32_t*)MM_heap.malloc(4*256);
    for (i=256, c=(uint32_t*)sp->handle;i!=0;c++, pal++, i--)
    *c = RGB_PixelFormat(pal->r, pal->g, pal->b);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3X_CSP_Upload(GXSPRITE *sp, int bpp)
*
* DESCRIPTION :
*
*/
void V3X_CSP_Upload(GXSPRITE *sp, int bpp)
{
	SYS_ASSERT(sp->data);
    if (V3X.Client->Capabilities&GXSPEC_HARDWARE)
    {
		sp->handle = V3X.Client->TextureDownload(sp, GX.ColorTable, bpp, 0);
        if (!sp->handle)
			V3X.Setup.warnings|=V3XWARN_NOENOUGHSurfaces;
    }
    else
    if (GX.View.BytePerPixel>1)
    {
        v3x_SpriteBuildLUT(sp);
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3X_CSP_Unload(GXSPRITE *sp)
*
* DESCRIPTION :
*
*/
void V3X_CSP_Release(GXSPRITE *sp)
{
	if (sp->handle)
	{
		V3X.Client->TextureFree(sp->handle);
		sp->handle = 0;
	}
}

