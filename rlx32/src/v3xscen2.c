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
#include <assert.h>
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
#include "v3xtrig.h"
#include "v3xcoll.h"
#include "v3xmaps.h"
#include "v3xrend.h"
#include "v3xscene.h"

#include "v3x_old.h"

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3XScene_Type_Counts(V3XSCENE *pScene, int type)
*
* DESCRIPTION :
*
*/
int V3XScene_Type_Counts(V3XSCENE *pScene, int type)
{
    int i, n=0;
    V3XOVI *OVI = pScene->OVI;
    for (i=0;i<pScene->numOVI;i++, OVI++)
    {
        V3XORI *ORI = OVI->ORI;
        if (ORI) if (ORI->type == type) n++;
    }
    return n;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XOVI *V3XScene_Type_GetByIndex(V3XSCENE *pScene, int index_OVI, int type)
*
* DESCRIPTION :
*
*/
V3XOVI *V3XScene_Type_GetByIndex(V3XSCENE *pScene, int index_OVI, int type)
{
    int i, j=0;
    V3XOVI *OVI = pScene->OVI;
    for (i=0;i<pScene->numOVI;i++, OVI++)
    {
        if (OVI->ORI->type == type)
        {
            if (j==index_OVI) return OVI;
            j++;
        }
    }
    return NULL;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3x_Create_ORI(V3XORI *ORI, int i)
*
* DESCRIPTION :
*
*/
void RLXAPI static V3x_Create_ORI(V3XORI *ORI, int i)
{
    sysMemZero(ORI, sizeof(V3XORI));
    sprintf(ORI->name, "object.%d", i);
    ORI->type = V3XOBJ_NONE;
    ORI->node = NULL;
    ORI->sub_class = 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3x_Create_OVI(V3XOVI *OVI, int i)
*
* DESCRIPTION :
*
*/
void RLXAPI static V3x_Create_OVI(V3XOVI *OVI, int al)
{
    sysMemZero(OVI, sizeof(V3XOVI));
    if (al)
    {
        OVI->node = (V3XNODE*)MM_heap.malloc(sizeof(V3XNODE));
        OVI->Tk = &OVI->node->Tk;
        OVI->Tk->vinfo.target.x = CST_ONE;
        OVI->Tk->vinfo.target.y = CST_ONE;
        OVI->Tk->vinfo.target.z = CST_ONE;
    }
    OVI->state|=V3XSTATE_VALIDPOINTER;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3x_Create_TRI(V3XTRI     *TRI, int i)
*
* DESCRIPTION :
*
*/
void RLXAPI static V3x_Create_TRI(V3XTRI *TRI, int i)
{
    sysMemZero(TRI, sizeof(V3XTRI));
    TRI->flags |= V3XKF_VALID;
    UNUSED(i);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3x_Create_TVI(V3XTVI  *TVI, int i)
*
* DESCRIPTION :
*
*/
void RLXAPI static V3x_Create_TVI(V3XTVI  *TVI, int i)
{
    sysMemZero(TVI, sizeof(V3XTVI));
    TVI->flags|=1;
    UNUSED(i);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
V3XORI RLXAPI *V3XScene_NewORI(V3XSCENE *pScene)
{
    V3XORI *ORI = pScene->ORI;
    int p;
    while ((ORI->type!=V3XOBJ_NONE)&&(ORI->type!=0)) ORI++;
    p = ORI - pScene->ORI + 1; if (p>pScene->numORI) pScene->numORI = (uint16_t)p;
    V3x_Create_ORI(ORI, ORI-pScene->ORI);
    return ORI;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int RLXAPI V3XScene_ReleaseObject(V3XSCENE *pScene, V3XOVI *OVI, uint32_t flags)
*
* Description :
*
*/
int RLXAPI V3XScene_ReleaseObject(V3XSCENE *pScene, V3XOVI *OVI, uint32_t flags)
{
    V3XORI *ORI = OVI->ORI;
    ORI->type = V3XOBJ_NONE;
    OVI->parent = NULL;
    OVI->target = NULL;
    if (flags&1)
    {
        ORI->mesh = NULL;
        OVI->mesh = NULL;
    }
    UNUSED(pScene);
    return 1;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XScene_NewOVI(V3XSCENE *pScene)
*
* DESCRIPTION :
*
*/
V3XOVI RLXAPI *V3XScene_NewOVI(V3XSCENE *pScene)
{
    V3XOVI *OVI = pScene->OVI;
    int p;
    while (OVI->ORI!=NULL)
    {
        OVI++;
    }
    p = OVI - pScene->OVI + 1;
	if (p>pScene->numOVI) pScene->numOVI = (uint16_t)p;
    V3x_Create_OVI(OVI, TRUE);
    return OVI;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
V3XTRI * RLXAPI V3XScene_NewTRI(V3XSCENE *pScene)
{
    V3XTRI *TRI = pScene->TRI;
    int p;
    while (TRI->flags&V3XKF_VALID) TRI++;
    p = TRI - pScene->TRI + 1; if (p>pScene->numTRI) pScene->numTRI = (uint16_t)p;
    V3x_Create_TRI(TRI, TRI-pScene->TRI);
    return TRI;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XScene_NewTVI(V3XSCENE *pScene)
*
* DESCRIPTION :
*
*/
V3XTVI * RLXAPI V3XScene_NewTVI(V3XSCENE *pScene)
{
    V3XTVI *TVI = pScene->TVI;
    int p;
    while (TVI->flags&1) TVI++;
    p = TVI - pScene->TVI + 1; if (p>pScene->numTVI) pScene->numTVI = (uint16_t)p;
    V3x_Create_TVI(TVI, TVI-pScene->TVI);
    return TVI;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XOVI *V3XScene_NewObject(V3XSCENE *pScene, uint32_t flags)
*
* DESCRIPTION :
*
*/
V3XOVI *V3XScene_NewObject(V3XSCENE *pScene, uint32_t flags)
{
    V3XORI    *ORI;
    V3XOVI *OVI;
    V3XTRI     *TRI;
    V3XTVI  *TVI;
    OVI = V3XScene_NewOVI(pScene);
    OVI->TVI = NULL;
    if ((flags&4)==0)
    {
        ORI = V3XScene_NewORI(pScene);
        OVI->ORI = ORI;
        if (flags&2) ORI->node = (V3XNODE* )MM_heap.malloc(sizeof(V3XNODE ));
    }
    if (flags&1)
    {
        TRI = V3XScene_NewTRI(pScene);
        TVI = V3XScene_NewTVI(pScene);
        TVI->TRI = TRI;
        OVI->TVI = TVI;
    }
    OVI->state|=V3XSTATE_VALIDPOINTER;
    return OVI;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XScene_CloneOVI(V3XSCENE *pScene, V3XOVI *OVI, int fois)
*
* DESCRIPTION :
*
*/
void RLXAPI V3XScene_CloneOVI(V3XSCENE *pScene, V3XOVI *OVI, int fois)
{
    int i, k = pScene->numOVI;
    V3XOVI *OV2 = pScene->OVI + k;
    unsigned klone = OVI->index_ORI;
    if (klone>65535L) klone = OVI->ORI - pScene->ORI;
    for (i=fois;i!=0;i--, k++, OV2++)
    {
        V3x_Create_OVI(OV2, TRUE);
        OV2->TVI = pScene->TVI + k;
        OV2->ORI = pScene->ORI + klone;
    }
    pScene->numOVI = (uint16_t)k;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XOVI RLXAPI *V3XScene_OVI_GetByName(V3XSCENE *pScene, char *name)
*
* DESCRIPTION :
*
*/
V3XOVI RLXAPI *V3XScene_OVI_GetByName(V3XSCENE *pScene, const char *name)
{
    int i, f=0;
    V3XOVI *OVI, *OVIf=NULL;
    for ( i=pScene->numOVI, OVI=pScene->OVI; (i!=0)&&(f!=1); OVI++, i--)
    {
        V3XORI *ORI = (OVI->index_ORI<0xffff) ? pScene->ORI + OVI->index_ORI : OVI->ORI;
        if (ORI)
        {
            if (sysStriCmp(ORI->name, name)==0)
            {
                f = 1;
                OVIf = OVI;
            }
        }
    }
    return OVIf;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XORI RLXAPI *V3XScene_ORI_GetByName(V3XSCENE *pScene, char *name)
*
* DESCRIPTION :
*
*/
V3XORI RLXAPI *V3XScene_ORI_GetByName(V3XSCENE *pScene, const char *name)
{
    int i, f=-1;
    V3XORI *ORI;
    for (i=0, ORI=pScene->ORI;(i<pScene->numORI)&&(f==0); ORI++, i++)
    {
        if (sysStriCmp(ORI->name, name)==0) f = i;
    }
    return f>=0 ? pScene->ORI + f : NULL;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RLXAPI V3XScene_Camera_Select(V3XOVI *OVI)
*
* DESCRIPTION :
*
*/
void RLXAPI V3XScene_Camera_Select(V3XOVI *OVI)
{
    if (OVI==NULL) return;

    V3X.Camera.matrix_Method = V3XMATRIX_Vector;

    V3X.Camera.Tk = OVI->mesh->Tk;
    V3XMatrix_MeshTransform(&V3X.Camera);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XOVI RLXAPI *V3XScene_Camera_GetByName(V3XSCENE *pScene, char *name)
*
* DESCRIPTION :
*
*/
V3XOVI RLXAPI *V3XScene_Camera_GetByName(V3XSCENE *pScene, const char *name)
{
    int i;
    V3XOVI *OVI, *OVIf=NULL;
    for (i=pScene->numOVI, OVI=pScene->OVI;i!=0;OVI++, i--)
    {
        if ((OVI->ORI)&&(OVI->ORI->type==V3XOBJ_CAMERA))
        {
            if (sysStriCmp(OVI->ORI->name, name)==0)
            {
                OVI->state&=~V3XSTATE_HIDDEN;
                OVIf = OVI;
            }
            else
            OVI->state|=V3XSTATE_HIDDEN;
        }
    }
    return OVIf;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RLXAPI V3XScene_CRC_Check(V3XSCENE* pScene)
*
* Description :
*
*/
void RLXAPI V3XScene_CRC_Check(V3XSCENE* pScene)
{
    V3XOVI *OVI;
    V3XORI *nORI, *cORI;
    int i;
    for (OVI=pScene->OVI, i=0;i<pScene->numOVI;i++, OVI++)
    {
        if ((i)&&(OVI->ORI == pScene->ORI)) OVI->ORI = NULL;
        nORI = OVI->ORI;
        if (nORI)
        if (nORI->type!=V3XOBJ_NONE)
        {
            if (OVI->index_INSTANCE>0)
            {
                cORI = pScene->ORI + OVI->index_INSTANCE;
				if (cORI!=nORI)
				{
					nORI->mesh = (V3XMESH*) MM_heap.malloc(sizeof(V3XNODE));
					if (nORI->type == V3XOBJ_MESH)
					{
						if (cORI->mesh)
	                    V3XMesh_Duplicate(nORI->mesh, cORI->mesh);
						nORI->flags|=V3XORI_DUPLICATED;
		            }
					else
					{
	                    *nORI->node = *cORI->node;
						nORI->flags|=V3XORI_DUPLICATED;
					}
				}
            }
            // Calcul Matrice
            switch( nORI->type) {
                case V3XOBJ_LIGHT:
				{
					nORI->light->Tk = *OVI->Tk;
					nORI->light->pos = OVI->Tk->vinfo.pos;
					*OVI->light = *nORI->light;
				}
                break;
                case V3XOBJ_MESH:
                {
                    V3XMATRIX pMat = OVI->mesh->matrix;
                    V3XKEY Tk = OVI->mesh->Tk;
                    if ((OVI->state&V3XSTATE_INSTANCED)&&(!OVI->index_INSTANCE))
                    {
                        V3XMesh_Duplicate(OVI->mesh, nORI->mesh);
                    }
                    else
					{
						if (nORI->mesh)
							*OVI->mesh = *nORI->mesh;
					}
                    if ((OVI->state&V3XSTATE_INSTANCED)==0)
                    {
                        OVI->mesh->matrix = pMat;
                        OVI->mesh->Tk = Tk;
                    }
                }
                break;
            }
        }
    }
    if (!pScene->Layer.lt.palette.lut) pScene->Layer.lt.palette.lut = (rgb24_t*) MM_heap.malloc(768);
}

/* 1 */
void RLXAPI V3XOVI_BuildChildren(V3XOVI *OVI, V3XSCENE *pScene)
{
    int i, n=0, j=0;
    for (i=0;i<pScene->numOVI;i++)
    {
        if (pScene->OVI[i].parent == OVI) n++;
    }
    if (OVI->child) MM_heap.free(OVI->child);
    OVI->child = V3X_CALLOC(n+1, V3XOVI*);
    for (i=0;i<pScene->numOVI;i++)
    {
        if (pScene->OVI[i].parent == OVI)
        {
            OVI->child[j] = pScene->OVI + i;
            j++;
        }
    }
    OVI->child[n]=NULL;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RLXAPI V3XScene_Validate(V3XSCENE* pScene)
*
* DESCRIPTION :
*
*/
void RLXAPI V3XScene_Validate(V3XSCENE* pScene)
{
    V3XTVI *TVI;
    V3XTRI *TRI;
    V3XOVI *OVI;
    unsigned i;
    for (OVI=pScene->OVI, i=0;i<pScene->numOVI;i++, OVI++)
    {
        if ((OVI->state&V3XSTATE_VALIDPOINTER)==0)
        {
			SYS_ASSERT(OVI->index_ORI>=0 && OVI->index_ORI<pScene->numORI);
            OVI->ORI = pScene->ORI + OVI->index_ORI;

			if (OVI->index_TVI)
            {
				SYS_ASSERT(OVI->index_TVI>=0 && OVI->index_TVI<=pScene->numTVI);
                TVI = pScene->TVI + OVI->index_TVI;
                if (TVI->index_TRI < pScene->numTRI)
                {
                    TRI = pScene->TRI + TVI->index_TRI;

					if (0!=TRI->index_NEXT)
						TRI->next = pScene->TRI + TRI->index_NEXT;

					if (0!=TRI->index_CHAIN)
						TRI->chain = pScene->TRI + TRI->index_CHAIN;

					TVI->TRI = TRI;
                    OVI->TVI = TVI;
                }
                else
                {
                    OVI->TVI = NULL;
                }
            }
            else
            {
                OVI->TVI = NULL;
            }
            if (0!=OVI->index_NEXT)
			{
				SYS_ASSERT(OVI->index_NEXT>=0 && OVI->index_NEXT<pScene->numOVI);
				OVI->next = pScene->OVI + OVI->index_NEXT;
			}
		}
        OVI->Tk = &OVI->mesh->Tk;

    }
    for (OVI=pScene->OVI, i=0;i<pScene->numOVI;i++, OVI++)
    {
        if ((OVI->state&V3XSTATE_VALIDPOINTER)==0)
        {
            unsigned p;
            p = OVI->index_PARENT;
            if ((p>0)&&(p<0xffff)&&(p!=i))
            {
                OVI->parent = pScene->OVI + p;
            }
            else OVI->parent = NULL;
            p = OVI->index_TARGET;
            if ((p>0)&&(p<0xffff)&&(p!=i))
            {
                OVI->target = pScene->OVI + p;
            }
            else OVI->target = NULL;
        }
    }
    for (OVI=pScene->OVI, i=0;i<pScene->numOVI;i++, OVI++)
    {
        if (!OVI->child) V3XOVI_BuildChildren(OVI, pScene);
        OVI->state|=V3XSTATE_VALIDPOINTER;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void Destroy_ORI(V3XSCENE *pScene, int i)
*
* DESCRIPTION :
*
*/
void RLXAPI static v3x_freeMorph3D(V3XTWEEN *mo)
{
    unsigned int i;
    for (i=0;i<mo->numFrames;i++) MM_heap.free(mo->frame[i].vertex);
    MM_heap.free(mo->frame);
    MM_heap.free(mo);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XLight_Release(V3XLIGHT *light)
*
* Description :
*
*/
void V3XLight_Release(V3XLIGHT *light)
{
    if ((light->material)&&(light->flags&V3XLIGHTCAPS_LENZFLARE))
    {
        V3XMaterial_Release(light->material, NULL);
        MM_heap.free(light->material);
    }
    sysMemZero(light,sizeof(V3XLIGHT));
    MM_heap.free(light);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void v3x_Destroy_ORI(V3XSCENE *pScene, int i)
*
* DESCRIPTION :
*
*/
void RLXAPI static v3x_Destroy_ORI(V3XSCENE *pScene, int i)
{
    V3XORI *ORI = pScene->ORI + i;
    switch(ORI->type) {
        case V3XOBJ_LIGHT:
        if (ORI->light)
		{
			if ((ORI->flags&V3XORI_DUPLICATED)==0)
			{
				V3XLight_Release(ORI->light);
			}
			else
			{
				MM_heap.free(ORI->light);
			}
		}
        break;
        case V3XOBJ_VIEWPORT:
        case V3XOBJ_DUMMY:
        MM_heap.free(ORI->mesh);
        break;
        case V3XOBJ_MESH:
        if (ORI->mesh)
        {

            if ((ORI->flags&V3XORI_DUPLICATED)==0)
            {
                V3XMesh_Release(ORI->mesh);
            }
			else
			{
				V3XMesh_ReleaseDup(ORI->mesh);
			}
        }
        break;
    }
    ORI->node = NULL;
    if ((ORI->flags & V3XORI_CSDUPLICATED)==0)
    {
        if (ORI->Cs) V3XCL_Release(ORI->Cs);
    }
    if (ORI->morph) v3x_freeMorph3D(ORI->morph);
    V3x_Create_ORI(ORI, i);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void v3x_Destroy_TRI(V3XSCENE *pScene, int i)
*
* DESCRIPTION :
*
*/
void RLXAPI static v3x_Destroy_TRI(V3XSCENE *pScene, int i)
{
    V3XTRI *TRI = pScene->TRI + i;
    MM_heap.free(TRI->keys);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void v3x_Destroy_OVI(V3XSCENE *pScene, int mp)
*
* DESCRIPTION :
*
*/
void RLXAPI static v3x_Destroy_OVI(V3XSCENE *pScene, int mp)
{
    V3XOVI *OVI = pScene->OVI + mp;
    if (OVI->index_INSTANCE)
    {
		if (OVI->ORI)
        if ((OVI->ORI->type == V3XOBJ_MESH)&&(OVI->mesh)&&(OVI->ORI->mesh))
        {
            if (OVI->mesh->face!=OVI->ORI->mesh->face)
            {
                MM_heap.free(OVI->mesh->face);
                OVI->mesh->face = NULL;
            }
        }
    }
    if (OVI->child) MM_heap.free(OVI->child);
    if (OVI->mesh)
    if (OVI->mesh!=OVI->ORI->mesh) MM_heap.free(OVI->mesh);
    V3x_Create_OVI(OVI, FALSE);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void v3x_Destroy_TVI(V3XSCENE *pScene, int i)
*
* DESCRIPTION :
*
*/
static void RLXAPI v3x_Destroy_TVI(V3XSCENE *pScene, int i)
{
    pScene = pScene;
    i = i;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : V3XSCENE *load_V3XSCENE(char *mode, FlushClass *f);
*
* DESCRIPTION :
*
*/
static void v3xtx_load(V3XLAYER_CLITEM *item)
{
    if (item->filename[0])
    {
		char tex[256];
        sprintf(tex, "%s.png", item->filename);
        item->table = REALCOLOR_LoadFn(tex);
        REALCOLOR_Simply(item->table);
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void v3xtx_free(V3XLAYER_CLITEM *item)
*
* DESCRIPTION :
*
*/
static void v3xtx_free(V3XLAYER_CLITEM *item)
{
    if (item->table)
    {
        REALCOLOR_Free(item->table);
        item->table = NULL;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RLXAPI V3XScene_Release(V3XSCENE *pScene)
*
* DESCRIPTION :
*
*/
void RLXAPI V3XScene_Release(V3XSCENE *pScene)
{
    int i;
    V3XLAYER *layer = &pScene->Layer;
    if (pScene==NULL) return;
    for (i=0;i<pScene->numOVI;i++) v3x_Destroy_OVI(pScene, i);
    for (i=0;i<pScene->numTVI;i++) v3x_Destroy_TVI(pScene, i);
    for (i=0;i<pScene->numORI;i++)
    {
        v3x_Destroy_ORI(pScene, i);
    }
    for (i=0;i<pScene->numTRI;i++) v3x_Destroy_TRI(pScene, i);
    MM_heap.free(pScene->ORI);
    MM_heap.free(pScene->OVI);
    MM_heap.free(pScene->TRI);
    MM_heap.free(pScene->TVI);
    if (GX.View.BytePerPixel==1)
    {
        v3xtx_free(&layer->lt.alpha50);
        v3xtx_free(&layer->lt.additive);
        v3xtx_free(&layer->lt.blur);
        if (layer->lt.gouraud.table)
        {
            REALCOLOR_Free(layer->lt.gouraud.table);
            MM_heap.free(layer->lt.phong.table);
            layer->lt.gouraud.table = NULL;
        }
    }
    if (layer->bg.bitmap.handle) V3X_CSP_Unload(&layer->bg.bitmap);
    if (layer->lt.palette.table)
    MM_heap.free(layer->lt.palette.table);
    MM_heap.free(pScene);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : V3XSCENE* init_V3XSCENE(void)
*
* DESCRIPTION :
*
*/
V3XSCENE *V3XScene_New(int option)
{
    V3XSCENE *pScene;
    V3XTVI  *TVI;
    V3XTRI     *TRI;
    V3XOVI *OVI;
    V3XORI    *ORI;
    unsigned int i;
    pScene = (V3XSCENE*) MM_heap.malloc(sizeof(V3XSCENE));
    pScene->numOVI = (uint16_t)V3X.Setup.MaxStartObjet;
    pScene->numTVI = (uint16_t)V3X.Setup.MaxStartObjet;
    pScene->numTRI = (uint16_t)V3X.Setup.MaxStartObjet;
    pScene->numORI = (uint16_t)V3X.Setup.MaxStartObjet;
    if (V3X.Setup.MaxExtentableObjet<V3X.Setup.MaxStartObjet)
    V3X.Setup.MaxExtentableObjet=V3X.Setup.MaxStartObjet*2;
    pScene->ORI = (V3XORI*) MM_heap.malloc(V3X.Setup.MaxExtentableObjet*sizeof(V3XORI));
    pScene->OVI = (V3XOVI*) MM_heap.malloc(V3X.Setup.MaxExtentableObjet*sizeof(V3XOVI));
    pScene->TRI = (V3XTRI*) MM_heap.malloc(V3X.Setup.MaxExtentableObjet*sizeof(V3XTRI));
    pScene->TVI = (V3XTVI*) MM_heap.malloc(V3X.Setup.MaxExtentableObjet*sizeof(V3XTVI));
    for (ORI=pScene->ORI, i=0;i<V3X.Setup.MaxExtentableObjet;i++, ORI++)
    {
        switch(i) {
            case 0: if (option&V3XSCENE_NEWVIEWPORT)
            {
                sysMemZero(ORI, sizeof(V3XORI));
                sprintf(ORI->name, "viewport");
                OVI = V3XScene_NewOVI(pScene);
                OVI->ORI = ORI;
                ORI->type = V3XOBJ_VIEWPORT;
                ORI->mesh = (V3XMESH*) MM_heap.malloc(sizeof(V3XNODE));
                OVI->TVI = V3XScene_NewTVI(pScene);
                OVI->TVI->TRI = V3XScene_NewTRI(pScene);
            }
            else if (option&V3XSCENE_NEWNODES) V3x_Create_ORI(ORI, i);
            break;
            case 1: if (option&V3XSCENE_NEWCAMERA)
            {
                sysMemZero(ORI, sizeof(V3XORI));
                sprintf(ORI->name, "camera");
                OVI = V3XScene_NewOVI(pScene);
                OVI->ORI = ORI;
                ORI->type = V3XOBJ_CAMERA;
                ORI->mesh = (V3XMESH*) MM_heap.malloc(sizeof(V3XMESH));
                OVI->TVI = V3XScene_NewTVI(pScene);
                OVI->TVI->TRI = V3XScene_NewTRI(pScene);
            }
            else if (option&V3XSCENE_NEWNODES) V3x_Create_ORI(ORI, i);
            break;
            case 2:
				if (option&V3XSCENE_NEWLIGHT)
            {
                sysMemZero(ORI, sizeof(V3XORI));
                sprintf(ORI->name, "light");
                OVI = V3XScene_NewOVI(pScene);
                OVI->ORI = ORI;
                ORI->type = V3XOBJ_LIGHT;
                ORI->light = (V3XLIGHT*) MM_heap.malloc(sizeof(V3XNODE));
                ORI->light->type = V3XLIGHTTYPE_DIRECTIONAL;
                ORI->light->intensity = 255.f;
                RGB_Set(ORI->light->color, 180, 180, 180);
                *OVI->node = *ORI->node;
                OVI->TVI = V3XScene_NewTVI(pScene);
                OVI->TVI->TRI = V3XScene_NewTRI(pScene);
            }
            else
            if (option&V3XSCENE_NEWNODES)
            {
                V3x_Create_ORI(ORI, i);
            }
            break;
            default:
            if (option&V3XSCENE_NEWOBJECTS) V3x_Create_ORI(ORI, i);
            else ORI->type = V3XOBJ_NONE;
            break;
        }
    }
    if (option&V3XSCENE_NEWKF)
    for (TVI=pScene->TVI, TRI=pScene->TRI, i=0;i<V3X.Setup.MaxExtentableObjet;i++, TRI++, TVI++)
    {
        V3x_Create_TVI(TVI, i);
        V3x_Create_TRI(TRI, i);
        TVI->TRI = TRI;
    }
    for (OVI=pScene->OVI, i=0;i<V3X.Setup.MaxExtentableObjet;i++, OVI++)
    {
        if (option&V3XSCENE_NEWOBJECTS)
        {
            if (!OVI->ORI)
            {
                V3x_Create_OVI(OVI, TRUE);
                if (option&V3XSCENE_NEWKF)
                OVI->TVI = pScene->TVI + i;
                OVI->ORI = pScene->ORI + i;
            }
        }
        OVI->state|=V3XSTATE_VALIDPOINTER;
    }
    if (option&V3XSCENE_NEWNODES)
    V3XScene_Validate(pScene);
    sysStrCpy(pScene->Layer.bg.filename, ".");
    pScene->Layer.bg.flags= V3XBG_BLACK;
    return pScene;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : static void V3x_GetRenderBackground(V3XSCENE *pScene, char *res)
*
* DESCRIPTION :
*
*/
static void RLXAPI V3x_GetRenderBackground(V3XSCENE *pScene)
{
    V3XLAYER *layer = &pScene->Layer;
    if (FIO_cur->exists(layer->bg.filename))
    {
        layer->bg.bitmap.handle = &layer->bg._bitmap;
        V3X_CSP_GetFn(layer->bg.filename, &layer->bg.bitmap, 1);
    }
    else
    {
        layer->bg.bitmap.handle = NULL;
        switch(layer->bg.flags&(0x10-1)) {
            case V3XBG_IMG:
            layer->bg.flags&=~V3XBG_IMG;
            layer->bg.flags|= V3XBG_COLOR;
            break;
        }
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XScene_LoadTextures(V3XSCENE *pScene, void (*callback)(void *))
*
* DESCRIPTION :
*
*/
void V3XScene_LoadTextures(V3XSCENE *pScene, void (*callback)(void *))
{
    int i, x, m=MM_heap.active;
    V3XLAYER *layer = &pScene->Layer;
    V3XORI    *ORI;
    V3X.Setup.warnings &=~ V3XWARN_NOENOUGHSurfaces;
    /*
    *  Palette
    */
    if (FIO_cur->exists(layer->lt.palette.filename))
    {
        ACT_LoadFn(layer->lt.palette.lut, layer->lt.palette.filename);
        GX.ColorClut = layer->lt.palette.lut;
    }
    else
    {
        GX.ColorClut = GX.ColorTable;
    }
    /*
    BackGround
    */
    switch(layer->bg.flags&15) {
        case V3XBG_BLACK:
        case V3XBG_NONE:
        break;
        case V3XBG_COLOR:
        layer->bg.index_color = (GX.View.BytePerPixel==1)
        ? RGB_findNearestColor((rgb24_t*)&layer->bg.BG_color, GX.ColorClut)
        : RGB_PixelFormat(layer->bg.BG_color.r, layer->bg.BG_color.g, layer->bg.BG_color.b);
        break;
        case V3XBG_IMG:
        V3x_GetRenderBackground(pScene);
        break;
        default:
        break;
    }
    /*
    Charge Les Maps
    */
    for (i=0, ORI=pScene->ORI;i<pScene->numORI;i++, ORI++)
    {
        switch(ORI->type) {
            case V3XOBJ_MESH:
			{
				if (ORI->mesh)
				V3XMaterials_LoadFromMesh(ORI->mesh);
			}
			break;
            case V3XOBJ_LIGHT:
            {
                if (ORI->light->flare)
                {
                    V3XSPRITE *lt = (V3XSPRITE *)ORI->light->flare;
                    V3XMaterial_LoadTextures(&lt->_sp.mat);
                    V3X_CSP_Prepare(&lt->sp, 0x20);
                }
            }
            break;
        }
    }

    /*
    *   Charge les Tables realColor
    */
    if (!(V3X.Setup.flags&V3XOPTION_TRUECOLOR))
    {
        if( GX.View.BytePerPixel==1)
        {
            v3xtx_load(&layer->lt.alpha50);
            v3xtx_load(&layer->lt.additive);
            v3xtx_load(&layer->lt.blur);
            v3xtx_load(&layer->lt.gouraud);
            if (layer->lt.gouraud.table)
            {
                if (layer->lt.shift)
					REALCOLOR_Reduce(layer->lt.gouraud.table, layer->lt.shift);
                layer->lt.phong.table = (uint8_t**) MM_heap.malloc(256*sizeof(uint8_t*));
                for (i=0;i<128;i++)
                {
                    x = (int)sin16(i<<3);
                    x = MULF32_SQR(MULF32_SQR(MULF32_SQR(x)));
                    layer->lt.phong.table[i]=layer->lt.gouraud.table[x>>8];
                }
            }
        }
    }
    GX.ColorClut=NULL;
    V3XViewport_Setup(&V3X.Camera, GX.View);
    V3XMatrix_MeshTransform(&V3X.Camera);
    UNUSED(callback);
    MM_heap.active = m;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : V3XSCENE *load_V3XSCENE(char *mode, FlushClass *f);
*
* DESCRIPTION :
*
*/
static void RLXAPI *v3x_read_buf(void *buf, int32_t sz, int32_t n, SYS_FILEHANDLE in)
{
	int s = FIO_gzip.fread(buf, sz, n, in);
	SYS_ASSERT(n == s);
	return buf;
}

static void RLXAPI *v3x_read_alloc(int32_t sz, int32_t n, int32_t n2, SYS_FILEHANDLE in)
{
    uint8_t *tmp;
    int s;
    if (n2<n)
		n2 = n;
    s = sz * n2;
    if (!s)
		return NULL;
	tmp = (uint8_t*)MM_heap.malloc(s);
    s = FIO_gzip.fread(tmp, sz, n, in);
	SYS_ASSERT(n == s);
    return tmp;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XMATERIAL *V3XMaterials_GetFp(SYS_FILEHANDLE in, int mt)
*
* DESCRIPTION :
*
*/
#ifdef __BIG_ENDIAN__
static uint32_t BGETFIELD(uint32_t bf, int base, int length)
{
	return (bf >> base) & (( 1<<length ) - 1);
}
#endif

V3XMATERIAL *V3XMaterials_GetFp(SYS_FILEHANDLE in, int numMaterial)
{
    V3XMATERIAL *Mat;
	SYS_ASSERT(numMaterial < 255);
	Mat = (V3XMATERIAL*)MM_heap.malloc(sizeof(V3XMATERIAL) * numMaterial);

	{
		V3XMATERIAL *p = Mat;
		int i;

		for ( i = numMaterial; i; --i, ++p )
		{
			V3XMATERIALDISK d;
			v3x_read_buf(&d, sizeof(d), 1, in);
			MaterialDiskToMem(&d, p);
#ifdef __BIG_ENDIAN__
			{
				uint32_t info = p->lod;
				BSWAP32(&info, 1);
				p->info.TwoSide = BGETFIELD(info, 0, 1);
				p->info.Opacity = BGETFIELD(info, 1, 1);
				p->info.Perspective = BGETFIELD(info, 2, 1);
				p->info.Filtering = BGETFIELD(info, 3, 1);
				p->info.Texturized = BGETFIELD(info, 4, 2);
				p->info.Transparency = BGETFIELD(info, 6, 2);
				p->info.Shade = BGETFIELD(info, 8, 2);
				p->info.Sprite = BGETFIELD(info, 10, 2);
				p->info.Environment = BGETFIELD(info, 12, 4);
				p->info.Dynamic = BGETFIELD(info, 16, 1);
				p->info.AlphaLight = BGETFIELD(info, 15, 1);
				p->info.AlphaLight = BGETFIELD(info, 16, 1);
				p->info.Transparency2 = BGETFIELD(info, 17, 3);
				p->info.MultiPassBlend = BGETFIELD(info, 20, 2);
			}
#endif
		}
	}

    return Mat;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XMESH static RLXAPI *v3x_VMX_unpack_object(SYS_FILEHANDLE in)
*
* DESCRIPTION :
*
*/
static void V3XRGB_ConvertToMono(V3XSCALAR *mono, rgb32_t *rgb, unsigned n)
{
    for (;n!=0L;mono++, rgb++, n--) *mono = (V3XSCALAR)RGB_ToGray(rgb->r, rgb->g, rgb->b);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XMESH static RLXAPI *v3x_VMX_unpack_object(SYS_FILEHANDLE in)
*
* Description :
*
*/
V3XNODE static RLXAPI *v3x_VMX_unpack_node(SYS_FILEHANDLE in)
{
	V3XMESHDISK d;
	V3XMESH *obj = MM_heap.malloc(sizeof(*obj));
	v3x_read_buf(&d, sizeof(d), 1, in);
	MeshDiskToMem(&d, obj);
#ifdef __BIG_ENDIAN__
	BSWAP32((uint32_t*)&obj->matrix, 12);
	BSWAP32((uint32_t*)&obj->Tk, 3+3+1);
#endif

	return (V3XNODE*)obj;
}

static V3XMESH RLXAPI *v3x_VMX_unpack_object(SYS_FILEHANDLE in)
{
	V3XMESHDISK d;
	V3XMESH *obj = MM_heap.malloc(sizeof(*obj));
	int i;
	V3XPOLY *f;
	v3x_read_buf(&d, sizeof(d), 1, in);
	MeshDiskToMem(&d, obj);
#ifdef __BIG_ENDIAN__
	BSWAP16((uint16_t*)&obj->numVerts, 4);
	BSWAP32((uint32_t *)&obj->flags, 1);
	BSWAP32((uint32_t *)&obj->scale, 1);
	BSWAP32((uint32_t*)&obj->matrix, 12);
	BSWAP32((uint32_t*)&obj->Tk, 3+3+1);
#endif
	if ( obj->numVerts )
	{
		obj->vertex = (V3XVECTOR*)v3x_read_alloc(sizeof(V3XVECTOR), obj->numVerts, -1, in);
#ifdef __BIG_ENDIAN__
		BSWAP32((uint32_t*)obj->vertex , obj->numVerts*3);
#endif
		{
			V3XPOLYDISK d;
			int i;
			V3XPOLY *p = obj->face = MM_heap.malloc(sizeof(V3XPOLY) * obj->numFaces);

			for ( i = obj->numFaces; i; --i, ++p )
			{
				v3x_read_buf(&d, sizeof(d), 1, in);
				PolyDiskToMem(&d, p);
			}
		}

		if ( obj->uv )
		{
			obj->uv =(V3XUV*) v3x_read_alloc(sizeof(V3XUV), obj->numVerts, -1, in);
#ifdef __BIG_ENDIAN__
			BSWAP32((uint32_t*)obj->uv, obj->numVerts*2);
#endif
		}

		if ( obj->normal )
		{
			obj->normal =(V3XVECTOR*)v3x_read_alloc(sizeof(V3XVECTOR), obj->numVerts, -1, in);
#ifdef __BIG_ENDIAN__
			BSWAP32((uint32_t*)obj->normal, obj->numVerts*3);
#endif
		}

		if ( obj->flags & V3XMESH_HASSHADETABLE )
		{
			unsigned nb = ( obj->flags & V3XMESH_FLATSHADE )
					? obj->numFaces
					: obj->numVerts;
			obj->rgb = (rgb32_t*)v3x_read_alloc(sizeof(rgb32_t), nb, -1, in);
#ifdef __BIG_ENDIAN__
			BSWAP32((uint32_t*)obj->rgb, nb);
#endif
			if ( ! (V3X.Client->Capabilities & GXSPEC_RGBLIGHTING ) )
				V3XRGB_ConvertToMono(obj->shade, obj->rgb, nb);
		}
		else
			obj->rgb = NULL;

		if ( (V3X.Setup.flags & V3XOPTION_97) || ! obj->scale )
		{
			obj->scale = CST_ONE;
		}

		obj->normal_face = (V3XVECTOR*)v3x_read_alloc(sizeof(V3XVECTOR), obj->numFaces, -1, in);
#ifdef __BIG_ENDIAN__
		BSWAP32((uint32_t*)obj->normal_face, obj->numFaces*3);
#endif
		obj->material = V3XMaterials_GetFp(in, obj->numMaterial);

		for ( f = obj->face, i = obj->numFaces; i; ++f, --i )
		{
			int numEdges = f->numEdges;
			V3XMATERIAL *pMat;
#ifdef __BIG_ENDIAN__
			BSWAP32((uint32_t*)&f->matIndex, 1);
#endif
			--f->matIndex;
			SYS_ASSERT( ! ( ( f->matIndex < 0 ) || ( f->matIndex >= obj->numMaterial ) ) );
			f->Mat = obj->material + f->matIndex;
			pMat  = (V3XMATERIAL*) f->Mat;

			if ( ( V3X.Client->Capabilities & GXSPEC_ENABLEPERSPECTIVE )
						&& ( ! pMat->info.Sprite )
						&& ( pMat->info.Texturized ) )
			{
				pMat->info.Perspective = 1;
			}

			f->dispTab = (V3XPTS*)MM_heap.malloc(sizeof(V3XPTS)*numEdges);
			f->faceTab = (uint32_t *)v3x_read_alloc(sizeof(uint32_t), numEdges, -1, in);
#ifdef __BIG_ENDIAN__
			BSWAP32((uint32_t*)f->faceTab, numEdges);
#endif
			f->shade = ( pMat->info.Shade )
					? (V3XSCALAR *)v3x_read_alloc(sizeof(V3XSCALAR), numEdges, -1, in)
					: 0;

			if ( f->shade && ! ( V3X.Client->Capabilities & GXSPEC_RGBLIGHTING ) )
				V3XRGB_ConvertToMono(f->shade, f->rgb, numEdges);

			if ( pMat->info.Texturized )
            {
				unsigned j;
				unsigned n = pMat->info.Environment&V3XENVMAPTYPE_DOUBLE ? 2 : 1;
				f->uvTab = V3X_CALLOC(V3X_MAXTMU, V3XUV*);
				f->uvTab[0] = (V3XUV *) v3x_read_alloc(sizeof(V3XUV), numEdges, -1, in);
#ifdef __BIG_ENDIAN__
				BSWAP32((uint32_t*)f->uvTab[0], 2 * numEdges);
#endif
				for ( j = 0; j < numEdges; ++j )
				{
					f->uvTab[0][j].u /= 255.f;
					f->uvTab[0][j].v /= 255.f;
					SYS_ASSERT(f->faceTab[j] < obj->numVerts);
				}

				f->uvTab[1] = ( n > 1 ) ? V3X_CALLOC(numEdges, V3XUV) : 0;
			}
			else
			{
				f->uvTab = 0;
			}

			f->ZTab = ( pMat->info.Perspective ) ? V3X_CALLOC(numEdges, V3XWPTS) : 0;
		}
	}

	return obj;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XLIGHT static RLXAPI *v3x_VMX_unpack_light(SYS_FILEHANDLE in)
*
* Description :
*
*/
V3XLIGHT static RLXAPI *v3x_VMX_unpack_light(SYS_FILEHANDLE in)
{
    V3XLIGHT *light;
    light = (V3XLIGHT*)v3x_read_alloc(sizeof(V3XNODE), 1, -1, in);
#ifdef __BIG_ENDIAN__
    BSWAP32((uint32_t*)&light->pos, 16);
#endif
    if ((light->flags&V3XLIGHTCAPS_LENZFLARE)&&(light->material))
    {
        light->material = V3XMaterials_GetFp(in, 1);
    }
    else
    {
        light->material = NULL;
        light->flags&=~V3XLIGHTCAPS_LENZFLARE;
    }
	light->flags |= V3XLIGHTCAPS_RANGE;
    return light;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XCAMERA static RLXAPI *v3x_VMX_unpack_camera(SYS_FILEHANDLE in)
*
* Description :
*
*/
V3XCAMERA static RLXAPI *v3x_VMX_unpack_camera(SYS_FILEHANDLE in)
{
    V3XCAMERA *camera;
    camera = (V3XCAMERA*)v3x_read_alloc(sizeof(V3XNODE), 1, -1, in);
#ifdef __BIG_ENDIAN__
    BSWAP32((uint32_t*)&camera->M, 16);
#endif
    return camera;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XCOLLISION static *v3x_VMX_unpack_collide(SYS_FILEHANDLE in)
*
* DESCRIPTION :
*
*/
V3XCL static RLXAPI *v3x_VMX_unpack_collide(SYS_FILEHANDLE in)
{
	V3XCL *Cs = MM_heap.malloc(sizeof(*Cs));
	V3XCLDISK d;
	int i;
	V3XCL_ITEM *p;
	v3x_read_buf(&d, sizeof(d), 1, in);
	ClDiskToMem(&d, Cs);
#ifdef __BIG_ENDIAN__
	BSWAP32((uint32_t*)Cs, 2);
#endif
	p = Cs->item = (V3XCL_ITEM*)MM_heap.malloc(sizeof(V3XCL_ITEM) * Cs->numItem);

	for ( i = Cs->numItem; i; --i, ++p )
	{
		uint8_t buf[64]; // on disk the items are all 64 bytes
		V3XCL_ITEM *it = (V3XCL_ITEM*)buf;
		v3x_read_buf(buf, sizeof(buf), 1, in);

		switch ( it->type )
		{
			case V3XCTYPE_SPHERE:
				SphereDiskToMem((const V3XCL_SPHEREDISK*)it, (V3XCL_SPHERE*)p);
				break;
			case V3XCTYPE_AXISBOX:
				BoxDiskToMem((const V3XCL_BOXDISK*)it, (V3XCL_BOX*)p);
				break;
			case V3XCTYPE_MESH:
				CollMeshDiskToMem((const V3XCL_MESHDISK*)it, (V3XCL_MESH*)p);
				break;
			default:
				SYS_Debug("v3x_VMX_unpack_collide: unknown V3XCL_ITEM id=%d\n", it->type);
				break;
		}
	}

#ifdef __BIG_ENDIAN__
	BSWAP32((uint32_t*)&Cs->global, 9);
	{
		V3XCL_ITEM *item = Cs->item;
		for ( int i = Cs->numItem; i; ++item, --i )
		{
			BSWAP32((uint32_t*)&item->box.type , 16);
		}
	}
#endif

	if ( Cs->old[0] )
	{
		Cs->flags |= V3XCMODE_INVERTCONDITION;
	}

	return Cs;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XMorph static *v3x_VMX_unpack_morph3D(SYS_FILEHANDLE in)
*
* DESCRIPTION :
*
*/
V3XTWEEN static RLXAPI *v3x_VMX_unpack_morph3D(SYS_FILEHANDLE in)
{
	unsigned int i;
	V3XTWEENDISK d;
    V3XTWEEN *Mo;

	v3x_read_buf(&d, sizeof(d), 1, in);
	Mo = MM_heap.malloc(sizeof(*Mo));
	Mo->numFrames = d.numFrames;
	Mo->numVerts = d.numVerts;
	Mo->numFaces = d.numFaces;
#ifdef __BIG_ENDIAN__
	BSWAP32((uint32_t*)&Mo->numFrames, 3);
#endif
	if ( ( ! Mo->numFrames ) || ( ! Mo->numVerts ) )
		return 0;

	Mo->frame = (V3XTWEENFRAME*) MM_heap.malloc(Mo->numFrames*sizeof(V3XTWEENFRAME));

	for ( i = 0; i < Mo->numFrames; ++i )
	{
		Mo->frame[i].vertex = (V3XVECTOR*) v3x_read_alloc(sizeof(V3XVECTOR), Mo->numVerts, -1, in);
#ifdef __BIG_ENDIAN__
		BSWAP32((uint32_t*)Mo->frame[i].vertex, Mo->numVerts*3);
#endif
	}

	return Mo;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void RLXAPI v3x_VMX_unpack_ORI(V3XORI *ORI, SYS_FILEHANDLE in)
*
* Description :
*
*/
static void RLXAPI v3x_VMX_unpack_ORI(V3XORI *ORI, SYS_FILEHANDLE in, int bFormat97)
{
	if (ORI->type!=V3XOBJ_NONE)
    {
        if (ORI->node)
        {
            switch(ORI->type) {
                case V3XOBJ_LIGHT:
					ORI->light = v3x_VMX_unpack_light(in);
				break;
                case V3XOBJ_CAMERA:
					ORI->camera = v3x_VMX_unpack_camera(in);
				break;
				case V3XOBJ_MESH:
					ORI->mesh = v3x_VMX_unpack_object(in);
				break;
				default:
					ORI->node = v3x_VMX_unpack_node(in);
				break;
            }
        }

        if (ORI->morph)
			ORI->morph = v3x_VMX_unpack_morph3D(in);

        if (ORI->Cs)
			ORI->Cs = v3x_VMX_unpack_collide(in);
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void RLXAPI v3x_VMX_unpack_OVI(V3XOVI *OVI, SYS_FILEHANDLE in)
*
* DESCRIPTION :
*
*/
static void RLXAPI v3x_VMX_unpack_OVI(V3XOVI *OVI, SYS_FILEHANDLE in)
{
	OVI->node = v3x_VMX_unpack_node(in);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void RLXAPI v3x_VMX_unpack_TRI(V3XTRI *TRI, SYS_FILEHANDLE in)
*
* DESCRIPTION :
*
*/
static void RLXAPI v3x_VMX_unpack_TRI(V3XTRI *TRI, SYS_FILEHANDLE in)
{
#ifdef __BIG_ENDIAN__
	BSWAP32((uint32_t *)&TRI->index_CHAIN, 1);
	BSWAP16((uint16_t*)&TRI->numFrames, 3);
#endif

    if (TRI->keys)
    {
        if (TRI->flags&V3XKF_KEYEX)
        {
            TRI->keyEx=(V3XKEYEX*)v3x_read_alloc(sizeof(V3XKEYEX), TRI->numKeys, -1, in);
#ifdef __BIG_ENDIAN__
            {
                int i;
                V3XKEYEX *kf = TRI->keyEx;
                for (i=TRI->numFrames;i!=0;i--, kf++)
                {
                    BSWAP32((uint32_t*)kf, (sizeof(V3XKEY)>>2)+3);
                    BSWAP16((uint16_t*)&kf->frame, 2);
                }
            }
#endif
        }
        else
        {
            TRI->keys=(V3XKEY*)v3x_read_alloc(sizeof(V3XKEY), TRI->numFrames, -1, in);
#ifdef __BIG_ENDIAN__
            BSWAP32((uint32_t*)TRI->keys, TRI->numFrames*7);
#endif
        }
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3XLAYER_Read(V3XLAYER *layer, SYS_FILEHANDLE in)
*
* DESCRIPTION :
*
*/
static void v3xORI_Convert97(V3XSCENE *pScene, SYS_FILEHANDLE in)
{
	int numORI = pScene->numORI;
	V3XORI97 *ori97A = (V3XORI97*)MM_std.malloc(numORI*sizeof(V3XORI97));
	V3XORI97 *ori97;
	V3XORI *ori;
	int i;
	uintptr_t x;
	FIO_gzip.fread(ori97A, numORI, sizeof(V3XORI97), in);
	pScene->ORI = MM_CALLOC(numORI, V3XORI);

	for ( ori97 = ori97A, ori = pScene->ORI, i = 0
			; i < numORI
			; ++i, ++ori97, ++ori )
	{
		const uint8_t objTable[8] = { V3XOBJ_NONE, V3XOBJ_MESH, V3XOBJ_DUMMY, V3XOBJ_LIGHT, V3XOBJ_NONE, V3XOBJ_CAMERA, V3XOBJ_VIEWPORT};

#ifdef __BIG_ENDIAN__
		BSWAP32((uint32_t*)&ori97->global_rayon, 1);
		BSWAP32((uint32_t*)&ori97->global_pivot, 3);
		BSWAP16((uint16_t*)&ori97->index_Parent, 1);
#endif
		ori->flags = 0;
		sysStrnCpy(ori->name, ori97->name, 16);
		ori->type = objTable[ori97->Type];
		x = ori97->mesh;
		ori->mesh = (V3XMESH*)x;
		x = ori97->morph;
		ori->morph = (V3XTWEEN*)x;
		x = ori97->Cs;
		ori->Cs = (V3XCL*)x;
		ori->global_center = ori97->global_pivot;
		ori->global_rayon = ori97->global_rayon;
		ori->dataSize = ori97->index_Parent;
		ori->pad2[0]  = ori97->matrix_Method;
		ori->index_color = ori97->index_Color;
	}

	MM_std.free(ori97A);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void v3xOVI_Convert97(V3XSCENE *pScene, SYS_FILEHANDLE in)
*
* DESCRIPTION :
*
*/
static void v3xOVI_Convert97(V3XSCENE *pScene, SYS_FILEHANDLE in)
{
	int numOVI = pScene->numOVI;
	V3XOVI97 *ovi97A = (V3XOVI97*)MM_std.malloc(numOVI*sizeof(V3XOVI97));
	V3XOVI97 *ovi97;
	V3XOVI *ovi;
	int i;

	FIO_gzip.fread(ovi97A, numOVI, sizeof(V3XOVI97), in);
	pScene->OVI = MM_CALLOC(numOVI, V3XOVI);

	for ( ovi97 = ovi97A, ovi = pScene->OVI, i = numOVI; i; --i, ++ovi97, ++ovi )
	{
#ifdef __BIG_ENDIAN__
		BSWAP16((uint16_t*)&ovi97->index_OVI, 3);
#endif
		ovi->state = V3XSTATE_MATRIXUPDATE;

		if ( ovi97->Hide_Never )
			ovi->state |= V3XSTATE_CULLNEVER;

		if ( ovi97->Hide_ByDisplay )
			ovi->state |= V3XSTATE_HIDDEN;

		uintptr_t x = ovi97->mesh;
		ovi->mesh = (V3XMESH*)x;
		ovi->index_ORI = ovi97->index_ORI;
		ovi->index_INSTANCE = ovi97->index_OVI;
		ovi->index_TVI = ovi97->index_TVI;
		ovi->matrix_Method = pScene->ORI[ovi->index_ORI].pad2[0];
		ovi->index_PARENT = pScene->ORI[ovi->index_ORI].dataSize;
	}

	for ( ovi = pScene->OVI, i = numOVI; i; --i, ++ovi )
	{
		int j = ovi->index_PARENT;

		if ( j )
		{
			ovi->index_PARENT
				= V3XScene_OVI_GetByName(pScene, pScene->ORI[j].name) - pScene->OVI;
		}
	}

	MM_std.free(ovi97A);
}

static void ReadSceneNodes(V3XSCENE *pScene, SYS_FILEHANDLE in, int bFormat97)
{
	V3XLAYER *layer = &pScene->Layer;
	layer->tm.numFrames = 0;
	layer->tm.firstFrame = 0;

	if ( bFormat97 )
	{
		v3xORI_Convert97(pScene, in);
	}
	else
	{
		int numORI = pScene->numORI;
		int numAlloc = max(numORI, V3X.Setup.MaxExtentableObjet);
		V3XORI *p = pScene->ORI = MM_heap.malloc(sizeof(V3XORI) * numAlloc);

		for ( int i = numORI; i; --i, ++p )
		{
			V3XORIDISK d;

			v3x_read_buf(&d, sizeof(d), 1, in);
			OriDiskToMem(&d, p);
		}
	}

	{
		V3XORI *p = pScene->ORI;

		for ( uint32_t i = V3X.Setup.MaxExtentableObjet; i; --i, ++p )
		{
			if ( p->type == 0 )
				p->type = V3XOBJ_NONE;
		}
	}

    if ( bFormat97 )
		v3xOVI_Convert97(pScene, in);
    else
	{
		int numOVI = pScene->numOVI;
		int numAlloc = max(numOVI, V3X.Setup.MaxExtentableObjet);
		V3XOVI *p = pScene->OVI = MM_heap.malloc(sizeof(V3XOVI) * numAlloc);

		for ( int i = numOVI; i; --i, ++p )
		{
			V3XOVIDISK d;

			v3x_read_buf(&d, sizeof(d), 1, in);
			OviDiskToMem(&d, p);
		}
	}

	{
		int numTRI = pScene->numTRI;
		int numAlloc = max(numTRI, V3X.Setup.MaxExtentableObjet);
		V3XTRI *p = pScene->TRI = MM_heap.malloc(sizeof(V3XTRI) * numAlloc);

		for ( int i = numTRI; i; --i, ++p )
		{
			V3XTRIDISK d;

			v3x_read_buf(&d, sizeof(d), 1, in);
			TriDiskToMem(&d, p);
		}
	}

	{
		int numTVI = pScene->numTVI;
		int numAlloc = max(numTVI, V3X.Setup.MaxExtentableObjet);
		V3XTVI *p = pScene->TVI = MM_heap.malloc(sizeof(V3XTVI) * numAlloc);

		for ( int i = numTVI; i; --i, ++p )
		{
			V3XTVIDISK d;

			v3x_read_buf(&d, sizeof(d), 1, in);
			TviDiskToMem(&d, p);
		}
	}

	if ( bFormat97 )
	{
		V3XTVI *p = pScene->TVI;

		for ( int i = pScene->numTVI; i; --i, ++p )
		{
#ifdef __BIG_ENDIAN__
			BSWAP16((uint16_t*)&p->pad, 1);
#endif
			p->index_TRI = p->pad;
		}
	}

	{
		V3XORI *p = pScene->ORI;

		for ( int i = pScene->numORI; i; --i, ++p )
			v3x_VMX_unpack_ORI(p, in, bFormat97);
	}

	{
		V3XOVI *p = pScene->OVI;

		for ( int i = pScene->numOVI; i; --i, ++p )
			v3x_VMX_unpack_OVI(p, in);
	}

	{
		V3XTRI *p = pScene->TRI;

		for ( int i = pScene->numTRI; i; --i, ++p )
		{
			v3x_VMX_unpack_TRI(p, in);

			if ( layer->tm.firstFrame < p->startFrame )
				layer->tm.firstFrame = p->startFrame;

			if ( layer->tm.numFrames < p->numFrames )
				layer->tm.numFrames = p->numFrames;
		}
	}
}

#define HEAD1 sizeof(V3XSCENE) - sizeof(V3XLAYER)
_RLXEXPORTFUNC V3XSCENE RLXAPI *V3XScene_GetFromFile_VMX(const char *filename)
{
#define HEAD1_SIZE (sizeof(V3XSCENEHEADER))
#define HEAD_TOTAL_SIZE (HEAD1_SIZE + sizeof(V3XLAYER97))
	V3XSCENEHEADER *hdr
		= (V3XSCENEHEADER*) MM_std.malloc(HEAD_TOTAL_SIZE);
	SYS_FILEHANDLE in = FIO_gzip.fopen(filename, "rb");
	V3XSCENE *pScene = (V3XSCENE*) MM_heap.malloc(sizeof(V3XSCENE));
	V3XLAYER *layer = &pScene->Layer;
	V3X.Setup.flags |= V3XOPTION_97;
	FIO_gzip.fread(hdr, HEAD_TOTAL_SIZE, 1, in);
	pScene->numOVI = hdr->numOVI;
	pScene->numTVI = hdr->numTVI;
	pScene->numTRI = hdr->numTRI;
	pScene->numORI = hdr->numORI;
#ifdef __BIG_ENDIAN__
	BSWAP16(&pScene->numOVI, 4);
#endif

	SYS_Debug("sizeof(V3XSCENEHEADER)=%zu\n", sizeof(V3XSCENEHEADER));
	SYS_Debug("sizeof(V3XLAYER97)=%zu\n", sizeof(V3XLAYER97));
	SYS_Debug("sizeof(V3XLAYER)=%zu\n", sizeof(V3XLAYER));
	SYS_Debug("sizeof(V3XSCENEDISK)=%zu, sizeof(V3XSCENE)=%zu\n"
					, sizeof(V3XSCENEDISK), sizeof(V3XSCENE));

	// sizeof(V3XORIDISK) should be 64
	// sizeof(V3XOVIDISK) should be 64
	// sizeof(V3XTRIDISK) should be 32
	// sizeof(V3XTVIDISK) should be 16
	SYS_Debug("sizeof(V3XORIDISK)=%zu"
					", sizeof(V3XOVIDISK)=%zu"
					", sizeof(V3XTRIDISK)=%zu"
					", sizeof(V3XTVIDISK)=%zu"
					", sizeof(V3XKEY)=%zu\n"
					, sizeof(V3XORIDISK)
					, sizeof(V3XOVIDISK)
					, sizeof(V3XTRIDISK)
					, sizeof(V3XTVIDISK)
					, sizeof(V3XKEY));

	SYS_Debug("sizeof(V3XMATERIALDISK)=%zu, sizeof(V3XMATERIAL)=%zu\n"
					, sizeof(V3XMATERIALDISK)
					, sizeof(V3XMATERIAL));
	SYS_Debug("sizeof(V3XMESHDISK)=%zu, sizeof(V3XMESH)=%zu\n"
					, sizeof(V3XMESHDISK)
					, sizeof(V3XMESH));

	SYS_Debug("ReadSceneNodes start pos=%ld\n", FIO_gzip.ftell(in));
	ReadSceneNodes(pScene, in, 1);
	V3XLAYER97 *bk = (V3XLAYER97*)((uint8_t*)hdr + HEAD1_SIZE);
	sysStrCpy(layer->lt.palette.filename, bk->ColorTable_name);
	sysStrCpy(layer->bg.filename, bk->background_name);
	layer->bg.BG_color = bk->SolidColor;
	layer->bg.flags  |= V3XBG_COLOR;
	layer->fg.color.r = bk->FogColor.r;
	layer->fg.color.g = bk->FogColor.g;
	layer->fg.color.b = bk->FogColor.b;

	if ( bk->FogActivate )
	{
		layer->fg.flags |= V3XFG_LIN;
	}

	MM_std.free(hdr);
	FIO_gzip.fclose(in);
	return pScene;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RLXAPI V3XScene_Verify(V3XSCENE *pScene)
*
* DESCRIPTION :
*
*/
int RLXAPI V3XScene_Verify(V3XSCENE *pScene)
{
    V3XOVI *OVI;
    V3XORI    *ORI;
    V3XCL             *OCs;
    int i, j;
    V3X.Buffer.MaxObj = 0;
    V3X.Light.numSource = 0;
    V3X.Light.tables = &pScene->Layer.lt;
    for (OVI=pScene->OVI, i=0;i<pScene->numOVI;i++, OVI++)
    {
        if (!OVI->ORI)
        {
            OVI->ORI = pScene->ORI + 0;
        }
        ORI = OVI->ORI;
        OVI->state|=V3XSTATE_TOPROCESS|V3XSTATE_MATRIXUPDATE;
        if (ORI->type == V3XOBJ_MESH )
        {
			if (OVI->ORI->mesh)
			{
				V3XMesh_SetRender(OVI->mesh);
				OVI->mesh->radius = OVI->ORI->global_rayon;
				SYS_ASSERT(OVI->mesh->numVerts<V3X.Buffer.MaxPointsPerMesh);
			}
        }
        else
        if (ORI->type == V3XOBJ_LIGHT)
        {
		     if (!OVI->light->diminish)
			OVI->light->diminish=1.f/2000.f;
        }
    }
    for (OVI=pScene->OVI, i=0;i<pScene->numOVI;i++, OVI++)
    {
        ORI = OVI->ORI;
        if (ORI->type!=V3XOBJ_NONE)
        {
            if (OVI->node) OVI->Tk = &OVI->node->Tk;
            else
            {
                if (ORI->type==V3XOBJ_CAMERA) OVI->Tk = &V3X.Camera.Tk;
            }
        }
        else OVI->Tk = NULL;
        OCs = ORI->Cs;
        if (ORI->type == V3XOBJ_MESH )
        {
            if (OVI->state&V3XSTATE_CULLNEVER)
            {
                for (j=0;j<OVI->mesh->numMaterial;j++)
                OVI->mesh->material[j].render_far = OVI->mesh->material[j].render_near;
            }
        }
        if (OCs)
        {
            V3XVector_Set(&OCs->velocity, CST_ZERO, CST_ZERO, CST_ZERO);
        }
        if (ORI->type!=V3XOBJ_NONE)
        {
            if (ORI->type == V3XOBJ_MESH) OVI->mesh->flags|=V3XMESH_FULLUPDATE;
            V3XScene_MatrixBuild(OVI);
            OVI->state |=V3XSTATE_MATRIXUPDATE;
            V3XScene_ObjectBuild(OVI, TRUE);
            if (ORI->type==V3XOBJ_MESH)   OVI->mesh->flags&=~V3XMESH_FULLUPDATE;
            OVI->state |=V3XSTATE_MATRIXUPDATE;
        }
    }
    V3XViewport_Setup(&V3X.Camera, GX.View);
    i = V3X.Setup.flags;
    V3X.Setup.flags = 0;
    V3XScene_Viewport_Build(pScene, NULL);
    if (pScene->Layer.lt.palette.lut) sysMemCpy(GX.ColorTable, pScene->Layer.lt.palette.lut, 768);
    V3X.Setup.flags = i;
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : V3XSCENE *V3XScene_GetFromFile(char *filename)
*
* DESCRIPTION : Charge une scene
*
*/
V3XSCENE RLXAPI *V3XScene_GetFromFile(const char *filename)
{
    V3XSCENE *pScene = V3XScene_GetFromFile_VMX(filename);

    if (pScene)
    {
        pScene->Layer.lt.palette.lut = NULL;
        V3XScene_Validate(pScene);
        V3XScene_CRC_Check(pScene);
    }
    return pScene;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XOVI RLXAPI *V3XOVI_GetFromFile(V3XSCENE *pScene, char *filename)
*
* DESCRIPTION :
*
*/
V3XOVI RLXAPI *V3XOVI_GetFromFile(V3XSCENE *pScene, const char *filename, int doChild)
{
    uint32_t flags;
    V3XOVI **ovl = (V3XOVI **)V3X.Buffer.OVI;
    V3XOVI *OVI, *master=NULL;
    unsigned j=0;
    SYS_FILEHANDLE in = FIO_gzip.fopen(filename, "rb");
    if (!in) return NULL;
    if (!pScene) return NULL;
    // Read header
    FIO_gzip.fread(&flags, 1, sizeof(uint32_t), in);
#ifdef __BIG_ENDIAN__
    BSWAP32(&flags, 1);
#endif
    for (j=0;j<flags;j++)
    {
        OVI = V3XScene_NewObject(pScene, 0);
        if (OVI)
        {
            ovl[j]=OVI;
            OVI->state|=V3XSTATE_MATRIXUPDATE;
            OVI->matrix_Method = V3XMATRIX_Quaternion;
            OVI->ORI->type = V3XOBJ_DUMMY;
        }
    }
    ovl[j]=NULL;
    j = 0;
    do
    {
        FIO_gzip.fread(&flags, 1, sizeof(uint32_t), in);
#ifdef __BIG_ENDIAN__
        BSWAP32(&flags, 1);
#endif
        OVI = ovl[j];
        if (OVI)
        {
            V3XORI *ORI = OVI->ORI;
            int k;
            j++;
            FIO_gzip.fread(ORI, 1, sizeof(V3XORI), in);
            v3x_VMX_unpack_ORI(ORI, in, 0);
            *OVI->node =* ORI->node;
            k = (flags>>8);
            if (k) OVI->parent = ovl[k-1];
            else
            {
                OVI->parent = NULL;
                if (!master) master=OVI;
            }
        }
    }while((flags&4)==0);
    if (doChild) V3XOVI_BuildChildren(master, pScene);
    // Unpack data
    V3XScene_MatrixBuild(master);
    FIO_gzip.fclose(in);
    return master;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XTVI RLXAPI *V3XTVI_GetFromFile(V3XSCENE *pScene, char *filename)
*
* DESCRIPTION :
*
*/
V3XTVI RLXAPI *V3XTVI_GetFromFile(V3XSCENE *pScene, const char *filename)
{
    uint32_t flags;
    SYS_FILEHANDLE in = FIO_gzip.fopen(filename, "rb");
    V3XTVI *master=NULL;
    if (!in)    return NULL;
    if (!pScene) return NULL;
    do
    {
        // Read
        V3XTRI    *TRI = NULL;
        V3XTVI *TVI = NULL;
        TRI = V3XScene_NewTRI(pScene);
        TVI = V3XScene_NewTVI(pScene);
        TVI->TRI  = TRI;
        TVI->index_TRI = TRI - pScene->TRI;
        if (!master) master = TVI;
        // Read from disk
        FIO_gzip.fread(&flags, 1, sizeof(uint32_t), in);
#ifdef __BIG_ENDIAN__
        BSWAP32(&flags, 1);
#endif
        FIO_gzip.fread(TRI, 1, sizeof(V3XTRI), in);
        v3x_VMX_unpack_TRI(TRI, in);
    }while((flags&4)==0);
    return master;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XTRI RLXAPI *V3XTRI_GetFromFile(V3XSCENE *pScene, char *filename)
*
* Description :
*
*/
V3XTRI RLXAPI *V3XTRI_GetFromFile(V3XSCENE *pScene, char *filename)
{
    uint32_t flags;
    SYS_FILEHANDLE in = FIO_gzip.fopen(filename, "rb");
    V3XTRI *master=NULL;
    if (!in)    return NULL;
    if (!pScene) return NULL;
    do
    {
        // Read
        V3XTRI    *TRI = NULL;
        TRI = V3XScene_NewTRI(pScene);
        if (!master) master = TRI;
        // Read from disk
        FIO_gzip.fread(&flags, 1, sizeof(uint32_t), in);
#ifdef __BIG_ENDIAN__
        BSWAP32(&flags, 1);
#endif
        FIO_gzip.fread(TRI, 1, sizeof(V3XTRI), in);
        v3x_VMX_unpack_TRI(TRI, in);
    }while((flags&4)==0);
    return master;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XOVI *V3XScene_MergeMesh(V3XMesh *mesh)
*
* DESCRIPTION :
*
*/
V3XOVI *V3XScene_Mesh_Merge(V3XSCENE *pScene, V3XMESH *mesh, const char *name)
{
    V3XOVI *OVI = pScene->OVI;
    V3XORI *ORI = pScene->ORI;
    while (ORI->type!=V3XOBJ_NONE)  ORI++;
    while ((OVI->ORI != NULL)&&(OVI->ORI->type != V3XOBJ_NONE)) OVI++;
    OVI->ORI = ORI;
    OVI->TVI = NULL;
    // Reset OVI
    OVI->mesh = (V3XMESH*)MM_heap.malloc(sizeof(V3XMESH));
    sysMemCpy(OVI->mesh, mesh, sizeof(V3XMESH));
    OVI->matrix_Method = V3XMATRIX_Euler;
    OVI->state|=V3XSTATE_MATRIXUPDATE;
    // Reset ORI
    sysMemZero(ORI, sizeof(V3XORI));
    ORI->mesh = mesh;
    ORI->type = V3XOBJ_MESH;
    sysStrCpy(ORI->name, name);
    return OVI;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XOVI *V3XOVI_InstanceOVI(V3XSCENE *pScene, V3XOVI *cOVI)
*
* Description :
*
*/
V3XOVI *V3XOVI_InstanceOVI(V3XSCENE *pScene, V3XOVI *cOVI, int mode)
{
    V3XORI    *cORI = cOVI->ORI;
    V3XOVI *OVI = V3XScene_NewObject(pScene, (mode & 1) ? 6 : 2);
	V3XORI    *ORI = OVI->ORI;
	if (mode & 2)
	{
		V3XNODE *node = ORI->node;
		*ORI = *cORI;
		ORI->node = node;
        ORI->Cs = NULL;
		V3XMesh_Duplicate(OVI->mesh, cORI->mesh);
		ORI->flags|=V3XORI_DUPLICATED;
		OVI->index_INSTANCE = 0;
		*ORI->mesh = *OVI->mesh;
		sysStrCpy(ORI->name, cORI->name);
	}
	else
    if (mode & 1)
    {
        OVI->ORI = cORI;
        OVI->index_INSTANCE = 0;
		OVI->state|=V3XSTATE_INSTANCED;
    }
    else
    {
		char tex[256];
        V3XORI *ORI = OVI->ORI;
		assert(cORI!=NULL);
        *ORI = *cORI;
        ORI->Cs = NULL;//cORI->Cs;
        sprintf(tex, "%s~%x", cOVI->ORI->name, (unsigned)(OVI-pScene->OVI));
        sysStrnCpy(ORI->name, tex, 16);
        ORI->flags|=V3XORI_DUPLICATED|V3XORI_CSDUPLICATED;
        OVI->index_INSTANCE = cORI - pScene->ORI;
		assert(OVI->ORI!=NULL);
		OVI->state|=V3XSTATE_INSTANCED;
    }
    OVI->state|=V3XSTATE_VALIDPOINTER;
    return OVI;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XOVI *V3XOVI_InstanceGroup(V3XSCENE *pScene, V3XOVI *cOVI)
*
* Description :
*
*/
V3XOVI *V3XOVI_InstanceGroup(V3XSCENE *pScene, V3XOVI *cOVI, int mode)
{
    V3XOVI **child = cOVI->child;
    V3XOVI *OVI;
    int n = 0;
	if (!child) V3XOVI_BuildChildren(cOVI, pScene);
	child = cOVI->child;

	while (*child!=NULL) {child++;n++;}
    OVI = V3XOVI_InstanceOVI(pScene, cOVI, mode);
    OVI->parent = cOVI->parent;
    if (!n) n++;
    OVI->child = (V3XOVI **)MM_heap.malloc((n+1)*sizeof(V3XOVI **));
    for (n = 0, child = cOVI->child;*child!=NULL;child++, n++)
    {
        OVI->child[n] = V3XOVI_InstanceOVI(pScene, *child, mode);
    }
    OVI->child[n] = NULL;
    for (n = 0, child = cOVI->child;*child!=NULL;child++, n++)
    {
        OVI->child[n]->parent = OVI;
    }
    return OVI;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XOVI_Release(V3XSCENE *pScene, V3XOVI *OVI, int doChild)
*
* Description :
*
*/
void V3XOVI_Release(V3XSCENE *pScene, V3XOVI *OVI, int doChild)
{
    int i = OVI - pScene->OVI;
    int j = OVI->ORI - pScene->ORI;
    if ((doChild)&&(OVI->child))
    {
        int n;
        V3XOVI **child = OVI->child;
        for (n = 0, child = OVI->child;*child!=NULL;child++, n++)
        {
            V3XOVI *ch = *child;
            int k = ch->ORI - pScene->ORI;
            v3x_Destroy_OVI(pScene, ch - pScene->OVI);
            if (ch->ORI)
            v3x_Destroy_ORI(pScene, k);
        }
    }
    v3x_Destroy_OVI(pScene, i);
    v3x_Destroy_ORI(pScene, j);
}

