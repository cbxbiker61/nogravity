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
#include <windows.h>
#include <math.h>
#include <dsound.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "systools.h"
#include "sysresmx.h"

#include "iss_defs.h"
#include "iss_fx.h"
#include "iss_av.h"
#include "snd_ds6.h"
#include "win32/w32_dx.h"

#define DS3D_DEFMODE DS3D_DEFERRED

typedef struct 
{
    int32_t       m_nWritePosition, m_nPreviousPosition;
    int32_t       m_nWriteBytes, m_nTotalBytes, m_nUploadedBytes;
    u_int16_t     nState, m_nStreamState;
	float	   m_nPlayVolume;
    short      channel;
    V3XA_HANDLE sample;
}DS_stream;

LPDIRECTSOUND           g_lpDSDevice;                      //  Objet principal direct sound
LPDIRECTSOUNDBUFFER     g_lpPrimaryBuffer;
LPDIRECTSOUND3DLISTENER g_lpDS3DListener;
DS_handle               g_pDSHandles[DSOUNDMAXBUFFER];     //  buffers sons pour stocker les samples
DS_handle               DS_SecBuffer;

static WAVEFORMATEX	g_cDSPCMOutFormat;
static DSBUFFERDESC	DS_BufferDesc;
static DS_stream	g_pDSStreams[MAX_STREAM];

static DS3DBUFFER Buffer3dProps = {
    sizeof(DS3DBUFFER), // size
    0, 0, 0,  		  // Position  Gets updated as things move.
    0, 0, 0,              // Velocity  Fixed for this demo.
    0,     		  // Inner angle
    0,     		  // Outer angle
    1, 0, 0,    	  // Cone orientation
    0,     		  // Cone outer volume
    DS3D_DEFAULTMINDISTANCE, // Min distance
    DS3D_DEFAULTMAXDISTANCE, // Max distance
    DS3DMODE_NORMAL    	     // Mode
};
static DS3DLISTENER Listener3dProps = {
    sizeof(DS3DLISTENER), // Size
    0, 0, 0,    // Position   Gets set to center of screen coordinates
    0, 0, 0,    // Velicity   Fixed for this demo
    0, 0, 1,    // Orientation Front
    0, 1, 0,    // Orientation Top
    DS3D_DEFAULTDISTANCEFACTOR, // Distance Factor
    DS3D_MINROLLOFFFACTOR, // Rolloff factor
    DS3D_DEFAULTDOPPLERFACTOR // Doppler factor
};

typedef struct {
   int       index;
   int       mode;
   GUID FAR *lpGUID;
}DX_Context;

int32_t CALLING_C Poll(int32_t p)
{
    return 0;
}

static u_int32_t DriverGetMemory(void)
{
    DSCAPS caps;
    sysMemZero(&caps, sizeof(DSCAPS));
    caps.dwSize = sizeof(DSCAPS);
    g_lpDSDevice->GetCaps(&caps);
    return caps.dwMaxContigFreeHwMemBytes;
}

static BOOL WINAPI DSEnumCallback(GUID FAR *lpGUID, char const* lpDescription, char const * lpModule, LPVOID lpContext)
{
    DX_Context *cx = (DX_Context*)lpContext;

    if (cx->mode == 1)
    {
		if (cx->index==RLX.Audio.WaveDeviceId)
		{
			sysStrnCpy(V3XA.Client->s_DrvName, lpDescription, 64);
			cx->lpGUID = lpGUID;
		}
    }
    else
    if (cx->mode==3)
    {
		V3XA.p_driverList[cx->index] = (char*)MM_std.malloc(strlen(lpDescription)+1);	   
		sysStrCpy(V3XA.p_driverList[cx->index], lpDescription);
    }
    UNUSED(lpModule);
    cx->index++;
	if (cx->mode==3) V3XA.p_driverList[cx->index] = NULL;
    return TRUE;
}


static int Enum(void)
{
    DX_Context ds;
    ds.index = 0;
    ds.mode = 2;
    ds.lpGUID = NULL;
    DirectSoundEnumerate((LPDSENUMCALLBACKA)DSEnumCallback, &ds);
    if (ds.index)
    {
      V3XA.p_driverList = (char**)MM_std.malloc((ds.index+1)*sizeof(char*));
      ds.lpGUID = NULL;
      ds.index = 0;
      ds.mode = 3;
      DirectSoundEnumerate((LPDSENUMCALLBACKA)DSEnumCallback, &ds);
    }
    return 1;
}

static void Start(void)
{
	HRESULT  hr;
    if (g_lpPrimaryBuffer)
		hr = g_lpPrimaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
    hr = g_lpPrimaryBuffer->SetVolume(0);
    return;
}

static void Stop(void)
{
    if (g_lpPrimaryBuffer)
    {
      ULONG status;
      g_lpPrimaryBuffer->GetStatus(&status);
      if ( (status&DSBSTATUS_PLAYING)==DSBSTATUS_PLAYING )
      g_lpPrimaryBuffer->SetVolume(DSBVOLUME_MIN);
      g_lpPrimaryBuffer->Stop();
    }
    return;
}

