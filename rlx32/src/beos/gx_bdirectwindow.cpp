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

#include "gx_bdirectwindow.h"
#include "rlx_system"
#include "rlx_gx"
#include "_rlx.h"
#include "beos/stub.h"

extern "C" struct RLXSYSTEM	    *	g_pRLX;

extern sysApplication		*	g_pApp;

inline struct GXSYSTEM *GET_GX() { return g_pRLX->pGX; }

GX_BDirectWindow			*	GX_BDirectWindow::m_pInstance;

static long RenderThread( void *data )
{
	return g_pApp->RunApplication(data);
}

GX_BDirectWindow::GX_BDirectWindow(BRect window_rect, const char *pTitle)
: BDirectWindow(window_rect, pTitle, B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE|B_NOT_ZOOMABLE)
{
    m_pInstance = this;
	m_bConnected = false;
	m_bDirty = false;

	BView *view = g_pApp->CreateSubview(this);
	if (view)
	{
	   AddChild(view);
	}
	AddShortcut('\n', B_COMMAND_KEY, new BMessage(MSG_SYSAPPLICATION_SWITCHFS));
	AddShortcut('x', B_COMMAND_KEY, new BMessage(MSG_SYSAPPLICATION_CLOSE));

	m_SemID = create_sem(1, "BDirectWindow lock sem");

	m_DrawThread = spawn_thread(RenderThread, "drawing_thread", B_NORMAL_PRIORITY, (void *) this);
	if (m_DrawThread<0)
	{
		printf("Can't spawn drawing thread, bailing out...\n" );
		
	}
	Center();

	resume_thread(m_DrawThread);
}

GX_BDirectWindow::~GX_BDirectWindow()
{

}

void GX_BDirectWindow::Center()
{
	BRect rect = Bounds();
	BScreen b;
	BRect bounds = b.Frame();
	float32_t w = bounds.right, h = bounds.bottom;
	MoveTo( (w - (rect.right - rect.left)) / 2, (h - (rect.bottom - rect.top)) / 2 );
	return;
}

void GX_BDirectWindow::Create()
{
    return;
}

void GX_BDirectWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case MSG_SYSAPPLICATION_CLOSE:
			printf("Close thread BDirectWindow\n");
			g_pApp->Kill();
		break;
	
		case MSG_SYSAPPLICATION_OPEN:
			printf("Resume thread BDirectWindow\n");
			resume_thread(m_DrawThread);
			
		break;
		case MSG_SYSAPPLICATION_SWITCHFS:
			printf("Toggle fullscreen BDirectWindow\n");
			SetFullScreen(!IsFullScreen());
		break;
	
		default:
			g_pApp->WindowMessage(msg);
			BDirectWindow::MessageReceived(msg);
		break;
	}
}

void GX_BDirectWindow::WindowActivated(bool active)
{	
	BDirectWindow::WindowActivated(active);
}

void GX_BDirectWindow::ForceRedraw()
{
     return;
}

void GX_BDirectWindow::Quit(void)
{
	printf("Quit window\n");
	Hide();
	Sync();
	if (m_SemID)
		delete_sem(m_SemID);
	m_SemID = 0;
	g_pApp->StopDrawing();
	g_pApp->Kill();
	BDirectWindow::Lock();
	BDirectWindow::Quit();

}

bool GX_BDirectWindow::QuitRequested()
{
 	if (!g_pApp->IsStopped())
	{
		g_pApp->Stop();
		g_pApp->SetActive(false);
	}
	return false;
}

void GX_BDirectWindow::DirectConnected(direct_buffer_info *info)
{
	switch(info->buffer_state & B_DIRECT_MODE_MASK) {
		case B_DIRECT_START:
		{
			m_bConnected = true;
			g_pApp->StartDrawing();
		}
		case B_DIRECT_MODIFY:
		{
			m_pBits = (uint8 *) info->bits;
			fRowBytes = info->bytes_per_row;
			GET_GX()->View.BitsPerPixel = 
			GX_BDirectWindow::m_pInstance->BitsPerPixel = info->bits_per_pixel;
			GET_GX()->View.BytePerPixel = GET_GX()->View.BitsPerPixel >> 3;
			fFormat = info->pixel_format;
			fBounds = info->window_bounds;
			m_bDirty = true;
		}
		break;
		case B_DIRECT_STOP:
		{
			m_bConnected = false;
			g_pApp->StopDrawing();
		}
		break;
	}
	return;
}
