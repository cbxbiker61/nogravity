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
#ifndef __GUI_OS_H
#define __GUI_OS_H

typedef struct _button_item
{
    short						LX, LY, X, Y;
    struct _button_item		*	parent;
}RW_Button;

enum RW_KEY_MODE 
{
	RW_BOTH,
	RW_VERT,
	RW_HORIZ	
};

typedef struct _RW_Interface
{
    int							numItem;
    u_int8_t					*	parent;
    RW_Button				*	item;
    int							type;
	enum RW_KEY_MODE 			keymode;
}RW_Interface;

typedef int (*PFRWCALLBACK)( struct _RW_Interface *p, int mode);
typedef int (*PFRWTEXTCALLBACK)( char *text, int mode);

typedef struct
{
    GXSPRITEGROUP			*	pFont;
	RW_Interface			*	pCurrent;
	int							isMouseHidden, isLocked, insideMouse;
	int							dx, dy, ch, current, key;
	char					*	szInputText;
}RW_SYSTEM;

__extern_c

   RW_Interface *RW_Interface_Create(int mode);
   RW_Interface *RW_Interface_GetFn(char *filename);
   void RW_Interface_Release(RW_Interface *pInt);
   void RW_Zone_CreateWithSize(RW_Interface *pInt, int x, int y, int lx, int ly);
   void RW_Zone_CreateWithButton(RW_Interface *pInt, RW_Button *But);
   void RW_Zone_CreateWithText(RW_Interface *pInt, char *szText, int x, int y, int bAllocate);
   int  RW_Interface_Scan(RW_Interface *pInt, int pCurSel, int *pRetSel, PFRWCALLBACK pfCallback);
   void RW_Create(GXSPRITEGROUP *pFont);
   void RW_Interface_BuildTree(RW_Interface *pInt);
   void RW_Release(void);
   int RW_InputText(RW_Interface *, char *t, int numChar, PFRWCALLBACK pfCallback);

_RLXEXPORTDATA    extern RW_SYSTEM RW;

__end_extern_c

#endif
