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
#ifndef __SYSCTRL_H
#define __SYSCTRL_H

// Joystick
typedef struct _RClientDriver_Joystick
{
    int      (* RLXAPI Open)(void *, int bForceFeedback);
    void     (* RLXAPI Release)(void);
    unsigned long (* RLXAPI Update)(void *device);

	int		numButtons; // Num buttons
	int		numControllers; // Num controllers
	int		numAxes;
	int		numPOVs;
	void     *  device;
    int		lX;  // X-axis, usually the left-right movement of a stick.
    int		lY;  // Y-axis, usually the forward-backward movement of a stick.
    int		lZ;  // Z-axis, often the throttle control. If the joystick does not have this axis, the value is 0.
    int		lRx; // X-axis rotation. If the joystick does not have this axis, the value is 0.
    int		lRy; // Y-axis rotation. If the joystick does not have this axis, the value is 0. 
    int		lRz; // Z-axis rotation (often called the rudder). If the joystick does not have this axis, the value is 0.
    int		rglSlider[2]; // Two additional axis values
    unsigned	rgdwPOV[4]; // Direction controllers, such as point-of-view hats. The position is indicated in hundredths of a degree clockwise from north (away from the user). The center position is normally reported as –1; but see Remarks. For indicators that have only five positions, the value for a controller is –1, 0, 9,000, 18,000, or 27,000. 
    u_int8_t	rgbButtons[128]; // Array of buttons. The high-order bit of the byte is set if button is set.
	u_int8_t	steButtons[128]; // Array of previous buttons.
    int		lVX; // X-axis velocity. 
    int		lVY; // Y-axis velocity. 
    int		lVZ; // Z-axis velocity. 
    int		lVRx; // X-axis angular velocity.
    int		lVRy; // Y-axis angular velocity.
    int		lVRz; //Z-axis angular velocity
    int		rglVSlider[2]; // Extra axis velocities
    int		lAX; // X-axis acceleration
    int		lAY; // Y-axis acceleration
    int		lAZ; // Z-axis acceleration
    int		lARx; // X-axis angular acceleration
    int		lARy; // Y-axis angular acceleration
    int		lARz; // Z-axis angular acceleration
    int		rglASlider[2]; // Extra axis accelerations
    int		lFX; // X-axis force.
    int		lFY; // Y-axis force.
    int		lFZ; // Z-axis force.
    int		lFRx; // X-axis torque.
    int		lFRy; // Y-axis torque.
    int		lFRz; // Z-axis torque.
    int		rglFSlider[2]; // Extra axis forces.
    
}JOY_ClientDriver;

#define sJOY_GetValue(b) sJOY->rgbButtons[b]
#define sJOY_IsHeld(b) !!sJOY->rgbButtons[b]
#define sJOY_IsClicked(b)  (sJOY->rgbButtons[b] && (!sJOY->steButtons[b]))
#define sJOY_IsReleased(b)  ((!sJOY->rgbButtons[b]) && sJOY->steButtons[b])


__extern_c
  _RLXEXPORTFUNC JOY_ClientDriver *JOY_SystemGetInterface_STD(void);
  extern JOY_ClientDriver *sJOY;
__end_extern_c

#if defined __BEOS__
#define SKEY_SCANTABLESIZE 16

enum s_keymap
{
    s_none=0, s_esc=1, s_f1, s_f2, s_f3, s_f4, s_f5, s_f6, s_f7, s_f8, s_f9, s_f10, s_f11, s_f12, 
    s_tilda=0x11, s_1, s_2, s_3, s_4, s_5, s_6, s_7, s_8, s_9, s_0, s_minus, s_equals, s_backslash=0x33, 
    s_backspace=0x1e, s_tab=0x26, s_q, s_w, s_e, s_r, s_t, s_y, s_u, s_i, s_o, s_p, s_opensquare, s_closequare, 
    s_capslock=0x3b, s_a, s_s, s_d, s_f, s_g, s_h, s_j, s_k, s_l, s_semicolon, s_quote, s_return,
    s_leftshift=0x4b, s_z, s_x, s_c, s_v, s_b, s_n, s_m, s_coma, s_period, s_slash, s_rightshift,
    s_ctrl1=0x5c, s_alt1, s_space, s_alt, s_ctrl,
    s_printscreen=0x0e, s_scrolllock, s_pause,
    s_numinsert=0x1f, s_home, s_pageup, s_numlock, s_numslash, s_numstar, s_numminus,
    s_numdelete=0x34, s_end, s_pagedown, s_padhome, s_numup, s_numpageup, s_numplus=0x3a,
    s_numleft=0x48, s_num5, s_numright,
    s_up=0x57, s_numend, s_numdown, s_numpagedown, s_numenter,
    s_left=0x61, s_down, s_right, s_insert, s_delete,
    s_winleft=0x66, s_winright, s_winapp, s_sysreq=0x7e, s_break = 0x7f,
	s_count
};

