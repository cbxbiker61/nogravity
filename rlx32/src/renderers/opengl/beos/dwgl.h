/****************************************************************************
; * 																		
; * 	File		:  dwgl.h
; * 																		
; * 	Description :  BDirectGLWindow driver
; * 																		
; * 	Copyright © Realtech VR 1995 - 2003 - http://www.v3x.net.
; * 																		
; * 	Permission to use, copy, modify, distribute and sell this software	
; * 	and its documentation for any purpose is hereby granted without fee,
; * 	provided that the above copyright notice appear in all copies and
; * 	that both that copyright notice and this permission notice appear	
; * 	in supporting documentation.  Realtech makes no representations 	
; * 	about the suitability of this software for any purpose. 			
; * 	It is provided "as is" without express or implied warranty. 		
; * 																		
; ***************************************************************************/

#ifndef __DWGL_HH
#define __DWGL_HH

#include <DirectGLWindow.h>
#include <GL/gl.h>
#include <support/Locker.h>
#include <kernel/OS.h>

#define BGL_DEVICE_HARDWARE 1

class V3X_BDirectGLWindow : public BDirectGLWindow
{
	public:
		V3X_BDirectGLWindow( BRect rect, const char *title );
		virtual ~V3X_BDirectGLWindow( void );
		virtual bool	QuitRequested( void );
		virtual void	MessageReceived( BMessage *msg );
		virtual void	FrameResized( float32_t width, float32_t height );
		virtual BRect	UpdateGLRect( BRect window_rect );
		virtual void    DeviceInfo( uint32_t device_id, uint32_t monitor, const char *name, bool depth, bool stencil, bool accum );
		virtual void    VideoModeInfo( uint32_t width, uint32_t height, uint32_t color, uint32_t depth, uint32_t stencil, uint32_t accum );
		bool            IsFullScreenEnabled() const { return m_bFS; }
		void            SetFullScreenMode(bool bFs, int lx, int ly);
		void            Center();
		status_t        RestartGL(uint32_t deviceID);
		static          V3X_BDirectGLWindow *GetInstance() { return m_pInstance; }

	private:
		static          V3X_BDirectGLWindow * m_pInstance;	
		thread_id		m_DrawThread;
		bool			m_bTearDown;
		bool			m_bInitialized;
		bool			m_bFS;
};

#endif





