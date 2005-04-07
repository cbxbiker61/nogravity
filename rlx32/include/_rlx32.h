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

#ifndef __RLX32_H
#define __RLX32_H

#include <assert.h>
/* Compiler setup ***********************************************************/

#include "config.h"
#include <sys/types.h>

/* C/C++ Interface **********************************************************/

#ifndef __extern_c

  #ifdef __cplusplus
     #define __extern_c            extern "C" {
     #define __end_extern_c        }
  #else
     #define __extern_c
     #define __end_extern_c
  #endif
#endif

  #define UNUSED(var)  (void)var

/* Dll export  **************************************************************/

  #define _RLXEXPORTDATA
  #define _RLXEXPORTFUNC

/* Functions register conventions *******************************************/

#if defined _MSC_VER
    // MS Visual C
    #define CALLING_C
	#define CALLING_STD __stdcall
    #define RLXAPI

#else
    // Others compiler
    #define CALLING_C
	#define CALLING_STD
    #define RLXAPI
#endif

/* Filename and stream conventions ******************************************/

#ifndef DUMMYUNIONNAMEN
  #if defined(__cplusplus) || !defined(NONAMELESSUNION)
	#define DUMMYUNIONNAMEN(n)
  #else
	#define DUMMYUNIONNAMEN(n)      u##n
  #endif
#endif

/* Low level structures *****************************************************/

#ifndef _WIN32
   typedef unsigned BOOLEAN;
#endif

#ifndef FALSE
  enum {
         FALSE, 
         TRUE
   };
#endif


/* Data Types */
#if !defined __STDC_VERSION__ || __STDC_VERSION__ < 199901L
    #if defined __LCC__ || defined LINUX || (defined __BEOS__ && B_BEOS_VERSION >=0x0520)
       #include <stdint.h>
       #include <stddef.h>       
    #elif defined __APPLE__ && defined __MACH__
		#include <sys/types.h>
	#else
	   /* 8 bit type. */
	   typedef signed char			int8_t;
       /* unsigned 8 bit type. */
       typedef unsigned char		u_int8_t;
       /* 16bit type (2 bytes). */
       typedef short int			int16_t;
       /* unsigned 16 bit type. */
       typedef unsigned short int	u_int16_t;
	   /* 32bit type (4 bytes). */
	   typedef int					int32_t;
       /* unsigned 32 bit type. */
       typedef unsigned int			u_int32_t;
    #if defined __GNUC__
       typedef long long int		int64_t;
       typedef unsigned long long int u_int64_t;
    #else
	   /* 64 bit type (8 bytes). */
	   typedef __int64				int64_t;
       /* unsigned 64 bit type. */
       typedef unsigned __int64		u_int64_t;
    #endif // __GNUC__
   #endif // LINUX
#endif // STDC_VERSION


#define float64_t double
#define float32_t float

typedef struct _gx_sprite
{
    u_int32_t    LX, LY;
    u_int8_t   *data;
    void    *handle;
} GXSPRITE;

typedef struct _gx_rgb24{
    u_int8_t    r, g, b;
} rgb24_t;

typedef struct _gx_rgb32{
    u_int8_t    r, g, b, a;
} rgb32_t;

typedef struct _gx_sprite_sw{
	int		bpp;
	u_int32_t	palette[256];
	void *  reserved;	
} GXSPRITESW;


/* Thread, and I/O handle *******************************************************/

#include <stdio.h>

typedef FILE * SYS_FILEHANDLE ; /* I/O file handle. */
#ifdef __BEOS__
#include <kernel/OS.h>
typedef thread_id SYS_THREADHANDLE;
typedef void* SYS_MUTEXHANDLE; /* Mutex handle. */
#elif defined __MACH__ || defined __LINUX__ 
#include <pthread.h>
typedef pthread_mutex_t SYS_MUTEXHANDLE; /* Mutex handle. */
typedef pthread_t SYS_THREADHANDLE; /* Thread handle. */
#else
typedef void* SYS_MUTEXHANDLE; /* Mutex handle. */
typedef void* SYS_THREADHANDLE; /* Thread handle. */
#endif

/* Error/Debug/Trace functions *****************************************************/

#if (!defined _WINCE)
#include <assert.h>
#if defined _DEBUG || defined DEBUG
#define SYS_ASSERT(_condition) assert(_condition)
#else
#define SYS_ASSERT(_condition)
#endif
#else

#ifdef DEBUG
#define SYS_ASSERT(_condition) if (!(_condition)) { SYS_Msg("#Assertion failed!\n\nProgram: %s\nLine: %d\n\nExpression: %s\n", __FILE__,  __LINE__, #_condition);}
#else
#define SYS_ASSERT(_condition)
#endif
#endif

#define SYS_BREAK()
#define SYS_FAILED(condition) ((condition)!=0)

__extern_c
_RLXEXPORTFUNC void     RLXAPI   SYS_Msg(char *fmt, ...);
_RLXEXPORTFUNC void     RLXAPI   SYS_Debug(char *fmt, ...);
_RLXEXPORTFUNC void     RLXAPI   SYS_Error(char *fmt, ...);
_RLXEXPORTFUNC int      RLXAPI   RLX_ErrorGetCodeString(int error_code);
__end_extern_c

#endif
