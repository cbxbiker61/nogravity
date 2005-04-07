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
// http://www.geisswerks.com/ryan/FAQS/timing.html
#include <windows.h>
#include <stdio.h>
#include "rlx_system"
#include "systime.h"
#include "_stub.h"

// Use CreateThread
#ifndef _XBOX
#define USE_THREAD
#endif

// Use Sleep function (CPU yielding)
#ifndef _XBOX
#define USE_YIELD
#endif

// Use QueryPerformanceCounter
#define USE_QPC

// Use TimeGetTime
#ifndef _XBOX
#pragma comment(lib, "winmm.lib")
#include <mmsystem.h>
#define USE_TGT
#endif

// Use TimePeriod
#ifndef _XBOX
#define USE_TIME_PERIOD
#endif

// Get time in millisecond
#ifdef USE_TGT
#define GET_MILLISEC() timeGetTime()
#else
#define GET_MILLISEC() GetTickCount()
#endif

// Fallback with no x86rdtsc.c file support
#if !defined USE_RDTSC

// Get tick
#ifdef USE_QPC
#define GET_TICK(tmp) QueryPerformanceCounter((LARGE_INTEGER*)tmp)
#else
#define GET_TICK(tmp) *tmp = GET_MILLISEC()
#endif

static u_int64_t g_iFreq = 0;

// Start timer
void timer_Start(struct _sys_timerhandle *tm, int iFreq, int iMinFrame)
{
	tm->iFreq = iFreq;
	tm->iMinFrame = iMinFrame;
	tm->flags|=SYS_TIMER_FLAGS_START | (iMinFrame ? SYS_TIMER_FLAGS_NOSKIP : 0);

#ifdef USE_TIME_PERIOD
    timeBeginPeriod(1);
#endif

#ifdef USE_QPC
   	QueryPerformanceFrequency((LARGE_INTEGER*)&g_iFreq);
#else
    g_iFreq = 1000;
#endif

    GET_TICK(&tm->tStart);
    timer_Update(tm);
    return;
}

// Reset timer
void timer_Reset(struct _sys_timerhandle *tm)
{
    tm->iCounter = 0;
    tm->fCounter = 0.f;
    return;
}

// Update timer
void timer_Update(struct _sys_timerhandle *tm)
{
	int64_t ticks_to_wait = tm->iMinFrame ? (int64_t)tm->iMinFrame * (int64_t)g_iFreq / (int64_t)tm->iFreq : 0;
	int64_t ticks_min = g_iFreq * (int64_t)2 / (int64_t)1000;

    int64_t ticks_passed;
	int64_t ticks_left;

    do
    {
        GET_TICK(&tm->tEnd);
    	ticks_passed = tm->tEnd - tm->tStart;
		ticks_left = (int64_t)ticks_to_wait - (int64_t)ticks_passed;

#ifdef USE_YIELD
		if (ticks_left<=0)
			break;

        //   if > 0.002s left, do Sleep(1), which will actually sleep some
        //   steady amount, probably 1-2 ms,
        //   and do so in a nice way (cpu meter drops; laptop battery spared).
        //   otherwise, do a few Sleep(0)'s, which just give up the timeslice,
        //   but don't really save cpu or battery, but do pass a tiny
        //   amount of time.

		if (ticks_left > ticks_min)
			Sleep(1);
		else
		{
			int i=0;
			for (;i<10;i++)
				Sleep(0);
		}
#endif

    }while(ticks_left>0);
    
	tm->fFrameDelta = (float)((double)ticks_passed / (double)g_iFreq);
	tm->fCounter = tm->fFrameDelta * (float) tm->iFreq;
    tm->iCounter = (int32_t)(tm->fCounter * 65535.f);

    GET_TICK(&tm->tStart);
    return;
}

// Stop timer
void timer_Stop(struct _sys_timerhandle *tm)
{
    tm->flags &= ~SYS_TIMER_FLAGS_START;
#ifdef USE_TIME_PERIOD
    timeEndPeriod(1);
#endif

    return;
}

#endif

// Get time in ms
u_int32_t timer_ms(void)
{
	return GET_MILLISEC();
}

// Get time in secs
u_int32_t timer_sec(void)
{
	return GET_MILLISEC() / 1000;
}

// Snooze (yield)
void timer_snooze(u_int32_t n)
{
#ifdef USE_YIELD	
	Sleep(n);
#endif
}

// Start thread
int32_t thread_begin(SYS_THREAD *pFunc, enum SYS_THREAD_PRIORITY_ENUM priority)
{
	HANDLE hThread = CreateThread(
		NULL, 					// no security attributes
		0, 						// use default stack size
		pFunc->pFunc, 			// thread function
		pFunc->pArgument, 		// argument to thread function
		0, 						// use default creation flags
		NULL
	);							// returns the thread identifier
	DWORD p = THREAD_PRIORITY_NORMAL;
	pFunc->nStatus = 1;
	switch(priority)
	{
		case 0:	p = THREAD_PRIORITY_IDLE; break;
		case 1: p = THREAD_PRIORITY_LOWEST; break;
		case 2: p = THREAD_PRIORITY_BELOW_NORMAL; break;
		case 3: p = THREAD_PRIORITY_NORMAL; break;
		case 4: p = THREAD_PRIORITY_ABOVE_NORMAL; break;
		case 5: p = THREAD_PRIORITY_HIGHEST; break;
		case 6: p = THREAD_PRIORITY_TIME_CRITICAL; break;
	}
	SetThreadPriority(hThread, p);
	pFunc->hThread = hThread;
	return 0;

}

// End thread
void thread_end(SYS_THREAD *pFunc)
{
	HANDLE handle = (HANDLE)pFunc->hThread;	
	WaitForSingleObject(handle, INFINITE);
	CloseHandle(handle);
	pFunc->hThread = 0;
	return;
}

// Exit thread
void thread_exit(int code)
{
	ExitThread(code);
	return;
}

// Win32
int mutex_init(SYS_MUTEX *mutex)
{
	/* Allocate mutex memory */
	if ( mutex )
    {
		/* Create the mutex, with initial value signaled */
		mutex->hMutex = CreateMutex(NULL, FALSE, NULL);
		if ( ! mutex->hMutex )
        {
			mutex = NULL;
		}
	}
	return mutex ? 0 : -1;
}

int mutex_destroy(SYS_MUTEX *mutex)
{
   if ( mutex ) 
   {
		if ( mutex->hMutex ) 
		{
			CloseHandle((HANDLE)mutex->hMutex);
			mutex->hMutex = 0;
		}
		return 0;
   }
   return -1;
}

int mutex_lock(SYS_MUTEX *mutex)
{
	if ( mutex == NULL )
		return -1;

	if ( WAIT_FAILED == WaitForSingleObject((HANDLE)mutex->hMutex, INFINITE) ) 
		return -1;

	return 0;
}

int mutex_unlock(SYS_MUTEX *mutex)
{
	if ( mutex == NULL )
		return -1;

	if ( FALSE == ReleaseMutex((HANDLE)mutex->hMutex)) 
		return -1;

	return 0;
}

