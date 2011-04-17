#pragma once
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
#ifndef __SYSRESMX_H
#define __SYSRESMX_H

#define MAX_WAD_OPEN_FILE	 8
#define MAX_WAD_FILE_NAME	32

#ifndef _MAX_PATH

#if defined __MACOS__
#define _MAX_PATH 1024
#elif (defined _WIN32 || defined _WIN64)
#define _MAX_PATH 260
#else
#define _MAX_PATH 256
#endif

#endif

enum SYS_WAD_STATUS
{
	SYS_WAD_STATUS_DISABLED		= 0x0,	// Resource stream is disabled
	SYS_WAD_STATUS_ENABLED		= 0x1,	// Resource stream is enabled
	SYS_WAD_STATUS_OVERLAY		= 0x2,	// Resource is at the trail of the executable
	SYS_WAD_STATUS_PROFILE		= 0x4	// Enable profiler
};

// Stream class (remapped from the standard STDIO.H functions).
typedef struct _sys_fileio
{
	SYS_FILEHANDLE	(CALLING_C *fopen )(const char *filename, const char *mode);
	int			(CALLING_C *fclose)(SYS_FILEHANDLE  stream);
	int			(CALLING_C *fseek )(SYS_FILEHANDLE  stream, long offset, int whence);
	size_t			(CALLING_C *fread )(void *ptr, size_t size, size_t n, SYS_FILEHANDLE  stream);
	int			(CALLING_C *fgetc )(SYS_FILEHANDLE  stream);
	size_t			(CALLING_C *fwrite)(const void *ptr, size_t size, size_t n, SYS_FILEHANDLE  stream);
	long				(CALLING_C *ftell )(SYS_FILEHANDLE  stream);
	int			(CALLING_C *eof )(SYS_FILEHANDLE  stream);
	char *			(CALLING_C *fgets )(char *s, int n, SYS_FILEHANDLE  stream);
	int				(CALLING_C *fsize )(SYS_FILEHANDLE  stream);
	int			(CALLING_C *exists)(const char *filename);
	void			(CALLING_C *init)(void);
	void			(CALLING_C *close)(void);
	int			(CALLING_C *fputc)(int c, SYS_FILEHANDLE  stream);
	int			(CALLING_C *fflush)(SYS_FILEHANDLE  stream);
}SYS_FILEIO;

// Single file structure in a resource
typedef struct _sys_wadentry
{
	char					name[MAX_WAD_FILE_NAME];// file name
	uint32_t				size;					// file size
	uint32_t				offset;				// offset
}SYS_WADENTRY;

// Resource fat
typedef struct _sys_waddir
{
	uint32_t				numEntries;			// fat entry
	uint32_t				fatSize;				// fat size
	struct _sys_wadentry*	entries;				// number of entries
}SYS_WADDIR;

typedef struct _sys_wad_ref
{
	struct _sys_wadentry*	file;
	SYS_FILEHANDLE			handle;
}SYS_WADREF;

// Resource structures
typedef struct _sys_wad
{
	SYS_WADDIR				fat;					//  FAT
	char					s_Path[_MAX_PATH];		//  Path
	char					s_FileName[_MAX_PATH];	//  Data filename
	SYS_WADREF				hFileHandles[MAX_WAD_OPEN_FILE];	//	File opened
	int32_t				mode;					//	current mode (SYS_WAD_STATUS_ENABLED, off)
}SYS_WAD;

__extern_c

_RLXEXPORTFUNC		int32_t				RLXAPI	file_length(const char *filename);
_RLXEXPORTFUNC		char			*	RLXAPI	file_searchpath(const char *name);

_RLXEXPORTFUNC		SYS_WAD			*	RLXAPI	filewad_open(const char *filename, int flags);
_RLXEXPORTFUNC		void				RLXAPI	filewad_close(SYS_WAD *resource);
_RLXEXPORTFUNC		void				RLXAPI	filewad_chdir(SYS_WAD *resource, const char *newpath);
_RLXEXPORTFUNC		void				RLXAPI	filewad_getcwd(const SYS_WAD *resource, char *curpath, int len);
_RLXEXPORTFUNC		SYS_FILEHANDLE		RLXAPI	filewad_get(SYS_FILEHANDLE  fp);
_RLXEXPORTFUNC		int				RLXAPI	filewad_closeall(SYS_WAD *resource);
_RLXEXPORTFUNC		void				RLXAPI	filewad_makepath(SYS_WAD *resource, char *_s, const char *s, int nLen);

_RLXEXPORTFUNC		void				RLXAPI	sysInitZlib();
_RLXEXPORTFUNC		void				RLXAPI	sysInitFS();

extern				SYS_FILEIO			FIO_std,
										FIO_res,
										FIO_gzip;

extern				SYS_FILEIO		*	FIO_cur;
extern				SYS_WAD			*	FIO_wad;

__end_extern_c

#define filewad_setcurrent(a) FIO_wad = a
#define filewad_getcurrent() FIO_wad

#endif