#define SKEY_MSK(x) (1L<<(7-((x)&7)))
#define SKEY_GET_BIT(table, x) (table[(x)>>3] & SKEY_MSK(x))
#define SKEY_SET_BIT(table, x, v) {if (v) table[(x)>>3]|=SKEY_MSK(x); else table[(x)>>3]&=~SKEY_MSK(x);}

#elif defined __MACOS__
#define SKEY_SCANTABLESIZE 16
enum s_keymap
{
	s_a=0x00, s_s, s_d, s_f, s_g, s_h, s_z, s_x, s_c, s_v, s_unka, s_unkb, s_q, s_w, s_e, s_r, 
	s_y=0x10, s_t, s_1, s_2, s_3, s_4, s_6, s_5, s_equals, s_9, s_7, s_minus, s_8, s_0, s_closesquare, s_o, 
	s_u=0x20, s_opensquare, s_i, s_p, s_return, s_l, s_j, s_quote, s_k, semicolon, s_backslash, s_coma, s_slash, s_n, s_m, s_period, 
	s_tab=0x30, s_space, s_tilda, s_backspace, s_unk34, s_esc, s_unk36, s_unk37, s_rightshift, s_winleft, s_capslock, s_alt, s_ctrl, s_unk3d, s_unk3e, s_unk3f, 
	s_unk6=0x40, s_numdelete, s_unk42, s_numstar, s_unk44, s_numplus, s_unk46, s_numlock, s_unk48, s_unk49, s_unk4a, s_numslash, s_numreturn, s_unk4d, s_numminus, 
	s_unk5=0x50, s_numslash2, s_numinsert, s_numend, s_numdown, s_numpagedown, s_numleft, s_num5, s_numright, s_numhome, s_unk5a, s_numup, s_numpageup, 
	s_f5=0x60, s_f6, s_f7, s_f3, s_f8, s_f9, s_unk66, s_f11, s_unk68, s_f13, s_unk6a, s_f14, s_unk6c, s_f10, s_unk6e, s_f12, 
	s_unk70=0x70, s_pause, s_insert, s_home, s_pageup, s_delete, s_f4, s_end, s_f2, s_pagedown, s_f1, s_left, s_right, s_down, s_up, 
	s_none=0x7f
};
#define s_winright    s_winleft
#define s_leftshift   s_rightshift
#define s_winapp      s_unk70
#define s_printscreen s_f13

#define SKEY_MSK(x) (1L<<((x)&7))
#define SKEY_GET_BIT(table, x) (table[(x)>>3] & SKEY_MSK(x))
#define SKEY_SET_BIT(table, x, v) {if (v) table[(x)>>3]|=SKEY_MSK(x); else table[(x)>>3]&=~SKEY_MSK(x);}

#else

