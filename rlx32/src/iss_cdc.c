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
// Internal define for codec.
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

extern int SFX_DecodeOGG(SYS_FILEHANDLE in, int options, V3XA_HANDLE *sinfo);

/* Windows WAVE File Encoding Tags */
#define WAVE_FORMAT_UNKNOWN		0x0000 /* Unknown Format */
#ifndef _XBOX
#define WAVE_FORMAT_PCM			0x0001 /* PCM */
#endif
#define WAVE_FORMAT_ADPCM		0x0002 /* Microsoft ADPCM Format */
#define WAVE_FORMAT_IEEE_FLOAT		0x0003 /* IEEE Float */
#define WAVE_FORMAT_VSELP		0x0004 /* Compaq Computer's VSELP */
#define WAVE_FORMAT_IBM_CSVD		0x0005 /* IBM CVSD */
#define WAVE_FORMAT_ALAW		0x0006 /* ALAW */
#define WAVE_FORMAT_MULAW		0x0007 /* MULAW */
#define WAVE_FORMAT_OKI_ADPCM		0x0010 /* OKI ADPCM */
#define WAVE_FORMAT_DVI_ADPCM		0x0011 /* Intel's DVI ADPCM */
#define WAVE_FORMAT_MEDIASPACE_ADPCM	0x0012 /*Videologic's MediaSpace ADPCM*/
#define WAVE_FORMAT_SIERRA_ADPCM	0x0013 /* Sierra ADPCM */
#define WAVE_FORMAT_G723_ADPCM		0x0014 /* G.723 ADPCM */
#define WAVE_FORMAT_DIGISTD		0x0015 /* DSP Solution's DIGISTD */
#define WAVE_FORMAT_DIGIFIX		0x0016 /* DSP Solution's DIGIFIX */
#define WAVE_FORMAT_DIALOGIC_OKI_ADPCM	0x0017 /* Dialogic OKI ADPCM */
#define WAVE_FORMAT_MEDIAVISION_ADPCM	0x0018 /* MediaVision ADPCM */
#define WAVE_FORMAT_CU_CODEC		0x0019 /* HP CU */
#define WAVE_FORMAT_YAMAHA_ADPCM	0x0020 /* Yamaha ADPCM */
#define WAVE_FORMAT_SONARC		0x0021 /* Speech Compression's Sonarc */
#define WAVE_FORMAT_TRUESPEECH		0x0022 /* DSP Group's True Speech */
#define WAVE_FORMAT_ECHOSC1		0x0023 /* Echo Speech's EchoSC1 */
#define WAVE_FORMAT_AUDIOFILE_AF36	0x0024 /* Audiofile AF36 */
#define WAVE_FORMAT_APTX		0x0025 /* APTX */
#define WAVE_FORMAT_AUDIOFILE_AF10	0x0026 /* AudioFile AF10 */
#define WAVE_FORMAT_PROSODY_1612	0x0027 /* Prosody 1612 */
#define WAVE_FORMAT_LRC			0x0028 /* LRC */
#define WAVE_FORMAT_AC2			0x0030 /* Dolby AC2 */
#define WAVE_FORMAT_GSM610		0x0031 /* GSM610 */
#define WAVE_FORMAT_MSNAUDIO		0x0032 /* MSNAudio */
#define WAVE_FORMAT_ANTEX_ADPCME	0x0033 /* Antex ADPCME */
#define WAVE_FORMAT_CONTROL_RES_VQLPC	0x0034 /* Control Res VQLPC */
#define WAVE_FORMAT_DIGIREAL		0x0035 /* Digireal */
#define WAVE_FORMAT_DIGIADPCM		0x0036 /* DigiADPCM */
#define WAVE_FORMAT_CONTROL_RES_CR10	0x0037 /* Control Res CR10 */
#define WAVE_FORMAT_VBXADPCM		0x0038 /* NMS VBXADPCM */
#define WAVE_FORMAT_ROLAND_RDAC		0x0039 /* Roland RDAC */
#define WAVE_FORMAT_ECHOSC3		0x003A /* EchoSC3 */
#define WAVE_FORMAT_ROCKWELL_ADPCM	0x003B /* Rockwell ADPCM */
#define WAVE_FORMAT_ROCKWELL_DIGITALK	0x003C /* Rockwell Digit LK */
#define WAVE_FORMAT_XEBEC		0x003D /* Xebec */
#define WAVE_FORMAT_G721_ADPCM		0x0040 /* Antex Electronics G.721 */
#define WAVE_FORMAT_G728_CELP		0x0041 /* G.728 CELP */
#define WAVE_FORMAT_MSG723		0x0042 /* MSG723 */
#define WAVE_FORMAT_MPEG		0x0050 /* MPEG Layer 1,2 */
#define WAVE_FORMAT_RT24		0x0051 /* RT24 */
#define WAVE_FORMAT_PAC			0x0051 /* PAC */
#define WAVE_FORMAT_MPEGLAYER3		0x0055 /* MPEG Layer 3 */
#define WAVE_FORMAT_CIRRUS		0x0059 /* Cirrus */
#define WAVE_FORMAT_ESPCM		0x0061 /* ESPCM */
#define WAVE_FORMAT_VOXWARE		0x0062 /* Voxware (obsolete) */
#define WAVE_FORMAT_CANOPUS_ATRAC	0x0063 /* Canopus Atrac */
#define WAVE_FORMAT_G726_ADPCM		0x0064 /* G.726 ADPCM */
#define WAVE_FORMAT_G722_ADPCM		0x0065 /* G.722 ADPCM */
#define WAVE_FORMAT_DSAT		0x0066 /* DSAT */
#define WAVE_FORMAT_DSAT_DISPLAY	0x0067 /* DSAT Display */
#define WAVE_FORMAT_VOXWARE_BYTE_ALIGNED 0x0069 /* Voxware Byte Aligned (obsolete) */
#define WAVE_FORMAT_VOXWARE_AC8		0x0070 /* Voxware AC8 (obsolete) */
#define WAVE_FORMAT_VOXWARE_AC10	0x0071 /* Voxware AC10 (obsolete) */
#define WAVE_FORMAT_VOXWARE_AC16	0x0072 /* Voxware AC16 (obsolete) */
#define WAVE_FORMAT_VOXWARE_AC20	0x0073 /* Voxware AC20 (obsolete) */
#define WAVE_FORMAT_VOXWARE_RT24	0x0074 /* Voxware MetaVoice (obsolete) */
#define WAVE_FORMAT_VOXWARE_RT29	0x0075 /* Voxware MetaSound (obsolete) */
#define WAVE_FORMAT_VOXWARE_RT29HW	0x0076 /* Voxware RT29HW (obsolete) */
#ifndef _XBOX
#define WAVE_FORMAT_VOXWARE_VR12	0x0077 /* Voxware VR12 (obsolete) */
#define WAVE_FORMAT_VOXWARE_VR18	0x0078 /* Voxware VR18 (obsolete) */
#endif
#define WAVE_FORMAT_VOXWARE_TQ40	0x0079 /* Voxware TQ40 (obsolete) */
#define WAVE_FORMAT_SOFTSOUND		0x0080 /* Softsound */
#define WAVE_FORMAT_VOXWARE_TQ60	0x0081 /* Voxware TQ60 (obsolete) */
#define WAVE_FORMAT_MSRT24		0x0082 /* MSRT24 */
#define WAVE_FORMAT_G729A		0x0083 /* G.729A */
#define WAVE_FORMAT_MVI_MV12		0x0084 /* MVI MV12 */
#define WAVE_FORMAT_DF_G726		0x0085 /* DF G.726 */
#define WAVE_FORMAT_DF_GSM610		0x0086 /* DF GSM610 */
#define WAVE_FORMAT_ISIAUDIO		0x0088 /* ISIAudio */
#define WAVE_FORMAT_ONLIVE		0x0089 /* Onlive */
#define WAVE_FORMAT_SBC24		0x0091 /* SBC24 */
#define WAVE_FORMAT_DOLBY_AC3_SPDIF	0x0092 /* Dolby AC3 SPDIF */
#define WAVE_FORMAT_ZYXEL_ADPCM		0x0097 /* ZyXEL ADPCM */
#define WAVE_FORMAT_PHILIPS_LPCBB	0x0098 /* Philips LPCBB */
#define WAVE_FORMAT_PACKED		0x0099 /* Packed */
#define WAVE_FORMAT_RHETOREX_ADPCM	0x0100 /* Rhetorex ADPCM */
#define WAVE_FORMAT_IRAT		0x0101 /* BeCubed Software's IRAT */
#define WAVE_FORMAT_VIVO_G723		0x0111 /* Vivo G.723 */
#define WAVE_FORMAT_VIVO_SIREN		0x0112 /* Vivo Siren */
#define WAVE_FORMAT_DIGITAL_G723	0x0123 /* Digital G.723 */
#define WAVE_FORMAT_CREATIVE_ADPCM	0x0200 /* Creative ADPCM */
#define WAVE_FORMAT_CREATIVE_FASTSPEECH8 0x0202 /* Creative FastSpeech8 */
#define WAVE_FORMAT_CREATIVE_FASTSPEECH10 0x0203 /* Creative FastSpeech10 */
#define WAVE_FORMAT_QUARTERDECK		0x0220 /* Quarterdeck */
#define WAVE_FORMAT_FM_TOWNS_SND	0x0300 /* FM Towns Snd */
#define WAVE_FORMAT_BTV_DIGITAL		0x0400 /* BTV Digital */
#define WAVE_FORMAT_VME_VMPCM		0x0680 /* VME VMPCM */
#define WAVE_FORMAT_OLIGSM		0x1000 /* OLIGSM */
#define WAVE_FORMAT_OLIADPCM		0x1001 /* OLIADPCM */
#define WAVE_FORMAT_OLICELP		0x1002 /* OLICELP */
#define WAVE_FORMAT_OLISBC		0x1003 /* OLISBC */
#define WAVE_FORMAT_OLIOPR		0x1004 /* OLIOPR */
#define WAVE_FORMAT_LH_CODEC		0x1100 /* LH Codec */
#define WAVE_FORMAT_NORRIS		0x1400 /* Norris */
#define WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS 0x1500 /* Soundspace Music Compression */
#define WAVE_FORMAT_DVM			0x2000 /* DVM */
#define WAVE_FORMAT_EXTENSIBLE		0xFFFE /* SubFormat */
#define WAVE_FORMAT_DEVELOPMENT         0xFFFF /* Development */

