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
#include <stdlib.h>
#include <stdio.h>
#include "_rlx32.h"
#include "sysresmx.h"
#include "systools.h"
/*****/
#include "gx_struc.h"
#include "gx_tools.h"
/*****/
#include "v3xdefs.h"
#include "v3xrend.h"
#include "v3x_2.h"
#include "v3x_1.h"
#include "v3xsort.h"
/*------------------------------------------------------------------------
*
* PROTOTYPE  :void quicksort_recurse( V3XPOLY *si, V3XPOLY *sj)
*
* DESCRIPTION : Trie de faces 'Quick Sort'
*
*/
#define ALGO1(fce)  ((fce).distance)
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RLXAPI v3xpoly_SortByDistance( V3XPOLY **si, V3XPOLY **sj)
*
* DESCRIPTION :  
*
*/
void RLXAPI v3xpoly_SortByDistance( V3XPOLY **si, V3XPOLY **sj)
{
    V3XPOLY **p = (si+((sj-si)>>1)), **sk, **sl;
    V3XPOLY  *q;
    V3XSCALAR p0 = ALGO1(**p);
    for ( sk = si, sl = sj ; sk<=sl ; )
    {
        for( ; ALGO1(**sk)<p0 ; sk++ ){}
        for( ; p0<ALGO1(**sl) ; sl-- ){}
        if (sk<=sl)
        {
            if (sk!=sl)
            {
                q = *sk;
                *sk = *sl;
                *sl = q;
            }
            sk++; sl--;
        }
    }
    if (si<sl) v3xpoly_SortByDistance(si, sl);
    if (sk<sj) v3xpoly_SortByDistance(sk, sj);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  #define ALGO2(fce)  (((V3XMATERIAL*)(fce).Mat)->RenderID)
*
* DESCRIPTION :  
*
*/
#define ALGO2(fce)  (((V3XMATERIAL*)(fce).Mat)->RenderID)
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void RLXAPI v3xpoly_SortByTranparency( V3XPOLY **si, V3XPOLY **sj)
*
* DESCRIPTION :  
*
*/
void RLXAPI v3xpoly_SortByID( V3XPOLY **si, V3XPOLY **sj)
{
    V3XPOLY **p = (si+((sj-si)>>1)), **sk, **sl;
    V3XPOLY  *q;
    u_int32_t p0 = ALGO2(**p);
    for ( sk = si, sl = sj ; sk<=sl ; )
    {
        for( ; ALGO2(**sk)<p0 ; sk++ ){}
        for( ; p0<ALGO2(**sl); sl-- ){}
        if (sk<=sl)
        {
            if (sk!=sl)
            {
                q = *sk;
                *sk = *sl;
                *sl = q;
            }
            sk++; sl--;
        }
    }
    if (si<sl) v3xpoly_SortByID(si, sl);
    if (sk<sj) v3xpoly_SortByID(sk, sj);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  #define ALGO3(fce)  ((u_int32_t)(((V3XMATERIAL*)(fce).Mat)->texture[0].handle))
*
* Description :  
*
*/
#define ALGO3(fce)  ((u_int32_t)(((V3XMATERIAL*)(fce).Mat)->texture[0].handle))
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void RLXAPI v3xpoly_SortByTexture( V3XPOLY **si, V3XPOLY **sj)
{
    V3XPOLY **p = (si+((sj-si)>>1)), **sk, **sl;
    V3XPOLY  *q;
    u_int32_t p0 = ALGO3(**p);
    for ( sk = si, sl = sj ; sk<=sl ; )
    {
        for( ; ALGO3(**sk)<p0 ; sk++ ){}
        for( ; p0<ALGO3(**sl); sl-- ){}
        if (sk<=sl)
        {
            if (sk!=sl)
            {
                q = *sk;
                *sk = *sl;
                *sl = q;
            }
            sk++; sl--;
        }
    }
    if (si<sl) v3xpoly_SortByTexture(si, sl);
    if (sk<sj) v3xpoly_SortByTexture(sk, sj);
    return;
}
