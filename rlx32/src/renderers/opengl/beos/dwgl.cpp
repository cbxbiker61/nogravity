/****************************************************************************
; *
; *	File		:  dwgl.cpp
; *
; *	Description :  BDirectGLWindow driver
; *
; *	Copyright © Realtech VR 1995 - 2003 - http://www.v3x.net.
; *
; *	Permission to use, copy, modify, distribute and sell this software
; *	and its documentation for any purpose is hereby granted without fee,
; *	provided that the above copyright notice appear in all copies and
; *	that both that copyright notice and this permission notice appear
; *	in supporting documentation.  Realtech makes no representations
; *	about the suitability of this software for any purpose.
; *	It is provided "as is" without express or implied warranty.
; *
; ***************************************************************************/
#include "v3x_std"
#include "beos/stub.h"
#include "../gl_v3x.h"

extern struct RLXSYSTEM	    *	g_pRLX;
#define GET_GX() g_pRLX->pGX
#define GET_V3X() g_pRLX->pV3X

extern sysApplication		*	g_pApp;

V3X_BDirectGLWindow *V3X_BDirectGLWindow::m_pInstance;

// C Callback ******************************************************************************

static int RLXAPI SetDisplayMode(GXDISPLAYMODEHANDLE mode)
{
	GL_ResetViewport(1);
	V3X_BDirectGLWindow::GetInstance()->SetFullScreenMode(false, GET_GX()->View.lWidth, GET_GX()->View.lHeight);
	return 0;
}

static GXDISPLAYMODEINFO RLXAPI *EnumDisplayList(int bpp)
{
    return 0;
}

static void RLXAPI PageFlip(void)
{
    V3X_BDirectGLWindow::GetInstance()->MakeCurrent();
	V3X_BDirectGLWindow::GetInstance()->SwapBuffers();
	V3X_BDirectGLWindow::GetInstance()->ReleaseCurrent();
	return;
}

static long RenderThread( void *data )
{
	g_pApp->RunApplication(data);
	return 0;
}

#define MAX_DEVICES 32
#define MAX_DISPLAY_MODES 256

struct GLDeviceInfo
{
	uint device_id;
	uint monitor;
	char name[128];
};

struct GLDisplayModeInfo
{
	int32_t width;
    int32_t height;
    int32_t depth;
    int32_t color;
};

static GLDeviceInfo							g_cgDeviceList[MAX_DEVICES];
static GLDisplayModeInfo				    g_cgDisplayModeList[MAX_DISPLAY_MODES];
static int									m_NumDevice, m_NumVideoModes;

void V3X_BDirectGLWindow::DeviceInfo( uint32_t device_id, uint32_t monitor, const char *name, bool depth, bool stencil, bool accum )
{
	printf("Device %d:, Monitor %d, %s, depth:%d, stencil:%d, accum:%d\n" , device_id, monitor, name, depth,
    stencil, accum);

	if (m_NumDevice < MAX_DEVICES - 1)
	{
		GLDeviceInfo *p = g_cgDeviceList + m_NumDevice;
		p->device_id = device_id;
		p->monitor = monitor;
		sysStrnCpy(p->name, name, sizeof(p->name));
		m_NumDevice++;
		p[0].name[0] = 0;
    }
}

void V3X_BDirectGLWindow::VideoModeInfo( uint32_t width, uint32_t height, uint32_t color, uint32_t depth, uint32_t stencil, uint32_t accum )
{
    printf("%d x %d, %d, %d, %d, %d\n", width, height, color, depth, stencil, accum);
    if (m_NumVideoModes < MAX_DISPMODES - 1)
    {
		GLDisplayModeInfo *p = g_cgDisplayModeList + m_NumVideoModes;
		p->width = width;
		p->height = height;
		p->color = color;
		p->depth = depth;
		m_NumVideoModes++;
		p[1].width = 0;
    }
}