/*?#define WAVE_FORMAT_IBM_MULAW	0x0101 IBM MULAW? */
/*?#define WAVE_FORMAT_IBM_ALAW		0x0102 IBM ALAW? */
/*?#define WAVE_FORMAT_IBM_ADPCM	0x0103 IBM ADPCM? */
/*?#define WAVE_FORMAT_DIVX_AUDIO160	0x00000160 DivX Audio? */
/*?#define WAVE_FORMAT_DIVX_AUDIO161	0x00000161 DivX Audio? */

#ifndef _XBOX
#define WAVE_FORMAT_XBOX_ADPCM		WAVE_FORMAT_VOXWARE_BYTE_ALIGNED
#endif

typedef struct	_wave_format 
{
	u_int16_t		wFormatTag;
	u_int16_t		nChannels;
	int			nSamplesPerSec;
	int			nAvgBytesPerSec;
	u_int16_t		nBlockAlign;
	u_int16_t		wBitsPerSample;
}WAVEFORMAT;

typedef struct _wave_chunk
{
	char			ckID[4];
	int			ckSize;
}WAVCHUNK;

void static *V3XA_PCM_Depack(SYS_FILEHANDLE fp, int length)
{
	u_int8_t *sample = (u_int8_t*) MM_heap.malloc(length);
	if (sample)
	{
        FIO_cur->fread(sample, 1, length, fp);
	}
	return sample;
}

