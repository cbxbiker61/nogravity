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
#include <windows.h>
#include <dinput.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>

#include "_rlx32.h"
#include "_rlx.h"
#include "_stub.h"
#include "sysctrl.h"
#include "systools.h"
#include "resource.h"
#include "win32/sys_di.h"

#ifdef _DEBUG
// #define _DEBUG_EXCEPTION
#endif

#if defined  _DEBUG_EXCEPTION// Exception manager by Konstantin Boukreev
#include "../exception/se_translator.h"
#include "../exception/debug_stream.h"
#include "../exception/exception2.h"
#endif

#define STUB_SIG "application/x-vnd.Realtech-nogravity"

static char szActualFolder[_MAX_PATH];

static BOOL g_bIsActive;
static int g_nCmdShow;
static int g_bQuit;

static HDC g_hDC;
char tmp[2048];
static HWND g_hWnd;
static HINSTANCE g_hInstance;
static RECT g_cRect;

LPDIRECTINPUT8  g_lpDI;

#define WM_MOUSEWHEEL       0x020a
#define WHEEL_DELTA         120
#define GET_X_PARAM(param)   (param&0xffff)
#define GET_Y_PARAM(param)  ((param>>16)&0xffff)

static void OnClose(void)
{
    ShowWindow(g_hWnd, SW_HIDE);
    ShowCursor(TRUE);
    UpdateWindow(g_hWnd);
    SetCurrentDirectory(szActualFolder);
}

static void SetFocusApp(bool b)
{
	g_bIsActive = b;
}

static void OnSwitchApp(WPARAM wParam, LPARAM lParam)
{
	if (wParam == TRUE)
		SetFocusApp(TRUE);
}

