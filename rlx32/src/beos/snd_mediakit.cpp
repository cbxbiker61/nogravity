//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2004 - realtech VR

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

#include <MediaDefs.h>
#include <SimpleGameSound.h>
#include <StreamingGameSound.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "_rlx32.h"
#include "systools.h"
#include "iss_defs.h"

// Couldn't test the audio on my BeOS (no sound support).
// #define SUPPORT_STREAMING

// Stream functions
struct GS_steam
{
    V3XA_HANDLE             sample;
	BStreamingGameSound *	handle;

	int32_t					m_nWritePosition;
	int32_t					m_nPreviousPosition;
	int32_t					m_nUploadedBytes;

	int32_t					PlayPosition;
	int32_t					SegmentSize;

	short					m_nStreamState;
	u_int16_t				flags;
};

struct GS_handle
{
	BSimpleGameSound	*	pbuffer;
	V3XA_HANDLE		    *	sample;
};

static GS_handle g_pVoiceHandles[MAX_V3XA_AUDIO_MIX];
static GS_steam	g_StreamHandles[MAX_V3XA_AUDIO_STREAM];
static float g_fGain = 0.9f;

static int32_t CALLING_C Poll(int32_t param)
{
	return 0;
}

static int Enum(void)
{
	return 1;
}

static void Start(void)
{
	return;
}

static void Stop(void)
{
	return;
}

static int Detect(void)
{
	return FALSE;	// success
}

static int Initialize(void *hwnd)
{
    UNUSED(hwnd);
	return FALSE; 	// success
}

static void Release(void)
{
	return;
}

