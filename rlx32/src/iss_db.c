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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "sysresmx.h"
#include "systools.h"
#include "iss_defs.h"
#include "iss_fx.h"
#include "iss_av.h"

//  #define _TRACE

enum V3XA_STREAM_STATE
{
	V3XA_STREAM_STATE_OK 	 		= 1,
	V3XA_STREAM_STATE_LOOP	 		= 2,
	V3XA_STREAM_STATE_PACKED  		= 4,
	V3XA_STREAM_STATE_STANDBY 		= 8,
	V3XA_STREAM_STATE_DUAL	 		= 16,
	V3XA_STREAM_STATE_MUTE1	 		= 32,
	V3XA_STREAM_STATE_MUTE2	 		= 64,
	V3XA_STREAM_STATE_EOF	 		= 128,
	V3XA_STREAM_STATE_SEEKABLE 		= 256,
	V3XA_STREAM_STATE_PLAY	  		= 512,
	V3XA_STREAM_STATE_REWIND   		= 1024,
	V3XA_STREAM_FEED				= 2048

};
typedef struct _v3xa_stream
{
	V3XA_HANDLE					info;
	int32_t						numTrack;
	int32_t						dwState;
	
	size_t						chunkLength;
	size_t						dwFileOffset;
	size_t						dwLength;
	size_t						dwPositionRead;
	size_t						dwPositionWrite;

	V3XA_STREAM					handle[MAX_V3XA_TRACKS_PER_STREAM]; 
	void				*		sample[MAX_V3XA_TRACKS_PER_STREAM]; 
	void				*		context[MAX_V3XA_TRACKS_PER_STREAM];
	
	SYS_FILEHANDLE 				pFile;
	SYS_FILEIO			*		pStream;
	SYS_WAD				*		pFatMgr;
	
}V3XA_STREAMEX;

static V3XA_STREAMEX g_pStreams[MAX_V3XA_AUDIO_STREAM];

/*------------------------------------------------------------------------
*
* PROTOTYPE  :	static void V3XA_STREAM_Close(V3XA_STREAMEX *pHandle)
*
* DESCRIPTION :
*
*/
static void V3XA_STREAM_Stop(V3XA_STREAMEX *pHandle)
{
	int i;
	if ((pHandle->dwState&V3XA_STREAM_STATE_OK)==0)
		return;
	pHandle->dwState&=~(V3XA_STREAM_STATE_EOF|V3XA_STREAM_STATE_STANDBY|V3XA_STREAM_STATE_PLAY);
	for (i=0;i<pHandle->numTrack;i++)
	{
		V3XA.Client->StreamStop(pHandle->handle[i]); 
	}
	return;
}

