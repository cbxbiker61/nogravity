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
Linux/SDL Port: 2005 - Matt Williams
*/
//-------------------------------------------------------------------------

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "_rlx32.h"
#include "_rlx.h"
#include "systools.h"
#include "iss_defs.h"

struct channel
{
  int play;
  V3XA_HANDLE *sam;
};

struct stream
{
  int used;
  int play;
  size_t pos;
  ssize_t start;
  ssize_t end;
  int8_t buf[409600];
};

struct sample
{
  int16_t *data;
  Mix_Chunk *chnk;
};

static struct channel g_pchannels[100];
static int g_nchannels = 0;

static struct stream g_stream;

static struct sample g_psamples[100];
static int g_nsamples = 0;

static void ChannelFinished(int channel)
{
  g_pchannels[channel].play = FALSE;
}

static int Enum(void)
{
  return 0;
}

static int Detect(void)
{
  SDL_AudioDriverName(V3XA.Client->s_DrvName, 64);
  return 0;
}

static int Initialize(void *hnd)
{
  // TODO: Check for failures (generally, not just in this file).
  SDL_InitSubSystem(SDL_INIT_AUDIO);
  Mix_OpenAudio(44100, AUDIO_S16, 2, 1024);
  Mix_ChannelFinished(ChannelFinished);
  return 0;
}

