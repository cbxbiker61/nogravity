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
#ifndef __GX_STRUC_H
#define __GX_STRUC_H


// Video caps
enum {
	GX_CAPS_FBDIRECT = 0x1,      // FB can be accessed direclty
	GX_CAPS_FBLINEAR = 0x2,      // FB is linear
	GX_CAPS_FBLINEARIZED = 0x4,      // FB was linearized
	GX_CAPS_BACKBUFFERINVIDEO = 0x8,      // The backbuffer is in video memory
	GX_CAPS_VSYNC = 0x10,     // Vertical sync after page flipping
	GX_CAPS_FBINTERLEAVED = 0x20,     // FB is interleaved (1 line/2)
	GX_CAPS_FBINTERLEAVESWAP = 0x40,     // FB is interleaved odd/even and swap
	GX_CAPS_DRAWFRONTBACK = 0x80,     // FB write on the display page (not on the back page)
	GX_CAPS_3DSYSTEM = 0x200,    // 3D display buffer (for Direct3D)
	GX_CAPS_WINDOWED = 0x400,    // Display in a window (for BeOS, Windows)

	GX_CAPS_FBTRIPLEBUFFERING = 0x2000,   // Enable Triple buffering
	GX_CAPS_MULTISAMPLING = 0x4000      // Reserved
};

enum
{
	GX_STATE_BACKBUFFERPAGE = 0x100,    // Internal (buffer page 0/1)
	GX_STATE_LOCKED = 0x800,    // FB is locked (write is allowed)
    GX_STATE_SCENEBEGUN = 0x1000,   // 3D Scene has begun.
	GX_STATE_MENUBAR = 0x2000,
	GX_STATE_VIEWPORTCHANGED = 0x4000
};

enum
{
	GX_CAPS2_CANRENDERWINDOW = 0x1,
	GX_CAPS2_CANRENDERFULLSCREEN = 0x2
};

// User video driver
typedef struct _gx_colormask {
	unsigned char		RedMaskSize;       // RGB Pixel format
    unsigned char		RedFieldPosition;
    unsigned char		GreenMaskSize;
    unsigned char		GreenFieldPosition;
    unsigned char		BlueMaskSize;
    unsigned char		BlueFieldPosition;
    unsigned char		RsvdMaskSize;
    unsigned char		RsvdFieldPosition;
}GXRGBCOMPONENT;

typedef struct _gx_viewport{

    uint8_t            *lpBackBuffer;      // Pointer to the back buffer
    uint8_t            *lpFrontBuffer;     // Pointer to the front buffer
    uint32_t            lPitch;            // Pitch in byte (byte per lines)
    uint32_t            lSurfaceSize;      // Surface size in bytes
    int32_t              lWidth;            // Width size of the screen
    int32_t              lHeight;           // Height size of the screen
    int32_t              lRatio;            // Ratio
    uint32_t            lVideoSize;        // Video memory size
    int32_t              xmin;              // Viewport coordinates
    int32_t              ymin;
    int32_t              xmax;
    int32_t              ymax;
    void CALLING_C  (*Flip)(void);   // Display page function
    uint32_t            RGB_Magic;         // RGB mask for alpha blending
    uint32_t           *RGB_Mask;          // RGB table
    uint32_t            Flags;             // Flags (see "Video Caps")
	int					 State;
    uint16_t            DisplayMode;       // Display mode ID
    uint8_t             BytePerPixel;      // Numbers of bytes per pixel
    uint8_t             BitsPerPixel;      // Numbers of bits per pixel
    uint8_t			 Multisampling;
	uint8_t             Reserved;
	GXRGBCOMPONENT		 ColorMask;

}GXVIEWPORT;

// Extended GXSPRITE structure
typedef struct {
     uint8_t U, V, Alpha, Mode;
     void *Page;
}GXSPRITEUV;

// Offscreen informations (private)
typedef struct _gx_offplain {
    uint8_t            *lpSurface[16];
    unsigned          maxSurface;
    unsigned          currentSurface;
    uint8_t             flags[16];
}GXSCREENBUFFERS;

// DOS and BeOS hardware chipset features
enum {
      GX_HARDWARE_cursor = 0x1, // Has hardware cursor
      GX_HARDWARE_rect = 0x2, // Can draw rectangle
      GX_HARDWARE_line = 0x4, // Can draw lines
      GX_HARDWARE_blitter  = 0x8, // Can do BitBLT
      GX_HARDWARE_scrn = 0x10, // Reserved
      GX_HARDWARE_turbo = 0x20  // Reserved
};

// Hardware chipset features
typedef struct {
    int32_t              IdChip, feature, featureSav;
    int32_t              Memory, Version, SubVers;
    char             *Chipname;
    char              xdefault[8];
    int32_t              feature3D;
    uint8_t             Familly;
}GX_HardwareInfo;

