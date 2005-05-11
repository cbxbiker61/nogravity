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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "_stub.h"
#include "sysctrl.h"
#include "systools.h"
static bool _bFinderLaunch = false;
static bool _bIsBundle = false; // No bundle (too large, 30Mb)
static bool _bActive;
static bool _bStopped;
static Rect _rcWindow;
static WindowPtr _hWnd;
enum 
{   
	kMenuApple = 128,
	kMenuFile = 129,
	kAppleAbout = 1,
	kFileQuit = 2
};

static void SetDefaultDirectory(char *AppName, int shouldChdir, int useBundleFolder)
{
    char parentdir[1024];
    char *c;
    
    sysStrnCpy ( parentdir, AppName, sizeof(parentdir) );
    c = (char*) parentdir;

    while (*c != '\0')     /* go to end */
        c++;

    while (*c != '/')      /* back up to parent */
        c--;
    
    *c++ = '\0';             /* cut off last part (binary name) */
  
    if (shouldChdir || useBundleFolder)
    {
		chdir (parentdir);   /* chdir to the binary app's parent */
		if (useBundleFolder)
			chdir ("../Resources/");
		else
			chdir ("../../../"); /* chdir to the .app's parent */

    }
}

// Onsize
static bool OnSize(int x, int y)
{
	return false;
}

static bool OnMove(int x, int y)
{
	return false;
}

static bool g_bFocus;

static bool IsFocused()
{
	return g_bFocus;
}

static void SetFocus(bool b)
{
	g_bFocus = b;
}

// OnClose
static bool OnClose()
{
	DisposeWindow(_hWnd);
    FlushEvents(everyEvent, 0L);
    ExitToShell();
    return false;
}

// Quit event
static pascal OSErr QuitAppleEventHandler( const AppleEvent *appleEvt, AppleEvent* reply, SInt32 refcon )
{
	#pragma unused (appleEvt, reply, refcon)
	OnClose();
	return false;
}

// Stop
static void Stop()
{
   _bStopped = 1;
}

// New application

#ifdef __cplusplus
extern "C" char **_Argv;
#endif

char **_Argv;
int _Argc;

static void OnInit(int argc, char *argv[])
{
    OSErr err;

	/* Init vars */
	_hWnd = 0;
	sysMemZero(&_rcWindow, sizeof(_rcWindow));
	_bActive = TRUE;
	_bStopped = 0;

	/* Init toolbox */
	InitCursor();
	
    err = AEInstallEventHandler( 
          kCoreEventClass,
          kAEQuitApplication,
          NewAEEventHandlerUPP(QuitAppleEventHandler), 0, false);
    if (err != noErr)
        ExitToShell();
	
	/* Copy the arguments into a global variable */
	int i;

	/* This is passed if we are launched by double-clicking */
	if ( argc >= 2 && strncmp (argv[1], "-psn", 4) == 0 ) 
	{
		_Argc = 1;
		_bFinderLaunch = 1;
	} 
	else 
	{
		_Argc = argc;
		_bFinderLaunch = 0;
	}
	_Argv = (char**) malloc (sizeof(*_Argv) * (_Argc+1));
	SYS_ASSERT (_Argv != NULL);
	for (i = 0; i < _Argc; i++)
		_Argv[i] = argv[i];
	_Argv[i] = NULL;

	/* Set Default Directory */
	SetDefaultDirectory(_Argv[0], _bFinderLaunch, _bIsBundle);
    return;
}

static void OnMenu (SInt32 menuResult)
{
	SInt16 theMenu;
	SInt16 theItem;
	MenuRef theMenuHandle;

	theMenu = HiWord(menuResult);
	theItem = LoWord(menuResult);
	theMenuHandle = GetMenuHandle(theMenu);

	switch (theMenu)
	{
		case kMenuApple:
		switch (theItem)
		{
			case kAppleAbout:
			break;
			default:
			break;
		}
		break;
		case kMenuFile:
		switch (theItem)
		{
			case kFileQuit:
			Stop();
			break;
		}
		break;
	}
	HiliteMenu(0);
	DrawMenuBar();
}

