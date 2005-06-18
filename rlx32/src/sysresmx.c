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
#include <stdlib.h>
#include <stdio.h>
#if defined __GNU_ || __BEOS__
#include <unistd.h>
#endif
#if defined _MSC_VER 
#include <direct.h>
#endif
#ifndef _MAX_PATH
#define _MAX_PATH 256
#endif
#include "_rlx32.h"
#include "systools.h"
#include "sysresmx.h"
SYS_FILEIO  *	FIO_cur = &FIO_std;
SYS_WAD		*	FIO_wad;

extern void *lib_gzfopen (SYS_FILEHANDLE fd, const char *mode);


// Private definition
void		MakePathUUU(char *dest, const char *path, const char *lpFilename);
int			filewad_closeFP(SYS_WAD *pWad, SYS_FILEHANDLE fp);

static void AppendTrail(char *p)
{	
	if (*p)
	{
		while(p[1]!=0)
		{
			p++;
		}
		if (*p!='/' && *p!='\\')
		{
			p[1] = '\\';
			p[2] = 0;
		}
	}
}
void MakePathUUU(char *dest, const char *path, const char *lpFilename)
{
	sysStrnCpy(dest, path, _MAX_PATH);
	AppendTrail(dest);
	strcat(dest, lpFilename);
}


