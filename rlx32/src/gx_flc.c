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

#include <stdlib.h>
#include <stdio.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "systools.h"
#include "sysresmx.h"
#include "systime.h"
#include "gx_struc.h"
#include "gx_init.h"
#include "gx_tools.h"
#include "gx_csp.h"
#include "gx_rgb.h"
#include "gx_flc.h"
#define BUFFERSIZE         65536L

// Authentified flags
#define CT_FLI_BLACK          13
#define CT_FLI_BRUN           15
#define CT_FLI_COPY           16
#define CT_FLI_SS2             7
#define CT_FLI_COLOR8BIT       4
#define CT_FLI_COLOR6BIT      11
// Unauthentified flags
#define CT_FLI_DELTA_FLI      120
#define CT_FLI_MINI           180
#define CT_FLC            0x00A1
// Define
#define memcpy_16(edi, esi, ecx) \
for (;ecx!=0;edi+=2, esi+=2, ecx--) *(uint16_t*)edi=*(uint16_t*)esi;
// Define 2
#define setmem_16(edi, ax, ecx) \
for (;ecx!=0;edi+=2, ecx--) *(uint16_t*)edi=ax;

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C Unpack_FLI_Chunk (uint8_t *Buffer, uint32_t Chunks, uint8_t *output, uint32_t dwWidth, uint32_t dwHeight)
*
* DESCRIPTION :
*
*/
void CALLING_C FLI_ChunkDecode(uint8_t *Buffer, uint32_t Chunks, uint8_t *output, uint32_t dwWidth, uint32_t dwHeight, uint8_t *ColorTable)
{
	char tex[256];
    uint8_t *esi, *edi, *oesi, *oedi;
    uint32_t chunk_size;
    uint16_t ax, bx;
    short cx, dx, dl;
    esi = Buffer;
    while (Chunks!=0)
    {
	Chunks--;
        chunk_size = *(uint32_t*)Buffer;
	ax = *(uint16_t*)(esi+4);
        #ifdef __BIG_ENDIAN__
	BSWAP32(&chunk_size, 1);
	BSWAP16((uint16_t*)&ax, 1);
	#endif
	oesi=esi;
	esi+=6;
	edi=output;
	switch(ax) {
	    case CT_FLI_SS2:
	    bx = *(uint16_t*)esi;
	    #ifdef __BIG_ENDIAN__
	    BSWAP16((uint16_t*)&bx, 1);
	    #endif
	    esi+=2;
	    while(bx!=0)
	    {
		bx--;
		do
		{
		  dx = *(short*)esi;
		  #ifdef __BIG_ENDIAN__
		  BSWAP16((uint16_t*)&dx, 1);
		  #endif
		  esi+=2;
		  if (dx<0) edi+=dwWidth*(-dx);
		}while(dx<0);

		{
		    if (dx>(int)dwHeight)
				dx=0;
		    oedi=edi;
		    while (dx!=0)
		    {
			dx--;
			cx = *(unsigned char*)(esi+0);
			edi+= cx;
			cx = *(signed char*)(esi+1);
			if (cx<0)
			{
			    cx = -cx;
			    ax = *(uint16_t*)(esi+2);
			    #ifdef __BIG_ENDIAN__
			    BSWAP16((uint16_t*)&ax, 1);
			    #endif
			    esi+=4;
			    setmem_16(edi, ax, cx);
			}
			else
			{
			    esi+=2;
			    memcpy_16(edi, esi, cx);
			}
		    }
		    edi=oedi+dwWidth;
		}
	    }
	    break;
	    case CT_FLI_COLOR8BIT:
	    bx = *(uint16_t*)esi;
	    #ifdef __BIG_ENDIAN__
	    BSWAP16((uint16_t*)&bx, 1);
	    #endif
	    esi+=2;
	    for (;bx!=0;bx--)
	    {
		ax = *(esi+0);
		edi = (uint8_t*)(ColorTable+ax);
		cx = *(esi+1);
		if (cx==0) cx=256;
		esi+=2;
		for (cx+=(cx<<1);cx!=0;cx--, edi++, esi++) *edi = *esi;
	    }
	    tex[31]=127;
	    break;
	    case CT_FLI_COLOR6BIT:
	    bx = *(uint16_t*)esi;
	    #ifdef __BIG_ENDIAN__
	    BSWAP16((uint16_t*)&bx, 1);
	    #endif
	    esi+=2;
	    for (;bx!=0;bx--)
	    {
		ax = *(esi+0);
		edi = (uint8_t*)(ColorTable+ax);
		cx = *(esi+1);
		if (cx==0) cx=256;
		esi+=2;
		for (cx+=(cx<<1);cx!=0;cx--, edi++, esi++) *edi = (*esi)<<2;
	    }
	    tex[31]=127;
	    break;
	    case CT_FLI_DELTA_FLI:
	    ax = *(uint16_t*)esi ;
	    #ifdef __BIG_ENDIAN__
	    BSWAP16((uint16_t*)&ax, 1);
	    #endif
	    ax*= (uint16_t)dwWidth;
	    edi = output + ax;
	    bx = *(uint16_t*)(esi+2);
	    #ifdef __BIG_ENDIAN__
	    BSWAP16((uint16_t*)&bx, 1);
	    #endif
	    esi+= 4;
	    while (bx!=0)
	    {
		bx--;
		dl = *(uint8_t*)esi;
		esi++;
		oesi=edi;
		while (dl!=0)
		{
		    dl--;
		    cx = *(signed char*)(esi+0);
		    edi+= cx;
		    cx = *(signed char*)(esi+1);
		    if (cx<0)
		    {
                        ax=*(uint8_t*)(esi+2);
			esi+=3;
                        cx=-cx;
			sysMemSet(edi, ax, cx);
		    }
		    else
		    {
			esi+=2;
			sysMemCpy(edi, esi, cx);
			esi+=cx; // Non fait par sysMemCpy
		    }
		    edi+=cx;
		}
		edi=oesi+dwWidth;
	    }
	    break;
	    case CT_FLI_BRUN:
	    bx = (uint16_t)dwHeight;
	    do
	    {
		dl = *(uint8_t*)esi;
		esi++;
		oedi = edi;
		while(dl!=0)
		{
		    dl--;
		    cx= *(signed char*)esi;
		    if (cx>=0)
		    {
			ax = *(uint8_t*)(esi+1);
			esi+=2;
			sysMemSet(edi, ax, cx);
		    }
		    else
		    {
			esi++;
			cx=-cx;
			sysMemCpy(edi, esi, cx);
			esi+=cx;
		    }
		    edi+=cx; // Non fait pas sysMemSet et setmem
		}
		edi = oedi + dwWidth;
		bx--;
	    }while (bx!=0);
	    break;
	    case CT_FLI_COPY:
	    sysMemCpy(output, esi, dwHeight*dwWidth);
	    break;
	    case CT_FLI_BLACK:
	    sysMemZero(output, dwHeight*dwWidth);
	    break;
	    default:
	    break;
	}
	esi=oesi+chunk_size;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int LOCAL DefaultDecode(unsigned id, void *data, uint32_t size)
*
* DESCRIPTION :
*
*/
uint32_t static DefaultDecode(unsigned id, void *data, uint32_t size)
{
    UNUSED(id);
    UNUSED(data);
    UNUSED(size);
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  FLI_STRUCT *FLI_Open(SYS_FILEHANDLE in, int md)
*
* DESCRIPTION :
*
*/
_RLXEXPORTFUNC FLI_STRUCT *FLI_Open( SYS_FILEHANDLE in, int md)
{
    uint32_t memleft, s;
    FLI_STRUCT *pAnim;
    pAnim = (FLI_STRUCT*) MM_heap.malloc(sizeof(FLI_STRUCT));
    pAnim->fli_stream = in;
    if (md&FLI_LZWPACKED) FIO_cur = &FIO_gzip;
    pAnim->start = FIO_cur->ftell( pAnim->fli_stream ) + 128L;
    FIO_cur->fread(pAnim->Header.Raw, sizeof(char), 128, in);
    #ifdef __BIG_ENDIAN__
    BSWAP32((uint32_t*)&pAnim->Header.Struct.size, 1);
    BSWAP16(&pAnim->Header.Struct.type, 6);
    BSWAP32((uint32_t*)&pAnim->Header.Struct.frame1, 2);
    BSWAP32((uint32_t*)&pAnim->Header.Struct.speed, 1);
    #endif
    if (pAnim->Header.Struct.type!=0xAF11)  // FLI
    if (pAnim->Header.Struct.type!=0xAF12)  // FLC
    {
        MM_heap.free(pAnim);
        return NULL;
    }
    s = pAnim->Header.Struct.width*pAnim->Header.Struct.height;
    pAnim->decompBuffer = (uint8_t*) MM_heap.malloc(s) ;
    pAnim->MaximumFrame = pAnim->Header.Struct.Chunks;
    pAnim->bitmap.LY = pAnim->Header.Struct.height;
    pAnim->bitmap.LX = pAnim->Header.Struct.width;
    pAnim->bitmap.data = pAnim->decompBuffer;
    pAnim->LastTime = timer_ms();
    pAnim->Flags       |= FLX_DECOMPRESSFRAME + FLX_LOOPANIMATION + FLX_ISPLAYING;
    pAnim->CurrentFrame = 0;

    memleft = md&FLI_USEMEMORY ? 1<<31 : 0;
    if (md&FLI_LZWPACKED)
		memleft = 0;
    if (md&FLI_EXPANDED)
    {
        int i;
        GXSPRITE *sp;
        pAnim->frames = MM_CALLOC(pAnim->MaximumFrame, GXSPRITE);
        for (i=pAnim->MaximumFrame, sp = pAnim->frames;i!=0;sp++, i--)
        {
            sp->data = (uint8_t*) MM_heap.malloc(pAnim->bitmap.LX*pAnim->bitmap.LY);
            sp->LX = pAnim->bitmap.LX;
            sp->LY = pAnim->bitmap.LY;
        }
        memleft = 0;
    }
    if (memleft>(uint32_t)(pAnim->Header.Struct.size+BUFFERSIZE))
    {
        pAnim->fileBuffer = (uint8_t*)MM_heap.malloc(pAnim->Header.Struct.size);
        pAnim->ReadMode = FLI_USEMEMORY;
        pAnim->start = 0;
    }
    else
    {
        pAnim->fileBuffer = (uint8_t*)MM_heap.malloc(BUFFERSIZE);
        pAnim->ReadMode = (md&FLI_LZWPACKED) ? FLI_LZWPACKED : FLI_DIRECTFROMDISK;
    }
    if (pAnim->Header.Struct.type==0xAF12)
    {
        pAnim->start = pAnim->Header.Struct.frame1-128L;
        pAnim->start_buf = FIO_cur->ftell(pAnim->fli_stream);
    }
    if (pAnim->ReadMode==FLI_USEMEMORY)
    {
        FIO_cur->fread(pAnim->fileBuffer, sizeof(uint8_t), pAnim->Header.Struct.size-128L-pAnim->start, pAnim->fli_stream);
        pAnim->start_buf = (uint32_t)pAnim->fileBuffer;
        FIO_cur->fclose(pAnim->fli_stream);
        pAnim->fli_stream = NULL;
    }
    if (md&FLI_EXPANDED)
    {
        int i;
        GXSPRITE *sp;
        uint8_t *temp = pAnim->decompBuffer;
        pAnim->Flags|=FLX_ISPLAYING;
        for (i=pAnim->MaximumFrame, sp= pAnim->frames;i!=0;sp++, i--)
        {
            sysMemCpy(sp->data, pAnim->decompBuffer, s);
            pAnim->decompBuffer = sp->data;
            FLI_Unpack(pAnim);
        }
        MM_heap.free(temp);
        MM_heap.free(pAnim->fileBuffer);
        pAnim->decompBuffer = NULL;
        pAnim->fileBuffer = NULL;
        pAnim->ReadMode = FLI_EXPANDED;
        FLI_Rewind(pAnim);
    }
    pAnim->Flags|=FLX_RESETPALETTE;
    return pAnim;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void FLI_Close(FLI_STRUCT *pAnim)
*
* DESCRIPTION : Ferme un Flc et libere la memoire
*
*/
_RLXEXPORTFUNC void FLI_Close(FLI_STRUCT *pAnim)
{
    switch(pAnim->ReadMode){
        case FLI_USEMEMORY:
        pAnim->fileBuffer = (uint8_t*)pAnim->start_buf;
        pAnim->start = 0;
        break;
        case FLI_LZWPACKED:
		FIO_cur = &FIO_res;
        FIO_gzip.fclose(pAnim->fli_stream);
		pAnim->fli_stream = NULL;
		pAnim->ReadMode = 255;
        break;
        case FLI_DIRECTFROMDISK:
        FIO_cur->fclose(pAnim->fli_stream);
        break;
        case FLI_EXPANDED:
        {
            int i;
            GXSPRITE *sp;
            for (i=pAnim->MaximumFrame, sp= pAnim->frames;i!=0;sp++, i--) MM_heap.free(sp->data);
            MM_heap.free(pAnim->frames);
        }
        break;
    }
    MM_heap.free(pAnim->decompBuffer); //ok
    MM_heap.free(pAnim->fileBuffer); // ok
    MM_heap.free(pAnim);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void FLI_Rewind(FLI_STRUCT *pAnim)
*
* DESCRIPTION : Rembobinne un Flc
*
*/
_RLXEXPORTFUNC void FLI_Rewind(FLI_STRUCT *pAnim)
{
    switch(pAnim->ReadMode){
        case FLI_DIRECTFROMDISK:
        if (pAnim->Header.Struct.type==0xAF12) pAnim->start = pAnim->start_buf;
        FIO_cur->fseek(pAnim->fli_stream, pAnim->start, SEEK_SET);
        break;
        case FLI_LZWPACKED:
        if (pAnim->Header.Struct.type==0xAF12) pAnim->start = pAnim->start_buf;
        FIO_gzip.fseek(pAnim->fli_stream, pAnim->start, SEEK_SET);
        break;
        case FLI_USEMEMORY:
        pAnim->fileBuffer = (uint8_t*)pAnim->start_buf;
        if (pAnim->Header.Struct.type==0xAF12)
        {
            pAnim->start = pAnim->Header.Struct.frame1 - 128;
        } else pAnim->start = 0;
        break;
        case FLI_EXPANDED:
        break;
    }
    pAnim->CurrentFrame = 1;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void FLI_Unpack(FLI_STRUCT *pAnim)
*
* DESCRIPTION : Decompresse une frame du flc. Resultat dans pAnim->decompBuffer
*
*/
_RLXEXPORTFUNC void FLI_SetPalette(FLI_STRUCT *pAnim)
{
	static char __temp[2048];
    int i;
    uint32_t *b = (uint32_t*)__temp;
    rgb24_t *c = pAnim->ColorTable;
    for (i=256;i!=0;b++, c++, i--)
    {
        *b = RGB_PixelFormat(c->r, c->g, c->b);
        if (!*b) *b=1;
    }
    pAnim->bitmap.handle = __temp;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  _RLXEXPORTFUNC void FLI_Unpack(FLI_STRUCT *pAnim)
*
* Description :
*
*/
_RLXEXPORTFUNC void FLI_Unpack(FLI_STRUCT *pAnim)
{
    int extended=0;
    do
    {
        FLC_Header j;
        pAnim->Flags &= ~FLX_DECOMPRESSFRAME;

        if (!(pAnim->Flags&FLX_ISPLAYING))
			return;

        switch(pAnim->ReadMode)
		{
            case FLI_DIRECTFROMDISK:
				FIO_cur->fread(j.Raw, sizeof(char), 16, pAnim->fli_stream);
            break;
            case FLI_LZWPACKED:
				FIO_gzip.fread(j.Raw, sizeof(char), 16, pAnim->fli_stream);
            break;
            case FLI_USEMEMORY:
				sysMemCpy(j.Raw, pAnim->fileBuffer, 16);
				pAnim->start      += 16;
				pAnim->fileBuffer += 16;
            break;
            case FLI_EXPANDED:
				pAnim->bitmap  = pAnim->frames[pAnim->CurrentFrame-1];
            break;
        }
        if (pAnim->ReadMode!=FLI_EXPANDED)
        {
#ifdef __BIG_ENDIAN__
            BSWAP32((uint32_t*)&j.Struct.size, 1);
            BSWAP16(&j.Struct.type, 6);
#endif
            j.Struct.size -= 16;
            if (j.Struct.size>0)
            switch(j.Struct.type){
                case 0xF1FA:
                switch(pAnim->ReadMode){
                    case FLI_LZWPACKED:
                    FIO_gzip.fread(pAnim->fileBuffer, sizeof(char), j.Struct.size, pAnim->fli_stream);
					FLI_ChunkDecode(pAnim->fileBuffer, j.Struct.Chunks, pAnim->decompBuffer, pAnim->Header.Struct.width, pAnim->Header.Struct.height, (uint8_t*)pAnim->ColorTable);
                    break;
                    case FLI_DIRECTFROMDISK:
                    if (j.Struct.size<=BUFFERSIZE)
                    FIO_cur->fread(pAnim->fileBuffer, sizeof(char), j.Struct.size, pAnim->fli_stream);
                    else
                    FIO_cur->fseek(pAnim->fli_stream, j.Struct.size, SEEK_CUR);
                    FLI_ChunkDecode(pAnim->fileBuffer, j.Struct.Chunks, pAnim->decompBuffer, pAnim->Header.Struct.width, pAnim->Header.Struct.height,(uint8_t*)pAnim->ColorTable);
                    break;
                    case FLI_USEMEMORY:
                    FLI_ChunkDecode(pAnim->fileBuffer, j.Struct.Chunks, pAnim->decompBuffer, pAnim->Header.Struct.width, pAnim->Header.Struct.height,(uint8_t*)pAnim->ColorTable);
                    pAnim->fileBuffer+=j.Struct.size;
                    pAnim->start     +=j.Struct.size;
                    break;
                }
                extended = 0;
                break;
                default:
                switch(pAnim->ReadMode){
                    case FLI_DIRECTFROMDISK:
					SYS_ASSERT(j.Struct.size<=BUFFERSIZE);
                    FIO_cur->fread(pAnim->fileBuffer, sizeof(char), j.Struct.size, pAnim->fli_stream);
                    extended = DefaultDecode(j.Struct.type, pAnim->fileBuffer, j.Struct.size);
                    break;
                    case FLI_LZWPACKED:     // Jamais normalement
                    SYS_ASSERT(j.Struct.size<=BUFFERSIZE);
                    FIO_gzip.fread(pAnim->fileBuffer, sizeof(char), j.Struct.size, pAnim->fli_stream);
                    extended = DefaultDecode(j.Struct.type, pAnim->fileBuffer, j.Struct.size);
                    break;
                    case FLI_USEMEMORY:
                    extended = DefaultDecode(j.Struct.type, pAnim->fileBuffer, j.Struct.size);
                    pAnim->fileBuffer+=j.Struct.size;
                    pAnim->start     +=j.Struct.size;
                    break;
                }
                break;
            }
        }
    } while(extended!=0);
    pAnim->Flags|=FLX_RESETPALETTE;
    pAnim->CurrentFrame++;
    if (pAnim->CurrentFrame>=pAnim->MaximumFrame)
    {
        if (!(pAnim->Flags&FLX_LOOPANIMATION))
			pAnim->Flags&=~FLX_ISPLAYING;
        else FLI_Rewind(pAnim);
    }
    if (pAnim->Flags&FLX_FORMARD1FRAME)
		pAnim->Flags&=~FLX_ISPLAYING;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static GXSPRITEGROUP *Unpack_FLI_to_SpriteGroup(SYS_FILEHANDLE in, int diet)
{
    GXSPRITEGROUP *pSpriteGroup;
    GXSPRITE *sp;
    int32_t i, k;
    FLI_STRUCT *a ;
    a = FLI_Open(in, FLI_DIRECTFROMDISK);
    if (!a)
		return NULL;
    pSpriteGroup = MM_CALLOC(1, GXSPRITEGROUP);
    pSpriteGroup->maxItem = a->MaximumFrame;
    pSpriteGroup->item = (GXSPRITE *) MM_heap.malloc(sizeof(GXSPRITE)*pSpriteGroup->maxItem);
    for (i=0, sp=pSpriteGroup->item;i<pSpriteGroup->maxItem;sp++, i++)
    {
        FLI_Unpack(a);
        sp->LY = a->Header.Struct.height;
        sp->LX = a->Header.Struct.width;
        k = sp->LX * sp->LY;
        sp->data = (uint8_t*)MM_heap.malloc(k);
        sysMemCpy(sp->data, a->decompBuffer, k);
		GX.Client->UploadSprite(sp, a->ColorTable, 1);
    }

    pSpriteGroup->speed = (uint16_t)a->Header.Struct.speed;
    FLI_Close(a);
    return pSpriteGroup;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  GXSPRITEGROUP *FLI_LoadToSpriteGroup(char *filename, char *res, int diet)
*
* DESCRIPTION :
*
*/
_RLXEXPORTFUNC GXSPRITEGROUP *FLI_LoadToSpriteGroup(const char *filename, int diet)
{
    SYS_FILEHANDLE in = FIO_cur->fopen(filename, "rb");
    GXSPRITEGROUP *f=NULL;
    if (in)
		f = Unpack_FLI_to_SpriteGroup(in, diet);
    return f;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
_RLXEXPORTFUNC void FLI_Draw(FLI_STRUCT *a, int xoff, int yoff)
{
    FLI_Unpack(a);
    GX.csp.pset(xoff, yoff, &a->bitmap);
}

