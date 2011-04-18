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
#include "gui_os.h"
#include "systime.h"
#include "iss_defs.h"
//
#include "lt_struc.h"
#include "lt_data.h"
#include "lt_func.h"

#ifndef __amigaos__
#define USE_MUTEX
extern SYS_MUTEX m_Mutex;
#endif

#define FIX100TO255(x) ((int32_t)x*255)/300

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void NG_StarfieldCreate(void)
{
    int i;
    Starfield.maxStars = 300;
    Starfield.Zclip = 1000.f;
    Starfield.Mx = (Starfield.maxStars/2);
    Starfield.Stars = MM_CALLOC(Starfield.maxStars, V3XVECTOR);
    for (i=0;i<2;i++)
    Starfield.Prj[i] = MM_CALLOC(Starfield.maxStars, V3XVECTOR);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_StarfieldRelease(void)
*
* DESCRIPTION :
*
*/
void NG_StarfieldRelease(void)
{
    int i;
    MM_heap.free(Starfield.Stars);
    for (i=0;i<2;i++) MM_heap.free(Starfield.Prj[i]);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_ResizeInterface(RW_Interface *Interf, int x, int y, int halfY)
*
* DESCRIPTION :
*
*/

static int SCALE_X(int x)
{
	return x * GX.View.lWidth / 640;
}

static int SCALE_Y(int y)
{
	return y * GX.View.lHeight / 480;
}

static int CENTER_X(int x)
{
	return (GX.View.lWidth - x) / 2;
}

static int CENTER_Y(int y)
{
	return (GX.View.lHeight -y) / 2;
}

void NG_ResizeInterface(RW_Interface *Interf)
{
    int32_t i;
    RW_Button but, *b;
    // Resize les zones 'Peres'
    for (b=Interf->item, i=0;i<Interf->numItem;b++, i++)
    {
        but = *b;
        if (b->parent==NULL) // Uniquement les 'peres'
        {
            b->X = (short) SCALE_X(but.X);
            b->Y = (short) SCALE_Y(but.Y);

			if(b->Y + but.LY > GX.View.ymax)
				b->Y = (short)(GX.View.ymax - but.LY);

            if(b->Y < GX.View.ymin)
				b->Y = (short)GX.View.ymin;

            switch(i)
			{
                case 0:
					b->X = (short)CENTER_X(but.LX);
					b->Y = (short)CENTER_Y(but.LY);;
                break;
                case 3:
                case 5:
					b->X = (short)(GX.View.xmax - but.LX);
                break;
                case 4:
                if (g_pCurrentGame->ship!=0)
					b->X = (short)(GX.View.xmax - but.LX);
                break;
            }
        }
    }
    // Recalcule la position des zones 'item'
    for (b=Interf->item, i=0;i<Interf->numItem;b++, i++)
    {
        if (b->parent) // Uniquement les 'item' (qui ont un parent)
        {
            b->X=b->parent->X+b->X;
            b->Y=b->parent->Y+b->Y;
        }
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/

char **g_szLanguageList;

void NG_SetLanguage(int l)
{
    int i = 1;
	char tex[256];
	FILE *in;
	sprintf(tex, ".\\voix\\%s.msg", g_szLanguageList[l]);
	in = FIO_cur->fopen(tex, "rt");
	SYS_ASSERT(in);

	if (in)
	{
		do
		{
			char *v, szTrailer[]={13, 0};
			char *s;
			FIO_cur->fgets(tex, 64, in);
			s = strstr(tex, ",");
			SYS_ASSERT(s);
			sysStrnCpy(g_szGmT[i], s + 1, 31);
			v = strstr(g_szGmT[i], szTrailer);
			if (v)
				*v=0;
			i++;
			SYS_ASSERT(i<MAX_TEXT_MENUS);
		}while(strstr(tex, "@")==NULL);
		FIO_cur->fclose(in);
	}
	sysConPrint("Region set is %s", g_szLanguageList[g_SGSettings.Language]);
	g_SGSettings.LangCode = g_szLanguageList[g_SGSettings.Language][0];
}

void NG_ReadLanguagePack(void)
{

    SYS_FILEHANDLE in;
    FIO_cur = g_pGameIO;

    in = FIO_cur->fopen(".\\voix\\language.msg", "rt");
	SYS_ASSERT(in);
    g_szLanguageList = array_loadtext(in, 16, -1);
    FIO_cur->fclose(in);

	NG_SetLanguage(g_SGSettings.Language);

    FIO_cur = g_pGameIO;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_StretchFont(GXSPRITEGROUP *pSpg, int fx, int fy)
*
* DESCRIPTION :
*
*/
void NG_StretchFont(GXSPRITEGROUP *pSpg, int fx, int fy)
{
    GXSPRITE *sp=pSpg->item;
    int i;
    for (i=pSpg->maxItem;i!=0;i--, sp++)
    {
        CSP_Resize(sp, (sp->LX*fx)>>8, (sp->LY*fy)>>8, GX.View.BytePerPixel);
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_LoadBackground(char *file, GXSPRITE *sp)
*
* DESCRIPTION :
*
*/
void NG_LoadBackground(char *szFilename, GXSPRITE *sp)
{
    int bpp = IMG_LoadFn(szFilename, sp);
	GX.Client->UploadSprite(sp, GX.ColorTable, bpp>>3);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  GXSPRITEGROUP *NG_LoadSpriteGroup(char *filename, int resize)
*
* DESCRIPTION :
*
*/
GXSPRITEGROUP *NG_LoadSpriteGroup(char *filename, int resize)
{
    char tx[256];
    GXSPRITEGROUP *pSpriteGroup;
    GXSPRITE *sp;
    int i;
    if (resize&128)
    {
        sprintf(tx, ".\\brief\\%s", filename);
        resize&=127;
    }
    else
    if (resize&64)
    {
        sprintf(tx, ".\\int\\%s", filename);
        resize&=63;
    }
    else
    {
        sprintf(tx, ".\\menu\\%s", filename);
    }

	if (V3X.Client->Capabilities&GXSPEC_SPRITEAREPOLY)
	{
		pSpriteGroup = CSPG_GetFn(tx, FIO_cur, CSPLOAD_SURFACE);
		SYS_ASSERT(pSpriteGroup);
		return pSpriteGroup;
	}

    pSpriteGroup = CSPG_GetFn(tx, FIO_cur, CSPLOAD_POSTERIZE|CSPLOAD_SURFACE);
	SYS_ASSERT(pSpriteGroup);

    if (((GX.View.xmax!=639)||(GX.View.ymax!=479))&&(resize))
    {
        int32_t dp = GX.View.BytePerPixel;
        for (sp=pSpriteGroup->item, i=0;i<pSpriteGroup->maxItem;i++, sp++)
        {
            switch(resize) {
                case 1:
                CSP_Resize(sp, (sp->LX*GX.View.lWidth)/640, (sp->LY*GX.View.lHeight)/480, dp);
                break;
                case 2:
                CSP_Resize(sp, sp->LX, (sp->LY>>1)+(sp->LY&1), dp);
                break;
            }
        }
    }
    return pSpriteGroup;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static void NG_ResetGameData(void)
{
    g_SGGame.numEnemies = 0;
    g_SGGame.numWeapons = 0;
    sysMemZero(g_SGGame.pWea, sizeof(SGWeapon) * MAX_WEAPONS);
    sysMemZero(g_SGGame.pPlayer, sizeof(SGPlayer) * MAX_NETWORK_PLAYER);
    sysMemZero(g_SGGame.pEnemy , sizeof(SGActor) * MAX_ENEMYS);
}

static void NG_AllocGameData(void)
{
    g_pPlayerInfo = (SGScript*) MM_heap.malloc(sizeof(SGScript)*MAX_PLAYER);
    g_SGGame.numEffects = 128;
    g_SGGame.pWea = MM_CALLOC(MAX_WEAPONS, SGWeapon);
    g_SGGame.pPlayer = MM_CALLOC(MAX_NETWORK_PLAYER, SGPlayer);
    g_SGGame.pEnemy = MM_CALLOC(MAX_ENEMYS, SGActor);
    g_SGGame.pExpl= MM_CALLOC(g_SGGame.numEffects, SGEffect);
}

static void NG_FreeGameData()
{
	MM_heap.free(g_pPlayerInfo);
	MM_heap.free(g_SGGame.pExpl);
	MM_heap.free(g_SGGame.pEnemy);
	MM_heap.free(g_SGGame.pPlayer);
	MM_heap.free(g_SGGame.pWea);

	g_SGGame.pExpl = 0;
	g_SGGame.pEnemy = 0;
	g_SGGame.pPlayer = 0;
	g_SGGame.pWea = 0;

}

void NG_ReleaseFonts()
{
	int x = MM_heap.active;
    MM_heap.active = 0;

    CSPG_Release(g_pspMsg);
    CSPG_Release(g_pFont);
    CSPG_Release(g_pSmallFont);
	CSPG_Release(g_pFontMenuLrg);
	CSPG_Release(g_pFontMenuSml);
    CSPG_Release(g_pspHud2);
    CSPG_Release(g_pspCat);

    g_pspMsg = NULL;
    g_pFont = NULL;
    g_pSmallFont = NULL;
    g_pFontMenuLrg = NULL;
    g_pFontMenuSml = NULL;

	MM_heap.active = x;
}

void NG_LoadFonts()
{
    int x = MM_heap.active;
    MM_heap.active = 0;

	g_pFontMenuLrg = NG_LoadSpriteGroup("lit_mn02", 1);
	g_pFontMenuLrg->HSpacing=0;

	g_pFontMenuSml = NG_LoadSpriteGroup("lit_mn03", 1);
	g_pFontMenuSml->HSpacing=0;

	g_pspMsg = NG_LoadSpriteGroup("mesg", 0);
    g_pFont = NG_LoadSpriteGroup("ffonte", 0);
	g_pFont->HSpacing=1;

    g_pSmallFont = NG_LoadSpriteGroup("little", 0);
	g_pSmallFont->HSpacing=1;
	g_pSmallFont->VSpacing = 1;

    g_pspHud2 = NG_LoadSpriteGroup("vx_sprit", 0);
	g_pspHud2->HSpacing = 1;

    g_pspCat = NG_LoadSpriteGroup("sheba", 0);
	g_pspCat->HSpacing = 1;

	sysConSetFont(g_pFont);

    MM_heap.active = x;
}

void GX_drawGouraudRect(int32_t x, int32_t y, int32_t dx, int32_t dx2, int32_t dy, rgb32_t *c, rgb32_t *d)
{
    int i=0;
    for (i=0;i<dx2;i++)
    {
        rgb32_t e;
        uint32_t cl;
        e.r = (uint8_t)(((int32_t)(d->r - c->r)*i) / dx);
        e.g = (uint8_t)(((int32_t)(d->g - c->g)*i) / dx);
        e.b = (uint8_t)(((int32_t)(d->b - c->b)*i) / dx);
        cl = GX.View.BytePerPixel>1 ? RGB_PixelFormatEx((rgb24_t*)&e) : 0;
        GX.gi.drawVerticalLine(i + x, y, dy, cl);
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_DrawLoadingBar(int step)
*
* Description :
*
*/
void NG_DrawLoadingBar(int step)
{
    int lw = (7*GX.View.lWidth)/8, x = (GX.View.lWidth-lw)/2;
    int p = (lw * step)/100;
    rgb32_t cs ={0, 0, 0};
    rgb32_t cf ={180, 0, 0};

	if ((V3XA.State & 1))
	{
#ifdef USE_MUTEX
		mutex_lock(&m_Mutex);
#endif
		V3XAStream_PollAll();
#ifdef USE_MUTEX
		mutex_unlock(&m_Mutex);
#endif
	}

    GX.Client->Lock();
	GX.csp.zoom_pset(&g_csPicture,0,0,GX.View.lWidth, GX.View.lHeight);
	NG_DrawLoadingScreen();
    GX.gi.drawWiredRect(x, GX.View.ymax-10, x+lw, GX.View.ymax, 0);
    GX_drawGouraudRect(x, GX.View.ymax-10, lw, p, 10, &cs, &cf);
    GX.Client->Unlock();
    GX.View.Flip();
}

static void NG_DrawExtra(void)
{
    V3XOVI *OVI = g_SGGame.Scene->OVI;
    int i=g_SGGame.Scene->numOVI;
    for ( ;  i!=0;  OVI++, i--)
    {
        int j;
        V3XORI *ORI = OVI->ORI;
        if (ORI->type == V3XOBJ_LIGHT)
        {
            OVI->light->intensity = 255.;
            OVI->light->type = V3XLIGHTTYPE_OMNI;
            OVI->light->flags = V3XLIGHTCAPS_LENZFLARE;
            RGB_Set(OVI->light->color, 255, 255, 255);
            OVI->light->range = V3X.Light.fogDistance;
        }
        if (ORI->type == V3XOBJ_MESH)
        {
            V3XMESH     *mesh = OVI->mesh;
			V3XMATERIAL *Mat = mesh->material;
			int p=0;

		    for (j=0;j<mesh->numMaterial;j++, Mat++)
		    {
                Mat->scale = 255;
                if (sysStriCmp(Mat->mat_name, "REACTEURS")==0)
                {
                    if (OVI==g_pPlayer->J.OVI)
                    {
                        g_SGGame.pReactorMaterial = Mat;
                    }
                }
                if (!Mat->info.Shade)
                {
                    RGB_Set(Mat->diffuse, 255, 255, 255);
                }

                if (Mat->info.Transparency)
                {
                    Mat->info.Transparency = (V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? V3XBLENDMODE_ADD : V3XBLENDMODE_ALPHA;
                    Mat->alpha  = (V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? 200 : 128;
                }

                if (((V3X.Setup.flags&V3XOPTION_TRUECOLOR))&&(g_SGSettings.VisualsFx>1)) // Ameliorations moteur 3D
                {
                    if (sysStriCmp(Mat->mat_name, "ASTER01")==0)
                    {
                        Mat->RenderID = V3XID_TEX_GOURAUD;
                        Mat->info.Shade = 2;
                        mesh->flags |= V3XMESH_HASDYNLIGHT;
                        p=1;
                    }

                    if (V3X.Client->Capabilities&(GXSPEC_HARDWAREBLENDING|GXSPEC_OPACITYTRANSPARENT))
                    {
                        if ((sysStriCmp(Mat->mat_name, "TIR1")==0))
                        {
                            Mat->info.Transparency = (V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? V3XBLENDMODE_ADD : V3XBLENDMODE_ALPHA;
                            Mat->diffuse.r =
                            Mat->diffuse.g =
                            Mat->diffuse.b = 255;
                            //&&(GX.View.BytePerPixel<4)
                            Mat->alpha  = 128;//(V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? 200 : 128;
                            Mat->RenderID = V3XID_T_TEX;
                            Mat->Render = V3XRCLASS_transp_mapping;
                        }
                        else
                        if (sysStriCmp(Mat->mat_name, "WARP")==0)
                        {
                            Mat->info.Transparency = (V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? V3XBLENDMODE_ADD : V3XBLENDMODE_ALPHA;
                            Mat->alpha  = (V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? 200 : 128;
                            Mat->Render = V3XRCLASS_bitmap_transp;
                            Mat->RenderID = V3XID_T_SPRITE+Mat->info.Transparency;
                        }
                        else
                        if (sysStriCmp(Mat->mat_name, "REACTEURS")==0)
                        {
                            Mat->info.Transparency = (V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? V3XBLENDMODE_ADD : V3XBLENDMODE_ALPHA;
                            Mat->info.TwoSide = TRUE;
                            Mat->alpha  = (V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? 200 : 128;
                        }
                        else
                        if (sysStriCmp(Mat->mat_name, "LIGHT2")==0)
                        {
                            Mat->diffuse.r = i*8;
                            Mat->diffuse.g = i*8;
                            Mat->diffuse.b = i*8;
                        }
                        else
                        if (strstr(Mat->mat_name, "TUROK"))
                        {
                            Mat->info.Transparency = (V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? V3XBLENDMODE_ADD : V3XBLENDMODE_ALPHA;
                            Mat->alpha = (V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? 200 : 128;
                            Mat->info.TwoSide = TRUE;
                            Mat->scale = 128;
                        }
                        else
                        if (Mat->info.Transparency)
                        {
                            Mat->info.Transparency = (V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? V3XBLENDMODE_ADD : V3XBLENDMODE_ALPHA;
                            Mat->alpha  = (V3X.Client->Capabilities&GXSPEC_ALPHABLENDING_ADD) ? 200 : 128;

                        }
                        if (Mat->info.Sprite)
							Mat->info.Opacity = 1;
                    }
					else
					{
						SYS_ASSERT(0);
					}

					if (Mat->info.Transparency)
						Mat->diffuse.r = Mat->diffuse.g = Mat->diffuse.b = 240;

					if (g_SGSettings.VisualsFx==2)
						Mat->info.Transparency=0;

					if ((Mat->info.Transparency)&&(Mat->info.Sprite))
						Mat->RenderID=V3XID_T_SPRITE + Mat->info.Transparency;

                    V3XMaterial_Register(Mat);
                }
                else
                {
                    if (strstr(Mat->mat_name, "TUROK"))
                    {
                        Mat->RenderID = V3XRCLASS_shadow;
                        Mat->info.Transparency = 0;
						Mat->info.Shade = 0;
						Mat->info.Environment = 0;
                        Mat->render_near = Mat->render_far = Mat->render_clip = V3XRENDER_Null;
                    }
                }
            }
            if (p)
            {
                V3XPOLY *fe = mesh->face;
                for (j=0;j<mesh->numFaces;j++, fe++)
                {
                    Mat = (V3XMATERIAL*)fe->Mat;
                    if (p==1)
                    {
                        if (!fe->shade)
                        fe->shade = MM_CALLOC(fe->numEdges, V3XSCALAR);
                        mesh->flags|=V3XMESH_HASDYNLIGHT;
                    }
                }
            }
        }
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_FixTrackAnim(V3XSCENE *Scene)
*
* Description :
*
*/
static void NG_FixTrackAnim(V3XSCENE *Scene)
{
    int i;
    V3XTRI *TRI = Scene->TRI;
    for (i=0;i<Scene->numTRI;i++, TRI++)
    {
        if (TRI->numFrames>2)
        {
            int f = TRI->numFrames - 1;
            V3XVECTOR v;
            V3XVector_Dif(&v, &TRI->keys[f-1].vinfo.pos, &TRI->keys[f-2].vinfo.pos);
            if (V3XVector_DistanceSq(&TRI->keys[f].vinfo.pos, &TRI->keys[f-1].vinfo.pos)==CST_ZERO)
            {
                V3XVector_Sum( &TRI->keys[f].vinfo.pos, &TRI->keys[f-1].vinfo.pos, &v);
            }
        }
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_InitGameScene(void)
*
* DESCRIPTION :
*
*/
static void NG_InitGameScene(void)
{
    char pth[32];
    SYS_WAD *old = filewad_getcurrent();
    rgb32_t AmbientLevel[]={
        {FIX100TO255(72), FIX100TO255(0), FIX100TO255(0)},
        {FIX100TO255(25), FIX100TO255(25), FIX100TO255(33)},
        {FIX100TO255(55), FIX100TO255(33), FIX100TO255(47)},
        {FIX100TO255(72), FIX100TO255(27), FIX100TO255(0)},
    {FIX100TO255(0), FIX100TO255(36), FIX100TO255(59)}};

	NG_FXLoadData();

    sprintf(pth, ".\\%s\\", g_SGObjects.World.path_name+2);
    if (g_SGSettings.AddOn)
    {
        filewad_setcurrent(g_SGGame.AddOnResource);
    }
    else
    {
        filewad_chdir(FIO_wad, pth);
    }
    sysStrExtChg(g_SGObjects.World.scene_name, g_SGObjects.World.scene_name, "vmx");
    g_SGGame.Scene = V3XScene_GetFromFile(g_SGObjects.World.scene_name);

    if (g_SGSettings.AddOn)
    {
        if (g_SGSettings.AddOn) filewad_close(g_SGGame.AddOnResource);
        filewad_setcurrent(old);
        filewad_chdir(FIO_wad, pth);
    }
    // Table Realcolor
    if (GX.View.BytePerPixel==1)
    {
        sysStrCpy(g_SGGame.Scene->Layer.lt.gouraud.filename, "!gouraud");
        sysStrCpy(g_SGGame.Scene->Layer.lt.alpha50.filename, "!glenz");
        sysStrCpy(g_SGGame.Scene->Layer.lt.additive.filename, "!glenz2");
        sysStrCpy(g_SGGame.Scene->Layer.lt.blur.filename, "!blur");
    }
    // Couleur
    V3X.Setup.flags|=V3XOPTION_COLLISION|V3XOPTION_USESAMELUT;
    g_SGGame.Scene->Layer.lt.shift=4;

	// Customisation NG
    if (GX.View.BitsPerPixel>8)
		V3X.Setup.flags|=V3XOPTION_AMBIANT;

	V3X.Light.ambiant = V3X.Light.ambiantMaterial = AmbientLevel[g_pCurrentGame->episode>4 ? 0 : g_pCurrentGame->episode];

    V3XScene_LoadTextures(g_SGGame.Scene, NULL);
	sysMemCpy(GX.ColorTable, g_SGGame.Scene->Layer.lt.palette.lut, 768);

	g_SGGame.CI_COL2 = RGB_convert(212, GX.ColorTable);
    g_SGGame.CI_BLACK = RGB_convert(0, GX.ColorTable);
    g_SGGame.CI_YELLOW = RGB_convert(207, GX.ColorTable);
    g_SGGame.CI_RED = RGB_convert(216, GX.ColorTable);
    g_SGGame.CI_WHITE = GX.View.BytePerPixel>1 ? (GX.View.BytePerPixel > 1 ? RGBA_PixelFormat(255, 255, 255, 0) : 255) : RGB_convert(211, GX.ColorTable);
    g_SGGame.CI_GREEN = RGB_convert(210, GX.ColorTable);
    g_SGGame.CI_BLUE = RGB_convert(195, GX.ColorTable);
    g_SGGame.CI_BLUELIGHT = RGB_convert(192, GX.ColorTable);
    g_SGGame.pColorRadar[0] = g_SGGame.CI_WHITE;
    g_SGGame.pColorRadar[1] = g_SGGame.CI_BLUE;//RGB_convert(213, GX.ColorTable);
    g_SGGame.pColorRadar[2] = g_SGGame.CI_RED;
    g_SGGame.pColorRadar[3] = g_SGGame.CI_YELLOW;
    g_SGGame.pColorRadar[4] = RGB_convert(220, GX.ColorTable);
    g_SGGame.pColorRadar[5] = (GX.View.BytePerPixel > 1 ? RGBA_PixelFormat(255, 255, 255, 0) : 255);
    g_SGGame.pColorRadar[6] = g_SGGame.CI_BLUELIGHT;
    g_SGGame.pColorRadar[7] = g_SGGame.CI_COL2;

    V3XScene_Verify(g_SGGame.Scene);
	NG_FXSetSceneShading(g_SGGame.Scene, 2);

    NG_FixTrackAnim(g_SGGame.Scene);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static void NG_CMXToObject(void)
{
    int32_t i, j, f, Coliable, numPlayer=0;
    SGActor *Jf=g_SGGame.pEnemy;
    char name[32];
    SGWeapon *W;
    char *s;
    V3XOVI *OVI;
    V3XORI    *ORI;
    SGScript *Sif = g_pPlayerInfo, *Sf;
    filewad_chdir(FIO_wad, "");
    for (i=0;i<8;i++)
		g_SGGame.MaxAim[i] = g_SGGame.MaxProtect[i] = 0;
    g_SGGame.numWeapons = 0;
    g_SGGame.numEnemies = 0;
    g_cGameStat.total_nmy = 0;
    g_cGameStat.newlevel = 0;
    g_cGameStat.total_amy = 0;
    g_cGameStat.killed_nmy = 0;
    g_cGameStat.killed_amy = 0;
    g_cGameStat.total_bonus = 0;
    for (i=g_SGGame.Scene->numOVI, OVI=g_SGGame.Scene->OVI;i!=0;OVI++, i--)
    {
        ORI = OVI->ORI;
		OVI->mesh->scale = 1.f;
        if ((ORI->name[0])&&(ORI->type==V3XOBJ_MESH))
        {
            sysStrnCpy(name, ORI->name, 31);
			s = strstr(name, "_");
#ifdef _DEBUG
		//	SYS_Debug("%s\n", name);
#endif
            if (s)
            {
                f=0;
                *s=0;
                Sf=NULL;
                Coliable=0;
                for (Sif=g_pPlayerInfo, j=0;(j<g_SGSettings.maxCase)&&(!Sf);j++, Sif++)
                {
#ifdef _DEBUG
//					SYS_Debug("Compare with %s vs %s\n", name, Sif->Basename);
#endif

                    if (sysStriCmp(name, Sif->Basename)==0)
                    {
                        f = 1;
                        Sf = Sif;
                        if (Sf->NeverHide==1)
							OVI->state|=V3XSTATE_CULLNEVER;
                        if (Sf->NeverHide==2)
							OVI->state|=V3XSTATE_CULLNEVER;
                        if  (Sif->CollisionStyle==t_CS_BONUS)
							g_cGameStat.total_bonus++;
                        if ((Sif->CollisionStyle==t_CS_BUMP)||  (Sif->CollisionStyle==t_CS_BUMP_LOSE_SHIELD))
							Coliable = 2;
						else
							Coliable =1;
                        g_SGGame.MaxAim[NG_ColorToNAV(ORI->index_color)]+=Sif->Mission+Sif->Protect;
                        g_SGGame.MaxProtect[NG_ColorToNAV(ORI->index_color)]+=Sif->Protect;
						SYS_ASSERT(g_SGGame.numWeapons<MAX_WEAPONS);
						SYS_ASSERT(g_SGGame.numEnemies<MAX_ENEMYS);

#ifdef _DEBUG
//						SYS_Debug("Type = %d\n", Sif->Type);
#endif
                        switch(Sif->Type)
						{
                            case t_SPECIAL:
                            Jf->pInf = *Sif;
                            Jf->OVI = OVI;
                            OVI->data = Jf;
                            if (Sif->Code==0)
							{
								OVI->state |= V3XSTATE_HIDDEN;

							}
                            g_SGGame.numEnemies++; Jf++;
                            break;
                            case t_ENEMY:
                            case t_PIRAT:
                            case t_OBSTACLE:
                            case t_FRIEND:
                            case t_DECOR:
                            Jf->pInf = *Sif;
                            if (Sif->Type==t_FRIEND)
							{
								g_cGameStat.total_amy++;
							}
                            if (Sif->Type==t_ENEMY)
                            {
                                g_cGameStat.total_nmy++;
                                if ((g_SGSettings.Difficulty==0)&&(Jf->pInf.ShieldMax>2))
                                {
                                    Jf->pInf.ShieldMax-=Jf->pInf.ShieldMax/2;
                                    Jf->pInf.Shield-=Jf->pInf.Shield/2;
                                    if (Jf->pInf.fSpeedMax>2)
										Jf->pInf.fSpeedMax-=(Jf->pInf.fSpeedMax/4);
                                }
                            }
                            Jf->OVI = OVI;
                            OVI->data = Jf;
                            g_SGGame.numEnemies++;
                            Jf->pEffect = NULL;
                            Jf++;
                            break;
                            case t_ACCESSOIRE:
                            W = g_SGGame.pWea + g_SGGame.numWeapons;
                            W->Available  = 0;
                            OVI->data = W;
                            OVI->state &= ~V3XSTATE_HIDDEN;
                            OVI->state    |= V3XSTATE_CULLNEVER;
                            g_SGGame.numWeapons++;
                            W->pInf  = *Sif;
                            W->OVI  = OVI;
                            Coliable = 0;
                            break;
                            case t_PLAYER:
                            OVI->state |= V3XSTATE_HIDDEN;
#ifdef _DEBUG
//							SYS_Debug("Cmp with %s\n", g_pShip[g_pCurrentGame->ship].name);
#endif
                            f = 2;
                            if (((g_SGSettings.SerialGame)&&(numPlayer<g_SGGame.numPlayer))
                            ||(!numPlayer))
                            {
                                if (
                                (g_SGSettings.SerialGame)
                                || (strstr(Sif->Basename, g_pShip[g_pCurrentGame->ship].name))
                                )
                                {
                                    // oublie CMX
                                    Sif->CollisionStyle = t_CS_BUMP_LOSE_SHIELD;
                                    Sif->Animation  = t_MPLAYER;
                                    Sif->Tactic = 0;
                                    g_pPlayer = g_SGGame.pPlayer + numPlayer;
                                    g_pPlayer->J.pInf = *Sif;
                                    if ((numPlayer==g_SGGame.IdPlayer)||(!g_SGSettings.SerialGame))
                                    {
                                        Jf->pInf.Type = t_PLAYER;
                                        g_pPlayer->J.pInf.ColorRadar = 7;
                                    }
                                    else
                                    {
                                        Jf->pInf.Type = t_FRIEND;
                                        sysStrnCpy(g_pPlayer->J.pInf.Realname, g_SGGame.PlayersName[numPlayer], 12);
                                        g_pPlayer->J.pInf.ColorRadar = 6;
                                    }
                                    OVI->data = &g_pPlayer->J.pInf;
                                    OVI->state &= ~V3XSTATE_HIDDEN;
                                    g_pPlayer->J.OVI = OVI;
                                    g_pPlayer->Rot = &OVI->Tk->info;
                                    g_pPlayer->Mat = &OVI->mesh->matrix;//  V3X.Camera.M;
                                    g_pPlayer->Si = g_pShip + g_pCurrentGame->ship;
                                    OVI->matrix_Method = V3XMATRIX_Custom;
                                    Coliable = 2;
                                    numPlayer++;
                                    f = 1;
                                }
                            }
                            break;
                            default:
                            break;
                        }
		    }
                }
                if ((f==1)&&(Sf))
                {
                    if (Sf->NoFog)
                    OVI->mesh->flags|=V3XMESH_NOZSHADING;
                    if ((Sf->Collision==t_COL_AUTOSPHERE)
                    || (Sf->Collision==t_COL_AUTOSPHERE2))
                    {
                        if (!ORI->Cs)
                        {
                            ORI->Cs = V3XCL_NewFromMesh(OVI->mesh, V3XCTYPE_SPHERE );
                        }
                    }
                    if (ORI->Cs)
                    {
                        V3XCL *Cs = ORI->Cs;
                        Cs->weight = Sf->Poids;
                        Cs->reaction = 0.25;
                        ORI->global_rayon = Cs->global.radius*2;
                        ORI->global_center = Cs->global.center;
                        if (Coliable==2)                      Cs->flags|=V3XCMODE_REFLEX;
                        if (Sf->Collision==t_COL_AUTOSPHERE2) Cs->flags|=V3XCMODE_NOFEEDBACK;
                        if ((Sf->Type==t_ACCESSOIRE)||(Sf->Poids>=10000L))
                        {
                            Cs->flags|=V3XCMODE_DONTMOVE;
                        }
                        if (Cs->flags&V3XCMODE_INVERTCONDITION)
                        {
                            Cs->flags|=V3XCMODE_DONTMOVE;
                            Cs->flags|=V3XCMODE_REFLEX;
                            Cs->reaction = 1.;
                        }
                        if ((Sf->Type==t_OBSTACLE)||(Sf->NeverShock))
						{
                            Cs->flags|=V3XCMODE_DONOTTESTSAMEID;
                            Cs->ID = Sf->NeverShock ? Sf->NeverShock : Sf->Type;
                        }
						OVI->state|=V3XSTATE_CULLNEVER;
                    }
					else
                    {
                        ORI->global_rayon = 1024;
						ORI->global_center.x = ORI->global_center.y = ORI->global_center.z = CST_ZERO;
						OVI->state|=V3XSTATE_CULLNEVER;
                    }
                }
                else
                {
				//	SYS_ASSERT(f!=0);
                }
            }
        }
    }
    /* Initial g_pPlayer */
    g_pPlayer = g_SGGame.pPlayer + g_SGGame.IdPlayer;
	SYS_ASSERT(g_pPlayer->J.OVI);
    g_pPlayer->mode = 0;
    /* Clipping en Z */
    V3X.Light.fogDistance =  -15000;
    V3X.Clip.Far =  -65000;
    V3X.Clip.Near =  -1;
    g_SGGame.Scene->Layer.bg.flags= V3XBG_NONE;
    /* Details */
    V3XVector_Set(&g_pPlayer->J.Mv.vel, 0, 0, 0);
    g_pPlayer->J.pInf.fSpeed = g_pPlayer->J.pInf.fSpeedMax/2;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static void NG_ReadCMXFile(int level)
{
    char fn[32];
    int32_t p;
    p = MM_heap.push();
    sysMemZero(g_pPlayerInfo, sizeof(SGScript)*MAX_PLAYER);
    g_SGSettings.maxCase = 0;
    if (g_SGSettings.AddOn)
    {
        SYS_WAD *old = filewad_getcurrent();
		g_SGGame.AddOnResource = filewad_open(g_pGameItem->EI[g_pCurrentGame->episode].LI[g_pCurrentGame->level[g_pCurrentGame->episode]].tex[15], 0);
        filewad_setcurrent(g_SGGame.AddOnResource);
        sprintf(fn, "addon.cmx");
        NG_StageReadFile(fn, g_SGSettings.AddOn);
        filewad_setcurrent(old);
    }
    else
    {
        sprintf(fn, "stage_%d.cmx", level);
        NG_StageReadFile(fn, g_SGSettings.AddOn);
    }
    MM_heap.pop(p);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_NAVReset(int reset)
*
* DESCRIPTION :
*
*/
void NG_NAVReset(int reset)
{
    int i, nv;
    SGActor *J;
	char tex[256];
    V3XSCENE *Scene = g_SGGame.Scene;
    V3XOVI *OVI, *OVIp;
    V3XORI *ORI;
    g_SGGame.WarpOk = 0;
    if (g_SGObjects.NAV)
		sprintf(tex, "NAVPTS_#%d", g_SGObjects.NAV);
    else
		sysStrCpy(tex, "NAVPTS_#");
    OVI = V3XScene_OVI_GetByName(g_SGGame.Scene, tex);
    if (OVI==NULL)
    {
        OVI = V3XScene_OVI_GetByName(g_SGGame.Scene, "WARP_");
		SYS_ASSERT(OVI);
        OVI->state    |= V3XSTATE_CULLNEVER;
    }
    g_SGObjects.NavCam = OVI;
    // Find camera
    sprintf(tex, "NAV%d", g_SGObjects.NAV);
    OVI = V3XScene_Camera_GetByName(Scene, tex);
	SYS_ASSERT(OVI);
    if (OVI)
    {
        g_SGObjects.Cam = OVI;
        V3XScene_Camera_Select(OVI);
        V3X.Camera.focal = (V3XSCALAR)(g_SGObjects.World.Focal*2);
        V3XViewport_Setup(&V3X.Camera, GX.View);
        g_pCamera = &OVI->Tk->vinfo;
        if (reset)
        {
            // Start Cam pour la camera
            g_pCamera->target = g_pCamera->pos;
            V3XVector_Madd(&g_pCamera->pos, &V3X.Camera.M.v.K, &g_pCamera->pos, 65535);
            // Tout les joueurs
            for (i=0;i<g_SGGame.numPlayer;i++)
            {
                V3XMatrix_Transpose(g_SGGame.pPlayer[i].Mat->Matrix, V3X.Camera.M.Matrix);
                g_SGGame.pPlayer[i].Rot->pos = V3X.Camera.Tk.vinfo.pos;
            }
        }
    }
    for (i=Scene->numOVI, OVI=Scene->OVI;i!=0;OVI++, i--)
    {
        ORI=OVI->ORI;
        if (ORI->index_color>=4)
        {
            nv = NG_ColorToNAV(ORI->index_color);
            if (nv!=g_SGObjects.NAV)
            {
                OVI->state|=V3XSTATE_HIDDEN;
            }
            else
            {
                OVI->state&=~V3XSTATE_HIDDEN;
            }
        } else nv=0;
        if (OVI->data)
        {
            J=(SGActor*)OVI->data;
            if (J->pInf.AmbiantChannel)
				V3XA.Client->ChannelSetVolume(J->pInf.AmbiantChannel, 0 );
            J->pInf.ColorNav = nv;
            J->Mv.k = 1;
            if (J->pInf.Wait) OVI->state|=V3XSTATE_HIDDEN;
            OVI->state|=V3XSTATE_MATRIXUPDATE;
        }
    }
    for (i=Scene->numOVI, OVI=Scene->OVI;i!=0;OVI++, i--)
    {
        ORI = OVI->ORI;
        if (OVI->data)
	{
            J=(SGActor*)OVI->data;
            if (J->pInf.Replace)
            {
                char *g=strstr(OVI->ORI->name, "_");
                if (g)
                {
                    sprintf(tex, "%s%s", g_pPlayerInfo[J->pInf.Replace].Basename, g);
                    OVIp = V3XScene_OVI_GetByName(g_SGGame.Scene, tex);
                    if (OVIp) OVIp->state |= V3XSTATE_HIDDEN;
                }
            }
        }
    }
    g_SGGame.WarpOk = 0;
    g_SGObjects.Nav = g_SGObjects.World.Nav + g_SGObjects.NAV;
    SETBITFIELD(!g_SGObjects.Nav->Skybox, g_SGObjects.Sky->state, V3XSTATE_HIDDEN);
    NG_WeaponCreate();
    NG_DisplayWarp();
    sysMemZero(&g_cAI, sizeof(SGAI));
    NG_AISetTacticMode();
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_NAVReset(void)
*
* DESCRIPTION :
*
*/
void NG_NAVClear(void)
{
    int i;
    V3XSCENE *Scene = g_SGGame.Scene;
    V3XOVI *OVI;
    g_SGObjects.NAV = 0;    // FIXME : Start Nav
    g_SGObjects.MaxNAV = 0;
    for (i=Scene->numOVI, OVI=Scene->OVI;i!=0;OVI++, i--)
    {
        V3XORI *ORI;
        ORI=OVI->ORI;
        if (ORI->index_color>4)
        {
            unsigned nv = (ORI->index_color-4)>>2;
            if ((nv>g_SGObjects.MaxNAV)&&(nv<8)) g_SGObjects.MaxNAV=nv;
        }
    }
    g_SGObjects.MaxNAV++;
    NG_NAVReset(1);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static void NG_InitDisplay(void)
{
	GX.View.DisplayMode = (uint16_t)GX.Client->SearchDisplayMode(g_SGSettings.ResolutionX, g_SGSettings.ResolutionY, g_SGSettings.ColorDepth);
    GX.View.Flags = 0;

    if (V3X.Client->Capabilities&GXSPEC_HARDWARE)
	{
		V3X.Setup.flags|=V3XOPTION_TRUECOLOR;
		GX.View.Flags |= GX_CAPS_3DSYSTEM;
	}

    if ((g_SGSettings.Stereo))
		GX.View.Flags&=~GX_CAPS_BACKBUFFERINVIDEO;

	if (V3X.Client->Capabilities&GXSPEC_HARDWARE)
		GX.View.Flags|=GX_CAPS_BACKBUFFERINVIDEO;

	if (g_SGSettings.VerticalSync)
		GX.View.Flags|=GX_CAPS_VSYNC;

	g_SGSettings.ZBuffer = (V3X.Client->Capabilities&GXSPEC_HARDWARE);
    if ((g_SGSettings.ZBuffer)&&(V3X.Client->Capabilities&GXSPEC_HARDWARE))
	{
        V3X.Client->SetState(V3XCMD_SETZBUFFERSTATE, TRUE);
		sysConPrint("Zbuffer on");
	}
    else
	{
		V3X.Client->SetState(V3XCMD_SETZBUFFERSTATE, FALSE);
		sysConPrint("Zbuffer off");
	}
    NG_ChangeScreenMode(GX.View.DisplayMode);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_LoadGameInterface(void)
*
* DESCRIPTION :
*
*/
static void NG_LoadGameInterface(void)
{
    int g=0 ? 2+64 : 0+64;
    char tex[256];
    const uint8_t I_normal_Hie[44]={
        255, 255, 255, 255, 255,
        255, 255, 255, 2, 2,
        3, 3, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 0,
        0, 0, 0, 5, 6,
        7, 6, 6, 4, 5,
        6, 4, 255, 37, 37, 255};

	sprintf(tex, "v%d_sprit", g_pCurrentGame->ship+1);
    g_pspHud = NG_LoadSpriteGroup(tex, g);
    SYS_ASSERT(g_pspHud);

    sprintf(tex, ".\\int\\v%d_zones.spc", g_pCurrentGame->ship+1);
    g_pGameBoard = RW_Interface_GetFn(tex);
    SYS_ASSERT(g_pGameBoard);

	g_pGameBoard->parent = (uint8_t*)I_normal_Hie;
    RW_Interface_BuildTree(g_pGameBoard);
    g_pGameBoard->parent = NULL;

    NG_ResizeInterface(g_pGameBoard);
}

static void NG_ReleaseGameInterface(void)
{
    CSPG_Release(g_pspHud);
    RW_Interface_Release(g_pGameBoard);
	g_pspHud = 0;
	g_pGameBoard = 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_InitGameVariables(void)
*
* DESCRIPTION :
*
*/
static void NG_InitGameVariables(void)
{
    int i;

    g_SGObjects.Time = g_pGameItem->EI[g_pCurrentGame->episode].LI[g_pCurrentGame->level[g_pCurrentGame->episode]].Time;
    g_SGObjects.IsTimed = g_SGObjects.Time!=0;
    g_pPlayer->CurArt = 1;
    for (i=0;i<8;i++)
    {
        g_pPlayer->Art[i].MAX_val = g_SGSettings.Cheater ? 99 : 10;
        g_pPlayer->Art[i].val = g_SGSettings.Cheater ? 99 : 0;
    }
    g_cGameStat.bonus = 0;
    g_cGameStat.shooted = 0;
    g_SGObjects.FinCode = GAMESTATE_PLAY;
    g_SGObjects.quit = 0;
    g_SGObjects.CallMode[t_ENEMY] =ORDER_Attack;
    g_SGObjects.CallMode[t_FRIEND]=ORDER_Attack;
    g_SGGame.CameraMode = 0;
    g_SGGame.Count = 0;
    g_SGGame.ComMode = 0;
    g_SGGame.RadarRange = RADARMAX;
    g_SGGame.DeathDist = 1400;
    g_SGGame.CameraMode = g_SGSettings.SerialGame||g_SGSettings.DemoMode ? 0 :CAMERA_START;
    g_SGGame.FlashAlpha = -32;
    g_SGSettings.ComNumber = 0;
    g_SGGame.LockMAX = 0;
    g_pLockTarget = NULL;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_InitGameGraphics(void)
*
* DESCRIPTION :
*
*/
void NG_InitGameGraphics(void)
{
    g_SGObjects.Sky = V3XScene_OVI_GetByName(g_SGGame.Scene, "sky_");
    g_SGObjects.Sky->state |= V3XSTATE_GIANT;
    g_SGObjects.Shield = V3XScene_OVI_GetByName(g_SGGame.Scene, "shield_");
	SYS_ASSERT(g_SGObjects.Shield);
    NG_DrawExtra();
    V3X.Camera.focal = (V3XSCALAR)(g_SGObjects.World.Focal<<8);
    V3XViewport_Setup(&V3X.Camera, GX.View);
    GX.csp_cfg.table = g_SGGame.Scene->Layer.lt.alpha50.table;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_InitGameShip(void)
*
* DESCRIPTION :
*
*/
void NG_InitGameShip(void)
{
    g_cShip = g_pShip[g_pPlayer->Si->code];
    switch (g_pPlayer->Si->code) {
        case 0: // Viper
        g_pPlayer->Mv.inert = 8;
        g_pPlayer->J.pInf.Attack = 1;
        break;
        case 1: // Raiden
        g_pPlayer->Mv.inert = 8;
        g_pPlayer->J.pInf.Attack = 2;
        break;
        case 2: // Raptor
        g_pPlayer->Mv.inert = 8;
        g_pPlayer->J.pInf.Attack = 3;
        break;
    }
    g_pPlayer->J.pInf.Scoring = g_pCurrentGame->score;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_InitGameSounds(void)
*
* DESCRIPTION :
*
*/
static void NG_InitGameSounds(void)
{
    // Start Sound
    NG_AudioPlayMusic();
    g_cFXTable.Engine = V3XA.Client->ChannelGetFree(NULL);
    g_cFXTable.SoundEngine = g_pPlayer->J.pInf.StartSound-1;
    g_cFXTable.SoundAlarm = NG_AudioGetByName("alarm")-1;
    g_cFXTable.Alarm = 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_InitGameDisplay(void)
*
* DESCRIPTION :
*
*/
static void NG_InitGameDisplay(void)
{
	int bAllowTrsp = 1; // (GX.View.Flags&GX_CAPS_BACKBUFFERINVIDEO)==0;

    if (g_SGSettings.TexFiltering)
		V3X.Client->Capabilities|=GXSPEC_ENABLEFILTERING;
    else
		V3X.Client->Capabilities&=~GXSPEC_ENABLEFILTERING;

	if (g_SGSettings.TexPOT)
		V3X.Client->Capabilities&=~GXSPEC_NONPOWOF2;

	if (g_SGSettings.Dithering)
		V3X.Client->Capabilities|=GXSPEC_ENABLEDITHERING;
    else
		V3X.Client->Capabilities&=~GXSPEC_ENABLEDITHERING;

    if (!bAllowTrsp)
    {
        GX.csp.Trsp50 = GX.csp.put;
        GX.csp.TrspADD = GX.csp.put;
    }

    if (V3X.Client->Capabilities & GXSPEC_HARDWAREBLENDING)
    {
		if (!(V3X.Client->Capabilities & GXSPEC_FULLHWSPRITE))
		{
			GX.csp.zoom_put = V3XCSP_3DSprite;
			GX.csp.zoom_Trsp50 = GX.csp.zoom_TrspSUB = GX.csp.zoom_TrspALPHA = GX.csp.zoom_TrspADD = V3XCSP_3DSprite_Alpha;
		}
    }

    sMOU->SetPosition(GX.View.xmax/2, GX.View.ymax/2);

    sysMemCpy(GX.ColorTable, g_SGGame.Scene->Layer.lt.palette.lut, 768);

    if (g_SGSettings.Stereo==1)
		PAL_SetRedCyanPalette();

    if (GX.View.BytePerPixel==1)
		PAL_Black();
    else
		PAL_Full();

    NG_InstallHandlers();
}

static uint32_t time_maxi;

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int NG_NetWaiter(void)
*
* DESCRIPTION :
*
*/
static int NG_NetWaiter(int mode)
{
	char tex[128];
    uint32_t t = timer_sec();
    int32_t y = GX.View.ymax/2, ly = g_pspDispFont->item[0].LY+2;
    rgb24_t bleu = {0, 0, 64};
    uint32_t cl = RGB_PixelFormatEx(&bleu);
    sKEY->Update(0);
	sJOY->Update(0);
    if (sKEY_IsHeld(s_esc)||(t>time_maxi)) return 0;
    GX.Client->Lock();
    GX.gi.drawFilledRect(0, 0, GX.View.xmax, GX.View.ymax, cl);
    sprintf(tex, "%s", g_szGmT[193]);
    CSP_WriteCenterText(tex, y+=ly, g_pspDispFont);
    sprintf(tex, "%d secs.", (int)(time_maxi-t));
    CSP_WriteCenterText(tex, y+=ly, g_pFont);
    sprintf(tex, "%d %%", mode*50);
    CSP_WriteCenterText(tex, y+=ly, g_pFont);
    GX.Client->Unlock();
    GX.View.Flip();
    return 1;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int TX_Warn(void)
*
* Description :
*
*/
static int TX_Warn(void)
{
    rgb24_t bleu = {64, 0, 0};
    uint32_t cl = RGB_PixelFormatEx(&bleu);
    GX.Client->Lock();
    GX.gi.drawFilledRect(0, 0, GX.View.xmax, GX.View.ymax, cl);
    CSP_WriteCenterText("No enough video memory for textures!", GX.View.ymax/2, g_pspDispFont);
    GX.Client->Unlock();
    GX.View.Flip();
    timer_snooze(2000);
    return 1;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_GameStart(void)
*
* DESCRIPTION :
*
*/
void NG_GameStart(void)
{
	int start_level = 0;
	char tex[256];
    // Envoie le level via le network
	if (!g_pCurrentGame)
		g_pCurrentGame = g_pSaveGames;

    if ((g_SGGame.IsHost)&&(g_SGSettings.SerialGame))
    {
        tex[0]= 'œ';
        tex[1]= (char)g_pCurrentGame->episode;
        tex[2]= g_pCurrentGame->level[g_pCurrentGame->episode];
        tex[3]= 0; // security
        sNET->SendData(NET_EVERYBODY, tex, 3);
    }
    switch(g_SGSettings.DemoMode) {
        case 2:
        case 4:
#if (SGTARGET ==NG_FULL_VERSION)
        g_pCurrentGame->ship = sysRand(3);
#endif
        break;
    }

    // Test Controller
    NG_CheckSystems();

	// Level
    start_level = (g_pCurrentGame->episode+1)*10L+(g_pCurrentGame->level[g_pCurrentGame->episode]+1);
    MM_heap.reset();

	g_cTimer.iMinFrame = g_SGGame.Demo ? 1 : g_SGSettings.FrameSkip;
	if (!g_cTimer.iMinFrame)
		g_cTimer.iMinFrame = 1;
    timer_Start(&g_cTimer, 70, 1);

    // Mode video
    NG_InitDisplay();
	NG_PlayLoadingScreen();

    g_pspDispFont = g_pFont;
	if (g_SGSettings.TexCompression)
		V3X.Client->Capabilities|=GXSPEC_ENABLECOMPRESSION;
    else
		V3X.Client->Capabilities&=~GXSPEC_ENABLECOMPRESSION;

    NG_DrawLoadingBar(5);
    // Interface
    NG_LoadGameInterface();
    NG_DrawLoadingBar(10);
    // Enregistrement
    switch(g_SGSettings.DemoMode) {
        case 1:
        NG_ReplayStart();
        g_SGGame.IsHost=1;
        break;
        case 2:
        case 4:
        start_level = NG_ReplayLoad();
        g_SGGame.IsHost=1;
        break;
    }
    // Reset Game Value
    NG_ResetGameData();
    NG_DrawLoadingBar(20);
    // Lit le CodeMax
    NG_ReadCMXFile(start_level);
    NG_DrawLoadingBar(30);
    // Charge Scene 3D
    NG_InitGameScene();
    NG_DrawLoadingBar(40);
    // Convertit CMX
    NG_CMXToObject();
    NG_DrawLoadingBar(50);
    // Valeur du jeu
    NG_InitGameVariables();
    NG_DrawLoadingBar(60);
    // Valeur 3D
    NG_InitGameGraphics();
    NG_DrawLoadingBar(70);
    // Detail Graphique
    NG_ChangeGameDetail();
    NG_DrawLoadingBar(75);
    // Init des vaisseaux
    NG_InitGameShip();
    NG_DrawLoadingBar(80);
    // Init des tirs
    NG_WeaponCreate();
    NG_DrawLoadingBar(85);
    // Init des nav
    NG_NAVClear();
    NG_DrawLoadingBar(90);
    // Init des animations
    filewad_chdir(FIO_wad, "");
    NG_DrawLoadingBar(95);
    NG_DrawLoadingBar(97);
    NG_InitAnimateStage();
    NG_DrawLoadingBar(100);

    GX.Client->ReleaseSprite(&g_csPicture);


    NG_InitGameDisplay();

    if (g_SGSettings.SerialGame)
    {
        time_maxi = timer_sec()+120;
        PAL_Full();
        if (!sNET->Synchronise(0, NG_NetWaiter))
        {
            g_SGSettings.SerialGame=0;
        }
        PAL_Black();
    }

    if (V3X.Setup.warnings&V3XWARN_NOENOUGHSurfaces)
    {
        TX_Warn();
        V3X.Setup.warnings&=~V3XWARN_NOENOUGHSurfaces;
    }

	V3X.Client->Capabilities&=~GXSPEC_ENABLECOMPRESSION;

	NG_InitGameSounds();
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_GameStop(void)
*
* DESCRIPTION :
*
*/
void NG_GameStop(void)
{
	NG_AudioStopSound(g_cFXTable.Engine);
    NG_AudioStopSound(g_cFXTable.Alarm );
    NG_AudioStopTrack();

	if (g_SGSettings.DemoMode==1)
		NG_ReplaySave();

    if (g_SGSettings.DemoMode)
		NG_ReplayRelease();

	NG_ReleaseGameInterface();
    NG_FXReleaseData();
    V3XScene_Release(g_SGGame.Scene);
    if (g_SGGame.AddOnResource)
    {
        filewad_close(g_SGGame.AddOnResource);
        g_SGGame.AddOnResource = NULL;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_LoadGameData(void)
*
* DESCRIPTION :
*
*/
void NG_LoadGameData(void)
{
	NG_AllocGameData();
	NG_LoadFonts();
    NG_ReadMissionList();
    NG_StarfieldCreate();
	NG_FXLoadList();
    NG_AudioLoadList();
    NG_FXCreate();
    NG_AudioLoadWave();
}

void NG_ReleaseGameData()
{
	NG_FXRelease();
	NG_StarfieldRelease();
	NG_ReleaseFonts();
	NG_FreeGameData();
}

