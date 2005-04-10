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

#ifdef __MACH__
#include "al.h"
#include "alc.h"

#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#include "_rlx32.h"
#include "_rlx.h"
#include "systools.h"
#include "iss_defs.h"

struct channel
{
  ALuint src;
  int play;
  int stream;
  V3XA_HANDLE *sam;
};

#define MAX_AL_BUFFERS 256

struct stream
{
  int used;
  int play;
  int chan;
  ALint fmt;
  int rate;
  size_t pos;
  int first;
  int last;
  ALuint bufs[MAX_AL_BUFFERS];
};

static struct channel *g_pchannels;
static int g_nchannels = 0;
static struct stream *g_pstreams;
static int g_nstreams = 0;
static int g_nbufsperstream = MAX_AL_BUFFERS;

static int Enum(void)
{
  return 0;
}

static int Detect(void)
{
  return 0;
}

static int Initialize(void *hnd)
{
  ALCdevice *dev;
  ALCcontext *ctxt;
  ALCint attrs[3];
  // TODO: Check for failures (generally, not just in this file).
  dev = alcOpenDevice(NULL);
  attrs[0] = ALC_FREQUENCY;
  attrs[1] = RLX.Audio.SR_to_Khz[RLX.Audio.SamplingRate];
  attrs[2] = 0;
  ctxt = alcCreateContext(dev, (ALCint *)&attrs);
  alcMakeContextCurrent(ctxt);
  alDistanceModel(AL_INVERSE_DISTANCE);

  RLX.Audio.Config|=RLXAUDIO_Use3D;
  return 0;
}

static void Release(void)
{
  ALCcontext *ctxt;
  ALCdevice *dev;
  if (g_pchannels != NULL)
  {
    MM_std.free(g_pchannels);
    g_pchannels = NULL;
    g_nchannels = 0;
  }
  if (g_pstreams != NULL)
  {
    MM_std.free(g_pstreams);
    g_pstreams= NULL;
    g_nstreams = 0;
  }
  ctxt = alcGetCurrentContext();
  if (ctxt != NULL)
  {
    dev = alcGetContextsDevice(ctxt);
    alcDestroyContext(ctxt);
    alcCloseDevice(dev);
  }
}

static void SetVolume(float volume)
{
  alListenerf(AL_GAIN, volume);
}

static void Start(void)
{
  ALCcontext *ctxt;
  ctxt = alcGetCurrentContext();
  alcProcessContext(ctxt);
}

static void Stop(void)
{
  ALCcontext *ctxt;
  ctxt = alcGetCurrentContext();
  alcSuspendContext(ctxt);
}

static int32_t Poll(int32_t v)
{
  int channel;
  ALint status;
  for (channel = 0; channel < g_nchannels; channel ++)
  {
    if (g_pchannels[channel].play)
    {
      alGetSourcei(g_pchannels[channel].src, AL_SOURCE_STATE, &status);
      if (status != AL_PLAYING)
      {
	g_pchannels[channel].play = FALSE;
      }
    }
  }
  return v;
}

void RLXAPI Render(void)
{
  // Do nothing.
}

V3XVECTOR g_ListenerPos;
static void RLXAPI UserSetParms(V3XMATRIX *lpMAT,
				V3XVECTOR *lpVEL,
				float *lpDistanceF,
				float *lpDopplerF,
				float *lpRolloffF )
{
  int channel;
  if (lpMAT != NULL)
  {
    alListenerfv(AL_ORIENTATION, (ALfloat *)&lpMAT->v.I);
    alListenerfv(AL_POSITION, (ALfloat *)&lpMAT->v.Pos);
	g_ListenerPos = lpMAT->v.Pos;
  }
  if (lpVEL != NULL)
  {
    alListenerfv(AL_VELOCITY, (ALfloat *)lpVEL);
  }
  if (lpDistanceF != NULL)
  {
    for (channel = 0; channel < g_nchannels; channel ++)
    {
      alSourcef(g_pchannels[channel].src, AL_REFERENCE_DISTANCE, (ALfloat)*lpDistanceF);
    }
  }
  if (lpDopplerF != NULL)
  {
    alDopplerFactor(*lpDopplerF);
  }
  if (lpRolloffF != NULL)
  {
    for (channel = 0; channel < g_nchannels; channel ++)
    {
      alSourcef(g_pchannels[channel].src, AL_ROLLOFF_FACTOR, (ALfloat)*lpRolloffF);
    }
  }
}

