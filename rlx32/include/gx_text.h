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
#ifndef __GX_TEXTH
#define __GX_TEXTH
enum {
      C_NOIR, C_BLEU, C_VERT,  C_CYAN,  C_ROUGE, C_VIOLET,  C_MARRON,   C_BLANC
};
#define C_CLAIR(x)                    (x)+8
#define GXT_print(tx, X, Y, COL1, COL2)   writeStr(tx, X, Y, COLORTEXT(COL1, COL2, 0), strlen(tx));
#define GXT_string(tx, c, len)          {sysMemSet(tx, c, len);tx[len]=0;}
#define GXT_lineX(a, b, c, lx)           {tex[0]=a;sysMemSet(tex+1, b, lx);tex[lx+1]=c;tex[lx+2]=0;}
#define GXT_getColor(x, y, c)           c=GX.View.lpFrontBuffer[(y*GX.View.xmax+x)*2]
#define GXT_getColor2(x, y, cA, cB)      {cA=GX.View.lpFrontBuffer[(y*GX.View.xmax+x)*2+1];cB=cA>>4;cA&=15;}
__extern_c

    void     GXT_shadow(int x, int y);
    int      GXT_DrawMenuBox(int colA, int colB, int lx, int y, int ly);
    int      GXT_DrawMenuBox2(char *name, int colA, int colB, int lx, int y, int ly);
    int      GXT_Display_BoxMenu(char *name, char *item[], char *info[], int *but, int *ok, int lx, int miny);
    void     GXT_DisplayScreen(char *game, char *copy);
    void     GXT_dispMessage(char *szTitle, char *tex);
    void     GXT_init(void);

__end_extern_c
#endif
