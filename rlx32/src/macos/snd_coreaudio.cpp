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

#include <stdio.h>
#include <pthread.h>

uint32_t g_pThreadMutex;

#include <CoreAudio/AudioHardware.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>



#include "_rlx32.h"
#include "systools.h"
#include "iss_defs.h"

#define DSH_PLAYING  1
#define DSH_RESERVED 2
#define DSH_COPY     4
#define DSH_LOOPED   8
#define DSH_SINGLE   0


#ifdef _DEBUG
static int PrintDebug(int err, const char *szFile, int line)
{
    if (err)
        printf("%s(%d) : ERR%d\n", szFile, line, err);
    return err;
}
#define AUDIO_CHECK(x) PrintDebug(x, __FILE__, __LINE__)
#else
#define AUDIO_CHECK(x) x
#endif

// Audio Unit Sound Buffer
class auSoundBuffer
{

    public:
    	int						Create(int sampleFormat, int sampleRate, int length, bool b);
    	int						Release();
    	int						GetPlayPosition(int32_t	*lpPos);
    	int						Play(int32_t option);
    	int						Stop();
    	int						Rewind();
    	int						WriteSample(int	offset,	void *data,	int	size);
    	int						SetPanValues(int v,	float *v);
    	int						SetGain(float gain);
    	int						SetPriority(float priority);
    	int						SetSampleRate(int sampleRate);
    	int						GetStatus();
    	int						Lock();
    	int						Unlock();
    	int						DuplicateSource(auSoundBuffer *src);
    
    	u_int32_t	    			m_nIndex;		 //	channel
    	volatile u_int32_t			m_nFlags;		 //	flags
    	int32_t					m_nNumFrame;	 //	number of frames
    	int32_t					m_nCurFrame;	 //	current	write position
		int32_t					m_nPlayPosition; // current play position
		
    	float			    	m_fPan[2];		 //	pan	values
    	float			     	m_fGain;		 //	gain values
    	int32_t					m_nSampleRate;	 //	sample rate
    
    	float		      		m_fPriority;	 //	priority
    	float		        *   m_pData;		 //	sample data
    	V3XA_HANDLE		    *	m_pSample;		 //	sample ID
    	int						m_nInstance;
};

static auSoundBuffer		g_pHandle[MAX_V3XA_AUDIO_MIX];

typedef struct
{
    V3XA_CHANNEL			nChannel;
    V3XA_HANDLE				sample;

	int 					nState;

	int32_t					m_nWritePosition,
							m_nPreviousPosition;

	int32_t					m_nWriteBytes,
							m_nTotalBytes,
							m_nUploadedBytes;
	u_int32_t		    	m_nStreamState;
}auStreamBuffer;

static auStreamBuffer       g_pStream[MAX_V3XA_AUDIO_STREAM];

typedef struct AudioUnitInputCallback	AudioUnitInputCallback;
typedef struct AudioUnitConnection		AudioUnitConnection;

static AudioUnit	        g_AuMixer;
static AudioUnit            g_AuReverb;
static AudioUnit            g_AuOutput;
static float                g_fGain = 1.f;

static int 					theNumberDevices;
static AudioDeviceID 	*	theDeviceList;
#include <sys/sysctl.h>
#include <ppc_intrinsics.h>

// File buffer with 32bit float
static bool HasAltivec;

static bool OSX_HasAltivec (void)
{
    int result = 0;
    int selectors[2] = { CTL_HW, HW_VECTORUNIT };
    size_t length = sizeof(result);
    sysctl(selectors, 2, &result, &length, NULL, 0);
	return !!result;
}

// Set single float across the four values
static inline vector float vec_load_ps1(const float *pF )
{
    vector float data = vec_lde(0, pF);
    return vec_splat(vec_perm(data, data, vec_lvsl(0, pF)), 0);
}
// Set vector to 0
static inline const vector float vec_setzero()
{
    return (const vector float) (0.);
}


