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
#ifndef __SYSTOOLS_H
#define __SYSTOOLS_H

#ifdef __WATCOMC__
 #include <direct.h>
#else
 #include <string.h>
#endif

// File attributes for filefirst/next
#define _FILE_NORMAL       0x00    /* Normal file - read/write permitted */
#define _FILE_RDONLY       0x01    /* Read-only file */
#define _FILE_HIDDEN       0x02    /* Hidden file */
#define _FILE_SYSTEM       0x04    /* System file */
#define _FILE_VOLID        0x08    /* Volume-ID entry */
#define _FILE_SUBDIR       0x10    /* Subdirectory */
#define _FILE_ARCH         0x20    /* Archive file */
#ifdef __BEOS__
   #define _MAX_PATH 256
#endif

struct file_find_t 
{
    char reserved[21];      /* reserved for use           */
    char attrib;            /* attribute byte for file    */
    unsigned short wr_time; /* time of last write to file */
    unsigned short wr_date; /* date of last write to file */
    u_int32_t  size;    /* length of file in bytes    */
    char name[256];         /* null-terminated filename   */
};

// Memory manager
typedef struct _sys_memory
{
    void    	*(*malloc)(size_t size);
    void    	 (*free)(void *block);
    void    	*(*realloc)(void *block, size_t size);
    void    	 (*heapalloc)(void *block, size_t size);
    unsigned	 (*push)(void);
    void    	 (*pop)(int32_t id);
    void    	 (*reset)(void);
    u_int8_t     *heapAddress;
    u_int32_t     Size;
    u_int32_t     PreviousAddress;
    u_int32_t     CurrentAddress;
    u_int32_t     TotalAllocated;
    u_int32_t     Stack;
    unsigned	  active;
}SYS_MEMORYMANAGER;


__extern_c

    // Strings operations
_RLXEXPORTFUNC    void    RLXAPI   sysStrExtChg(char *nouvo, const char *old, const char *ext);
_RLXEXPORTFUNC    char    RLXAPI  *sysStrUpr(char *name);
_RLXEXPORTFUNC    char    RLXAPI  *sysStrLwr(char *name);
_RLXEXPORTFUNC    int     RLXAPI   sysStriCmp(const char *s1, const char *s2);

    // Array operations
_RLXEXPORTFUNC    void    RLXAPI   array_justifytext(const char **text, int numCharsPerLine, void (*callback)(char *text, int line));
_RLXEXPORTFUNC    int     RLXAPI   array_size(const char **tt);
_RLXEXPORTFUNC    void    RLXAPI   array_remove(void *array, int pos, int sizeitem, int sizearray);
_RLXEXPORTFUNC    void    RLXAPI   array_insert(void *array, const void *data, int pos, int sizeitem, int sizearray);
_RLXEXPORTFUNC    void    RLXAPI   array_free(char **tt);
_RLXEXPORTFUNC    char    RLXAPI  **array_loadtext(SYS_FILEHANDLE in, int maxy, int maxx);

    // Lsb Motorola/Intel conversion
_RLXEXPORTFUNC    void    RLXAPI   BSWAP16(u_int16_t *val, int n);
_RLXEXPORTFUNC    void    RLXAPI   BSWAP32(u_int32_t *val, int n);

    // File operations
_RLXEXPORTFUNC    int      RLXAPI  file_exists(const char *filename);
_RLXEXPORTFUNC    void     RLXAPI  file_describe(char *filename, const char *desc);
_RLXEXPORTFUNC    char     RLXAPI *file_searchpath(const char *name);
_RLXEXPORTFUNC    char     RLXAPI *file_searchpathES(char *fileName, const char *pathSearch);
_RLXEXPORTFUNC    char     RLXAPI *file_name(char *a);
_RLXEXPORTFUNC    void     RLXAPI  file_path(char *t);
_RLXEXPORTFUNC    char     RLXAPI *file_extension(char *t);

_RLXEXPORTDATA    extern    SYS_MEMORYMANAGER MM_std, MM_heap, MM_video, MM_audio;


__end_extern_c
 
#define sysMemSet memset
#define sysMemCpy memcpy
#define sysStrCpy strcpy
#define sysStrnCpy strncpy
#define sysStrLen strlen
#define sysMemZero(a, b) sysMemSet(a, 0, b)
#define sysRand(a)  (rand()%(a))

#ifndef min
#define min(a,b) ((a)<(b) ? a : b) 
#define max(a,b) ((a)>(b) ? a : b) 
#endif

#define randomf(i_max) ((i_max) * (float)rand() / (float)RAND_MAX)

// Macros
#define SETBITFIELD(cond, dest, value) if (cond) dest|=value; else dest&=~value;
#define XCHG(a, b, type) {type x=a;a=b;b=x;}
#define MM_CALLOC(n, type) (type*)MM_heap.malloc( (n) * sizeof(type))

#endif