static void ChannelOpen(int nGain, int numChannels)
{
  int idx;
  // TODO: What do I do with nGain?
  numChannels = 32;
  g_nchannels = numChannels;
  g_nstreams = numChannels;

  g_pchannels = (struct channel *)MM_std.malloc(g_nchannels * sizeof(struct channel));
  memset(g_pchannels, 0, g_nchannels * sizeof(struct channel));
  for (idx = 0; idx < g_nchannels; idx ++)
  {
    alGenSources(1, &g_pchannels[idx].src);
  }

  g_pstreams = (struct stream *)MM_std.malloc(g_nstreams * sizeof(struct stream));
  memset(g_pstreams, 0, g_nstreams * sizeof(struct stream));
}

static void ChannelSetVolume(int channel, float volume)
{
  alSourcef(g_pchannels[channel].src, AL_GAIN, volume);
}

static void ChannelSetPanning(int channel, float pan)
{

  alSourcefv(g_pchannels[channel].src, AL_POSITION, (ALfloat *)&g_ListenerPos);
}

static void ChannelSetSamplingRate(int channel, int sampleRate)
{
  alSourcef(g_pchannels[channel].src, AL_PITCH, 44100.0 / sampleRate);
}

static void ChannelSetParms(int channel, V3XVECTOR *pos, V3XVECTOR *velocity, V3XRANGE *fRange)
{
  // TODO: Get this working.
  if (pos)
  alSourcefv(g_pchannels[channel].src, AL_POSITION, (ALfloat *)pos);
  if (velocity)
  alSourcefv(g_pchannels[channel].src, AL_VELOCITY, (ALfloat *)velocity);
  if (fRange != NULL)
  {
    // TODO: Pass minimum distance into OpenAL.
    alSourcef(g_pchannels[channel].src, AL_MAX_DISTANCE, fRange->max);
  }
}

static int ChannelPlay(int channel, int sampleRate, float volume, float pan, V3XA_HANDLE *sam)
{
  static const V3XVECTOR vector = {0.0, 0.0, 0.0};
  static const V3XRANGE range = {0.0, 10000.0};
  g_pchannels[channel].sam = sam;
  g_pchannels[channel].play = TRUE;
  alSourcei(g_pchannels[channel].src, AL_BUFFER, (ALuint)sam->sampleID);
  ChannelSetParms(channel, (V3XVECTOR *)&vector, (V3XVECTOR *)&vector, (V3XRANGE *)&range);
  ChannelSetSamplingRate(channel, sampleRate);
  ChannelSetVolume(channel, volume);
  ChannelSetPanning(channel, pan);
  // TODO: Implement looping as suggested by interface (e.g. pay attention to loopstart).
  if (sam->loopend != 0)
  {
    alSourcei(g_pchannels[channel].src, AL_LOOPING, AL_TRUE);
  }
  else
  {
    alSourcei(g_pchannels[channel].src, AL_LOOPING, AL_FALSE);
  }
  alSourcePlay(g_pchannels[channel].src);
  return 0;
}

static void ChannelStop(int channel)
{
  alSourceStop(g_pchannels[channel].src);
}

static int ChannelGetStatus(int channel)
{
  return g_pchannels[channel].play;
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
    if ((!g_pchannels[channel].play) &&
        (!g_pchannels[channel].stream))
    {
      break;
    }
  }
  if (channel >= g_nchannels)
  {
    channel = -1;
  }
  return channel;
}

static void ChannelFlushAll(int mode)
{
  int channel;
  for (channel = 0; channel < g_nchannels; channel ++)
  {
    if (ChannelGetStatus(channel))
    {
      ChannelStop(channel);
    }
  }
}