static void FillBufferF32Stereo(const float *lpSrc,
                                float *lpDst,
                                float fVolLeft,
                                float fVolRight,
                                int sz,
                                int offset,
								int n
                                )
{
    int			i;
	lpSrc+=offset;
  	if ((fVolLeft==1) && (fVolRight==1)) 
    {
		sysMemCpy(lpDst, lpSrc, sz * sizeof(float));
		return;
	}
	else
    if ((fVolLeft==0) && (fVolRight==0)) {

		sysMemZero(lpDst, sz * sizeof(float));
		return;
    }

    int remain = sz;

    if (HasAltivec)
    {
        const vector float gain_l = vec_load_ps1(&fVolLeft);
        const vector float gain_r = vec_load_ps1(&fVolRight);
        const vector float gain = vec_mergeh(gain_l, gain_r);
        const vector float mix = vec_setzero();
        remain = sz & 15;
       	for (i=0;i<sz;i+=16)
    	{
    		vector float v0 = vec_ld(0x0, lpSrc + i);
    		vector float v1 = vec_ld(0x10, lpSrc + i);
    		vector float v2 = vec_ld(0x20, lpSrc + i);
    		vector float v3 = vec_ld(0x30, lpSrc + i);

    		vector float v4 = vec_madd(v0, gain, mix);
    		vector float v5 = vec_madd(v1, gain, mix);
    		vector float v6 = vec_madd(v2, gain, mix);
    		vector float v7 = vec_madd(v3, gain, mix);

    		vec_st(v4, 0x0, lpDst + i);
    		vec_st(v5, 0x10, lpDst + i);
    		vec_st(v6, 0x20, lpDst + i);
    		vec_st(v7, 0x30, lpDst + i);
        }
    }

	for (;remain;i+=2,remain--)
	{
		lpDst[i  ]=lpSrc[i  ]*fVolLeft;
		lpDst[i+1]=lpSrc[i+1]*fVolRight;
	}

}

static void FillBufferF32Mono(const float *lpSrc,
                                float *lpDst,
                                float fVolLeft,
                                float fVolRight,
                                int sz,
								int offset,
                                int n)
{
   	lpSrc+=offset;
    if ((fVolLeft==0) && (fVolRight==0)) {

		sysMemZero(lpDst, sz * sizeof(float) << 1);
		return;
    }
	int remain = sz;
	int i = 0, j = 0;

	if ((fVolLeft==1) && (fVolRight==1))
	{
	    if (HasAltivec)
	    {
     	    remain = sz & 3;
    	    for (;i<sz;i+=4,j+=8)
    		{
    			vector float v0 = vec_ld(0, lpSrc + i); // v0(s0,s1,s2,s3)
    			vector float v2 = vec_mergeh(v0, v0);
    			vector float v3 = vec_mergel(v0, v0);
    			vec_st(v2, 0, lpDst + j);
    			vec_st(v3, 0x10, lpDst + j);
    	    }
    	    sz = remain;
    	}

        for (i=0;i<remain;i++,j+=2)
		{
            lpDst[j+0]=*lpSrc;
            lpDst[j+1]=*lpSrc;
            lpSrc++;
        }
        return;
    }

    if (HasAltivec)
    {
      const vector float gain_l = vec_load_ps1(&fVolLeft);
      const vector float gain_r = vec_load_ps1(&fVolRight);
      const vector float gain = vec_mergeh(gain_l, gain_r);
      const vector float mix = vec_setzero();


       for (;i<sz;i+=4,j+=8)
       {
    		vector float v0 = vec_ld(0, lpSrc + i); // v0(s0,s1,s2,s3)
    		vector float v1 = vec_madd(v0, gain, mix);
    		vector float v2 = vec_mergeh(v1, v1);
    		vector float v3 = vec_mergel(v1, v1);
    		vec_st(v2, 0, lpDst + j);
    		vec_st(v3, 0x10, lpDst + j);
       }
       sz = remain;
    }

    for (i=0;i<sz;i++,j+=2)
	{
        lpDst[j]=(*lpSrc)*fVolLeft;
        lpDst[j]=(*lpSrc)*fVolRight;
        lpSrc++;
    }
}