extern int V3XA_DecodeWAV(SYS_FILEHANDLE fp, int options, V3XA_HANDLE *pHandle);

static size_t read_chunk(SYS_FILEHANDLE fp, WAVCHUNK *chunk, int tagOnly)
{
	if (tagOnly)
		return FIO_cur->fread(chunk->ckID, 4, 1, fp);
	else
	{
		size_t ret = FIO_cur->fread(chunk, 1, sizeof(WAVCHUNK), fp);
#ifdef LSB_FIRST
		BSWAP32((u_int32_t*)&chunk->ckSize, 1);
#endif
		return ret;
	}
}

extern V3XA_AUDIOCODEC* V3XA_CodecADPCM( V3XA_HANDLE *pHandle);

static int SFX_DecodeWAV(SYS_FILEHANDLE fp, int options, V3XA_HANDLE *pHandle)
{
	int dwSampleLength = 0; // Number of samples (per channel)
	WAVEFORMAT format;
	WAVCHUNK chunk;
	if (!read_chunk(fp, &chunk, 0))
		goto failed;
	
	if (memcmp(chunk.ckID, "RIFF", 4))
		goto failed;

	if (!read_chunk(fp, &chunk, 1))
		goto failed;

	if (memcmp(chunk.ckID, "WAVE", 4))
		goto failed;

	read_chunk(fp, &chunk, 0);

	while(1)
	{			
		if (!memcmp(chunk.ckID, "fmt ", 4))
			break;
		
		if (!memcmp(chunk.ckID, "fact", 4))
		{
			FIO_cur->fread(&dwSampleLength, sizeof(int), 1, fp);
			FIO_cur->fseek(fp, chunk.ckSize - 4, SEEK_CUR);
		}
		else		
			FIO_cur->fseek(fp, chunk.ckSize, SEEK_CUR);

		read_chunk(fp, &chunk, 0);

	}
	FIO_cur->fread(&format, sizeof(WAVEFORMAT), 1, fp);	

#ifdef LSB_FIRST
	BSWAP16((u_int16_t*)&format.wFormatTag, 2);
	BSWAP32((u_int32_t*)&format.nSamplesPerSec, 2);
	BSWAP16((u_int16_t*)&format.nBlockAlign, 2);	
#endif

	FIO_cur->fseek(fp, chunk.ckSize - sizeof(WAVEFORMAT), SEEK_CUR);		
	while(1)
	{			
		read_chunk(fp, &chunk, 0);
		if (!memcmp(chunk.ckID, "data", 4))
			break;
		FIO_cur->fseek(fp, chunk.ckSize, SEEK_CUR);
	}

	pHandle->samplingRate = format.nSamplesPerSec;
	pHandle->length = chunk.ckSize;

	if (format.wBitsPerSample == 16)
		pHandle->sampleFormat|= V3XA_FMT16BIT;

	if (format.nChannels==2)
		pHandle->sampleFormat|= V3XA_FMTSTEREO;

	if (options & 1)
	{
		pHandle->sample = V3XA_PCM_Depack(fp, chunk.ckSize);
	}
    
	return 0;

failed:
	return -11;
}



