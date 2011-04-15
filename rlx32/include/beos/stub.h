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

#ifndef _STUB_BE_HH
#define _STUB_BE_HH

#include <Be.h>

class sysApplication : public BApplication
{
public:
		sysApplication();
		bool        QuitRequested();
		void        ReadyToRun ();

		virtual long	    RunApplication(void *param);

		static sysApplication * GetInstance() { return m_pInstance; }
		static sysApplication * m_pInstance;

		virtual BView	*	CreateSubview(BWindow *h);
		virtual void		StartDrawing();
		virtual void		StopDrawing();
		virtual void		Stop();
		virtual void		WindowMessage(BMessage *msg);
		virtual void		MessageReceived(BMessage *msg);
		virtual bool		IsStopped();
		virtual void		Kill();
		virtual bool		IsFocused();
		virtual void		SetActive(bool active);
		virtual void		SetWindowHandle(BWindow *h);

		BWindow	*	m_hWnd;
		BView	*	m_hView;
		bool		m_bActive;
		bool		m_bStopped;
};

static const uint32 MSG_SYSAPPLICATION_SWITCHFS = 'swfs';
static const uint32 MSG_SYSAPPLICATION_CLOSE = 'clos';
static const uint32 MSG_SYSAPPLICATION_OPEN = 'open';

#endif