static void ChannelInvalidate(V3XA_HANDLE *sam)
{
  int channel;
  for (channel = 0; channel < g_nchannels; channel ++)
  {
    if ((g_pchannels[channel].sam == sam) &&
	(ChannelGetStatus(channel)))
    {
      ChannelStop(channel);
    }
  }
}

static V3XA_HANDLE *ChannelGetSample(int channel)
{
  return g_pchannels[channel].sam;
}

static void StreamRelease(V3XA_STREAM handle)
{
  ALuint buf;
  alSourceStop(g_pchannels[g_pstreams[handle].chan].src);
  while (g_pstreams[handle].first != g_pstreams[handle].last)
  {
    alSourceUnqueueBuffers(g_pchannels[g_pstreams[handle].chan].src, 1, &g_pstreams[handle].bufs[g_pstreams[handle].first]);
    alDeleteBuffers(1, &g_pstreams[handle].bufs[g_pstreams[handle].first]);
    g_pstreams[handle].first ++;
    if (g_pstreams[handle].first > g_nbufsperstream)
    {
      g_pstreams[handle].first = 0;
    }
  }
  g_pchannels[g_pstreams[handle].chan].stream = FALSE;
  g_pstreams[handle].play = FALSE;
  g_pstreams[handle].used = FALSE;
}

static void StreamReset(int stream)
{
    static const V3XVECTOR vector = {0.0, 0.0, 0.0};
    static const V3XRANGE range = {0.0, 10000.0};
 
	g_pstreams[stream].used = TRUE;
	g_pstreams[stream].play = TRUE;
	g_pstreams[stream].pos = 0;
	g_pstreams[stream].first = 0;
	g_pstreams[stream].last = 0;
	g_pchannels[g_pstreams[stream].chan].stream = TRUE;
	ChannelSetSamplingRate(g_pstreams[stream].chan, g_pstreams[stream].rate);
	ChannelSetVolume(g_pstreams[stream].chan, 1.0);
	ChannelSetPanning(g_pstreams[stream].chan, 0.0);
	ChannelSetParms(g_pstreams[stream].chan, (V3XVECTOR *)&vector, (V3XVECTOR *)&vector, (V3XRANGE *)&range);
	alSourcei(g_pchannels[g_pstreams[stream].chan].src, AL_LOOPING, AL_FALSE);
 
}

static V3XA_STREAM StreamInitialize(int sampleFormat, int sampleRate, size_t size)
{
  int stream;
  int channel;
  for (stream = 0; stream < g_nstreams ; stream ++)
  {
    if (!g_pstreams[stream].used)
    {
        break;
    }
  }
  if (stream < g_nstreams)
  {
    channel = 31;
    if (channel != -1)
    {
	   g_pstreams[stream].chan = channel;
	   g_pstreams[stream].fmt = (sampleFormat & V3XA_FMTSTEREO) ?  ((sampleFormat & V3XA_FMT16BIT) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16) : ((sampleFormat & V3XA_FMT16BIT) ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8);
   	   g_pstreams[stream].rate = sampleRate;

   	   StreamReset(stream);
	}
    else
    {
      stream = -1;
    }
  }
  else
  {
    stream = -1;
  }
  return stream;
}

static int StreamGetChannel(V3XA_STREAM handle)
{
  return g_pstreams[handle].chan;
}

static size_t StreamGetPosition(V3XA_STREAM handle)
{
  return g_pstreams[handle].pos;
}

