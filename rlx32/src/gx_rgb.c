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
#include <stdlib.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "systools.h"
#include "sysresmx.h"
#include "fixops.h"
#include "gx_struc.h"
#include "gx_init.h"
#include "gx_tools.h"
#include "gx_csp.h"
#include "gx_rgb.h"
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  u_int32_t static *CreateSquareArray(void)
*
* DESCRIPTION :
*
*/
u_int32_t static *CreateSquareArray(void)
{
    static u_int32_t  *pTable=NULL;
    if (pTable)
       return pTable + 255;
    else
    {
        int32_t i;
        //  Should be initialized only once
        pTable = (u_int32_t*)MM_std.malloc(512*sizeof(u_int32_t));
        for (i=0;i<512;i++)
        {
            int32_t j=i-255;
            pTable[i]=j*j;
        }
        return pTable + 255;
    }
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void PAL_fadeChannel(rgb24_t *pf, int st, int fi, int start, int fin, rgb48_t *coul, int revrse)
*
* DESCRIPTION :
*
*/

#define SGN(a)       ((a)==0 ? 0 : (( (a) >0) ? (1) : (-1)))

void PAL_fadeChannel(rgb24_t *pf, int st, int fi, int start, int fin, rgb48_t *coul, int revrse)
{
    u_int8_t *palfade = (u_int8_t*)pf;
    u_int8_t *Pa2 = (u_int8_t*) GX.ColorTables[0];
    u_int8_t  mapalet[768];
    u_int8_t  *mpal, *palf, *Inv=&GX.DefaultColor.r;
    int32_t   j, i, l, speedfade=SGN(fin-start), k;
    i=start;
    do
    {
        if (GX.View.BytePerPixel==1)
        {
            mpal = mapalet+3*st;
            palf = palfade+3*st;
            for (j=st;j<=fi;j++)
            {
                unsigned *c;
                for (c=&coul->rouge, l=0;l<3;l++, mpal++, palf++, c++)
                {
                    k=((int32_t)(*palf)*i)/(*c);
                    switch(revrse) {
                        case 0:
                        *mpal= (u_int8_t)k;
                        break;
                        case 1:
                        *mpal= (u_int8_t)((*palf)+ (((Inv[l]-(int32_t)(*palf))*i)/(*c) )) ;
                        break;
                        case 2:
                        *mpal = (u_int8_t)(Inv[l]-k);
                        break;
                        default:
                        *mpal= (u_int8_t)((*palf)+(i*(Pa2[3*j+l]-(*palf))/(*c)));
                        break;
                    }
                }
            }
            if (RLX.Video.Gamma)
            for (j=0;j<768;j++)
            {
                k=mapalet[j]+RLX.Video.Gamma*8;
                if (k>255) k=255;
                mapalet[j]=(u_int8_t)k;
            }
            i+=speedfade;
            // RGB Texture ambient color
        }
        else
        {
            unsigned *c = &coul->rouge;
            u_int8_t    *b = &GX.AmbientColor.r;
            u_int8_t    *d = &GX.DefaultColor.r;
            i+=speedfade;
            for (j=0;j<3;j++, c++, b++, d++)
            {
                if (revrse) k = 128+ ((((unsigned)*d-128)*i)/(*c) ) ;
                else        k = (i<<7)/(unsigned)*c;
                k+=RLX.Video.Gamma*2;
                if (k>255) k=255;
                *b = (u_int8_t)k;
            }
        }
        GX.gi.setPalette((u_int32_t)st, (u_int32_t)(fi-st+1), mapalet+3*st);
    }while (i!=fin);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void PAL_cycle(rgb24_t *pf, int start, int fin)
*
* DESCRIPTION :
*
*/
void PAL_cycle(rgb24_t *pf, int start, int fin)
{
    u_int8_t *palfade = (u_int8_t*)pf;
    u_int8_t *st=palfade+start*3, *fi=palfade+fin*3;
    u_int8_t
    r=*(st), 
    g=*(st+1), 
    b=*(st+2);
    sysMemCpy(st, st+3, (fin-start)*3);
    *(fi) =r;
    *(fi+1)=g;
    *(fi+2)=b;
    GX.gi.setPalette((u_int32_t)start, (u_int32_t)(fin-start+1), st);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void PAL_fadeChannel(u_int8_t *palfade, int st, int fi, int start, int fin, rgb48_t coul, u_int8_t revrse)
*
* DESCRIPTION :
*
*/
void PAL_fade(rgb24_t *pf, int pal_start, int pal_fin, int start, int fin, int echelle, int revrse)
{
    rgb48_t coul;
    coul.rouge=coul.bleu=coul.vert=echelle;
    PAL_fadeChannel(pf, pal_start, pal_fin, start, fin, &coul, revrse);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void PAL_fadeChannel(u_int8_t *palfade, int st, int fi, int start, int fin, RGB coul, u_int8_t revrse)
*
* DESCRIPTION :
*
*/
void PAL_fading(rgb24_t *palfade, int start, int fin, int echelle, int revrse)
{
    rgb48_t coul;
    coul.bleu=coul.vert=coul.rouge=echelle;
    PAL_fadeChannel(palfade, 0, 255, start, fin, &coul, revrse);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void ACT_LoadFn(char *filename2, char *mode2, FIO_cur->fopen f)
*
* DESCRIPTION :
*
*/
void ACT_LoadFp(rgb24_t *pal, SYS_FILEHANDLE in)
{
    int i, j;
    u_int8_t *tmp;
    tmp = (u_int8_t*) MM_heap.malloc(256*sizeof(rgb24_t));
    FIO_cur->fread(tmp, sizeof(rgb24_t), 256, in);
    for(i=0;i<256;i++)
    {
        j=255-i;
        pal[i].r=tmp[j*3+0];
        pal[i].g=tmp[j*3+1];
        pal[i].b=tmp[j*3+2];
    }
    MM_heap.free(tmp);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void ACT_LoadFn(rgb24_t *pal, char *filename2, char *mode2)
*
* DESCRIPTION :
*
*/
void ACT_LoadFn(rgb24_t *pal, char *filename2)
{
    SYS_FILEHANDLE in;
    in = FIO_cur->fopen(filename2, "rb");
    if (in!=NULL)
    {
        ACT_LoadFp(pal, in);
        FIO_cur->fclose(in);
    }
    return;
}

void RGBA_to_RGB(rgb32_t *tab, unsigned size)
{
	for (;size!=0;tab++, size--)
	{
		tab->r = (u_int8_t)xMUL8(tab->a, tab->r);
		tab->g = (u_int8_t)xMUL8(tab->a, tab->g);
		tab->b = (u_int8_t)xMUL8(tab->a, tab->b);
		tab->a = 0;
	}
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : u_int32_t RGBconvert(int c, u_int8_t *palette)
*
* DESCRIPTION :
*
*/
u_int32_t RGB_convert(int c, rgb24_t *palette)
{
    rgb24_t *p = palette+c;
    switch(GX.View.BytePerPixel){
        case 1:
        return c;
        default:
        return RGB_PixelFormat((u_int32_t)p->r, (u_int32_t)p->g, (u_int32_t)p->b);
    }
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RGB_GetPixelFormat(rgb24_t *rgb, u_int32_t c)
*
* DESCRIPTION :
*
*/
unsigned RGB_SetPixelFormat(int r, int g, int b)
{
	return RGB_PixelFormat(r,g,b);
}
void RGB_GetPixelFormat(rgb24_t *rgb, u_int32_t c)
{
    rgb->r = (u_int8_t)(((c>>GX.View.ColorMask.RedFieldPosition  ) & ((1<<GX.View.ColorMask.RedMaskSize)  -1)) << (8-GX.View.ColorMask.RedMaskSize  ));
    rgb->g = (u_int8_t)(((c>>GX.View.ColorMask.GreenFieldPosition) & ((1<<GX.View.ColorMask.GreenMaskSize)-1)) << (8-GX.View.ColorMask.GreenMaskSize));
    rgb->b = (u_int8_t)(((c>>GX.View.ColorMask.BlueFieldPosition ) & ((1<<GX.View.ColorMask.BlueMaskSize) -1)) << (8-GX.View.ColorMask.BlueMaskSize ));
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  #define RGB565(c) ((c->r>>3) + ((c->g>>2)<<5) + ((c->b>>3)<<11))
*
* Description :
*
*/
#define RGB565(c) ((c->r>>3) + ((c->g>>2)<<5) + ((c->b>>3)<<11))
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  u_int8_t *RGB_SmartConverter(void *tgt, rgb24_t *target_pal, int target_bpp, void *source, rgb24_t *source_pal, int source_bpp, u_int32_t size)
*
* DESCRIPTION :   RGB Color format converter.
*
*/
u_int8_t *RGB_SmartConverter(void *tgt, rgb24_t *target_pal, int target_bpp, void *source, const rgb24_t *source_pal, int source_bpp, u_int32_t size)
{
    u_int8_t *target=NULL;
    int i=size;
    switch(source_bpp){
        case 1: // ============================ SOURCE 8bit ==================
        {
            u_int8_t *a=(u_int8_t*)source;
            switch(target_bpp) {
                case 1:  // remap
                {
                    if (target_pal!=source_pal)
                    {
                        u_int8_t map[256];
                        for (i=0;i<256;i++) map[i]=(u_int8_t)RGB_findNearestColor(source_pal+i, target_pal);
                        for (i=size;i!=0;a++, i--) *a=map[*a];
                    }
                    return (u_int8_t*)source;
                }
                case 2:  // posterize
                {
                    u_int16_t *t=tgt ? (u_int16_t*)tgt : (u_int16_t*)MM_heap.malloc(sizeof(short)*size);
                    u_int16_t sz =  GX.View.ColorMask.RedMaskSize +
                    GX.View.ColorMask.GreenMaskSize +
                    GX.View.ColorMask.BlueMaskSize;
                    target = (u_int8_t*)t;
                    if ((sz==15)||(sz==16))
                    {
                        for (;i!=0;t++, a++, i--)
                        {
                            const rgb24_t *mp = source_pal + (*a);
                            *t=(u_int16_t)(RGB_PixelFormat(mp->r, mp->g, mp->b));
                        }
                    }
                    else
                    {
                        for (;i!=0;t++, a++, i--)
                        {
                            const rgb24_t *mp=source_pal + (*a);
                            *t=(u_int16_t)RGB565(mp);
                        }
                    }
                }
                break;
                case 3:  // posterize
                {
                    rgb24_t *t=tgt ? (rgb24_t*)tgt : (rgb24_t*)MM_heap.malloc(sizeof(rgb24_t)*size);
                    target = (u_int8_t*)t;

					SYS_ASSERT(source);
					SYS_ASSERT(source_pal);

                    for (;i!=0;t++, a++, i--)
                    {
                        const rgb24_t *mp=source_pal + (*a);
                        t->r = GX.View.ColorMask.RedFieldPosition&&(GX.View.BytePerPixel>=3) ? mp->b : mp->r;
                        t->g = mp->g;
                        t->b = GX.View.ColorMask.RedFieldPosition&&(GX.View.BytePerPixel>=3) ? mp->r : mp->b;
                    }
                }
                break;
                case 4:  // posterize
                {
                    rgb32_t *t=tgt ? (rgb32_t*)tgt : (rgb32_t*)MM_heap.malloc(sizeof(rgb32_t)*size);
                    target = (u_int8_t*)t;
                    for (;i!=0;t++, a++, i--)
                    {
                        const rgb24_t *mp = source_pal + (*a);
                        t->r = GX.View.ColorMask.RedFieldPosition&&(GX.View.BytePerPixel>=3) ? mp->b : mp->r;
                        t->g = mp->g;
                        t->b = GX.View.ColorMask.RedFieldPosition&&(GX.View.BytePerPixel>=3) ? mp->r : mp->b;
                        t->a = (*a) ? 255 : 0;
                    }
                }
                break;
            }
        }
        break;
        case 2: // ============================ SOURCE 16bit ==================
        {
            u_int16_t *a=(u_int16_t*)source;
            switch(target_bpp) {
                case 1:  // redusize
                {
                    u_int8_t *t;
                    target = tgt ?  (u_int8_t*)tgt : (u_int8_t*)MM_heap.malloc(size);
                    t = target;
                    if (target_pal)
                    {
                        for (;i!=0;a++, t++, i--)
                        {
                            rgb24_t x;
                            RGB_GetPixelFormat(&x, *a);
                            *t = (u_int8_t)RGB_findNearestColor(&x, source_pal);
                        }
                    }
                    else
                    {
                        for (;i!=0;a++, t++, i--)
                        {
                            rgb24_t x;
                            RGB_GetPixelFormat(&x, *a);
                            *t=(u_int8_t)RGB_332(x);
                        }
                    }
                }
                break;
                case 2:  //
                return (u_int8_t*)source;
                case 3:  // posterize special
                {
                    rgb24_t *t;
                    target = tgt ? (u_int8_t*)tgt : (u_int8_t*)MM_heap.malloc(size*sizeof(rgb24_t));
                    t = (rgb24_t*)target;
                    for (;i!=0;a++, t++, i--)
                    {
                        RGB_GetPixelFormat(t, *a);
                    }
                }
                break;
                case 4:  // posterize special
                {
                    rgb32_t *t;
                    target = tgt ? (u_int8_t*)tgt : (u_int8_t*)MM_heap.malloc(size*sizeof(rgb32_t));
                    t = (rgb32_t*)target;
                    for (;i!=0;a++, t++, i--)      {
                        RGB_GetPixelFormat((rgb24_t*)t, *a);
                        t->a = (u_int8_t)RGB_ToGray(t->r, t->b, t->g);
                    }
                }
                break;
            }
        }
        break;
        case 3: // ============================ SOURCE 24bit ==================
        {
            rgb24_t *a=(rgb24_t *)source;
            switch(target_bpp) {
                case 1:  // redusize
                {
                    u_int8_t *t;
                    target = tgt ?  (u_int8_t*)tgt : (u_int8_t*)MM_heap.malloc(size);
                    t = target;
                    if (target_pal)
                    {
                        for (;i!=0;a++, t++, i--) *t=(u_int8_t)RGB_findNearestColor(a, target_pal);
                    }
                    else
                    {
                        for (;i!=0;a++, t++, i--) *t=(u_int8_t)RGB_332(*a);
                    }
                }
                break;
                case 2:  // posterize special
                {
                    u_int16_t *t;
                    target = tgt ?  (u_int8_t*)tgt : (u_int8_t*)MM_heap.malloc(size*2);
                    t = (u_int16_t*)target;
                    for (;i!=0;a++, t++, i--)
                    {
                        *t = (u_int16_t)RGB_PixelFormat(a->r, a->g, a->b);
                    }
                }
                break;
                case 3:  //
                return (u_int8_t*)source;
                case 4:  // posterize special
                {
                    rgb32_t *t;
                    target = tgt ?  (u_int8_t*)tgt : (u_int8_t*)MM_heap.malloc(size*sizeof(rgb32_t));
                    t = (rgb32_t*)target;
                    for (;i!=0;a++, t++, i--)
                    {
						t->r = GX.View.ColorMask.RedFieldPosition ? a->b : a->r;
                        t->g = a->g;
						t->b = GX.View.ColorMask.RedFieldPosition ? a->r : a->b;
						t->a = a->r>1 || a->b>1 || a->g>1 ? 255 : 0;
                    }
                }
                break;
            }
        }
        break;
        case 4: // ============================ SOURCE 32bit ==================
        {
            rgb32_t *a=(rgb32_t *)source;
            switch(target_bpp) {
                case 1:
                {
                    u_int8_t *t;
                    target = tgt ?  (u_int8_t*)tgt : (u_int8_t*)MM_heap.malloc(size);
                    t = target;
                    if (target_pal)
                    {
                        for (;i!=0;a++, t++, i--) *t=(u_int8_t)RGB_findNearestColor((rgb24_t*)a, target_pal);
                    }
                    else
                    {
                        for (;i!=0;a++, t++, i--) *t=(u_int8_t)RGB_332(*a);
                    }
                }
                break;
                case 2:  // posterize special
                {
                    u_int16_t *t;
                    target = tgt ?  (u_int8_t*)tgt : (u_int8_t*)MM_heap.malloc(size*2);
                    t = (u_int16_t*)target;
                    for (;i!=0;a++, t++, i--)
                    {
                        *t = (u_int16_t)RGBA_PixelFormat(
							GX.View.ColorMask.RedFieldPosition ? a->b : a->r, 
							a->g, 
							GX.View.ColorMask.RedFieldPosition ? a->r : a->b, 
							a->a);
                    }
                }
                break;
                case 3:  // posterize special
                {
                    rgb24_t *t;
                    target = tgt ?  (u_int8_t*)tgt : (u_int8_t*)MM_heap.malloc(size*sizeof(rgb24_t));
                    t = (rgb24_t*)target;
                    for (;i!=0;a++, t++, i--)
                    {
                        t->r = a->r;
                        t->g = a->g;
                        t->b = a->b;
                    }
                }
                break;
                case 4:
                return (u_int8_t*)source;
            }
        }
        break;
        default:
        return (u_int8_t*)source;
    }
    if (!tgt) MM_heap.free(source);
    return target;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static void RGB_Blur(GXSPRITE *sp)
{
    u_int32_t i, size=sp->LX*(sp->LY-2);
    rgb24_t *edi=(rgb24_t*)sp->data, *a, *b, *c, *d, *e, *esi;
    i = sizeof(rgb24_t)*sp->LX*sp->LY;
    esi = (rgb24_t*) MM_heap.malloc(i);
    sysMemCpy(esi, edi, i);
    a = edi+=sp->LX;
    b = esi+1+sp->LX;
    c = esi-1+sp->LX;
    d = esi-sp->LX+sp->LX;
    e = esi+sp->LX+sp->LX;
    for (i=size;i!=0;a++, b++, c++, d++, e++, i--)
    {
        a->r = (u_int8_t)(((u_int32_t)b->r + (u_int32_t)c->r + (u_int32_t)d->r + (u_int32_t)e->r)>>2);
        a->g = (u_int8_t)(((u_int32_t)b->g + (u_int32_t)c->g + (u_int32_t)d->g + (u_int32_t)e->g)>>2);
        a->b = (u_int8_t)(((u_int32_t)b->b + (u_int32_t)c->b + (u_int32_t)d->b + (u_int32_t)e->b)>>2);
    }
    MM_heap.free(esi);
    return ;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void RGBA_Blur(GXSPRITE *sp)
*
* DESCRIPTION :
*
*/
static void RGBA_Blur(GXSPRITE *sp)
{
    u_int32_t i, size=sp->LX*(sp->LY-2);
    rgb32_t *edi=(rgb32_t*)sp->data, *a, *b, *c, *d, *e;
    edi+=sp->LX;
    a = edi;
    b = edi+1;
    c = edi-1;
    d = edi-sp->LX;
    e = edi+sp->LX;
    for (i=size;i!=0;a++, b++, c++, d++, e++, i--)
    {
        a->r = (u_int8_t)(((u_int32_t)b->r + (u_int32_t)c->r + (u_int32_t)d->r + (u_int32_t)e->r)>>2);
        a->g = (u_int8_t)(((u_int32_t)b->g + (u_int32_t)c->g + (u_int32_t)d->g + (u_int32_t)e->g)>>2);
        a->b = (u_int8_t)(((u_int32_t)b->b + (u_int32_t)c->b + (u_int32_t)d->b + (u_int32_t)e->b)>>2);
    }
    return ;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void RGB_Darker(GXSPRITE *sp, int alpha)
*
* DESCRIPTION :
*
*/
static void RGB_Darker(GXSPRITE *sp, int alpha)
{
    u_int32_t i, size=sp->LX*sp->LY;
    rgb24_t *a=(rgb24_t*)sp->data;
    for (i=size;i!=0;a++, i--)
    {
        a->r = (u_int8_t)((a->r * alpha)>>8);
        a->g = (u_int8_t)((a->g * alpha)>>8);
        a->b = (u_int8_t)((a->b * alpha)>>8);
    }
    return ;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  #define MAT_R(n) (u_int32_t) (Mat[n]->r)
*
* DESCRIPTION :
*
*/
#define MAT_R(n) (u_int32_t) (Mat[n]->r)
#define MAT_G(n) (u_int32_t) (Mat[n]->g)
#define MAT_B(n) (u_int32_t) (Mat[n]->b)
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RGB_AntiAlias(GXSPRITE *sp)
*
* DESCRIPTION :
*
*/
void RGB_AntiAlias(GXSPRITE *sp)
{
    u_int32_t
    lx = sp->LX, 
    i = sizeof(rgb24_t)*lx*sp->LY, 
    size= lx * (sp->LY-2)-2;
    rgb24_t *edi = (rgb24_t*) sp->data,
    *esi = (rgb24_t*) MM_heap.malloc(i), *oesi;
    sysMemCpy(esi, edi, i);
    oesi=esi;
    edi+=lx+1;
    esi+=lx+1;
    for (i=size;i!=0;esi++, edi++, i--)
    {
        rgb24_t *Mat[9];
        Mat[0]=esi-lx-1;  Mat[1]=esi-lx; Mat[2]=esi-lx+1;
        Mat[3]=esi   -1;  Mat[4]=esi   ; Mat[5]=esi   +1;
        Mat[6]=esi+lx-1;  Mat[7]=esi+lx; Mat[8]=esi+lx+1;
        edi->r = (u_int8_t)(( MAT_R(0)   + MAT_R(1)*2 + MAT_R(2)
        + MAT_R(3)*2 + MAT_R(4)*4 + MAT_R(5)*2
        + MAT_R(6)   + MAT_R(7)*2 + MAT_R(8)   )>>4);
        edi->g = (u_int8_t)(( MAT_G(0)   + MAT_G(1)*2 + MAT_G(2)
        + MAT_G(3)*2 + MAT_G(4)*4 + MAT_G(5)*2
        + MAT_G(6)   + MAT_G(7)*2 + MAT_G(8)   )>>4);
        edi->b = (u_int8_t)(( MAT_B(0)   + MAT_B(1)*2 + MAT_B(2)
        + MAT_B(3)*2 + MAT_B(3)*4 + MAT_B(5)*2
        + MAT_B(6)   + MAT_B(6)*2 + MAT_B(8)   )>>4);
    }
    MM_heap.free(oesi);
    return ;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void CSP_AntiAliasRGB(GXSPRITE *sp, int tbpp, int sbpp)
{
    int32_t sz=sp->LX*sp->LY;
    switch(sbpp) {
        case 3: RGB_AntiAlias(sp); break;
        default: // FIXME : gourmand en memoire en mode MM_CALLOC
        {
            int a = MM_heap.active;
            u_int8_t *xp = NULL;
            if (a)  MM_heap.active=0; // conv en mem centrale
            sp->data = (u_int8_t*)RGB_SmartConverter(NULL, GX.ColorTable, 3, sp->data, GX.ColorTable, sbpp, sz);
            RGB_AntiAlias(sp);
            if (a)  {MM_heap.active = a; xp=sp->data;}
            sp->data = (u_int8_t*)RGB_SmartConverter(NULL, GX.ColorTable, tbpp, sp->data, GX.ColorTable, 3, sz);
            if (a)  {MM_std.free(xp);}
        }
        break;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CSP_SmoothRGB(GXSPRITE *sp, int tbpp, int sbpp)
*
* DESCRIPTION :
*
*/
void CSP_SmoothRGB(GXSPRITE *sp, int tbpp, int sbpp)
{
    int32_t sz=sp->LX*sp->LY;
    switch(GX.View.BytePerPixel) {
        case 4: RGBA_Blur(sp); break;
        case 3: RGB_Blur(sp); break;
        default: // FIXME : gourmand en memoire en mode MM_CALLOC
        sp->data = (u_int8_t*)RGB_SmartConverter(NULL, GX.ColorTable, 3, sp->data, GX.ColorTable, sbpp, sz);
        RGB_Blur(sp);
        sp->data = (u_int8_t*)RGB_SmartConverter(NULL, GX.ColorTable, tbpp, sp->data, GX.ColorTable, 3, sz);
        break;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CSP_DarkRGB(GXSPRITE *sp, int alpha, int tbpp, int sbpp)
*
* DESCRIPTION :
*
*/
void CSP_DarkRGB(GXSPRITE *sp, int alpha, int tbpp, int sbpp)
{
    int32_t sz=sp->LX*sp->LY;
    switch(sbpp) {
        case 3: RGB_Darker(sp, alpha);
        default: // FIXME : gourmand en memoire en mode MM_CALLOC
        sp->data = (u_int8_t*)RGB_SmartConverter(NULL, GX.ColorTable, 3, sp->data, GX.ColorTable, sbpp, sz);
        RGB_Darker(sp, alpha);
        sp->data = (u_int8_t*)RGB_SmartConverter(NULL, GX.ColorTable, tbpp, sp->data, GX.ColorTable, 3, sz);
        break;
    }
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  u_int32_t RGB_findNearestColor(rgb24_t *col, rgb24_t *pal)
*
* DESCRIPTION :
*
*/
u_int32_t RGB_findNearestColor(const rgb24_t *col, const rgb24_t *pal)
{
    unsigned d, best_dist=0x7eFFFFFF;
    int i, c=0;
    u_int32_t r, g, b;
    u_int32_t *carre = CreateSquareArray();
    for (i=0;i<256;i++, pal++)
    {
        r = (u_int32_t)pal->r-(u_int32_t)col->r;
        g = (u_int32_t)pal->g-(u_int32_t)col->g;
        b = (u_int32_t)pal->b-(u_int32_t)col->b;
        d = RGB_ToGray(carre[r], carre[g], carre[b]);
        if (d<best_dist)
        {
            best_dist=d;
            c = i;
        }
    }
    return c;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CSP_Remap8bit(GXSPRITE *sp, rgb24_t *oldpal, rgb24_t *newpal)
*
* DESCRIPTION :
*
*/
void CSP_Remap8bit(GXSPRITE *sp, rgb24_t *oldpal, rgb24_t *newpal)
{
    RGB_SmartConverter(NULL, newpal, 1, sp->data, oldpal, 1, sp->LX*sp->LY);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  u_int32_t RGB_PixelFormatEx(rgb24_t *p)
*
* Description :  
*
*/
u_int32_t RGB_PixelFormatEx(rgb24_t *p)
{
    switch(GX.View.BytePerPixel){
        case 1:
        return RGB_findNearestColor(p, GX.ColorTable);
        default:
        return RGB_PixelFormat((u_int32_t)p->r, (u_int32_t)p->g, (u_int32_t)p->b);
    }
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : u_int8_t **load_realcolor(char *xpal, char *res)
*
* DESCRIPTION :
*
*/
void REALCOLOR_Reduce(u_int8_t **real, int factor)
{
    int i, x;
    if (real==NULL) return;
    for (i=0;i<256;i++)
    {
        x=i>>factor;x<<=factor;
        if (i!=x)
        {
            real[i]=real[x];
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int REALCOLOR_Simply(u_int8_t **real)
*
* DESCRIPTION :
*
*/
int REALCOLOR_Simply(u_int8_t **real)
{
    int i, j, k, s, good=0;
    u_int32_t *a, *b;
    if (real==NULL) return 0;
    for (i=0;i<255;i++)
    {
        for (j=i+1;j<256;j++)
        {
            s=0;
            if (real[i]!=real[j])
            {
                for (a=(u_int32_t*)real[i], b=(u_int32_t*)real[j], k=64;k!=0;a++, b++, k--)
                {
                    if ((*a)!=(*b)) s=1;
                }
                if (s==0)
                {
                    real[j]=real[i];
                    good++;
                }
            }
        }
    }
    return good;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void REALCOLOR_Free(u_int8_t **real)
*
* DESCRIPTION :
*
*/
void REALCOLOR_Free(u_int8_t **real)
{
    if (real==NULL) return;
    MM_heap.free(real[0]);
    MM_heap.free(real);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  u_int8_t **REALCOLOR_LoadFn(char *xpal, char *res)
*
* DESCRIPTION :
*
*/
u_int8_t **REALCOLOR_LoadFn(const char *xpal)
{
    int i;
    u_int8_t **pe;
    GXSPRITE sp;
    if (!IMG_LoadFn(xpal, &sp)) return NULL;
    pe = (u_int8_t**) MM_heap.malloc(256*sizeof(u_int8_t*));
    for(i=0;i<256;i++) pe[i] = sp.data+i*256;
    return pe;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : u_int8_t *create_shadepal(u_int8_t *pal, int r, int g, int b, int mode)
*
* DESCRIPTION :cherche dans pal, les couleur les + proches dans GX.ColorTable
*
*/
static void REALCOLOR_RgbMul(rgb24_t *res, rgb24_t *a, rgb24_t *b)
{
    res->r = (u_int8_t)(((u_int32_t)a->r * (u_int32_t)b->r )>>8);
    res->g = (u_int8_t)(((u_int32_t)a->g * (u_int32_t)b->g )>>8);
    res->b = (u_int8_t)(((u_int32_t)a->b * (u_int32_t)b->b )>>8);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void REALCOLOR_RgbAdd(rgb24_t *res, rgb24_t *a, rgb24_t *b)
*
* DESCRIPTION :
*
*/
static void REALCOLOR_RgbAdd(rgb24_t *res, rgb24_t *a, rgb24_t *b)
{
    u_int32_t v;
    v = ((u_int32_t)a->r + (u_int32_t)b->r ); res->r = (v<255) ? (u_int8_t)v : (u_int8_t)255;
    v = ((u_int32_t)a->g + (u_int32_t)b->g ); res->g = (v<255) ? (u_int8_t)v : (u_int8_t)255;
    v = ((u_int32_t)a->b + (u_int32_t)b->b ); res->b = (v<255) ? (u_int8_t)v : (u_int8_t)255;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void REALCOLOR_RgbSub(rgb24_t *res, rgb24_t *a, rgb24_t *b)
*
* DESCRIPTION :
*
*/
static void REALCOLOR_RgbSub(rgb24_t *res, rgb24_t *a, rgb24_t *b)
{
    int32_t v;
    v = ((int32_t)a->r - (int32_t)b->r ); res->r = (v>0) ? (u_int8_t)v : (u_int8_t)0;
    v = ((int32_t)a->g - (int32_t)b->g ); res->g = (v>0) ? (u_int8_t)v : (u_int8_t)0;
    v = ((int32_t)a->b - (int32_t)b->b ); res->b = (v>0) ? (u_int8_t)v : (u_int8_t)0;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void REALCOLOR_RgbAlpha(rgb24_t *res, rgb24_t *a, rgb24_t *b, int alpha)
*
* DESCRIPTION :
*
*/
static void REALCOLOR_RgbAlpha(rgb24_t *res, rgb24_t *a, rgb24_t *b, int alpha)
{
    u_int32_t alpha_1 = 256L - (u_int32_t)alpha;
    res->r = (u_int8_t)(((u_int32_t)a->r * alpha + (u_int32_t)b->r * alpha_1)>>8);
    res->g = (u_int8_t)(((u_int32_t)a->g * alpha + (u_int32_t)b->g * alpha_1)>>8);
    res->b = (u_int8_t)(((u_int32_t)a->b * alpha + (u_int32_t)b->b * alpha_1)>>8);
    return;
}
// pal: palette de la map, clr : couleur … mixer  mode, alpha, quantize
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  u_int8_t *REALCOLOR_Compute(rgb24_t *pal, rgb24_t *clr, int mode, int alpha, int quantize)
*
* DESCRIPTION :
*
*/
u_int8_t *REALCOLOR_Compute(rgb24_t *pal, rgb24_t *clr, int mode, int alpha, int quantize)
{
    rgb24_t *pal2=NULL;
    //  Table de correspondance
    if (mode!=REALCOLOR_RgbReal)
    {
        u_int32_t i;
        rgb24_t *ppal = pal, 
        *ppal2;
        pal2 = (rgb24_t*) MM_heap.malloc(256*sizeof(rgb24_t));
        for (ppal2=pal2, 
        i=256;
        i!=0;
        ppal++, 
        ppal2++, 
        i--)
        {
            switch(mode) {
                case REALRGBMIX_Alpha:
                REALCOLOR_RgbAlpha(ppal2, ppal, clr, alpha);
                break;
                case REALRGBMIX_Mul:
                REALCOLOR_RgbMul(ppal2, ppal, clr);
                break;
                case REALRGBMIX_Add:
                REALCOLOR_RgbAdd(ppal2, ppal, clr);
                break;
                case REALRGBMIX_Sub:
                REALCOLOR_RgbSub(ppal2, ppal, clr);
                break;
                case REALRGBMIX_Neg:
                REALCOLOR_RgbSub(ppal2, clr, ppal);
                break;
            }
        }
    }
    switch(quantize) {
        case 1:
        // Recherche des couleurs
        {
            u_int8_t *palmap = (u_int8_t*) MM_heap.malloc(256), *ppal3;
            rgb24_t *ppal2;
            int32_t i;
            if (mode==REALCOLOR_RgbReal) pal2=clr;
            for (i=256,
            ppal3=palmap, 
            ppal2=pal2;
            i!=0;
            ppal2++, 
            ppal3++, 
            i--)
            {
                *ppal3 = (u_int8_t)RGB_findNearestColor(ppal2, pal);
            }
            if (mode!=REALCOLOR_RgbReal)  MM_heap.free(pal2);
            return palmap;
        }
        case 2:
        {
            u_int16_t *pm, *palmap = (u_int16_t*) MM_heap.malloc(256*2);
            rgb24_t *ppal2;
            int32_t i;
            for (i=256, pm=palmap, ppal2=pal2;i!=0;pm++, ppal2++, i--)
            {
                *pm = (u_int16_t)RGB_PixelFormat(ppal2->r, ppal2->g, ppal2->b);
            }
            sysMemCpy(pal2, palmap, 256*2);
            MM_heap.free(palmap);
            return (u_int8_t*)pal2;
        }
        case 3:
        return (u_int8_t*)pal2;
        case 4:
        {
            rgb32_t *palmap = MM_CALLOC(256, rgb32_t);
            rgb24_t *ppal2;
            u_int32_t *pm;
            int32_t i;
            for (i=256, pm=(u_int32_t*)palmap, ppal2=pal2;i!=0;pm++, ppal2++, i--)
            {
                *pm = RGB_PixelFormat(ppal2->r, ppal2->g, ppal2->b);
            }
            return (u_int8_t*)palmap;
        }
    }
    return NULL;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void PAL_SetRedCyanPalette(void)
*
* DESCRIPTION :
*
*/

__extern_c
u_int8_t      *StereoRed, *StereoBlue;
__end_extern_c

void PAL_SetRedCyanPalette(void)
{
    int i, j;
    char bid[768];
    rgb24_t *pnew=GX.ColorTable, *pold=(rgb24_t*)bid;
    sysMemCpy(pold, pnew, 768);
    if (StereoRed==NULL)
    {
        StereoRed = (u_int8_t*)MM_std.malloc(256);
        StereoBlue = (u_int8_t*)MM_std.malloc(256);
    }
    // Nouvelle Palette
    for (i=0;i<16;i++)
    {
        for (j=0;j<16;j++, pnew++)
        {
            pnew->r = (u_int8_t)(j<<4);
            pnew->b = (u_int8_t)(i<<4);
            pnew->g = (u_int8_t)(i<<4);
        }
    }
    for (i=0;i<256;i++, pold++)
    {
        StereoRed[i] = (u_int8_t)(RGB_ToGray(pold->r, pold->g, pold->b)>>4);
        StereoBlue[i] = (u_int8_t)(StereoRed[i]<<4);
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RGB_SetAlphaBit(u_int16_t *mp, int32_t size)
*
* DESCRIPTION :
*
*/
void RGB_SetAlphaBit(u_int16_t *mp, int32_t size)
{
    int32_t i;
    for (i=size;i!=0;mp++, i--)
    {
        if (*mp) (*mp)|=(1L<<15);
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RGB_Build332ColorTable(rgb24_t *lut)
*
* DESCRIPTION :  
*
*/
void RGB_Build332ColorTable(rgb24_t *lut)
{
    unsigned r, g, b;
    for (b=0;b<4;b++) for (g=0;g<8;g++) for (r=0;r<8;r++, lut++)
    {
        RGB_Set(*lut, r<<5, g<<5, b<<6);
    }
    return;
}
