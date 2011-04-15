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
#ifndef __amigaos4__
#include <sched.h>
#endif

#include "_rlx32.h"
#include "systools.h"
#include "systime.h"

// Time in millisecond
uint32_t timer_ms(void)
{
  // Return the value from SDL_GetTicks(), which returns time in ms.
  return (uint32_t)SDL_GetTicks();
}

// Time in seconds
uint32_t timer_sec(void)
{
  // Return the value from SDL_GetTicks(), converting to seconds.
  return (uint32_t)(SDL_GetTicks() / 1000);
}

// Snooze (wait + release CPU)
void timer_snooze(uint32_t t)
{
  // Just call through into SDL.
  SDL_Delay(t);
}

// 1 microsecond = 0.001 millisecond

static const int64_t g_iFreq = 1000000;

#define GET_TICK(tmp) *tmp = (((int64_t)SDL_GetTicks()) * 1000)

// stop timer
void timer_Stop(SYS_TIMER *tm)
{
    tm->flags &= ~SYS_TIMER_FLAGS_START;
}

// clear timer
void timer_Reset(SYS_TIMER *tm)
{
    tm->iCounter = 0;
    tm->fCounter = 0.f;
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
			timer_snooze(1);
		else
		{
			int i=0;
			for (;i<10;i++)
				#ifdef __amigaos4__
					SDL_Delay( 0 );
				#else
					sched_yield();
				#endif
		}

    }while(ticks_left>=0);

	tm->fFrameDelta = (float)((double)ticks_passed / (double)g_iFreq);
	tm->fCounter = tm->fFrameDelta * (float) tm->iFreq;
    tm->iCounter = (int32_t)(tm->fCounter * 65535.f);

    GET_TICK(&tm->tStart);
}

void timer_Start(SYS_TIMER *tm, int iFreq, int iMinFrame)
{
	tm->iFreq = iFreq;
	tm->iMinFrame = iMinFrame;

    GET_TICK(&tm->tStart);
    timer_Update(tm);
}

int32_t thread_begin(SYS_THREAD *pThread, enum SYS_THREAD_PRIORITY_ENUM priority)
{
  pThread->nStatus = 0;
  pThread->hThread = (void *)SDL_CreateThread((int(*)(void *))pThread->pFunc, pThread->pArgument);
  if (pThread->hThread != NULL)
  {
    pThread->nStatus = 1;
    return 0;
  }
  else
    return -1;
}

void thread_end(SYS_THREAD *pThread)
{
  SDL_WaitThread(pThread->hThread, NULL);
  pThread->nStatus = 0;
}

void thread_exit(int code)
{
  // TODO: Implement. However, this function is not actually necessary for No Gravity.
}

int mutex_init(SYS_MUTEX *mutex)
{
  if (mutex == NULL)
  {
    return -1;
  }
  if (mutex->hMutex != NULL)
  {
    SDL_DestroyMutex((SDL_mutex *)mutex->hMutex);
  }
  mutex->hMutex = SDL_CreateMutex();
  return (mutex->hMutex != NULL) ? 0 : -1;
}

int mutex_destroy(SYS_MUTEX *mutex)
{
  if ((mutex == NULL) ||
      (mutex->hMutex == NULL))
  {
    return -1;
  }
  SDL_DestroyMutex((SDL_mutex *)mutex->hMutex);
  mutex->hMutex = NULL;
  return 0;
}

int mutex_lock(SYS_MUTEX *mutex)
{
  if ((mutex == NULL) ||
      (mutex->hMutex == NULL))
  {
    return -1;
  }
  SDL_mutexP((SDL_mutex *)mutex->hMutex);
  return 0;
}

int mutex_unlock(SYS_MUTEX *mutex)
{
  if ((mutex == NULL) ||
      (mutex->hMutex == NULL))
  {
    return -1;
  }
  SDL_mutexV((SDL_mutex *)mutex->hMutex);
  return 0;
}

