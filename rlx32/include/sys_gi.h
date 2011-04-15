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
#ifndef __RLXGI_H
#define __RLXGI_H

// Not yet documented
enum {
   GI_INPUTKEYAXISX =  0x1,
   GI_INPUTKEYAXISY =  0x2,
   GI_INPUTKEYMOUSE =  0x4,
   GI_SYSTEMCTRL =  0x8,
   GI_KEYMNG_ENABLE = 0x10,
   GI_KEYMNG_BUFFERED = 0x20
};

#define GI_KEYNOMESSAGE  0x0
#define GI_KEYNEXTITEM   0x1
#define GI_KEYPREVITEM   0x2
#define GI_KEYCLICKITEM  0x4
#define GI_KEYCANCELITEM 0x8

#define GI_AREARELATIVE  0x1


#define GI_MAXMESSAGES    32

#define GI_CURSORFUNC_REDRAW    0x1
#define GI_CURSORFUNC_SHOW      0x2
#define GI_CURSORFUNC_FLIP      0x4
#define GI_CURSORFUNC_CLICKED   0x8
#define GI_CURSORFUNC_RESET  (1|2|4)


typedef struct {
  int32_t           x, y, lx, ly;
  unsigned       flags;
  unsigned       keyMessage;
  unsigned       clickMessage;
}GI_ClickArea;

typedef struct {
  unsigned       id_mesg;
  unsigned char  key;
  unsigned char  flags;
  unsigned char  pad[2];
}GI_Message;

typedef struct _gi_panel{
  unsigned       maxItems;
  unsigned       maxAllocatedItems;
  unsigned       maxKeyMessages;
  unsigned       mode;
  unsigned       display;
  int            onArea;
  int            onAreaClicked;
  int            resX, resY;
  GI_ClickArea  *item;
  GI_Message    *keyMesg;
  int          (*Cursor)(struct _gi_panel *panel, unsigned mode, int message);
}GI_Panel;

typedef struct {

  // Interface Tools
  GI_Panel     *(*PanelOpen)(GI_Panel *panel, char *name, unsigned mode);
  GI_Panel     *(*PanelNew)(unsigned mode, unsigned maxClickAreas);
  GI_Panel     *(*PanelScale)(GI_Panel *panel, unsigned mode, int32_t x, int32_t y, int32_t dx, int32_t dy);
  GI_ClickArea *(*ClickAreaNew)(GI_Panel *panel, unsigned mode, int32_t x, int32_t y, int32_t x2, int32_t y2);
  void          (*SetKeyMessage)(GI_Panel *panel, unsigned key, unsigned id_mesg);
  void          (*SetClickMessage)(GI_Panel *panel, unsigned clickArea, unsigned id_mesg);
  void          (*SetCursorCallback)(GI_Panel *panel, int (*callback)(GI_Panel *panel, unsigned mode, int mesg));
  unsigned      (*GetMessage)(GI_Panel *panel, unsigned mode);
  void          (*SetDefaultMessaging)(GI_Panel *panel);
  void          (*HoldPanel)(unsigned slot);
  int           (*GetText)(char *texte, unsigned maxCharacters, int (*callback)(char *, unsigned, int ));

  // GDI Tools
  void          (*Printf)(char *tx, int x, int y, unsigned flags);
  void          (*ProgressBarSet)(int x, int mx);
  void          (*DisplayWindow)(int lx, int ly, unsigned flags);
  void          (*Clear)(void);
  int           (*OpenLoadPanel)(char *name, char *title, int len, char *filter);
  int           (*OpenSavePanel)(char *name, char *title, int len, char *filter);
  int           (*Dialog)(char *question);
}GI_Descriptor;

__extern_c
   extern  GI_Descriptor *GI_GetBasicDescriptor(GI_Descriptor *GI);
   extern  GI_Descriptor *GI_GetSystemDescriptor(GI_Descriptor *GI);
   extern  GI_Descriptor RLX_GI;
__end_extern_c

#endif