static void V3XA_STREAM_Close(V3XA_STREAMEX *pHandle)
{
	int i;
	SYS_WAD *pFatMgr = filewad_getcurrent();
		if ((pHandle->dwState&V3XA_STREAM_STATE_OK)==0) return;
		V3XA_STREAM_Stop(pHandle);
	filewad_setcurrent( pHandle->pFatMgr );

	pHandle->dwState&=~(V3XA_STREAM_STATE_OK);
	if (pHandle->dwState & V3XA_STREAM_STATE_SEEKABLE)
	{
		pHandle->pStream->fseek(pHandle->pFile, (int32_t)pHandle->dwFileOffset, SEEK_SET);
	}
	pHandle->pStream->fclose(pHandle->pFile);
	for (i=0;i<pHandle->numTrack;i++)
	{
		MM_std.free(pHandle->sample[i]);
		V3XA.Client->StreamRelease(pHandle->handle[i]); 
		pHandle->info.codec->release(pHandle->context[i]);
	}
	filewad_setcurrent( pFatMgr );
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	static void V3XA_STREAM_Rewind(V3XA_STREAMEX *pHandle)
*
* DESCRIPTION :
*
*/
static void V3XA_STREAM_Rewind(V3XA_STREAMEX *pHandle)
{
	int i;
	SYS_WAD *pFatMgr = filewad_getcurrent();

	if ((pHandle->dwState&V3XA_STREAM_STATE_OK)==0)
		return ;

	if ((pHandle->dwState&V3XA_STREAM_STATE_REWIND))
		return ;

	filewad_setcurrent( pHandle->pFatMgr );
	V3XA_STREAM_Stop(pHandle);
	pHandle->pStream->fseek(pHandle->pFile, (int32_t)pHandle->dwFileOffset, SEEK_SET);

	for (i=0; i<pHandle->numTrack; i++)
	{
		if (pHandle->context[i])
		{	
			pHandle->info.codec->release(pHandle->context[i]);
			pHandle->context[i] = pHandle->info.codec->initialize(pHandle->chunkLength, pHandle->pStream, &MM_std);
			if (pHandle->info.codec->open)
			{
				if (pHandle->info.codec->open(pHandle->context[i], pHandle->pFile, &pHandle->info) == 1)
					pHandle->dwState|=V3XA_STREAM_FEED;
			}
		}

		if (pHandle->handle[i])
			V3XA.Client->StreamStart(pHandle->handle[i]);
	}

	pHandle->dwPositionRead = pHandle->dwPositionWrite =  0;
	pHandle->dwState&=~(V3XA_STREAM_STATE_EOF | V3XA_STREAM_STATE_STANDBY);
	pHandle->dwState|=V3XA_STREAM_STATE_PLAY | V3XA_STREAM_STATE_REWIND;

	filewad_setcurrent(pFatMgr);
	return;
}

static int V3XA_STREAM_Load(V3XA_STREAMEX *pHandle, size_t size)
{
	int ret = 0;
	int i;
	size_t EndPosition = pHandle->dwPositionWrite + size; // minimum : 16K

	if ((pHandle->dwState & V3XA_STREAM_STATE_OK)==0)
		return 0;

	pHandle->dwState&=~V3XA_STREAM_STATE_REWIND;

	while ((pHandle->dwPositionWrite<EndPosition)&&((pHandle->dwState&V3XA_STREAM_STATE_EOF)==0))
	{
		for (i=0;i<pHandle->numTrack;i++)
		{
			void *ptr = NULL;
			size_t sizeWrite = 0, sizeRead = 0;
			do
			{				
				if (pHandle->dwState & V3XA_STREAM_FEED)
				{
					sizeRead = size;
				}
				else
				{
					sizeRead = pHandle->pStream->fread(pHandle->sample[i], sizeof(char), size, pHandle->pFile);
					if (sizeRead<=0)
					{
						pHandle->dwState|=V3XA_STREAM_STATE_EOF;
						return -1;
					}
					else
						pHandle->dwPositionRead+=sizeRead;
				}

				ret = pHandle->info.codec->decode(pHandle->context[i],
									   pHandle->sample[i], sizeRead,
									   &ptr, &sizeWrite);
				if (ret == -1)
				{
					pHandle->dwState|=V3XA_STREAM_STATE_EOF;
					return -1;
				}
				else
				if (ret == 1)
					pHandle->dwState|=V3XA_STREAM_FEED;
				else
					pHandle->dwState&=~V3XA_STREAM_FEED;

			}while (!ptr);

			ret = V3XA.Client->StreamLoad(pHandle->handle[i], ptr, sizeWrite);
#ifdef _PROFILE
			SYS_Debug("!State=%d\n",ret);
#endif

			pHandle->dwPositionWrite += sizeWrite;
		}
	}
	return ret;
}

static int V3XA_STREAM_Update(V3XA_STREAMEX *pHandle)
{
	int n = -1;
	int i;

#ifdef _TRACE
	SYS_Debug("!Update stream\n");
#endif

	pHandle->dwState&=~V3XA_STREAM_STATE_REWIND;

	for (i=0;i<pHandle->numTrack;i++)
	{
		int status = V3XA.Client->StreamPoll(pHandle->handle[i]);
		if (!i)
			n = status;

#ifdef _TRACE
		SYS_Debug("!Poll status = %d\n", status);
#endif

	}

	if (n!=-1)
	{
		if (((pHandle->dwState&V3XA_STREAM_STATE_EOF)==0)&&
			((pHandle->dwState&V3XA_STREAM_STATE_STANDBY)==0))
		{
#ifdef _TRACE
			SYS_Debug("!Load new chunk of data %d\n", pHandle->chunkLength);
#endif
			if (V3XA_STREAM_Load(pHandle, pHandle->chunkLength)==-1)
			{
				pHandle->dwState|=V3XA_STREAM_STATE_STANDBY;
				return 2;
			}
		}
		else
		{
#ifdef _TRACE
			SYS_Debug("!Wait");
#endif
		}
	}
	else
	{
#ifdef _TRACE
			SYS_Debug("!Return :%d\n", n);
#endif
	}

	if (pHandle->dwState & V3XA_STREAM_STATE_STANDBY)
	{
		size_t l = V3XA.Client->StreamGetPosition(pHandle->handle[0]);
#ifdef _TRACE
		SYS_Debug("!Get position %d\n", l);
#endif

		if ( n!=-1 )
		{
			if (l>=pHandle->info.length)
			{
#ifdef _TRACE
				SYS_Debug("!End of track\n");
#endif

				if (pHandle->dwState&V3XA_STREAM_STATE_LOOP)
				{
					V3XA_STREAM_Rewind(pHandle);
					return -1;
				}
				else
				{
					V3XA_STREAM_Stop(pHandle);
					return 0;
				}
			}
		}
	}
	return 1;
}

// Public interface

int V3XAStream_Poll(V3XA_STREAM handle)
{
	V3XA_STREAMEX *pHandle = g_pStreams + handle;

#ifdef _TRACE
	SYS_Debug("!Poll stream\n");
#endif

	if ((pHandle->dwState&V3XA_STREAM_STATE_OK)&&(pHandle->dwState&V3XA_STREAM_STATE_PLAY))
	{
#ifdef _TRACE
		SYS_Debug("!Update\n");
#endif
	   return V3XA_STREAM_Update(pHandle);
	}
	else
	{
#ifdef _TRACE
		SYS_Debug("!Wait\n");
#endif
		return 0;
	}
}

int V3XAStream_PollAll()
{
	int i, s=0;
	for (i=0;i<MAX_V3XA_AUDIO_STREAM;i++)
	{
	   s+=V3XAStream_Poll((V3XA_STREAM)i);
	}
	return s;

}


void V3XAStream_Release(V3XA_STREAM handle)
{
	V3XA_STREAMEX *pHandle=g_pStreams+handle;
	if (pHandle->dwState&V3XA_STREAM_STATE_OK) 
		V3XA_STREAM_Close(pHandle);
	return ;
}

void V3XAStream_ReleaseAll(void)
{
	int i;
	V3XA_STREAMEX *pHandle=g_pStreams;
	for (i=0;i<MAX_V3XA_AUDIO_STREAM;i++, pHandle++)
	{
	   if (pHandle->dwState&V3XA_STREAM_STATE_OK) 
		   V3XA_STREAM_Close(pHandle);
	   V3XAStream_Poll((V3XA_STREAM)i);
	}
	return ;
}

void V3XAStream_Rewind(V3XA_STREAM handle)
{
	V3XA_STREAMEX *pHandle = g_pStreams + (int)handle;
	if (pHandle->dwState&V3XA_STREAM_STATE_OK)
	{
		V3XA_STREAM_Rewind(pHandle);
	}
}

void V3XAStream_Stop(V3XA_STREAM handle)
{
	V3XA_STREAMEX *pHandle = g_pStreams + (int)handle;
	int i;
	if (pHandle->dwState&V3XA_STREAM_STATE_OK)
	{
		for (i=0;i<pHandle->numTrack;i++)
		{
			V3XA.Client->StreamStop( pHandle->handle[i] );
		}
	}
}

int V3XAStream_Start(V3XA_STREAM handle)
{
	V3XA_STREAMEX *pHandle = g_pStreams + (int)handle;
	int i;
	if (pHandle->dwState&V3XA_STREAM_STATE_OK)
	{
		for (i=0;i<pHandle->numTrack;i++)
		{
			if (!V3XA.Client->StreamStart( pHandle->handle[i] ))
				return 0;
		}
	}
	return 1;
}


void V3XAStream_SetVolume(V3XA_STREAM handle, int channel, float volume)
{
	V3XA_STREAMEX *pHandle = g_pStreams + (int)handle;
	V3XA_STREAM hnd;
	if (channel>=0)
	{
		SYS_ASSERT(channel < MAX_V3XA_TRACKS_PER_STREAM);
		hnd = pHandle->handle[(int)channel];
		V3XA.Client->StreamSetVolume(hnd, volume);
	}
	else
	{
		int i;
		for (i=0;i<pHandle->numTrack;i++)
		{
			hnd = pHandle->handle[i];
			V3XA.Client->StreamSetVolume(hnd, volume);
		}
	}
	return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :	int V3XAStream_GetFn(char *filename, int loop)
*
* DESCRIPTION :
*
*/

int V3XAStream_GetFn( V3XA_STREAM *stream, const char *szFilename, int loop)
{
	char *filename = (char *)szFilename;
	V3XA_STREAMEX *pHandle = NULL;
	unsigned bestCache;
	V3XA_FILECODEC * codec = V3XA_CodecFind((char*)szFilename);	

	int i;
	size_t chunkLength = 0;
	int ret;

	if (!codec)
		return -10;	// bad codec

	for (i=0;i<MAX_V3XA_AUDIO_STREAM;i++)
	{
		if (!(g_pStreams[i].dwState&V3XA_STREAM_STATE_OK))
			pHandle = g_pStreams + i;
	}

	SYS_ASSERT(pHandle);
	if (!pHandle)
		return -20;	// no enough slot

	sysMemZero(pHandle, sizeof(V3XA_STREAM));
	pHandle->pFatMgr = filewad_getcurrent();
	pHandle->pStream = FIO_cur;		
	pHandle->pFile = pHandle->pStream->fopen(filename, "rb");
	if (!pHandle->pFile)
		return -21;	// file not found

	// default stream buffer size
	bestCache = 100 * 1024;
	pHandle->dwFileOffset = pHandle->pStream->ftell(pHandle->pFile);
	pHandle->info.chunkLength = pHandle->chunkLength = bestCache;	 

	// parse header
	ret = codec->decode(pHandle->pFile, 0, &pHandle->info);

	// read internal codec
	pHandle->dwState = V3XA_STREAM_STATE_PLAY | V3XA_STREAM_STATE_SEEKABLE | 
		               V3XA_STREAM_STATE_REWIND | V3XA_STREAM_STATE_OK | 
					   (loop ? V3XA_STREAM_STATE_LOOP : 0);

	// parse header
	if (1 == ret)
	{
		pHandle->numTrack = 1;
		pHandle->context[0] = pHandle->info.codec->initialize(pHandle->chunkLength, pHandle->pStream, &MM_std);
		if (!pHandle->context[0])
			return -22; // Codec error

		if (pHandle->info.codec->open)
		{
			if (pHandle->info.codec->open(pHandle->context[0], pHandle->pFile, &pHandle->info) == 1)
				pHandle->dwState|=V3XA_STREAM_FEED;
		}
	}
	else
	{
		pHandle->numTrack = 1;
		SYS_ASSERT(pHandle->numTrack < MAX_V3XA_TRACKS_PER_STREAM);
		for (i=0;i<pHandle->numTrack;i++)
			pHandle->context[i] = pHandle->info.codec->initialize(pHandle->chunkLength, pHandle->pStream, &MM_std);
	}

	if (!pHandle->info.chunkLength)
		pHandle->chunkLength = bestCache;
	else
		pHandle->chunkLength = pHandle->info.chunkLength;

	SYS_ASSERT(pHandle->chunkLength);	

	
	chunkLength = pHandle->chunkLength * 2;
	pHandle->dwLength = pHandle->info.length;	

	SYS_ASSERT(pHandle->dwLength);
	SYS_ASSERT(pHandle->info.length);	

	for (i=0;i<pHandle->numTrack;i++)
	{
		pHandle->sample[i] = MM_std.malloc(pHandle->chunkLength);
		pHandle->handle[i] = V3XA.Client->StreamInitialize(pHandle->info.sampleFormat, 
			pHandle->info.samplingRate, chunkLength);
	}

	SYS_ASSERT(pHandle!=g_pStreams);
	*stream = (int)(pHandle - g_pStreams);
	return 0;
}