static void Release(void)
{
  Mix_HookMusic(NULL, NULL);
  Mix_ChannelFinished(NULL);
  Mix_CloseAudio();
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

static void SetVolume(float volume)
{
  Mix_Volume(-1, volume * MIX_MAX_VOLUME);
}

static void Start(void)
{
  Mix_Resume(-1);
}

static void Stop(void)
{
  Mix_HaltChannel(-1);
}

static int32_t Poll(int32_t v)
{
  return v;
}

void RLXAPI Render(void)
{
  // TODO: Implement.
}

static void RLXAPI UserSetParms(V3XMATRIX *lpMAT,
				V3XVECTOR *lpVEL,
				float *lpDistanceF,
				float *lpDopplerF,
				float *lpRolloffF )
{
  // TODO: Implement.
}

static void ChannelOpen(int nGain, int numChannels)
{
  int channel;
  // TODO: What do I do with nGain?
  g_nchannels = numChannels;
  Mix_AllocateChannels(g_nchannels);
  Mix_ReserveChannels(g_nchannels);
}

static void ChannelSetVolume(int channel, float volume)
{
  Mix_Volume(channel, volume * MIX_MAX_VOLUME);
}

static void ChannelSetPanning(int channel, float pan)
{
  Mix_SetPanning(channel, 127 - 127 * pan, 127 + 127 * pan);
}

static void ChannelSetSamplingRate(int channel, int sampleRate)
{
  // Not supported.
}

static int ChannelPlay(int channel, int sampleRate, float volume, float pan, V3XA_HANDLE *sam)
{
  Mix_HaltChannel(channel);
  g_pchannels[channel].play = TRUE;
  g_pchannels[channel].sam = sam;
  ChannelSetVolume(channel, volume);
  ChannelSetPanning(channel, pan);
  Mix_PlayChannel(channel, g_psamples[sam->sampleID].chnk, (sam->loopend == 0) ? 0 : -1);
  return 0;
}

static void ChannelStop(int channel)
{
  Mix_HaltChannel(channel);
}

static int ChannelGetStatus(int channel)
{
  return Mix_Playing(channel);
}

static void ChannelSetParms(int channel, V3XVECTOR *pos,
			    V3XVECTOR *velocity,
			    V3XRANGE *fRange)
{
  // Not supported.
}

static int ChannelSetEnvironment(V3XA_CHANNEL channel, V3XA_REVERBPROPERTIES *cfg)
{
  // TODO: Implement.
  return 0;
}

static int ChannelGetFree(V3XA_HANDLE *sam)
{
  int channel;
  for (channel = 0; channel < g_nchannels; channel ++)
  {
    if (!g_pchannels[channel].play)
    {
      return channel;
    }
  } 
 return -1;
}

static void ChannelFlushAll(int mode)
{
  Mix_HaltChannel(-1);
}

static void ChannelInvalidate(V3XA_HANDLE *snd)
{
  int channel;
  for (channel = 0; channel < g_nchannels; channel ++)
  {
    if ((g_pchannels[channel].sam == snd) &&
	(g_pchannels[channel].play))
    {
      ChannelStop(channel);
    }
  }
}

static V3XA_HANDLE *ChannelGetSample(int channel)
{
  return g_pchannels[channel].sam;
}

static void StreamFill(void *dummy, Uint8 *stream, int len)
{
  int music_len;
  music_len = (g_stream.end - g_stream.start);
  music_len = (music_len < 0) ? music_len + 409600 : music_len;
  music_len = (music_len > len) ? len : music_len;
  if (g_stream.start + music_len <= 409600)
  {
    memcpy(stream, &g_stream.buf[g_stream.start], music_len);
    g_stream.start += music_len;
  }
  else
  {
    memcpy(stream, &g_stream.buf[g_stream.start], 409600 - g_stream.start);
    memcpy(&stream[409600 - g_stream.start], g_stream.buf, g_stream.start + music_len - 409600);
    g_stream.start = g_stream.start + music_len - 409600;
  }
  g_stream.pos += music_len;
  if (music_len < len)
  {
    memset(&stream[music_len], 0, len - music_len);
  }
}

static void StreamRelease(V3XA_STREAM handle)
{
  g_stream.used = FALSE;
  Mix_HookMusic(NULL, NULL);
}

static V3XA_STREAM StreamInitialize(int sampleFormat, int sampleRate, size_t nSize)
{
  int stream;
  if (!g_stream.used)
  {
    stream = 1;
    g_stream.used = TRUE;
    g_stream.play = TRUE;
    g_stream.pos = 0;
    g_stream.start = 0;
    g_stream.end = 0;
  }
  else
  {
    stream = 0;
  }
  return stream;
}

static int StreamGetChannel(V3XA_STREAM handle)
{
  // TODO: Implement properly.
  return -1;
}

static size_t StreamGetPosition(V3XA_STREAM handle)
{
  return g_stream.pos;
}

static int StreamPoll(V3XA_STREAM handle)
{
  int ret_code = -1;
  int music_len = g_stream.end - g_stream.start;
  music_len = (music_len < 0) ? music_len + 409600 : music_len;
  if (music_len < 40960)
  {
    ret_code = 0;
  }
  return ret_code;
}

static void StreamSetVolume(V3XA_STREAM handle, float volume)
{
  // TODO: Implement.
}

static int StreamLoad(V3XA_STREAM handle, void *data, size_t size)
{
  int ret_code = -1;
  int music_len = g_stream.start - g_stream.end - 1;
  music_len = (music_len < 0) ? music_len + 409600 : music_len;
  if (music_len >= size)
  {
    if (g_stream.end + size < 409600)
    {
      memcpy(&g_stream.buf[g_stream.end], data, size);
      g_stream.end += size;
    }
    else
    {
      memcpy(&g_stream.buf[g_stream.end], data, 409600 - g_stream.end);
      memcpy(g_stream.buf, &((int8_t *)data)[409600 - g_stream.end], g_stream.end + size - 409600);
      g_stream.end = g_stream.end + size - 409600;
    }
    music_len = g_stream.end - g_stream.start;
    music_len = (music_len < 0) ? music_len + 409600 : music_len;
    if ((g_stream.play) &&
        (music_len >= 40960))
    {
      Mix_HookMusic(StreamFill, NULL);
    }
    ret_code = 0;
  }
  return ret_code;
}

static int StreamStart(V3XA_STREAM handle)
{
  Mix_HookMusic(StreamFill, NULL);
  return 0;
}

static void StreamStop(V3XA_STREAM handle)
{
  Mix_HookMusic(NULL, NULL);
}

static int SmpLoad(V3XA_HANDLE *sam)
{
  int index;
  int bytes_per_sample;
  // TODO: Implement a better allocation system than this.
  sam->sampleID = g_nsamples;
  bytes_per_sample = ((sam->sampleFormat & V3XA_FMTSTEREO) ? 2 : 1) * ((sam->sampleFormat & V3XA_FMT16BIT) ? 2 : 1);
  if ((sam->sampleFormat & V3XA_FMTSTEREO) &&
      (sam->sampleFormat & V3XA_FMT16BIT))
  {
    g_psamples[g_nsamples].data = NULL;
    g_psamples[g_nsamples].chnk = Mix_QuickLoad_RAW(sam->sample, sam->length);
  }
  else
  {
    // TODO: Support other formats.
    g_psamples[g_nsamples].data = (int16_t *)MM_std.malloc(sam->length * 2);
    for (index = 0; index < sam->length / sizeof(int16_t); index ++)
    {
      g_psamples[g_nsamples].data[index * 2] = ((int16_t *)sam->sample)[index];
      g_psamples[g_nsamples].data[index * 2 + 1] = ((int16_t *)sam->sample)[index];
    }
    g_psamples[g_nsamples].chnk = Mix_QuickLoad_RAW((Uint8 *)g_psamples[g_nsamples].data, sam->length * 2);
  }
  g_nsamples ++;
  return 0;
}

static void SmpRelease(V3XA_HANDLE *sam)
{
  Mix_FreeChunk(g_psamples[sam->sampleID].chnk);
  if (g_psamples[sam->sampleID].data != NULL)
  {
    MM_std.free(g_psamples[sam->sampleID].data);
  }
}

void RLXAPI V3XA_EntryPoint(struct RLXSYSTEM *rlx)
{
  static V3XA_WaveClientDriver SDLMixer_Client =
  {
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

    "SDL"
  };

  V3XA.Client = &SDLMixer_Client;

  return;
}