static int32_t MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
    {
        case WM_ACTIVATEAPP:
	        OnSwitchApp(wParam, lParam);
        break;
		case WM_CLOSE:
		{
			g_bQuit = 1;
			STUB_Down();
			OnClose();
		}
		break;
		case WM_DESTROY:
		{
			g_bQuit = 1;
			OnClose();
			PostQuitMessage(0);
		}
        break;
		// Keyboard
		case WM_KEYUP:
		case WM_KEYDOWN:
		if (sKEY)
		{
			int push = (msg==WM_KEYDOWN) ? 0x80 : 0;
			sKEY->scanCode = (uint8_t)((lParam >> 16) & 0xff) | (uint8_t)((lParam >> 17) & 0x80);
			if (!push)
			{
				sKEY->charCode = 0;
				sKEY->scanCode = 0;
			}
		}
		break;

        case WM_CHAR :
        case WM_SYSCHAR:
        case WM_IME_CHAR:
			sKEY->charCode = (uint8_t)(wParam & 0xff);
        break;
		case WM_POWERBROADCAST:
        switch( wParam )
        {
            case PBT_APMQUERYSUSPEND:
				SetFocusApp(FALSE);
                return TRUE;

            case PBT_APMRESUMESUSPEND:
				SetFocusApp(TRUE);
                return TRUE;
        }
        break;

		case WM_SYSCOMMAND:
        // Prevent moving/sizing and power loss in fullscreen mode
        switch( wParam )
        {
			case SC_SCREENSAVE:
            case SC_MONITORPOWER:
				SetFocusApp(FALSE);
			case SC_MINIMIZE:
				return FALSE;
			break;
            case SC_MAXIMIZE:
				SetFocusApp(TRUE);
			case SC_MOVE:
            case SC_SIZE:
            case SC_KEYMENU:
            break;
        }
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int STUB_TaskControl(void)
{
    MSG msg;

	do
    {
        while (PeekMessage(&msg, g_hWnd, 0, 0, PM_REMOVE)!=0)
        {
			TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		if (g_bQuit)
			return -1;

		if (!g_bIsActive)
			Sleep(100);

    }while(g_bIsActive!=1);

	return g_bQuit ? -1 : 0;
}

static void GetModuleDirectory(LPSTR defaut)
{
    int i;
	char tex[256];
    if (defaut)
    {
        sysStrCpy(tex, defaut); //path override
    }
    else
    {
        GetModuleFileName(NULL, tex, 256);
    }
    for (i=strlen(tex); i>=0 ; i--)
    {
        if (tex[i] == '\\')
        {
            if ((i != 0) && (tex[i-1] == '\\'))
            tex[i-1] = 0;
            else
            tex[i] = 0;
            break;
        }
    }
    GetCurrentDirectory(256, szActualFolder);
    SetCurrentDirectory(tex);
}

static HWND CreateAppWindow(HINSTANCE hInstance)
{
	WNDCLASS WndClass;
	sysMemZero(&WndClass, sizeof(WNDCLASS));
	// WndClass.hIcon = LoadIcon(hInstance, "STUB_ICON");
	WndClass.lpszClassName = STUB_SIG;
	WndClass.hbrBackground =(HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hInstance = hInstance;
	WndClass.style = CS_VREDRAW | CS_HREDRAW;
	WndClass.lpfnWndProc = (WNDPROC) MessageHandler;

	if (!RegisterClass(&WndClass))
		return 0;

	return CreateWindowEx(
		WS_EX_APPWINDOW,
		RLX.Dev.TeamSignature,
		RLX.Dev.ApplicationName,
		WS_POPUP|WS_MINIMIZEBOX|WS_CAPTION,
		0, 0,
		1,
		1,
		0,
		0,
		g_hInstance,
		0
    );
}

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG_EXCEPTION
	se_translator	translator;
	std::ostringstream	os;
	try
	{
#endif
    g_hInstance = hInstance;
	g_bQuit = 0;

    DirectInput8Create(g_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_lpDI, NULL);
	if (!g_lpDI)
		return -2;

    GetModuleDirectory(NULL);
    STUB_OsStartup(lpCmdLine);
    STUB_Default();

    HWND h = CreateAppWindow(hInstance);
	if (!h)
		return -1;


	g_hWnd = h;

	STUB_OsCustom(lpCmdLine);
    STUB_CheckUp(h);

    STUB_ReadyToRun();
    RLX.System.Running = true;

    STUB_MainCode();
    STUB_Down();
    STUB_QuitRequested();

    OnClose();

    if (g_lpDI)
    {
        g_lpDI->Release();
        g_lpDI = NULL;
    }

#ifdef _DEBUG_EXCEPTION
	}
	catch(se_translator::access_violation& ex)
	{
		os	<< "*" << ex.name() << " at 0x" << std::hex << ex.address()
			<< ", thread attempts to " << (ex.is_read_op() ? "read" : "write")
			<< " at 0x" << std::right << (int) ex.inaccessible_address() << std::endl
			<< std::endl
			<< "Stack : " << std::endl;
		sym_engine::stack_trace(os, ex.info()->ContextRecord);
		::MessageBox(::GetActiveWindow(), (os.str().c_str()), ("Generic exception"), MB_ABORTRETRYIGNORE|MB_ICONERROR);
	}
	catch(se_translator::no_memory& ex)
	{

		os  << "*" << ex.name() << " at 0x" << std::hex << ex.address()
			<< ", unable to allocate " << std::dec << (int)ex.mem_size() << " bytes" << std::endl
			<< std::endl
			<< "Stack : " << std::endl;
		sym_engine::stack_trace(os, ex.info()->ContextRecord);
		::MessageBox(::GetActiveWindow(), (os.str().c_str()), ("Generic exception"), MB_ABORTRETRYIGNORE|MB_ICONERROR);
	}
	catch(exception2& ex)
	{
		os  << "*" << ex.what() << std::endl
			<< std::endl
			<< "Stack :" << std::endl
			<< ex.stack_trace() << std::endl;
		::MessageBox(::GetActiveWindow(), (os.str().c_str()), ("Generic exception"), MB_ABORTRETRYIGNORE|MB_ICONERROR);
	}

#endif	// _DEBUG_EXCEPTION

    return 0;
}

