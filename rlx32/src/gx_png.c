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

#include <zlib.h>
#define _ZLIB_H
#include <png.h>

#include "_rlx32.h"
#include "systools.h"
#include "sysresmx.h"
#include "gx_rgb.h"
#include "gx_cdc.h"

#ifndef PNG_USER_MEM_SUPPORTED
#error "Please compile with PNG_USER_MEM_SUPPORTED"
#endif

static void CALLING_C
pngx_readm_pData(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_size_t check;

	/* fread() returns 0 on error, so it is OK to store this in a png_size_t
	* instead of an int, which is what fread() actually returns.
	*/
	check = (png_size_t)FIO_cur->fread(data, (png_size_t)1, length,
	  (SYS_FILEHANDLE)png_ptr->io_ptr);

	if (check != length)
	{
		png_error(png_ptr, "**read error**");
	}
}

static png_voidp pngx_malloc(png_structp png_ptr, png_uint_32 size)
{
	UNUSED(png_ptr);
	SYS_ASSERT(size>0);
	return MM_std.malloc(size);
}

static void pngx_free(png_structp png_ptr, png_voidp ptr)
{
	UNUSED(png_ptr);
	SYS_ASSERT(ptr!=0);
	MM_std.free(ptr);
}

static void pngx_error(png_structp png_ptr, png_const_charp message)
{
#ifdef _DEBUG
	SYS_Msg("!%s", message);
#endif
	longjmp(png_ptr->jmpbuf, 1);
}

static void pngx_warning(png_structp png_ptr, png_const_charp message)
{
#ifdef _DEBUG
	SYS_Msg("!%s", message);
#endif
	UNUSED(png_ptr);
}

uint8_t *PNG_unpack(SYS_FILEHANDLE infile, uint32_t *LX, uint32_t *LY, int *dwBitsPerPixel, rgb24_t *pClut, int options)
{
    uint8_t *out_buffer;
	uint32_t i;
	unsigned char sig[8];
	png_structp png_ptr;
	png_infop info_ptr;

	FIO_cur->fread(sig, 1, 8, infile);
	if (!png_check_sig(sig, 8))
	{
		return NULL;	/* bad signature */
	}
	png_ptr = png_create_read_struct_2(

	PNG_LIBPNG_VER_STRING,
	(png_voidp)NULL,
	(png_error_ptr)pngx_error,
	(png_error_ptr)pngx_warning,
	(png_voidp)NULL,
	(png_malloc_ptr)pngx_malloc,
	(png_free_ptr)pngx_free);

	png_init_io(png_ptr, (FILE*)infile);
	png_set_sig_bytes(png_ptr, 8);	/* we already read the 8 signature bytes */
	png_set_read_fn(png_ptr, (FILE*)infile, pngx_readm_pData);

if (!pClut)
		pClut = (rgb24_t*)MM_heap.malloc(3*256);

	info_ptr = png_create_info_struct(png_ptr);
	png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */

	if (setjmp(png_ptr->jmpbuf))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return NULL;
	}

	out_buffer = 0;

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE))
	{
		png_colorp palette;
		int num_palette;
		png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
		if (palette)
			sysMemCpy(pClut, palette, num_palette * 3);
	}
	else
	{
		for (i=0;i<256;i++)
			pClut[i].r = pClut[i].g = pClut[i].b = (uint8_t)i;
	}

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA))
	{
		double gamma;
		png_get_gAMA(png_ptr, info_ptr, &gamma);
	}

	if (options & 1)
	{
		png_uint_32 width, height;
		int bit_depth, rowbytes;
		int color_type;
		uint8_t **row_pointers = NULL;

		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

		if (bit_depth<8)
			png_set_packing(png_ptr);

		if ((color_type == PNG_COLOR_TYPE_RGB_ALPHA && bit_depth>8))
			png_set_bgr(png_ptr);

		png_read_update_info(png_ptr, info_ptr);
		rowbytes = png_get_rowbytes(png_ptr, info_ptr);

		out_buffer	 = (uint8_t*) MM_heap.malloc(height * rowbytes);
		row_pointers = (uint8_t**) MM_std.malloc(sizeof(char*)*height);
		SYS_ASSERT(row_pointers!=0);
		for (i=0;i<height;i++)
			row_pointers[i] = out_buffer + i * rowbytes;
		png_read_image(png_ptr, row_pointers);
		MM_std.free(row_pointers);
		png_read_end(png_ptr, NULL);

		*LX  = width;
		*LY  = height;
		*dwBitsPerPixel = (rowbytes<<3) / width;
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	}
	return out_buffer;
}

