/*
** Copyright (c) 1999, 2000 3Dfx Interactive, Inc. 
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
*/

#ifndef __GLIDEMEM_H__
#define __GLIDEMEM_H__

#define GR_BAD_OFFSET 0xffffffff
typedef unsigned GrMemOffset;

typedef struct GrMemBlock {
    struct GrMemBlock   *next;
    unsigned            isfree;
    GrMemOffset         offset;
    size_t              size;
    GrChipID_t          tmu;
}GrMemBlock;

#ifdef __cplusplus

class GrMemManager {
public:
    GrMemManager();
    ~GrMemManager();
    GrMemOffset alloc(size_t size,GrChipID_t tmu = GR_TMU0);
    void        free(GrMemOffset offset,GrChipID_t tmu = GR_TMU0);     
    void        garbageCollection(GrChipID_t tmu = GR_TMU0);
    unsigned    coreleft(GrChipID_t tmu = GR_TMU0) const { return _maxMemory[tmu]; }
    unsigned    getNumbersOfBlock(GrChipID_t tmu = GR_TMU0) const;

protected:      
    GrMemBlock* addBlock(size_t size,GrChipID_t tmu, bool connect);
    GrMemBlock* findBlockByOffset(GrMemOffset offset,GrChipID_t tmu);
    GrMemBlock* findEmptyBlock(size_t size,GrChipID_t tmu);
    void        clearList(GrChipID_t tmu);
    void        initList(GrChipID_t tmu);
    GrMemBlock *_lastBlock[ GLIDE_NUM_TMU ], *_firstBlock[ GLIDE_NUM_TMU ];     
    unsigned    _maxMemory[ GLIDE_NUM_TMU ];        
    int         _strategy;
};

#endif
#endif //__GLIDEMEM_H__