static int Detect()
{
	DX_Context ds;
    g_lpDSDevice = NULL;
    sysMemZero(g_pDSHandles, DSOUNDMAXBUFFER * sizeof(DS_handle));
    ds.lpGUID = NULL;

    if (RLX.Audio.WaveDeviceId)
    {
       ds.index = 0;
       ds.mode = 1;
       DirectSoundEnumerate((LPDSENUMCALLBACKA)DSEnumCallback, &ds);
    }
	
    if (SYS_DXTRACE(DirectSoundCreate(ds.lpGUID, &g_lpDSDevice, NULL)))
    {
       if (ds.lpGUID)
       {
			ds.lpGUID = NULL;
			RLX.Audio.WaveDeviceId = 0;
       }
	   if (SYS_DXTRACE(DirectSoundCreate(ds.lpGUID, &g_lpDSDevice, NULL)))
		   return -1;
    }   
    return 0;
}

static BOOL FindAlternateSampleFormat()
{
    const u_int32_t aFormatOrder[]={
        44216, 44116, 44208, 44108, 22216, 22116, 22208, 22108, 
    11216, 11116, 11208, 11108, 8216, 8116, 8208, 8108, 0};

    int nCurFormat = 0;
	HRESULT hr;
    do
    {		
        u_int32_t
        dwFormat = aFormatOrder[nCurFormat++], 
        dwFreq = dwFormat/1000;
        g_cDSPCMOutFormat.nSamplesPerSec = (dwFreq==8)? 8000:(dwFreq/11)*11025;
        g_cDSPCMOutFormat.wBitsPerSample = (u_int16_t)(dwFormat%100);
        g_cDSPCMOutFormat.nChannels = (u_int16_t)((dwFormat%1000)/100);
        g_cDSPCMOutFormat.nBlockAlign = (u_int16_t)(g_cDSPCMOutFormat.nChannels   * (g_cDSPCMOutFormat.wBitsPerSample>>3));
        g_cDSPCMOutFormat.nAvgBytesPerSec = (u_int32_t)g_cDSPCMOutFormat.nBlockAlign * (u_int32_t)g_cDSPCMOutFormat.nSamplesPerSec;
        hr = SYS_DXTRACE(g_lpPrimaryBuffer->SetFormat(&g_cDSPCMOutFormat));
    }while( (hr!=DS_OK) && (nCurFormat!=16));

    if ( (nCurFormat==16) && (hr!=DS_OK) )
    {
        SYS_DXTRACE(g_lpPrimaryBuffer->SetFormat(&g_cDSPCMOutFormat));
        return TRUE;
    }    
	return 0;
}

static int Initialize(void *hwnd)
{
    DSBUFFERDESC primaryDesc;
    if (!g_lpDSDevice) return TRUE;
    sysMemZero(&primaryDesc, sizeof(DSBUFFERDESC));
    primaryDesc.dwSize = sizeof(DSBUFFERDESC);
    primaryDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    if (RLX.Audio.Config & RLXAUDIO_Use3D)
		primaryDesc.dwFlags|= DSBCAPS_CTRL3D;

	if (SYS_DXTRACE(g_lpDSDevice->SetCooperativeLevel((HWND)hwnd, DSSCL_EXCLUSIVE)) != DS_OK) 
		return -1;

	if (SYS_DXTRACE(g_lpDSDevice->CreateSoundBuffer(&primaryDesc, &g_lpPrimaryBuffer, NULL))!=DS_OK )
		return 0;
    else
    {	
		HRESULT hr;
		sysMemZero(&g_cDSPCMOutFormat, sizeof(WAVEFORMATEX));
		g_cDSPCMOutFormat.wFormatTag = WAVE_FORMAT_PCM;
		g_cDSPCMOutFormat.wBitsPerSample = 16;
		g_cDSPCMOutFormat.nChannels = 2;
		g_cDSPCMOutFormat.nSamplesPerSec = 44100;
		g_cDSPCMOutFormat.nBlockAlign = (u_int16_t)(g_cDSPCMOutFormat.nChannels   *  (g_cDSPCMOutFormat.wBitsPerSample>>3));
		g_cDSPCMOutFormat.nAvgBytesPerSec = (u_int32_t)g_cDSPCMOutFormat.nBlockAlign *  (u_int32_t)g_cDSPCMOutFormat.nSamplesPerSec;
		hr = g_lpPrimaryBuffer->SetFormat(&g_cDSPCMOutFormat);
		if ( hr != DS_OK )
			FindAlternateSampleFormat();

        if (RLX.Audio.Config & RLXAUDIO_Use3D)
        {
			hr = SYS_DXTRACE(g_lpPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener, (void**)&g_lpDS3DListener));
            if (hr== DS_OK)
				hr = g_lpDS3DListener->SetAllParameters(&Listener3dProps, DS3D_IMMEDIATE);
        }
    }
    return 0; // no problemo.
}

static void DriverReleaseA(void)
{
	if (g_lpDS3DListener)	    
		g_lpDS3DListener->Release();	

	if (g_lpPrimaryBuffer)	
		g_lpPrimaryBuffer->Release();

}

