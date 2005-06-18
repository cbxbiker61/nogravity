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
#include "_rlx.h"
#include "sysresmx.h"
#include "gx_csp.h"
#include "sysctrl.h"
#include "systools.h"
#include "syslist.h"
#include "iss_defs.h"
#include "sys_gi.h"
GI_Descriptor RLX_GI;
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  GI_ClickArea *ClickAreaNew(GI_Panel *panel, unsigned mode, int32_t x, int32_t y, int32_t x2, int32_t y2)
*
* DESCRIPTION :
*
*/
static GI_ClickArea *ClickAreaNew(GI_Panel *panel, unsigned mode, int32_t x, int32_t y, int32_t x2, int32_t y2)
{
    GI_ClickArea *click = panel->item + panel->maxItems;
    if (panel->maxItems>=panel->maxAllocatedItems) return NULL;
    panel->maxItems++;
    click->x = x;
    click->y = y;
    if (mode&GI_AREARELATIVE)
    {
        click->lx = x2;
        click->ly = y2;
    }
    else
    {
        click->lx = x2 - x;
        click->ly = y2 - y;
    }
    return click;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  GI_Panel *PanelOpen(char *name, unsigned mode)
*
* DESCRIPTION :
*
*/
static GI_Panel *PanelOpen(GI_Panel *p, char *name, unsigned mode)
{
    SYS_FILEHANDLE in = FIO_cur->fopen(name, "rb");
    GI_Panel *panel = NULL;
    if (in)
    {
        char header[4];
        unsigned i, max;
        if (!p)
        {
            panel = MM_CALLOC(1, GI_Panel);
            panel->mode |= mode;
        }
        else panel = p;
        FIO_cur->fread(header, sizeof(char), 4, in);
        {
            u_int16_t _m;
            FIO_cur->fread(&_m, sizeof(unsigned short int), 1, in);
            #ifdef __BIG_ENDIAN__
            BSWAP16(&_m, 1);
            #endif
            max = _m;
        }
        panel->maxItems += max;
        if (!p)
        {
            panel->item = (GI_ClickArea*) MM_heap.malloc(max * sizeof(GI_ClickArea));
        }
        else
        {
            panel->item = (GI_ClickArea*) MM_heap.realloc(panel->item, panel->maxItems*sizeof(GI_ClickArea));
        }
        for (i=0;i<max;i++)
        {
            GXSPRITEFORMAT spl;
            FIO_cur->fread(&spl, sizeof(GXSPRITEFORMAT), 1, in);
            #ifdef __BIG_ENDIAN__
            BSWAP16((u_int16_t*)&spl.a, 4);
            #endif
            ClickAreaNew(panel, 0, spl.a, spl.b, spl.c, spl.d);
        }
        FIO_cur->fclose(in);
    }
    return panel;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  GI_Panel *PanelNew(unsigned mode, unsigned maxClickAreas)
*
* DESCRIPTION :
*
*/
static GI_Panel *PanelNew(unsigned mode, unsigned maxClickAreas)
{
    GI_Panel *panel = MM_CALLOC(1, GI_Panel);
    panel->maxAllocatedItems = maxClickAreas;
    panel->item = MM_CALLOC(maxClickAreas, GI_ClickArea);
    panel->keyMesg = MM_CALLOC(GI_MAXMESSAGES, GI_Message);
    UNUSED(mode);
    return panel;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  GI_Panel *PanelScale(GI_Panel *panel, unsigned mode, int32_t x, int32_t y, int32_t dx, int32_t dy)
*
* DESCRIPTION :
*
*/
static GI_Panel *PanelScale(GI_Panel *panel, unsigned mode, int32_t x, int32_t y, int32_t dx, int32_t dy)
{
    unsigned i;
    GI_ClickArea *click = panel->item, *click2;
    GI_Panel *pan;
    if (mode & 1)
    {
        pan = PanelNew(0, panel->maxItems);
    }
    else
    {
        pan = panel;
    }
    click2 = pan->item;
    for (i=0;i<panel->maxItems;i++, click++, click2++)
    {
        click2->x =  click->x * dx;
        click2->y =  click->y * dy;
        click2->lx = (click->lx * x) / panel->resX;
        click2->ly = (click->ly * y) / panel->resY;
    }
    UNUSED(mode);
    return panel;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void SetKeyMessage(GI_Panel *panel, unsigned key, unsigned id_mesg)
*
* DESCRIPTION :
*
*/
static void SetKeyMessage(GI_Panel *panel, unsigned key, unsigned id_mesg)
{
    GI_Message    *mesg = panel->keyMesg + panel->maxKeyMessages;
    if ( panel->maxKeyMessages < GI_MAXMESSAGES)
    {
        mesg->key = (u_int8_t)key;
        mesg->id_mesg = id_mesg;
        mesg->flags  |= GI_KEYMNG_ENABLE|GI_KEYMNG_BUFFERED;
    }
    panel->maxKeyMessages++;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void SetClickMessage(GI_Panel *panel, unsigned clickArea, unsigned id_mesg)
*
* DESCRIPTION :
*
*/
static void SetClickMessage(GI_Panel *panel, unsigned clickArea, unsigned id_mesg)
{
    panel->item[clickArea].clickMessage = id_mesg;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void SetCursorCallback(GI_Panel *, (void)callback(GI_Panel *panel, unsigned mode))
*
* DESCRIPTION :
*
*/
static void SetCursorCallback(GI_Panel *panel, int (*callback)(GI_Panel *panel, unsigned mode, int message))
{
    panel->Cursor = callback;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void GetMessage(GI_Panel *panel, unsigned mode)
*
* DESCRIPTION :
*
*/
static int GI_IsMouseInsideArea(GI_ClickArea *area)
{
    int32_t x = (int32_t) sMOU->x;
    int32_t y = (int32_t) sMOU->y;
    return (
    (x>=area->x)
    && (x<=area->x+area->lx)
    && (y>=area->y)
    && (y<=area->y+area->ly)
    );
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int GI_GetArea(GI_Panel *panel)
*
* DESCRIPTION :
*
*/
static int GI_GetArea(GI_Panel *panel)
{
    unsigned i;
    int area = -1;
    GI_ClickArea *click = panel->item;
    for (i=0;i<panel->maxItems;i++, click++)
    {
        if (GI_IsMouseInsideArea(click)) area = i;
    }
    panel->display|=1;
    return area;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static unsigned GetMessage(GI_Panel *panel, unsigned mode)
*
* DESCRIPTION :
*
*/
static unsigned GetMessage(GI_Panel *panel, unsigned mode)
{
    unsigned mesg = 0;
    unsigned i, md = 0;
	int moved = 1;
    int onArea, oldArea = panel->onArea;
    GI_Message *ms = panel->keyMesg;
    for (i=0;i<panel->maxKeyMessages;i++, ms++)
    {
        if (ms->flags&GI_KEYMNG_ENABLE)
        {
            if (ms->flags&GI_KEYMNG_BUFFERED)
            {
                if (sKEY_IsClicked( ms->key  )) mesg =  ms->id_mesg;
            }
            else
            {
                if (sKEY_IsHeld( ms->key )) mesg =  ms->id_mesg;
            }
        }
    }
    onArea = moved ? GI_GetArea(panel) : panel->onArea;
    if (mesg == GI_KEYNEXTITEM)        { panel->onArea++; panel->display^=1;}
    if (mesg == GI_KEYPREVITEM)        { panel->onArea--; panel->display^=1;}
    if (panel->onArea<0)                 panel->onArea = panel->maxItems-1;
    if (panel->onArea>(int)(panel->maxItems-1)) panel->onArea = 0;
    if (sMOU_IsClicked(0))
    {
        panel->onAreaClicked = panel->onArea;
        md |= GI_CURSORFUNC_CLICKED;
    }
    else
    {
        if ((panel->onArea>=0) && (panel->onArea == panel->onAreaClicked))
        {
            mesg = GI_KEYCLICKITEM;
            panel->onAreaClicked = -1;
        }
    }
    panel->onArea = onArea;
    if (onArea!=oldArea)
    {
        md |= GI_CURSORFUNC_REDRAW;
    }
    if (panel->display&1) md |= GI_CURSORFUNC_SHOW;
    if (panel->display&2) md |= GI_CURSORFUNC_FLIP;
    if (panel->Cursor) mesg = panel->Cursor(panel, md, mesg);
    UNUSED(mode);
    return mesg;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  GI_Descriptor *GI_GetBasicDescriptor(GI_Descriptor *GI)
*
* DESCRIPTION :
*
*/
static void SetDefaultMessaging(GI_Panel *panel)
{
    GI_Message *keyMesg = panel->keyMesg;
    keyMesg[0].key = s_return;
    keyMesg[0].id_mesg = GI_KEYCLICKITEM;
    keyMesg[0].flags  |= GI_KEYMNG_ENABLE|GI_KEYMNG_BUFFERED;
    keyMesg[1].key = s_esc;
    keyMesg[1].id_mesg = GI_KEYCANCELITEM;
    keyMesg[1].flags  |= GI_KEYMNG_ENABLE|GI_KEYMNG_BUFFERED;
    keyMesg[2].key = s_right;
    keyMesg[2].id_mesg = GI_KEYNEXTITEM;
    keyMesg[2].flags  |= GI_KEYMNG_ENABLE|GI_KEYMNG_BUFFERED;
    keyMesg[3].key = s_left;
    keyMesg[3].id_mesg = GI_KEYPREVITEM;
    keyMesg[3].flags  |= GI_KEYMNG_ENABLE|GI_KEYMNG_BUFFERED;
    keyMesg[4] = keyMesg[2]; keyMesg[4].key = s_up;
    keyMesg[5] = keyMesg[3]; keyMesg[4].key = s_down;
    panel->maxKeyMessages = 6;
    panel->display       |= 2|1;
    panel->onArea = GI_GetArea(panel);
    panel->onAreaClicked = -1;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int GetText(char *texte, unsigned maxCharacters, int (*callback)(char *, unsigned, int ))
*
* Description :  
*
*/
static int GetText(char *texte, unsigned maxCharacters, int (*callback)(char *, unsigned, int ))
{
    int ok = 0;
    char __temp[256];
    unsigned l = strlen(texte);
    sysStrnCpy(__temp, texte, maxCharacters);
    do
    {
        unsigned ch;
        int mode = 0;
        sKEY->Update(0);
		sJOY->Update(0);
		ch = sKEY->charCode;
        if (ch)
        {
            switch(ch) {
                case 8:  // BackSpace
                if (l>0)
                {
                    __temp[l-1] = 0;
                    l--;
                    mode|=1;
                }
                break;
                case 13: // Return
                sysStrCpy(texte, __temp);
                case 27: // Espace
                ok=1;
                break;
                default: // Alpha
                if (l<maxCharacters)
                {
                    if ((ch>=32) && (ch<128))
                    {
                        __temp[l]=(u_int8_t)ch;
                        l++;
                        __temp[l]=0;
                        mode|=1;
                    }
                }
                break;
            }
        }
        if (callback) ok = callback(__temp, mode, ok);
    }while(ok!=1);
    return ok;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  GI_Descriptor *GI_GetBasicDescriptor(GI_Descriptor *GI)
*
* DESCRIPTION :
*
*/
GI_Descriptor *GI_GetBasicDescriptor(GI_Descriptor *GI)
{
    if (!GI) GI = &RLX_GI;
    GI->PanelOpen = PanelOpen;
    GI->PanelNew = PanelNew;
    GI->PanelScale = PanelScale;
    GI->ClickAreaNew = ClickAreaNew;
    GI->SetKeyMessage  = SetKeyMessage;
    GI->SetClickMessage = SetClickMessage;
    GI->SetCursorCallback = SetCursorCallback;
    GI->GetMessage = GetMessage;
    GI->SetDefaultMessaging = SetDefaultMessaging;
    GI->GetText  = GetText;
    return GI;
}