/*------------------------------------------------------------------------
*
* PROTOTYPE  :	u_int32_t filewad_fsize(SYS_FILEHANDLE   fp)
*
* DESCRIPTION :
*
*/
static int CALLING_C filewad_fsize(SYS_FILEHANDLE   fp)
{
	SYS_WAD *pWad = filewad_getcurrent();
	return ((pWad->mode&SYS_WAD_STATUS_ENABLED)==0) 
		? (int32_t)FIO_std.fsize(fp) 
		: (int32_t)((SYS_WADREF*)fp)->file->size;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  SYS_WAD *filewad_open(char *lpFilename, int flags)
*
* DESCRIPTION :
*
*/
SYS_WAD *filewad_open(const char *lpFilename, int flags)
{
	SYS_FILEHANDLE   fdata;
    SYS_WAD *pWad = MM_CALLOC(1, SYS_WAD);
	struct _sys_wadentry *file;
	size_t i;
	int32_t dataOffset = 0, overlayOffset = 0;
	
	// Override pWad ptr
	if (!lpFilename)
		return NULL;

	filewad_setcurrent( pWad );
	sysStrnCpy(pWad->s_FileName, lpFilename, _MAX_PATH);
	pWad->mode |= (u_int8_t) flags;
	pWad->mode &= ~SYS_WAD_STATUS_ENABLED;

	// Read hash table
    fdata = FIO_std.fopen(pWad->s_FileName, "rb");
	if (fdata==(SYS_FILEHANDLE )NULL)
		return NULL;

	pWad->mode   |=  SYS_WAD_STATUS_ENABLED;

	// Overlayed executable
	if (pWad->mode & SYS_WAD_STATUS_OVERLAY)
	{
        FIO_std.fseek( fdata, -4, SEEK_END);
        FIO_std.fread(&overlayOffset, sizeof(u_int32_t), 1, fdata);
#ifdef __BIG_ENDIAN__
		BSWAP32((u_int32_t*)&overlayOffset, 1);
#endif
        FIO_std.fseek(fdata, -overlayOffset, SEEK_END);
        overlayOffset = FIO_std.ftell(fdata);
	}

	// Table offset
    FIO_std.fread(&dataOffset, sizeof(u_int32_t), 1, fdata);
#ifdef __BIG_ENDIAN__
	BSWAP32((u_int32_t*)&dataOffset, 1);
#endif
	if (dataOffset < 0)
	{
		dataOffset = -dataOffset;
		FIO_std.fread(&pWad->fat.numEntries, sizeof(u_int32_t), 1, fdata);
#ifdef __BIG_ENDIAN__
		BSWAP32(&pWad->fat.numEntries, 1);
#endif
		SYS_ASSERT(pWad->fat.numEntries>0);
		pWad->fat.entries = MM_CALLOC(pWad->fat.numEntries, struct _sys_wadentry);

		FIO_std.fread(&pWad->fat.fatSize, sizeof(u_int32_t), 1, fdata);
#ifdef __BIG_ENDIAN__
		BSWAP32(&pWad->fat.fatSize, 1);
#endif

		FIO_std.fread(&i, sizeof(u_int32_t), 1, fdata);

        i = FIO_std.fread(pWad->fat.entries, sizeof(struct _sys_wadentry), pWad->fat.numEntries, fdata);
		SYS_ASSERT(i==pWad->fat.numEntries);

        FIO_std.fclose(fdata);
	}
	else
	{
        
		SYS_FILEHANDLE gz = (SYS_FILEHANDLE ) lib_gzfopen (fdata, "rb");
		SYS_ASSERT(gz);
	
		FIO_gzip.fread(&pWad->fat.numEntries, sizeof(u_int32_t), 1, gz);
#ifdef __BIG_ENDIAN__
		BSWAP32(&pWad->fat.numEntries, 1);
#endif
		SYS_ASSERT(pWad->fat.numEntries>0);
		pWad->fat.entries = MM_CALLOC(pWad->fat.numEntries, struct _sys_wadentry);

		FIO_gzip.fread(&pWad->fat.fatSize, sizeof(u_int32_t), 1, gz);
#ifdef __BIG_ENDIAN__
		BSWAP32(&pWad->fat.fatSize, 1);
#endif

		FIO_gzip.fread(&i, sizeof(u_int32_t), 1, gz);

        i = FIO_gzip.fread(pWad->fat.entries, sizeof(struct _sys_wadentry), pWad->fat.numEntries, gz);
		SYS_ASSERT(i==pWad->fat.numEntries);

        FIO_gzip.fclose(gz);

    }
  

	// Reallocate offset
	for (file = pWad->fat.entries, i = 0;i < pWad->fat.numEntries; i++, file++)
	{
#ifdef __BIG_ENDIAN__
		BSWAP32(&file->size, 1);
#endif
		SYS_ASSERT(file->size>0);
		file->offset = i!=0 ? (int32_t)(file[-1].offset + file[-1].size) : (int32_t)(dataOffset + overlayOffset);
	}
	return pWad;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	SYS_FILEHANDLE   static *filewad_fopen(const char *lpFilename, const char *mode)
*
* DESCRIPTION :
*
*/

static int CALLING_C filewad_fexist(const char *szFilename)
{
	SYS_WAD *pWad;
	pWad = filewad_getcurrent();
	if (*szFilename == 0)
		return 0;

	if (!pWad)
		return FIO_std.exists(szFilename);
	else
	if (pWad->mode & SYS_WAD_STATUS_DISABLED)
	{
		int ret;
		char name[_MAX_PATH];
		sprintf(name, "%s%s", pWad->s_Path, szFilename);
		ret = FIO_std.exists(name);
		return ret;
	}
	else
	{
		char name[_MAX_PATH];
		u_int32_t i;
		struct _sys_wadentry *file = pWad->fat.entries;
		sprintf(name, "%s%s", pWad->s_Path, szFilename);
		for (i=0;i<pWad->fat.numEntries;file++, i++)
		{
			if (sysStriCmp(name, file->name)==0)
			{
				return i + 1;
			}
		}
		return 0;
	}
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	SYS_FILEHANDLE   static *filewad_fopen(const char *lpFilename, const char *mode)
*
* DESCRIPTION :
*
*/
static SYS_FILEHANDLE  CALLING_C filewad_fopen(const char *lpFilename, const char *mode)
{
	char name[_MAX_PATH];
    SYS_WAD *pWad = filewad_getcurrent();	
	
	if (!pWad)
	{
		SYS_FILEHANDLE  fp = FIO_std.fopen(lpFilename, mode);
#ifdef _DEBUG
		if (!fp)
			SYS_Debug("%s not found.\n", lpFilename);
#endif
		return fp;
	}

	MakePathUUU(name, pWad->s_Path, lpFilename);

	if (SYS_WAD_STATUS_DISABLED == pWad->mode)	// Using normal file system
	{
		SYS_FILEHANDLE  handle = FIO_std.fopen(name, mode);
#ifdef _DEBUG
		if (!handle)
			SYS_Debug("%s not found.\n", name);
#endif
		return handle;
	}
	else
	{
		int index = filewad_fexist(lpFilename);
#ifdef _DEBUG
		if (!index)
			SYS_Debug("%s: %s not found.\n", pWad->s_FileName, name);
#endif
		if (index > 0)
		{
			int i;
			for (i=0;i<MAX_WAD_OPEN_FILE;i++)
			{
				SYS_WADREF *ref = &pWad->hFileHandles[i];
				if (!ref->handle)
				{
					
					ref->file = pWad->fat.entries + index - 1;
					ref->handle = FIO_std.fopen(pWad->s_FileName, "rb");

#ifdef _DEBUG
					SYS_Debug("%s: %s (%s) (%d)\n", pWad->s_FileName, lpFilename, mode, i);
#endif		
   				FIO_std.fseek( ref->handle, ref->file->offset, SEEK_SET);
					return (SYS_FILEHANDLE )ref;
				}
			}
			return (SYS_FILEHANDLE )NULL;
		}
		else
		{
#ifdef _DEBUG
			SYS_Debug("%s: %s not found.\n", pWad->s_FileName, name);
#endif
			return (SYS_FILEHANDLE )NULL;
		}
	}
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	static int filewad_fclose(SYS_FILEHANDLE   fp)
*
* DESCRIPTION :
*
*/
static int CALLING_C filewad_closeFlush(void)
{
    SYS_WAD *pWad = filewad_getcurrent();
	int i;
	for (i=0;i<MAX_WAD_OPEN_FILE;i++)
	{
		SYS_FILEHANDLE   fp = pWad->hFileHandles[i].handle;
		if (fp)
            FIO_std.fclose(fp);
	}
	return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void filewad_close(SYS_WAD *Res)
*
* Description :
*
*/
void filewad_close(SYS_WAD *pWad)
{
	filewad_closeFlush();
	MM_heap.free( pWad->fat.entries );
	MM_heap.free( pWad );
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	int filewad_closeall(void)
*
* Description :
*
*/
int filewad_closeall(SYS_WAD *pWad)
{
	if ((!pWad)||((pWad->mode&SYS_WAD_STATUS_ENABLED)==0))
	{
		return 0;
	}
	else
	{
		return filewad_closeFlush();
	}
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	static int filewad_closeFP(SYS_FILEHANDLE   fp)
*
* Description : Close file handle
*
*/

int filewad_closeFP(SYS_WAD *pWad, SYS_FILEHANDLE  fp)
{	
	int i;
	for (i=0;i<MAX_WAD_OPEN_FILE;i++)
	{
		SYS_WADREF *ref = &pWad->hFileHandles[i];
		if (ref->handle == fp)
		{
			FIO_std.fclose(ref->handle);
			ref->handle = 0;
			ref->file = 0;
		}
	}
	return 0;
}

static int CALLING_C filewad_fclose(SYS_FILEHANDLE  fp)
{
    SYS_WAD *pWad = filewad_getcurrent();
	if (!fp)
		return 0;
	if ((!pWad)||((pWad->mode&SYS_WAD_STATUS_ENABLED)==0))
	{
        return FIO_std.fclose(fp);
	}
	else
	{
		SYS_WADREF *ref = (SYS_WADREF*)fp;
		FIO_std.fclose(ref->handle);
		ref->handle = 0;
		ref->file = 0;
	}
	return 1;
}

static int CALLING_C filewad_fseek(SYS_FILEHANDLE  stream, long offset, int whence)
{
    SYS_WAD *pWad = filewad_getcurrent();
	if (!pWad)
		pWad = filewad_getcurrent();

	if ((!pWad)||((pWad->mode & SYS_WAD_STATUS_ENABLED)==0))
	{
	   FIO_std.fseek(stream, offset, whence);
	}
	else
	{
		SYS_WADREF *ref = (SYS_WADREF*)stream;
		switch(whence)
		{
			case SEEK_SET:
				FIO_std.fseek(ref->handle, ref->file->offset+offset, SEEK_SET);
			break;
			case SEEK_CUR:
				FIO_std.fseek(ref->handle, offset, SEEK_CUR);
			break;
			case SEEK_END:
				FIO_std.fseek(ref->handle, ref->file->offset + ref->file->size-offset, SEEK_SET);
			break;
			default:
			SYS_ASSERT(0);
			break;
		}
		
	}
	return 1;
}


static long CALLING_C filewad_ftell(SYS_FILEHANDLE  stream)
{
    SYS_WAD *pWad = filewad_getcurrent();
	if (!pWad)
		pWad = filewad_getcurrent();

	if ((!pWad)||((pWad->mode & SYS_WAD_STATUS_ENABLED)==0))
	{
	    return FIO_std.ftell(stream);
	}
	else
	{
		SYS_WADREF *ref = (SYS_WADREF*)stream;
		return FIO_std.ftell(ref->handle) - ref->file->offset;
	}
}

static int CALLING_C filewad_eof(SYS_FILEHANDLE  stream)
{
    SYS_WAD *pWad = filewad_getcurrent();
	if (!pWad)
		pWad = filewad_getcurrent();

	if ((!pWad)||((pWad->mode & SYS_WAD_STATUS_ENABLED)==0))
	{
	    return FIO_std.eof(stream);
	}
	else
	{
		SYS_WADREF *ref = (SYS_WADREF*)stream;
		return FIO_std.ftell(ref->handle) - ref->file->offset >= ref->file->size;
	}
}


static int CALLING_C filewad_fgetc(SYS_FILEHANDLE  stream)
{
    SYS_WAD *pWad = filewad_getcurrent();
	if (!pWad)
		pWad = filewad_getcurrent();

	if ((!pWad)||((pWad->mode & SYS_WAD_STATUS_ENABLED)==0))
	{
	    return FIO_std.fgetc(stream);
	}
	else
	{
		SYS_WADREF *file = (SYS_WADREF*)stream;
		return FIO_std.fgetc(file->handle);
	}
}


static size_t CALLING_C filewad_fread(void *ptr, size_t size, size_t nitems, SYS_FILEHANDLE  stream)
{
    SYS_WAD *pWad = filewad_getcurrent();
	if (!pWad)
		pWad = filewad_getcurrent();

	if ((!pWad)||((pWad->mode & SYS_WAD_STATUS_ENABLED)==0))
	{
	   return FIO_std.fread(ptr, size, nitems, stream);
	}
	else
	{
		SYS_WADREF *ref = (SYS_WADREF*)stream;
		if (ref)
		{		
			long f = FIO_std.ftell(ref->handle) - ref->file->offset;
			size_t bytes = nitems * size;
			if (f + bytes > ref->file->size)
			{
				bytes = ref->file->size - f;
				if (bytes<=0)
				return 0;
				nitems = bytes / size;
			}
			return FIO_std.fread(ptr, size, nitems, ref->handle);
		}
		SYS_ASSERT(ref);
		return 0;
	}	
}

static char * CALLING_C filewad_fgets(char *s, int n, SYS_FILEHANDLE  stream)
{
	SYS_WAD *pWad = filewad_getcurrent();
	if (!pWad)
		pWad = filewad_getcurrent();

	if ((!pWad)||((pWad->mode & SYS_WAD_STATUS_ENABLED)==0))
	{
	   return FIO_std.fgets(s, n, stream);
	}
	else
	{
		SYS_WADREF *ref = (SYS_WADREF*)stream;
		SYS_ASSERT(ref);
		return FIO_std.fgets(s, n, ref->handle);
	}
}

SYS_FILEHANDLE  filewad_get(SYS_FILEHANDLE  fp)
{
	SYS_WAD *pWad = filewad_getcurrent();
	if (!pWad)
		pWad = filewad_getcurrent();

	if ((!pWad)||((pWad->mode & SYS_WAD_STATUS_ENABLED)==0))
	{
	   return fp;
	}
	else
	{
		SYS_WADREF *ref = (SYS_WADREF*)fp;
		SYS_ASSERT(ref);
		return ref->handle;
	}
}


/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/

void filewad_makepath(SYS_WAD *pWad, char *dest, const char *fname, int nLen)
{
	if (!pWad)
		pWad = filewad_getcurrent();

	if (pWad)
		MakePathUUU(dest, pWad->s_Path, fname);

	UNUSED(nLen);
	return;
}

void filewad_getcwd(const SYS_WAD *pWad, char *curpath, int len)
{
	if (!pWad)
		pWad = filewad_getcurrent();

	if (pWad)
		sysStrnCpy(curpath, pWad->s_Path, len);
#if !defined __MACOS__ && !defined _WINCE && !defined SN_TARGET_PS2 && !defined _XBOX
	else
		getcwd(curpath, len);
#endif
	return;
}

void filewad_chdir(SYS_WAD *pWad, const char *szNewPath)
{
	if (!pWad)
		pWad = filewad_getcurrent();

	if (pWad)
	{
		char *d = pWad->s_Path;
		const char *s = szNewPath;

		if (s)
		{
			while (*s!=0)
			{
				*d = *s == '/' ? '\\' : *s;
				s++;
				d++;
			}
			if (s>szNewPath)
			{
				if (*d == '\\')
				{
					*d = 0;
				}
			}
		}
		*d = 0;
	}
#if !defined __MACOS__ && !defined _WINCE && !defined SN_TARGET_PS2
	else
	{
		if (szNewPath)
			chdir(szNewPath);
	}
#endif
	return;
}


SYS_FILEIO FIO_res =
{
	filewad_fopen,
	filewad_fclose,
	filewad_fseek,
	filewad_fread,
	filewad_fgetc,
	NULL, // fwrite
	filewad_ftell,
	filewad_eof,
	filewad_fgets,
	filewad_fsize,
	filewad_fexist,
	NULL, // fputc
	NULL // fflush
};

void sysInitFS()
{
  
}
