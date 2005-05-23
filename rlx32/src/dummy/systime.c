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

#include "_rlx32.h"
#include "systools.h"
#include "systime.h"

// Time in millisecond
u_int32_t timer_ms(void)
{
  // Return the value from SDL_GetTicks(), which returns time in ms.
  return 0;
}

// Time in seconds
u_int32_t timer_sec(void)
{
  // Return the value from SDL_GetTicks(), converting to seconds.
  return 0;
}

// Snooze (wait + release CPU)
void timer_snooze(u_int32_t t)
{
 
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
    return;
}

void timer_Start(SYS_TIMER *tm, int iFreq, int iMinFrame)
{	
    return;
}

// http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html
typedef void *(*SYS_PTHREAD)(void *);

int32_t thread_begin(SYS_THREAD *pThread, enum SYS_THREAD_PRIORITY_ENUM priority)
{
    return -1;
}

void thread_end(SYS_THREAD *pThread)
{
	return;
}

void thread_exit(int code)
{
    return;
}

// Unix
int mutex_init(SYS_MUTEX *mutex)
{
	return 0;
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

time_t time(time_t *t)
{
	return *t;
}