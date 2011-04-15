#pragma once
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

#ifndef _GX_BDirectWindow_HH
#define _GX_BDirectWindow_HH

#include <Be.h>
#include "_rlx32.h"

class GX_BDirectWindow : public BDirectWindow
{
	public:
		GX_BDirectWindow(BRect rect, const char *pTitle);
		~GX_BDirectWindow();

		virtual bool		QuitRequested();
		virtual void		MessageReceived(BMessage *msg);
		virtual void		WindowActivated(bool active);
		void				DirectConnected(direct_buffer_info *info);
		void				Create();
		virtual void		Quit();
		void				ForceRedraw(void);
		void				Center();

		int				lWidth;
		int				lHeight;
		int				BitsPerPixel;

		static GX_BDirectWindow * m_pInstance;

		long				m_SemID;
		long				m_KillID;

		thread_id			m_DrawThread;
		uint8_t		*	m_pBits;
		int32_t				fRowBytes;
		color_space			fFormat;
		clipping_rect		fBounds;

		bool				m_bConnected, m_bDirty;

};

#endif // _GX_BDirectWindow_HH

