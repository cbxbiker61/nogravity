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

#ifndef __amigaos__
#define USE_THREAD
#define USE_MUTEX
#endif

#include "_rlx.h"
#include "_stub.h"
#include "systools.h"

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

#include "v3x_1.h"
#include "gui_os.h"
#include "gx_init.h"
#include "gx_rgb.h"
#include "iss_defs.h"
#include "iss_fx.h"

#include "lt_struc.h"
#include "lt_data.h"
#include "lt_func.h"

static SND_DWHANDLE g_pSoundList[MAX_SAMPLE];
static void (* CALLING_C ScreenPageFlip)(void);
static V3XA_HANDLE *g_pFXTable;
static int			g_nSample;
static V3XA_STREAM	g_pWavStream;
static volatile int	m_Status;
static SYS_THREAD	m_Thread;
SYS_MUTEX m_Mutex;

typedef struct
{
    int			playChannel;
    int			samplingRate;
    float			volume;
    float			pan;
    V3XA_HANDLE		*smpHandle;
    SND_DWHANDLE	*smpInfo;
}NG_SAMPLE_AUDIO;

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_AudioBeep(int cod)
*
* DESCRIPTION :
*
*/
void NG_AudioBeep(int code)
{
    switch(code)
	{
        case 0: // Weapons
        NG_AudioPlaySound(NG_AudioGetByName("bridge1")-1, 0);
        break;
        case 1: // NG_AudioBeep
        NG_AudioPlaySound(NG_AudioGetByName("bridge2")-1, 0);
        break;
        case 2: // Add bonus
        NG_AudioPlaySound(NG_AudioGetByName("door")-1, 0);
        break;
        case 3: // Locked
        NG_AudioPlaySound(NG_AudioGetByName("message")-1, 0);
        break;
        case 4: // New message
        NG_AudioPlaySound(NG_AudioGetByName("incoming")-1, 0);
        break;
        case 5: // Move
        NG_AudioPlaySound(NG_AudioGetByName("select")-1, 0);
        break;
    }
}

void NG_AudioLoadList(void)
{
    SYS_FILEHANDLE in;
    int i, k;
    char *v;
    char fil[256];
	char tex[256];
    sprintf(tex, "%s\\voix\\track.lst", ".");
    in = FIO_cur->fopen(tex, "rb");
	SYS_ASSERT(in);

    FIO_cur->fgets(tex, 80, in);
    i=-1;
    do
    {
        FIO_cur->fgets(tex, 80, in);
        i++;
        v=strstr(tex, "=");
        if (v)
        {
            v+=2;
            sscanf(v, "%s%hd%hd\n", fil, &g_pMusicInfo[i].track, &g_pMusicInfo[i].mode);
            sysStrnCpy(g_pMusicInfo[i].filename, fil, 16);
        }
    }while(strstr(tex, "*")==NULL);
    FIO_cur->fclose(in);
    in = FIO_cur->fopen(".\\voix\\soundfx.lst", "rb");
    SYS_ASSERT(in);
    i=-1;
    do
    {
		char tmp[128];
        i++;

		FIO_cur->fgets(tmp, 128, in);
        sscanf(tmp, "%s %d %d %d %s\n",
        g_pSoundList[i].name,
        &g_pSoundList[i].randomPitch,
        &g_pSoundList[i].defaultVol,
        &k, tex);
        g_pSoundList[i].priority = k;


	}while(strstr(g_pSoundList[i].name, "*")==NULL);
    g_pSoundList[i].name[0] = 0;

	FIO_cur->fclose(in);
}

int NG_AudioGetByName(char *s)
{
    int i=0;
	char *ss = s;
	while(*s)
	{
		if (*s == '.')
			*s = 0;
		s++;
	}

    while(*g_pSoundList[i].name)
    {
        if (sysStriCmp(g_pSoundList[i].name, ss)==0)
			return i+1;
        i++;
    }
	SYS_ASSERT(0);
    return 0;
}

static void SFX_SampleBatchRelease(V3XA_HANDLE *smp)
{
    int i;
    for (i=0;i<g_nSample;i++)
		V3XA_Handle_Release(smp+i);
    MM_heap.free(smp);
    g_nSample = 0;
}

#ifndef __APPLE__

