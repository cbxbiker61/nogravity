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
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "systools.h"
#include "sysresmx.h"
#include "sysnetw.h"
#include "sysctrl.h"
#include "_stub.h"

KEY_ClientDriver *sKEY;
MSE_ClientDriver *sMOU;
JOY_ClientDriver *sJOY;
NET_ClientDriver *sNET;

void BSWAP16(uint16_t *pValue, int n)
{
   while(n)
	{
		uint16_t x = *pValue;
		*pValue = (x<<8) | (x>>8);
		pValue++;
		n--;
	}
}

void BSWAP32(uint32_t *pValue, int n)
{
   while (n)
	{
		uint32_t x = *pValue;
		*pValue = (x<<24) | ((x<<8) & 0x00ff0000) | ((x>>8) & 0x0000ff00) | (x>>24);
		pValue++;
		n--;
	}
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void sysStrExtChg(char *nouvo, char *old, char *ext)
*
* DESCRIPTION :
*
*/
void sysStrExtChg(char *nouvo, const char *old, const char *ext)
{
    char *p=NULL;
    while(*old != 0)
    {
        if (*old=='.') p=nouvo;
        *nouvo++=*old++;
    }
    if (p) nouvo=p;
    *nouvo++='.';
    while(*ext) *nouvo++=*ext++;
    *nouvo=0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int sysStriCmp(const char *s1, const char *s2)
*
* DESCRIPTION :
*
*/
int sysStriCmp(const char *s1, const char *s2)
{
    while (!((toupper(*s1)!=toupper(*s2))||(*s1==0)||(*s2==0)))
    {
        s1++;
        s2++;
    }
    return (*s1)-(*s2);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  char *sysStrUpr(char *s1)
*
* DESCRIPTION :
*
*/
char *sysStrUpr(char *s1)
{
    char *s2=s1;
    while (*s1!=0) {*s1=(char)toupper(*s1);s1++;}
    return s2;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  char *sysStrLwr(char *s1)
*
* DESCRIPTION :
*
*/
char *sysStrLwr(char *s1)
{
    char *s2=s1;
    while (*s1!=0) {*s1=(char)tolower(*s1);s1++;}
    return s2;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : int tab_size(char **tt)
*
* DESCRIPTION :
*
*/
int array_size(const char **tt)
{
    int i=0;
    while (*tt!=NULL)
    {
        tt++;
        i++;
    }
    return i;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void array_remove(void *array, int pos, int sizeitem, int sizearray)
*
* DESCRIPTION :
*
*/
void array_remove(void *array, int pos, int sizeitem, int sizearray)
{
    uint8_t *a=(uint8_t*)array, *b=a+sizeitem*pos;
    memmove(b, b+sizeitem, (sizearray-pos-1)*sizeitem);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void array_insert(void *array, void *data, int pos, int sizeitem, int sizearray)
*
* DESCRIPTION :
*
*/
void array_insert(void *array, const void *data, int pos, int sizeitem, int sizearray)
{
    uint8_t *a=(uint8_t*)array, *b=a+sizeitem*pos;
    memmove(b+sizeitem, b, (sizearray-pos)*sizeitem);
    sysMemCpy(b, data, sizeitem);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void array_free(char **tt)
*
* DESCRIPTION :
*
*/
void array_free(char **tt)
{
    char **t0=tt;
    while (*tt!=NULL)
    {
        MM_heap.free(*tt);
        tt++;
    }
    MM_heap.free(t0);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  char **array_loadtext(SYS_FILEHANDLE in, int maxy)
*
* DESCRIPTION :
*
*/
char **array_loadtext(SYS_FILEHANDLE in, int maxy, int maxx)
{
	char tex[256]; // ?????
    char **t = MM_CALLOC(maxy+1, char*);
	int i = 0;
    while ((!FIO_cur->eof(in))&&(maxy>0))
    {
        unsigned len;
		*tex = 0;
        if (FIO_cur->fgets(tex, maxx>0 ? maxx : 255, in))
        {
            len = strlen(tex);
            if (len>0)
            {
                char *v = tex + len - 1;
                while ((0x0<v[0])&&(v[0]<0x20))
					*v--=0;
            }
            if (tex[0]=='@')
            {
                t[i] = NULL;
                break;
            }
            else
            {
                unsigned l = strlen(tex);
                t[i] = MM_CALLOC(maxx >0 ? maxx : l + 1, char);
                sysStrnCpy(t[i], tex, l);
            }
            maxy--;
			i++;
        }
    }
    return t;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :char *file_name(char *a)
*
* DESCRIPTION :
*
*/
char *file_name(char *a)
{
    int l=strlen(a);
    while (!((a[l]=='/') || (a[l]=='\\') || (l==0))) l--;
    return((l>0) ? a+l+1 : a);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int file_exists(char *f)
*
* DESCRIPTION :
*
*/
int file_exists(const char *f)
{
    SYS_FILEHANDLE in = FIO_std.fopen(f, "rb");
    if (in)
    {
        FIO_std.fclose(in);
        return 1;
    }
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  char *file_extension(char *t)
*
* DESCRIPTION :
*
*/
char *file_extension(char *t)
{
    while ((*t!='.') && (*t!=0)) t++;
    return t;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void file_path(char *t)
*
* DESCRIPTION :
*
*/
void file_path(char *t)
{
    char *f=file_name(t);
    if (strlen(t)>0) f--;
    *f=0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
SYS_MEMORYMANAGER MM_std={
    malloc,
    free,
realloc };

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
uint8_t static *MM_heap_reserve(size_t size)
{
    uint8_t *v = MM_heap.heapAddress     + MM_heap.CurrentAddress;
    MM_heap.PreviousAddress = MM_heap.CurrentAddress;
    {
        int32_t off = ((uint32_t)v+size)&31;
        if (off) size+=32-off;           // Alignement sur 32 octets
    }
    MM_heap.CurrentAddress+=size;
	SYS_ASSERT(MM_heap.CurrentAddress<MM_heap.Size) ;
    return v;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void *MM_heap_malloc(size_t size)
*
* DESCRIPTION :
*
*/
static void *MM_heap_malloc(size_t size)
{
    uint8_t *v=NULL;
	SYS_ASSERT(size);
    if (MM_heap.active)
    {
        v = MM_heap_reserve(size);
    }
    else
    {
        v = (uint8_t*) MM_std.malloc(size);
		SYS_ASSERT(v);
        if (!v)
            size = 0;
    }
    if (v)
    {
        MM_heap.TotalAllocated+=size;
        sysMemZero(v, size);
    }
    return v;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void MM_heap_free(void *block)
*
* DESCRIPTION :
*
*/
static void MM_heap_free(void *block)
{
    if (MM_heap.active)
    {
        if (block==MM_heap.heapAddress+MM_heap.PreviousAddress)
        {
            MM_heap.CurrentAddress=MM_heap.PreviousAddress;
        }
        return;
    }
    if (block!=NULL)
    {
        MM_std.free(block);
        block = NULL;
    }
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void *MM_heap_realloc(void *block, size_t size)
*
* DESCRIPTION :
*
*/
static void *MM_heap_realloc(void *block, size_t size)
{
    uint8_t *v;
    if (MM_heap.active)
    {
        v = MM_heap_reserve(size);
        sysMemCpy(v, block, size);
    } else v=(uint8_t*)MM_std.realloc(block, size);
    return v;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void  MM_heapalloc(void *block, size_t size)
*
* DESCRIPTION :
*
*/
static void  MM_heapalloc(void *block, size_t size)
{
    MM_heap.heapAddress = (uint8_t*)block;
    MM_heap.Size = size;
	SYS_ASSERT(block);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static unsigned MM_heap_push(void)
*
* DESCRIPTION :
*
*/
static unsigned MM_heap_push(void)
{
    MM_heap.Stack = MM_heap.CurrentAddress;
    return MM_heap.Stack;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void MM_heap_pop(int32_t id)
*
* DESCRIPTION :
*
*/
static void MM_heap_pop(int32_t id)
{
    MM_heap.CurrentAddress = (id<0) ? MM_heap.Stack : id;
    MM_heap.PreviousAddress = 0;
}

void array_justifytext(const char **text, int numCharsPerLine, void (*callback)(char *text, int line))
{
    char __t[1024];
    int ln = 0;
    while (*text!=NULL)
    {
        char    *t1 =  __t;
        const char    *t2 = *text;
        unsigned l;
        while (*t2!=0)
        {
            l = strlen(t2);
            if (l<(unsigned)numCharsPerLine)
            {
                sysStrCpy(t1, t2);
                callback(t1, ln);
                ln++;
            }
            else
            {
                l = numCharsPerLine;
                while (isspace(t2[l])==0)
                {
                    l--;
                }
                sysStrnCpy(t1, t2, l); t1[l]=0;
                callback(t1, ln);
                ln++;
            }
            t2+=l;
        }
        text++;
    }
}

static void MM_heap_reset(void)
{
    MM_heap.active = (MM_heap.heapAddress!=NULL);
    MM_heap.CurrentAddress = 0;
    return;
}
SYS_MEMORYMANAGER MM_heap={
    MM_heap_malloc,
    MM_heap_free,
    MM_heap_realloc,
    MM_heapalloc,
    MM_heap_push,
    MM_heap_pop,
    MM_heap_reset,
0};

char *file_searchpathES(char *fileName, const char *pathSearch)
{
    static char ps[128];
	if ( FIO_cur->exists(fileName) ) return fileName;
    {
        const char *s0 = pathSearch;
		char *s1;
        if (s0[0]=='\0') return NULL;
        filewad_chdir(FIO_wad, "");
        while (*s0!='\0')
        {
            s1 = ps; *s1 = '\0';
            while ( (*s0!=';') && (*s0!='\0') )
            {
                *s1 = *s0;
                s0++;
                s1++;
            }
            *s1 = 0;
            if (*ps)
            {
                strcat(ps, "\\");
                strcat(ps, fileName);
                if (FIO_cur->exists(ps))
                {
                    return ps;
                }
            }
            if (*s0 == ';') s0++;
        }
    }
    return NULL;
}