static OSStatus snd_input_callback(void *inRefCon,
                                   AudioUnitRenderActionFlags inActionFlags,
                                   const AudioTimeStamp *inTimeStamp,
                                   UInt32 inBusNumber,
                                   AudioBuffer *ioData)
{
    int					 bufsz, writesz,
                         m_nCurFrame, nEndFrame, m_nNumFrame;
    float			 	 fVol[2];
    float               *m_pData;
    auSoundBuffer		*chan;

    chan = &g_pHandle[(int)inRefCon];

    // get the proper channel for this audio unit
	chan->Lock();
	
	m_nCurFrame = chan->m_nCurFrame;
    m_nNumFrame = chan->m_nNumFrame;

	fVol[0] = chan->m_fGain * chan->m_fPan[0] * g_fGain;
    fVol[1] = chan->m_fGain * chan->m_fPan[1] * g_fGain;

    // the buffer
    bufsz = ioData->mDataByteSize / sizeof(float);
    m_pData = (float*)ioData->mData;

    int isStereo = chan->m_pSample->sampleFormat & V3XA_FMTSTEREO;
    if (!isStereo)
        bufsz>>=1;
    // will be converting mono to stereo
    // is this one-shot and is the buffer over

    if (m_nCurFrame>=m_nNumFrame) {
        if (!(chan->m_nFlags & DSH_LOOPED)) {
            chan->Stop();
            goto end;
        }
        m_nCurFrame -= m_nNumFrame;
    }

    // how much buffer can we write

    nEndFrame = m_nCurFrame + bufsz;
    if (nEndFrame > m_nNumFrame) {
        writesz = nEndFrame-m_nNumFrame;
        chan->m_nCurFrame = (chan->m_nFlags & DSH_LOOPED) ? 0 : m_nNumFrame;
    }
    else {
        writesz=bufsz;
        chan->m_nCurFrame = m_nCurFrame + bufsz;
    }

    // fill the buffer

    if (isStereo)
	FillBufferF32Stereo(
             chan->m_pData,
		     m_pData,
             fVol[0],
             fVol[1],
             writesz,
             m_nCurFrame,
             m_nNumFrame);
    else
	FillBufferF32Mono(
             chan->m_pData,
             m_pData,
             fVol[0],
             fVol[1],
             writesz,
             m_nCurFrame,
             m_nNumFrame);
  
	chan->m_nPlayPosition = m_nCurFrame + writesz;
    m_pData = m_pData + writesz;

   // if looping, fill any remaining buffer

    if (!(chan->m_nFlags & DSH_LOOPED))
       goto end;

    writesz = bufsz - writesz;
    if (writesz <= 0)
	   goto end;

    if (isStereo)
    FillBufferF32Stereo(chan->m_pData,
           m_pData,
           fVol[0],
           fVol[1],
           writesz,
           0,
           m_nNumFrame);		// start over, looping

    else
    FillBufferF32Mono(chan->m_pData,
            m_pData,
            fVol[0],
            fVol[1],
            writesz,
            0,
            m_nNumFrame);		// start over, looping

     chan->m_nPlayPosition = writesz;

    writesz = bufsz - writesz;
    if (writesz <= 0)
	   goto end;
end:
    
	chan->Unlock();
	
	return noErr;
}

// Driver poll
static int32_t CALLING_C Poll(int32_t v)
{
	return v;
}

// Enum. Devices
static int Enum(void)
{
    UInt32 theSize;
    OSErr theStatus;

    // Get the size of the list
    theStatus = AudioHardwareGetPropertyInfo(
                kAudioHardwarePropertyDevices,
                &theSize, NULL);

    theNumberDevices = theSize / sizeof(AudioDeviceID);

    // Allocate enough space to hold the list
    theDeviceList = (AudioDeviceID*) malloc ( theNumberDevices * sizeof(AudioDeviceID));

    // Get the device list
    theSize = theNumberDevices * sizeof(AudioDeviceID);

    AUDIO_CHECK(AudioHardwareGetProperty(
                kAudioHardwarePropertyDevices,
                &theSize, theDeviceList));
	return 1;
}

// Stop audio
static void Stop(void)
{
    if (g_AuOutput!=NULL) {
        AUDIO_CHECK(AudioOutputUnitStop(g_AuOutput));
    }
    return;
}

// Start audio
static void Start(void)
{
    if (g_AuOutput!=NULL) {
        AUDIO_CHECK(AudioOutputUnitStart(g_AuOutput));
    }
    return;
}

// Detect driver
static int Detect(void)
{
	return 0;
}

// Release driver
static void Release(void)
{
    int						i;
    auSoundBuffer		*chan;
    // Disconnect sounds

    chan = g_pHandle;
    for ((i=0);(i!=MAX_V3XA_AUDIO_MIX);i++) {
        chan->Release();
        chan++;
    }

    // Release audio units

    if (g_AuOutput!=NULL) {
        AUDIO_CHECK(AudioUnitUninitialize(g_AuOutput));
        CloseComponent(g_AuOutput);
    }
    if (g_AuReverb!=NULL) {
        AUDIO_CHECK(AudioUnitUninitialize(g_AuReverb));
        CloseComponent(g_AuReverb);
    }
    if (g_AuMixer!=NULL) {
        AUDIO_CHECK(AudioUnitUninitialize(g_AuMixer));
        CloseComponent(g_AuMixer);
    }

	return;
}


