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
#ifndef __SYSLIST_H
#define __SYSLIST_H

typedef struct _q_item {
    void           *data;
    struct _q_item *next;
} QueueItem;

typedef struct {
  QueueItem  *firstItem;
  QueueItem  *lastItem;
} Queue;

__extern_c

_RLXEXPORTFUNC    void  RLXAPI  QueueMerge(Queue   *q1, Queue   *q2);
_RLXEXPORTFUNC    Queue RLXAPI *QueueCreate(void);
_RLXEXPORTFUNC    void  RLXAPI  QueueDestroy( Queue *q );
_RLXEXPORTFUNC    void  RLXAPI  QueueDestroyFirst( Queue *q );
_RLXEXPORTFUNC    void  RLXAPI  QueueDestroyLast( Queue *q );
_RLXEXPORTFUNC    void  RLXAPI  QueueSort(Queue *q, int (*sort_function)( const void *a, const void *b));
_RLXEXPORTFUNC    void  RLXAPI  QueueDestroyPosition( Queue *q, int n);
_RLXEXPORTFUNC    void  RLXAPI *QueuePosition( const Queue *q, int n);
_RLXEXPORTFUNC    void  RLXAPI  QueueRandom( Queue *q, int fileco);
_RLXEXPORTFUNC    int   RLXAPI  QueueSize( const Queue *q );
_RLXEXPORTFUNC    void  RLXAPI  QueueInsertTop( Queue *q, void *item );
_RLXEXPORTFUNC    void  RLXAPI  QueueInsertBottom( Queue *q, void *item );
_RLXEXPORTFUNC    void  RLXAPI *QueueGetItem(Queue *q );
_RLXEXPORTFUNC    int   RLXAPI  QueueSearchItem( const Queue *q, void *item, int itemSize );

__end_extern_c

#endif

