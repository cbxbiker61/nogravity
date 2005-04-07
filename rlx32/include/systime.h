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
#ifndef __SYSTIME_HH
#define __SYSTIME_HH

// Purpose : Running a timer at frequency=75, while increment Counter every 70th second.
// Timer structure

#define SYS_TIMER_FLAGS_START	1
#define SYS_TIMER_FLAGS_NOSKIP  2

// timer
typedef struct _sys_timerhandle
{
	u_int32_t	flags;		   // Flags : 0x1:Accurate timer 0x2:immediate
	u_int32_t	iFreq;		   // Internal (frequency)
	float		fFrameDelta;   // Min Frame Delta
	float		fCounter;	   // <=) floating point coefficient
	int			iCounter;	   // Fixed point counter
	int			iMinFrame;	   // Number of frame to skip
	int64_t		tStart, tEnd;
}SYS_TIMER;

// Thread manager

typedef u_int32_t  (CALLING_STD * PSYSTHREADFUNC)(void*);

typedef struct _sys_thread
{
	PSYSTHREADFUNC			pFunc;
	void				*	pArgument;
	u_int32_t				nStatus;
	SYS_THREADHANDLE		hThread;
}SYS_THREAD;		

typedef struct _sys_mutex
{
	SYS_MUTEXHANDLE			hMutex;
}SYS_MUTEX;	


#if defined __WATCOMC__
void timer_rdtsc( u_int64_t *dest );
#pragma aux timer_rdtsc = \
		"rdtsc"\
		"mov [ebx], eax"\
		"mov [ebx + 4], edx"\
		parm  [ebx] modify [edx] [eax];

#elif defined _MSC_VER && (!defined _WINCE)

__inline void timer_rdtsc( u_int64_t *dest )
{
	__asm {
		rdtsc
		mov ebx, dest
		mov [ebx], eax
		mov [ebx+4], edx
	 }
}
#elif defined ( __DJGPP__ ) || (defined __BEOS__)
extern __inline__ void timer_rdtsc( u_int64_t *dest )
{
	int dummy;
	__asm__ __volatile__ (
		"pushl %%edx          \n"
		"pushl %%eax          \n"
		"rdtsc                \n"
		"mov  (%%ecx),%%eax \n"
		"mov  4(%%ecx),%%edx \n"
		"popl  %%eax          \n"
		"popl  %%edx"
		: "=a" (dummy)
		:"c"  (dest)
	);
	return;
}

#endif

enum SYS_THREAD_PRIORITY_ENUM
{
	SYS_THREAD_PRIORITY_IDLE,
	SYS_THREAD_PRIORITY_LOWEST,
	SYS_THREAD_PRIORITY_BELOW_NORMAL,
	SYS_THREAD_PRIORITY_NORMAL,
	SYS_THREAD_PRIORITY_ABOVE_NORMAL,
	SYS_THREAD_PRIORITY_HIGHEST,
	SYS_THREAD_PRIORITY_TIME_CRITICAL
};

__extern_c

// Thread
_RLXEXPORTFUNC	int32_t		thread_begin(SYS_THREAD *pFunc, enum SYS_THREAD_PRIORITY_ENUM priority);
_RLXEXPORTFUNC	void		thread_exit(int code);
_RLXEXPORTFUNC	void		thread_end(SYS_THREAD *pFunc);

// Timer
_RLXEXPORTFUNC	void		timer_Start(SYS_TIMER *tm, int frequency, int bFrameSkip);
_RLXEXPORTFUNC	void		timer_Stop(SYS_TIMER *tm);
_RLXEXPORTFUNC	void		timer_Reset(SYS_TIMER *tm);
_RLXEXPORTFUNC	void		timer_Update(SYS_TIMER *tm);

// Mutex
_RLXEXPORTFUNC  int 		mutex_init(SYS_MUTEX *mutex);
_RLXEXPORTFUNC  int 		mutex_destroy(SYS_MUTEX *mutex);
_RLXEXPORTFUNC  int 		mutex_lock(SYS_MUTEX *mutex);
_RLXEXPORTFUNC  int 		mutex_unlock(SYS_MUTEX *mutex);

_RLXEXPORTFUNC	u_int32_t	timer_sec(void);
_RLXEXPORTFUNC	u_int32_t	timer_ms(void);
_RLXEXPORTFUNC	void		timer_snooze(u_int32_t ms);

__end_extern_c

#endif