static void Resample44Khz(V3XA_HANDLE *smp)
{
	uint8_t *src = (uint8_t*)smp->sample;
	int factor = 44100 / smp->samplingRate;
	short *dst = (short*)MM_std.malloc(smp->length * factor * 2);
	unsigned i;
	for (i=0;i<smp->length;i++)
	{
		int j;
		for (j=0;j<factor;j++)
		{
			dst[factor*i+j] = (short)((src[i]+(src[i] << 8))-32768);
		}
	}
	smp->sampleFormat|=V3XA_FMT16BIT;
#ifdef __BIG_ENDIAN__
	smp->sampleFormat|=V3XA_FMTBIGENDIAN;
#endif
	smp->samplingRate = 44100;
	smp->sample = dst;
	smp->length*=factor*2;
	MM_heap.free(src);
}
#endif //

static V3XA_HANDLE *SFX_SampleBatchLoad(SND_DWHANDLE *sef)
{
    V3XA_HANDLE *WT, *sinfo;
    int  i;
    g_nSample = 0;
    while (sef[g_nSample].name[0]!=0)
		g_nSample++;
    WT = MM_CALLOC(g_nSample, V3XA_HANDLE);  sinfo = WT;
    for (i=0;i<g_nSample;i++, sinfo++, sef++)
    {
		char tex[256];
		sprintf(tex, ".\\voix\\%s.WAV", sef->name);
		V3XA_Handle_LoadFromFn(sinfo, tex);
#ifndef HAVE_OPENAL
		if (sinfo->samplingRate < 44100)
			Resample44Khz(sinfo);
#endif
		V3XA.Client->SmpLoad( sinfo );
    }
    return WT;
}
void NG_AudioReleaseWave(void)
{
	SFX_SampleBatchRelease(g_pFXTable);
	V3XA.Client->Stop();
}
void NG_AudioLoadWave(void)
{
    if (!(V3XA.State & 1))
		return;
	filewad_chdir(FIO_wad, "");
    g_pFXTable = SFX_SampleBatchLoad(g_pSoundList);
	V3XA.Client->Start();
    return;
}

static uint32_t CALLING_STD Thread(void *context)
{
	UNUSED(context);

	while (m_Status)
	{
#ifdef USE_MUTEX
		mutex_lock(&m_Mutex);
#endif
		V3XAStream_Poll(g_pWavStream);
#ifdef USE_MUTEX
		mutex_unlock(&m_Mutex);
#endif
		timer_snooze(75);
	}
	thread_exit(0);
	return 0;
}

void NG_AudioPlayTrack(int i)
{
	char tex[256];
	if (!(V3XA.State & 1))
		return;
#ifdef __BEOS__
     return;
#endif

	SYS_ASSERT(*g_pMusicInfo[i].filename);
	SYS_ASSERT(g_pWavStream == 0);

#ifdef USE_MUTEX
	mutex_init(&m_Mutex);
#endif
    sprintf(tex, "%s\\MUSIC\\%s", ".", g_pMusicInfo[i].filename);
    g_pWavStream = 0;
	V3XAStream_GetFn(&g_pWavStream, tex, i == Ms_INTRO ? FALSE : TRUE);
	if (g_pWavStream)
	{
#ifdef USE_MUTEX
		mutex_lock(&m_Mutex);
#endif
		V3XAStream_Poll(g_pWavStream);
		V3XA.Client->Poll(0);
		NG_AudioSetMusicVolume();
#ifdef USE_MUTEX
		mutex_unlock(&m_Mutex);
#endif
	}

#ifdef USE_THREAD
	m_Thread.hThread = 0;
	m_Thread.pArgument = NULL;
	m_Thread.pFunc = Thread;
	m_Status = 1;
	thread_begin(&m_Thread, SYS_THREAD_PRIORITY_NORMAL);
#endif
}

static int NG_AllocSoundChannel( NG_SAMPLE_AUDIO *info, int index)
{
    int pitch;

    info->playChannel = -1;
	SYS_ASSERT( index>=0 );

    info->smpHandle = g_pFXTable + index;
    info->smpInfo  = g_pSoundList + index;
    info->smpHandle->priority = (uint8_t)info->smpInfo->priority;
    pitch = info->smpInfo->randomPitch+1;
    info->samplingRate = info->smpInfo->samplingRate + sysRand(pitch)-(pitch>>1);

	if (info->playChannel == -1)
		info->playChannel = V3XA.Client->ChannelGetFree(info->smpHandle);

    if (info->playChannel == -1)
		return -1;

	g_ubSampleUsed[info->playChannel] = index;

	info->volume = ((float)g_SGSettings.VolFX) / 100.f;
    return info->playChannel;
}

