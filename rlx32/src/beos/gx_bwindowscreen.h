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

#ifndef _GX_BWINSCREEN_HH_
#define _GX_BWINSCREEN_HH_

#include <Be.h>
#include <WindowScreen.h>

struct GK_DisplayModeSettings
{
	int32	lWidth;
	int32	lHeight;
	int32	BitsPerPixel;
	int32	Space;
};

typedef int32 (*PFCARDHOOKPROC0)(uchar *xorMask, uchar *andMask, int32 width, int32 height, int32 hotX, int32 hotY);
typedef int32 (*PFCARDHOOKPROC1)(int32 screenX, int32 screenY);
typedef int32 (*PFCARDHOOKPROC2)(bool flag);
typedef int32 (*PFCARDHOOKPROC3)(int32 startX, int32 endX, int32 startY, int32 endY, uint8 colorIndex, bool clipToRect, int16 clipLeft, int16 clipTop, int16 clipRight, int16 clipBottom);
typedef int32 (*PFCARDHOOKPROC4)(int32 startX, int32 endX, int32 startY, int32 endY, uint32 color, bool clipToRect, int16 clipLeft, int16 clipTop, int16 clipRight, int16 clipBottom);
typedef int32 (*PFCARDHOOKPROC5)(int32 left, int32 top, int32 right, int32 bottom, uint8 colorIndex);
typedef int32 (*PFCARDHOOKPROC6)(int32 left, int32 top, int32 right, int32 bottom, uint32 color);
typedef int32 (*PFCARDHOOKPROC7)(int32 sourceX, int32 sourceY, int32 destinationX, int32 destinationY, int32 width, int32 height);
typedef int32 (*PFCARDHOOKPROC8)(indexed_color_line *array, int32 numItems, bool clipToRect, int16 clipLeft, int16 clipTop, int16 clipRight, int16 clipBottom);
typedef int32 (*PFCARDHOOKPROC9)(rgb_color_line *array, int32 numItems, bool clipToRect, int16 clipLeft, int16 clipTop, int16 clipRight, int16 clipBottom);
typedef int32 (*PFCARDHOOKPROC10)(void);
typedef int32 (*PFCARDHOOKPROC11)(int32 left, int32 top, int32 right, int32 bottom);
typedef int32 (*PFCARDHOOKPROC12)(int32 startX, int32 endX, int32 startY, int32 endY, uint16 color, bool clipToRect, int16 clipLeft, int16 clipTop, int16 clipRight, int16 clipBottom);
typedef int32 (*PFCARDHOOKPROC13)(int32 left, int32 top, int32 right, int32 bottom, uint16 color);
typedef int32 (*PFCARDHOOKPROC14)(rgb_color_line *array, int32 numItems, bool clipToRect, int16 clipLeft, int16 clipTop, int16 clipRight, int16 clipBottom);

class GX_BWindowScreen : public BWindowScreen
{
public:

		GX_BWindowScreen( const char *name, status_t *error);
		bool									QuitRequested();
		virtual void							Quit();
		virtual void							ScreenConnected(bool active);
		virtual void							MessageReceived(BMessage *message);

		int 									InitDisplayMode(int mode);
		void									ShowPage(int page);

		static GX_BWindowScreen			*		GetInstance() { return m_pInstance; }
		static GX_BWindowScreen         *       m_pInstance;
		graphics_card_info				*		m_pGci;
		const struct GK_DisplayModeSettings	*	m_DisplayMode;
		uint32									m_Space;
		long									m_Offset;
		uint32									m_Pitch;
		thread_id								m_DrawThread;
		sem_id									m_SemID, m_KillID;

		PFCARDHOOKPROC0 						define_cursor;
	    PFCARDHOOKPROC1 						move_cursor;
	    PFCARDHOOKPROC2 						show_cursor;
		PFCARDHOOKPROC3 						draw_line_with_8_bit_depth;
		PFCARDHOOKPROC4 						draw_line_with_32_bit_depth;
		PFCARDHOOKPROC5 						draw_rect_with_8_bit_depth;
		PFCARDHOOKPROC6 						draw_rect_with_32_bit_depth;
		PFCARDHOOKPROC7 						blit;
		PFCARDHOOKPROC8 						draw_array_with_8_bit_depth;
		PFCARDHOOKPROC9 						draw_array_with_32_bit_depth;
		PFCARDHOOKPROC10 						sync;
		PFCARDHOOKPROC11 						invert_rect;
		PFCARDHOOKPROC12 						draw_line_with_16_bit_depth;
		PFCARDHOOKPROC13 						draw_rect_with_16_bit_depth;
		PFCARDHOOKPROC14 						draw_array_with_16_bit_depth;
};

#endif