// Initialise driver
static int Initialize(void *hwnd)
{
	int							i, err;
    AudioUnitConnection			auconnect;
    auSoundBuffer			*	chan;
	ComponentDescription		compdesc;
    Component					compid;

	HasAltivec = OSX_HasAltivec();
    // Clear the sound channels

    chan = g_pHandle;
    for ((i=0);(i!=MAX_V3XA_AUDIO_MIX);i++) {
        chan->m_nIndex = i;
      	chan->m_fPan[0] = chan->m_fPan[1] = chan->m_fGain = 1.f;
      	chan->m_pData = 0;
        chan->m_nInstance = 0;
        chan++;
    }

    g_AuMixer = g_AuReverb = g_AuOutput=NULL;

    // Open the mixer unit

    compdesc.componentType = kAudioUnitComponentType;
    compdesc.componentSubType = kAudioUnitSubType_Mixer;
    compdesc.componentManufacturer = kAudioUnitID_StereoMixer;
    compdesc.componentFlags = 0;
    compdesc.componentFlagsMask = 0;
    compid = FindNextComponent(NULL, &compdesc);
    if (compid==NULL) {
        Release();
        return -1;
    }
    g_AuMixer=(AudioUnit)OpenComponent(compid);
    if (g_AuMixer==NULL) {
        Release();
        return -2;
    }

    // Open the reverb unit

    compdesc.componentType = kAudioUnitComponentType;
    compdesc.componentSubType = kAudioUnitSubType_Effect;
    compdesc.componentManufacturer = kAudioUnitID_MatrixReverb;
    compdesc.componentFlags = 0;
    compdesc.componentFlagsMask = 0;
    compid = FindNextComponent(NULL, &compdesc);
    if (compid==NULL) {
        Release();
        return -3;
    }
    g_AuReverb = (AudioUnit)OpenComponent(compid);
    if (g_AuReverb==NULL) {
        Release();
        return -4;
    }

    // Open the output unit

    g_AuOutput = (AudioUnit)OpenDefaultComponent(kAudioUnitComponentType, kAudioUnitSubType_Output);
    if (g_AuOutput==NULL) {
        Release();
        return -5;
    }

    // Connect the units

    auconnect.sourceAudioUnit = g_AuMixer;
    auconnect.sourceOutputNumber = 0;
    auconnect.destInputNumber = 0;

    err = AUDIO_CHECK(AudioUnitSetProperty(g_AuOutput,
                                kAudioUnitProperty_MakeConnection,
                                kAudioUnitScope_Input,
                                0,
                                (void*)&auconnect,
                                sizeof(AudioUnitConnection)));
    if (err!=noErr) {
        Release();
        return -6;
    }

    // Initialize the units

    if (AUDIO_CHECK(AudioUnitInitialize(g_AuMixer))!=noErr) {
        Release();
    }
    if (AUDIO_CHECK(AudioUnitInitialize(g_AuReverb))!=noErr) {
        Release();
    }
    if (AUDIO_CHECK(AudioUnitInitialize(g_AuOutput))!=noErr) {
        Release();
    }

    V3XA.State |= 1;
    return 0;
}


int auSoundBuffer::GetPlayPosition(int32_t *lpPos)
{
	 Lock();
     *lpPos = m_nPlayPosition * sizeof(float);
	 Unlock();
	 return 0;
}

int auSoundBuffer::Play(int32_t option)
{
 	AudioUnitInputCallback	aucallback;
    
    aucallback.inputProc = snd_input_callback;
    aucallback.inputProcRefCon = (void*)m_nIndex;

    if (AUDIO_CHECK(AudioUnitSetProperty(g_AuMixer,
                         kAudioUnitProperty_SetInputCallback,
                         kAudioUnitScope_Input,
                         m_nIndex,
                         (void*)&aucallback,
                         sizeof(AudioUnitInputCallback)))==noErr)

    {
    	m_nFlags|=DSH_PLAYING;
        if (option & DSH_LOOPED)
        	m_nFlags|=DSH_LOOPED;
        else
       	    m_nFlags&=~DSH_LOOPED;
    }
    return 0;
}

int auSoundBuffer::Stop()
{
    if (m_nFlags & DSH_PLAYING)
    {
	   Lock();
  	   AudioUnitInputCallback	aucallback;
       aucallback.inputProc = NULL;
       aucallback.inputProcRefCon = (void*)m_nIndex;
  	   AUDIO_CHECK(AudioUnitSetProperty(g_AuMixer,
                           kAudioUnitProperty_SetInputCallback,
                           kAudioUnitScope_Input,
                           m_nIndex,
                           (void*)&aucallback, sizeof(AudioUnitInputCallback)));
       m_nFlags&=~DSH_PLAYING;
	   Unlock();
	}
    return 0;
}

int auSoundBuffer::Rewind()
{
    m_nCurFrame = 0;
    return 0;
}

int auSoundBuffer::DuplicateSource(auSoundBuffer *src)
{
    m_nNumFrame = src->m_nNumFrame;
    m_pData = src->m_pData;
    m_nFlags |= DSH_COPY;
    src->m_nInstance++;
    return 0;
}

int auSoundBuffer::Lock()
{
    pthread_mutex_lock((pthread_mutex_t*)&g_pThreadMutex);
    return 0;
}

int auSoundBuffer::Unlock()
{
	pthread_mutex_unlock((pthread_mutex_t*)&g_pThreadMutex);
    return 0;
}

#define P_RESAMPLE_BITS 10

