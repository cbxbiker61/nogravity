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
#include <dsound.h>
#include <objbase.h>
#include <initguid.h>
#include <stdio.h>

#include <EAX/eax.h>
#include "_rlx32.h"
#include "systools.h"
#include "sysresmx.h"
#include "iss_defs.h"
#include "iss_fx.h"
#include "snd_ds6.h"
#include "snd_eax.h"
#include "win32/w32_dx.h"

static struct _v3xa_reverbproperties EAX_cfg;

int EAX_SetProperty(LPKSPROPERTYSET pPropertySet, struct _v3xa_reverbproperties *cfg, int mode)
{
    EAX_REVERBPROPERTIES rev;
    rev.environment = cfg->environment; 
    rev.fVolume = cfg->fVolume;
    rev.fDecayTime_sec = cfg->fDecayTime_sec;
    rev.fDamping = cfg->fDamping;
    if (!pPropertySet)
       return 0;

	if (SYS_DXTRACE(pPropertySet->Set(
			DSPROPSETID_EAX_ReverbProperties, 
			DSPROPERTY_EAX_ALL, 
			NULL, 
			0, 
			&rev, 
			sizeof(EAX_REVERBPROPERTIES) )))
		return 0;
	else
		return 1;
}

int EAX_CreateBuffer(DS_handle *hnd)
{
    int             bEaxOK = FALSE;  // assume it's not there till we check it is.
    WAVEFORMATEX    pcmOut;          // Format of the wave for secondary buffer if we need to make one.
    DSBUFFERDESC    dsbdSecondary;   // description for creating secondary buffer if we need to make one.
    ULONG           ulSupport = 0;
    HRESULT hr;

    if (hnd->pbuffer)
		return 0;
    
	// we don't have a secondary to work with so we will create one.
	ZeroMemory( &dsbdSecondary, sizeof(DSBUFFERDESC));
	ZeroMemory( &pcmOut, sizeof(WAVEFORMATEX));

	// any format should do I just say 11kHz 16 bit mono
	pcmOut.wFormatTag = WAVE_FORMAT_PCM;
	pcmOut.nChannels = 1;
	pcmOut.nSamplesPerSec  = 44100;
	pcmOut.nBlockAlign = 2;
	pcmOut.nAvgBytesPerSec = pcmOut.nBlockAlign * pcmOut.nSamplesPerSec;	
	pcmOut.wBitsPerSample = 16;
	pcmOut.cbSize = 0;

	// size is just arbitary but not too small as I have seen problems with single sample buffers...
	dsbdSecondary.dwSize = sizeof(DSBUFFERDESC);
	dsbdSecondary.dwBufferBytes = 1024;
	dsbdSecondary.dwFlags  = DSBCAPS_CTRL3D | DSBCAPS_STATIC ;
	dsbdSecondary.lpwfxFormat = &pcmOut;
	if( SYS_DXTRACE(g_lpDSDevice->CreateSoundBuffer( &dsbdSecondary, &hnd->pbuffer, NULL )))
	    return (FALSE);

	// Create a 3D Sound Buffer
    if (!hnd->pbuffer3D)
    	SYS_DXTRACE(hnd->pbuffer->QueryInterface(IID_IDirectSound3DBuffer, (void**)&hnd->pbuffer3D));

	// Query interface
    if (SYS_DXTRACE(hnd->pbuffer->QueryInterface( IID_IKsPropertySet, (void**)&hnd->pReverb)))
         return (FALSE);

	// check if our listener related property set is available and supports everything.
    if( !SYS_DXTRACE( hr = hnd->pReverb->QuerySupport(
		DSPROPSETID_EAX_ReverbProperties,
		DSPROPERTY_EAX_ALL, &ulSupport)))
    {
		// check if we can get and set them.
		if ((ulSupport &(KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)) != (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET))
		{
			bEaxOK = 0;
		}
		else
		{
			bEaxOK = 1;
		}
		SYS_DXTRACE(hnd->pbuffer->Play(0, 0, DSBPLAY_LOOPING));
    }
    return bEaxOK;
}

int EAX_ChannelSetEnvironment(int channel, V3XA_REVERBPROPERTIES *cfg)
{
    // Save last environment.
    if (!cfg) 
		cfg = &EAX_cfg;
    else 
		EAX_cfg = *cfg;

    if (!DS_SecBuffer.pbuffer)
	    if (!EAX_CreateBuffer(&DS_SecBuffer)) 
				return 0;
    
	return EAX_SetProperty(
	     channel<0
	   ? DS_SecBuffer.pReverb
	   : g_pDSHandles[channel].pReverb, 
	     cfg, 
	     channel<0);
}
