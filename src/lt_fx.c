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
#define NG_EXPORT extern
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "_stub.h"
#include "systools.h"
#include "sysctrl.h"
#include "sysini.h"
#include "sysnetw.h"
//
#include "sysresmx.h"
#include "gx_struc.h"
#include "gx_csp.h"
#include "gx_flc.h"
#include "v3xdefs.h"
#include "v3x_2.h"
#include "v3xcoll.h"
#include "v3xrend.h"
#include "v3xmaps.h"
#include "v3xscene.h"

#include "v3x_1.h"
#include "gx_init.h"
#include "gx_rgb.h"
#include "gx_tools.h"
#include "v3xrend.h"
#include "v3xmaps.h"
#include "v3xtrig.h"
#include "gui_os.h"
#include "systime.h"
#include "iss_defs.h"
//
#include "lt_struc.h"
#include "lt_data.h"
#include "lt_func.h"

void NG_FXCreate(void)
{
    int i;
    SGEffect *pExpl;

    for (pExpl=g_SGGame.pExpl, i=g_SGGame.numEffects;i!=0;pExpl++, i--)
    {
        pExpl->fce.numEdges = 4;
        pExpl->fce.dispTab  = (V3XPTS*)MM_heap.malloc(4*sizeof(V3XPTS)); // 1 fois
        pExpl->fce.uvTab = (V3XUV**)MM_heap.malloc(sizeof(V3XUV*));   // 1 fois
        pExpl->fce.uvTab[0] = (V3XUV*) MM_heap.malloc(4*sizeof(V3XUV));  // 1 fois
        sysMemZero(&pExpl->material, sizeof(V3XMATERIAL));
        
		pExpl->fce.Mat = &pExpl->material;
        pExpl->material.info.Texturized = 1;
        pExpl->material.info.Sprite = 1;
        pExpl->material.info.Opacity  = 1;
        pExpl->material.Render  = V3XRCLASS_bitmap_transp;
		pExpl->material.alpha = (V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? 200 : 128;
    }    
    return;
}


void NG_FXDraw(void)
{
    int32_t i;
    SGEffect *p;
    for (p=g_SGGame.pExpl, i=g_SGGame.numEffects;i!=0;p++, i--)
    {
      	if (p->fce.visible)
      	{            
			int j;
            for (j=0;j<2;j++)
            {       
				GXSPRITE *sp = p->Sprite->item + p->Step[j];

                CSP_Color(RGB_PixelFormat(
					p->material.diffuse.r/(j+1),
					p->material.diffuse.g/(j+1), 
					p->material.diffuse.b/(j+1)
				));

				GX.csp.zoom_TrspADD(sp,
                        (int32_t)(p->pos2D[j].x - p->size2D[j].x/2),
                        (int32_t)(p->pos2D[j].y - p->size2D[j].y/2),
                        (int32_t)p->size2D[j].x, 
						(int32_t)p->size2D[j].y
				);
            }
        }         
    }
}

// FX
void NG_FXUpdate(void)
{
    int32_t i, delta;
    int32_t t = V3X.Time.ms;
    SGEffect *p;
    for (p=g_SGGame.pExpl, i=g_SGGame.numEffects;i!=0;p++, i--)
    {
        if (!p->Dispo)
        {
			int alpha = 255;
            if (p->FxCode>=0)
            {
                if (p->PlaySample)
                {
                    p->TagFx = NG_Audio3DPlay(p->FxCode, p->pos, NULL);
                    p->PlaySample = 0;
                }
                else
                {
                    if ((p->TagFx)&&(!NG_Audio3DUpdate(p->pos, NULL, p->TagFx, p->FxCode)))
                       p->TagFx=0;
                }
            }

            delta = (t - p->LastFrame) / ( 1 + p->Sprite->speed);

            if (p->useKeep)
				p->OVI->state|=V3XSTATE_MATRIXUPDATE;

            if ((p->type==FX_DEBRIS)||(p->type==FX_SMOKE))
            {
				float scale = randomf(p->type==FX_DEBRIS ? 3+4 : 2+8);
                V3XVector_PolarSet(p->pos, &p->cent, (V3XSCALAR)p->angle.x, (int32_t)p->angle.y, (int32_t)p->angle.z);
                p->angle.x+= (int)(g_cTimer.fCounter * scale);
                p->OVI->state|=V3XSTATE_MATRIXUPDATE;
				if (p->type == FX_DEBRIS)
					alpha = 255;
				else
					alpha = 128;
            }

            if (delta)
            {
				p->Step[1]=p->Step[0];
                p->Step[0]+=delta;
                p->LastFrame = t;
                if (p->Step[0]>=p->Sprite->maxItem-1)
                {
                    p->Nloop--;
                    if (p->Nloop==0)
                    {
                        p->Dispo = 1;
                        p->fce.visible = 0;
                    }
                    else
                    {
                        p->Step[0]%=p->Sprite->maxItem;
                        p->PlaySample = 1;
                    }
                }
                // Fade out (on last frame)
                if (p->Nloop == 1)
                {
                    unsigned a = p->Step[0] > p->FadeStart ? alpha - ((alpha*(p->Step[0] - p->FadeStart))
                                                                            / (p->Sprite->maxItem - p->FadeStart)) : alpha;
                    RGB_Set(p->material.diffuse, a, a, a);
                }
            }

			if ((!p->Dispo)&&(p->type==FX_FINISH)&&(p->OVI->state&V3XSTATE_HIDDEN))
            {
                p->Dispo = 1;
                p->fce.visible = 0;
            }
            if (!p->Dispo)
            {
				int j;           		
				for (j=0;j<2;j++)
				{

					if (V3XVector_TransformProject_pts(p->pos, &p->pos2D[j]))
					{
						V3XSCALAR sx = V3X.ViewPort.Focal / p->pos2D[j].z;
						p->size2D[j].y = p->Size * sx;
						p->size2D[j].x = p->size2D[j].y * V3X.ViewPort.Ratio;
						p->pos2D[j].z = -p->pos2D[j].z;
						if (j == 0)
						p->fce.visible = 1;
					}
					else 
					{
						if (j == 0)
						p->fce.visible = 0;
					}
				}
				p->pos2D[1] = p->pos2D[0];
            }
        }
    }
    return;
}

// Explosion sphere
void NG_FXUpdateEx(void)
{
    int32_t i;
    SGEffect *pExpl;
    if (V3X.Client->Capabilities&GXSPEC_HARDWARE)
	{
		for (pExpl=g_SGGame.pExpl, i=g_SGGame.numEffects;i!=0;pExpl++, i--)
		{
			V3XOVI *OVI = pExpl->Sphere;
			if ((OVI)&&(pExpl->radius<256.f))
			{
				V3XMATRIX m;
				V3XSCALAR rapport = DIVF32(10 * pExpl->OVI->ORI->global_rayon, OVI->ORI->global_rayon);
				V3XSCALAR radius = (pExpl->radius * rapport)/256.f;
				pExpl->radius+= g_cTimer.fCounter*3.f;
				if (pExpl->radius > 256.f)
					pExpl->radius = 256.f;
				V3XMatrix_SetIdentity(m.Matrix);
				m.v.Pos = *pExpl->pos;
				NG_SetSpherePos(OVI, &m, radius, 1);
				{
					V3XMATERIAL *Mat = OVI->mesh->material+0;
					Mat->diffuse.r = 255;
					Mat->diffuse.g = 255;
					Mat->diffuse.b = 255;
					Mat->info.Transparency = V3XBLENDMODE_ALPHA;
					Mat->alpha = (u_int8_t)(256.f- pExpl->radius);
				}
			}
		}
	}
    return;
}


void NG_FXLoadData(void)
{
	int i;    
	if (V3X.Client->Capabilities&GXSPEC_HARDWARE)
    {            
        GXSPRITE *sp;
		g_pspFlares = MM_CALLOC(1, GXSPRITEGROUP);
        g_pspFlares->maxItem = 12;
        g_pspFlares->item = MM_CALLOC(g_pspFlares->maxItem, GXSPRITE);
        g_pspFlares2 = NULL;
		sp = g_pspFlares->item;
        for (i=0;i<g_pspFlares->maxItem;i++, sp++)
        {
			char tex[256];
            sprintf(tex, ".\\int\\le%02d.png", i+1);
            IMG_LoadFn(tex, sp);
			GX.Client->UploadSprite(sp, GX.ColorTable, 1);
        }
    }
	else
	{
		g_pspFlares = FLI_LoadToSpriteGroup(".\\int\\flare.flc", 0);
        g_pspFlares2 = FLI_LoadToSpriteGroup(".\\int\\flare2.flc", 0);
	}

	i = 0;
	while (g_pszAnimList[i]!=NULL)
    {
		char tex[256]; 
		sprintf(tex, ".\\anim\\%s", g_pszAnimList[i]);
        g_pspFX[i] = FLI_LoadToSpriteGroup(tex, 0);
		SYS_ASSERT(g_pspFX[i]);
        i++;
    }    

	{
		SGEffect *pExpl;
	   
		for (pExpl=g_SGGame.pExpl, i=g_SGGame.numEffects;i!=0;pExpl++, i--)
		{
			pExpl->Dispo = 1;
			pExpl->fce.visible = 0;
			V3XMaterial_Register(&pExpl->material);
		}
	}
    return;
}

void NG_FXReleaseData()
{
	int i = 0;
	while (g_pszAnimList[i]!=NULL)
    {
		if (g_pspFX[i])
			CSPG_Release(g_pspFX[i]);
		g_pspFX[i] = 0;
        i++;
    }
	if (g_pspFlares)
		CSPG_Release(g_pspFlares);
	g_pspFlares = 0;

	if (g_pspFlares2)
		CSPG_Release(g_pspFlares2);
	g_pspFlares2 = 0;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_FXRelease(void)
*
* DESCRIPTION :
*
*/
void NG_FXRelease(void)
{
    int i;
    SGEffect *pExpl;
    for (pExpl=g_SGGame.pExpl, i=g_SGGame.numEffects;i!=0;pExpl++, i--)
    {
		MM_heap.free(pExpl->fce.dispTab);
		MM_heap.free(pExpl->fce.uvTab[0]);
		MM_heap.free(pExpl->fce.uvTab);
    }
    return;
}

#define MAXLENZ 12

typedef struct
{
    V3XSCALAR	pos;
    V3XSCALAR	radius;
    unsigned	type;
}FlareInfo;

static const FlareInfo Flare[16]={
    {-.4f, 3, 0}, 
    { .4f, 3, 0}, 
    { .5f, 8, 0}, 
    { .6f, 4, 0},
    { .8f, 3, 0}, 
    { .9f, 1, 0}, 
    {1.3f, 2, 0},
    {1.4f, 5, 0}, 
    {1.5f, 8, 0}, 
    {1.6f, 16, 1}, 
    {1.8f, 10, 0}, 
	{2.0f, 4, 0}
};

void NG_FXFlare(void)
{
    V3XVECTOR zero;
    int32_t i, j, op = V3X.Setup.flags;
    V3XLIGHT *light;
    V3XVECTOR lensSize;
    GX.csp_cfg.table = g_SGGame.Scene->Layer.lt.alpha50.table;
    zero.x = V3X.ViewPort.center.x;
    zero.y = V3X.ViewPort.center.y;
    zero.z = CST_ZERO;
    V3X.Setup.flags|=V3XOPTION_DUPPOLY;
    for (i=0, light=V3X.Light.light;i<V3X.Light.numSource;light++, i++)
    {
        V3XSCALAR it = light->intensity / 255.f;
        V3XSCALAR sz = MAXLENZ * it;
        V3XVECTOR lightOnView;
        lensSize.z = V3X.ViewPort.Focal* .025f * it;
        if (light->flags&V3XLIGHTCAPS_LENZFLARE)
        {
            V3XVECTOR light2cam;   
            switch (light->type) {
                case V3XLIGHTTYPE_OMNI:
                case V3XLIGHTTYPE_DIRECTIONAL:
                V3XVector_Dif(&light2cam, &V3X.Camera.M.v.Pos, &light->pos);
                break;
                case V3XLIGHTTYPE_SPOT:
                V3XVector_Dif(&light2cam, &light->target, &light->pos);
                break;
            }
            //
            if (V3XVector_TransformProject_pts(&light->pos, &lightOnView))
            {
                V3XSCALAR alphaLevel;
                V3XVector_Normalize(&light2cam, &light2cam);
                lightOnView.z = light->pos.z;
                alphaLevel = V3XVector_DotProduct(&light2cam, &V3X.Camera.M.v.K);
                if (alphaLevel>.75f)
                {
                    V3XVECTOR direction;
                    V3XVECTOR lenzOnView;
                    const FlareInfo *F = Flare;
                    if (alphaLevel> CST_ONE) 
						alphaLevel = CST_ONE;

                    if (alphaLevel>=0.98f)
                    {
                        RGB_Set(g_SGGame.FlashColor, 255, 255, 255);
                        g_SGGame.FlashAlpha = (short)(8.f * alphaLevel);
                    }
                    V3XVector_Dif(&direction, &zero, &lightOnView);
                    if (GX.View.BytePerPixel>1)
                    {
                        CSP_Color(RGB_PixelFormat(255, 255, 255));
                        GX.csp_cfg.alpha = 255;
                    }
                    else
                    {
                        alphaLevel = MULF32((V3XSCALAR)(g_pspFlares->maxItem - 1), (CST_ONE - alphaLevel));
                        GX.csp_cfg.table = g_SGGame.Scene->Layer.lt.alpha50.table;
                    }
                    lenzOnView = lightOnView;
                    if (!sz) sz++;
                    for (j=(int32_t)sz;j!=0;F++, j--)
                    {
                        GXSPRITE *sp;
                        int32_t x, y;
                        lenzOnView.x =  lightOnView.x + direction.x * F->pos;
                        lenzOnView.y =  lightOnView.y + direction.y * F->pos;
                        sp =  (GX.View.BytePerPixel>1)
                        ?  g_pspFlares->item + MAXLENZ - j
                        : ((F->type ? g_pspFlares2->item : g_pspFlares->item) + (int32_t)alphaLevel);
                        lensSize.y = (- F->radius * lensSize.z);
                        lensSize.x = (lensSize.y * V3X.ViewPort.Ratio);
                        if (lensSize.x<0) lensSize.x=-lensSize.x;
                        if (lensSize.y<0) lensSize.y=-lensSize.y;
                        x = (int32_t)(lenzOnView.x - (lensSize.x/2));
                        y = (int32_t)(lenzOnView.y - (lensSize.y/2));    
                        GX.csp.zoom_TrspADD(sp, x, y, (int32_t)lensSize.x, (int32_t)lensSize.y);
                    }
                }
            }
        }
    }
    V3X.Setup.flags = op;
    return;
}

void NG_AddFaceMoreObjects(void)
{
	/*
    SGEffect *pExpl;

    int i;
    if (V3X.Setup.flags&V3XOPTION_DUPPOLY) 
		return;
	
    for (pExpl=g_SGGame.pExpl, i=g_SGGame.numEffects;i!=0;pExpl++, i--)
    {
        if ((pExpl->fce.visible)&&(!pExpl->Dispo))
        {
            V3XPOLY *p = V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY ? V3XPoly_XYClipping(&pExpl->fce) : &pExpl->fce;
            if ((p->visible)&&(V3X.Buffer.MaxFaces<V3X.Buffer.MaxFacesDisplay))
            {
                V3X.Buffer.RenderedFaces[V3X.Buffer.MaxFaces++] = p;
            }     
        }
    }
	*/
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_FXImpact(int power)
*
* DESCRIPTION :
*
*/
void NG_FXImpact(int power)
{
    g_SGGame.Shock=power<12 ? power : 12;
    return;
}


int NG_FXGetByName(char *s)
{
    int i=0;
    while(g_pszAnimList[i]!=NULL)
    {
        if (sysStriCmp(g_pszAnimList[i], s)==0)
        {
            return i+1;
        }
        i++;
    }
	SYS_ASSERT(i);
    return 0;
}

void NG_FXLoadList()
{
	FILE *in;	   
    in = FIO_cur->fopen(".\\anim\\anim.lst", "rb");
	SYS_ASSERT(in);
    g_pszAnimList = array_loadtext(in, 32, -1);
    FIO_cur->fclose(in);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void NG_FXAvary(V3XOVI *mOVI, SGScript *pInf)
{
    int32_t i;
    V3XSCALAR dii, sz=(20*(mOVI->ORI->global_rayon+1)), per;
    V3XOVI *OVI;
    V3XORI *ORI;
    SGScript        *Sif;
    V3XVECTOR dist, *pos=&mOVI->mesh->matrix.v.Pos;
    sz = sz * sz;
    for (i=g_SGGame.Scene->numOVI, OVI=g_SGGame.Scene->OVI;i!=0;OVI++, i--)
    {
        if ((OVI->data)&&(((OVI->state&V3XSTATE_HIDDEN)==0))&&(mOVI!=OVI))
        {
            Sif = (SGScript*)OVI->data;
            if ((Sif->Type!=t_DECOR)&&(Sif->Type!=t_SPECIAL))
            {
                V3XVector_Dif(&dist, &OVI->mesh->matrix.v.Pos, pos);
                dii = V3XVector_LengthSq(&dist);
                if ((dii<sz)&&(sz))
                {
                    V3XCL *Cs;
                    ORI = OVI->ORI;
                    Cs = ORI->Cs;
                    per = CST_ONE - DIVF32(dii, sz);
                    if (per<0.) per=0.;
                    if (per>1.) per=1.;
                    if ((Cs)&&((Cs->flags&V3XCMODE_DONTMOVE)))
                    {
                        V3XVector_Madd(&Cs->velocity,
                        &dist, 
                        &Cs->velocity, 
                        per);
                    }
                    dii = MULF32((V3XSCALAR)pInf->Avary, per);
                    if (dii>=Sif->Shield) dii = (V3XSCALAR)Sif->Shield;
                    switch(Sif->Type) {
                        case t_PLAYER:
                        if (OVI==g_pPlayer->J.OVI)
                        {
                            if (!(g_pPlayer->mode&GODMODE))
                            {
                                if (dii>Sif->Shield) dii = (V3XSCALAR)Sif->Shield;
                                Sif->Shield-=(int32_t)dii;
                                g_pPlayer->Notify+=SGNET_SHIELDCHANGE;
                                g_SGGame.FlashColor.r=255;g_SGGame.FlashColor.g=0;g_SGGame.FlashColor.b=0;
                                g_SGGame.FlashAlpha = (short)xADJUSTFIX(per, 5);
                                if (g_SGObjects.FinCode!=GAMESTATE_DEAD)
                                {
                                    g_SGSettings.ComNumber=COM_ShipAttacked;g_SGSettings.ComTime=MAX_COM_DELAY;
                                }
                                NG_FXImpact((int)xADJUSTFIX(per, 4));
                            }
                        }
                        break;
                        case t_ENEMY:
                        case t_PIRAT:
                        case t_FRIEND:
                        case t_OBSTACLE:
                        case t_NEUTRAL:
                        Sif->Shield-=(int32_t)dii;
                        //if (!Sif->Shield) Sif->Shield=1;
                        break;
                    }
                }
            }
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_FXDebris(SGScript *pInf, V3XOVI *OVI)
*
* DESCRIPTION :
*
*/
void NG_FXDebris(SGScript *pInf, V3XOVI *OVI)
{
    int i, x;
    SGEffect *p;
    V3XMESH *obj = OVI->mesh;
    V3XVECTOR v;
    i = (int32_t)((xSHRD(OVI->ORI->global_rayon, 3))+2); 
    if (i>MAX_DEBRIS) 
		i=MAX_DEBRIS;
    for (;i!=0;i--)
    {
        x = sysRand(obj->numVerts);
        V3XVector_ApplyMatrixTrans(v, obj->vertex[x], obj->matrix.Matrix);
        p = NG_FXNew(&v, FX_DEBRIS, 8, pInf, 1, OVI);
        if (p)
        {
            p->Step[1] = p->Step[0]= sysRand(p->Sprite->maxItem);
            p->Size = OVI->ORI->global_rayon / 8;
            if (p->Size < 256*g_SGSettings.WorldUnit) 
				p->Size = 256*g_SGSettings.WorldUnit;
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_FXBlast(SGScript *pInf, V3XOVI *OVI)
*
* DESCRIPTION :
*
*/
void NG_FXBlast(SGScript *pInf, V3XOVI *OVI)
{
    int i, x, f=0;
    SGEffect *p;
    V3XMESH *obj = OVI->mesh;
    V3XVECTOR v;
    i = (int)(xSHRD(OVI->ORI->global_rayon, 3)+2); 
	if (i>MAX_BLASTS) 
		i=MAX_BLASTS;
    for (;i!=0;i--)
    {
        x = sysRand(obj->numVerts);
        V3XVector_ApplyMatrixTrans(v, obj->vertex[x], obj->matrix.Matrix);
        if (!f) 
			v = obj->matrix.v.Pos;
        p=  NG_FXNew(&v, FX_IMPACT, 4, pInf, 1, OVI);
        if (p)
        {
            if (!f)
            {
                p->Sphere = NG_GetFreeSphere();
                p->radius = CST_ZERO;
                f = 1;
            }
            p->Step[1] = p->Step[0] = sysRand(p->Sprite->maxItem);
            p->Size = OVI->ORI->global_rayon / 2;
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  SGEffect *NG_FXNew(V3XVECTOR *pos, int type, int lop, SGScript *pInf, int kp, V3XOVI *OVI)
*
* DESCRIPTION :
*
*/
SGEffect *NG_FXNew(V3XVECTOR *pos, int type, int lop, SGScript *pInf, int kp, V3XOVI *OVI)
{
   	int bAllowTrsp = 1; // (GX.View.Flags&GX_CAPS_BACKBUFFERINVIDEO)==0;
    SGEffect *p, *EJ=NULL;
    int i, f=0;
    for (p=g_SGGame.pExpl, i=g_SGGame.numEffects;(i>0) && (f!=1);p++, i--)
    {
        if (p->Dispo)
        {
            int transpa= 0;
            OVI->state|=V3XSTATE_MATRIXUPDATE;
            p->Step[0] = p->Step[1] = 0;
            p->Nloop = lop;
            p->Sphere = NULL;
            p->type = type;
            p->OVI = OVI;
            p->Size = OVI->ORI->global_rayon;
            p->Dispo = 0;
            p->LastFrame = V3X.Time.ms;
            p->PlaySample = 1;
            if (kp)
            {
                p->keep = *pos;
                if (kp==1)
                {
                    p->useKeep = 1;
                }
                else
                {
                    p->useKeep = 0;
                    p->Size*=(float)(1<<(kp-1));
                }
                p->pos = &p->keep;
            }  
			else 
				p->pos = pos;
            EJ = p;
            f = 1;
            switch(p->type) 
			{
                case FX_FINISH:
					p->FxCode = pInf->StartSound-1;
					p->Sprite = g_pspFX[pInf->FinisHim-1];
					SYS_ASSERT(p->Sprite);
					p->Size*=2;
					p->FadeStart = p->Sprite->maxItem;
					transpa = 1;
				break;
                case FX_SMOKE:
					p->angle.x = 0;
					p->angle.y = sysRand(4096);
					p->angle.z = sysRand(4096);
					{
						V3XMESH *obj = OVI->mesh;
						int x = sysRand(obj->numVerts);
						V3XVector_ApplyMatrixTrans(p->cent, obj->vertex[x], obj->matrix.Matrix);
					}
                case FX_SMOKE2:
					p->Sprite = g_pspFX[0];
					SYS_ASSERT(p->Sprite);
					p->FxCode = -1;
					p->FadeStart = 0;
					transpa = 1;
                break;
                case FX_CUSTOM:
					NG_FXAvary(OVI, pInf);
					transpa = 1;
                case FX_IMPACT:
					if (!pInf->ExplodeMode)
					{
						p->Dispo = 1;
						EJ = NULL;
						transpa = 0;
      					p->FadeStart = p->Sprite->maxItem;
					}
					else
					{
						p->Sprite = g_pspFX[pInf->ExplodeMode-1];
						SYS_ASSERT(p->Sprite);
						p->FxCode = pInf->SoundExplode-1;
      					p->FadeStart = p->Sprite->maxItem/2;
						p->Size/= 2;
						transpa = 1;
						p->Light = 128;
					}
                break;
                case FX_WARPIN:
					p->FxCode = pInf->SoundAppear-1;
					p->Sprite = g_pspFX[pInf->AppearMode-1];
					SYS_ASSERT(p->Sprite);
					if (p->OVI->state&V3XSTATE_HIDDEN) 
						p->Size*=4;
					p->OVI->state^=V3XSTATE_HIDDEN;
					p->Light = 255;
					p->FadeStart = p->Sprite->maxItem;
					transpa = 1;
                break;
                case FX_DEBRIS:
					p->FxCode = -1;
					p->Sprite = g_pspFX[((pInf->DebrisMode2)&&(i&1)) ? pInf->DebrisMode2-1 : pInf->DebrisMode-1];
					SYS_ASSERT(p->Sprite);
					{
						V3XMESH *obj = OVI->mesh;
						int x = sysRand(obj->numVerts)-1;
						V3XVector_ApplyMatrixTrans(p->cent, obj->vertex[x], obj->matrix.Matrix);
					}
					p->angle.x = 0;
					p->angle.y = sysRand(4096);
					p->angle.z = sysRand(4096);
					transpa = 0;
					p->FadeStart = p->Sprite->maxItem/2;
                break;
                default:
                break;
			}

			if ((transpa)&&(g_SGSettings.VisualsFx>1)&&bAllowTrsp)
			{
				p->material.info.Transparency = (V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD)
					                     ? (transpa==2 ? V3XBLENDMODE_SUB: V3XBLENDMODE_ADD)
										 : V3XBLENDMODE_ALPHA;
				p->material.Render = V3XRCLASS_bitmap_transp;				
				p->material.RenderID = V3XID_T_SPRITE+p->material.info.Transparency;
				p->material.render_near = p->material.render_far =
				p->material.render_clip  = V3XRENDER_SpriteAny;
			}
			else
			{
				p->material.info.Transparency = V3XBLENDMODE_NONE;
				p->material.Render = V3XRCLASS_bitmap;
				p->material.RenderID = V3XID_SPRITE;
				p->material.render_near = p->material.render_far =
				p->material.render_clip = V3XRENDER_SpriteAny;
            }

            if (GX.View.BytePerPixel>1)
			{
				SYS_ASSERT(p->Sprite);
			}
        }
    }
    return EJ;
}

void NG_FXChangeRenderObject(V3XMESH *obj, int nouv)
{
    int i;
    for (i=0;i<obj->numMaterial;i++)
     obj->material[i].Render=nouv;
    return;
}

void NG_ChangeGameDetail(void)
{
    int i;
    V3XMESH *obj;
    V3X_GXTexPrimitives *Gs = NULL;
    if (V3X.Client->primitive)
		Gs = V3X.Client->primitive->Linear256x256x8b;
    if (g_SGObjects.Sky)
    {
        obj = g_SGObjects.Sky->mesh;  
        for (i=0;i<obj->numMaterial;i++)
        {
            V3XMATERIAL *mat = obj->material+i;
            mat->diffuse.r =  mat->diffuse.g =  mat->diffuse.b = 255;
            mat->info.Shade = 0;
            switch(g_SGSettings.Sky) 
			{
                case 2:
                if (Gs) mat->render_far = Gs->tex;
                g_SGGame.Scene->Layer.bg.flags &=~ V3XBG_COLOR|V3XBG_BLACK;
                mat->Render = V3XRCLASS_normal_mapping;
                V3XMaterial_Register(mat);
                V3X.Client->Capabilities|=GXSPEC_FORCEHWPERSPECTIVE;
                break;               
				default:
                g_SGGame.Scene->Layer.bg.flags &=~ V3XBG_COLOR|V3XBG_BLACK;
                mat->Render = V3XRCLASS_normal_mapping;
                V3XMaterial_Register(mat);
                if (Gs) mat->render_far = Gs->tex_rough;
                V3X.Client->Capabilities&=~GXSPEC_FORCEHWPERSPECTIVE;
                break;

            }
        }
    }
    return;
}


void NG_FXSetSceneShading(V3XSCENE *Scene, int limit)
{
    int i, j, k;
    V3XORI    *ORI;
    V3XOVI    *OVI;
    V3XMESH *obj;
    
    for (i=0, ORI=Scene->ORI;i<Scene->numORI;i++, ORI++)
    {
        if (ORI->type==V3XOBJ_MESH)
        {
            V3XMATERIAL *Mat;
            obj = ORI->mesh;
            for (Mat=obj->material, j=0;j<obj->numMaterial;j++, Mat++)
            {
                k = Mat->info.Shade;
                if (k)
                {
                   	Mat->info.Shade = max(k, limit);
		            V3XMaterial_Register(Mat);
                }
        
				if (Mat->info.Shade==2) 
					obj->flags&=~V3XMESH_FLATSHADE;

				
            }
        }
    }
    if (limit>1)
	{
		for (i=0, OVI=Scene->OVI;i<Scene->numOVI;i++, OVI++)
		{
			ORI = OVI->ORI;
			if (ORI->type==V3XOBJ_MESH)
			{
				OVI->mesh->flags&=~V3XMESH_FLATSHADE;
			}
		}
	}
    return;
}

