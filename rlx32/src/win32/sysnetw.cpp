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

#define MAXRECVBUFFER 4096
#define MAXSENDBUFFER 4096
// LZ Packet compression
//#define LZ
// Asynchronous com
//#define ASYNC

#define IDIRECTPLAY4_OR_GREATER
#include <stdio.h>
#include <windows.h>
#include <cguid.h>
#include <dplay.h>
#include <zlib.h>
#include "_rlx32.h"
#include "systools.h"
#include "sysnetw.h"
#include "win32/w32_dx.h"

static LPDIRECTPLAY4A        lpDirectPlay4;
static LPNET_Informations    lpDPInfo;
static GUID g_cGUID;
static HWND g_hWnd;

BOOL FAR PASCAL EnumConnectionsCallback(LPCGUID lpguidSP, LPVOID lpConnection, DWORD dwConnectionSize, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
    LPVOID  lpConnectionBuffer;
    UNUSED(lpguidSP);
    UNUSED(dwFlags);
    UNUSED(lpContext);
    // make space for connection shortcut
    lpConnectionBuffer = MM_std.malloc(dwConnectionSize);
    if (lpConnectionBuffer)
    {     
        NET_ConnectionType *cn = lpDPInfo->lpConnectionType + lpDPInfo->numConnections;
        sysMemCpy(lpConnectionBuffer, lpConnection, dwConnectionSize);
        sysStrnCpy(cn->name, (char *)lpName->lpszShortNameA, NET_MAXPLAYERLEN);
        cn->lpConnection = lpConnectionBuffer;
        cn->ID = lpDPInfo->numConnections;
        lpDPInfo->numConnections++;
    }
    return TRUE;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NET_ReleaseSessionList(void)
*
* Description :  
*
*/
void DPN_ReleaseSessionList(void)
{
    int i;
    if (!lpDPInfo->numSessions) return;
    for (i=0;i<lpDPInfo->numSessions;i++)
    {
        NET_SessionName *sn = lpDPInfo->lpSessionName + i;
        if (sn->lpSession)
        {
            DPSESSIONDESC2 *p = (DPSESSIONDESC2 *)lpDPInfo->lpSessionName[i].lpSession;
            if (p->lpszPasswordA)    MM_std.free(p->lpszPasswordA);
            if (p->lpszSessionNameA) MM_std.free(p->lpszSessionNameA);
            MM_std.free(p);
        }
    }
    sysMemZero(lpDPInfo->lpSessionName, sizeof(NET_SessionName)*lpDPInfo->numSessions);
    lpDPInfo->numSessions = 0;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void sNET->ReleaseConnectionList(void)
*
* Description :  
*
*/
void DPN_ReleaseConnectionList(void)
{
    int i;
	if (!lpDPInfo)
		return;

    if (lpDPInfo->numConnections)
    {
        for (i=0;i<lpDPInfo->numConnections;i++)
        {
            if (lpDPInfo->lpConnectionType[i].lpConnection) MM_std.free(lpDPInfo->lpConnectionType[i].lpConnection);
        }
        sysMemZero(lpDPInfo->lpConnectionType, lpDPInfo->numConnections*sizeof(NET_ConnectionType));
    }
    DPN_ReleaseSessionList();
    lpDPInfo->numConnections = 0;
    if (lpDirectPlay4)  lpDirectPlay4->Release();
    lpDirectPlay4 = NULL;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  NET_ConnectionType *sNET->CreateConnectionList(void)
*
* DESCRIPTION :
*
*/
NET_ConnectionType *DPN_CreateConnectionList(void)
{
    HRESULT   hr;    
    // Initialize COM library    
    if (lpDirectPlay4) 
		DPN_ReleaseConnectionList();
    
	// Create an IDirectPlay4 interface
    hr = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay4A, (LPVOID*)&lpDirectPlay4);
    if (SYS_DXTRACE(hr)) 
		return NULL;

    // Enumerate Connections
    lpDPInfo->numConnections = 0;
    hr = lpDirectPlay4->EnumConnections(&g_cGUID, EnumConnectionsCallback, lpDPInfo, 0);
    if (SYS_DXTRACE(hr)) 
		return NULL;

    if (lpDPInfo->numConnections==0) 
		return NULL;
    lpDPInfo->lpConnectionType[lpDPInfo->numConnections].name[0] = 0;
    lpDPInfo->lpConnectionType[lpDPInfo->numConnections].ID = lpDPInfo->numConnections;
    lpDPInfo->numConnections++;
    return lpDPInfo->lpConnectionType;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int sNET->SelectConnection(int id)
*
* DESCRIPTION :  Get pointer to the selected connection
*
*/
int DPN_SelectConnection(int id)
{
    LPVOID lpConnection;
    lpConnection = lpDPInfo->lpConnectionType[id].lpConnection; 
    if (lpConnection)
    {
        DPCAPS  dpCaps;
        HRESULT hr = lpDirectPlay4->InitializeConnection(lpConnection, 0);
        if (SYS_DXTRACE(hr)) return -1;
        sysMemZero(&dpCaps, sizeof(DPCAPS));
        dpCaps.dwSize = sizeof(DPCAPS);
        hr = lpDirectPlay4->GetCaps(&dpCaps, DPGETCAPS_GUARANTEED);
        if (SYS_DXTRACE(hr)) return -3;
        return 0;
    }
    return -2;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int NET_CloseConnection(void)
*
* DESCRIPTION :
*
*/
int DPN_CloseConnection(void)
{
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  BOOL FAR PASCAL EnumSessionsCallback(LPCDPSESSIONDESC2 lpSessionDesc, LPDWORD lpdwTimeOut, DWORD dwFlags, LPVOID lpContext)
*
* DESCRIPTION :
*
*/
BOOL FAR PASCAL EnumSessionsCallback(LPCDPSESSIONDESC2 lpSessionDesc, LPDWORD lpdwTimeOut, DWORD dwFlags, LPVOID lpContext)
{
    DPSESSIONDESC2 *p;
    NET_SessionName *sess = lpDPInfo->lpSessionName + lpDPInfo->numSessions;
    // see if last session has been enumerated      
    if (dwFlags & DPESC_TIMEDOUT)  return FALSE;

    // store pointer to guid in list    
    sysStrnCpy(sess->name, lpSessionDesc->lpszSessionNameA, 64);    
    sess->ID = lpDPInfo->numSessions;
    
    //
    p = (DPSESSIONDESC2*) MM_std.malloc(sizeof(DPSESSIONDESC2));
    sysMemCpy(p, lpSessionDesc, sizeof(DPSESSIONDESC2));
    sess->lpSession = p;
    sess->flags = 0;
	sess->nbPlayers = p->dwCurrentPlayers;
	sess->maxPlayers = p->dwMaxPlayers;	
	
 
    if (p->dwFlags&DPSESSION_JOINDISABLED)
    {
        sess->flags|=NET_SESSIONISCLOSED;
    }
    if (p->dwFlags&DPSESSION_NEWPLAYERSDISABLED)
    {
        sess->flags|=NET_SESSIONISCLOSED;
    }
    if (lpSessionDesc->lpszSessionNameA)
    {
        p->lpszSessionNameA = (char*)MM_std.malloc(strlen(lpSessionDesc->lpszSessionNameA)+1);
        sysStrCpy(p->lpszSessionNameA, lpSessionDesc->lpszSessionNameA);        
    }
    if (lpSessionDesc->lpszPasswordA)
    {
        p->lpszPasswordA = (char*)MM_std.malloc(strlen(lpSessionDesc->lpszPasswordA)+1);
        sysStrCpy(p->lpszPasswordA, lpSessionDesc->lpszPasswordA);
    }	

    lpDPInfo->numSessions++;
    UNUSED(lpdwTimeOut);
    UNUSED(lpContext);
    return (TRUE);
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  NET_SessionName *sNET->CreateSessionList(unsigned flag)
*
* DESCRIPTION :
*
*/
NET_SessionName *DPN_CreateSessionList(unsigned flag)
{
    DPSESSIONDESC2   sessionDesc;
    HRESULT          hr;
 
	// free older session list
    sNET->ReleaseSessionList();
    
	 // Enum session
    sysMemZero(&sessionDesc, sizeof(DPSESSIONDESC2));
    sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
    sessionDesc.guidApplication = g_cGUID;
    
	hr = SYS_DXTRACE(lpDirectPlay4->EnumSessions(&sessionDesc, 0, EnumSessionsCallback, g_hWnd, 
          DPENUMSESSIONS_ALL|DPENUMSESSIONS_ASYNC|DPENUMSESSIONS_PASSWORDREQUIRED));
    
	if (lpDPInfo->numSessions==0) 
		return NULL;
    
	// select Session
    lpDPInfo->lpSessionName[lpDPInfo->numSessions].name[0] = 0;
    lpDPInfo->lpSessionName[lpDPInfo->numSessions].ID = lpDPInfo->numSessions;
    lpDPInfo->lpSessionName[lpDPInfo->numSessions++].lpSession = NULL;
    UNUSED(flag);
    return lpDPInfo->lpSessionName;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : unsigned sNET->QuitSession(unsigned flag)
*
* DESCRIPTION : player quit session
*
*/
unsigned DPN_QuitSession(void)
{
    HRESULT         hr;
    if (lpDirectPlay4 == NULL) return -1;
     if (lpDPInfo->dpidPlayer)
    {
        hr = lpDirectPlay4->DestroyPlayer(lpDPInfo->dpidPlayer);
        if (SYS_DXTRACE(hr)) return -1;
    }
    hr = lpDirectPlay4->Close();
    if (SYS_DXTRACE(hr)) return -1;
    lpDPInfo->dpidPlayer = 0;
    lpDPInfo->IsHost = FALSE;
    lpDPInfo->IsClient = FALSE;
	lpDPInfo->wSessionName[0] = 0;
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  unsigned sNET->CloseSession(int numero)
*
* DESCRIPTION :
*
*/
unsigned DPN_CloseSession(void)
{
    HRESULT         hr;
    DPSESSIONDESC2 *lpSession;
    if (lpDirectPlay4 == NULL) return -1;
    // join an existing session
    sNET->CreateSessionList(0);
    sNET->CreatePlayerList(0);
    hr = lpDirectPlay4->DestroyPlayer((DPID)lpDPInfo->dpidPlayer);
    if (SYS_DXTRACE(hr)) 
		return -1;
    lpSession = (DPSESSIONDESC2*)(lpDPInfo->lpSessionName[lpDPInfo->currentSession].lpSession);
    if (!lpSession) 
		return -1;
    hr = lpDirectPlay4->Close();
    if (SYS_DXTRACE(hr)) 
		return -1;
    lpDPInfo->dpidPlayer = 0;
    lpDPInfo->IsHost = FALSE;
    lpDPInfo->currentSession = 0;
    lpDPInfo->IsClient = FALSE;	
    lpDPInfo->wSessionName[0] = 0;
    sNET->CreateSessionList(0);
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  unsigned sNET->CreateNewSession(char *SessionName, unsigned MaxGamers)
*
* DESCRIPTION :
*
*/
int DPN_CreateNewSession(char *SessionName, unsigned MaxGamers, char *password)
{
    DPID           dpidPlayer;
    DPNAME         dpName;
    DPSESSIONDESC2 sessionDesc;
    HRESULT        hr;
    
	// should have an interface by now
    if (!lpDirectPlay4) return -1;
 
	// Open Session
    lpDPInfo->bytesSent = 0;
    lpDPInfo->bytesRecv = 0;
    lpDPInfo->packetsPending = 0;
    lpDPInfo->dpidPlayer = NULL;
    lpDPInfo->IsHost = FALSE;

    // Re-entrant protection
    if (lpDPInfo->IsHost) sNET->CloseSession();
    if (lpDPInfo->IsClient) sNET->QuitSession();

    // Create session
    sysStrCpy(lpDPInfo->wSessionName, SessionName ? SessionName : lpDPInfo->wPlayerName);
    sysMemZero(&sessionDesc, sizeof(DPSESSIONDESC2));
    sessionDesc.dwSize = sizeof(DPSESSIONDESC2);
    sessionDesc.dwFlags = DPSESSION_MIGRATEHOST | DPSESSION_KEEPALIVE;
    sessionDesc.guidApplication = g_cGUID;
    sessionDesc.dwMaxPlayers = MaxGamers;
    sessionDesc.lpszSessionNameA = lpDPInfo->wSessionName;
    if (!password)
    {
        hr = SYS_DXTRACE(lpDirectPlay4->Open(&sessionDesc, DPOPEN_CREATE));
    }
    else
    {     
	    DPCREDENTIALS lpCredentials;        
        lpCredentials.dwSize = sizeof(DPCREDENTIALS);
        lpCredentials.lpszPasswordA = password;
        lpCredentials.lpszUsernameA = lpDPInfo->wPlayerName;
        lpCredentials.lpszDomainA = lpDPInfo->domain;
        hr = lpDirectPlay4->SecureOpen(&sessionDesc, DPOPEN_CREATE, NULL, &lpCredentials);        
	}
    if (hr!=DPERR_ALREADYINITIALIZED)
    {
        if (hr) 
			return hr;
    }

    // Create g_pPlayer
    sysMemZero(&dpName, sizeof(DPNAME));
    dpName.dwSize = sizeof(DPNAME);
    dpName.lpszShortNameA = lpDPInfo->wPlayerName;
    hr = lpDirectPlay4->CreatePlayer(&dpidPlayer, &dpName, NULL, NULL, 0, 0);
    if (SYS_DXTRACE(hr)) 
		return -2;    
    lpDPInfo->dpidPlayer = dpidPlayer;
    lpDPInfo->IsHost  = TRUE;
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  unsigned sNET->JoinSession(unsigned numero)
*
* DESCRIPTION :  join the nth session. Password is optional (NULL for no secure session)
*
*/
int DPN_JoinSession(unsigned numero, char *password)
{
    DPID            dpidPlayer = 0;
    DPNAME          dpName;
    HRESULT         hr;
    DPSESSIONDESC2 *lpSession;
    if (lpDirectPlay4 == NULL) return -1;
    
	// Clean    
    if (lpDPInfo->IsHost)   sNET->CloseSession();
    if (lpDPInfo->IsClient) sNET->QuitSession();

    // Get session list
    lpDPInfo->dpidPlayer = NULL;
    lpDPInfo->IsClient = FALSE;
    sNET->CreateSessionList(0);

    // Join session
    lpDPInfo->currentSession = (u_int8_t)numero;
    lpSession = (DPSESSIONDESC2*)(lpDPInfo->lpSessionName[lpDPInfo->currentSession].lpSession);	
    if (!lpSession) return -1;        
	lpDPInfo->maxPlayers = (u_int8_t)lpSession->dwMaxPlayers;
    if (!password)
    {
        hr = lpDirectPlay4->Open(lpSession, DPOPEN_JOIN);
    }
    else
    {     
	    DPCREDENTIALS lpCredentials;        
        sysMemZero(&lpCredentials, sizeof(DPCREDENTIALS));
        lpCredentials.dwSize = sizeof(DPCREDENTIALS);
        lpCredentials.lpszPasswordA = password;
        lpCredentials.lpszUsernameA = lpDPInfo->wPlayerName;        
        lpCredentials.lpszDomainA = lpDPInfo->domain;
        hr = lpDirectPlay4->SecureOpen(lpSession, DPOPEN_JOIN, NULL, NULL);
        if (hr == DPERR_LOGONDENIED)
        {
            hr = lpDirectPlay4->SecureOpen(lpSession, DPOPEN_JOIN, NULL, &lpCredentials);
        }
	}    
    if (SYS_DXTRACE(hr)) 
		return -1;
    sysStrCpy(lpDPInfo->wSessionName, lpSession->lpszSessionNameA);

    // Create player
    sysMemZero(&dpName, sizeof(DPNAME));
    dpName.dwSize = sizeof(DPNAME);
    dpName.lpszShortNameA = lpDPInfo->wPlayerName;    
    hr = lpDirectPlay4->CreatePlayer(&dpidPlayer, &dpName, NULL, NULL, 0, 0);
    if (SYS_DXTRACE(hr)) 
		return -1;        
    lpDPInfo->dpidPlayer = dpidPlayer;
    lpDPInfo->IsClient = TRUE;    
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  BOOL FAR PASCAL EnumPlayersCallback(DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
*
* DESCRIPTION :
*
*/
BOOL FAR PASCAL EnumPlayersCallback(DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
    sysStrCpy(lpDPInfo->lpPlayerName[lpDPInfo->numPlayers].name, lpName->lpszShortNameA);
    lpDPInfo->lpPlayerName[lpDPInfo->numPlayers].ID = dpId;        
    if (strcmp(lpName->lpszShortNameA, lpDPInfo->wPlayerName)==0)    lpDPInfo->index = lpDPInfo->numPlayers;
    lpDPInfo->numPlayers++;
    UNUSED(dwPlayerType);
    UNUSED(dwFlags     );
    UNUSED(lpContext   );
    return(TRUE);
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  NET_PlayerName *sNET->CreatePlayerList(unsigned flag)
*
* DESCRIPTION :
*
*/
NET_PlayerName *DPN_CreatePlayerList(unsigned flag)
{
    HRESULT  hr;
    // MM_std.free older player list
    lpDPInfo->numPlayers = 0;
    
    // Enum players
    if ((!lpDPInfo->IsClient)&&(!lpDPInfo->IsHost)) return NULL;
    hr = lpDirectPlay4->EnumPlayers(NULL, EnumPlayersCallback, lpDPInfo, 0 );
    hr = SYS_DXTRACE(hr);
    lpDPInfo->lpPlayerName[lpDPInfo->numPlayers  ].name[0] = 0;
    lpDPInfo->lpPlayerName[lpDPInfo->numPlayers++].ID = lpDPInfo->numPlayers;
    UNUSED(flag);
    return lpDPInfo->lpPlayerName;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int sNET->SetPlayerName(char *name)
*
* DESCRIPTION :
*
*/
int DPN_SetPlayerName(char *name)
{
    if (name) sysStrnCpy(lpDPInfo->wPlayerName, name, 64);
    if (lpDPInfo->dpidPlayer)
    {
        DPNAME name2;
        sysMemZero(&name2, sizeof(DPNAME));
        name2.dwSize = sizeof(DPNAME);
        name2.lpszShortNameA = name2.lpszLongNameA = name;
        lpDirectPlay4->SetPlayerName(lpDPInfo->dpidPlayer, &name2, DPSET_GUARANTEED);
    }
    return TRUE;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int NET_SendData(unsigned dest, void *data, unsigned size)
*
* DESCRIPTION :
*
*/
int DPN_SendData(unsigned dest, void *data, unsigned size)
{
    DPID idTo;
	HRESULT hr;
    switch(dest) {
        case NET_EVERYBODY: idTo = DPID_ALLPLAYERS;     break;
        case NET_HOST     : idTo = lpDPInfo->dpidHost;  break;
        default:            idTo = lpDPInfo->lpPlayerName[dest].ID; break;
    } 
    // compress packet
#ifdef LZ
    if ( compress( lpDPInfo->lpzBuffer, (u_int32_t*)&size, (u_int8_t*)data, size) == Z_OK )
    data = lpDPInfo->lpzBuffer;  
    else return 0;
#endif
    lpDPInfo->bytesSent +=size;

    hr = lpDirectPlay4->SendEx(
    lpDPInfo->dpidPlayer, 
        idTo, 
        #ifdef ASYNC
             DPSEND_ASYNC | // Async mode
        #endif
        DPSEND_GUARANTEED, 
        data, 
        size, 
        0, 
        0, 
        NULL, 
        NULL
    );
    if (hr == DPERR_PENDING)
    {
       lpDPInfo->packetsPending++;
       return TRUE;
    }
    if (hr!=DP_OK) return FALSE;
    return TRUE;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  unsigned NET_ReceiveData(unsigned from, void *xdata, unsigned xmaxbuf)
*
* DESCRIPTION :
*
*/
unsigned DPN_ReceiveData(unsigned from, void *xdata, unsigned xmaxbuf)
{
    DPID       idFrom=0, idTo;
    DWORD      maxbuf=0;
    HRESULT    hr;
    void      *data = NULL;
    int        retMode = 0;
    switch(from) {
        case NET_EVERYBODY : idFrom = 0;                  break;
        case NET_HOST      : idFrom = lpDPInfo->dpidHost; break;
        default            : idFrom = lpDPInfo->lpPlayerName[from].ID; break;
    }
    if ((!lpDPInfo->IsClient)&&(!lpDPInfo->IsHost)) return 0;
    do
    {
        idTo = lpDPInfo->dpidPlayer;
        hr = lpDirectPlay4->Receive(
               &idFrom, 
                &idTo, 
                DPRECEIVE_ALL, 
                data, 
                &maxbuf);
        if (hr == DPERR_BUFFERTOOSMALL) // Calcul de la taille max
        {
            if (data == NULL)
            {
                 if (maxbuf<MAXRECVBUFFER)
                 data = lpDPInfo->lpRecBuffer;
                 else
                 data = MM_std.malloc(maxbuf);
            }
            else
             data = MM_std.realloc(data, maxbuf);
        }
    }while (hr == DPERR_BUFFERTOOSMALL);
    // Messages systemes
    if ((data)&&(maxbuf))
    {
        if (idFrom==DPID_SYSMSG)
        {
            LPDPMSG_GENERIC lpMsg = (LPDPMSG_GENERIC)data;
            switch(lpMsg->dwType){
                case DPSYS_CREATEPLAYERORGROUP:
                // New incoming player
                // sNET->CreatePlayerList(0);
                break;
                case DPSYS_DESTROYPLAYERORGROUP:
                // A player was killed
                // sNET->CreatePlayerList(0);
                break;
                case DPSYS_SESSIONLOST:
                // Session lost
                break;
                case DPSYS_HOST:
                if (!lpDPInfo->IsHost)
                {
                    lpDPInfo->IsHost = 1;
                    lpDPInfo->IsClient = 0;
                    lpDPInfo->dpidHost = lpDPInfo->dpidPlayer;
                }
                break;
                case DPSYS_SENDCOMPLETE:
                lpDPInfo->packetsPending--;
                break;
                case DPSYS_SETSESSIONDESC:
                // session desc has changed ;
                break;
               
            }
            idFrom = 0;
        }
        else
        {
            // Buffer big enough
            if (maxbuf < xmaxbuf)
            {             
                #ifdef LZ
                    if ( uncompress((u_int8_t*)xdata, &maxbuf, (u_int8_t*)data, maxbuf) != Z_OK )
                        idFrom = 0; // bad packet
                #else
                    sysMemCpy(xdata, data, maxbuf);
                #endif
                lpDPInfo->bytesRecv +=maxbuf;
            }
            else idFrom = 0;
        }
    }
    if ((data) && (data!=lpDPInfo->lpRecBuffer))  MM_std.free(data);
    return idFrom;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int NET_PIDtoIndex(unsigned Pid)
*
* DESCRIPTION :
*
*/
int DPN_PIDtoIndex(unsigned Pid)
{
    int i;
    sNET->CreatePlayerList(0);
    for (i=0;i<lpDPInfo->numPlayers;i++)
    {
        if (lpDPInfo->lpPlayerName[i].ID == Pid) return i;
    }
    return -1;
}
int DPN_SIDtoIndex(char *seName)
{
    int i;
    for (i=0;i<lpDPInfo->numSessions;i++)
    {
        NET_SessionName *sn = lpDPInfo->lpSessionName + i;        
        DPSESSIONDESC2 *p = (DPSESSIONDESC2*)sn->lpSession;
        if( p)
        {
            if (strcmp(p->lpszSessionNameA, seName)==0) return i;
        }
    }
    return -1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  unsigned sNET->Synchronise(void)
*
* DESCRIPTION :
*
*/
#define VER "0x300"
static char str_sender[32]="$host::Query%";
static char str_recv[32] ="$client::Ok~%";

unsigned DPN_Synchronise(int id, int (*callback)(int numConnected))
{
    char tx[256];        
    int ok = 0;    
    {
        int i;
        for (i=0;i<lpDPInfo->numPlayers;i++) lpDPInfo->lpPlayerName[i].connected = 0;
    }
    if (lpDPInfo->IsHost)
    {        
        *(unsigned*)(str_sender+16) = lpDPInfo->dpidPlayer;        
        sNET->SendData(NET_EVERYBODY, str_sender, 32);
        do
        {  
            int i, c;
            unsigned pid;
            if (callback)  if (!callback(1)) return 0;
            pid = sNET->ReceiveData(NET_EVERYBODY, tx, 256);
            if (pid) 
            {
                int cm = sNET->PIDtoIndex(pid);
                if (cm>=0)
                {                    
                    if (strcmp(tx, str_recv)==0) lpDPInfo->lpPlayerName[ cm ].connected = 1;
                }
            }            
            for (c=1, i=0;i<lpDPInfo->numPlayers;i++) c+=lpDPInfo->lpPlayerName[i].connected;
            if (c==lpDPInfo->numPlayers-1) ok = 1;
        }while (ok!=1);
        return 1;
    }
    else
    {
        while(1)
        {
            unsigned pid;
            if (callback)  
				if (!callback(2)) 
					return 0;
            pid = sNET->ReceiveData(NET_EVERYBODY, tx, 256);
            if (pid)
            {
                tx[255]=0;
                if (strcmp(tx, str_sender)==0)
                {                    
                    lpDPInfo->dpidHost = *(unsigned*)(tx+16);
                    *(unsigned*)(str_recv+16) = lpDPInfo->dpidPlayer;
                    sNET->SendData(NET_EVERYBODY, str_recv, 32); 
                    return 1;
                }
            }
        }
    }
}

#define nalloc(size, type) (type*)MM_std.malloc(size*sizeof(type)); 
static LPNET_Informations DPN_Initialize(int maxPlayers, unsigned port)
{
	char tex[256];
    ULONG len;
	lpDPInfo = (LPNET_Informations) nalloc(1, NET_Informations);      
	sysMemZero(lpDPInfo, sizeof(NET_Informations));
    lpDPInfo->lpConnectionType = nalloc(8, NET_ConnectionType);      
	sysMemZero(lpDPInfo->lpConnectionType,  8*sizeof(NET_ConnectionType));
    lpDPInfo->lpSessionName = nalloc(32, NET_SessionName);       
	sysMemZero(lpDPInfo->lpSessionName, 32*sizeof(NET_SessionName));
    lpDPInfo->lpPlayerName = nalloc(maxPlayers, NET_PlayerName); 
	sysMemZero(lpDPInfo->lpPlayerName, maxPlayers * sizeof(NET_PlayerName));
    lpDPInfo->maxPlayers  = maxPlayers;
    lpDPInfo->lpBuffer = (u_int8_t*)MM_std.malloc(MAXSENDBUFFER);
    lpDPInfo->lpRecBuffer = (u_int8_t*)MM_std.malloc(MAXRECVBUFFER);
    lpDPInfo->lpzBuffer = (u_int8_t*)MM_std.malloc(MAXRECVBUFFER);
    SYS_DXTRACE(CoInitialize(NULL)); 
	len = 64;
	sysStrCpy(tex, "New user");
	GetComputerName(tex, &len);
	sNET->SetPlayerName(tex);
	sprintf(lpDPInfo->wSessionName, "%s'game", tex);	
    return lpDPInfo;
    
}
int DPN_Release(LPNET_Informations obj)
{
    if (!obj) 
		obj = lpDPInfo;	
	if (!obj) 
		return 0;
	sNET->ReleaseConnectionList();
	CoUninitialize();	
    MM_std.free(obj->lpConnectionType);
    MM_std.free(obj->lpSessionName);
    MM_std.free(obj->lpPlayerName);
    MM_std.free(obj->lpBuffer);
    MM_std.free(obj->lpRecBuffer);    
    MM_std.free(obj->lpzBuffer);
	sysMemZero(obj, sizeof(NET_Informations));
    MM_std.free(obj);
	lpDPInfo = NULL;    
    return 1;
}

NET_ClientDriver net_dplay = {
	DPN_Initialize, 
    DPN_Release, 
    DPN_CreateConnectionList, 
    DPN_SelectConnection, 
    DPN_CloseConnection, 
    DPN_CreateSessionList, 
    DPN_ReleaseConnectionList, 
    DPN_JoinSession, 
    DPN_CreateNewSession, 
    DPN_QuitSession, 
    DPN_CloseSession, 
    DPN_SetPlayerName, 
    DPN_CreatePlayerList, 
    DPN_SendData, 
    DPN_ReceiveData, 
    DPN_PIDtoIndex, 
    DPN_SIDtoIndex, 
    DPN_Synchronise, 
	DPN_ReleaseSessionList	
};

NET_ClientDriver *NET_GetDirectPlayInterface(void *hwnd, const void *cGuid)
{
	g_cGUID = *(const GUID*)cGuid;
	g_hWnd = (HWND)hwnd;
    return &net_dplay;    
}

