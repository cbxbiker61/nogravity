#if (defined _WIN32 || defined _WIN64) && !defined _XBOX
#include <windows.h>
#elif defined __BEOS__
#include <KernelKit.h>
#define SUPPORT_IEEE_AUDIO
#elif defined __APPLE__ && defined __MACH__
#define SUPPORT_IEEE_AUDIO
#endif

#include "_rlx32.h"
#include "systools.h"
#include "sysresmx.h"
#include "iss_defs.h"

#define OGG_BUFFER_SIZE 	(4096)*16
#pragma pack(8) // See http://www.xiph.org/archives/vorbis/200204/0218.html
#include <vorbis/codec.h>


typedef struct _ogg_context
{
	vorbis_info 	 vi; /* struct that stores all the static vorbis bitstream	settings */
	vorbis_comment	 vc; /* struct that stores all the bitstream user comments */
	vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
	vorbis_block	 vb; /* local working space for packet->PCM decode */

	ogg_stream_state os; /* take physical pages, weld into a logical stream of packets */
	ogg_page		 og; /* one Ogg bitstream page.  Vorbis packets are inside */
	ogg_packet		 op; /* one raw packet of data for decode */
	ogg_sync_state	 oy; /* sync and verify incoming physical bitstream */

	float		**	 pcm;
	char			 uncompressed[OGG_BUFFER_SIZE];
	unsigned int	 mode;
	unsigned int	 eos;
	size_t			 input_size;
	int				 state;

}OGG_context;

extern V3XA_AUDIOCODEC V3XA_Codec_OGG;


// Entry point : must pass file manager and memory manager
static void *Initialize(size_t size, SYS_FILEIO *pFileIO, SYS_MEMORYMANAGER *pMem)
{
	OGG_context *ctx;
	ctx = (OGG_context*) MM_heap.malloc(sizeof(OGG_context));
	SYS_ASSERT(ctx);
	sysMemZero(ctx, sizeof(OGG_context));
	vorbis_info_init(&ctx->vi);
	vorbis_comment_init(&ctx->vc);
	return ctx;
}

// loop context. Positioned on the first frame

static int ReadHeader(OGG_context *ctx, SYS_FILEHANDLE file)
{
	const size_t size = 4096;
	int theError;
	void *buffer;
	int i, serialNo;
	size_t bytes = 0;

	ogg_sync_init(&ctx->oy); /* Now we can read pages */
	/* submit a 4k block to libvorbis' Ogg layer */
	if (!ctx->eos)
	{
		buffer = ogg_sync_buffer(&ctx->oy, (int32_t)size);
		bytes = FIO_cur->fread(buffer, 1 , size, file);
		ogg_sync_wrote(&ctx->oy, (int32_t)size);
	}

	/* Get the first page. */
	theError = ogg_sync_pageout(&ctx->oy, &ctx->og);
	SYS_ASSERT(theError == 1);
	if (theError!=1)
	{
		/* error case.	Must not be Vorbis data */
		SYS_ASSERT(bytes<size);
		return 0;
	}

	/* Get the serial number and set up the rest of decode. */
	/* serialno first; use it to set up a logical stream */
	serialNo = ogg_page_serialno(&ctx->og);
	theError = ogg_stream_init(&ctx->os, serialNo);
	SYS_ASSERT(theError==0);

	/* extract the initial header from the first page and verify that the
	   Ogg bitstream is in fact Vorbis data */

	/* I handle the initial header first instead of just having the code
	   read all three Vorbis headers at once because reading the initial
	   header is an easy way to identify a Vorbis bitstream and it's
	   useful to see that functionality seperated out. */

	theError = ogg_stream_pagein(&ctx->os, &ctx->og);
	SYS_ASSERT(theError>=0);
	if (theError < 0)
	{
		/* error; stream version mismatch perhaps */
		return 0;
	}
	theError = ogg_stream_packetout(&ctx->os, &ctx->op);
	SYS_ASSERT(theError == 1);
	if (theError!=1)
	{
		/* no page? must not be vorbis */
		return 0;
	}
	theError = vorbis_synthesis_headerin(&ctx->vi, &ctx->vc, &ctx->op);
	SYS_ASSERT(theError >=0);
	if (theError < 0)
	{
		return 0;
	}
	/* At this point, we're sure we're Vorbis.	We've set up the logical
	   (Ogg) bitstream decoder.  Get the comment and codebook headers and
	   set up the Vorbis decoder */

	/* The next two packets in order are the comment and codebook headers.
	   They're likely large and may span multiple pages.  Thus we reead
	   and submit data until we get our two pacakets, watching that no
	   pages are missing.  If a page is missing, error out; losing a
	   header page is the only place where missing data is fatal. */

	i=0;
	while(i<2)
	{
		while(i<2)
		{
			int result = ogg_sync_pageout(&ctx->oy, &ctx->og);
			if (result==0)
				break; /* Need more data */
			/* Don't complain about missing or corrupt data yet.  We'll
			catch it at the packet output phase */
			if(result==1)
			{
				ogg_stream_pagein(&ctx->os, &ctx->og); /* we can ignore any errors here
				as they'll also become apparent
				at packetout */
				while(i<2)
				{
					result = ogg_stream_packetout(&ctx->os, &ctx->op);
					if (result==0)
						break;
					if (result<0)
					{
						/* Uh oh; data at some point was corrupted or missing!
						We can't tolerate that in a header.  Die. */
						SYS_ASSERT(0);
						return 0;
					}
					vorbis_synthesis_headerin(&ctx->vi, &ctx->vc, &ctx->op);
					i++;
				}
			}
		}
		buffer = ogg_sync_buffer(&ctx->oy, (int32_t)size);
		bytes  = FIO_cur->fread(buffer, 1 , size, file);
		SYS_ASSERT(!(bytes==0 && i<2));
		ogg_sync_wrote(&ctx->oy,(int32_t) size);
	}
	/* OK, got and parsed all three headers. Initialize the Vorbis
	 packet->PCM decoder. */
	vorbis_synthesis_init(&ctx->vd, &ctx->vi); /*	central decode state */
	vorbis_block_init(&ctx->vd, &ctx->vb);		/*	local state for most of the decode
													so multiple block decodes can
													proceed in parallel.  We could init
													multiple vorbis_block structures
													for vd here */

	ctx->input_size = size;
	ctx->state = 0;
	return 1;
}


