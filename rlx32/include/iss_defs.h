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
#ifndef _ISS_DEFS_HH
#define _ISS_DEFS_HH

#include "v3xtypes.h"

// Max supported volume
#define MAX_V3XA_AUDIO_VOLUME			1.f

// Max client driver name
#define MAX_V3XA_CLIENT_DRIVER_NAME		128

// Max file search path
#define MAX_V3XA_AUDIO_PATH				256

// Max filename
#define MAX_V3XA_FILENAME				256

// Max mixing channels
#define MAX_V3XA_AUDIO_MIX				32

// Max simultaneous audio streams
#define MAX_V3XA_AUDIO_STREAM			8

// Max tracks per streams
#define MAX_V3XA_TRACKS_PER_STREAM		4

// Default stream buffer size in Kb
#define V3X_DEFAULT_STREAM_BUFFER_SIZE	57

// Max supported file codecs
#define MAX_V3XA_FILECODECS				32

// Max time in milliseconds before a sound is removed from cache
#define MAX_V3XA_CACHE_RESIDENT_TIMEOUT	60000


typedef int V3XA_CHANNEL;
// Sample format
enum {
    V3XA_FMT16BIT = 0x1,        // 16Bit samples
    V3XA_FMTSTEREO = 0x2,        // Stereo 2-Channels sample
    V3XA_FMTPCM = 0x4,        // PCM format (AMIGA)
    V3XA_FMTPACKED = 0x8,        // Compressed sample
    V3XA_FMTMULTI = 0x10,       // Reserved
    V3XA_FMTVOLATILE = 0x20,       // Reserved
    V3XA_FMTBIGENDIAN = 0x40,       // ADPCM format 
    V3XA_FMT3D = 0x80,       // 3D Samples (in use with DirectSound 3D)
	V3XA_FMTIEEE = 0x100,      // OGG
	V3XA_FMTUNKSIZE  = 0x200,      // Unknown size
    V3XA_PANSURROUND = 100           // Surround panning value for Surround play.
};


// Sample handle
typedef struct _v3xa_handle
{
    void              *sample;            // Raw data sample
    u_int32_t              length;            // Sample length in byte 
    u_int32_t              loopstart;         // Loop start offset
    u_int32_t              loopend;           // Loop end offset
    u_int32_t              samplingRate;      // Sample sampling rate in hz 
    u_int32_t              chunkLength;       // Reserved
    u_int16_t             sampleFormat;      // Sample format (see before).
    u_int16_t             sampleID;          // Reserved
    u_int8_t              mode;              // Reserved
    u_int8_t              numTracks;        // Reserved 
    u_int8_t              priority;          // Play priority 0..255 (MAX)
    u_int8_t              filler;            // Reserved
	struct _v3xa_audiocodec *	codec;	
}V3XA_HANDLE;

// Streaming functions
typedef int V3XA_STREAM;

// Sound decoder and processor
struct _sys_fileio;

typedef void * (*PFV3XAAUDIOINIT)(size_t size, struct _sys_fileio *, 
										struct _sys_memory *);
typedef int (*PFV3XAAUDIORELEASE)(void *context);
typedef int (*PFV3XAAUDIOOPEN)(void *context, SYS_FILEHANDLE fp, V3XA_HANDLE *info);
typedef int (*PFV3XAAUDIODECODER)(void *context, const void *pdataRaw, size_t sizeRaw,
                                      void **pdataPCM, size_t *sizePcm);

// Audio codec definition
typedef struct _v3xa_audiocodec
{
    /* initialise decoder. returns a 'context' */
	PFV3XAAUDIOINIT				initialize;
    /* release context */
    PFV3XAAUDIORELEASE			release;
	/* open file from file handle and sample information */
    PFV3XAAUDIOOPEN				open;
    /* decode audio from raw data and size. Returns a PCM formatted data and uncompressed size.
	   returns 0 if data fully decompressed, 1 if data frame is partially decompressed, or -1 if end of file
	*/
    PFV3XAAUDIODECODER			decode;
	/* encode audio */
}V3XA_AUDIOCODEC;