// Codec format 
static V3XA_FILECODEC fileCodecs[]={
    {".WAV", SFX_DecodeWAV}, // WAV
    {".OGG", SFX_DecodeOGG}, // OGG 
    {NULL , NULL           }
};



 V3XA_FILECODEC *V3XA_CodecFind(char *filename)
{
     V3XA_FILECODEC *upk = fileCodecs;
    while (upk->ext!=NULL)
    {
        if (!*upk->ext) 
			break;
		// TODO :unicode support
        if (strstr(filename, upk->ext))
        {
            return upk;
        }
        upk++;
    }
    return 0;
}

const V3XA_FILECODEC *V3XA_CodecGetByIndex(int i)
{
    return *fileCodecs[i].ext ? fileCodecs+i : 0;
}


void static *V3XA_PCM_setup(size_t size, SYS_FILEIO *fl, SYS_MEMORYMANAGER *_cMem)
{
	UNUSED(size);
	return NULL;
}

int static V3XA_PCM_release(void *context)
{
	UNUSED(context);
	return 1;
}

int static V3XA_PCM_decode(void *context, const void *indata, size_t size, void **outdata, size_t *sizePcm)
{
	UNUSED(context);
	*sizePcm = size;
	*outdata = (void*)indata;
	return 0;
}

int static V3XA_SPCM8_decode(void *context, const void *indata, size_t size, void **outdata, size_t *sizePcm)
{
	u_int8_t *s = (u_int8_t*)indata;
	u_int8_t *d;
	*sizePcm = size;
	if (!indata)
		return 0;
	d = (u_int8_t *)indata;
	*outdata = d;

	while (size>0)
	{
		*d = (*s) ^ 128;
		size--;
		s++;
		d++;
	}
	return 0;
}

// PCM
int static V3XA_SPCM16_decode(void *context, const void *indata, size_t size, void **outdata, size_t *sizePcm)
{
	u_int16_t *s = (u_int16_t*)indata;
	u_int16_t *d;
	d = (u_int16_t *)indata;
	*sizePcm = size;
	if (!indata)
		return 0;
		
	*outdata = d;	
	while (size>0)
	{
		u_int8_t *p = (u_int8_t*)s;
		*d = ((u_int16_t)p[1]) | (((u_int16_t)p[0])<<8);
		size-=2;
		s++;
		d++;
	}
	return 0;
}

V3XA_AUDIOCODEC V3XA_Codec_SPCM8 = 
{
	V3XA_PCM_setup,
	V3XA_PCM_release,
	NULL,
	V3XA_SPCM8_decode
};

V3XA_AUDIOCODEC V3XA_Codec_SPCM16 = 
{
	V3XA_PCM_setup,
	V3XA_PCM_release,
	NULL,
	V3XA_SPCM16_decode
};

V3XA_AUDIOCODEC V3XA_Codec_PCM = 
{
	V3XA_PCM_setup,
	V3XA_PCM_release,
	NULL,
	V3XA_PCM_decode
};


// Detect codec from sample info
V3XA_AUDIOCODEC *V3XA_Handle_GetCodec( V3XA_HANDLE *pHandle)
{
	return &V3XA_Codec_PCM;
}

