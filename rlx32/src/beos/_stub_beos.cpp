//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2004 - realtech VR

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

#include <Be.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "_stubini.h"
#include "_stub.h"
#include "beos/stub.h"
#include "sysctrl.h"
#include "systools.h"

static status_t POSTMESSAGE(BLooper *p, BMessage *m)
{
#ifndef ZETA
    return p->PostMessage(m);
#else
	BMessenger messenger(p);
	return messenger.SendMessage(*m);	
#endif
}

class sysView : public BView
{
public :
	sysView();
	void		KeyDown(const char *bytes, int32 num);
	void		KeyUp(const char *bytes, int32 num);
	void		MouseDown(BPoint pos);
	void		MouseUp(BPoint pos);
	void		MouseMoved(BPoint point, uint32, const BMessage *);
};

sysView::sysView()
	 :BView(BRect(0, 0, 640, 480),(const char*) "subview", B_FOLLOW_ALL_SIDES, 0)
{

}

void
sysView::MouseUp(BPoint point)
{
	if (point.x < 0)
		point.x = 0;

	if (point.y < 0)
		point.y = 0;

	SYS_ASSERT(sMOU);
	if (sMOU)
	{
	   sMOU->lX = (int)point.x - sMOU->x;
	   sMOU->lY = (int)point.y - sMOU->y;	   
	   sMOU->x = (int)point.x;
	   sMOU->y = (int)point.y;
	   sysMemCpy(sMOU->steButtons, sMOU->rgbButtons, sizeof(sMOU->steButtons));
	   sMOU->rgbButtons[0] = 0;
	}
	return;
}

void
sysView::MouseDown(BPoint point)
{
    uint32 button;
	if (point.x<0)
		point.x = 0;

	if (point.y<0)
		point.y = 0;


	SYS_ASSERT(sMOU);
	if (sMOU)
	{
	   sMOU->lX = (int)point.x - sMOU->x;
	   sMOU->lY = (int)point.y - sMOU->y;
	   sMOU->x = (int)point.x;
	   sMOU->y = (int)point.y;

	   this->GetMouse(&point, &button);
	   
	   sysMemCpy(sMOU->steButtons, sMOU->rgbButtons, sizeof(sMOU->steButtons));
	   sMOU->rgbButtons[0] =  button&1;
	   sMOU->rgbButtons[1] = (button>>1)&1;
	   sMOU->rgbButtons[2] = (button>>2)&1;
	}
	return;
}

void
sysView::MouseMoved(BPoint point, uint32 transit, const BMessage *)
{
	if (point.x<0)
		point.x = 0;
	if (point.y <0)
		point.y = 0;

	SYS_ASSERT(sMOU);
    if (sMOU && transit == B_INSIDE_VIEW)
	{
	   sMOU->lX = (int)point.x - sMOU->x;
	   sMOU->lY = (int)point.y - sMOU->y;	   
	   sMOU->x = (int)point.x;
	   sMOU->y = (int)point.y;
	}
	return;
}

void
sysView::KeyDown(const char *bytes, int32 numBytes)
{
    if ( numBytes == 1 )
    {
		sKEY->charCode = bytes[0];
    }
	return;
}

void
sysView::KeyUp(const char *bytes, int32 numBytes)
{
	sKEY->charCode = 0;
}


sysApplication * sysApplication::m_pInstance;

// Poller
int STUB_TaskControl(void)
{
	sysApplication *p = sysApplication::m_pInstance;
	if (p->IsStopped())
	{
#ifdef _DEBUG
		SYS_Debug("Task control : Stop!\n");
#endif
		return -1;
	}

    while(!p->IsFocused())
    {
		if (p->IsStopped())
		{
#ifdef _DEBUG
			SYS_Debug("Task control : Stop!!\n");
#endif
			return -1;
		}
		snooze((bigtime_t)2e5);		
    }
    return 0;
}