static int Release(void *context)
{
	OGG_context *ctx = (OGG_context*)context;

	/* clean up this logical bitstream; before exit we see if we're
	   followed by another [chained] */

	ogg_stream_clear(&ctx->os);

	/* ogg_page and ogg_packet structs always point to storage in
	   libvorbis.  They're never freed or manipulated directly */

	vorbis_block_clear(&ctx->vb);
	vorbis_dsp_clear(&ctx->vd);
	vorbis_comment_clear(&ctx->vc);
	vorbis_info_clear(&ctx->vi);  /* must be called last */

	MM_heap.free(ctx);

	return 1;
}

static int Open(void *context, SYS_FILEHANDLE file, V3XA_HANDLE *info)
{
	OGG_context *ctx = (OGG_context*)context;
	ReadHeader(ctx, file);

	if (info)
	{
		int playLength, fileSize;
		info->samplingRate = ctx->vi.rate;
#ifdef SUPPORT_IEEE_AUDIO
		info->sampleFormat = V3XA_FMTIEEE;
#else
		info->sampleFormat = V3XA_FMT16BIT;
#endif
		if (ctx->vi.channels >= 2)
			info->sampleFormat|= V3XA_FMTSTEREO;
		fileSize = FIO_cur->fsize(file);
		playLength = (fileSize * 8) / ctx->vi.bitrate_nominal;
		info->length = playLength * ctx->vi.rate << 1;
		info->loopend = 0;
		info->loopstart = 0;
		info->numTracks = 0;
		info->codec = &V3XA_Codec_OGG;
	}
	return 1;
}

#ifdef SUPPORT_IEEE_AUDIO

static void DeinterleaveAudio(void *outdata, 
							   float **pcm, 
							  int channels, size_t n)
{
	size_t j;
	int i;
	SYS_ASSERT(outdata);
	for(i=0;i<channels;i++)
	{
		const float *mono = pcm[i];
		float *ptr = (float *)outdata + i;
		for(j=0;j<n;j++,ptr+=channels,mono++)
		{
			*ptr = *mono;
		}
	}
}
#else

/* convert floats to 16 bit signed ints (host order) and
   interleave */
#define FIST_MAGIC ((((65536.0 * 65536.0 * 16)+(65536.0 * 0.5))* 65536.0))