int NG_AudioPlaySound(int index, float pan)
{
    NG_SAMPLE_AUDIO info;
	if (!(V3XA.State & 1))
		return 0;

	SYS_ASSERT((GX.View.State & GX_STATE_LOCKED) == 0);

    info.playChannel = -1;
    if (NG_AllocSoundChannel(&info, index)>=0)
        V3XA.Client->ChannelPlay(info.playChannel, 44100, info.volume, pan, info.smpHandle);
    return info.playChannel;
}

void NG_AudioStopSound(int playChannel)
{
    if (!(V3XA.State & 1))
		return;

	SYS_ASSERT((GX.View.State & GX_STATE_LOCKED) == 0);

	V3XA.Client->ChannelStop(playChannel);
    g_ubSampleUsed[playChannel] = 0;
}

void NG_AudioKickSound(int playChannel, int index, float pan)
{
    NG_SAMPLE_AUDIO info;
	if (!(V3XA.State & 1))
		return;

    info.playChannel = playChannel;
    if (NG_AllocSoundChannel(&info, index)>=0)
	{
		info.smpHandle->loopend = info.smpHandle->length;
		SYS_ASSERT((GX.View.State & GX_STATE_LOCKED) == 0);
        V3XA.Client->ChannelPlay(info.playChannel, 44100, info.volume, pan, info.smpHandle);
	}
}

static void V3XSND_GetPosition(float *pan, float *volume, V3XVECTOR *v, V3XSCALAR cueDistance)
{
    V3XSCALAR  n;
    V3XVECTOR VV;
    V3XVector_Dif(&VV, v, &V3X.Camera.M.v.Pos);
    n  = V3XVector_Normalize(&VV, &VV);
    // pan : -1 … +1
    *pan = V3XVector_DotProduct(&VV, &V3X.Camera.M.v.I);
    // volume : 0 - 1;
    *volume = n<cueDistance ? 1 : DIVF32( cueDistance, n );
}

int NG_Audio3DUpdate(V3XVECTOR *pos, V3XVECTOR *speed, int playChannel, int index)
{
    float Vol, Pan;
    SYS_ASSERT((GX.View.State&GX_STATE_LOCKED)==0);

    if ((!(V3XA.State & 1))||(playChannel<0))
		return 0;

	if (g_ubSampleUsed[playChannel] != index)
		return 0;

	if (!V3XA.Client->ChannelGetStatus(playChannel))
    {
        NG_AudioStopSound(playChannel);
		return 0;
    }

    if (RLX.Audio.Config&RLXAUDIO_Use3D)
    {
        V3XA.Client->ChannelSetParms(playChannel, pos, speed, NULL);
		return 1;
    }
    else
    {
        float volume;
        V3XSND_GetPosition(&Pan, &volume, pos, 4096.);
        Vol = (volume * (float)g_SGSettings.VolFX) / 100;
		V3XA.Client->ChannelSetVolume(  playChannel, Vol);
		V3XA.Client->ChannelSetPanning( playChannel, Pan );
		return Vol>8.f/100.f;
    }
}

int NG_Audio3DPlay(int index, V3XVECTOR *pos, V3XVECTOR *speed)
{
    NG_SAMPLE_AUDIO info;

	if (!(V3XA.State & 1))
       return 0;

    SYS_ASSERT((GX.View.State&GX_STATE_LOCKED)==0);

    info.playChannel = -1;

    if (NG_AllocSoundChannel(&info, index)>=0)
    {
        if (RLX.Audio.Config & RLXAUDIO_Use3D)
        {
            V3XRANGE range = {1000.f, 100000.f};
            info.smpHandle->sampleFormat|=V3XA_FMT3D;
            V3XA.Client->ChannelPlay(info.playChannel, 44100, 1, 0, info.smpHandle);
            V3XA.Client->ChannelSetParms(info.playChannel, pos, speed, &range);
        }
        else
        {
            V3XSND_GetPosition(&info.pan, &info.volume, pos, 4096.);
            info.volume = (info.volume * (float)g_SGSettings.VolFX) / 100;
            V3XA.Client->ChannelPlay(info.playChannel, 44100, info.volume, info.pan, info.smpHandle);
        }
    }
    else
		info.playChannel = -1;

    return info.playChannel;
}

void NG_AudioSetFreq(int playChannel, int newFrequency)
{
    if (!(V3XA.State & 1))
		return;
    SYS_ASSERT((GX.View.State&GX_STATE_LOCKED)==0);

    if (playChannel>=0)
	   V3XA.Client->ChannelSetSamplingRate(playChannel, newFrequency);
}