// Codec for file format
typedef struct _v3xa_filecodec {
    char      *ext;
    int      (*decode)(SYS_FILEHANDLE in, int options, V3XA_HANDLE *sinfo);
}V3XA_FILECODEC;

struct _v3xa_audiocodec;

// Channel infos
typedef struct _v3xa_channelInfo
{
    u_int8_t              Volume;
    u_int8_t              filler[3];
}V3XA_CHANNELINFO;

// EAX environment preset
enum V3XA_ENVIRONMENT
{
    V3XA_ENVIRONMENT_GENERIC,
    V3XA_ENVIRONMENT_PADDEDCELL, 
    V3XA_ENVIRONMENT_ROOM, 
    V3XA_ENVIRONMENT_BATHROOM, 
    V3XA_ENVIRONMENT_LIVINGROOM,
    V3XA_ENVIRONMENT_STONEROOM, 
    V3XA_ENVIRONMENT_AUDITORIUM, 
    V3XA_ENVIRONMENT_CONCERTHALL, 
    V3XA_ENVIRONMENT_CAVE, 
    V3XA_ENVIRONMENT_ARENA, 
    V3XA_ENVIRONMENT_HANGAR, 
    V3XA_ENVIRONMENT_CARPETEDHALLWAY, 
    V3XA_ENVIRONMENT_HALLWAY, 
    V3XA_ENVIRONMENT_STONECORRIDOR, 
    V3XA_ENVIRONMENT_ALLEY, 
    V3XA_ENVIRONMENT_FOREST, 
    V3XA_ENVIRONMENT_CITY, 
    V3XA_ENVIRONMENT_MOUNTAINS, 
    V3XA_ENVIRONMENT_QUARRY,
    V3XA_ENVIRONMENT_PLAIN, 
    V3XA_ENVIRONMENT_PARKINGLOT, 
    V3XA_ENVIRONMENT_SEWERPIPE, 
    V3XA_ENVIRONMENT_UNDERWATER, 
    V3XA_ENVIRONMENT_DRUGGED, 
    V3XA_ENVIRONMENT_DIZZY, 
    V3XA_ENVIRONMENT_PSYCHOTIC, 
    V3XA_ENVIRONMENT_COUNT, 
	V3XA_ENVIRONMENT_FORCEDWORD = 0xffff
};

// Reverb properties
typedef struct _v3xa_reverbproperties
{
	/* environment */
    u_int32_t		environment; 
	/* volume */
    float		fVolume;
	/* decay in msec */
    float		fDecayTime_sec;
	/* damping in sec. */
    float		fDamping;  
}V3XA_REVERBPROPERTIES;