static void SetVolume(float volume)
{
    g_fGain = volume;
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void ChannelSetVolume(int channel, float volume)
*
* DESCRIPTION :  Modify a channel volume
*
*/
static void ChannelSetVolume(int channel, float volume)
{
	BSimpleGameSound *pDS = g_pVoiceHandles[channel].pbuffer;
	if (!pDS)
		return;
	pDS->SetGain((float)volume * g_fGain);
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void ChannelSetPanning(int channel, float pan)
*
* DESCRIPTION :  Set panning (only with 2D buffers)
*
*/
static void ChannelSetPanning(int channel, float pan)
{
	BSimpleGameSound *pDS = g_pVoiceHandles[channel].pbuffer;
	if (!pDS)
		return;
	pDS->SetPan((float)pan, 0);
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void ChannelSetSamplingRate(int channel, int frequency)
*
* DESCRIPTION :  Set frequency.
*
*/
static void ChannelSetSamplingRate(int channel, int newSamplingRate)
{
	BSimpleGameSound *pDS = g_pVoiceHandles[channel].pbuffer;
	if (!pDS)
		return;
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void ChannelOpen(int gain, int channel)
*
* DESCRIPTION :  Allocate sound drivers.
*
*/
static void ChannelOpen(int gain, int channel)
{
	if (!channel)
		channel = 16;
	V3XA.numChannel = channel;
	UNUSED(gain);
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void SmpRelease(SAMPLE *pSample)
*
* DESCRIPTION : Free a sound handle
*
*/
static void SmpRelease(V3XA_HANDLE *pSample)
{
	if (pSample->sample)
		MM_std.free(pSample->sample);
	pSample->sample = NULL;
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void SmpLoad(V3XA_HANDLE *pSample)
*
* DESCRIPTION :
*
*/
static int SmpLoad( V3XA_HANDLE *pSample)
{
	return 1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : int ChannelGetFree(V3XA_HANDLE *pSample)
*
* DESCRIPTION : Intelligent Free Channel Finder (IFCF(tm)). Find a channel with the sample handle.
*
*/
static int ChannelGetFree(V3XA_HANDLE *pSample)
{
	int i;
	int prefSecure			=-1;
	int prefSamready		=-1;
	int prefSamStoppedMatch =-1;
	int prefSamPriority 	=-1;
	int newChannel			=-1;
	int n = 0;
	GS_handle *p = g_pVoiceHandles;

	n = V3XA.numChannel;
	for (i=0;i<n;p++, i++)
	{
		if (!p->pbuffer) // Handle free
		{
			if ((prefSecure<0) && (p->sample == NULL))
				prefSecure = i;
			if (i)
				prefSamready = i;
		}
		else
		{
			if (pSample)
			{
				if (p->pbuffer->IsPlaying()==false)
				{
					if (i) prefSamready = i;
					if (p->sample==pSample)
					{
						prefSamStoppedMatch=i;
					}
				}
				else
				{
					if (p->sample)
					{
					if (pSample->priority>p->sample->priority)
						prefSamPriority=i;
					}
				}
				}
				else
				{
				if (!p->sample)
				{
					if (prefSecure<0) prefSecure = i;
				}
			}
		}
	}
	/*if (prefSamStoppedMatch>=0) newChannel = prefSamStoppedMatch; // use a non-playing channel with a matching sample
	else
	*/
	if (prefSamready>=0)	
	   newChannel = prefSamready; // use a channel without playing sound
	else
	if (prefSamPriority>=0) 	
	   newChannel = prefSamPriority; // use a channel with less priority than the requested
	else
	if (prefSecure>=0)		
	   newChannel = prefSecure; // use a blank channel without sample already allocated
	return newChannel>=32 ? -1 : newChannel;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	int ChannelPlay(int channel, int frequency, float volume, float pan, V3XA_HANDLE *pSample)
*
* DESCRIPTION :  Play a sound in a given channel, sampling rate, volume, pan
*
*/
static unsigned SetFormat(struct gs_audio_format *format, int frequency, unsigned sampleFormat, unsigned size)
{
	unsigned frame = 0;
	sysMemZero(format, sizeof(struct gs_audio_format));
	if (sampleFormat&V3XA_FMTIEEE)
	{
		format->format = gs_audio_format::B_GS_F;
		frame = 4;
	}
    else
	if (sampleFormat&V3XA_FMT16BIT)
	{
		frame = 2;
		format->format = gs_audio_format::B_GS_S16;
	}
	else
	{
		frame = 1;
    	format->format = gs_audio_format::B_GS_U8;
    }

	if (sampleFormat&V3XA_FMTSTEREO)
	{
		format->channel_count = 2;
		frame*=2;
	}
	else
		format->channel_count = 1;

	format->byte_order = 1;
	format->frame_rate = (float)frequency;
	format->buffer_size = size;
	return frame;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	int ChannelPlay(int channel, int frequency, float volume, float pan, V3XA_HANDLE *pSample)
*
* DESCRIPTION :
*
*/
static int ChannelPlay(int channel, int frequency, float volume, float pan, V3XA_HANDLE *pSample)
{
	GS_handle *hnd = g_pVoiceHandles + channel;
	int i;
	struct gs_audio_format format;
	// Sample already exists at the very same slot. Use it, don't reload
	if ((hnd->pbuffer)&&(hnd->sample==pSample))
	{
		hnd->pbuffer->StartPlaying();
		hnd->pbuffer->SetPan((float)pan, 0);
		hnd->pbuffer->SetGain((float)volume * g_fGain);
		return 1;
	}
	// A different buffer. Kill it
	if ((hnd->pbuffer)&&(hnd->sample!=pSample))
	{
		delete hnd->pbuffer;
		hnd->pbuffer = NULL;
	}
	// Scan other channel if the sample was loaded and try to 'clone' it.
	for (i=0;(i<V3XA.numChannel)&&(!hnd->pbuffer);i++)
	{
		GS_handle *hnd2 = g_pVoiceHandles + i;
		if ((i!=channel) && (hnd2->sample == pSample) && (hnd2->pbuffer))
		{
			hnd->pbuffer = new BSimpleGameSound(*hnd2->pbuffer);
		}
	}
	// We must instance an new BSimpleGameSound at this point
	if (!hnd->pbuffer)
	{
		unsigned frameSize = SetFormat(&format, frequency, pSample->sampleFormat, pSample->length);
		hnd->pbuffer = new BSimpleGameSound(pSample->sample, pSample->length / frameSize, &format, NULL);
		if (pSample->loopend)
		{
		   hnd->pbuffer->SetIsLooping(TRUE);
		   pSample->priority = 255;
		}
	}
	if (hnd->pbuffer)
	{
		hnd->sample = pSample;
		hnd->pbuffer->SetPan((float)pan, 0);
		hnd->pbuffer->SetGain((float)volume * g_fGain);
		hnd->pbuffer->StartPlaying();
	}

	return 1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void ChannelStop(int channel)
*
* DESCRIPTION :   Stop the sound in a channel.
*
*/
static void ChannelStop(int channel)
{
	BSimpleGameSound *pDS = g_pVoiceHandles[channel].pbuffer;
	if (!pDS)
		return;
	pDS->StopPlaying();
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	int ChannelGetStatus(int channel)
*
* DESCRIPTION :   Get the channel status
*
*/
static int ChannelGetStatus(int channel)
{
	BSimpleGameSound *pDS = g_pVoiceHandles[channel].pbuffer;
	if (!pDS)
		return FALSE;
	return pDS->IsPlaying();
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void StreamRelease(void)
*
* DESCRIPTION : Release a stream buffer
*
*/
static void StreamRelease(V3XA_STREAM handle)
{
	GS_steam *pHandle = g_StreamHandles + handle;
	if (pHandle->flags)
	{
		delete pHandle->handle;
		free(pHandle->sample.sample);
	}
	pHandle->flags=0;
	return;
}

static size_t StreamGetPosition(V3XA_STREAM handle)
{
	GS_steam *pHandle = (GS_steam*)handle;
	return pHandle->PlayPosition;
}

static int StreamStart(V3XA_STREAM handle)
{
	GS_steam *pHandle = (GS_steam*)handle;
	if (pHandle->flags)
	{
		pHandle->handle->StartPlaying();
	}
	return -1;
}

static void StreamStop(V3XA_STREAM handle)
{
	GS_steam *pHandle = (GS_steam*)handle;
	if (pHandle->flags)
	{
		pHandle->handle->StopPlaying();
	}
	return ;
}

static void StreamSetVolume(V3XA_STREAM handle, float volume)
{
	GS_steam *pHandle = g_StreamHandles + handle;
	if (pHandle->flags)
	{
		pHandle->handle->SetGain(volume * g_fGain);
	}
	return;
}
static int StreamGetChannel(V3XA_STREAM handle)
{
	GS_steam *pHandle = g_StreamHandles + handle;
	UNUSED(pHandle);
	return 0;
}

// WARNING: This streaming code doesn't work at all.

static void SoundRenderer(void *cookie, void *inBuffer, size_t byteCount, BStreamingGameSound *object)
{
	GS_steam *pHandle = (GS_steam*)cookie;
	if (!inBuffer || !byteCount)
	   return;

	if (!pHandle->sample.sample) 
	   return;


	if ((signed)(pHandle->PlayPosition + byteCount) < (signed)pHandle->sample.length )
	{
		sysMemCpy((u_int8_t*)inBuffer, (char*)pHandle->sample.sample + pHandle->PlayPosition, byteCount);
		pHandle->PlayPosition+=byteCount;
	}
	else
	{
		int32_t left  = pHandle->sample.length - pHandle->PlayPosition;
		int32_t right = byteCount-left;
		if (left)  sysMemCpy((u_int8_t*)inBuffer, (char*)pHandle->sample.sample + pHandle->PlayPosition, left);
		if (right) sysMemCpy((u_int8_t*)inBuffer+left, (char*)pHandle->sample.sample, right);
		pHandle->PlayPosition = right;
	}

	return;
}

static V3XA_STREAM StreamInitialize(int sampleFormat, int frequency, size_t size)
{
	GS_steam	*pHandle;
	V3XA_STREAM handle = -1;
	struct gs_audio_format format;
	int b;
	for (b=0;b<MAX_V3XA_AUDIO_STREAM;b++)
	{
		if (g_StreamHandles[b].flags==0)
            handle = b;
	}
	if (handle==-1)
		return -1;
	pHandle = g_StreamHandles + handle;
	sysMemZero(pHandle, sizeof(GS_steam));
	pHandle->sample.length = size;
	pHandle->m_nStreamState	= 2;
	pHandle->sample.sampleFormat = sampleFormat;
	pHandle->sample.sample = malloc(pHandle->sample.length);
	memset(pHandle->sample.sample, 0, pHandle->sample.length);
	pHandle->flags = 1;
	int sampleSize = SetFormat(&format, frequency, sampleFormat, size);
	int sampleCount = size / sampleSize; // precache ??
	pHandle->handle = new BStreamingGameSound(sampleCount, &format, 2, NULL);
	if ( pHandle->handle)
	{
		if ( pHandle->handle->InitCheck() != B_OK)
		{
			pHandle->flags = 0;
			delete pHandle->handle;
			return 0;
		}
	}
	pHandle->handle->SetStreamHook(SoundRenderer, pHandle);
	return handle;
}

static int StreamPoll(V3XA_STREAM handle)
{
	int ret = -1;
	GS_steam	*pHandle = g_StreamHandles + handle;
	if (pHandle->m_nStreamState)
		ret = 1;
	else
	{
		if (pHandle->m_nPreviousPosition<=pHandle->PlayPosition)
			pHandle->m_nUploadedBytes+= pHandle->PlayPosition-pHandle->m_nPreviousPosition;
		else
			pHandle->m_nUploadedBytes+=(pHandle->PlayPosition+pHandle->sample.length)-pHandle->m_nPreviousPosition;
		pHandle->m_nPreviousPosition = pHandle->PlayPosition;
		if (pHandle->m_nUploadedBytes>=pHandle->SegmentSize)
		{
			pHandle->m_nUploadedBytes-=pHandle->SegmentSize;
			ret = 2;
		}
	}
	return ret;
}

static int StreamLoad(V3XA_STREAM handle, void *data, size_t size)
{
	u_int8_t *buffer = (u_int8_t*)data;
	GS_steam   *pHandle = g_StreamHandles + handle;
	pHandle->SegmentSize = size;

	if (pHandle->m_nWritePosition + (int32_t)pHandle->SegmentSize < (int32_t) pHandle->sample.length)
	{
		sysMemCpy((char*)pHandle->sample.sample + pHandle->m_nWritePosition, buffer , pHandle->SegmentSize);
		pHandle->m_nWritePosition+=pHandle->SegmentSize;
	}
	else
	{
		int32_t left  = pHandle->sample.length - pHandle->m_nWritePosition;
		int32_t right = pHandle->SegmentSize-left;
		if (left)
            sysMemCpy((char*)pHandle->sample.sample + pHandle->m_nWritePosition, buffer, left);
		if (right)
            sysMemCpy((char*)pHandle->sample.sample, buffer + left, right);
		pHandle->m_nWritePosition = right;
	}
	if (pHandle->m_nStreamState)
	{
		pHandle->m_nStreamState--;
		if (!pHandle->m_nStreamState)
		{
			pHandle->handle->StartPlaying();
		}
	}
	return pHandle->m_nStreamState;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void ChannelFlushAll(int mode)
*
* DESCRIPTION :
*
*/
static void ChannelFlushAll(int mode)
{
	GS_handle *p = g_pVoiceHandles;
	int i;
	for (i=0;i<MAX_V3XA_AUDIO_MIX;i++, p++)
	{
		if (p->pbuffer)
		{
			p->pbuffer->StopPlaying();
			delete p->pbuffer;
			p->pbuffer = NULL;
			p->sample = NULL;
		}
	}
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void ChannelInvalidate(V3XA_HANDLE *snd)
*
* DESCRIPTION :
*
*/
static void ChannelInvalidate(V3XA_HANDLE *snd)
{
	GS_handle *p = g_pVoiceHandles;
	int i;
	for (i=0;i<32;i++, p++)
	{
		if (p->sample == snd)
		{
			p->pbuffer->StopPlaying();
			delete p->pbuffer;
			p->pbuffer = NULL;
			p->sample = NULL;
		}
	}
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	V3XA_HANDLE *ChannelGetSample(int channel)
*
* DESCRIPTION :
*
*/
static V3XA_HANDLE *ChannelGetSample(int channel)
{
	GS_handle *p = g_pVoiceHandles + channel ;
	return p->sample;
}

static void ChannelSetParms(int channel, V3XVECTOR *pos, V3XVECTOR *velocity, V3XRANGE *fRange)
{
	return;
}

static void UserSetParms( V3XMATRIX *lpMAT, V3XVECTOR *velocity, float *lpDistanceF, float *lpDopplerF, float *lpRolloff)
{
	return;
}

static void Render(void)
{
	return;
}

static int ChannelSetEnvironment(int channel,V3XA_REVERBPROPERTIES *cfg)
{
   return 0;
}


void RLXAPI V3XA_EntryPoint(struct RLXSYSTEM *p)
{
	 static V3XA_WaveClientDriver driver = {
	
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
		ChannelSetEnvironment,
	
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
	
		"Mediakit"
	};
	V3XA.Client = &driver;
	return;
}
