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
#include "_rlx32.h"
#include "systools.h"
#include "sysresmx.h"
#include "gx_struc.h"
#include "gx_tools.h"
#include "fixops.h"
#include "gx_rgb.h"
#include "gx_csp.h"
#include "_rlx.h"

static const short CSP_Offset[] =
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, 36, -1, 53, 51, 45, -1, 47, 41, 42, 49, 43, 51, 44, 38, 40,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 39, -1, -1, 46, -1, 37,
	52,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, 50, -1, -1, 48,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, -1,  4, -1, -1,  0, -1, -1,  0, -1,  4, -1, -1, -1,  4, -1,
	-1, -1, -1, 14, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1,  4, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1,  4, -1,  8, -1,  8,
	-1, 13, -1, 14, -1, -1, 14, -1, -1, -1, -1, -1, 20, -1, -1, -1,
	-1, -1, -1, -1,  0, -1, -1, -1, -1,  4, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, 14, -1, -1, 14, -1, -1, -1, -1, -1, 20, -1, -1, -1
};
static int CSP_Offset2[]={
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 
    31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 
    47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 
    63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 
    79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 
    -1, -1, 68, -1, -1, 64, -1, -1, 64, -1, 68, -1, -1, -1, 68, -1, 
    -1, -1, -1, 78, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 30, 
    -1, -1, -1, -1, 64, -1, -1, -1, -1, -1, -1, 68, -1, 72, -1, 72, 
    -1, 77, -1, 78, -1, -1, 78, -1, -1, -1, -1, -1, 84, -1, -1, -1, 
    -1, -1, -1, -1, 64, -1, -1, -1, -1, 68, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, 78, -1, -1, 78, -1, -1, -1, -1, -1, 84, -1, -1, -1, 
};
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  #define SpriteGroup_Car2Val(car, type) (type ? CSP_Offset2[car] : CSP_Offset[car])
*
* Description :  
*
*/
#define SpriteGroup_Car2Val(car, type) (type ? CSP_Offset2[(int)(car)] : CSP_Offset[(int)(car)])
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CSP_Get(int32_t xx, int32_t yy, GXSPRITE *capt, u_int8_t *buf, int32_t width, int32_t byte)
*
* DESCRIPTION :
*
*/
_RLXEXPORTFUNC void CSP_Get(int32_t xx, int32_t yy, GXSPRITE *capt, u_int8_t *buf, int32_t width, int32_t byte)
{
    u_int32_t cx, lx=capt->LX*byte, lxx=width*byte;
    u_int8_t *v=buf+(yy*width+xx)*byte, 
    *w=capt->data;
	SYS_ASSERT(buf);
	SYS_ASSERT(width);
    for (cx=capt->LY;cx>0;cx--)
    {
        sysMemCpy(w, v, lx);
        w+=lx;
        v+=lxx;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CSPG_Release(GXSPRITEGROUP *pSpriteGroup)
*
* DESCRIPTION :
*
*/
_RLXEXPORTFUNC void CSPG_Release(GXSPRITEGROUP *pSpriteGroup)
{
    int i;
	SYS_ASSERT(pSpriteGroup);
    for (i=0;i<pSpriteGroup->maxItem;i++)
        GX.Client->ReleaseSprite(pSpriteGroup->item+i);
    MM_heap.free(pSpriteGroup->item);
    MM_heap.free(pSpriteGroup);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  GXSPRITEGROUP static *CSPG_GetFp(SYS_FILEHANDLE in, u_int8_t *buf, int32_t pitch, int bpp, int inSurfaces)
*
* DESCRIPTION :
*
*/
static void CSPG_GetFp(GXSPRITEGROUP *pSpriteGroup, SYS_FILEHANDLE in, u_int8_t *buf, int32_t pitch, int bpp, int inSurfaces)
{
    char      SPChead[4];
    int  i;    
    GXSPRITE   *sp;    
    FIO_cur->fread(SPChead, sizeof(char), 4, in);
    FIO_cur->fread(&pSpriteGroup->maxItem, sizeof(unsigned short int), 1, in);
#ifdef LSB_FIRST
    BSWAP16((u_int16_t*)&pSpriteGroup->maxItem, 1);
#endif
    pSpriteGroup->item = MM_CALLOC(pSpriteGroup->maxItem, GXSPRITE);
    for (i=0, sp=pSpriteGroup->item;i<pSpriteGroup->maxItem;i++, sp++)
    {
        GXSPRITEFORMAT   spl;
        FIO_cur->fread(&spl, sizeof(GXSPRITEFORMAT), 1, in);
#ifdef LSB_FIRST
        BSWAP16((u_int16_t*)&spl, 4);
#endif
        sp->LX = spl.c - spl.a + 1;
        sp->LY = spl.d - spl.b + 1;
        sp->data = (u_int8_t *) MM_heap.malloc(sp->LX*sp->LY*bpp);
        CSP_Get(spl.a, spl.b, sp, buf, pitch, bpp);
        switch(inSurfaces) 
		{
            case 1:
				SYS_ASSERT(GX.Client->UploadSprite);
				GX.Client->UploadSprite(sp, GX.ColorTable, bpp);
            break;
            case 2:
            {
                GXSPRITEUV *pPage = (GXSPRITEUV*) MM_heap.malloc(sizeof(GXSPRITEUV));
                pPage->U = (u_int8_t)spl.a;
                pPage->V = (u_int8_t)spl.b;
                pPage->Page = sp->handle;
                pPage->Alpha = 0;
                pPage->Mode = 0;
                sp->handle = pPage;
            }
            break;
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  GXSPRITEGROUP *CSPG_GetFn(char *filename, char *mode, SYS_FILEIO *f, unsigned option)
*
* DESCRIPTION :
*
*/
static int CSPG_GetES(GXSPRITEGROUP *s, const char *bitname, const char *spcname, int option)
{
    SYS_FILEHANDLE in;
    GXSPRITE sp;
    unsigned bs, bt = (RLX.V3X.Id==RLX3D_OPENGL) ? 4 : (option&CSPLOAD_FORCE8 ? 1 : GX.View.BytePerPixel);
    int h = MM_heap.active;    
    MM_heap.active = 0;
	bs = IMG_LoadFn(bitname, &sp); 
	bs = (bs+1)>>3;
	
	if ((option&CSPLOAD_RGBAFIX)&&(bs==4)) 
		RGBA_to_RGB((rgb32_t*)sp.data, sp.LX*sp.LY);

	if (option&CSPLOAD_POSTERIZE)
    {			
		if ((bt>1)||(option&CSPLOAD_FORCE8))
        {		
            if (bs!=bt)
				sp.data = RGB_SmartConverter(NULL, NULL, bt, sp.data, GX.ColorTable, bs, sp.LX * sp.LY);
        }
    }
    MM_heap.active = h;
    in = FIO_cur->fopen(spcname, "rb");
	SYS_ASSERT(in);
    CSPG_GetFp(s, in, sp.data, sp.LX, bt, (option&CSPLOAD_SURFACE ? 1 : 0) + (option&CSPLOAD_HWSPRITE  ? 1 : 0));
    FIO_cur->fclose(in);    
    MM_heap.active = 0;
    MM_heap.free(sp.data);
    MM_heap.active = h;
	return bt;
 
}
_RLXEXPORTFUNC GXSPRITEGROUP *CSPG_GetFn(char *filename, SYS_FILEIO *f, unsigned option)
{
    GXSPRITEGROUP *s = (GXSPRITEGROUP*) MM_heap.malloc(sizeof(GXSPRITEGROUP));
	char bitname[256];	
	char spcname[256];
	FIO_cur = f;	
	SYS_ASSERT(*GX.csp_cfg.ext);
	sprintf(bitname, "%s.%s", filename, GX.csp_cfg.ext);
	sprintf(spcname, "%s.spc", filename);
	CSPG_GetES(s, bitname, spcname, option);
    return s;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CSP_DrawRect(int32_t x, int32_t y, int32_t lx, int32_t ly, GXSPRITE *sp)
*
* DESCRIPTION :
*
*/
_RLXEXPORTFUNC void CSP_DrawRect(int32_t x, int32_t y, int32_t lx, int32_t ly, GXSPRITE *sp)
{
    int32_t nx=lx/sp->LX, ny=ly/sp->LY, i, j;
    GXVIEWPORT OLD=GX.View;
    GX.View.xmin=x;
    GX.View.ymin=y;
    GX.View.xmax=x+lx;
    GX.View.ymax=y+ly;
    for (i=0;i<=nx;i++) for(j=0;j<=ny;j++) GX.csp.pset(x+i*sp->LX, y+j*sp->LY, sp);
    GX.View=OLD;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : int32_t realan(char *texte, GXSPRITEGROUP *Fonte)
*
* DESCRIPTION :
*
*/
_RLXEXPORTFUNC int32_t CSPG_TxLen(const char *texte, const GXSPRITEGROUP *Fonte)
{
    int32_t ll=0, llx = (Fonte->item[0].LX>>1);
    while ((*texte)!=0)
    {
        int32_t a = SpriteGroup_Car2Val(*texte, Fonte->Caps&1);
        ll += ((a >= Fonte->maxItem)||(a < 0))
		   ? Fonte->HSpacing + llx
		   : Fonte->HSpacing + (Fonte->item[a].LX);
        texte++;
    }
    return ll ;
}

_RLXEXPORTFUNC int32_t CSPG_TxLenS(const char *texte, int32_t fx, GXSPRITEGROUP *Fonte)
{
    int32_t ll=0, llx = (VMUL(fx, Fonte->item[0].LX)>>1);
    while ((*texte)!=0)
    {
        int32_t a = SpriteGroup_Car2Val(*texte, Fonte->Caps&1) ;
        ll += ((a >= Fonte->maxItem)||(a < 0)) 
			? Fonte->HSpacing + llx
			: Fonte->HSpacing +  VMUL(fx, Fonte->item[a].LX);
        texte++;
    }
    return ll ;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
_RLXEXPORTFUNC void CSP_DrawText(const char *texte, int32_t xx, int32_t yy, const GXSPRITEGROUP *Fonte, CSP_FUNCTION spz)
{
    int32_t a, l, ox=xx;
    GXSPRITE *sp0 = Fonte->item + 0, *sp;
    while ((*texte)!=0)
    {
        switch(*texte) {
            case '\n':
            case '³':
            xx = ox;
            yy += Fonte->VSpacing + sp0->LY ;
            break;
            default:
            a = SpriteGroup_Car2Val(*texte, Fonte->Caps&1);
            if ((a >= Fonte->maxItem)||(a < 0)) l = Fonte->HSpacing + (sp0->LX>>1);
            else
            {
                sp = Fonte->item + a;
                l = Fonte->HSpacing + sp->LX;
                if (Fonte->Caps&2) xx += sp0->LX - sp->LX;
                spz.fonct(xx, yy, sp);
            }
            xx+=l;
            break;
        }
        texte++;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CSP_DrawTextC(char *texte, int xx, int yy, int attr1, int attr2, GXSPRITEGROUP *Fonte, CSP_Ptr spz)
*
* DESCRIPTION :
*
*/
_RLXEXPORTFUNC void CSP_DrawTextC(const char *texte, int xx, int yy, int attr1, int attr2, const  GXSPRITEGROUP *Fonte, CSP_FUNCTION spz)
{
    int32_t a, l, ox=xx, col=0;
    GXSPRITE *sp0=Fonte->item+0, *sp;
    CSP_Color(attr1);
    while ((*texte)!=0)
    {
        switch (*texte) {
            case '~':
            col^=1;
            CSP_Color(col ? attr2 : attr1);
            break;
            case '\n':
            case '³':
            xx = ox;
            yy += Fonte->HSpacing + sp0->LY;
            break;
            case '©':
            {
                int tz = 10*sp0->LX; // Taille tabulation
                int bz = (xx-ox)/tz;// Nombre de tabulation
                xx = ox + (bz+1)*tz;      // Skip
            }
            break;
            default:
            a = SpriteGroup_Car2Val(*texte, Fonte->Caps&1) ;
            if ((a >= Fonte->maxItem)||(a < 0)) l = Fonte->HSpacing + (sp0->LX>>1);
            else
            {
                sp = Fonte->item + a;
                l = Fonte->HSpacing + sp->LX;
                if (Fonte->Caps&2) xx+=sp0->LX-sp->LX;
                spz.fonct(xx, yy, sp);
            }
            xx+=l;
            break;
        }
        texte++;
    }
    return;
}

_RLXEXPORTFUNC void CSP_ZoomText(const char *texte, int32_t xx, int32_t yy, int32_t sx, int32_t sy, const GXSPRITEGROUP *Fonte, CSP_FUNCTION func)
{
    int32_t a, l, ox=xx, lx0, ly0, lx, ly;
    const char *otexte=texte;
    GXSPRITE *sp;
    lx0 = VMUL(Fonte->item[0].LX, sx);
    ly0 = VMUL(Fonte->item[0].LY, sx);
    while ((*texte)!='\0')
    {
        switch(*texte) {
            case '\n':
            case '³':
            xx = ox;
            yy += Fonte->HSpacing + ly0;
            break;
            case '©':
            xx = ox + (texte-otexte)*lx0;
            break;
            default:
            a = SpriteGroup_Car2Val(*texte, Fonte->Caps&1);
            if ((a>=Fonte->maxItem)||(a < 0)) l = Fonte->HSpacing + (lx0>>1);
            else
            {
                sp = Fonte->item+a;
                lx = VMUL(sp->LX, sx);
                ly = VMUL(sp->LY, sy);
                l = Fonte->HSpacing + lx;
                if (Fonte->Caps&2) xx+=lx0-lx;
                func.zoomf(sp, xx, yy, lx, ly);
            }
            xx+=l;
            break;
        }
        texte++;
    }
    return;
}
_RLXEXPORTFUNC GXSPRITE *CSP_GetFn(const char *filename, unsigned option)
{
    SYS_FILEHANDLE in;
    int32_t dp = RLX.V3X.Id == RLX3D_OPENGL ? 4 : GX.View.BytePerPixel;
    GXSPRITE *sp=(GXSPRITE *) MM_heap.malloc(sizeof(GXSPRITE));
    in = FIO_cur->fopen(filename, "rb");
    if( in )
    {
        int b = IMG_LoadFp(filename, in, sp);
        int bp = (b+1)>>3;
        if (sp->data)
        {
            if (option&CSPLOAD_POSTERIZE)
            {
                sp->data = RGB_SmartConverter(
                NULL, 
                GX.ColorTable, 
                dp, 
                sp->data, 
                GX.ColorTable, 
                bp, 
                sp->LX * sp->LY);
            }
            if (option&CSPLOAD_SURFACE) 
				GX.Client->UploadSprite(sp, GX.ColorTable, bp);
            FIO_cur->fclose(in);
            return sp;
        }
    }
    MM_heap.free(sp);
    FIO_cur->fclose(in);
    UNUSED(option);
    return NULL;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CSP_Resize(GXSPRITE *sp, int lx, int ly, int bpp)
*
* DESCRIPTION :
*
*/
_RLXEXPORTFUNC void CSP_Resize(GXSPRITE *sp, int lx, int ly, int bpp)
{
    u_int32_t sz = lx*ly, 
    sz2 = sz*bpp, 
    ok = (((u_int32_t)MM_heap.PreviousAddress==(u_int32_t)sp->data)&&(MM_heap.active));
    u_int8_t *tmp;
    tmp = (u_int8_t *)MM_heap.malloc(sz2);
    IMG_stretch(sp->data, tmp, sp->LY, ly, sp->LX, lx, bpp);
    if (sz<sp->LX*sp->LY)
    {
        sysMemCpy(sp->data, tmp, sz2);
        MM_heap.free(tmp);
    }
    else
    {
        if (ok)
        {
            memmove(sp->data, tmp, sz2);
            MM_heap.free(tmp);
        }
        else
        {
            MM_heap.free(sp->data);
            sp->data = tmp;
        }
    }
    sp->LX =lx;
    sp->LY =ly;
    return;
}
