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
struct V3XAUDIO V3XA;

// Free sample
void V3XA_Handle_Release(V3XA_HANDLE *pHandle)
{
	if (pHandle->sample)
	{
		V3XA.Client->SmpRelease(pHandle);
		MM_std.free(pHandle->sample);
		pHandle->sample = 0;
	}
	return;
}

// Load sample from file
int V3XA_Handle_LoadFromFp(V3XA_HANDLE *pHandle, SYS_FILEHANDLE fp, const V3XA_FILECODEC *upk)
{
	int ret = 0;

	if (!upk)
		return -1;

	ret = upk->decode(fp, 1, pHandle);

	if (!pHandle->codec)
		pHandle->codec = V3XA_Handle_GetCodec(pHandle);

	if (!pHandle->chunkLength)
		pHandle->chunkLength = 64 * 1024;

	return ret;
}

// Load sample from filename
int V3XA_Handle_LoadFromFn(V3XA_HANDLE *pHandle, char *szFilename)
{
	const char *fn = (szFilename);	
	int ret = 0;
	if (fn)
	{
		SYS_FILEHANDLE fp = FIO_cur->fopen(fn, "rb");
		if (fp)
		{
			ret = V3XA_Handle_LoadFromFp(pHandle, fp, V3XA_CodecFind(szFilename)); 
			if (ret)
			{
				V3XA.Client->SmpLoad( pHandle );
			}
			FIO_cur->fclose(fp);
		}
	}
	return ret;

}
