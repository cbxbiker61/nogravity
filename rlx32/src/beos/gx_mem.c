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
#include <stdlib.h>
#include <stdio.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "systools.h"

static u_int8_t *MM_video_reserve(size_t size)
{
    u_int8_t *v = MM_video.heapAddress    + MM_video.CurrentAddress;
    MM_video.PreviousAddress = MM_video.CurrentAddress;
    {
        size_t off = ((u_int32_t)v+size)&31;
        if (off) size+=32 - off;
    }
    MM_video.CurrentAddress+=size;
	SYS_ASSERT(MM_video.CurrentAddress<MM_video.Size);
    return v;
}

static void *MM_video_malloc(size_t size)
{
    u_int8_t *v=NULL;
    if (MM_video.active)
    {
        v = MM_video_reserve(size);
        sysMemZero(v, size);
    }
    else
    {
        v = (u_int8_t*) MM_std.malloc(size);
        MM_video.TotalAllocated+=size;
        return v;
    }
    return v;
}

static void MM_video_free(void *block)
{
    if (MM_video.active)
    {
        if (block==MM_video.heapAddress+MM_video.PreviousAddress)
        {
            MM_video.CurrentAddress=MM_video.PreviousAddress;
        }
        return;
    }
    if (block!=NULL)
    {
        MM_std.free(block);
        block = NULL;
    }
    return;
}

static void *MM_video_realloc(void *block, size_t size)
{
    u_int8_t *v;
    if (MM_video.active)
    {
        v = MM_video_reserve(size);
        sysMemCpy(v, block, size);
    } else v=(u_int8_t*)MM_std.realloc(block, size);
    return v;
}

static void  MM_videoalloc(void *block, size_t size)
{
    MM_video.heapAddress = (u_int8_t*)block;
    MM_video.Size = size;
	SYS_ASSERT(block);
    return;
}

static unsigned MM_video_push(void)
{
    MM_video.Stack = MM_video.CurrentAddress;
    return MM_video.Stack;
}

static void MM_video_pop(int32_t id)
{
    MM_video.CurrentAddress = (id<0) ? MM_video.Stack : id;
    MM_video.PreviousAddress = 0;
    return;
}

static void MM_video_reset(void)
{
    MM_video.active = (MM_video.heapAddress!=NULL);
    MM_video.CurrentAddress = 0;
    return;
}

SYS_MEMORYMANAGER MM_video = 
{
    MM_video_malloc, 
    MM_video_free, 
    MM_video_realloc, 
    MM_videoalloc, 
    MM_video_push, 
    MM_video_pop, 
    MM_video_reset, 
0};
