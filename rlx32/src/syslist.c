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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "_rlx32.h"
#include "systools.h"
#include "sysini.h"
#include "syslist.h"
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
Queue *QueueCreate(void)
{
    Queue *q;
    q = (Queue *) (sizeof(Queue));
    q->firstItem = q->lastItem = NULL;
    return q;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void QueueMerge(Queue *q1, Queue   *q2)
{
    if (q2->firstItem==NULL)
    {
        MM_heap.free(q2);
        return;
    }
    if (q1->firstItem==NULL)
    {
        *q1 = *q2;
        MM_heap.free(q2);
        return;
    }
    q1->lastItem->next=q2->firstItem;
    q1->lastItem=q2->lastItem;
    MM_heap.free(q2);
    return ;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void QueueDestroy( Queue *q )
{
    QueueItem*item, *item2;
    if( !q ) return;
    item = q->firstItem;
    while( item != NULL )
    {
        item2 = item->next;
        if (item->data) MM_heap.free(item->data);
        MM_heap.free(item);
        item = item2;
    }
    MM_heap.free(q);
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void QueueDestroyFirst( Queue *q )
{
    QueueItem*item, *item2;
    if( !q ) return;
    item = q->firstItem;
    item2= q->firstItem->next;
    if ( item != NULL )
    {
        if(item->data) MM_heap.free(item->data);
        MM_heap.free(item);
        q->firstItem=item2;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void QueueDestroyLast( Queue *
q )
{
    QueueItem*item, *item2;
    if( !q ) return;
    item = q->lastItem;
    item2= q->lastItem->next;
    if ( item != NULL )
    {
        if (item->data) MM_heap.free(item->data);
        MM_heap.free(item);
        q->lastItem=item2;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void QueueDestroyPosition( Queue *q, int n)
{
    QueueItem*item, *item2;
    int i = 0;
    if( !q ) return;
    item = q->firstItem;
    if (n==0) QueueDestroyFirst(q);
    else
    {
        while(!  ((i == n-1)  || (item == NULL))   )
        {
            item = item->next;
            i++;
        }
        if (item!=NULL)
        {
            item2 = item->next;
            if ( item2 != NULL )
            {
                if (item2->next==NULL)
                {
                    q->lastItem = item;
                    q->lastItem->next = q->firstItem;
                }
                if (item2->data) MM_heap.free(item2->data);
                item->next = item2->next;
                MM_heap.free(item2);
            }
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void QueueInsertTop( Queue *q, void *data )
{
    QueueItem*item = (QueueItem *) (sizeof(QueueItem));
    if( !q ) return;
    if( !item ) return;
    item->next = q->firstItem;
    q->firstItem = item;
    if( q->lastItem == NULL ) q->lastItem = item;
    item->data = data;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void QueueInsertBottom( Queue *q, void *data )
{
    QueueItem*item = (QueueItem *)MM_heap.malloc(sizeof(QueueItem));
    if( !q ) return;
    if( !item ) return;
    item->next = NULL;
    if( q->firstItem == NULL )
    {
        q->firstItem = q->lastItem = item;
    }
    else
    {
        q->lastItem->next = item;
        q->lastItem = item;
    }
    item->data = data;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void *QueueGetItem(  Queue *q )
{
    void *data;
    QueueItem *item;
    if( !q || !q->firstItem ) return NULL;
    data = q->firstItem->data;
    item = q->firstItem->next;
    MM_heap.free(q->firstItem);
    if((q->firstItem = item) == NULL) q->lastItem = NULL;
    return data;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
int QueueSearchItem( const Queue *q, void *data, int dataSize )
{
    QueueItem*item;
    if( !q ) return 0;
    item = q->firstItem;
    while( item && memcmp(data, item->data, dataSize) != 0 ) item = item->next;
    if( item ) return 1;
    else return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void *QueuePosition( const Queue *q, int n )
{
    QueueItem *item;
    void *itemd;
    int i=0;
    if (q==NULL) return(NULL);
    item = q->firstItem;
    do
    {
        itemd = item->data;
        item = item->next;
        i++;
    }while(!(((i-1) == n)  || (item == NULL)));
    return (itemd);
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
int QueueSize( const Queue *q )
{
    QueueItem*item;
    int i=0;
    if( q != NULL )
    {
        item = q->firstItem;
        while( item != NULL )
        {
            i++;
            item = item->next;
        }
    }
    return i;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void QueueRandom( Queue *q, int fileco)
{
    short int i, a;
    int b;
    QueueItem *i1, *i2;
    void        *temp;
    fileco=QueueSize(q);
    if( fileco > 1 )
    {
        for(i = 0; i < fileco; i++)
        {
            b = sysRand(fileco);
            if( i != b )
            {
                for( a = 0, i1 = q->firstItem; a < i; a++, i1 = i1->next ){}
                for( a = 0, i2 = q->firstItem; a < b; a++, i2 = i2->next ){
                }
                temp = i1->data;
                i1->data = i2->data;
                i2->data = temp;
            }
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void QueueSwapItem(QueueItem *a, QueueItem *b)
*
* DESCRIPTION :  
*
*/
static void QueueSwapItem(QueueItem *a, QueueItem *b)
{
    QueueItem c;
    c.data = a->data;
    a->data = b->data;
    b->data = c.data;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void QueueSort(Queue *q, int (*sort_function)( const void *a, const void *b))
*
* DESCRIPTION :  
*
*/
void QueueSort(Queue *q, int (*sort_function)( const void *a, const void *b))
{
    QueueItem *a = q->firstItem;
    while(a!=NULL)
    {
        QueueItem *b=a->next;
        while (b!=NULL)
        {
            if (sort_function(a->data, b->data))
            {
                QueueSwapItem(a, b);
            }
            b=b->next;
        }
        a=a->next;
    }
    return;
}
