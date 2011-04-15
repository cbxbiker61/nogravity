#include <stdio.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "systools.h"
#include "iss_defs.h"

// TODO: Implement audio driver.

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
  return 0;
}

static void Release(void)
{
}

static void SetVolume(float volume)
{
}

static void Start(void)
{
}

static void Stop(void)
{
}

static int32_t Poll(int32_t v)
{
  return v;
}

void RLXAPI Render(void)
{
}

static void RLXAPI UserSetParms(V3XMATRIX *lpMAT,
				V3XVECTOR *lpVEL,
				float *lpDistanceF,
				float *lpDopplerF,
				float *lpRolloffF )
{
}

static void ChannelOpen(int nGain, int numChannels)
{
}

static int ChannelPlay(int channel, int sampleRate, float volume, float pan, V3XA_HANDLE *sam)
{
  return 0;
}

static void ChannelStop(int channel)
{
}

static void ChannelSetVolume(int channel, float volume)
{
}

static void ChannelSetPanning(int channel, float pan)
{
}

static void ChannelSetSamplingRate(int channel, int sampleRate)
{
}

static int ChannelGetStatus(int channel)
{
  return 1;
}

static void ChannelSetParms(int channel, V3XVECTOR *pos,
			 V3XVECTOR *velocity,
			 V3XRANGE *fRange
			   )
{
}

static int ChannelSetEnvironment(V3XA_CHANNEL channel, V3XA_REVERBPROPERTIES *cfg)
{
  return 0;
}

static int ChannelGetFree(V3XA_HANDLE *sam)
{
  return 0;
}

static void ChannelFlushAll(int mode)
{
}

static void ChannelInvalidate(V3XA_HANDLE *snd)
{
}

static V3XA_HANDLE *ChannelGetSample(int channel)
{
  return NULL;
}

static V3XA_STREAM GetFreeStreamHandle()
{
  return -1;
}

static void StreamRelease(V3XA_STREAM handle)
{
}

static V3XA_STREAM StreamInitialize(int sampleFormat, int sampleRate, size_t nSize)
{
  return -1;
}

static int StreamGetChannel(V3XA_STREAM handle)
{
  return -1;
}

static size_t StreamGetPosition(V3XA_STREAM handle)
{
  return 0;
}

static int StreamPoll(V3XA_STREAM handle)
{
  return -1;
}

static void StreamSetVolume(V3XA_STREAM handle, float volume)
{
}

static int StreamLoad(V3XA_STREAM handle, void *data, size_t size)
{
  return 0;
}

static int StreamStart(V3XA_STREAM handle)
{
  return 0;
}

static void StreamStop(V3XA_STREAM handle)
{
}

static int SmpLoad(V3XA_HANDLE *sam)
{
  return 1;
}

static void SmpRelease(V3XA_HANDLE *sam)
{
}

void RLXAPI V3XA_EntryPoint(struct RLXSYSTEM *rlx)
{
  static V3XA_WaveClientDriver CoreAudio_Client =
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
    SmpRelease
  };

  V3XA.Client = &CoreAudio_Client;
}