void NG_AudioSetMusicVolume(void)
{
    if (!(V3XA.State & 1))
		return;

	if (g_pWavStream)
		V3XAStream_SetVolume(g_pWavStream, 0, ((float)g_SGSettings.VolMusic) / 100.f);

	V3XA.Client->SetVolume(((float)g_SGSettings.VolDIG) / 100.f);
}

void NG_AudioSay(char *voicename)
{
    UNUSED(voicename);
}

void NG_AudioPlayMusic(void)
{
	int track = g_pCurrentGame->episode == 7 ? 7 :  g_SGObjects.World.Track+7;
	if (track<7)
		track=7;
	if (track>17)
		track=7;
	NG_AudioPlayTrack(track);
}

void NG_AudioStopMusic(void)
{
	int i;
	if (!(V3XA.State & 1))
		return;

	if (g_pWavStream)
	{
#ifdef USE_MUTEX
		mutex_lock(&m_Mutex);
		m_Status = 0;
#endif
		for(i=g_SGSettings.VolMusic;i!=0;i--)
		{
			V3XAStream_SetVolume(g_pWavStream, 0, (float)i/100);
			V3XAStream_Poll(g_pWavStream);
			V3XA.Client->Poll(0);
			timer_snooze((64L*12L)/((g_SGSettings.VolMusic*4)+1));
		}

		V3XAStream_Release(g_pWavStream);

#ifdef USE_MUTEX
		mutex_unlock(&m_Mutex);
#endif

		thread_end(&m_Thread);
#ifdef USE_MUTEX
		mutex_destroy(&m_Mutex);
#endif
		g_pWavStream = 0;
	}

	V3XA.Client->ChannelFlushAll(1);
}

void NG_AudioStopTrack(void)
{
	if (!(V3XA.State & 1))
		return;

    NG_AudioStopMusic();
	V3XA.Client->ChannelFlushAll(1);
}

void NG_AudioPauseMusic(void)
{
	int i;
	if (!(V3XA.State & 1))
		return;

    for (i=0;i<RLX.Audio.ChannelToMix;i++)
		V3XA.Client->ChannelStop( i );
}

void NG_AudioResumeMusic(void)
{
	int i;
    if (!(V3XA.State & 1))
		return;

    for (i=0;i<RLX.Audio.ChannelToMix;i++)
		V3XA.Client->ChannelSetVolume( i, ((float)g_SGSettings.VolFX)/100.f );

    sysMemZero(g_ubSampleUsed, 32);
}


void NG_AudioUpdate()
{
    if (!(V3XA.State & 1))
		return;

    V3XA.Client->Poll(0);

#ifndef USE_THREAD
	if (g_pWavStream)
		V3XAStream_Poll(g_pWavStream);
#endif
}

void NG_AudioPlayWarp(void)
{
	V3XA_STREAM pWavStream = 0;
	unsigned int t;

    if ((V3XA.State & 1))
	{
#ifdef USE_MUTEX
		mutex_lock(&m_Mutex);
#endif
		V3XAStream_GetFn(&pWavStream, ".\\MUSIC\\warp03.OGG", FALSE);
		V3XAStream_Poll(pWavStream);
		V3XA.Client->Poll(0);
		V3XAStream_SetVolume(pWavStream, 0, ((float)g_SGSettings.VolMusic)/100.f);
#ifdef USE_MUTEX
		mutex_unlock(&m_Mutex);
#endif
		g_SGGame.FlashAlpha = 0;
	}

	t = timer_ms();

    do
    {
        if (STUB_TaskControl())
			break;

        RGB_Set(g_SGGame.FlashColor, g_SGGame.FlashAlpha, g_SGGame.FlashAlpha, g_SGGame.FlashAlpha);

		GX.Client->Lock();
        NG_RenderView();
        NG_DrawFlash();
        GX.Client->Unlock();
        if ((V3XA.State & 1))
			V3XAStream_PollAll();
        GX.View.Flip();
        g_SGGame.FlashAlpha+=4;
		if (g_SGGame.FlashAlpha>255)
			g_SGGame.FlashAlpha=255;


    }while(timer_ms()<t+5000);

    if (pWavStream)
		V3XAStream_Release(pWavStream);
}

extern SYS_TIMER g_cTimer;

static void CALLING_C TaskFlip(void)
{
	NG_AudioUpdate();
	SYS_ASSERT(ScreenPageFlip);
	ScreenPageFlip();
	timer_Update(&g_cTimer);
}

void NG_InstallHandlers(void)
{
	timer_Start(&g_cTimer, 70, 1);
    if (GX.View.Flip!=TaskFlip)
    {
        ScreenPageFlip = GX.View.Flip;
        GX.View.Flip = TaskFlip;
    }
}

