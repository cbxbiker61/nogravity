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


#define SPRITE_CLIPPING \
if (y>g_pRLX->pGX->View.ymax) \
return; \
if (x>g_pRLX->pGX->View.xmax) \
return; \
if (y+ly<g_pRLX->pGX->View.ymin) \
return; \
if (x+lx<g_pRLX->pGX->View.xmin) \
return; \
if (y+ly>g_pRLX->pGX->View.ymax+1) \
ly = g_pRLX->pGX->View.ymax+1 - y; \
if (x+lx>g_pRLX->pGX->View.xmax+1) \
lx = g_pRLX->pGX->View.xmax+1 - x; \
if (y<g_pRLX->pGX->View.ymin)\
{\
int j = g_pRLX->pGX->View.ymin - y;\
if (ly<=j) \
return; \
ly-=j; oy = j; y = g_pRLX->pGX->View.ymin;\
}\
if (x<g_pRLX->pGX->View.xmin) \
{\
int j = g_pRLX->pGX->View.xmin-x; \
if (lx<=j) \
return; \
lx-=j; ox = j; x = g_pRLX->pGX->View.xmin;\
}\
if ((lx==0)||(ly==0)) return;


#define SPRITE_CLIPPING_2 \
if (ly==0)\
return;\
if (lx==0) \
return;\
dx = (sp->LX<<16)/lx;\
dy = (sp->LY<<16)/ly;\
if (y>g_pRLX->pGX->View.ymax)\
return;\
if (x>g_pRLX->pGX->View.xmax) \
return;\
if (y+ly<g_pRLX->pGX->View.ymin) \
return;\
if (x+lx<g_pRLX->pGX->View.xmin) \
return;\
if (y+ly>=g_pRLX->pGX->View.ymax) \
ly = g_pRLX->pGX->View.ymax - y;\
if (x+lx>=g_pRLX->pGX->View.xmax)\
lx = g_pRLX->pGX->View.xmax - x;\
if (y<g_pRLX->pGX->View.ymin)\
{\
j = g_pRLX->pGX->View.ymin-y;\
ly-=j;\
oy+= dy * j;\
y = g_pRLX->pGX->View.ymin;\
}\
if (x<g_pRLX->pGX->View.xmin)\
{\
j = g_pRLX->pGX->View.xmin-x;\
lx-=j;\
ox+= dx * j;\
x=g_pRLX->pGX->View.xmin;\
}\
if (ly<=0) \
return;\
if (lx<=0) \
return;

