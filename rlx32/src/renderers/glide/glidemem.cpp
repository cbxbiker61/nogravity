/*
** Copyright (c) 1999, 3Dfx Interactive, Inc. 
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of 3Dfx Interactive, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of 3Dfx Interactive, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
**
** $Header$
** $Log$
** Revision 1.2  2005/02/26 00:15:39  realtech
** Release 6. BeOS bug fixes
**
**
** Description : Glide Texture Memory Manager
** Author : Stéphane Denis (Realtech)
**
*/

#if defined (_WIN32) || defined (WIN32) || defined(__NT__)
#include <windows.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "glide.h"
#include "glidemem.h"

void
GrMemManager::initList(GrChipID_t tmu)
{   
    _maxMemory[tmu] = grTexMaxAddress(tmu) - grTexMinAddress(tmu);  
    _lastBlock[tmu] = _firstBlock[tmu] = addBlock(_maxMemory[tmu], tmu, false);
    _firstBlock[tmu]->isfree = 1;
    _firstBlock[tmu]->offset = grTexMinAddress(tmu);    
    
}

GrMemManager::GrMemManager() : _strategy(0)
{
    initList(GR_TMU0);
    initList(GR_TMU1);      
}   

unsigned 
GrMemManager::getNumbersOfBlock(GrChipID_t tmu) const
{
    unsigned nb = 0;
    GrMemBlock *b = _firstBlock[tmu];
    while (b!=0)
    {
        if (!b->isfree) nb++;
        b = b->next;        
    }   
    return nb;
}
void
GrMemManager::clearList(GrChipID_t tmu)
{
    GrMemBlock *b = _firstBlock[tmu];
    char t[64];
    while (b!=0)
    {
        GrMemBlock *a = b;      
        b = b->next;        
        if (!a->isfree) // Resource leak error check ...
        {       
            sprintf(t,"Glide resource leak, allocated for TMU %d, HANDLE: 0x%p\n",
                a->tmu, a->offset);         
            #ifdef __WIN32__            
                OutputDebugString(t);
            #else
                fprintf(stderr,t);
            #endif
        }
        delete a;
    }   
}

GrMemManager::~GrMemManager()
{
    clearList(GR_TMU0);
    clearList(GR_TMU1); 
}

GrMemBlock *
GrMemManager::findBlockByOffset(GrMemOffset offset,GrChipID_t tmu)
{
     GrMemBlock *b = _firstBlock[tmu];
     while(b!=0)
     {
         if ((b->tmu == tmu) && (b->offset == offset))
         {
              return b;
         }
         b = b->next;
     }
     return NULL;
}

void
GrMemManager::garbageCollection(GrChipID_t tmu)
{
    int ok;
    // collect all small free block to a larger one;
    do
    {
        GrMemBlock *b = _firstBlock[tmu];
        ok = 0;
        while (b!=0)
        {   
            GrMemBlock *c = b->next;
            if ( (b->tmu == tmu) && c)
            {
                if (b->isfree && c->isfree)
                {
                    b->next = c->next;
                    b->size+= c->size;
                    delete c;
                    ok = 1;
                }
            }   
            b = b->next;            
        }
    }while(ok==1);
    return;
}

void
GrMemManager::free(GrMemOffset offset,GrChipID_t tmu)
{
   GrMemBlock *b = findBlockByOffset(offset, tmu);
   assert(b!=0); // add handle
   if (b && (b->tmu == tmu) && (!b->isfree))
   {
       b->isfree = 1; // free that block    
      _maxMemory[tmu]+=size;
   }
   return;
}

GrMemBlock *
GrMemManager::addBlock(size_t size,GrChipID_t tmu, bool connect )
{
    GrMemBlock *b = new GrMemBlock;
    b->size   = size;    
    b->tmu    = tmu;
    b->next   = 0;  
    if (connect)
    {
        b->isfree = 0;
        if (_lastBlock[tmu])
            _lastBlock[tmu]->next = b;  
        _lastBlock[tmu] = b;

        if (!_firstBlock[tmu])
            _firstBlock[tmu] = b;
        _maxMemory[tmu]-=size;
    }
    return b;
}

GrMemBlock*
GrMemManager::findEmptyBlock(size_t size,GrChipID_t tmu)
{
     garbageCollection(tmu);
     // First, try to get a same size block; could occurs often
     // Since the memory are mostly the same size (for a example a common texture is 256x256x2 bytes).       
     
     GrMemBlock *b = _firstBlock[tmu];
     while(b!=0)
     {
         if ((b->tmu==tmu)&&(b->isfree)&&(b->size==size))
         {
             _maxMemory[tmu]-=size;
              b->isfree = 0;
              return b;
         }
         b = b->next;
     }   

     // Not same size match, so, try to get a larger block, and split it     
     b = _firstBlock[tmu];
     GrMemBlock *prev = 0;
     while (b!=0)
     {       
         if ((b->tmu==tmu) && (b->isfree) && (b->size>size))
         {  
              _maxMemory[tmu]-=size              ;
              // split the block into b and c.                        
              GrMemBlock *c = addBlock(b->size - size,tmu, false); // alloc block ..
              c->isfree = 1; // c is a free block
              // Compute the offset ...
              if (prev) b->offset = prev->offset + prev->size;            
              c->offset = b->offset + size;
              _maxMemory[tmu]+=c->size; // ajust memory left
              c->next = b->next; // his next block is the b next block
              b->next = c; // and b next block becomes c              
              b->isfree = 0;
              b->size = size;
              return b;
         }
         prev = b;
         b = b->next;
     }
     // There is no more room. There 2 solutions
     // - Return 0 
     // - alloc into an other TMU ?  
     // - Try to find the largest map, grab the memory, resize it, and replace it.
      
     return NULL;
}


GrMemOffset
GrMemManager::alloc(size_t size,GrChipID_t tmu)
{
    GrMemBlock *b = findEmptyBlock(size, tmu);
    if (b)
    {
        // You can put here an breakpoint if you've got leak error, or an assert(b->offset!=<leak error address>
        return b->offset;
    }
    return GR_BAD_OFFSET;
}