void ChannelStop(int channel)
{
    LPDIRECTSOUNDBUFFER pDS = g_pDSHandles[channel].pbuffer;
    if (!pDS)  
		return;
    pDS->Stop();
    pDS->SetCurrentPosition(0);
    return;
}

static void DS_Unregister3D(int channel)
{
    DS_handle *hnd = g_pDSHandles + channel;
    if (hnd->pbuffer)   
	{
		hnd->pbuffer->Stop();
		hnd->pbuffer->Release();
	}
    if (hnd->pbuffer3D)
		hnd->pbuffer3D->Release();

    if (hnd->pReverb)   
		hnd->pReverb->Release();

    sysMemZero(hnd, sizeof(DS_handle));
    return;
}

static void ChannelFlushAll(int mode)
{
    int channel;
    for (channel = 0;channel < DSOUNDMAXBUFFER; channel ++)
    {
		DS_handle *p = g_pDSHandles + channel ;
		if (( p->flags & DSH_RESERVED ) == 0)
		{
			ChannelStop(channel);
			DS_Unregister3D(channel);
		}
    }
    return;
}

static void Release(void)
{
    if (!g_lpDSDevice) 
		return;

    if ( g_lpPrimaryBuffer!=NULL )
    {
        ULONG status;
		ChannelFlushAll(0);
        g_lpPrimaryBuffer->GetStatus(&status);

		if ( (status&DSBSTATUS_PLAYING)==DSBSTATUS_PLAYING )
			g_lpPrimaryBuffer->Stop();

		if (g_lpDS3DListener)
			g_lpDS3DListener->Release();

        g_lpPrimaryBuffer->Release();
        g_lpPrimaryBuffer = NULL;
    }
    if (V3XA.p_driverList) 
	{
		array_free(V3XA.p_driverList);
		V3XA.p_driverList = NULL;
	}	
    return;
}

static void ChannelSetParms(int channel, V3XVECTOR *pos, V3XVECTOR *velocity, V3XRANGE *range)
{
    IDirectSound3DBuffer *p3DS = g_pDSHandles[channel].pbuffer3D;

	HRESULT  hr;
    if (p3DS)
    {
        if ( pos )
        {
            hr = SYS_DXTRACE(p3DS->SetPosition(pos->x, pos->y, pos->z, DS3D_DEFMODE));
        }
		else
			hr = p3DS->SetPosition(0, 0, 0, DS3D_DEFMODE);
        if ( velocity )
        {
            hr = p3DS->SetVelocity(velocity->x, velocity->y, velocity->z, DS3D_DEFMODE);
        }
        if ( range )
        {
            hr = p3DS->SetMaxDistance(range->max, DS3D_DEFMODE);
            hr = p3DS->SetMinDistance(range->min, DS3D_DEFMODE);
        }
    }

    return;
}


static void RLXAPI UserSetParms(V3XMATRIX *lpMAT, V3XVECTOR *lpVEL,
								float*lpDistanceF, float*lpDopplerF, float*lpRolloffF)
{
	HRESULT  hr;
    if (!g_lpDS3DListener)  
		return;
    if (lpMAT)
    {
        V3XVECTOR *ps = &lpMAT->v.Pos;
        hr = g_lpDS3DListener->SetPosition(ps->x, ps->y, ps->z, DS3D_DEFMODE);
        		
		g_lpDS3DListener->SetOrientation(
        lpMAT->v.K.x, lpMAT->v.K.y, lpMAT->v.K.z, // 3D engine use a other orientation (right hand)
         lpMAT->v.J.x, lpMAT->v.J.y, lpMAT->v.J.z, 
        DS3D_DEFMODE);
		
    }
    if (lpVEL)        
		g_lpDS3DListener->SetVelocity(lpVEL->x, lpVEL->y, lpVEL->z, DS3D_DEFMODE);
    if (lpDistanceF)  
		g_lpDS3DListener->SetDistanceFactor(*lpDistanceF, DS3D_DEFMODE);
    if (lpDopplerF)   
		g_lpDS3DListener->SetDopplerFactor( *lpDopplerF, DS3D_DEFMODE);
    if (lpRolloffF)   
		g_lpDS3DListener->SetRolloffFactor( *lpRolloffF, DS3D_DEFMODE);
    return;
}