#define SKEY_SCANTABLESIZE 256
enum s_keymap 
{
	s_none=0,  s_esc=1, s_1, s_2, s_3, s_4, s_5, s_6, s_7, s_8, s_9, s_0, s_minus, s_equals, 
	s_backspace, s_tab, s_q, s_w, s_e, s_r, s_t, s_y, s_u, s_i, s_o, s_p, 
	s_opensquare, s_closesquare, s_return, s_ctrl, s_a, s_s, s_d, s_f, s_g, 
	s_h, s_j, s_k, s_l, s_semicolon, s_quote, s_tilda, s_leftshift, 
	s_backslash, s_z, s_x, s_c, s_v, s_b, s_n, s_m, s_coma, s_period, s_slash, 
	s_rightshift, s_printscreen, s_alt, s_space, s_capslock, 
	s_f1, s_f2, s_f3, s_f4, s_f5, s_f6, s_f7, s_f8, s_f9, s_f10, 
	s_numlock, s_scrolllock, s_numhome, s_numup, s_numpageup, s_numminus, s_numleft, 
	s_num5, s_numright, s_numplus, s_numend, s_numdown, s_numpagedown, s_numinsert, 
	s_numdelete, s_f11=87, s_f12=88, s_winleft=91, s_winright=92, s_winapp=93, 
	s_joyleft=110, s_joyright, s_joyup, s_joydown, 
	s_joybut1=120, s_joybut2=121, s_joybut3=122, s_joybut4=123, 
	s_home=199, s_up=200, s_pageup=201, s_right=205, s_left=203, s_end=207, 
	s_down=208, s_pagedown=209, s_insert=210, s_delete=211
};

#define SKEY_GET_BIT(table, x) (table[x] & 0x80)
#define SKEY_SET_BIT(table, x, v) { table[x] = v ? 0x80 : 0;}

#endif

typedef struct _RClientDriver_Keybrd 
{
    int		(* RLXAPI Open)(void *);
    void	(* RLXAPI Release)(void);
    char *	(* RLXAPI NameScanCode)(int scan);
    unsigned long (* RLXAPI Update)(void *device);
	void *		device;
	u_int8_t	scanCode; // scancode (see /scancode.h for values)
	char		charCode; // ASCII char code
	u_int8_t	rgbButtons[SKEY_SCANTABLESIZE]; // keys
	u_int8_t	steButtons[SKEY_SCANTABLESIZE]; // push buffers
	int		numControllers;
	
}KEY_ClientDriver;


#define sKEY_GetValue(b) ( SKEY_GET_BIT(sKEY->rgbButtons, b) )
#define sKEY_IsHeld(b) ( !!SKEY_GET_BIT(sKEY->rgbButtons, b) )
#define sKEY_IsClicked(b) ( SKEY_GET_BIT(sKEY->rgbButtons, b) && (!SKEY_GET_BIT(sKEY->steButtons, b)))
#define sKEY_IsReleased(b) ( (!SKEY_GET_BIT(sKEY->rgbButtons, b)) && SKEY_GET_BIT(sKEY->steButtons, b))

__extern_c
  _RLXEXPORTFUNC KEY_ClientDriver *KEY_SystemGetInterface_STD(void);
  extern KEY_ClientDriver *sKEY;
__end_extern_c

// Mouse

typedef struct _RClientDriver_Mouse
{
	int    (* RLXAPI Open)(void *);
    void   (* RLXAPI Release)(void);	
    void   (* RLXAPI Show)(void);
	void   (* RLXAPI Hide)(void);
	void   (* RLXAPI SetPosition)(u_int32_t x, u_int32_t y);
	unsigned long (* RLXAPI Update)(void *device);

	int		numButtons;
	int		numAxes;
	int		numControllers;
	void*	device;
	int     u_id; // 1 : using messaging
	int     x;   // X-axis (absolute)
    int     y;   // Y-axis (absolute)
    int		lX;  // X-axis. (relative)
    int		lY;  // Y-axis.
    int		lZ;  // Z-axis, typically a wheel. If the mouse does not have a z-axis, the value is 0.
    u_int8_t	rgbButtons[8]; // Array of buttons. The high-order bit of the byte is set if button is set.
	u_int8_t	steButtons[8]; // Array of previous buttons.	

}MSE_ClientDriver;

#define sMOU_GetValue(b) sMOU->rgbButtons[b]
#define sMOU_IsHeld(b) !!sMOU->rgbButtons[b]
#define sMOU_IsClicked(b)  (sMOU->rgbButtons[b] && (!sMOU->steButtons[b]))
#define sMOU_IsReleased(b)  ((!sMOU->rgbButtons[b]) && sMOU->steButtons[b])

__extern_c

_RLXEXPORTFUNC    MSE_ClientDriver *MSE_SystemGetInterface_STD(void);
_RLXEXPORTDATA    extern MSE_ClientDriver *sMOU;

__end_extern_c

#endif
