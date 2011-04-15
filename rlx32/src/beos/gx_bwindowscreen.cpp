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

#include "_rlx32.h"
#include "rlx_gx"
#include "gx_bwindowscreen.h"

#include "beos/stub.h"

extern struct RLXSYSTEM	    *	g_pRLX;
#define GET_GX() g_pRLX->pGX

extern sysApplication	*	g_pApp;
GX_BWindowScreen	*	GX_BWindowScreen::m_pInstance;

#define DEFAULT_DISPLAY_MODE B_32_BIT_640x480

static long RenderThread( void *data)
{
	return g_pApp->RunApplication(data);
}

GX_BWindowScreen::GX_BWindowScreen (const char *pszWindowTitle, status_t *error)
: BWindowScreen(pszWindowTitle, DEFAULT_DISPLAY_MODE, error,
#ifdef _DEBUG
true
#else
false
#endif
),

		m_pGci(0),
		m_DisplayMode(0),
		m_Space(DEFAULT_DISPLAY_MODE),
		m_Offset(0),
		m_Pitch(0),
		m_DrawThread(0),
		m_SemID(0), m_KillID(0)

{
	m_pInstance = this;
    if (*error == B_OK)
    {
		m_SemID  = create_sem(1, "BWindowScreen m_SemID" );
		m_KillID = create_sem(0, "BWindowScreen m_KillID" );

		BView *view = g_pApp->CreateSubview(this);
		if (view)
		{
			AddChild(view);
		}
		m_DrawThread = spawn_thread( RenderThread, "BWindowScreen drawing thread", B_DISPLAY_PRIORITY, this );
		if( m_DrawThread < 0 )
		{
			printf("Can't spawn drawing thread, bailing out...\n" );
			be_app_messenger.SendMessage( B_QUIT_REQUESTED );
		}
	}
}

int GX_BWindowScreen::InitDisplayMode(int mode)
{
	acquire_sem(m_SemID);
	if (m_Space != (uint32)mode)
	{
		m_Space = mode;
		if (SetSpace(m_Space)!=B_OK)
		{
			goto failed;
		}
	}

	release_sem(m_SemID);
	return 1;
failed:
	release_sem(m_SemID);
	return 0;
}

void GX_BWindowScreen::ShowPage(int page)
{
	acquire_sem(m_SemID);
	MoveDisplayArea(0, page ? m_pGci->height : 0);
	m_Offset = page ? 0 : m_pGci->height;
	release_sem(m_SemID);
}

bool GX_BWindowScreen::QuitRequested()
{
	return false;
}

void GX_BWindowScreen::Quit()
{
	Disconnect();
	if (m_SemID)
		delete_sem(m_SemID);

	if (m_KillID)
	   delete_sem(m_KillID);

	m_SemID = 0;
	m_KillID = 0;
	g_pApp->Stop();
	g_pApp->SetActive(false);
	BWindowScreen::Quit();
}

void GX_BWindowScreen::MessageReceived(BMessage *msg)
{
	switch(msg->what) {
		case MSG_SYSAPPLICATION_CLOSE:
			printf("Close thread BWindowScreen\n");
			g_pApp->Kill();
		break;
		case MSG_SYSAPPLICATION_OPEN:
			printf("Resume BWindowScreen\n");

			break;
		default:
		    g_pApp->WindowMessage(msg);
			BWindowScreen::MessageReceived(msg);
		break;
	}
}

void GX_BWindowScreen::ScreenConnected(bool active)
{
	acquire_sem(m_SemID);
	printf("BWindowScreen connected: %d\n", active);

	if (active==TRUE)
	{
		resume_thread(m_DrawThread);
		g_pApp->StartDrawing();

		SetSpace(m_Space);
		m_pGci = CardInfo();
		m_Pitch = m_pGci->bytes_per_row;
		m_Offset = 0;

		// Setup hooks
		uint32_t * p = (uint32_t *)&define_cursor;
	    int i;
	    for (i=0;i<=14;i++)
			 p[i] = (uint32_t)CardHookAt(i+1);
		g_pApp->SetActive(true);
	}
	else
	{
		g_pApp->SetActive(false);
	}
	release_sem(m_SemID);
}

