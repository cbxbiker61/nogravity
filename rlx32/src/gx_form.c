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

#include "systools.h"
#include "sysresmx.h"
#include "gx_struc.h"
#include "gx_init.h"
#include "gx_tools.h"
#include "gx_csp.h"
#include "gx_rgb.h"
#include "gx_flc.h"
#include "gx_cdc.h"
#include "_rlx.h"

#define NO_CODE -1
#define PROP    MakeID('P','R','O','P')
#define LIST    MakeID('L','I','S','T')
#define CAT     MakeID('C','A','T',' ')
#define ID_PBM  MakeID('P','B','M',' ')
#define ID_ILBM MakeID('I','L','B','M')
#define ID_BMHD MakeID('B','M','H','D')
#define ID_CMAP MakeID('C','M','A','P')
#define ID_GRAB MakeID('G','R','A','B')
#define ID_DEST MakeID('D','E','S','T')
#define ID_SPRT MakeID('S','P','R','T')
#define ID_CAMG MakeID('C','A','M','G')
#define ID_BODY MakeID('B','O','D','Y')
#define m_fread(dest, taille, size, src) \
{  \
    sysMemCpy(dest, src, taille*size);\
    src+=taille*size;\
}
#define m_fgetc(src) (*(src++))
#define RowBytes(w)   (((w) + 15) >> 4 << 1)