// Wave driver
typedef struct _v3xa_wave_client_driver
{
// Init Functions
	int 			(RLXAPI *Enum)(void);			// Enumerate devices
	int 			(RLXAPI *Detect)(void);			// Detect devices
	int 			(RLXAPI *Initialize)(void *);	// Initialize previously detected device
	void			(RLXAPI *Release)(void);		// Release driver
	void			(RLXAPI *SetVolume)(float volume);// Set master volume
	void			(RLXAPI *Start)(void); 		// Start to play
	void			(RLXAPI *Stop)(void);			// Stop to play
	int32_t			(RLXAPI *Poll)(int32_t param); // poller
	void			(RLXAPI *Render)(void);		// Render 3D environment sound (optional)
	void			(RLXAPI *UserSetParms)(V3XMATRIX *lpMAT, V3XVECTOR *lpVEL, 
										   float *lpDistanceF, float *lpDopplerF, 
										   float *lpRolloff);
// Sample Functions
	void			(RLXAPI *ChannelOpen)(int gain, int numbersOfchannel); // Open multichannel mixer
	int 			(RLXAPI *ChannelPlay)(V3XA_CHANNEL channel, int frequency, float volume, float panning, V3XA_HANDLE *handle); // Play channel given frequency, volume, panning and audio handle
	void			(RLXAPI *ChannelStop)(V3XA_CHANNEL channel); // stop channem

	void			(RLXAPI *ChannelSetVolume)(V3XA_CHANNEL channel, float volume); // set channel volume
	void			(RLXAPI *ChannelSetPanning)(V3XA_CHANNEL channel, float panning); // set channel panning
	void			(RLXAPI *ChannelSetSamplingRate)(V3XA_CHANNEL channel, int frequency); // set channel sampling rate

	int 			(RLXAPI *ChannelGetStatus)(V3XA_CHANNEL channel); // get channel status

	void			(RLXAPI *ChannelSetParms)(V3XA_CHANNEL channel, V3XVECTOR *pos, 
																    V3XVECTOR *velocity, 
																	V3XRANGE *fRange
																	); // set channel parameters for 3D placement
	int 			(RLXAPI *ChannelSetEnvironment)(V3XA_CHANNEL channel, V3XA_REVERBPROPERTIES *cfg); // set channel environmenet

	V3XA_CHANNEL 	(RLXAPI *ChannelGetFree)(V3XA_HANDLE *handle); // get free channel
	void			(RLXAPI *ChannelFlushAll)(int mode); // flush all channel
	void			(RLXAPI *ChannelInvalidate)(V3XA_HANDLE *handle); // invalid channel 
	V3XA_HANDLE* 	(RLXAPI *ChannelGetSample)(V3XA_CHANNEL channel); // get sound handle from channel

// Streaming
	void			(RLXAPI *StreamRelease)(V3XA_STREAM handle); // release streaming handle
	V3XA_STREAM		(RLXAPI *StreamInitialize)(int sampleFormat, int samplingRate, size_t size); // initialise stream channel with sample format, sampling rate, buffer size
	V3XA_CHANNEL 	(RLXAPI *StreamGetChannel)(V3XA_STREAM handle); // get play channel from steam handle
	size_t			(RLXAPI *StreamGetPosition)(V3XA_STREAM handle); // 
	int				(RLXAPI *StreamPoll)(V3XA_STREAM handle); // Should be threaded if possible
	void			(RLXAPI *StreamSetVolume)(V3XA_STREAM handle, float volume);
	int				(RLXAPI *StreamLoad)(V3XA_STREAM handle, void *data, size_t size);
	int 			(RLXAPI *StreamStart)(V3XA_STREAM handle);
	void			(RLXAPI *StreamStop)(V3XA_STREAM handle);

// Low level Load/Unload sample (used internally)
	int 			(RLXAPI *SmpLoad)(V3XA_HANDLE *handle);
	void			(RLXAPI *SmpRelease)(V3XA_HANDLE *handle);

	char			s_DrvName[MAX_V3XA_CLIENT_DRIVER_NAME];
	char		**	p_DriverList;
	
}V3XA_WaveClientDriver;


typedef struct {
    unsigned 					numTrack;
    unsigned 					channel;
    unsigned 					status;
    u_int32_t    					position;
    u_int32_t    					length;
	V3XA_HANDLE				*	sample;
}V3XA_STREAMINFO;


// V3X Audio object
struct V3XAUDIO
{
	V3XA_WaveClientDriver	*	Client;
	char					**	p_driverList;
	char						samplePath[MAX_V3XA_AUDIO_PATH];	
	u_int32_t						State;
	int32_t						nStreamBufferSizeKb;
	int32_t 						numChannel;
	int32_t 						samplingRate;
	int32_t						deviceID;
	int32_t						driverID;
};



__extern_c

_RLXEXPORTFUNC    int	 V3XAStream_GetFn(V3XA_STREAM *stream, const char *szFilename, int loop);
_RLXEXPORTFUNC    void   V3XAStream_SetVolume(V3XA_STREAM handle, V3XA_CHANNEL channel, float volume);
_RLXEXPORTFUNC    int    V3XAStream_Poll(V3XA_STREAM handle);
_RLXEXPORTFUNC    int    V3XAStream_PollAll(void);
_RLXEXPORTFUNC    void   V3XAStream_Release(V3XA_STREAM handle);
_RLXEXPORTFUNC	  void	 V3XA_EntryPoint(struct RLXSYSTEM *pRlx);
_RLXEXPORTDATA    extern struct V3XAUDIO  V3XA;

__end_extern_c

#endif