V3X_BDirectGLWindow::V3X_BDirectGLWindow(BRect rect, const char *title)
: BDirectGLWindow(	rect,
					title,
					B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
					B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE )
{
    m_pInstance = this;
	m_DrawThread = -1;
	m_bTearDown = false;
	m_bInitialized = false;
	m_bFS = false;
	Lock();
	AddChild(g_pApp->CreateSubview(this));
	Unlock();

	AddShortcut('\n', B_COMMAND_KEY, new BMessage(MSG_SYSAPPLICATION_SWITCHFS));
	AddShortcut('x', B_COMMAND_KEY, new BMessage(MSG_SYSAPPLICATION_OPEN));

    m_NumDevice = 0;
    m_NumVideoModes = 0;
	EnumerateDevices( BGL_MONITOR_ANY, BGL_ANY | BGL_DOUBLE, BGL_ANY, BGL_NONE, BGL_NONE );

	if (RestartGL(BGL_DEVICE_HARDWARE) < B_OK)
	{
		printf("Can't start OpenGL, bailing out...\n" );
		be_app_messenger.SendMessage( B_QUIT_REQUESTED );
    }

	Center();
	GET_V3X()->Client->SetDisplayMode = SetDisplayMode;
	GET_V3X()->Client->EnumDisplayList = SetDisplayMode;
	GET_V3X()->Client->PageFlip = PageFlip;
}

V3X_BDirectGLWindow::~V3X_BDirectGLWindow()
{
	status_t ret;
	Hide();
	Sync();
	m_bTearDown = true;
	wait_for_thread( m_DrawThread, &ret );
}

status_t V3X_BDirectGLWindow::RestartGL(uint32_t m_DeviceID)
{
	status_t ret;

	m_bInitialized = true;
	m_bTearDown = false;
	m_bFS = false;

	// Start the drawing thread.
	m_DrawThread = spawn_thread( RenderThread, "DGL drawing thread", B_DISPLAY_PRIORITY, this );
	if( m_DrawThread < 0 )
	{
		fprintf( stderr, "Can't spawn drawing thread, bailing out...\n" );
		be_app_messenger.SendMessage( B_QUIT_REQUESTED );
		return B_ERROR;
	}

	if( InitializeGL( m_DeviceID, BGL_FASTEST | BGL_DOUBLE, BGL_ANY, BGL_NONE, BGL_NONE ) < B_OK )
	{
		fprintf( stderr, "Error from InitializeGL \n" );
		be_app_messenger.SendMessage( B_QUIT_REQUESTED );
	}
	else
	{
		sysStrnCpy(GET_V3X()->Client->s_DrvName, gl_AllDevices[m_DeviceID].name);
	}

	return ret;
}

void V3X_BDirectGLWindow::Center()
{
	BScreen b;
	BRect bounds = b.Frame(), rect = Bounds();
	float32_t w = bounds.right,  h = bounds.bottom;
	MoveTo( (w - (rect.right - rect.left)) / 2, (h - (rect.bottom - rect.top)) / 2 );
}

bool V3X_BDirectGLWindow::QuitRequested()
{
	if (!g_pApp->IsStopped())
	{
		g_pApp->Stop();
		g_pApp->SetActive(false);
	}

	return false;
}

void V3X_BDirectGLWindow::MessageReceived( BMessage *msg )
{
	switch( msg->what )
	{
		case MSG_SYSAPPLICATION_SWITCHFS:
			SetFullScreenMode(!m_bFS, GET_GX()->View.lWidth, GET_GX()->View.lHeight);
		break;
		case MSG_SYSAPPLICATION_CLOSE:
			printf("Close thread BDirectWindow\n");
			QuitRequested();
		break;
		case MSG_SYSAPPLICATION_OPEN:
			printf("Resume thread BDirectWindow\n");
			resume_thread(m_DrawThread);
			g_pApp->StartDrawing();
		break;
		default:
			g_pApp->WindowMessage(msg);
			BDirectWindow::MessageReceived(msg);
		break;
	}
}

void V3X_BDirectGLWindow::FrameResized( float32_t width, float32_t height )
{
    printf("Frame resized %f x %f", width, height);
	g_pRLX->pfSetViewPort(&GET_GX()->View, width, height, 32);
	GL_InvalidateViewport();
}

BRect V3X_BDirectGLWindow::UpdateGLRect( BRect window_rect )
{
	return window_rect;
}

void V3X_BDirectGLWindow::SetFullScreenMode(bool bFs, int lx, int ly)
{
	if (!bFs)
	{
		FullscreenDisable();
		ResizeTo( lx - 1, ly - 1);
		m_bFS = false;
	}
	else
	{
		FullscreenEnable( lx, ly);
		m_bFS = true;
	}

	g_pRLX->pfSetViewPort(&GET_GX()->View, lx, ly, 32);
	GL_InvalidateViewport();
}

