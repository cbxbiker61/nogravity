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
*/
//-------------------------------------------------------------------------
#include <OS.h>
#include <stdio.h>
#include "_rlx32.h"
#include "systools.h"
#include "systime.h"

#define USE_YIELD

uint32_t timer_ms(void)
{
    return (uint32_t)(system_time() / (int64)1000);
}

uint32_t timer_sec(void)
{
	return (uint32_t)(system_time() / (int64)1000000);
}

void timer_snooze(uint32_t t)
{
    snooze((int64)t * (int64)1000);
}
// 1 microsecond = 0.001 millisecond

#define MICROSECOND 1000000
#define GET_TICK(tmp) *tmp = (uint64_t) system_time()

static uint64_t g_iFreq = 0;

// stop timer
void timer_Stop(struct _sys_timerhandle *tm)
{
    tm->flags &= ~SYS_TIMER_FLAGS_START;
}

// clear timer
void timer_Reset(struct _sys_timerhandle *tm)
{
    tm->iCounter = 0;
    tm->fCounter = 0.f;
}

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
			snooze(1 * 1000);
		else
		{
			int i=0;
			for (;i<10;i++)
				snooze(0);
		}
#endif

    }while(ticks_left>0);

	tm->fFrameDelta = (float)((double)ticks_passed / (double)g_iFreq);
	tm->fCounter = tm->fFrameDelta * (float) tm->iFreq;
    tm->iCounter = (int32_t)(tm->fCounter * 65535.f);

    GET_TICK(&tm->tStart);
}

void timer_Start(struct _sys_timerhandle *tm, int iFreq, int iMinFrame)
{
	tm->iFreq = iFreq;
	g_iFreq = MICROSECOND;
	tm->iMinFrame = iMinFrame;

    GET_TICK(&tm->tStart);
    timer_Update(tm);
}

// Thread

int32_t thread_begin(SYS_THREAD *pThread, enum SYS_THREAD_PRIORITY_ENUM priority)
{
    const int priorities[] = {
      B_LOW_PRIORITY,
      B_NORMAL_PRIORITY,
      B_NORMAL_PRIORITY,
      B_URGENT_PRIORITY,
      B_REAL_TIME_PRIORITY,
      B_REAL_TIME_PRIORITY,
      B_REAL_TIME_PRIORITY,
    };

    pThread->hThread = spawn_thread((thread_func)pThread->pFunc, "SYS_THREAD", priorities[priority], pThread->pArgument);
    pThread->nStatus = 0;

    resume_thread(pThread->hThread);

    return 0;
}

void thread_end(SYS_THREAD *pThread)
{
    status_t status;
    wait_for_thread(pThread->hThread, &status);
}

void thread_exit(int return_value)
{
    exit_thread(return_value);
}

// BeOS : TODO
int mutex_init(SYS_MUTEX *mutex)
{
	return mutex ? 0 : -1;
}

int mutex_destroy(SYS_MUTEX *mutex)
{
   return -1;
}

int mutex_lock(SYS_MUTEX *mutex)
{
	return 0;
}

int mutex_unlock(SYS_MUTEX *mutex)
{
	return 0;
}