// Display mode informations (private)
typedef struct _gx_display_mode_info {
    short             mode;
    uint16_t            lWidth, lHeight;
    uint16_t            BitsPerPixel;
}GXDISPLAYMODEINFO;

typedef int GXDISPLAYMODEHANDLE;

enum GX_EVENT_MODE {
	GX_EVENT_RESIZE = 1
};

// Video Driver
typedef struct {
    uint8_t          *(* RLXAPI Lock)(void);
    void               (* RLXAPI Unlock)(void);
    GXDISPLAYMODEINFO*(* RLXAPI EnumDisplayList)(int bpp);
    void               (* RLXAPI GetDisplayInfo)(GXDISPLAYMODEHANDLE mode);
    int	               (* RLXAPI SetDisplayMode)(GXDISPLAYMODEHANDLE mode);
    GXDISPLAYMODEHANDLE(* RLXAPI SearchDisplayMode)(int lx, int ly, int bpp);
    int                (* RLXAPI CreateSurface)(int numberOfSparePages);
    void               (* RLXAPI ReleaseSurfaces)(void);
    void               (* RLXAPI UploadSprite)(GXSPRITE *sp, rgb24_t *colorTable, int bpp);
    void               (* RLXAPI ReleaseSprite)(GXSPRITE *sp);
    unsigned           (* RLXAPI UpdateSprite)(GXSPRITE *sp, const uint8_t *bitmap, const rgb24_t *colorTable);
    int                (* RLXAPI RegisterMode)(int bpp);
    void               (* RLXAPI Shutdown)(void);
    int                (* RLXAPI Open)(void *hwnd);
    unsigned           (* RLXAPI NotifyEvent)(enum GX_EVENT_MODE mode, int x, int y);


    char               s_DrvName[64];
	int				   Capabilities;
	int				   State;
}GXCLIENTDRIVER;

// Graphic driver
typedef struct _gx_graphic_interface
{
    void  (* CALLING_C drawAnyLine)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t colour);
	void  (* CALLING_C drawAliasedLine)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t colour);
    void  (* CALLING_C drawHorizontalLine)(int32_t x1, int32_t y1, int32_t lx, uint32_t colour);
    void  (* CALLING_C drawVerticalLine)(int32_t x1, int32_t y1, int32_t lx, uint32_t colour);
    void  (* CALLING_C drawWiredRect)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t colour);
    void  (* CALLING_C drawShadedRect)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, void *palette);
    void  (* CALLING_C drawMeshedRect)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t colour);
    void  (* CALLING_C drawFilledRect)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t colour);
    void  (* CALLING_C drawPixel)(int32_t x, int32_t y, uint32_t colour);
	void  (* CALLING_C TrspPixel)(int32_t x, int32_t y, uint32_t colour);
    uint32_t (* CALLING_C getPixel)(int32_t x, int32_t y);
    void  (* CALLING_C clearBackBuffer)(void);
    void  (* CALLING_C clearVideo)(void);
    void  (* CALLING_C blit)(uint32_t dest, uint32_t src);
    void  (* CALLING_C waitDrawing)(void);
    void  (* CALLING_C setPalette)(uint32_t a, uint32_t b, void * pal);
    void  (* CALLING_C setCursor)(int32_t x, int32_t y);
    void  (* CALLING_C copyCursor)(uint8_t *map);
	void  (* CALLING_C setGammaRamp)(const rgb24_t *ramp);
}GXGRAPHICINTERFACE;

#include "gx_csp.h"

struct GXSYSTEM
{
	GXGRAPHICINTERFACE	gi;
	GXVIEWPORT		View;
	GXSCREENBUFFERS		Surfaces;
	GX_HardwareInfo		Accel;
	GXCLIENTDRIVER *	Client;
	GXSPRITEINTERFACE			csp;
	CSP_Config			csp_cfg;
	rgb24_t			 *  ColorClut;
	rgb24_t				ColorTables[4][256];
	rgb24_t				ColorTable[256];
	rgb24_t				AmbientColor;
	rgb24_t				DefaultColor;
	void (*init)(void);
};

__extern_c

extern struct GXSYSTEM	GX;

__end_extern_c

#define GFX_byte(dest, val)              *(uint8_t*)(dest)=(uint8_t)val
#define GFX_word(dest, val)              *(uint16_t*)(dest)=(uint16_t)val
#define GFX_dword(dest, val)             *(uint32_t*)(dest)=(uint32_t)val
#define GFX_read(target)                *target
#define GFX_memset2(target, val, size)    {int __i=size;uint16_t *__a=(uint16_t*)(target);for(;__i!=0;__a++, __i--) *__a=(uint16_t)(val);}
#define GFX_memset4(target, val, size)    {int __i=size;uint32_t *__a=(uint32_t*)(target);for(;__i!=0;__a++, __i--) *__a=(uint32_t)(val);}

#define GFX_memset(target, val, size)     sysMemSet(target, val, size)
#define GFX_memcpy(target, source, size)  sysMemCpy(target, source, size)

#endif