static int StreamPoll(V3XA_STREAM handle)
{
  ALint processed;
  ALint queued;
  ALint size;
  ALint status;
  int ret_code = -1;
  if (!g_pstreams[handle].used)
  {
      return -1;
  }

   ChannelSetParms(g_pstreams[handle].chan, &g_ListenerPos, NULL, NULL);
  
   alGetSourcei(g_pchannels[g_pstreams[handle].chan].src, AL_BUFFERS_PROCESSED, &processed);
   while (processed > 0)
   {
    alSourceUnqueueBuffers(g_pchannels[g_pstreams[handle].chan].src, 1, &g_pstreams[handle].bufs[g_pstreams[handle].first]);
    alGetBufferi(g_pstreams[handle].bufs[g_pstreams[handle].first], AL_SIZE, &size);
    g_pstreams[handle].pos += size;
    alDeleteBuffers(1, &g_pstreams[handle].bufs[g_pstreams[handle].first]);
    g_pstreams[handle].first ++;
    if (g_pstreams[handle].first > g_nbufsperstream)
    {
      g_pstreams[handle].first = 0;
    }
    processed --;
   }
  if (((g_pstreams[handle].last >= g_pstreams[handle].first) &&
       (g_pstreams[handle].last - g_pstreams[handle].first <= 20)) ||
      ((g_pstreams[handle].last < g_pstreams[handle].first) &&
       (g_pstreams[handle].last + g_nbufsperstream - g_pstreams[handle].first <= 20)))
  {
    ret_code = 0;
  }
  return ret_code;
}

static void StreamSetVolume(V3XA_STREAM handle, float volume)
{
  ChannelSetVolume(g_pstreams[handle].chan, volume);
}

static int StreamLoad(V3XA_STREAM handle, void *data, size_t size)
{
  ALint status;
  ALint queued;
  int ret_code = -1;
  if ((g_pstreams[handle].last != g_pstreams[handle].first - 2) &&
      (g_pstreams[handle].last - g_nbufsperstream != g_pstreams[handle].first - 2))
  {
    alGenBuffers(1, &g_pstreams[handle].bufs[g_pstreams[handle].last]);
    alBufferData(g_pstreams[handle].bufs[g_pstreams[handle].last], g_pstreams[handle].fmt, data, size, g_pstreams[handle].rate);
    alSourceQueueBuffers(g_pchannels[g_pstreams[handle].chan].src, 1, &g_pstreams[handle].bufs[g_pstreams[handle].last]);
    g_pstreams[handle].last ++;
    if (g_pstreams[handle].last > g_nbufsperstream)
    {
      g_pstreams[handle].last = 0;
    }
    if ((g_pstreams[handle].play) &&
        (((g_pstreams[handle].last >= g_pstreams[handle].first) &&
          (g_pstreams[handle].last - g_pstreams[handle].first > 2)) ||
         ((g_pstreams[handle].last < g_pstreams[handle].first) &&
          (g_pstreams[handle].last + g_nbufsperstream - g_pstreams[handle].first > 2))))
    {
      alGetSourcei(g_pchannels[g_pstreams[handle].chan].src, AL_SOURCE_STATE, &status);
      if (status != AL_PLAYING)	
      {
        alSourcePlay(g_pchannels[g_pstreams[handle].chan].src);
      }
    }
    ret_code = 0;
  }
  return ret_code;
}

static int StreamStart(V3XA_STREAM handle)
{
  g_pstreams[handle].play = TRUE;
  alSourcePlay(g_pchannels[g_pstreams[handle].chan].src);
  return 0;
}

static void StreamStop(V3XA_STREAM handle)
{
  g_pstreams[handle].play = FALSE;
  alSourceStop(g_pchannels[g_pstreams[handle].chan].src);
}

static int SmpLoad(V3XA_HANDLE *sam)
{
  alGenBuffers(1, (ALuint *)&sam->sampleID);
  alBufferData((ALuint)sam->sampleID, (sam->sampleFormat & V3XA_FMTSTEREO) ?  ((sam->sampleFormat & V3XA_FMT16BIT) ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8) : ((sam->sampleFormat & V3XA_FMT16BIT) ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8), sam->sample, sam->length, sam->samplingRate);
  return 0;
}

static void SmpRelease(V3XA_HANDLE *sam)
{
  alDeleteBuffers(1, (ALuint *)&sam->sampleID);
}

void RLXAPI V3XA_EntryPoint(struct RLXSYSTEM *rlx)
{
  static V3XA_WaveClientDriver OpenAL_Client =
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

    "OpenAL"
  };

  V3XA.Client = &OpenAL_Client;

  return;
}
