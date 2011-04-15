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
#define NG_EXPORT extern
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "_stub.h"
#include "sysresmx.h"
#include "systools.h"
#include "sysini.h"
#include "sysctrl.h"
#include "sysnetw.h"
#include "gx_tools.h"
#include "gx_init.h"
#include "gx_rgb.h"
#include "gx_csp.h"
#include "sysresmx.h"
#include "gx_struc.h"
#include "gx_csp.h"
#include "gx_flc.h"
#include "v3xdefs.h"
#include "v3x_2.h"
#include "v3xcoll.h"
#include "v3xrend.h"
#include "v3xmaps.h"
#include "v3xscene.h"

#include "v3xrend.h"
#include "iss_defs.h"
#include "iss_fx.h"
#include "gui_os.h"
//
#include "lt_struc.h"
#include "lt_data.h"
#include "lt_func.h"
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
#ifndef RLX_IOREADONLY
static char               *MenuGameType[]={g_szGmT[177+5], g_szGmT[177+6], g_szGmT[177+7], g_szGmT[118], NULL};
static char               *MenuProto[16];
static char               *MenuPlayer[16];

static NET_ConnectionType *g_pConnectionType;
static NET_SessionName    *g_pSession;
static NET_PlayerName     *g_pNetPlayer;
uint8_t                      g_szNetPacket[4096];
static LPNET_Informations  lpNet;
static int                 g_nConnectionType, g_nNumConnectionType;

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_NetInitialize(void)
*
* DESCRIPTION :
*
*/
void NG_NetInitialize(void)
{
	if (sNET)
		lpNet = sNET->Initialize(8, 0);
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_NetRelease(void)
*
* DESCRIPTION :
*
*/
void NG_NetRelease(void)
{
	if (!sNET)
		return;
    sNET->ReleaseConnectionList();
	sNET->Release(lpNet);
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void SGNet_ConnectionList(void)
*
* DESCRIPTION :
*
*/
static void SGNet_ConnectionList(void)
{
    unsigned y=0;
    NET_ConnectionType *list;
    if (g_pConnectionType)
       return;
    list = sNET->CreateConnectionList();
    g_pConnectionType = list;
    {
        MenuProto[y++]=g_szGmT[177+0];
        if (list)
        while((list->name[0]!=0)&&(y<7))
        {
            MenuProto[y++]=list->name;
            list++;
        }
    }
    g_nNumConnectionType = y-1;
    MenuProto[y++]=g_szGmT[118];
    MenuProto[y]=NULL;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int SGNet_SelectConnection(int numero)
*
* DESCRIPTION :
*
*/
static int SGNet_SelectConnection(int numero)
{
    if (!g_pConnectionType)
       return 0;
    g_nConnectionType = numero;
    return ( sNET->SelectConnection(g_pConnectionType[numero].ID) == 0 ) ? 1 : 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void SGNet_CreateSessionList(void)
*
* DESCRIPTION :
*
*/
static void SGNet_CreateSessionList(void)
{
    NET_SessionName *Session = sNET->CreateSessionList(0);
    int y = 0;
    if (!g_pConnectionType)
       return;
    g_pSession = Session;
    g_szMenuSessions[y++]=g_szGmT[177+1];
    if (Session==NULL)
    {
        g_SGGame.Session = y;
        g_szMenuSessions[y++]=g_szGmT[177+3];
    }
    else
    {
        while((Session->name[0]!=0)&&(y<15))
        {
            g_szMenuSessions[y++]=Session->name;
            Session++;
        }
    }
    g_SGGame.Session = y-2;
    g_szMenuSessions[y++]=0;
}

static void SGNet_PlayerList(void)
{
    NET_PlayerName *NPlayer = sNET->CreatePlayerList(g_SGGame.Session);
    int y = 0;
    if (!g_pConnectionType) return;
    g_pNetPlayer = NPlayer;
    MenuPlayer[y++]=g_szGmT[177+2];
    if (NPlayer==NULL)
    {
        MenuPlayer[y++]=g_szGmT[177+11];
    }
    else
    {
        g_SGGame.numPlayer=0;
        while((NPlayer->name[0]!=0)&&(y<15))
        {
            MenuPlayer[y++]=NPlayer->name;
            if (sysStriCmp(NPlayer->name, g_pCurrentGame->name)==0)
            {
                g_SGGame.IdPlayer = g_SGGame.numPlayer;
            }
            NPlayer++;
            g_SGGame.numPlayer++;
        }
    }
    MenuPlayer[y++]=0;
    g_SGGame.PlayersName = MenuPlayer+1;
}
#endif

int NG_NetSelectMode(void)
{
#ifndef RLX_IOREADONLY
    while (1)
    {
        switch(NG_ExecMainMenu(MenuGameType, 0, 0xffffffff, 0))
		{
            case 1:
            if (g_pConnectionType)
				return 1;
            SGNet_ConnectionList();
            if (g_pConnectionType)
            {
                while(1)
                {
                    int but = NG_ExecMainMenu(MenuProto, 0, 0xffffffff, 2);
                    if (but<0)
						return -1;
                    if ((but<g_nNumConnectionType)&&(but>=0))
                    {
                        if (SGNet_SelectConnection(but))
							return 1;
                        NG_MenuMessage(g_szGmT[177+8]);
                    }
                }
            }
            else
            {
                NG_MenuMessage(g_szGmT[177+9]);
                return -1;
            }
            case -1:
            return -1;
            default:
            return 0;
        }
    }
    #else
    return 0;
    #endif
}

void NG_NetRender()
{
    unsigned jaune = RGB_PixelFormatEx(GX.ColorTable+4);
    int x1 =  g_SGMenuPos.XZoneMin;
    int x2 =  g_SGMenuPos.XZoneMin+160*2;
    int y0 =  g_SGMenuPos.YZoneMin+RW.pFont->item[0].LY;
    int x = 0, y = y0;

	while (g_szMenuSessions[x]!=NULL)
	{
		if (x)
		{
			int s = x-1;
			if (s==g_SGGame.Session)
				CSP_Color(jaune);
			else
				CSP_Color((GX.View.BytePerPixel > 1 ? RGBA_PixelFormat(255, 255, 255, 0) : 255));
		}
		CSP_WriteText(g_szMenuSessions[x], x1, y, g_pFontMenuSml);
		y+=g_pFontMenuSml->item[0].LY<<(x==0);
		x++;
	}
	CSP_Color((GX.View.BytePerPixel > 1 ? RGBA_PixelFormat(255, 255, 255, 0) : 255));
	if (g_SGGame.Session>=x-1)
		g_SGGame.Session=0;

	if (x==g_SGGame.Session+1)
	{
		CSP_WriteText(g_szGmT[177+10], x1, y, g_pFontMenuSml);
		y+=g_pFontMenuSml->item[0].LY;
	}
	y = y0; x = 0;
	while (MenuPlayer[x]!=NULL)
	{
		CSP_WriteText(MenuPlayer[x], x2, y, g_pFontMenuSml);
		y+=g_pFontMenuSml->item[0].LY<<(x==0);
		x++;
	}
	if (x==1)
	{
		CSP_WriteText(g_szGmT[177+11], x2, y, g_pFontMenuSml);
		y+=g_pFontMenuSml->item[0].LY;
	}
	y = y0;
	x = 0;
}

void NG_NetDisplay(int force)
{
	static uint32_t tim2;
    uint32_t temps;
    if (!g_pConnectionType)
		return;

    temps = timer_ms();

    if ((temps - tim2 > 1000) || (force))
    {
        tim2 = temps;
        SGNet_CreateSessionList(); // View delock‚ avant par securit‚...
        SGNet_PlayerList();
    }
}

enum {
    NGNETp_STEALTH =  1,
    NGNETp_FROZEN =  2,
    NGNETp_NOSHIELD =  4,
    NGNETp_ISSHOT =  8,
    NGNETp_ISAPPEAR = 16,
    NGNETp_VECTOR = 32,
    NGNETp_POSITION = 64
};

typedef struct
{
    unsigned  id;
    V3XVECTOR vect;
    V3XVECTOR pos;
    unsigned  shield;
}PacketShip;

uint8_t *NG_NetTranslateShip(uint8_t *buffer)
{
#ifndef RLX_IOREADONLY
    uint32_t i;
    unsigned   *numPacket = (unsigned  *) buffer;
    PacketShip *packet = (PacketShip*)(buffer+sizeof(int32_t));
    for (i=*numPacket;i!=0;i--)
    {
        int id = packet->id & 1023;
        V3XOVI *OVI = g_SGGame.Scene->OVI + id;
        SGActor *J = (SGActor*)OVI->data;
        if (J)
        {
            if (OVI!=g_pPlayer->J.OVI)
            {
                V3XKEYVECTOR *nm = &OVI->mesh->Tk.dinfo;
                nm->vect = packet->vect;
                nm->pos = packet->pos;
            }
            J->pInf.Shield = packet->shield;
            // new shoot
            if ((OVI!=g_pPlayer->J.OVI)&&(J->pInf.Shield>0))
            {
                if ((packet->id&1024)&&(!J->pInf.ShootOk))
                {
                    NG_WeaponFire(J, J->pInf.Attack, J->OVItarget);
                }
                OVI->state|=V3XSTATE_MATRIXUPDATE;
            }
        }
        packet++;
    }
#endif
    return (uint8_t*)packet;
}

uint8_t *NG_NetDispatchShip(uint8_t *buffer)
{
#ifndef RLX_IOREADONLY
    int i;
    V3XOVI *OVI;
    unsigned   *numPacket = (unsigned  *) buffer;
    PacketShip *packet = (PacketShip*)(buffer+sizeof(int32_t));
    *numPacket = 0;
    for (i=g_SGGame.Scene->numOVI, OVI=g_SGGame.Scene->OVI;i!=0;OVI++, i--)
    {
        if (OVI->ORI->type==V3XOBJ_MESH)
        {
            SGActor *J = (SGActor*)OVI->data;
            if (J)
            if (OVI!=g_pPlayer->J.OVI)
            if (
            (!(OVI->state&V3XSTATE_HIDDEN))
            &&(!J->pInf.AfterDeath)
            &&(J->pInf.Type!=t_ACCESSOIRE)
            &&(J->pInf.Type!=t_OBSTACLE)
            )
            {
                V3XKEYVECTOR *nm = &OVI->mesh->Tk.dinfo;
                packet->id = OVI - g_SGGame.Scene->OVI;
                if (J->pInf.ShootAxe&128)
                {
                    packet->id |=1024;
                    J->pInf.ShootAxe&=127;
                }
                numPacket[0]++;
                packet->vect = nm->vect;
                packet->pos = nm->pos;
                packet->shield = J->pInf.Shield;
                packet++;
            }
        }
    }
    return (uint8_t*)packet;
#else
    return 0;
#endif
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void NG_NetTranslate(SGPlayer *pInf)
*
* DESCRIPTION :
*
*/
unsigned NG_NetTranslate(SGPlayer *pInf, uint8_t *be)
{
    unsigned length = 0;
#ifndef RLX_IOREADONLY
    uint8_t *buffer;
    if (!be) be = g_szNetPacket;
    buffer = be;
    *(uint8_t*)buffer = '$'; buffer++;
    *(uint8_t*)buffer = g_SGGame.IdPlayer ; buffer++;
    *(uint32_t*)buffer = g_SGGame.Count; buffer+=sizeof(int32_t);
    *(uint32_t*)buffer = pInf->Notify; buffer+=sizeof(int32_t);
    pInf->J.pInf.ModeX |= 2;
    if (g_SGSettings.SerialGame)
    {
        if (pInf->Notify&SGNET_HASMOVED)
        {
            sysMemCpy(buffer, &pInf->Rot->pos, sizeof(V3XVECTOR));
            buffer+=sizeof(V3XVECTOR);
        }

        if (pInf->Notify&SGNET_HASTURN)
        {
            sysMemCpy(buffer, pInf->Mat, sizeof(V3XMATRIX));
            buffer+=sizeof(V3XMATRIX);
        }

        if (pInf->Notify&SGNET_NEWSHOOT)
        {
            // Locker
            if (g_pLockTarget)
            {
                *(uint16_t*)buffer = g_pLockTarget - g_SGGame.Scene->OVI;
            }
            else
            *(uint16_t*)buffer = 0;
            buffer+= sizeof(uint16_t);
            // Type de tir
            *buffer = pInf->J.pInf.Attack;
            buffer++;
        }

        if (pInf->Notify&SGNET_UPDATEAIM)
        {
            *(uint8_t*)buffer = g_SGGame.MaxAim[g_SGObjects.NAV];
            buffer++;
        }
        if (pInf->Notify&SGNET_SHIELDCHANGE)
        {
            *(int32_t*)buffer = pInf->J.pInf.Shield;
            buffer+=sizeof(int32_t);
        }
        if (pInf->Notify&SGNET_COMMUNICATE)
        {
            *buffer = (uint8_t)g_SGSettings.ComNumber;
            g_SGSettings.ComTime = MAX_COM_DELAY;
            buffer++;
        }
        if (pInf->Notify&SGNET_CHAT)
        {
            g_SGSettings.ComNumber = COM_Chat;
            g_SGSettings.ComTime = MAX_COM_DELAY;
            NG_AudioBeep(4);
            if (g_szCOM[COM_Chat][0]) // Message non vide ?
            {
                int ml = strlen(g_szCOM[COM_Chat]);
                *buffer = ml;
                sysStrCpy((char*)buffer+1, g_szCOM[COM_Chat]);
                buffer+=ml+1;
            }
        }
    }
    // les avions des autres
    if (pInf->Notify&SGNET_SHIP)
    {
        buffer = NG_NetDispatchShip(buffer);
    }
    else
    {
        *(int32_t*)buffer = 0; buffer+=sizeof(int32_t);
    }
    length = buffer - be;
    if (g_SGSettings.SerialGame)
    {
        sNET->SendData(NET_EVERYBODY, be, length); // Envoie aux autre players
    }
    #endif
    return length;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : static void NG_NetDispatchPlayer(unsigned playerId, uint8_t *buffer)
*
* DESCRIPTION : Recoie l'info et interprete
*
*/
uint32_t NG_NetDispatchPlayer(unsigned playerId, uint8_t *buffer)
{
    uint32_t frame;
    uint8_t *be = buffer;
#ifndef RLX_IOREADONLY
    SGPlayer *pInf = g_SGGame.pPlayer + playerId;
    pInf->dispatched = 1;
    pInf->NetRetry = 128;
    if( !g_SGSettings.SerialGame) buffer+=2;
    frame  = *(uint32_t*)buffer; buffer+=sizeof(int32_t);
    pInf->Notify = *(uint32_t*)buffer; buffer+=sizeof(int32_t);
    pInf->J.pInf.ModeX |= 2;
    if (g_SGSettings.SerialGame)
    {
        if (pInf->Notify&SGNET_HASMOVED)
        {
            sysMemCpy(&pInf->Rot->pos, buffer, 12);
            buffer+=sizeof(V3XVECTOR);
        }
        if (pInf->Notify&SGNET_HASTURN)
        {
            sysMemCpy(pInf->Mat, buffer, sizeof(V3XMATRIX));
            buffer+=sizeof(V3XMATRIX);
        }
        if (pInf->Notify&SGNET_NEWSHOOT)
        {
            uint16_t target=*(uint16_t*)buffer;
            V3XOVI *OVI = target ? g_SGGame.Scene->OVI + target : NULL;
            if ((pInf!=g_pPlayer)&&(!pInf->J.pInf.ShootOk)) NG_WeaponFire(&pInf->J, buffer[2], OVI);
            buffer+=sizeof(uint16_t)+sizeof(uint8_t);
        }
        if (pInf->Notify&SGNET_HASWARP)
        {
            g_SGGame.CameraMode = CAMERA_NEWNAV;
            g_SGSettings.ComNumber = COM_Chat;
            g_SGSettings.ComTime = MAX_COM_DELAY;
            g_SGSettings.ComPlayer = 1+playerId;
            sysStrCpy(g_szCOM[COM_Chat], "has warped...");
            if ((g_SGGame.IsHost)&&(g_SGObjects.FinCode))
            {
                g_SGObjects.quit = 1;
            }
        }
        if (pInf->Notify&SGNET_HASLEFT)
        {
            pInf->J.pInf.AfterDeath = 4;
            pInf->J.pInf.Shield = 0;
            // Cache le player
            g_SGSettings.ComNumber = COM_Chat;
            g_SGSettings.ComTime = MAX_COM_DELAY;
            g_SGSettings.ComPlayer = (uint8_t)(1+playerId);
            sysStrCpy(g_szCOM[COM_Chat], "has leaved the game...");
            pInf->J.pInf.ModeX &= ~2;
        }
        if (pInf->Notify&SGNET_UPDATEAIM)
        {
            g_SGGame.MaxAim[g_SGObjects.NAV]=*(uint8_t*)buffer; buffer++;
            NG_DisplayWarp();
        }
        if (pInf->Notify&SGNET_HASDIE)
        {
            pInf->J.pInf.AfterDeath = 4;
            pInf->J.pInf.Shield = 0;
            // Cach‚ le g_pPlayer,
            g_SGSettings.ComNumber = COM_Chat;
            g_SGSettings.ComTime = MAX_COM_DELAY;
            g_SGSettings.ComPlayer = 1+playerId;
            sysStrCpy(g_szCOM[COM_Chat], "has been killed...");
            pInf->J.pInf.ModeX &= ~2;
        }
        if (pInf->Notify&SGNET_HASRESPAWN)
        {
            pInf->J.OVI->state |= V3XSTATE_HIDDEN; // il avait disparu
            NG_FXNew(&pInf->Rot->pos, FX_WARPIN, 1, &pInf->J.pInf, 2, pInf->J.OVI);
            pInf->J.OVI->state &= ~V3XSTATE_HIDDEN;
            pInf->J.pInf.AfterDeath = 0;
            pInf->J.pInf.Shield = 100;
            // Ressussite un player
        }
        if (pInf->Notify&SGNET_HASWON)
        {
            g_SGObjects.FinCode=GAMESTATE_WON;
            g_SGSettings.ComNumber = COM_Chat;
            g_SGSettings.ComTime = MAX_COM_DELAY;
            g_SGSettings.ComPlayer = 1+playerId;
            sysStrCpy(g_szCOM[COM_Chat], "has won...");
        }
        if (pInf->Notify&SGNET_SHIELDCHANGE)
        {
            pInf->J.pInf.Shield = *(int32_t*)buffer;
            if (pInf->J.pInf.Shield<=0)  sysStrCpy(g_szCOM[COM_Chat], "has been killed...");
            buffer+=sizeof(int32_t);
        }
        if (pInf->Notify&SGNET_COMMUNICATE)
        {
            g_SGSettings.ComNumber = buffer[0]; buffer++;
            g_SGSettings.ComTime = MAX_COM_DELAY;
            g_SGSettings.ComPlayer = 1+playerId;
            NG_AudioBeep(1);
        }
        if (pInf->Notify&SGNET_CHAT)
        {
            int ln=buffer[0];
            buffer++;
            if (ln)
            {
                g_SGSettings.ComNumber = COM_Chat;
                g_SGSettings.ComTime = MAX_COM_DELAY;
                g_SGSettings.ComPlayer = 1+playerId;
                sysMemZero(g_szCOM[COM_Chat], 32);
                sysStrnCpy(g_szCOM[COM_Chat], (char*)buffer, ln);
                buffer+=ln;
                NG_AudioBeep(2);
            }
        }
    }
    if (pInf->Notify&SGNET_SHIP)
    {
        buffer=NG_NetTranslateShip(buffer);
    }
    #endif
    return buffer - be;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void NG_NetDispatch(void)
*
* DESCRIPTION :
*
*/
void NG_NetDispatch(void)
{
#ifndef RLX_IOREADONLY
    unsigned pid, i;
    sysMemZero(g_szNetPacket, 2048);
    for (i=0;i<(unsigned)g_SGGame.numPlayer;i++)
    {
        SGPlayer *pInf = g_SGGame.pPlayer + i;
        pInf->dispatched = 0;
        pInf->J.pInf.ModeX &= ~2;
    }

    while((pid=sNET->ReceiveData(NET_EVERYBODY, g_szNetPacket, 2048))!=0)
    {
        if (pid!=0xff) //System message
        {
            if (g_szNetPacket[0]=='$') // Id Dispatcher
            {
                NG_NetDispatchPlayer(g_szNetPacket[1], (uint8_t*)(g_szNetPacket+2));
            }
            if (g_szNetPacket[0]=='&') // Id Resynchronise
            {
            }
        }
        else
        {
            g_SGGame.IsHost = lpNet->IsHost; // Check hosten
            // Check for lost connection
        }
    }
    /*
    for (i=0;i<g_SGGame.numPlayer;i++)
    {
        SGPlayer *pInf = g_SGGame.pPlayer + i;
        if (!pInf->dispatched)
        {
            pInf->NetRetry--;
            if (pInf->NetRetry<=0)
            {
                pInf->J.pInf.AfterDeath = 4;
                pInf->J.pInf.Shield = 0;
                g_SGSettings.ComNumber = COM_Chat;
                g_SGSettings.ComTime = MAX_COM_DELAY;
                g_SGSettings.ComPlayer = 1+i;
                sysStrCpy(g_szCOM[COM_Chat], "no more on the net...");
            }
        }
    }
    */
    #endif
}