static void RLXAPI Render(void)
{

	HRESULT  hr;
    if (g_lpDS3DListener)
    {
        hr = SYS_DXTRACE(g_lpDS3DListener->CommitDeferredSettings());
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int32_t DS_calc_dB(int32_t linear_min, int32_t linear_max, int32_t linear_level)
*
* DESCRIPTION :  Decibel to linear converter
*
*/
static int32_t DS_calc_dB(float linear_min, float linear_max, float linear_level)
{
    double mn, mx, lv, ratio;
    int32_t    result;
    //
    // Ensure extreme values return max/min results
    //
    if (linear_level == linear_min)
    {
        return -10000;
    }
    if (linear_level == linear_max)
    {
        return 0;
    }
    mn = (double) linear_min;
    mx = (double) linear_max;
    lv = (double) linear_level;
    if (lv != 0)
    {
        ratio = (mn + mx) / lv;
    }
    else
    {
        ratio = (mn + mx) / 1;
    }
    result = (int32_t) (-2000.0 * log10(ratio));
    return result;
}

static LONG GetVol(float volume)
{
	return DS_calc_dB(0, 1, volume);
}

static LONG GetPan(float pan)
{
	LONG result;

	pan += 1;
	if (pan > 1)
		result = - DS_calc_dB(0, 1, (2*1)-pan);
	else
	if (pan < 1)
		result = DS_calc_dB(0, 1, pan);
	else
		result = 0;
	return result;
}


/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void DS_Register3D(V3XA_HANDLE *sam, DS_handle *hnd, int options)
*
* DESCRIPTION :  Register a sound buffer.
*
*/
static int DS_Register3D(V3XA_HANDLE *sam, DS_handle *hnd, int options)
{

	HRESULT  hr;
    hnd->sample = sam;
    if ((!sam)||(!hnd->pbuffer)) 
		return 0;
    sam->sampleFormat&=~V3XA_FMTVOLATILE;
    if ((options&1)||(sam->sampleFormat & V3XA_FMT3D))
    {
        hr = hnd->pbuffer->QueryInterface(IID_IDirectSound3DBuffer, (void**)&hnd->pbuffer3D);
        if (hr==DS_OK)
        {
            hr = hnd->pbuffer3D->SetAllParameters(&Buffer3dProps, DS3D_IMMEDIATE);
        }
    }
    return  1;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void DriverSampleSetVolume(float volume)
*
* DESCRIPTION :  Master volume controle
*
*/
static void SetVolume(float volume)
{
    LPDIRECTSOUNDBUFFER pDS = g_lpPrimaryBuffer;
    if (!pDS) 
		return;
    SYS_DXTRACE(pDS->SetVolume(GetVol(volume)));
    int v = (int)(volume*255.f);
    auxSetVolume(0, v|(v<<8)|(v<<16)|(v<<24));
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void ChannelSetVolume(int chan, int volume)
*
* DESCRIPTION :  Modify a channel volume
*
*/
void ChannelSetVolume(int chan, float volume)
{
    LPDIRECTSOUNDBUFFER pDS = g_pDSHandles[chan].pbuffer;
    if (!pDS) return;
    pDS->SetVolume(GetVol(volume));
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void ChannelSetPanning(int chan, int pan)
*
* DESCRIPTION :  Set panning (only with 2D buffers)
*
*/
static void ChannelSetPanning(int chan, float pan)
{
    LPDIRECTSOUNDBUFFER pDS = g_pDSHandles[chan].pbuffer;
    if (!pDS) 
		return;
	SYS_DXTRACE(pDS->SetPan(GetPan(pan)));
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void ChannelSetSamplingRate(int chan, int freq)
*
* DESCRIPTION :  Set frequency.
*
*/
static void ChannelSetSamplingRate(int chan, int freq)
{
    LPDIRECTSOUNDBUFFER pDS = g_pDSHandles[chan].pbuffer;
    if (!pDS) return;
    pDS->SetFrequency(freq);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void ChannelOpen(int gain, int channel)
*
* DESCRIPTION :  Allocate sound drivers.
*
*/
static void ChannelOpen(int gain, int channel)
{
    if (!channel) 
		channel = 16;
    RLX.Audio.ChannelToMix = (u_int8_t)channel;
    UNUSED(gain);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void SND_SmpRelease(SAMPLE *sam)
*
* DESCRIPTION : Free a sound handle
*
*/
static void SmpRelease(V3XA_HANDLE *sam)
{
    if (sam->sample) MM_std.free(sam->sample);
    sam->sample = NULL;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : BOOL static DS_UploadSample(u_int16_t channel, u_int32_t offset, char *buffer, u_int32_t size, int mode)
*
* DESCRIPTION :  Upload a raw sample into a DSound buffer.
*
*/
static BOOL DS_UploadSample(u_int16_t channel, u_int32_t offset, void *buffer, u_int32_t size)
{

	HRESULT  hr;
    void *lpP1 = NULL;
    ULONG  dwB1 = 0;
    void *lpP2 = NULL;
    ULONG  dwB2 = 0;
    LPDIRECTSOUNDBUFFER pDS = g_pDSHandles[channel].pbuffer;
    if ((!pDS)||(!buffer))
    {
        return FALSE;
    }

	do
    {
		hr = pDS->Lock(offset, size, &lpP1, &dwB1, &lpP2, &dwB2, 0);
        if (hr == DSERR_BUFFERLOST)
			pDS->Restore();
    }while(hr==DSERR_BUFFERLOST);
    
	if (hr != DS_OK)
    {
        return FALSE;
    }
    if (lpP1) 
		sysMemCpy(lpP1, buffer     , dwB1);
    if (lpP2) 
		sysMemCpy(lpP2, (u_int8_t*)buffer+ dwB1, dwB2);

	hr = pDS->Unlock( lpP1, dwB1, lpP2, dwB2);
    if (hr != DS_OK)
    {
        return FALSE;
    }
    return TRUE;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  u_int16_t static DS_AllocBuffer(u_int32_t size, u_int32_t nSamplesPerSec, u_int16_t nBitsPerSample, u_int16_t nChannels, u_int16_t channel, DWORD smode)
*
* DESCRIPTION :  Allocate a DirectSound secondary buffer.
*
*/
static u_int16_t DS_AllocBuffer(u_int32_t size, u_int32_t nSamplesPerSec, u_int16_t nBitsPerSample, u_int16_t nChannels, short channel, DWORD smode)
{
	HRESULT  hr;
    PCMWAVEFORMAT   pwf;
    DS_handle      *hnd = channel>=0  ? g_pDSHandles + channel : &DS_SecBuffer;
    // Initialize
    sysMemZero(&pwf, sizeof(PCMWAVEFORMAT));
    pwf.wBitsPerSample =  nBitsPerSample;
    pwf.wf.nChannels =  nChannels;
    // Format Tag should be the same as the Primary (Compatiblity with some non certified drivers).
    pwf.wf.wFormatTag =  g_cDSPCMOutFormat.wFormatTag;
    pwf.wf.nSamplesPerSec  =  g_cDSPCMOutFormat.nSamplesPerSec ? g_cDSPCMOutFormat.nSamplesPerSec : nSamplesPerSec ;
    pwf.wf.nBlockAlign =  (u_int16_t)(pwf.wf.nChannels *  (pwf.wBitsPerSample>>3));
    pwf.wf.nAvgBytesPerSec =  (u_int32_t)pwf.wf.nSamplesPerSec * (u_int32_t)pwf.wf.nBlockAlign;
    // Initialisation de la structure buffer
    sysMemZero(&DS_BufferDesc, sizeof(DSBUFFERDESC));
    DS_BufferDesc.dwSize =  sizeof(DSBUFFERDESC);
    DS_BufferDesc.dwFlags  =  smode;
    DS_BufferDesc.dwBufferBytes =  size;
    DS_BufferDesc.lpwfxFormat =  (LPWAVEFORMATEX) &pwf;
    // Creation du buffer son
    hr = g_lpDSDevice->CreateSoundBuffer( &DS_BufferDesc, &hnd->pbuffer, NULL);
    if (SYS_DXTRACE(hr))
    {
		return 0;
    }
    return (hr != DS_OK) ? (u_int16_t)0 : channel<0 ? (u_int16_t)0xff : (u_int16_t)(channel+1);
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void SmpLoad(V3XA_HANDLE *sam)
*
* DESCRIPTION :
*
*/
int SmpLoad(V3XA_HANDLE *sam)
{
    sam->sampleFormat|=V3XA_FMTVOLATILE;
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int DSS_RegisterHandle(V3XA_HANDLE *sam, int channel)
*
* DESCRIPTION : Kill a channel
*
*/
static void DSS_KillChannel(int channel)
{
    DS_handle *hnd = g_pDSHandles + channel;
    LPDIRECTSOUNDBUFFER pDS = hnd->pbuffer;
    if (pDS)
    {
        ChannelStop(channel);
        DS_Unregister3D(channel);
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int DSS_RegisterHandle(V3XA_HANDLE *sam, int channel)
*
* DESCRIPTION :  Try to duplicate the sample.
*
*/
static int DSS_DuplicateHandle(V3XA_HANDLE *sam, int channel)
{

	HRESULT  hr;
    int i;
    DS_handle *hnd = g_pDSHandles + channel;
    for  (i=0;i<RLX.Audio.ChannelToMix;i++)
    {
        DS_handle *p = g_pDSHandles + i ;
        if ((i!=channel)&&(p->sample == sam))
        {
            // Same on a different channel
            hnd->sample = sam;
            hr = g_lpDSDevice->DuplicateSoundBuffer(p->pbuffer, &hnd->pbuffer);
            if (SYS_DXTRACE(hr))
            {
                return -1;
            }
            DS_Register3D(sam, hnd, 0);
            sam->sampleFormat&=~V3XA_FMTVOLATILE;
            return i;
        }
    }
    return -1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int DSS_RegisterHandle(V3XA_HANDLE *sam, int channel)
*
* DESCRIPTION :   Register a sample with a give channel.
*
*/
static int DSS_RegisterHandle(V3XA_HANDLE *sam, int channel)
{
	ULONG  lpStatus;
	HRESULT  hr;
    DS_handle *hnd = g_pDSHandles + channel;
    LPDIRECTSOUNDBUFFER pDS = hnd->pbuffer;
    DWORD mode = 0;
    // If there something in this channel ?
    if (pDS)
    {
        // Try to load in the same channel
        if ((g_pDSHandles[channel].sample==sam)&&(!(sam->sampleFormat&V3XA_FMTVOLATILE)))
        {
            hr = pDS->GetStatus(&lpStatus);
			if (hr==DS_OK)
			{
                if (lpStatus & DSBSTATUS_PLAYING)
                {
                    hr = pDS->Stop();
				}
				if (lpStatus & DSBSTATUS_BUFFERLOST)
				{
					if (DS_UploadSample( sam->sampleID, 0, (void*)sam->sample, sam->length))
					{
						return TRUE;
					}
					else
					{
						DS_Unregister3D(channel);
						return FALSE;
					}
				}
            }
            return TRUE;
        }
    }
    // Destroy channel if already exists
    DSS_KillChannel(channel);
    // Create format
    if ( sam->sampleFormat & V3XA_FMT3D)
    {
        mode = DSBCAPS_CTRL3D | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
    }
    else
    {
        mode = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2;
    }
    // FIXME : Hack
    if ((RLX.Audio.Config  & RLXAUDIO_UseHwMixer)&& (DriverGetMemory()>sam->length))
      mode |= DSBCAPS_STATIC;
    // Create a sound buffer.
    sam->sampleID =  DS_AllocBuffer(
    sam->length, 
    sam->samplingRate, 
    sam->sampleFormat & V3XA_FMT16BIT  ? 16 : 8, 
    sam->sampleFormat & V3XA_FMTSTEREO ?  2 : 1, 
    (u_int16_t)channel, 
    mode
    );
    // Load the data.
    if ((sam->sampleID == 0)||(!DS_UploadSample( (u_int16_t)channel, 0, (void*)sam->sample, sam->length)))
    {
		sam->sampleID = 0;
		return FALSE;
    }
    // Register it to 3D if necessary.
    DS_Register3D(sam, g_pDSHandles + channel, 0);
    return TRUE;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : int ChannelGetFree(V3XA_HANDLE *sam)
*
* DESCRIPTION : Find a channel with the sample handle.
*
*/
int ChannelGetFree(V3XA_HANDLE *sam)
{
    int i;
    int prefSecure =-1;
    int prefSamready =-1;
    int prefSamStoppedMatch =-1;
    int prefSamPriority =-1;
    int newChannel =-1;
	DS_handle *p;
    if (!(V3XA.State & 1)) 
		return -1;
    for (i = 0, p = g_pDSHandles;i<RLX.Audio.ChannelToMix-1;i++, p++)
    {
        
		ULONG  lpStatus;

        LPDIRECTSOUNDBUFFER pDS = p->pbuffer;
        if ((p->flags&DSH_RESERVED)==0) // n'y pense MEME PAS!
        {
            if (!pDS)
            {
                if ((prefSecure<0)&&(p->sample==NULL)) prefSecure = i;
                if (i) prefSamready = i;
            }
            else
            {
                if ((sam)&&(pDS->GetStatus(&lpStatus) == DS_OK))
                {
                    if (!(lpStatus & DSBSTATUS_PLAYING))
                    {
                        if (i) prefSamready = i;
						if ((p->sample==sam))
                        {
                            prefSamStoppedMatch=i;
                        }
                    }
                    else
                    {
                        if (p->sample)
                        {
                            if (sam->priority>=p->sample->priority) prefSamPriority=i;
                        }
                    }
                }
                else
                {
                    if (!p->sample)
                    {
                        if ((prefSecure<0)&&(p->sample==NULL)) prefSecure = i;
                    }
                }
            }
        }
    }
    if (prefSamStoppedMatch>=0) newChannel = prefSamStoppedMatch;
    else    
	if (prefSamready>=0)        newChannel = prefSamready;
    else
    if (prefSamPriority>=0)     newChannel = prefSamPriority;
    else
    if (prefSecure>=0)          newChannel = prefSecure;
    return newChannel>=32 ? -1 : newChannel;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int ChannelPlay(int channel, int freq, float volume, int pan, V3XA_HANDLE *sam)
*
* DESCRIPTION :  Play a sound in a given channel, sampling rate, volume, pan
*
*/
int ChannelPlay(int channel, int freq, float volume, float pan, V3XA_HANDLE *sam)
{
	ULONG  lpStatus;
	HRESULT  hr;
    int ret=FALSE;
	if (!g_lpDSDevice) 
		return FALSE;

    if ((DSS_DuplicateHandle(sam, channel)>=0) || DSS_RegisterHandle(sam, channel))
    {
		LPDIRECTSOUNDBUFFER pDS = g_pDSHandles[channel].pbuffer;
        if (pDS)
        if (SYS_DXTRACE(pDS->GetStatus(&lpStatus)) == DS_OK)
        {
            hr = SYS_DXTRACE(pDS->Play(0, 0, sam->loopend ? DSBPLAY_LOOPING : 0));
            if (hr == DS_OK)
            {
                SYS_DXTRACE(pDS->SetFrequency(freq));
                SYS_DXTRACE(pDS->SetVolume(GetVol(volume)));
                SYS_DXTRACE(pDS->SetPan(GetPan(pan)));
                ret = TRUE;
            }
        }
    }
    return ret;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int ChannelGetStatus(int channel)
*
* DESCRIPTION :   Get the channel status
*
*/
int ChannelGetStatus(int channel)
{
	ULONG  lpStatus;

    LPDIRECTSOUNDBUFFER pDS = g_pDSHandles[channel].pbuffer;
    if (!pDS) return FALSE;
    
    pDS->GetStatus(&lpStatus);
    if (lpStatus & DSBSTATUS_PLAYING) return TRUE;
    else return FALSE;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void StreamRelease(void)
*
* DESCRIPTION : Release a stream buffer
*
*/
void StreamRelease(V3XA_STREAM handle)
{
    DS_stream *pHandle = g_pDSStreams + handle;
    if (pHandle->nState&1)
    {
       LPDIRECTSOUNDBUFFER pDS = g_pDSHandles[pHandle->channel].pbuffer;
       g_pDSHandles[pHandle->channel].flags &=~DSH_RESERVED;
       ChannelStop(pHandle->channel);
       DSS_KillChannel(pHandle->channel);
    }
    pHandle->nState&=~1;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void StreamInitialize(int smode, int freq, int size, int precache)
*
* DESCRIPTION : Create a stream buffer.
*
*/
V3XA_STREAM StreamInitialize(int smode, int freq, size_t size, int precache)
{
    DS_stream *pHandle;
    int b;
    V3XA_STREAM handle = -1;
    for (b=0;b<MAX_STREAM;b++)
    {
        if ((g_pDSStreams[b].nState&1)==0) 
			handle = b;
    }
    if (handle==-1)  return -1;
    pHandle = g_pDSStreams + handle;
    sysMemZero(pHandle, sizeof(DS_stream));
    pHandle->channel = (short)ChannelGetFree(NULL);
    if (pHandle->channel<0) return 0;
    
    pHandle->sample.length  = size;
    pHandle->sample.priority = 255;
	pHandle->sample.samplingRate = freq;
    pHandle->sample.sampleFormat = (u_int16_t)smode;
	pHandle->m_nPlayVolume = 63;
    DSS_KillChannel(pHandle->channel);
    g_pDSHandles[pHandle->channel].flags |= DSH_RESERVED;

    pHandle->sample.sampleID = DS_AllocBuffer(
    pHandle->sample.length, 
    pHandle->sample.samplingRate, 
    pHandle->sample.sampleFormat & V3XA_FMT16BIT  ? 16 : 8, 
    pHandle->sample.sampleFormat & V3XA_FMTSTEREO ? 2  : 1, 
    pHandle->channel,  DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN |  DSBCAPS_GETCURRENTPOSITION2 );
    if (pHandle->sample.sampleID)
    {
        pHandle->nState   |= 1;
        pHandle->m_nStreamState = (u_int16_t)precache;
        DS_Register3D(&pHandle->sample, g_pDSHandles + pHandle->channel, 0);
        return handle;
    }
    else
    {
        pHandle->m_nStreamState = -1;
        pHandle->nState&=~1;
        return 0;
    }
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int StreamPoll(void)
*
* DESCRIPTION : Poller to the stream. If return -1, StreamLoad must be called.
*
*/
int StreamPoll(V3XA_STREAM handle)
{
    DS_stream *pHandle = g_pDSStreams + handle;
    DWORD lPlay=0;
	DWORD lWrite=0;
    int ret=-1;
    LPDIRECTSOUNDBUFFER pDS = g_pDSHandles[pHandle->channel].pbuffer;
    if (!pDS) 
		return ret;

    if ((pHandle->nState&1)==0)
		return -1;

    if (pHandle->m_nStreamState) 
	{
		ret = 1;
	}
    else
    {
		// Polled streaming
		int32_t elapsed;
		ULONG lpStatus;

		if (pDS->GetStatus(&lpStatus)==DS_OK)
		{
			if ((lpStatus & DSBSTATUS_PLAYING)==0)
			{
				SYS_DXTRACE(pDS->Play(0, 0, DSBPLAY_LOOPING));
			}
		}
		
		SYS_DXTRACE(pDS->GetCurrentPosition(&lWrite, &lPlay));

		elapsed = (pHandle->m_nPreviousPosition<=(int32_t)lPlay)  
				? lPlay - pHandle->m_nPreviousPosition
				: (lPlay + (int32_t)pHandle->sample.length) - pHandle->m_nPreviousPosition;

		pHandle->m_nWriteBytes+=elapsed;
		pHandle->m_nTotalBytes+=elapsed;
		pHandle->m_nPreviousPosition = lPlay;		

		if ((pHandle->m_nWriteBytes+elapsed>=pHandle->m_nUploadedBytes)
			&&(pHandle->m_nUploadedBytes))
		{			
			pHandle->m_nWriteBytes-=pHandle->m_nUploadedBytes;			
			pHandle->m_nUploadedBytes = 0;
			ret = 1;
		}
    }	
    return ret;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void StreamSetVolume(int volume)
*
* DESCRIPTION : Set stream volume
*
*/
void StreamSetVolume(V3XA_STREAM handle, float volume)
{
    DS_stream *pHandle = g_pDSStreams + handle;
    LPDIRECTSOUNDBUFFER pDS = g_pDSHandles[pHandle->channel].pbuffer;
	pHandle->m_nPlayVolume = volume;
    if (!pDS ) 
		return;
    SYS_DXTRACE(pDS->SetVolume( GetVol(volume) ));
    return;
}

int StreamGetChannel(V3XA_STREAM handle)
{
    DS_stream       *pHandle = g_pDSStreams + handle;
    return pHandle->channel;
}

int StreamStart(V3XA_STREAM handle)
{
	DS_stream       *pStr = g_pDSStreams + handle;
	LPDIRECTSOUNDBUFFER pSrc = g_pDSHandles[pStr->channel].pbuffer;
	SYS_ASSERT(pSrc);
	SYS_ASSERT(pStr->nState & 1);
	if (((pStr->nState&1)==0)||(!pSrc))
	{
		return 0;
	}

	if (SYS_DXTRACE(pSrc->Play(0, 0, DSBPLAY_LOOPING))!=DS_OK)
		return 0;

	SYS_DXTRACE(pSrc->SetCurrentPosition( 0 ));
	SYS_DXTRACE(pSrc->SetFrequency( pStr->sample.samplingRate ));

	pStr->m_nPreviousPosition = 0;
	pStr->m_nUploadedBytes = 0;
	pStr->m_nWritePosition = 0;
	pStr->m_nStreamState = 1;
	pStr->m_nTotalBytes = 0;
	pStr->m_nUploadedBytes = 0;
	pStr->m_nWriteBytes = 0;
	return 0;
}

void StreamStop(V3XA_STREAM handle)
{
	DS_stream		*pStr = g_pDSStreams + handle;
	LPDIRECTSOUNDBUFFER  pSrc = g_pDSHandles[pStr->channel].pbuffer;
	if (((pStr->nState&1)==0)||(!pSrc))
		return; 
	SYS_DXTRACE(pSrc->Stop());
}

static int StreamLoad(V3XA_STREAM handle, void *data, size_t size, int smode)
{
    DS_stream       *pHandle = g_pDSStreams + handle;
    LPDIRECTSOUNDBUFFER pDS = g_pDSHandles[pHandle->channel].pbuffer;
    if (((pHandle->nState&1)==0)||(!pDS)) 
		return -1;

    pHandle->sample.sampleFormat = (u_int16_t)smode;
    pHandle->sample.sample = data;
	
    if (DS_UploadSample(pHandle->channel, pHandle->m_nWritePosition, pHandle->sample.sample, size))
    {
		pHandle->m_nUploadedBytes += size;
		pHandle->m_nWritePosition += size;
		if (pHandle->m_nWritePosition >= (int32_t)pHandle->sample.length)
		{
			pHandle->m_nWritePosition -= (int32_t)pHandle->sample.length;
		}
	}
	else
	{
		return 0 ;
	} 
    if (pHandle->m_nStreamState)
    {
		pHandle->m_nStreamState--;
		if (!pHandle->m_nStreamState)
		{	
			pDS->SetCurrentPosition( 0 );
			pDS->Play( 0, 0, DSBPLAY_LOOPING );
			pHandle->m_nPreviousPosition = 0;
		}
    }
    return 0;
}


/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void ChannelInvalidate(V3XA_HANDLE *snd)
*
* DESCRIPTION : Invalidate a sample (for sound file cache).
*
*/
void ChannelInvalidate(V3XA_HANDLE *snd)
{
    int channel;
    for (channel = 0;channel < DSOUNDMAXBUFFER; channel ++)
    {
        DS_handle *p = g_pDSHandles + channel ;
        if ( (p->sample == snd) && (!( p->flags & DSH_RESERVED)))
        {
            ChannelStop(channel);
            DS_Unregister3D(channel);
        }
    }
    return;
}
V3XA_HANDLE *ChannelGetSample(int channel)
{
    DS_handle *p = g_pDSHandles + channel ;
    return p->sample;
}

size_t StreamGetPosition(V3XA_STREAM handle)
{
	ULONG  lpStatus;
	HRESULT  hr;
    DS_stream *pHandle = g_pDSStreams + handle;
    LPDIRECTSOUNDBUFFER pDS;
    if( handle<0) 
		return -1;
    pDS = g_pDSHandles[pHandle->channel].pbuffer;
    if (!pDS) 
		return -1;
    hr = pDS->GetStatus(&lpStatus);
    if (hr==DS_OK)
    {
		if (lpStatus & DSBSTATUS_PLAYING)
		{
			 return pHandle->m_nTotalBytes;
		}
    }
    return -1;
}


// Wave driver
int EAX_ChannelSetEnvironment(int channel, V3XA_REVERBPROPERTIES *cfg);

struct _v3xa_wave_client_driver V3XA_DS = {
	Enum,
	Detect,
	Initialize,
	Release,
	SetVolume,
	Start,
	Stop,
	Poll,
	Render,
	UserSetParms,
	ChannelOpen,
	ChannelPlay,
	ChannelStop,
	ChannelSetVolume,
	ChannelSetPanning,
	ChannelSetSamplingRate,
	ChannelGetStatus,
	ChannelSetParms,
	EAX_ChannelSetEnvironment,
	ChannelGetFree,
	ChannelFlushAll,
	ChannelInvalidate,
	ChannelGetSample,
	StreamRelease,
	StreamInitialize,
	StreamGetChannel,
	StreamGetPosition,
	StreamPoll,
	StreamSetVolume,
	StreamLoad,
	StreamStart,
	StreamStop,
	SmpLoad,
	SmpRelease,
	"DirectSound",
};

void V3XA_EntryPoint(struct RLXSYSTEM *p)
{
	V3XA.Client = &V3XA_DS;
}
