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
#ifndef __SYSNET_H
#define __SYSNET_H

#define NET_MAXPLAYERLEN 64
// Message type
typedef unsigned NET_PID;
enum {
      NET_DATA =0x1,
      NET_SAFEDATA,
      NET_CHAT
};
#define NET_PutBuffer(dta, size) {*(size*)lpNet->data = (size)(dta); lpNet->data+=sizeof(size);}
#define NET_GetBuffer(dta, size) {dta = *(size*)lpNet->data; lpNet->data+=sizeof(size);}

#define NET_PutString(dta) { uint8_t _l = (uint8_t)strlen(dta); NET_PutBuffer(_l, uint8_t); sysMemCpy(lpNet->data, dta, _l); lpNet->data+=_l; }
#define NET_GetString(dta) { uint8_t _l; NET_GetBuffer(_l, uint8_t); sysMemCpy(dta, lpNet->data, _l); lpNet->data+=_l; }

// Message mode
enum {
      NET_EVERYBODY=0xff,
      NET_HOST =0xffff
};

typedef struct {
     char     name[NET_MAXPLAYERLEN];
     NET_PID ID;
     void    *lpConnection;
}NET_ConnectionType;

enum {
       NET_SESSIONISCLOSED = 0x1
};
typedef struct _net_sessionname{
     char       name[NET_MAXPLAYERLEN];
     NET_PID    ID;
     unsigned   flags, nbPlayers, maxPlayers;
     unsigned   sID;
     void       *lpSession;
}NET_SessionName;

typedef struct _net_playername{
     char    name[NET_MAXPLAYERLEN];
     NET_PID ID;
     unsigned connected;
}NET_PlayerName;

typedef struct _net_informations{
    char               wPlayerName[NET_MAXPLAYERLEN];
    char               wSessionName[NET_MAXPLAYERLEN];
    char               domain[16];
    uint8_t              *data, *lpBuffer, *lpzBuffer, *lpRecBuffer;
    NET_SessionName    *lpSessionName;
    NET_PlayerName     *lpPlayerName;
    NET_ConnectionType *lpConnectionType;
    NET_PID            dpidHost;             // ID du host
    NET_PID            dpidPlayer;           // ID of player created
    unsigned           sID;
    unsigned           index;                // Index du player
    int                numConnections, numSessions, numPlayers, packetsPending;
    unsigned           bytesSent, bytesRecv, snapShotTime;
    unsigned char      IsHost, IsClient;      // User type : host or client 
    unsigned char      currentSession, maxPlayers;   // TRUE if we are hosting the session
} NET_Informations, *LPNET_Informations;

typedef struct _sysnet_driver {
    LPNET_Informations   (*Initialize)(int maxPlayers, unsigned port);
    int                  (*Release)(LPNET_Informations obj);
    NET_ConnectionType   *(*CreateConnectionList)(void);
    int                  (*SelectConnection)(int id);
    int                  (*CloseConnection)(void);
    NET_SessionName      *(*CreateSessionList)(unsigned flag);
    void                 (*ReleaseConnectionList)(void);
    int                  (*JoinSession)(unsigned numero, char *passWord);
    int                  (*CreateNewSession)(char *SessionName, unsigned MaxGamers, char *passWord);
    unsigned             (*QuitSession)(void);
    unsigned             (*CloseSession)(void);
    int                  (*SetPlayerName)(char *name);
    NET_PlayerName       *(*CreatePlayerList)(unsigned flag);
    int                  (*SendData)(NET_PID dest, void *data, unsigned size);
    NET_PID              (*ReceiveData)(unsigned from, void *data, unsigned wmaxbuf);
    int                  (*PIDtoIndex)(NET_PID Pid);
    int                  (*SIDtoIndex)(char *name);
    unsigned             (*Synchronise)(int id, int (*callback)(int mode));
	void			     (*ReleaseSessionList)(void);
}NET_ClientDriver;

__extern_c
	_RLXEXPORTFUNC NET_ClientDriver RLXAPI *NET_GetDirectPlayInterface(void *, const void *);
	extern NET_ClientDriver *sNET;

__end_extern_c


#endif

