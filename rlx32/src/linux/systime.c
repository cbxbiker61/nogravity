//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2004 - realtech VR

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
Linux/SDL Port: 2005 - Matt Williams
*/
//------------------------------------------------------------------------- 
#include <stdio.h>
#include <SDL/SDL.h>
#include <pthread.h>

#include "_rlx32.h"
#include "systools.h"
#include "systime.h"

// Time in millisecond
u_int32_t timer_ms(void)
{
  // Return the value from SDL_GetTicks(), which returns time in ms.
  return (u_int32_t)SDL_GetTicks();
}

// Time in seconds
u_int32_t timer_sec(void)
{
  // Return the value from SDL_GetTicks(), converting to seconds.
  return (u_int32_t)(SDL_GetTicks() / 1000);
}

// Snooze (wait + release CPU)
void timer_snooze(u_int32_t t)
{
  // Just call through into SDL.
  SDL_Delay((Uint32)t);
}

// Wait
u_int32_t timer_delay(u_int32_t t)
{
  // Just call through into SDL.
  SDL_Delay((Uint32)t);

  return 0;
}

// 1 microsecond = 0.001 millisecond

static const int64_t g_iFreq = 1000000;

#define GET_TICK(tmp) *tmp = (((int64_t)SDL_GetTicks()) * 1000)

// stop timer
void timer_Stop(SYS_TIMER *tm)
{
    tm->flags &= ~SYS_TIMER_FLAGS_START;
    return;
}

// clear timer
void timer_Reset(SYS_TIMER *tm)
{
    tm->iCounter = 0;
    tm->fCounter = 0.f;
    return;
}

void timer_Update(SYS_TIMER *tm)
{
    int64_t ticks_to_wait = tm->iMinFrame ? (int64_t)tm->iMinFrame * g_iFreq / (int64_t)tm->iFreq : (int64_t)0; // in microseconds
    int64_t ticks_passed;
	int64_t ticks_left;
	int64_t ticks_min = g_iFreq * (int64_t)10 / (int64_t)1000;

    do
    {
        GET_TICK(&tm->tEnd);
    	ticks_passed = tm->tEnd - tm->tStart;
		ticks_left = (int64_t)ticks_to_wait - (int64_t)ticks_passed;
		if (ticks_left > ticks_min)
			usleep(1000);
		else
		{
			int i=0;
			for (;i<10;i++)
                 sched_yield();
		}

    }while(ticks_left>=0);

	tm->fFrameDelta = (float)((double)ticks_passed / (double)g_iFreq);
	tm->fCounter = tm->fFrameDelta * (float) tm->iFreq;
    tm->iCounter = (int32_t)(tm->fCounter * 65535.f);

    GET_TICK(&tm->tStart);
    return;
}

void timer_Start(SYS_TIMER *tm, int iFreq, int iMinFrame)
{
	tm->iFreq = iFreq;
	tm->iMinFrame = iMinFrame;

    GET_TICK(&tm->tStart);
    timer_Update(tm);
    return;
}

// http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html
typedef void *(*SYS_PTHREAD)(void *);

int32_t thread_begin(SYS_THREAD *pThread, enum SYS_THREAD_PRIORITY_ENUM priority)
{
  // TODO: To reduce dependencies, it might be sensible to replace pthreads with SDL threads.
    pThread->nStatus = 0;
    if (!pthread_create ((pthread_t*)&pThread->hThread, NULL, (SYS_PTHREAD)pThread->pFunc, (void*)pThread->pArgument))
    {
		pThread->nStatus = 1;   // pthread_cond_t ?
        return 0;
    }
    else
        return -1;
}

void thread_end(SYS_THREAD *pThread)
{
	pthread_join((pthread_t)pThread->hThread, NULL);
	pThread->nStatus = 0;   // pthread_cond_t ?
	return;
}

void thread_exit(int code)
{
    pthread_exit((void*)&code);   // pthread_cond_t ?
    return;
}