static void gif_doextension(SYS_FILEHANDLE fp)
{
    int32_t n, i;
    do {
        if((n=FIO_cur->fgetc(fp)) != EOF) {
            for(i=0;i<n;++i) FIO_cur->fgetc(fp);
        }
    } while((n > 0) && (n != EOF));
    return;
}
static void img_unpackimage(SYS_FILEHANDLE fp, u_int8_t *buffer, int bits, int width, int depth, int flags)
{
    int32_t bits2, codesize, codesize2, nextcode, thiscode, oldtoken, currentcode, oldcode;
    int32_t bitsleft, blocksize;
    int32_t line=0, bytes=0, pass=0;
    u_int8_t *p, *q;
    u_int8_t b[255];
    u_int8_t *u, *linebuffer, *firstcodestack, *lastcodestack;
    short *codestack;
    short int wordmasktable[] = {
        0x0000, 0x0001, 0x0003, 0x0007, 
        0x000f, 0x001f, 0x003f, 0x007f, 
        0x00ff, 0x01ff, 0x03ff, 0x07ff, 
        0x0fff, 0x1fff, 0x3fff, 0x7fff
    };
    short int inctable[] = { 8, 8, 4, 2, 0 };
    short int startable[] = { 0, 4, 2, 1, 0 };
    p=q=b;
    bitsleft = 8;
    if (bits < 2 || bits > 8) return;
    bits2 = 1 << bits;
    nextcode = bits2 + 2;
    codesize2 = 1 << (codesize = bits + 1);
    oldcode=oldtoken=NO_CODE;
    if( (lastcodestack =(u_int8_t *)MM_std.malloc(4096)) == NULL) return;
    if( (linebuffer =(u_int8_t *)MM_std.malloc(width)) == NULL) return;
    if( (firstcodestack=(u_int8_t *)MM_std.malloc(4096)) == NULL) return;
    if( (codestack =(short *)MM_std.malloc(4096*sizeof(short))) == NULL) return;
    for(;;) {
        if(bitsleft==8) {
            if(++p >= q &&
            (((blocksize = FIO_cur->fgetc(fp)) < 1) ||
            (q=(p=b)+FIO_cur->fread(b, 1, blocksize, fp))<
            (b+blocksize)))
            {
                goto GIFCANCEL;
            }
            bitsleft = 0;
        }
        thiscode = *p;
        if ((currentcode=(codesize+bitsleft)) <= 8)
        {
            *p >>= codesize;
            bitsleft = currentcode;
        }
        else
        {
            if(++p >= q &&
	    (((blocksize = FIO_cur->fgetc(fp)) < 1) || (q=(p=b)+FIO_cur->fread(b, 1, blocksize, fp)) < (b+blocksize)))
            {
                goto GIFCANCEL;
            }
            thiscode |= *p << (8 - bitsleft);
            if(currentcode <= 16) *p >>= (bitsleft=currentcode-8);
            else
            {
                if(++p >= q &&
                (((blocksize = FIO_cur->fgetc(fp)) < 1) || (q=(p=b) + FIO_cur->fread(b, 1, blocksize, fp)) <(b+blocksize)))
                {
                    goto GIFCANCEL;
                }
                thiscode |= *p << (16 - bitsleft);
                *p >>= (bitsleft = currentcode - 16);
            }
        }
        thiscode &= wordmasktable[codesize];
        currentcode = thiscode;
        if(thiscode == (bits2+1)) break;
        if(thiscode > nextcode)
        {
            goto GIFCANCEL;
        }
        if(thiscode == bits2)
        {
            nextcode = bits2 + 2;
            codesize2 = 1 << (codesize = (bits + 1));
            oldtoken = oldcode = NO_CODE;
            continue;
        }
        u = firstcodestack;
        if(thiscode==nextcode)
        {
            if(oldcode==NO_CODE)
            {
                goto GIFCANCEL;
            }
            *u++ = (u_int8_t)oldtoken;
            thiscode = oldcode;
        }
        while (thiscode >= bits2)
        {
            *u++ = lastcodestack[thiscode];
            thiscode = codestack[thiscode];
	}
        oldtoken = thiscode;
        do
        {
            linebuffer[bytes++]=(u_int8_t)thiscode;
            if(bytes >= width)
            {
                sysMemCpy(
                (buffer+(int32_t)line*(int32_t)width), linebuffer, width);
                bytes=0;
                if(flags & 0x40)
                {
                    line+=inctable[pass];
                    if(line >= depth)
                    line=startable[++pass];
                }
                else ++line;
            }
            if (u <= firstcodestack) break;
            thiscode = *--u;
        } while(1);
        if(nextcode < 4096 && oldcode != NO_CODE)
        {
            codestack[nextcode] = (short)oldcode;
            lastcodestack[nextcode] = (u_int8_t)oldtoken;
            if (++nextcode >= codesize2 && codesize < 12)
            codesize2 = 1 << ++codesize;
        }
        oldcode = currentcode;
    }
    GIFCANCEL:
    MM_std.free(codestack);
    MM_std.free(firstcodestack);
    MM_std.free(linebuffer);
    MM_std.free(lastcodestack);
    return;
}
u_int8_t static *GIF_unpack(SYS_FILEHANDLE in, u_int32_t *lx, u_int32_t *ly, int *bpp, rgb24_t *table, int options)
{
    GIF_Header    iblkGIF;
    GIF_TagInfo   gh;
    u_int8_t       *bufferGIF=(u_int8_t*)NULL;
    int32_t b, width=0, depth=0, flags;
    int c;
    FIO_cur->fread(&gh, 13, 1, in);
#ifdef LSB_FIRST
    BSWAP16((u_int16_t*)&gh.screenwidth, 2);
#endif
    *bpp = 8;
    if ((memcmp(gh.sig, "GIF", 3))) 
		return NULL;

    if (gh.flags & 0x80)
    {
        c = 3L * (1L << ((gh.flags & 7) + 1L));
        if (c==16*3) 
			*bpp=4;
        FIO_cur->fread(table, sizeof(u_int8_t), c, in);
    }

    if (options&1)
    {
        c = FIO_cur->fgetc(in);
        while ((c==',') || (c=='!') || (c==0))
        {
            if (c == ',')
            {
                FIO_cur->fread(&iblkGIF, 9, 1, in);
#ifdef LSB_FIRST
                BSWAP16((u_int16_t*)&iblkGIF.left, 4);
#endif
                *lx = iblkGIF.width;
                *ly = iblkGIF.depth;
                width = iblkGIF.width;
                depth = iblkGIF.depth;
                if (iblkGIF.flags & (u_int8_t)0x80)
                {
                    b = 3*(1<<((iblkGIF.flags & 0x0007) + 1));
                    if (b==16*3) *bpp=4;
                    FIO_cur->fread(table, 1, b, in);
                }
                c=FIO_cur->fgetc(in);
                flags = iblkGIF.flags;
                if((bufferGIF = (u_int8_t*) MM_heap.malloc((int32_t)width*(int32_t)depth)) == (u_int8_t*)NULL) return (u_int8_t*)NULL;
                img_unpackimage(in, bufferGIF, c, width, depth, flags);
                return bufferGIF;
            }
	    else
            if(c == '!') 
				gif_doextension(in);
            c = FIO_cur->fgetc(in);
        }
    }
    return NULL;
}