__inline int Round(float inval)
{
	double dtemp = FIST_MAGIC + inval;
	return ((*(int *)&dtemp) - 0x80000000);
}

static void ConvertFloatToInteger(void *outdata, 
								  
								  float **pcm, 
								  int channels, size_t n)
{
	size_t j;
	int i;
	SYS_ASSERT(outdata);
	for(i=0;i<channels;i++)
	{
		const float *mono = pcm[i];
		ogg_int16_t *ptr = (ogg_int16_t *)outdata + i;
		for(j=0;j<n;j++,ptr+=channels,mono++)
		{
			int v = Round((*mono)*32767.f);
#if 1
			if (v> 32767)
				v= 32767;
			if (v<-32768)
				v=-32768;
#endif
			*ptr = (int)v;
		}
	}
}
#endif

// Decode
static int Decode(void *context, const void *dataRAW, 
					size_t sizeRAW, void **ppdataPCM, size_t *sizePCM)
{
	OGG_context *ctx = (OGG_context*)context;

	*sizePCM = 0;
	*ppdataPCM = 0;
	
	if (!dataRAW)
		return 0;

	switch(ctx->state)
	{
		// Sync buffer
		case 0xff:			
		{
			sysMemCpy(ogg_sync_buffer(&ctx->oy, (int32_t)sizeRAW), dataRAW, sizeRAW);
			ctx->input_size = sizeRAW;
			ogg_sync_wrote(&ctx->oy, (int32_t)ctx->input_size);
			ctx->state = 0;
		}
		break;
		// Sync page out
		case 0:
		{
			int result = ogg_sync_pageout(&ctx->oy, &ctx->og);			
			if (result == 0)
			{
				ctx->state = 0xff;
			}
			else
			if (result < 0)
			{
				ctx->state = -1;
			}
			else
			{
				ogg_stream_pagein(&ctx->os, &ctx->og); /* can safely ignore errors at this point */
				ctx->state = 1;
			}
		}
		break;

		// Stream packet out
		case 1:
		{
			int result = ogg_stream_packetout(&ctx->os, &ctx->op);			
			if (result == 0)
			{
				ctx->state = 0;
			}
			else
			if (result < 0)
			{
				ctx->state = -2;
			}
			else
			{
				/* we have a packet.	Decode it */
				if (vorbis_synthesis(&ctx->vb, &ctx->op)==0) /* test for success! */
					vorbis_synthesis_blockin(&ctx->vd, &ctx->vb);
				ctx->state = 3;
			}
		}
		break;

		// synthetis pcm out
		case 3:
		{
			size_t samples;
			float **pcm;		
			samples = vorbis_synthesis_pcmout(&ctx->vd, &pcm);
			if (samples<=0)
			{
				ctx->state = 1;
			}
			else
			{
				size_t convsize = ctx->input_size / ctx->vi.channels;
				size_t bytesCount = samples < convsize ? samples : convsize;
				SYS_ASSERT(bytesCount < OGG_BUFFER_SIZE);
#ifdef SUPPORT_IEEE_AUDIO
				DeinterleaveAudio(ctx->uncompressed, pcm, ctx->vi.channels, bytesCount);				
				*sizePCM = 4 * ctx->vi.channels * bytesCount;
#else
				ConvertFloatToInteger(ctx->uncompressed, pcm, ctx->vi.channels, bytesCount);
				*sizePCM = 2 * ctx->vi.channels * bytesCount;
#endif
				/* tell libvorbis how many samples we actually consumed */
				vorbis_synthesis_read(&ctx->vd, (int32_t)bytesCount); 				
				*ppdataPCM = ctx->uncompressed;
			}
		}
		break;

		// Invalid state
		default:
			SYS_ASSERT(0);
			return -1;
		break;

	}
	return ctx->state != 0xff ? 1 : 0;
}

V3XA_AUDIOCODEC V3XA_Codec_OGG =
{
	Initialize,
	Release,
	Open,
	Decode
};

int SFX_DecodeOGG(SYS_FILEHANDLE fp, int options, V3XA_HANDLE *pHandle)
{
	if (!pHandle->chunkLength)
		pHandle->chunkLength = OGG_BUFFER_SIZE;


	pHandle->codec = &V3XA_Codec_OGG;

	return 1;	
}
