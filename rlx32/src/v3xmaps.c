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
#include <stdio.h>
#include <stdlib.h>
#include "_rlx32.h"
#include "sysresmx.h"
#include "systools.h"
/*****/
#include "gx_struc.h"
#include "gx_csp.h"
#include "gx_flc.h"
#include "gx_tools.h"
#include "gx_init.h"
#include "gx_rgb.h"
/*****/
#include "v3xdefs.h"
#include "v3x_1.h"
#include "v3x_2.h"
#include "v3xrend.h"
#include "_rlx.h"
/******/
#include "v3xmaps.h"
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XResources_reset(V3XRESOURCE *bm)
*
* DESCRIPTION :
*
*/
void V3XResources_Reset(V3XRESOURCE *bm)
{
    int i;
    V3XRESOURCE_ITEM *bj = bm->item;
    for (i=0;i<bm->numItems;i++, bj++)
    {
        if (bj->data) MM_heap.free(bj->data);
        bj->data = NULL;
        bj->filename[0] = 0;
        bj->flags = 0;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3XResources_Put(V3XRESOURCE *bm, char *filename, void *data)
*
* DESCRIPTION :
*
*/
V3XRESOURCE_ITEM *V3XResources_Put(V3XRESOURCE *bm, const char *filename, void *data, int type)
{
    int i;
    V3XRESOURCE_ITEM *bi=NULL, *bj = bm->item;
    for (i=0;(i<bm->numItems) && (bi==NULL);i++, bj++)
    {
        if ((bj->flags&1)==0) bi = bj;
    }
    if (bi)
    {
        sysStrnCpy(bi->filename, filename, 56);
        bi->data = data;
        bi->flags   |= 1;
        bi->type = (u_int8_t)type;
    }
    return bi;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int V3XTCachedel(V3XRESOURCE *bm, char *filename)
*
* DESCRIPTION :
*
*/
int V3XResources_Del(V3XRESOURCE *bm, const char *filename)
{
    int i, j=0;
    char *s = file_name((char*)filename);
    V3XRESOURCE_ITEM *bi=NULL, *bj=bm->item;
    for (i=0;(i<bm->numItems) && (bi==NULL);i++, bj++)
    {
        if (bj->flags&1)
        if (sysStriCmp(file_name(bj->filename), s)==0)
        {
            if (j==0) bj->flags&=~3;
            j++;
        }
    }
    return j==1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XResources_Purge(V3XRESOURCE *bm, int purge)
*
* Description :  
*
*/
void V3XResources_Purge(V3XRESOURCE *bm, int purge)
{
    int i;
    V3XRESOURCE_ITEM *bj=bm->item;
    for (i=0;i<bm->numItems;i++, bj++)
    {
        if (bj->flags&1)
        { 
            if (purge)
            {
                if ((bj->type == V3XRESOURCETYPE_TEXTURE2)||(bj->type==V3XRESOURCETYPE_TEXTURE))   
                {
                    if (bj->flags&2)
                    {     
                        FLI_STRUCT *fli = (FLI_STRUCT*)bj->data;
                        if (fli->frames)
                        {
                            GXSPRITE *sp;
                            int j;
                            for (sp=fli->frames, j=fli->MaximumFrame;j!=0;sp++, j--)
                            {   
								if (sp->handle)
								{
									V3X.Client->TextureFree(sp->handle);
									sp->handle = NULL;
								}
                                sp->data = NULL; 
								
                            }
                        }
                        FLI_Close(fli);
                    }
                    else
                    {
                        GXSPRITE *texmap = (GXSPRITE*)bj->data;                        
						if (texmap->handle)
						{
							V3X.Client->TextureFree(texmap->handle);
							texmap->handle = 0;
						}
                    }
                    bj->flags&=~3;
                }      
            }
        }
    }
    return ;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void *V3XResources_Get(V3XRESOURCE *bm, char *filename)
*
* DESCRIPTION :
*
*/
void *V3XResources_Get(V3XRESOURCE *bm, const char *filename, int type)
{
    int i;
    V3XRESOURCE_ITEM *bi=NULL, *bj=bm->item;
    for (i=0;(i<bm->numItems) && (bi==NULL);i++, bj++)
    {
        if (bj->flags&1)
        {
            if (bj->type == type)
            {
                if (sysStriCmp(bj->filename, filename)==0)
                {
                    bi = bj;
                }
            }
        }
    }
    if (bi)
    {
        return bi->data;
    }
    else return NULL;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XResources_Animated(V3XRESOURCE *bm)
*
* DESCRIPTION :
*
*/
void V3XResources_Animated(V3XRESOURCE *bm)
{
    int i;
    V3XRESOURCE_ITEM *bi=NULL, *bj=bm->item;
    for (i=0;(i<bm->numItems) && (bi==NULL);i++, bj++)
    {
        if (bj->flags&2)
        {
            if ((bj->type == V3XRESOURCETYPE_TEXTURE2)
            || (bj->type == V3XRESOURCETYPE_TEXTURE))
            {
                FLI_STRUCT *a = (FLI_STRUCT*)bj->data;
                u_int32_t t = V3X.Time.ms;
                if (t > a->LastTime )
                {
                    a->LastTime = t + (u_int32_t)a->Header.Struct.speed;
                    a->Flags|=FLX_DECOMPRESSFRAME;
                }
            }
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XCache_Material(V3XMATERIAL *Mat, int CycleSpeed)
*
* DESCRIPTION :
*
*/
int V3XCache_Material(V3XMATERIAL *Mat, int option)
{
    if (Mat->fli)
    {
        FLI_STRUCT *a = (FLI_STRUCT*)Mat->fli;
        if (a->Flags & FLX_DECOMPRESSFRAME)
        {
            if ((option&2)==0)
			{
				FLI_Unpack(a);   
				if (a->decompBuffer)
					V3X.Client->TextureModify(Mat->texture+0, a->decompBuffer, a->ColorTable);
			}

            if (option&1) 
            {
                if (( a->Flags & FLX_ISPLAYING)==0) 
                {
                    FLI_Rewind(a);
                    return -2;
                }
            }      
        }
    }
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XCache_Mesh(V3XMESH *Obj)
*
* DESCRIPTION :
*
*/
void V3XCache_Mesh(V3XMESH *Obj)
{
    int i;
    V3XMATERIAL *Mat = Obj->material;
    for (i=0;i<Obj->numMaterial;i++, Mat++)  
		V3XCache_Material(Mat, 0);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :void deinit_V3XMATERIAL(V3XMATERIAL *Mat)
*
* DESCRIPTION :
*
*/
static void V3x_Mapper_del(V3XMESH *Obj, V3XMATERIAL *Mat, GXSPRITE *texmap, char *szFullPathName)
{
    u_int8_t *w = (u_int8_t*)texmap->handle;
    int i;
    if (!w)
		w = texmap->data;
    if (Mat->fli)
    {
        if (V3XResources_Del(&V3X.Cache, szFullPathName))
        {
            FLI_STRUCT *fli = (FLI_STRUCT*)Mat->fli;
            if (fli->frames)
            {
                GXSPRITE *sp;
                for (sp=fli->frames, i=fli->MaximumFrame;i!=0;sp++, i--)
                {     
					if (sp->handle)
					{
						V3X.Client->TextureFree(sp->handle);
						sp->handle = NULL;
					}
                    sp->data = NULL; 					
                }
            }
            FLI_Close(fli);
            w = NULL;
        }
    }
    if (w!=NULL)
    {
        if (Obj)
        {
            V3XMATERIAL *tos = Obj->material;
            for (i=0;i<Obj->numMaterial;i++, tos++)
            {
                if (tos->texture[0].data==texmap->data) 
					tos->texture[0].data=NULL;

                if (tos->texture[1].data==texmap->data) 
					tos->texture[1].data=NULL;
            }
        }
        if (V3XResources_Del(&V3X.Cache, szFullPathName))
        {
			if (texmap->handle)
			{
				V3X.Client->TextureFree(texmap->handle);
				texmap->handle = 0;
			}
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMaterial_Release(V3XMATERIAL *Mat, V3XMESH *Obj)
*
* DESCRIPTION :
*
*/
void V3XMaterial_Release(V3XMATERIAL *Mat, V3XMESH *Obj)
{
    V3XResources_Del(&V3X.Cache, Mat->mat_name);
    V3x_Mapper_del(Obj, Mat, &Mat->texture[0], Mat->tex_name);
    V3x_Mapper_del(Obj, Mat, &Mat->texture[1], Mat->ref_name);
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void V3XMaterial_UploadTexture(GXSPRITE *texture[0], GXSPRITE *surface, V3XMATERIAL *Mat)
*
* DESCRIPTION : Charge un GXSPRITE, resize, en Surfaces etc...
*
*/
static void V3XMaterial_UploadTexture(GXSPRITE *pDst, const GXSPRITE *pSrc, V3XMATERIAL *Mat, int bpp, int nTMU)
{
    int x = MM_heap.active;
    // Compute reduction factor
    unsigned option = 0;
    // Is bpp in raw format?
	
    if (bpp<128)
    {
        Mat->shift_size = 0;
        if ((Mat->Properties&V3XMAT_NORESIZE)
        &&((V3X.Client->Capabilities&GXSPEC_RESIZABLEMAP))&&(Mat->shift_size))
        {
            Mat->shift_size--;
        }    
		if (V3X.Client->texMaxSize)
		{	
			while((pSrc->LX>>Mat->shift_size)>(1UL<<V3X.Client->texMaxSize))
			{
				Mat->shift_size++;
			}	
		}
        if ((V3X.Client->Capabilities&GXSPEC_HARDWARE)||(Mat->info.Sprite))
        {
            pDst->LX = pSrc->LX>>Mat->shift_size;
            pDst->LY = pSrc->LY>>Mat->shift_size;
        }
        else
        {
            pDst->LX = 256>>Mat->shift_size;
            pDst->LY = 256>>Mat->shift_size;
        }
        // Fit in a 2^n box
        option = 1;  while ((1UL<<option)<pDst->LX) option++;
        pDst->LX = 1<<option;
        option = 1;  while ((1UL<<option)<pDst->LY) option++;
        pDst->LY = 1<<option;
        // copy texture to 3D Ram
        if  ((pDst->LY == pSrc->LY)&&(pDst->LX == pSrc->LX))
        {
            *pDst = *pSrc;
        }
        else // Resize texture
        {
            int bp=(bpp+1)>>3;
            MM_heap.active = 0;
            if ((Mat->shift_size>1)&&((bp==1)||(bp==2)))
				IMG_StretchBilinear(pDst, (GXSPRITE*)pSrc, bp, 0);
            else
				IMG_StretchPoint(pDst, (GXSPRITE*)pSrc, bp, 0);
            if (!Mat->fli) 
				MM_heap.free(pSrc->data);
        }
    }
    MM_heap.active = x;
    option = 0;
    if (nTMU)           
		option|=V3XTEXDWNOPTION_TMU2;
    else
    if (Mat->Render == V3XRCLASS_bump_mapping) 
		option|=V3XTEXDWNOPTION_BUMP;
    
	if (Mat->info.Opacity)        
		option|=V3XTEXDWNOPTION_COLORKEY;
    
	if (Mat->info.Dynamic)           
		option|=V3XTEXDWNOPTION_DYNAMIC;    
	
	pDst->handle = V3X.Client->TextureDownload(pSrc, GX.ColorTable, bpp, option);
	SYS_ASSERT(pDst->handle);

	if (V3X.Client->Capabilities&GXSPEC_HARDWARE)
		pDst->data = (u_int8_t*)pDst->handle;
    
	if (!pDst->handle)  
		V3X.Setup.warnings|=V3XWARN_NOENOUGHSurfaces; 
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void V3XMaterial_LoadTexturesFn(V3XMATERIAL *Mat, char *szFullPathName, GXSPRITE *dst_tex, char *mode)
*
* DESCRIPTION :
*
*/
static void V3XMaterial_LoadTexturesFn(V3XMATERIAL *Mat, char *szFilename, GXSPRITE *tex, int nTMU)
{
    int   texMode = (nTMU==1) ? V3XRESOURCETYPE_TEXTURE2 : V3XRESOURCETYPE_TEXTURE;
    char *szFullPathName;
    if (!szFilename[0]) 
		return;
		szFullPathName = file_searchpathES(szFilename, V3X.Setup.texturePath);
	if (!szFullPathName)
	{
		sysStrExtChg(szFilename, szFilename, "png");
		szFullPathName = file_searchpathES(szFilename, V3X.Setup.texturePath);
	}

	SYS_ASSERT(szFullPathName);
    if (szFullPathName)
    {
        SYS_FILEHANDLE fp;
        FLI_STRUCT *fli;
        GXSPRITE surface;
        GXSPRITE *sp;
        /* ************** mapping animé **************** */
        if (strstr(szFullPathName, ".fl"))
        {
            fli = (FLI_STRUCT*) V3XResources_Get(&V3X.Cache, szFullPathName, texMode);
            if (fli==NULL)
            {
                fp = FIO_cur->fopen(szFullPathName, "rb");
                if (fp)
				   fli = FLI_Open(fp, FLI_USEMEMORY);
				   
				SYS_ASSERT(fli);
                if (!fli)
                {
                    Mat->Render = V3XRCLASS_flat;
                    Mat->info.Shade = 2;
                }
                else
                {
                    V3XRESOURCE_ITEM *item;
                    Mat->fli = fli;
                    item = V3XResources_Put(&V3X.Cache, szFullPathName, fli, texMode);
                    if (item) 
						item->flags = 3;

                    if (fli->frames)
                    {
                        int i;
                        GXSPRITE *sp;
                        for (sp=fli->frames, i=fli->MaximumFrame;i!=0;sp++, i--)
                        {
                            FLI_Unpack(fli);
                            surface = *sp;							
                            V3XMaterial_UploadTexture(sp, &surface, Mat, 8, nTMU|0x1000);
                        }
                        *tex = fli->bitmap;
                        FIO_cur->fclose(fli->fli_stream);
                        fli->fli_stream = NULL;
                    }
                    else
                    {
                        FLI_Unpack(fli);
                        surface = fli->bitmap;
                        V3XMaterial_UploadTexture(tex, &surface, Mat, 8, nTMU|0x1000);
						fli->bitmap.handle = tex->handle;
                    }
					SYS_ASSERT(tex->handle);
                }
            }
            else // clone le fli
            {
                Mat->fli = fli;
                Mat->shift_size = 0;
                *tex = fli->bitmap;		
				SYS_ASSERT(tex->handle);
            }
			
        }
        /* ************** mapping fixe ****************** */
        else
        {			
            sp = (GXSPRITE*) V3XResources_Get(&V3X.Cache, szFilename, texMode);
            if (sp==NULL)
            {
                int x = MM_heap.active, bpp;

                MM_heap.active = 0;				
				bpp = IMG_LoadFn(szFullPathName, &surface);	
				SYS_ASSERT(bpp == 8);
			
				if (bpp)
                {
                    if (bpp<15)
                    {
                        if ((V3X.Setup.flags&V3XOPTION_USESAMELUT)&&GX.ColorClut)
							sysMemCpy(GX.ColorTable, GX.ColorClut, 768);
                    }

					MM_heap.active = x;
					V3XMaterial_UploadTexture(tex, &surface, Mat, bpp, nTMU);				
					MM_heap.active = 0;
					MM_heap.free(surface.data);
                    MM_heap.active = x;
                   
                    V3XResources_Put(&V3X.Cache, szFilename, tex, texMode);
                }
            }
            else
            {
                *tex = *sp;
                if (sp->data==NULL)
                {
                    Mat->Render = V3XRCLASS_flat;
                    Mat->info.Shade = 2;
                }
            }
			SYS_ASSERT(tex->handle);
        }
    }
    else
    {
        if (V3X.Setup.flags&V3XOPTION_WARNING)
        {
			char tex[256];
            sprintf(tex, "Texture file not found %s\n%s", szFilename, V3X.Setup.texturePath);
            SYS_Msg(tex);
            SYS_Debug(tex);
        }
        V3X.Setup.warnings|=V3XWARN_MISSINGTEXTURES;        
        Mat->info.Texturized = 0;
        Mat->Render = Mat->info.Shade ? V3XRCLASS_gouraud : V3XRCLASS_flat;
        Mat->texture[0].handle = NULL;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMaterial_LoadTextures(V3XMATERIAL *Mat)
*
* DESCRIPTION :
*
*/
void V3XMaterial_LoadTextures(V3XMATERIAL *Mat)
{
    if ((Mat->info.Texturized)||(Mat->info.Sprite))
    {
        V3XMaterial_LoadTexturesFn(Mat, Mat->tex_name, &Mat->texture[0], 0);
    }
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void V3XMaterials_LoadFromMesh(V3XMESH *Obj)
*
* DESCRIPTION : … reprendre
*
*/
void V3XMaterials_LoadFromMesh(V3XMESH *Obj)
{
    int i;
    V3XMATERIAL *Mat = Obj->material;
    for (i=Obj->numMaterial;i!=0;Mat++, i--) 
		V3XMaterial_LoadTextures( Mat );
    return;
}