u_int8_t static *ACT_unpack(SYS_FILEHANDLE in, u_int32_t *lx, u_int32_t *ly, int *bpp, rgb24_t *table, int options)
{
    u_int8_t *tb;
    ACT_LoadFp(table, in);
    *ly =   1;
    tb = (u_int8_t*)MM_heap.malloc(256);
    for (*lx=0;*lx<256;(*lx)++) tb[*lx]=(u_int8_t)(*lx);
    *lx = 256;
    *bpp = 8;
    UNUSED(options);
    return tb;
}

IMG_Codec IMG_FormList[]={
    {".gif", GIF_unpack, PNG_pack}, 
    {".png", PNG_unpack, PNG_pack}, 
    {".act", ACT_unpack, PNG_pack}, 
	{NULL, NULL}
};

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void IMG_SaveFp(char *filename, SYS_FILEHANDLE fp, GXSPRITE *sp, int bpp)
*
* DESCRIPTION :  
*
*/
void IMG_SaveFp(const char *filename, SYS_FILEHANDLE fp, GXSPRITE *sp, int bpp)
{
    IMG_Codec *upk=IMG_FormList;
    while (upk->ext!=NULL)
    {
        if (strstr(filename, upk->ext))
        {
            upk->pack(fp, sp->data, sp->LX, sp->LY, bpp, GX.ColorTable);
            return;
        }
        upk++;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int IMG_LoadFp(char *filename, SYS_FILEHANDLE fp, GXSPRITE *sp)
*
* DESCRIPTION :  
*
*/
int IMG_LoadFp(const char *filename, SYS_FILEHANDLE fp, GXSPRITE *sp)
{
    int bpp, op=1;
    GXSPRITE spp;
    IMG_Codec *upk=IMG_FormList;
    if (!sp) 
	{
		sp=&spp;
		op=0;
	}
    sp->data = NULL;
    sp->handle = NULL;
    if (RLX.Video.Config&RLXVIDEO_ConvertImg8bit) 
		op|=2;
    while (upk->ext!=NULL)
    {
        if (strstr(filename, upk->ext))
        {
            sp->data = upk->unpack(fp, &sp->LX, &sp->LY, &bpp, GX.ColorTable, op);
			SYS_ASSERT(sp->data);
            return bpp;
        }
        upk++;
    }
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int IMG_LoadFn(char *filename, char *res, GXSPRITE *sp)
*
* DESCRIPTION :
*
*/
int IMG_LoadFn(const char *filename, GXSPRITE *sp)
{
    SYS_FILEHANDLE fp;
    int bpp;
    FIO_cur=&FIO_res;
    fp = FIO_cur->fopen(filename, "rb");
    if (fp!=NULL)
    {
        bpp = IMG_LoadFp(filename, fp, sp);
        FIO_cur->fclose(fp);
        if ((!sp)||(sp->data!=NULL)) return bpp;
    }
    if (sp) sp->data = NULL;
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void Load_Background(char *filename, int options)
*
* DESCRIPTION :
*
*/
void IMG_LoadFnSprite(char *file, GXSPRITE *sp, int options)
{    
	int32_t dp = GX.View.BytePerPixel;

    if (options==1)
    {
        IMG_LoadFn(file, sp);
        return;
    }

    if (options&2)
    {		
        u_int8_t *p = NULL;
        int act = MM_heap.active, vp;
        MM_heap.active = 0;

        vp = IMG_LoadFn(file, sp);
		if (!vp) 
			return;

        if (vp==4)
			vp=1;

        if (vp>=8) 
			vp>>=3;

        if (options&4) 
			CSP_Resize(sp, GX.View.lWidth, GX.View.lHeight, vp);

        p = sp->data;
        MM_heap.active = act;
        if (options&8)
            sp->data = (u_int8_t*)RGB_SmartConverter(NULL, GX.ColorTable, dp, 
												 sp->data, GX.ColorTable, vp, sp->LX*sp->LY);            
        else
	        CSP_AntiAliasRGB(sp, dp, vp);
    }
    else
    {
        if (!IMG_LoadFn(file, sp)) 
			return;

        if (options&4)
			CSP_Resize(sp, GX.View.lWidth, GX.View.lHeight, dp);
    }
    return;
}