// 8bit->float
static void *snd_byte_to_float(void *dst, const void *src, size_t n, int scale)
{
    const float f = 1.f / 127.f;
	float *lpDst = (float *)dst;
	const int8_t *lpSrc = (const int8_t *)src;
	unsigned i;
    for (i=0;i<n;i++)
    {
        lpDst[i] = f *(float)lpSrc[i*scale>>P_RESAMPLE_BITS];
    }
	return dst;
}

// 16bit->float
static void *snd_int16BIG_to_float(void *dst, const void *src, size_t n, int scale)
{
	const float f = 1.f / 32767.f;
	float *lpDst = (float *)dst;
	const int16_t *lpSrc = (const int16_t *)src;
	n>>=1; // Convert to number of sample (/sizeof(short))
    unsigned i;
    for (i=0;i<n;i++)
    {
        lpDst[i]= f * (float)lpSrc[i*scale>>P_RESAMPLE_BITS];
    }
	return dst;
}


static void *snd_int16LITTLE_to_float(void *dst, const void *src, size_t n, int scale)
{
	const float f = 1.f / 32767.f;
	float *lpDst = (float *)dst;
	const int16_t *lpSrc = (const int16_t *)src;
	n>>=1; // Convert to number of sample (/sizeof(short))

    unsigned i;
    for (i=0;i<n;i++)
    {
        int16_t k = lpSrc[i*scale>>P_RESAMPLE_BITS];
        lpDst[i] = f * (float)(int16_t)(  (((u_int16_t)k)<<8) | (((u_int16_t)k)>>8) );
    }
	return dst;
}

// float->float
static void *snd_float_to_float(void *dst, const void *src, size_t n, int scale)
{
    unsigned i;
	float *lpDst = (float*)dst;
	const float *lpSrc = (float*)src;
    n>>=2;
	for (i=0;i<n;i++)
    {
    	lpDst[i] = lpSrc[i*scale>>P_RESAMPLE_BITS];
    }
	return dst;
}

typedef void *(*PFSAMPLECONVERTER)(void *, const void *, size_t, int scale );

int auSoundBuffer::WriteSample(int offset, void *data, int size)  // offset, size in byte
{
	PFSAMPLECONVERTER pf;
    Lock();

	int n = 0;
	
	if (m_pSample->sampleFormat & V3XA_FMTIEEE)
	{
		pf = snd_float_to_float;
		n = 4;
	}
	else
    if (m_pSample->sampleFormat & V3XA_FMT16BIT)
	{
		if (m_pSample->sampleFormat & V3XA_FMTBIGENDIAN)
			pf = snd_int16BIG_to_float;
		else
			pf = snd_int16LITTLE_to_float;
		n = 2;
	}
    else
	{
		pf = snd_byte_to_float;
		n = 1;
	}
	
	int length = m_nNumFrame * sizeof(float);
	void *lpP1 = (char*)m_pData + offset * sizeof(float) / n;
    int dwB1 = 0;
    void *lpP2 = NULL;
    int dwB2 = 0;

	if (size + offset <= length)
		dwB1 = size;
	else
	{
		dwB1 = length - offset;
		lpP2 = (char*)m_pData;
		dwB2 = size - dwB1;
		SYS_ASSERT(dwB2>=0);
		SYS_ASSERT(dwB2<=length);
	}

	SYS_ASSERT(data);
	SYS_ASSERT(size>0);

    int factor = (m_nSampleRate << P_RESAMPLE_BITS)/44100;

	if (lpP1)
		pf((char*)lpP1, data, dwB1, factor);

	if (lpP2)
		pf((char*)lpP2, (char*)data + dwB1, dwB2, factor);

    Unlock();
    return size;
}



int auSoundBuffer::Create(int sampleFormat, int sampleRate, int length, bool bStreaming)
{
    AudioStreamBasicDescription austream;
	Lock();

    m_nNumFrame = length;

	if (sampleFormat & V3XA_FMTIEEE)
       m_nNumFrame>>=2;
	else
    if (sampleFormat & V3XA_FMT16BIT)
       m_nNumFrame>>=1;

	size_t l = sizeof(float) * m_nNumFrame;
    m_pData = (float*)calloc(m_nNumFrame, sizeof(float));
    m_nInstance = 1;
	m_nPlayPosition = 0;
	m_nSampleRate = sampleRate;
    austream.mSampleRate = 44100;
    austream.mFormatID = kAudioFormatLinearPCM;
    austream.mFormatFlags = kLinearPCMFormatFlagIsFloat     // kAudioFormatFlagIsSignedInteger for 8/16 bits
                          | kLinearPCMFormatFlagIsBigEndian
                          | kLinearPCMFormatFlagIsPacked;

    austream.mChannelsPerFrame = 2;
    austream.mBitsPerChannel = 32;
    austream.mBytesPerPacket = 8;
    austream.mFramesPerPacket = 1;
    austream.mBytesPerFrame = 8;

    AUDIO_CHECK(AudioUnitSetProperty(g_AuMixer,
                         kAudioUnitProperty_StreamFormat,
                         kAudioUnitScope_Input,
                         m_nIndex,
                         (void*)&austream,
                         sizeof(AudioStreamBasicDescription)
    ));

	Unlock();
    return !!m_pData;
}