static int32_t MessageHandler(EventRecord *msg)
{
	SInt32 menuResult;
	SInt16 whatPart = 0;
    switch(msg->what)
    {
    
        case mouseUp:
		if (sMOU->u_id)
		{
			sMOU->rgbButtons[0] = 0;
		}
		break;
     	case mouseDown:
     	{
     		WindowPtr whichWindow;
			if (sMOU->u_id)
			{
				sMOU->rgbButtons[0] = 1;
			}
			whatPart = FindWindow(msg->where, &whichWindow);
    
			switch(whatPart)
     		{
				case inContent:
				   if (whichWindow != FrontWindow())
					SelectWindow (whichWindow);
 			    break;

			    case inMenuBar: // Menu bar
				{
				   DrawMenuBar();
				   menuResult = MenuSelect(msg->where);
				   if (HiWord(menuResult) != 0)
				   	   OnMenu(menuResult);
				}
			    break;
				case inDrag: // Track bar
				{
					Rect bounds;
					OSErr err;
					// let the system track the drag
					DragWindow(whichWindow, msg->where, GetRegionBounds(GetGrayRgn(), &bounds));

					// afterwords, align the window on a int32_t-word boundary
					err = GetWindowBounds(whichWindow, kWindowContentRgn, &_rcWindow);
					if (noErr == err)
					{
						_rcWindow.left &= ~7;
						MoveWindow(whichWindow, _rcWindow.left, _rcWindow.top, true);
    					OnMove(_rcWindow.left, _rcWindow.top);
                    }
				}
				break;

			    case inGoAway:  // Close button (the red one)
			    {
			    	int hit = TrackGoAway(whichWindow, msg->where);
			    	if (hit)
			    	{
                 	     Stop();
                    }
                    break;
                }
                case inZoomIn: // Zoom button (the orange one)
                case inZoomOut:
                {
                    int hit = TrackBox(whichWindow, msg->where, whatPart);
                    if (hit)
                    {
                       SetPort ((GrafPtr) GetWindowPort (whichWindow));   // window must be current port
#if TARGET_API_MAC_CARBON
                       EraseRect (GetWindowPortBounds (whichWindow, &_rcWindow));   // inval/erase because of ZoomWindow bug
                       ZoomWindow (whichWindow, whatPart, true);
                       InvalWindowRect (whichWindow, GetWindowPortBounds (whichWindow, &_rcWindow));
#else
                       EraseRect (&whichWindow->portRect);   // inval/erase because of ZoomWindow bug
                       ZoomWindow (whichWindow, whatPart, true);
                       InvalRect (&whichWindow->portRect);
#endif // TARGET_API_MAC_CARBON
                       OnSize(_rcWindow.right - _rcWindow.left, _rcWindow.bottom - _rcWindow.top);
                    }
                }
                break;
             }
     	}
     	break;
        case keyDown:
            sKEY->charCode = msg->message & charCodeMask;
        break;
        case keyUp:
            sKEY->charCode = 0;
            sKEY->scanCode = 0;
        break;
        
        // OS events: generally suspend/resume
		case osEvt:
		if (msg->message & 0x01000000)  // Suspend/resume event
		{
			if (msg->message & 0x00000001) // Resume
			{
				SetFocus(true);
			}
			else
			{
				SetFocus(false);
			}
		}
		break;

  	    break;

		// high level events; let the AppleEvent Manager do the work
		case kHighLevelEvent:
			AEProcessAppleEvent(msg);
		break;
    }
    return 0;
}


// Update system messages
int STUB_TaskControl(void)
{
    EventRecord msg;

	if (sMOU->u_id)
	{
		memcpy(sMOU->steButtons, sMOU->rgbButtons, sizeof(sMOU->steButtons));
	}
	do
	{
        if (WaitNextEvent(everyEvent, &msg, 0, nil))
        	MessageHandler(&msg);

		if (!IsFocused())
		{
		   unsigned long tmp;
		   Delay(100, &tmp);
		}
	}while(IsFocused()!=TRUE);

	return _bStopped ? -1 : 0;
}

static WindowPtr CreateAppWindow()
{
	WindowPtr hWnd;
	Rect rect;
	SetRect(&rect, 0, 0, 640, 480);
	CreateNewWindow (
          kDocumentWindowClass,
  	      kWindowStandardDocumentAttributes|
          kWindowStandardHandlerAttribute,
  		 &rect,
         &hWnd
     );

    return hWnd;
}

int main(int argc, char *argv[])
{
    OnInit(argc, argv);
    STUB_OsStartup(NULL);
    STUB_Default();
    WindowPtr h = CreateAppWindow();
	if (!h)
		return -1;

    STUB_OsCustom(NULL);
    STUB_CheckUp(h);
    STUB_ReadyToRun();
    RLX.System.Running = true;
    STUB_MainCode();

    STUB_QuitRequested();
    STUB_Down();
    OnClose();

    return 0;
}