// Main thread
long sysApplication::RunApplication(void *param)
{
#ifdef _DEBUG
	SYS_Debug("Run application thread\n");
#endif
	RLX.System.Running = true;		
    STUB_ReadyToRun();
    STUB_MainCode();
#ifdef _DEBUG
	SYS_Debug("Tear down application thread\n");
#endif
	STUB_Down();
	RLX.System.Running = false;
	{
		BMessage msg(MSG_SYSAPPLICATION_CLOSE);
		POSTMESSAGE(m_hWnd, &msg);	
	}
	be_app->Quit();
    return 0;
}

sysApplication::sysApplication (): BApplication (STUB_SIG)
{
 	m_pInstance = this;
	m_bActive = false;
	m_bStopped = false;
	m_hWnd = 0;
#ifdef _DEBUG
	SYS_Debug("OsStartup\n");
#endif
	STUB_OsStartup("");
	return;
}

bool sysApplication::QuitRequested()
{
 #ifdef _DEBUG
	SYS_Debug("QuitRequested\n");
#endif
    Stop();
    return false; // Do not quit
}

void sysApplication::ReadyToRun ()
{
#ifdef _DEBUG
	SYS_Debug("Ready to run\n");
#endif
	RLX.pApplication = this;
	STUB_Default();
	STUB_CheckUp(NULL); 
	SYS_ASSERT(m_hWnd);	

	// If got a m_hWnd, show it
	if (m_hWnd)
	{
		m_hWnd->Show();
		BMessage m(MSG_SYSAPPLICATION_OPEN);		
		POSTMESSAGE(m_hWnd, &m);
	}
	else
	{	
		BMessage m(B_QUIT_REQUESTED);
		POSTMESSAGE(this, &m);
	}	
	return;
}

BView * sysApplication::CreateSubview(BWindow *h)
{
#ifdef _DEBUG
	SYS_Debug("Create view\n");
#endif
	return new sysView();
}

void 
sysApplication::StartDrawing()
{
#ifdef _DEBUG
	SYS_Debug("Start drawing\n");
#endif
	SetActive(true);
}

void 
sysApplication::StopDrawing()
{
#ifdef _DEBUG
	SYS_Debug("Stop drawing\n");
#endif
	SetActive(false);
}

void 
sysApplication::Stop()
{
#ifdef _DEBUG
	SYS_Debug("Stop\n");
#endif
	StopDrawing();
	m_bStopped = true;
}

void 
sysApplication::SetActive(bool active)
{
	m_bActive = active;
}

void 
sysApplication::SetWindowHandle(BWindow *h)
{ 
#ifdef _DEBUG
	SYS_Debug("Set application window: %x\n", h);
#endif

	m_hWnd = h;
	return;
}

bool 
sysApplication::IsStopped()
{
	return m_bStopped;
}

bool 
sysApplication::IsFocused()
{
	return m_bActive;
}

void 
sysApplication::Kill()
{
#ifdef _DEBUG
	SYS_Debug("Kill application\n");
#endif

    StopDrawing();
	m_bStopped = true;
	return;
}

void 
sysApplication::WindowMessage(BMessage *msg)
{
	switch (msg->what) 
	{
	    case B_MOUSE_WHEEL_CHANGED:
     	{
         	float32_t dy;
	        msg->FindFloat("be:wheel_delta_y", &dy);
	        sMOU->lZ = (int)dy;
		}
		break;

	}
    return;
}

void 
sysApplication::MessageReceived(BMessage *msg)
{
	switch (msg->what) 
	{
		case MSG_SYSAPPLICATION_CLOSE:
		{
			BMessage m(B_QUIT_REQUESTED);

			if (m_hWnd)
			{
				SYS_Debug("Window quit requested\n");
				m_hWnd->Lock();
				m_hWnd->Quit();
			}
			
			while (IsFocused())
			{
				snooze(10000);
			}
			
			POSTMESSAGE(this, &m);	
		}		
		break;

		default:
			BApplication::MessageReceived(msg);
		break;
	}
    return;
}

// Main C.
int main()
{
#ifdef _DEBUG
	SYS_Debug("Starting No Gravity ...\n");
#endif
	STUB_OsCustom("");
    sysApplication *p = new sysApplication();
    p->Run();   
    delete p;

    return 0;
}