int auSoundBuffer::Release()
{
    Stop();
    if (m_pData)
    {
        m_nInstance--;
        if (m_nInstance == 0)
        {
            free(m_pData);
            m_pData = 0;
        }
    }
    m_nFlags = 0;
    return 0;
}

int auSoundBuffer::SetPanValues(int n, float *v)
{
    sysMemCpy(m_fPan, v, n * sizeof(float));
    return 0;
}

int auSoundBuffer::SetGain(float gain)
{
    m_fGain = gain;
    return 0;
}

int auSoundBuffer::SetSampleRate(int rate)
{
    m_nSampleRate = rate;
    return 0;
}

int auSoundBuffer::GetStatus()
{
    return m_nFlags & DSH_PLAYING;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void RLXAPI ChannelSetParms(int channel, struct _v3xvector3 *pos, struct _v3xvector3 *velocity, struct _v3xbbox *range)
*
* DESCRIPTION :
*
*/
static void ChannelSetParms(int channel, V3XVECTOR *pos,
										 V3XVECTOR *velocity,
										 V3XRANGE *fRange
					 	   )
{
    /*
        auSoundBuffer *chan=&g_pHandle[channel];
        Store parameters
    */
	return;
}

static void RLXAPI UserSetParms(V3XMATRIX *lpMAT,
								V3XVECTOR *lpVEL,
								float*lpDistanceF,
								float*lpDopplerF,
								float*lpRolloffF )
{
    /*
       Store parameters
    */
	return;
}

static int ChannelSetEnvironment(V3XA_CHANNEL channel, V3XA_REVERBPROPERTIES *cfg)
{
	return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void RLXAPI Render(void)
*
* DESCRIPTION :  Render sounds...
*
*/
void RLXAPI Render(void)
{
    /*
       For all channels
          if channel configure for 3D
          Compute new volume / panning
   */
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void DriverSampleSetVolume(float volume)
*
* DESCRIPTION :  Master volume control
*
*/
static void SetVolume(float volume)
{
    g_fGain = (float) volume / MAX_V3XA_AUDIO_VOLUME;
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
    auSoundBuffer		*chan;
    chan=&g_pHandle[channel];
    chan->SetGain((float)volume / MAX_V3XA_AUDIO_VOLUME);
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
    auSoundBuffer		*chan;
    chan=&g_pHandle[channel];
    if (!chan->GetStatus()) return;
    float f[2];
    float v = (float)pan / MAX_V3XA_AUDIO_VOLUME;
	if (pan<0)
	{
		f[0] = 1.f;
		f[1] = 1.f + v;
	}
	else
	{
		f[0] = 1.f - v;
		f[1] = 1.f;
	}
	
	chan->Lock();

	chan->SetPanValues(2, f);

	chan->Unlock();

	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void ChannelSetSamplingRate(int channel, int m_nSampleRate)
*
* DESCRIPTION :  Set frequency.
*
*/
static void ChannelSetSamplingRate(int channel, int sampleRate)
{
    auSoundBuffer		*chan;
    chan=&g_pHandle[channel];
    if (!chan->GetStatus()) return;
    chan->SetSampleRate(sampleRate);
	return;
}


/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void ChannelOpen(int gain, int channel)
*
* DESCRIPTION :  Allocate sound drivers.
*
*/
static void ChannelOpen(int nGain, int numChannels)
{
    V3XA.numChannel = numChannels;
	return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void SmpLoad(V3XA_HANDLE *sam)
*
* DESCRIPTION :
*
*/
static int SmpLoad(V3XA_HANDLE *sam)
{
	return 1;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : void SmpRelease(SAMPLE *sam)
*
* DESCRIPTION : Free a sound handle
*
*/
static void SmpRelease(V3XA_HANDLE *sam)
{
	return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  : int ChannelGetFree(V3XA_HANDLE *sam)
*
* DESCRIPTION : Find a channel with the m_pSample handle.
*
*/
static int ChannelGetFree(V3XA_HANDLE *sam)
{
    int i;
	int prefSecure		=-1;
	int prefSamready	=-1;
	int prefSamStoppedMatch =-1;
	int prefSamPriority 	=-1;
	int newChannel		=-1;
	auSoundBuffer *p;
	SYS_ASSERT(V3XA.numChannel>=0); // did you forget V3XA.Client->ChannelOpen() ?
	for (i = 0, p = g_pHandle;i < V3XA.numChannel;i++, p++)
	{
     	if ((p->m_nFlags&DSH_RESERVED)==0) // n'y pense MEME PAS!
		{
			if (!p->m_pSample)
			{
				if ((prefSecure<0)&&(p->m_pSample==NULL))
					prefSecure = i;
				if (i)
					prefSamready = i;
			}
			else
			{
				if (sam)
				{
					if (!p->GetStatus())
					{
						if (i) prefSamready = i;
						if ((p->m_pSample==sam))
						{
							prefSamStoppedMatch=i;
						}
					}
					else
					{
						if (p->m_pSample)
						{
							if (sam->priority>=p->m_pSample->priority) prefSamPriority=i;
						}
					}
				}
				else
				{
					if (!p->m_pSample)
					{
					if ((prefSecure<0)&&(p->m_pSample==NULL)) prefSecure = i;
					}
				}
			}
		}
	}
	if (prefSamStoppedMatch>=0)
		newChannel = prefSamStoppedMatch;
	else
	if (prefSamready>=0)
		newChannel = prefSamready;
	else
	if (prefSamPriority>=0)
		newChannel = prefSamPriority;
	else
	if (prefSecure>=0)
		newChannel = prefSecure;
	return newChannel>=32 ? -1 : newChannel;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int ChannelPlay(int channel, int sampleRate, float volume, float pan, V3XA_HANDLE *sam)
*
* DESCRIPTION :  Play a sound in a given channel, sampling rate, volume, pan
*
*/

static int ChannelPlay(int channel, int sampleRate, float volume, float pan, V3XA_HANDLE *sam)
{
    auSoundBuffer *chan=&g_pHandle[channel];
  
    // hookup the input to the mixer
    chan->Stop();
    chan->Rewind();
  
    if (sam != chan->m_pSample)
    {
        int bFound = 0;

        if (!bFound)
        {
            chan->Release();
            chan->Create(sam->sampleFormat, sampleRate, sam->length, false);
            chan->m_pSample = sam;
            chan->WriteSample(0, sam->sample, sam->length);
        }
    }
    
	ChannelSetVolume(channel, volume);
	ChannelSetPanning(channel, pan);
	return chan->Play(sam->loopend ? DSH_LOOPED : DSH_SINGLE);
    
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void ChannelStop(int channel)
*
* DESCRIPTION :	Stop the sound in a channel.
*
*/
static void ChannelStop(int channel)
{
    auSoundBuffer		*chan;
    chan=&g_pHandle[channel];
    if (!chan->GetStatus()) return;
    chan->Stop();
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	int ChannelGetStatus(int channel)
*
* DESCRIPTION :	Get the channel status
*
*/
static int ChannelGetStatus(int channel)
{
    auSoundBuffer		*chan;
    chan=&g_pHandle[channel];
	return chan->GetStatus();
}

static V3XA_HANDLE *ChannelGetSample(int channel)
{
    auSoundBuffer		*chan;
    chan = &g_pHandle[channel];
	return chan->m_pSample;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void StreamRelease(void)
*
* DESCRIPTION : Release a stream buffer
*
*/

static V3XA_STREAM GetFreeStreamHandle()
{
	int i;
	for (i=0;i<MAX_V3XA_AUDIO_STREAM;i++)
	{
		auStreamBuffer *pStr = g_pStream + i;
		if (pStr->nState == 0)
			return i;
	}
	return -1;
}

static void StreamRelease(V3XA_STREAM handle)
{
    auStreamBuffer *pStr = g_pStream + handle;
	if (!pStr->nState)
		return;
		
	auSoundBuffer   *pSrc = g_pHandle + pStr->nChannel;

	pSrc->Stop();
	pSrc->Release();
	pStr->nState = 0;

	return;
}

static V3XA_STREAM StreamInitialize(int sampleFormat, int sampleRate, size_t size)
{
    V3XA_STREAM handle = GetFreeStreamHandle();
	if (handle == -1)
		return -1;

    auStreamBuffer *pStr = g_pStream + handle;
	if (!pStr)
		return -1;

    pStr->nChannel = 0;
	
	pStr->nState = 1;
	sysMemZero(&pStr->sample, sizeof(V3XA_HANDLE));
	
	pStr->sample.length = size;
	pStr->sample.sampleFormat = sampleFormat;
	pStr->sample.samplingRate = sampleRate;
	pStr->m_nStreamState = 2;

	auSoundBuffer   *pSrc = g_pHandle + pStr->nChannel;
    pSrc->Create(sampleFormat, sampleRate, size, true);
    pSrc->m_nFlags |= DSH_RESERVED;
	pSrc->m_pSample = &pStr->sample;
	return handle;
}

static int StreamPoll(V3XA_STREAM handle)
{
    auStreamBuffer *pStr = g_pStream + handle;
	if (!pStr->nState)
		return 0;

	int ret = -1;

	if (pStr->m_nStreamState)
	{
		ret = 1;
	}
	else
	{
		auSoundBuffer   *pSrc = g_pHandle + pStr->nChannel;
        int32_t lPlay;

        pSrc->GetPlayPosition(&lPlay);

		int32_t elapsed = (pStr->m_nPreviousPosition<=lPlay)
				? lPlay - pStr->m_nPreviousPosition
				: lPlay + (int32_t)pStr->sample.length - pStr->m_nPreviousPosition;

		SYS_ASSERT(elapsed>=0);
		pStr->m_nWriteBytes += elapsed;
		pStr->m_nTotalBytes += elapsed;
		pStr->m_nPreviousPosition = lPlay;

		if (pStr->m_nWriteBytes + elapsed >= pStr->m_nUploadedBytes)
		{
			pStr->m_nWriteBytes-=pStr->m_nUploadedBytes;
			pStr->m_nUploadedBytes = 0;
			ret = 1;
		}
	}
	return ret;
}

static int StreamLoad(V3XA_STREAM handle, void *data, size_t size)
{
    auStreamBuffer *pStr = g_pStream + handle;
	if (!pStr->nState)
		return 0;

	auSoundBuffer   *pSrc = g_pHandle + pStr->nChannel;
	
	if (pSrc->WriteSample(pStr->m_nWritePosition, data, size))
	{
		pStr->m_nUploadedBytes += size;
		pStr->m_nWritePosition += size;
		if (pStr->m_nWritePosition >= (int32_t)pStr->sample.length)
		{
			pStr->m_nWritePosition -= pStr->sample.length;
		}
	}
	else
	{
		return 0;
	}
	if (pStr->m_nStreamState)
	{
		pStr->m_nStreamState--;
		if (!pStr->m_nStreamState)
		{
			pSrc->Rewind();
			pSrc->Play( DSH_LOOPED );
			pStr->m_nPreviousPosition = 0;
		}
	}

	return pStr->nState;
}

static void StreamStop(V3XA_STREAM handle)
{
    auStreamBuffer *pStr = g_pStream + handle;
	if (!pStr->nState)
		return;
	auSoundBuffer   *pSrc = g_pHandle + pStr->nChannel;
    pSrc->Stop();
	return;
}

static int StreamStart(V3XA_STREAM handle)
{
    auStreamBuffer *pStr = g_pStream + handle;
	if (!pStr->nState)
		return 0;

	auSoundBuffer *pSrc = g_pHandle + pStr->nChannel;

	if (!pStr->m_nStreamState)
	{
		pSrc->Rewind();
		pSrc->Play(DSH_LOOPED);
	}

	pStr->m_nPreviousPosition = 0;
	pStr->m_nWritePosition = 0;
	pStr->m_nStreamState = 1;
	pStr->m_nTotalBytes = 0;
	pStr->m_nUploadedBytes = 0;
	pStr->m_nWriteBytes = 0;

	return 0;
}

static int StreamGetChannel(V3XA_STREAM handle)
{
    auStreamBuffer *pStr = g_pStream + handle;
	if (!pStr->nState)
		return 0;
	return pStr->nChannel;
}

static void StreamSetVolume(V3XA_STREAM handle, float volume)
{
    auStreamBuffer *pStr = g_pStream + handle;
	if (!pStr->nState)
		return;
	ChannelSetVolume(pStr->nChannel, volume);
	return;
}

static size_t StreamGetPosition(V3XA_STREAM handle)
{
    auStreamBuffer *pStr = g_pStream + handle;
	if (!pStr->nState)
		return 0;
	return pStr->m_nTotalBytes;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :	void ChannelFlushAll(void)
*
* DESCRIPTION :  Kill all channel
*
*/
static void ChannelFlushAll(int mode)
{
    int i;
    for (i=0;i<MAX_V3XA_AUDIO_MIX;i++)
    {
    	g_pHandle[i].Stop();
    }
	return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void ChannelInvalidate(V3XA_HANDLE *snd)
*
* DESCRIPTION : Invalidate a m_pSample (for sound file cache).
*
*/
static void ChannelInvalidate(V3XA_HANDLE *snd)
{
    int i;
    for (i=0;i<MAX_V3XA_AUDIO_MIX;i++)
    {
    	if (g_pHandle[i].m_pSample == snd)
    	{
        	g_pHandle[i].Stop();
        	g_pHandle[i].m_pSample = 0;
        }
    }
	return;
}

static V3XA_WaveClientDriver CoreAudio_Client = {

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


void RLXAPI V3XA_EntryPoint(struct RLXSYSTEM *)
{
	V3XA.Client = &CoreAudio_Client;
	return;
}
