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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "_stub.h"
#include "sysresmx.h"
#include "systools.h"
#include "sysini.h"
#include "fixops.h"
#include "sysctrl.h"
#include "sysnetw.h"
#include "gx_tools.h"
#include "gx_init.h"
#include "gx_rgb.h"
#include "gx_csp.h"
#include "sysresmx.h"
#include "gx_struc.h"
#include "gx_csp.h"
#include "gx_flc.h"
#include "v3xdefs.h"
#include "v3x_2.h"
#include "v3xcoll.h"
#include "v3xrend.h"
#include "v3xmaps.h"
#include "v3xscene.h"

#include "v3xrend.h"
#include "iss_defs.h"
#include "iss_fx.h"
#include "iss_av.h"
#include "gui_os.h"
//
#include "lt_struc.h"
#include "lt_data.h"
#include "lt_func.h"

typedef struct {
    char **text;
    int maxline;
    int curline;
    int offset;
}ReadMeStruct;

typedef struct {
    GXSPRITEGROUP *pShapeAnim, *pSelectAnim;
    int32_t Frame, Frame2, Time, Time2, Mode, Max;
    GXSPRITEGROUP *desp;
}ShapeStruct;

int g_cFadeValue;
int g_cFadeValueDir;
static int bControlSetup = 0;

static char *g_pRootMain[]={g_szGmT[45], g_szGmT[46], g_szGmT[48], g_szGmT[49], g_szGmT[50], g_szGmT[51], g_szGmT[37], g_szGmT[52], NULL};

#if (SGTARGET ==NG_DEMO_VERSION)
static char *g_pEpisodeMenu[]={g_szGmT[53], g_szGmT[175], g_szGmT[118], NULL};
#elif (SGTARGET ==NG_FULL_VERSION)
static char *g_pEpisodeMenu[]={g_szGmT[53], g_szGmT[61], g_szGmT[54], g_szGmT[55], g_szGmT[56], g_szGmT[57], g_szGmT[58], g_szGmT[118], NULL};
#endif

#if (SGTARGET !=NG_FULL_VERSION)
static char *g_pMissionMenu[]={g_szGmT[47], NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, g_szGmT[118], NULL};
#endif

static char *g_pExitMenu[]={g_szGmT[52], g_szGmT[66], g_szGmT[67], NULL};

static char *g_pDeletePlayer[]={g_szGmT[162], g_szGmT[66], g_szGmT[67], NULL};

static char g_szLanguage[32] = "## English";

static char *s_pMenuOptions[]={ g_szGmT[62], g_szLanguage, g_szGmT[63], g_szGmT[64], g_szGmT[65],  g_szGmT[119], g_szGmT[101], "Joystick", g_szGmT[118], NULL};


static SGMenu g_pMenuKey[]={
    {"Up", &g_SGSettings.key[18], -1, -1, {NULL}},
    {"Down", &g_SGSettings.key[19], -1, -1, {NULL}}, 
    {"Right", &g_SGSettings.key[20], -1, -1, {NULL}},
    {"Left", &g_SGSettings.key[21], -1, -1, {NULL}},
    {"Throttle", &g_SGSettings.key[0], -1, -1, {NULL}},
    {"Speed+", &g_SGSettings.key[1], -1, -1, {NULL}}, 
    {"Speed-", &g_SGSettings.key[2], -1, -1, {NULL}}, 
    {"Turbo", &g_SGSettings.key[3], -1, -1, {NULL}}, 
    {"BackTurbo", &g_SGSettings.key[4], -1, -1, {NULL}}, 
    {"Roll left", &g_SGSettings.key[5], -1, -1, {NULL}}, 
    {"Roll right", &g_SGSettings.key[6], -1, -1, {NULL}},
    {"Engine off", &g_SGSettings.key[7], -1, -1, {NULL}},
    {"Next weapon", &g_SGSettings.key[8], -1, -1, {NULL}},
    {"Fire", &g_SGSettings.key[9], -1, -1, {NULL}},
    {"Lock", &g_SGSettings.key[10], -1, -1, {NULL}}, 
    {"Use item", &g_SGSettings.key[11], -1, -1, {NULL}}, 
    {"Next item", &g_SGSettings.key[12], -1, -1, {NULL}},
    {"Prev item", &g_SGSettings.key[13], -1, -1, {NULL}}, 
    {"Next NAV", &g_SGSettings.key[14], -1, -1, {NULL}}, 
    {"HUD", &g_SGSettings.key[15], -1, -1, {NULL}},
    {"Radar", &g_SGSettings.key[16], -1, -1, {NULL}}, 
    {"Com", &g_SGSettings.key[17], -1, -1, {NULL}}, 
    {"Talk", &g_SGSettings.key[22], -1, -1, {NULL}},
	{g_szGmT[118], NULL, 0, 0, {NULL}},

    {NULL}
};

static SGMenu s_pMenuGame[]={
    {g_szGmT[157], &g_SGSettings.Difficulty, 0, 2, {g_szGmT[148], g_szGmT[149], g_szGmT[150]}}, 
#ifdef LT_DEV
    {g_szGmT[125], &g_SGSettings.DemoMode, 0, 1, {g_szGmT[69], g_szGmT[68]}},
    {g_szGmT[127], &g_SGSettings.RecTime, 5, 30, {NULL}},
#endif
    {g_szGmT[128], &g_SGSettings.DemoDelay, 15, 60, {NULL}}, 
    {g_szGmT[118], NULL, 0, 0, {NULL}},
{NULL}};

static SGMenu g_pMenuAudio[]={
    {g_szGmT[98], &g_SGSettings.VolFX, 0, 100, {NULL}},
    {g_szGmT[142], &g_SGSettings.VolMusic, 0, 100},
    {g_szGmT[118], NULL, 0, 0, {NULL}},
{NULL}};

static SGMenu g_pMenuControl[]={
    {g_szGmT[100], &g_SGSettings.Ctrl, 0, 2, {g_szGmT[120], g_szGmT[101], g_szGmT[102]}},
    {g_szGmT[103], &g_SGSettings.FlipYMouse, 0, 1, {g_szGmT[67], g_szGmT[66]}}, 
	{"Alt. mouse", &g_SGSettings.AltMouse,  0, 1, {g_szGmT[67], g_szGmT[66]}}, 
    {g_szGmT[144], &g_SGSettings.MouseSensitivity, 1, 32, {NULL}}, 
	{"Joy.Axis Throttle", &g_SGSettings.AxisThrottle, 0, 4, {"None", "Z-axis", "X-axis rotation", "Y-axis rotation", "Z-axis rotation"}}, 
	{"Joy.Axis Roll", &g_SGSettings.AxisRoll, 0, 4, {"None", "Z-axis", "X-axis rotation", "Y-axis rotation", "Z-axis rotation"}}, 	
    {g_szGmT[118], NULL, 0, 0, {NULL}},
{NULL}};

 SGMenu g_pMenuVideo[]={
    {g_szGmT[80], &g_SGSettings.DisplayIndex, 0, 5,  {NULL}}, 
	{"Vertical Sync.", &g_SGSettings.VerticalSync, 0, 1, {g_szGmT[69], g_szGmT[68]}},
	{"Multisampling", &g_SGSettings.Multisampling, 0, 2, {"OFF","2X","4X"}}, 
    {"Limit FPS", &g_SGSettings.FrameSkip, 0, 2, {"70fps", "35fps", "24fps"}}, 
	{"Tex. filtering", &g_SGSettings.TexFiltering, 0, 1, {"Bilinear", "Trilinear"}},
	{"Tex. rectangle", &g_SGSettings.TexPOT, 0, 1, {g_szGmT[68], g_szGmT[69]}},
	{"Tex. compression", &g_SGSettings.TexCompression, 0, 1, {g_szGmT[69], g_szGmT[68]}},
    {"3D effects", &g_SGSettings.VisualsFx, 1, 4, {NULL, NULL} },
    {g_szGmT[147], &g_SGSettings.LensFX, 0, 1, {g_szGmT[69], g_szGmT[68]}}, 
    {g_szGmT[118], NULL, 0, 0, {NULL}},
    {NULL}
};

static GXSPRITE			g_csBackground;
static RW_Interface *	g_pMenu;
static u_int32_t			g_uiDemoDelay;

#if (SGTARGET ==NG_DEMO_VERSION)
static char			*	g_szQuestionMark="?";
#endif
static ShapeStruct		g_cCursorMenu;

static GXSPRITEGROUP*	g_pRewards;
static FLI_STRUCT	*	g_pShipAnims[3];
static u_int8_t			g_nShipAnim;

static u_int32_t COLOR_GRAY,COLOR_GRAY4, COLOR_WHITE, COLOR_GRAY1, COLOR_GRAY2, COLOR_BLUE, COLOR_GRAY3;

static int xADD8(int a, int b)
{
	return min(255, a + b);
}

u_int32_t NG_PixelFormat(int r, int g, int b, int a)
{
 	r = xADD8(RLX.Video.Gamma, xMUL8(r, g_cFadeValue));
	g = xADD8(RLX.Video.Gamma, xMUL8(g, g_cFadeValue));
	b = xADD8(RLX.Video.Gamma, xMUL8(b, g_cFadeValue));	
	return RGBA_PixelFormat(r,g,b,a);
}

void NG_UpdateColor()
{
	COLOR_WHITE = NG_PixelFormat(255, 255, 255, 0);
	COLOR_GRAY = NG_PixelFormat(180, 180, 190, 0);
	COLOR_GRAY1 = NG_PixelFormat(100, 100, 110, 0);	
	COLOR_GRAY2 = NG_PixelFormat(180, 180, 190, 0);
	COLOR_GRAY3 = NG_PixelFormat(180, 180, 255, 0);
	COLOR_GRAY4 = NG_PixelFormat(210, 210, 220, 0);
	COLOR_BLUE = NG_PixelFormat(0, 0, 180, 0);

	if (!g_cFadeValueDir)
		g_cFadeValueDir = 8;
	g_cFadeValue+=g_cFadeValueDir;

	if (g_cFadeValue>255)
		g_cFadeValue=255;
	if (g_cFadeValue<0)
		g_cFadeValue=0;
}

void NG_ResetColor()
{
	g_cFadeValue = 0;
	g_cFadeValueDir = 8;
	NG_UpdateColor();
	return;
}

static void NG_RenderingBackground(void)
{
	char tex[256];

	GX.csp.zoom_pset(&g_csBackground, 0, 0, GX.View.lWidth, GX.View.lHeight);

	if (g_SGMenuPos.Help==1)
    {
        sprintf(tex, "%s%s", g_szGmT[163], g_szGmT[164]);
        CSP_DrawTextC(tex, g_SGMenuPos.XZoneMin, g_SGMenuPos.YZoneMax, COLOR_GRAY, COLOR_WHITE, g_SGMenuPos.Font, GX.csp_cfg.put);
        sysMemZero(tex, 16);
        sysStrnCpy(tex, GAMEVERSION, 5);
        CSP_DrawTextC(tex, GX.View.xmax-CSPG_TxLen(tex, g_SGMenuPos.Font)-4, g_SGMenuPos.YZoneMax, COLOR_GRAY, COLOR_WHITE, g_SGMenuPos.Font, GX.csp_cfg.put);
    }
    
	if (g_SGMenuPos.Help==2)
    {
        sprintf(tex, "3D driver : ~%s~", V3X.Client->s_DrvName);
        CSP_DrawTextC(tex, g_SGMenuPos.XZoneMin, g_SGMenuPos.YZoneMax, COLOR_GRAY, COLOR_WHITE, g_SGMenuPos.Font, GX.csp_cfg.put);
    }

	if (g_SGMenuPos.Help==4)
    {
        sprintf(tex, "Display driver : ~%s~", GX.Client->s_DrvName);
        CSP_DrawTextC(tex, g_SGMenuPos.XZoneMin, g_SGMenuPos.YZoneMax, COLOR_GRAY, COLOR_WHITE, g_SGMenuPos.Font, GX.csp_cfg.put);
    }

    if (g_SGMenuPos.Help==3)
    {
        if ((V3XA.State & 1))
		    sprintf(tex, "Sound driver : ~%s~", V3XA.Client->s_DrvName);
        else
			sprintf(tex, "No sound.");
        CSP_DrawTextC(tex, g_SGMenuPos.XZoneMin, g_SGMenuPos.YZoneMax, COLOR_GRAY, COLOR_WHITE, g_SGMenuPos.Font, GX.csp_cfg.put);
    }

    CSP_Color(COLOR_WHITE);
    return;
}

static int SCALE_X(int x)
{
	return x * GX.View.lWidth / 640;
}

static int SCALE_Y(int y)
{
	return y * GX.View.lHeight / 480;
}

static void NG_LoadMenuInterface(void)
{
    g_pRewards = NG_LoadSpriteGroup("medaille", 2==1);
    g_pRewards->HSpacing = 2;
#if (SGTARGET != NG_DEMO_VERSION)
    {
        SYS_FILEHANDLE in;
        in = FIO_cur->fopen(".\\menu\\viper.flc", "rb");
        SYS_ASSERT(in);
        g_pShipAnims[0] = FLI_Open(in, FLI_USEMEMORY);

        in = FIO_cur->fopen(".\\menu\\raptor.flc", "rb");
        SYS_ASSERT(in);
        g_pShipAnims[2] = FLI_Open(in, FLI_USEMEMORY);

        in = FIO_cur->fopen(".\\menu\\raiden.flc", "rb");
        SYS_ASSERT(in);
        g_pShipAnims[1] = FLI_Open(in, FLI_USEMEMORY);

        if (GX.View.BytePerPixel>1)
        {
            int i;
            for (i=0;i<3;i++)
            {
   				GX.Client->UploadSprite(&g_pShipAnims[i]->bitmap, GX.ColorTable, 1);
			}
        }
    }
#endif
    g_cCursorMenu.pSelectAnim = FLI_LoadToSpriteGroup(".\\menu\\select.flc", 2==1);
    g_cCursorMenu.pShapeAnim = FLI_LoadToSpriteGroup(".\\menu\\shape.flc", 2==1);
    
	g_cCursorMenu.Frame2 = 0;
    g_cCursorMenu.Frame = 0;
    g_SGMenuPos.Xtitle = SCALE_X(110*2);
    g_SGMenuPos.Ytitle = SCALE_Y(60);

    g_SGMenuPos.item2X = SCALE_X(30*2);
    g_SGMenuPos.item1X = SCALE_X(66*2);

    g_SGMenuPos.XZoneMin = 15*2;
    g_SGMenuPos.YZoneMin = SCALE_Y(150);
    g_SGMenuPos.YZoneSize = SCALE_Y(285);

    g_SGMenuPos.Width2 = SCALE_X(140*2);
    g_SGMenuPos.captionX = SCALE_X(42*2);
    g_SGMenuPos.captionY = SCALE_Y(398);
    g_SGMenuPos.YZoneMax = GX.View.ymax-16;
    g_SGMenuPos.Font= (2==1) ? g_pSmallFont : g_pFont;

    return;
}

static void NG_ReleaseMenuInterface(void)
{
    CSPG_Release(g_cCursorMenu.pSelectAnim);
    CSPG_Release(g_cCursorMenu.pShapeAnim);
    CSPG_Release(g_pRewards);

#if (SGTARGET != NG_DEMO_VERSION)
    FLI_Close(g_pShipAnims[0]);
    FLI_Close(g_pShipAnims[1]);
    FLI_Close(g_pShipAnims[2]);
#endif
    return;
}

static GXSPRITE *NG_GetCursorSprite(void)
{
    if (timer_ms()-g_cCursorMenu.Time>g_cCursorMenu.pShapeAnim->speed)
    {
        g_cCursorMenu.Frame++;
        g_cCursorMenu.Time = timer_ms();
    }
    if (g_cCursorMenu.Frame>=g_cCursorMenu.pShapeAnim->maxItem) 
		g_cCursorMenu.Frame=0;
    return
		g_cCursorMenu.pShapeAnim->item + g_cCursorMenu.Frame;
}

static GXSPRITE *NG_GetCursorSprite2(void)
{
    if (timer_ms()-g_cCursorMenu.Time2>g_cCursorMenu.pSelectAnim->speed)
    {
        g_cCursorMenu.Frame2++;
        g_cCursorMenu.Time2 = timer_ms();
    }
    if (g_cCursorMenu.Frame2>=g_cCursorMenu.pSelectAnim->maxItem) g_cCursorMenu.Frame2=0;
    return g_cCursorMenu.pSelectAnim->item+g_cCursorMenu.Frame2;
}

void GX_DrawBoxEffect3D(int32_t x, int32_t y, int32_t lx, int32_t ly)
{
    int32_t xlx=x+lx, yly=y+ly;
    GX.gi.drawHorizontalLine ( x - 1, y - 1, lx + 3, COLOR_GRAY1);
    GX.gi.drawHorizontalLine ( x - 1, yly + 1, lx + 3, COLOR_GRAY2);
    GX.gi.drawVerticalLine ( x - 1, y - 1, ly + 2, COLOR_GRAY1);
    GX.gi.drawVerticalLine ( xlx + 1, y - 1, ly +2, COLOR_GRAY2);
    return;
}

static int RW_RenderSelection(RW_Interface *pInterface)
{
    static int OldCurs;
	int ret = 0;
    GXSPRITE *sp = NG_GetCursorSprite();
	RW_Button *b = pInterface->item + RW.current;

    int x = b->X-(b->X>0),
		yy = GX.View.ymin + b->Y-(b->Y>0);

	SYS_ASSERT(sp);
    GX.csp_cfg.alpha = 0x80;	
	CSP_Color(COLOR_BLUE);

    GX.gi.drawShadedRect(x, yy-1, x+b->LX, yy+b->LY+1, NULL);
    GX_DrawBoxEffect3D( x, yy, b->LX, b->LY);
    yy-=((sp->LY-pInterface->item[RW.current].LY)>>1);
    CSP_Color(COLOR_WHITE);
	GX.csp.put(x-sp->LX-2, yy, sp);

    if (g_cCursorMenu.Mode==2)
		GX.csp.put(x+pInterface->item[RW.current].LX+2, yy, g_cCursorMenu.pShapeAnim->item+g_cCursorMenu.pShapeAnim->maxItem-g_cCursorMenu.Frame-1);

    if (RW.current!=OldCurs)
	{		
		OldCurs = RW.current;
		ret = 1;
	}

	CSP_Color(COLOR_WHITE);
	if (sMOU->numControllers&&(!RW.isMouseHidden))
		GX.csp.put(sMOU->x, sMOU->y, g_pFontMenuLrg->item + 54);

	if (sysConIsActive())
		sysConRender();

    return ret;
}

static void NG_ChangeMenu(SGMenu *M, int mx, int value, int oldvalue)
{
    int i;
    for (i=0;i<mx;i++, M++)
    {
        if (*(M->ptr)==value)
			*(M->ptr)=oldvalue;
    }
    return;
}
static void NG_ChangeJoyValue(int mx, int value, int oldvalue)
{
    int i;
    for (i=0;i<mx;i++)
    {
        if (g_SGSettings.joy[i]==value)
			g_SGSettings.joy[i]=oldvalue;
    }
    return;
}

static void NG_ChangeMouValue(int mx, int value, int oldvalue)
{
    int i;
    for (i=0;i<mx;i++)
    {
        if (g_SGSettings.mou[i]==value)
			g_SGSettings.mou[i]=oldvalue;
    }
    return;
}

static int NG_RenderingKeybMenu(RW_Interface *p, int mode)
{
    SGMenu *pMenu = g_SGMenuPos.pMenu;
    GXSPRITEGROUP *pFont = g_pFontMenuSml;
    int i, numItems = 0, x = g_SGMenuPos.item2X, y, dy = pFont->item[0].LY + 1;
    int y0;
	g_cCursorMenu.Mode = 1;
	

    while(pMenu[numItems].szTitle!=0)
        numItems++;

    i = numItems * pFont->item[0].LY;
    y = y0 = g_SGMenuPos.YZoneMin;

    GX.Client->Lock();

    NG_RenderingBackground();
    CSP_Color(COLOR_WHITE);

	i = 0;
	while(GX.View.ymin + y0 + dy * (RW.current+1) > g_SGMenuPos.YZoneMax  - dy)
	{
		GX.View.ymin-=dy;
		y-=dy;
	}

    for (;i<numItems;i++, y+=dy)
    {
   	    SGMenu *M = pMenu + i;

		if (y>=y0 && y+dy<g_SGMenuPos.YZoneMax - dy)
		{
			char tex[256];
			CSP_Color(COLOR_WHITE);
			CSP_WriteText(M->szTitle, x, y, pFont);
			*tex = 0;

			if (i < numItems-1)
			{      			
				sprintf(tex, "%s", sKEY->NameScanCode(*(M->ptr)));
				
				if (g_SGSettings.joy[i])
				{
					char tmp[32];

					sprintf(tmp, ",JB%d", g_SGSettings.joy[i]);
					strcat(tex, tmp);
				}
				
				if (g_SGSettings.mou[i])
				{
					char tmp[32];
					sprintf(tmp, ",MB%d", g_SGSettings.mou[i]);	
					strcat(tex, tmp);
				}
	 
			}
			
			CSP_Color(COLOR_GRAY);
			CSP_WriteText(tex, x+(g_SGMenuPos.Width2*3/4), y, pFont);
		}
	
		
		if (!mode)
		{
			RW_Zone_CreateWithSize(p, x, y, (g_SGMenuPos.Width2*3/4)-8, pFont->item[0].LY);
		}

    }
	
	CSP_Color(COLOR_GRAY4);
    CSP_WriteText(g_SGMenuPos.name, g_SGMenuPos.Xtitle, g_SGMenuPos.Ytitle, g_pFontMenuSml);
	RW_RenderSelection(p);
	GX.View.ymin=0;

    GX.Client->Unlock();
    GX.View.Flip();
	NG_UpdateColor();

	sysConHandleInput();
	RW.isLocked = sysConIsActive();

    return STUB_TaskControl();
}


static int NG_RenderingSubMenu(RW_Interface *p, int mode)
{
	char tex[256];
    SGMenu *pMenu = g_SGMenuPos.pMenu;
    GXSPRITEGROUP *pFont = g_pFontMenuSml;
    int i, numItems = 0, x = g_SGMenuPos.item2X, y, dy = pFont->item[0].LY + 1;
    g_cCursorMenu.Mode = 1;

    while(pMenu[numItems].szTitle!=0)
        numItems++;

    i = numItems * (pFont->item[0].LY);
    y = g_SGMenuPos.YZoneMin + (g_SGMenuPos.YZoneSize-i)/2;

    GX.Client->Lock();

    NG_RenderingBackground();
    CSP_Color(COLOR_WHITE);

    for (i=0;i<numItems;i++, y+=dy)
    {
   	    SGMenu *M = pMenu + i;
        if (i==numItems-1)
			y+=dy;

        CSP_Color(COLOR_WHITE);
        CSP_WriteText(M->szTitle, x, y, pFont);

        if (i < numItems-1)
        {
             if (*(M->ptr)>M->max)
                 *(M->ptr) = M->max-1;

             if (M->Val[0]==0)
                  sprintf(tex, "%ld%c", ((int32_t)(*(M->ptr))*100L)/(M->max), 37);
             else
             {
                  sprintf(tex, "%s", M->Val[*(M->ptr)]);
                  if (M->max==0)
  	  	 	  	      M->max = array_size((const char**)M->Val);
             }
			 CSP_Color(COLOR_GRAY);
             CSP_WriteText(tex, x+g_SGMenuPos.Width2, y, pFont);
        }

        if (!mode)
            RW_Zone_CreateWithSize(p, x, y, g_SGMenuPos.Width2-8, pFont->item[0].LY);
    }

	{
		char raw[64];
		int axisX, axisY, axisRoll = 0, axisThrottle = 0, status;					
		SGJOY_ReadAxis(&axisX, &axisY, &axisRoll, &axisThrottle, &status);
		
		if (bControlSetup)
		{
			sprintf(raw,"Throttle=%d, Roll=%d", axisThrottle, axisRoll);
			CSP_Color(COLOR_GRAY1);
			CSP_DrawCenterText(raw, g_SGMenuPos.YZoneMax - 30, g_pFontMenuSml, GX.csp_cfg.put);
		}
	}


	CSP_Color(COLOR_GRAY4);
    CSP_WriteText(g_SGMenuPos.name, g_SGMenuPos.Xtitle, g_SGMenuPos.Ytitle, g_pFontMenuSml);
	RW_RenderSelection(p);


    GX.Client->Unlock();
    GX.View.Flip();
	NG_UpdateColor();

	sysConHandleInput();
	RW.isLocked = sysConIsActive();
    return STUB_TaskControl();
}

#define MEDAL_ODD 2
char static *makeSTRING(int n, int c)
{
	static char tex[256];
    int i;
    for (i=0;i<n;i++) 
        tex[i]=(char)c;
    tex[n]=0;
    return tex;
}

static void MedalTotal(char *tex, int medal)
{
    char med[256];
    char c='A';
    int j;
    if (medal==0) 
    {
    	tex[0]=0;
        return;
    }
    sysStrnCpy(med, makeSTRING( medal % MEDAL_ODD, c++), 31); medal/=MEDAL_ODD;
    for (j=0;j<g_pRewards->maxItem-1;j++)
    {
        strcat(med, makeSTRING(medal%MEDAL_ODD, c));
        c++;
        medal/=MEDAL_ODD;
    }
    sysStrCpy(tex, med);
    return;
}

static int NG_RenderingMainMenu(RW_Interface *p, int mode)
{
    int i, numItems, maxLength=0, x = g_SGMenuPos.item1X, y;
    GXSPRITEGROUP *sm;
	int ret = 0;

    char **menu = g_SGMenuPos.menu;
    u_int32_t defColor = g_SGMenuPos.defColor;
    u_int8_t HSpacing = g_SGMenuPos.HSpacing;
    sm = (HSpacing==2) ? g_SGMenuPos.Font : (HSpacing==1 ? g_pFontMenuSml : g_pFontMenuLrg);
    numItems = array_size((const char**)menu)-1;
    g_cCursorMenu.Mode = 2;
	
    for (i=0;i<numItems;i++)
    {
        int len = CSPG_TxLen(menu[i], sm);
        if (len > maxLength)
            maxLength = len;
    }

	if (mode == 2)
		numItems++;

	
	i = numItems * ((HSpacing ? sm->item[0].LY : sm->item[0].LY)+3);
    y = g_SGMenuPos.YZoneMin + (g_SGMenuPos.YZoneSize-i)/2;
    maxLength+= 16*2;
    x = (GX.View.xmax - maxLength) / 2;

    GX.Client->Lock();
    NG_RenderingBackground();
	CSP_Color(COLOR_GRAY4);
    CSP_WriteText(menu[0], g_SGMenuPos.Xtitle, g_SGMenuPos.Ytitle, g_pFontMenuLrg);

    for (i=0;i<numItems;i++)
    {
		char *s;
        int dx, dy;
        if (menu[i+1]==g_szGmT[169])
            y+=sm->item[0].LY;


		if (i==RW.current)
		{
			int j;
			for (j=0;j<10;j++)
			{
				if (menu[i+1] == g_pSaveGames[j].name)
				{
				
					char tex[256];
					char tmp[256];
					int k;
					int a = 0, b = 0, x2 = 0;
					*tmp = 0;
					
					for (k=0;k<8;k++)
					{
						a+=g_pSaveGames[j].level[k];
						b+=g_pGameItem->EI[k].numLevel;
					}

					sprintf(tex, "Score: ~%d~ Level: ~%s~ %s", 
						(int)g_pSaveGames[j].score,						
						g_pEpisodeMenu[g_pSaveGames[j].episode==7 ? 1 : g_pSaveGames[j].episode+2],
										
						asctime(localtime(&g_pSaveGames[j].last_time))
					);
					
	   				CSP_DrawTextC(tex, g_SGMenuPos.XZoneMin, g_SGMenuPos.YZoneMax, COLOR_GRAY, COLOR_WHITE, g_SGMenuPos.Font, GX.csp_cfg.put);

					sprintf(tex, "%3.1f %%", (float)a * 100.f / (float)b);					
					CSP_DrawTextC(tex, g_SGMenuPos.XZoneMin, g_SGMenuPos.YZoneMax - SCALE_Y(30), COLOR_GRAY, COLOR_WHITE, g_pFontMenuLrg, GX.csp_cfg.put);
					x2 = g_SGMenuPos.XZoneMin + CSPG_TxLen(tex, g_pFontMenuLrg) + SCALE_X(20);

					MedalTotal(tex, g_pSaveGames[j].rank);
					CSP_Color(COLOR_WHITE);					
					CSP_WriteText(tex, x2, g_SGMenuPos.YZoneMax - 30, g_pRewards);
				}
			}
     	}

		if (GX.View.BytePerPixel>1)
			CSP_Color(NG_PixelFormat(255-i*6, 255-i*5, 255-i*6, 255));
		else
			CSP_Color(COLOR_WHITE);

        if (i>=(int32_t)defColor)
            CSP_Color( COLOR_GRAY );

		if (mode==2)
			s = i == 0 ?  RW.szInputText : menu[i];
		else
			s = menu[i+1];		

		if (((HSpacing)&&(s!=g_szGmT[118])))
		{
			dy = dx = sm->item[0].LY+3;
			CSP_WriteCenterText(s, y, sm);
			if ((s==g_szGmT[61])) 
				dy+=dx/2;
		}
		else
		{
			dy = dx = RW.pFont->item[0].LY+3;
			if ((s==g_szGmT[118])||(s==g_szGmT[52]))
				y+=dx;
			CSP_WriteCenterText(s, y, g_pFontMenuLrg);
		}
        if (!mode)
        {
            RW_Zone_CreateWithSize(p, x, y, maxLength, dx-3);
        }

        y+=dy;
    }
	if (mode!=2)
		ret = RW_RenderSelection(p);

    GX.Client->Unlock();
    GX.View.Flip();
	NG_UpdateColor();
	if (ret)
		NG_AudioBeep(5);

	sysConHandleInput();
	RW.isLocked = sysConIsActive();
    return STUB_TaskControl();
}

static void NG_ExecSubMenu(char *name, SGMenu *pMenu,  PFRWCALLBACK  pf)
{
    int bKeyboardSetup = pMenu->max < 0 ? -1 : 0;
    int j = 0, but = 0, ok = 1, t = 1, vv, jm;
    u_int8_t *v;
    g_cCursorMenu.Mode = 1;
    while(pMenu[j].szTitle!=0) 
		j++;
    jm = j-1;

	RW.current = 0;

	if (bKeyboardSetup)
	{
		g_pMenu = RW_Interface_Create(RW_VERT);
	}
	g_SGMenuPos.name = name;
    
    while(ok!=0)
    {
		if (!bKeyboardSetup)
		{
			g_pMenu = RW_Interface_Create(RW_VERT);		
		}
        
        g_SGMenuPos.pMenu = pMenu;
        
	    if (t) 
			t = 0;

		but = RW_Interface_Scan(g_pMenu, RW.current, &ok, pf);
		if (STUB_TaskControl())
			break;

		if (ok)
			NG_AudioPlaySound(NG_AudioGetByName("bloup")-1, 0);

		if (but>= jm) 
			ok = 0;

        if (ok)
        {
            if (bKeyboardSetup)
            {  
				do
                {
                    if (STUB_TaskControl())
						break;
					sKEY->Update(0);
					sJOY->Update(0);
					sMOU->Update(0);
				}
				while(sKEY->scanCode);
				 
                do
                {
                    if (STUB_TaskControl())
						break;
					sKEY->Update(0);
					sJOY->Update(0);
					sMOU->Update(0);

					sysConHandleInput();                    
					if (sJOY)
					{
						int b;
						b = NG_SampleButtonJoystick();
						if (b)
						{
							sKEY->scanCode = 255;
							NG_ChangeJoyValue(jm, b, g_SGSettings.joy[but]);
							g_SGSettings.joy[but] = b;
						}
					}

					if (sMOU)
					{
						int b;
						b = NG_SampleButtonMouse();
						if (b)
						{
							sKEY->scanCode = 255;
							NG_ChangeMouValue(jm, b, g_SGSettings.mou[but]);
							g_SGSettings.mou[but] = b;
						}
					}

                }while(!sKEY->scanCode);

                if ((sKEY->scanCode<255)&&(sKEY->scanCode))
                {
                    v = pMenu[but].ptr;
                    NG_ChangeMenu(pMenu, jm, sKEY->scanCode, *v);
                    *v = sKEY->scanCode;
                }
				sKEY->Update(0);
            }
            else
            {
                v = pMenu[but].ptr;
                vv = *v;
                vv+= (RW.dx ? RW.dx : 1);

				if (vv<pMenu[but].min) 
					vv=pMenu[but].max;

                if (vv>pMenu[but].max)
					vv=pMenu[but].min;
                *v = vv;
           
				if (v==&g_SGSettings.VolMusic)
					NG_AudioSetMusicVolume();
                               
				if (v==&RLX.Video.Gamma)
				{
					PAL_Full();
					NG_UpdateColor();
				}
                
				if (v==&g_SGSettings.Language) 
					NG_ReadLanguagePack();
                
				if (v==&g_SGSettings.Ctrl) 
					NG_CheckSystems();

				if (v==&g_SGSettings.DisplayIndex)
				{
					g_SGSettings.ResolutionX = g_pDisplayMode[g_SGSettings.DisplayIndex].lWidth;
					g_SGSettings.ResolutionY = g_pDisplayMode[g_SGSettings.DisplayIndex].lHeight;
				}

				if (v==&g_SGSettings.VerticalSync) 
				{
					if (g_SGSettings.VerticalSync) 
						GX.View.Flags|=GX_CAPS_VSYNC;
					else
						GX.View.Flags&=~GX_CAPS_VSYNC;
				}
            }
        }
		// g_pMenu = RW_Interface_Create(RW_VERT);
    }
    
    return ;
}


int NG_ExecMainMenu(char **menu, int def, u_int32_t defColor, u_int8_t HSpacing)
{
    int but, ok=0;
    int j = array_size((const char **)menu)-1;
    g_pMenu = RW_Interface_Create(RW_VERT);
    g_SGMenuPos.menu = menu;
    g_SGMenuPos.def = def;
    g_SGMenuPos.defColor = defColor;
    g_SGMenuPos.HSpacing = HSpacing;

    if (!g_SGSettings.NextMenu)
    {
        g_SGSettings.NextMenu = 2;
    }

    but = RW_Interface_Scan(g_pMenu, def, &ok, NG_RenderingMainMenu);
	if (STUB_TaskControl())
		return 0;
		
	if (but==j-1)
		ok = 0;

    NG_AudioPlaySound(NG_AudioGetByName("door")-1, 0);
    RW_Interface_Release(g_pMenu);
    return (ok==1) ? but : -1;
}

static char *szMessage;

static void NG_RenderMenuMessage(RW_Interface *p, int mode)
{
	int y = GX.View.ymax/3;	
	
    GX.csp.zoom_pset(&g_csBackground, 0, 0, GX.View.lWidth, GX.View.lHeight);
    RW_Zone_CreateWithText(p, g_szGmT[104], g_SGMenuPos.captionX, g_SGMenuPos.captionY, mode); 
    CSP_Color(COLOR_WHITE);
    CSP_WriteText(szMessage, g_SGMenuPos.XZoneMin, y, g_pFontMenuSml);
	RW_RenderSelection(p);
}

static int MenuMessageCallback(RW_Interface *p, int mode)
{
	if (STUB_TaskControl())
		return -1;

	GX.Client->Lock();
	NG_RenderMenuMessage(p, mode);

	GX.Client->Unlock();
	GX.View.Flip();
	NG_UpdateColor();
	return 0;
}

void NG_MenuMessage(char *tex)
{
    int but=0, ok=1;
    RW_Interface *p = RW_Interface_Create(RW_VERT);
	szMessage = tex;
    but = RW_Interface_Scan(p, but, &ok, MenuMessageCallback);
    RW_Interface_Release(p);
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int NG_PlayerRosterMenu(void)
*
* DESCRIPTION :
*
*/
static int NG_PlayerRosterMenu(void)
{    
	char *szMenus[32];
    u_int8_t val[16];
    int ret, i, p, y, m, ok=0;
    g_cCursorMenu.Mode = 2;
    g_SGMenuPos.Help = 0;
    g_SGSettings.NextMenu = 2;
    do
    {
        MM_heap.push();
        y = 0;
        szMenus[y++] = g_szGmT[106];
        p =-1;
        m = 0;
        for (i=0;i<MAX_SAVE_GAMES;i++)
        {
            if (g_pSaveGames[i].active)
            {
                szMenus[y] = g_pSaveGames[i].name; y++;
                val[m]=i;
                m++;
            } else p=i;
        }
        szMenus[y++] = g_szGmT[108];
        szMenus[y++] = g_szGmT[109];
		g_SGMenuPos.defColor = y;
        szMenus[y++] = g_szGmT[118];
        szMenus[y] = NULL;
        ret = NG_ExecMainMenu(szMenus, 0, m, 0);
        if (ret < 0) 
            ok=-1;
        else
        {
            if ((ret<m)&&(ret>=0))
            {
                g_SGSettings.player = val[ret]; 
				g_pCurrentGame = g_pSaveGames +  g_SGSettings.player;
				ok=2;
            }
            else
            switch(ret-m) {
                case 0:
                if (p>=0)
                {
					char *s = g_pSaveGames[p].name; 
					szMenus[0]=g_szGmT[108];
                    NG_RosterSaveSlot(p);
                    sysStrnCpy(s,
						RLX.App.UserName[0] ? RLX.App.UserName : "Pilot", 16);

					g_pMenu = RW_Interface_Create(RW_VERT);
					if (RW_InputText(g_pMenu, s, 16, NG_RenderingMainMenu) == 1)
					{
						if (!(*s))
							sysStrnCpy(s, RLX.App.UserName[0] ? RLX.App.UserName : "Pilot", 16);
						g_pSaveGames[p].active=1;
						time(&g_pSaveGames[p].last_time);
						ok=0;
						g_pCurrentGame = g_pSaveGames + p;
					}                    
                }
                break;
                case 1:
				{
					szMenus[0]=g_szGmT[109];
					szMenus[m+1]=g_szGmT[118];
					szMenus[m+2]=0;
					g_SGSettings.NextMenu=2;
					ret = NG_ExecMainMenu(szMenus, 0, 0xffffffff, 0);
					if (ret>=0)
					{
						ret = val[ret];
						if (!NG_ExecMainMenu(g_pDeletePlayer, 0, (GX.View.BytePerPixel > 1 ? NG_PixelFormat(255, 255, 255, 0) : 255), 0))
						g_pSaveGames[ret].active=0;
					}
					szMenus[m+1]=NULL;
					g_SGSettings.NextMenu=3;
					ok=0;
				}
                break;
            }
        }
        MM_heap.pop(-1);
    }while(ok==0);

    NG_RosterSave();
    g_SGMenuPos.Help = 1;

    return ok;
}


static int NG_SelectBriefing(void)
{
	char tex[256];
    int ok = 0;
    SGLevelItem *Level = &g_pGameItem->EI[g_pCurrentGame->episode].LI[g_pCurrentGame->level[g_pCurrentGame->episode]];
	int y, ymin, i, ly=g_pFontMenuSml->item[0].LY+2;
#if (SGTARGET ==NG_FULL_VERSION)
    int episode = g_pCurrentGame->episode == 7 ? 7 : 1+g_pCurrentGame->episode;
    
#endif
    NG_AudioStopMusic();
    g_SGMenuPos.Help = 0;

#if (SGTARGET ==NG_FULL_VERSION)
	GX.Client->ReleaseSprite(&g_csPicture);
    sprintf(tex, ".\\brief\\bb%d0.png", episode);
    NG_DrawBackgroundPic(tex, -1, (1+2)|8);
    
#endif
#if (SGTARGET ==NG_DEMO_VERSION)
    ymin = g_SGMenuPos.YZoneMin+4;
#else
    ymin = (GX.View.ymax-ly*16)/2;
#endif

	NG_ResetColor();
    do
    {
        GX.Client->Lock();

#if (SGTARGET ==NG_DEMO_VERSION)
        NG_RenderMenuBackground();
#else

		CSP_Color(COLOR_WHITE);
		GX.csp.zoom_pset(&g_csPicture, 0, 0, GX.View.lWidth, GX.View.lHeight);
		
		CSP_WriteCenterText(Level->name, 30, g_pFontMenuLrg);
#endif        

        for (y=ymin, i=0;i<16;i++, y+=ly)
        {
            int x = GX.View.xmin+(((GX.View.xmax-GX.View.xmin)-CSPG_TxLen(Level->tex[i], g_pFontMenuSml))>>1);
   			if (GX.View.BytePerPixel>1)
   				CSP_Color(NG_PixelFormat(255-i*8, 255-i*8, 255-i*8, 255));
            CSP_WriteText(Level->tex[i], x, y, g_pFontMenuSml);

            // NG_AudioPlaySound(NG_AudioGetByName("message")-1, 0);
        }

		sKEY->Update(0);
		sJOY->Update(0);
		sMOU->Update(0);

		CSP_Color(COLOR_WHITE);

		if (sMOU->numControllers)
			GX.csp.put(sMOU->x, sMOU->y, g_pFontMenuLrg->item + 54);
	    
		if (sysConIsActive())
			sysConRender();

        GX.Client->Unlock();
        GX.View.Flip();
		NG_UpdateColor();

		if (sJOY_IsClicked(0) || sMOU_IsClicked(0) || sKEY->charCode)
			ok = 1;

		if (sJOY_IsClicked(1))
			ok = 2;

        if (sKEY_IsClicked(s_esc))
			ok=2;

        if (sKEY_IsClicked(s_return))
			ok=1;
		
        if (STUB_TaskControl())
			ok=2;

    } while(ok==0);

	GX.Client->ReleaseSprite(&g_csPicture);

#if (SGTARGET ==NG_DEMO_VERSION)
    if (ok)
		return 1;
#endif

    return ok==1;
}

static void NG_RenderMap(RW_Interface *pInterface, int numLevelAvail)
{
	RW_Button *bt;
	int j;

	CSP_Color(COLOR_WHITE);
	GX.csp.zoom_pset(&g_csPicture, 0, 0, GX.View.lWidth, GX.View.lHeight);
	CSP_Color(COLOR_GRAY4);
	CSP_WriteText(g_pEpisodeMenu[g_pCurrentGame->episode==7 ? 1 : g_pCurrentGame->episode+2], 20, 30, g_pFontMenuLrg);    	
    CSP_Color(COLOR_WHITE);
	CSP_WriteText("Select your mission and your ship with arrows or mouse.", 2, GX.View.ymax-g_pFont->item[0].LY-2, g_pFont);

	for (bt = pInterface->item, j=0;
		  j < g_pGameItem->EI[g_pCurrentGame->episode].numLevel;
		  j++, 
		  bt+=2)
    {
        if (j<=numLevelAvail)
        {
            CSP_WriteText(g_pGameItem->EI[g_pCurrentGame->episode].LI[j].name, 
				bt->X, bt->Y, j==numLevelAvail ? g_pFontMenuSml : g_pFont);
        }
    }
}

static int CallbackMenuMap(RW_Interface *pInterface, int mode)
{
	int id = g_pCurrentGame->ship;
    int sel;
    RW_Button *b;
	
	int numLevelAvail = (g_SGSettings.Cheater||g_SGSettings.SerialGame) 
		              ?  g_pGameItem->EI[g_pCurrentGame->episode].numLevel-1 
					  :  g_pCurrentGame->level[g_pCurrentGame->episode];

    GXSPRITE *sp = NG_GetCursorSprite(), *sp2 = 0;

	mode = mode;

	if (RW.current == 0)
		RW.current++;

	sel = RW.current + (RW.current&1 ? 0 : 1);

    if (sel>= g_cCursorMenu.Max)
		sel = g_cCursorMenu.Max;
	
    b = pInterface->item + sel;

	if (g_nShipAnim)
    {		
		FLI_STRUCT *a = g_pShipAnims[id];
		u_int32_t t = timer_ms();
		u_int32_t dl = t - a->LastTime ;
		sp2 = &a->bitmap;
		if (dl > (u_int32_t)a->Header.Struct.speed)
		{
			a->LastTime = t;
			FLI_Unpack(a);
			GX.Client->UpdateSprite(&a->bitmap, a->decompBuffer, GX.ColorTable);
		}
	}

	if (RW.dy)
    {
        int x = g_pCurrentGame->ship;
        x += RW.dy;
		NG_AudioPlaySound(NG_AudioGetByName("bloup")-1, 0);
        
		if (x>2) 
			x=0;

        if (x<0) 
			x=2;

        g_pCurrentGame->ship = x;
        RW.dy = RW.key = 0;
    }

	// Render 
	GX.Client->Lock();

	NG_UpdateColor();

	NG_RenderMap(pInterface, numLevelAvail);
    GX.csp.put(0, GX.View.ymax - sp2->LY, sp2);

	// Ship description
	if (g_nShipAnim)
	{
		if (g_cCursorMenu.desp)
		{
			GXSPRITE *sp = g_cCursorMenu.desp->item + 2 - id;
			GX.csp.put(0, GX.View.ymax - sp2->LY - sp->LY + 20, sp);
		}

		// Cursor sprite 2
		sp = NG_GetCursorSprite2();
		GX.csp.put(b->X+(((int32_t)b->LX-(int32_t)sp->LX)>>1), 
				b->Y+(((int32_t)b->LY-(int32_t)sp->LY)>>1), sp);
	    
	}

	if ((sMOU->numControllers)&&(!RW.isMouseHidden))
		GX.csp.put(sMOU->x, sMOU->y, g_pFontMenuLrg->item + 54);
    
	if (sysConIsActive())
		sysConRender();

	GX.Client->Unlock();
    GX.View.Flip();	

	sysConHandleInput();
	RW.isLocked = sysConIsActive();
	RW.current = sel;
    return STUB_TaskControl();
}

void NG_ResizeInterface2(RW_Interface *Interf)
{
    int32_t i;
    RW_Button *b;
    for (b=Interf->item, i=0;i<Interf->numItem;b++, i++)
    {
		b->X = SCALE_X(b->X);
		b->Y = SCALE_Y(b->Y);
		b->LX = SCALE_X(b->LX);
		b->LY = SCALE_Y(b->LY);      
    }
    return;
}

static int NG_SelectMap(void)
{
	int numLevelAvail = (g_SGSettings.Cheater||g_SGSettings.SerialGame) 
					  ? g_pGameItem->EI[g_pCurrentGame->episode].numLevel-1 
					  : g_pCurrentGame->level[g_pCurrentGame->episode];
	char tex[256];
    int but=0, ok = 1;
    int episode = g_pCurrentGame->episode == 7 ? 7 : 1 + g_pCurrentGame->episode;
    int32_t id = MM_heap.push();        
    RW_Interface *pInterface;

	// Clear background
	GX.Client->ReleaseSprite(&g_csPicture);
	NG_AudioStopTrack();
	NG_ResetColor();

	// Load
	g_cCursorMenu.desp = NG_LoadSpriteGroup("bbspec", 128); // description

	// Load interface
    sprintf(tex, ".\\brief\\br%d0.spc", episode);
    pInterface = RW_Interface_GetFn(tex);
	NG_ResizeInterface2(pInterface);

	SYS_ASSERT(pInterface);
	pInterface->keymode = RW_HORIZ;
    RW_Zone_CreateWithSize(pInterface, 0, GX.View.ymax-128, SCALE_X(148), SCALE_Y(80));

	g_cCursorMenu.Max = numLevelAvail * 2 + 1; // pInterface->numItem;    

	// Load picture
	sprintf(tex, ".\\brief\\br%d0.png", episode);	
    NG_DrawBackgroundPic(tex, Ms_CREDITS, (1+2)|8);
	
	g_nShipAnim = 1;
    but = g_cCursorMenu.Max;

    if (!g_SGGame.IsHost)
    {
        ok = 1;
    }
    else
    do
    {
        ok = 1;
        but = RW_Interface_Scan(pInterface, but, &ok, CallbackMenuMap);
        if ((ok)&&(!RW.dy))
        {
            if (but>0)
            {
                g_pCurrentGame->level[g_pCurrentGame->episode]=(but-1)>>1;
                ok = 2;
                NG_AudioBeep(2);
            }
            else
            {
                RW.dy=1;
            }
        }
    }while(ok==1);

    RW_Interface_Release(pInterface);
    if (g_cCursorMenu.desp)
	{
		CSPG_Release(g_cCursorMenu.desp);
		g_cCursorMenu.desp = 0;
	}
    g_nShipAnim = 0;

    sysMemCpy(GX.ColorTable, GX.ColorTables[1], 768);
    GX_FadeDownPalette(0);
    MM_heap.pop(id);

    g_SGSettings.NextMenu = 66;
    return ok;
}

static int NG_SelectPlayer(void)
{
    int j;
    do
    {
        j = NG_PlayerRosterMenu();
        if (j==-1) return 0;
    }while(j==3);
    return j;
}

static int NG_SelectEpisode(void)
{
    int j;
    if (!g_SGGame.IsHost) 
		return 1;

    g_SGSettings.AddOn = 0;
    j = NG_ExecMainMenu(g_pEpisodeMenu, 0, 0xffffffff, 1);

    if (j<0) 
		return 0;

#if (SGTARGET ==NG_DEMO_VERSION)
    g_pCurrentGame->episode=0;
#else
	
    if (j==0)  
		g_pCurrentGame->episode=7; 
	else 
		g_pCurrentGame->episode=j-1;

    if (g_pCurrentGame->episode>=MAX_EPISODE)
    {
        //NG_MenuMessage(g_szGmT[176]);
        //g_pCurrentGame->episode=MAX_EPISODE-1;
        return 0;
    }
#endif
    return 1;
}


#if (SGTARGET !=NG_FULL_VERSION)
static int NG_SelectMissionProfile(void)
{
    int nMissionCompleted, j, bSpecial;
	SYS_ASSERT(g_pGameItem->numEpisode);
	SYS_ASSERT(g_pCurrentGame->episode<g_pGameItem->numEpisode);

    if (!g_SGGame.IsHost) 
		return 1;
#if (SGTARGET ==NG_DEMO_VERSION)
    g_pGameItem->EI[g_pCurrentGame->episode].numLevel = 3;

#endif
    nMissionCompleted = g_pCurrentGame->level[g_pCurrentGame->episode];
    bSpecial = (g_SGSettings.Cheater) || (g_SGSettings.SerialGame) || (g_SGSettings.AddOn) ? 1 : 0;
    for (j=0;j<g_pGameItem->EI[g_pCurrentGame->episode].numLevel;j++)
    {
        g_pMissionMenu[j+1] = ((j<=nMissionCompleted) || bSpecial)
        ? g_pGameItem->EI[g_pCurrentGame->episode].LI[j].name
        : g_szQuestionMark;
    }
    g_pMissionMenu[g_pGameItem->EI[g_pCurrentGame->episode].numLevel+1] = g_szGmT[118];
    g_pMissionMenu[g_pGameItem->EI[g_pCurrentGame->episode].numLevel+2] = NULL;
    j = NG_ExecMainMenu(g_pMissionMenu, 0, bSpecial ? g_pGameItem->EI[g_pCurrentGame->episode].numLevel+1 : nMissionCompleted+1, 1);
    if (j>=0)
    {
        if ((!bSpecial)&&(j>nMissionCompleted)) 
			return 0;
        g_pCurrentGame->level[g_pCurrentGame->episode] = j;
        return 1;
    }
    return 0;
}
#endif

static void NG_NetRenderBackground(RW_Interface *pInterface, int mode, int bAllocate)
{
	NG_RenderingBackground();
	NG_NetRender();
	
	RW_Zone_CreateWithText(pInterface, mode ? g_szGmT[171] : g_SGGame.IsHost ? g_szGmT[107] : g_szGmT[52], g_SGMenuPos.captionX, g_SGMenuPos.captionY, bAllocate); 
	if (mode)
	{
		RW_Zone_CreateWithText(pInterface, g_szGmT[172], 130*2, g_SGMenuPos.captionY, bAllocate);
		RW_Zone_CreateWithText(pInterface, g_szGmT[118], 230*2, g_SGMenuPos.captionY, bAllocate);
	}

}
static int callbackNetMenu(RW_Interface *pInterface, int mode)
{
    if (RW.dy)
    {
        g_SGGame.mySession+=RW.dy;
        NG_NetDisplay(1);
    }
    else 
		NG_NetDisplay(0);

	if ((g_SGSettings.SerialGame==2)&&(!g_SGGame.IsHost))
    {
		char tex[256];
        if (sNET->ReceiveData(NET_EVERYBODY, tex, 255))
        {
            if (tex[0]=='œ') 
            {
                g_pCurrentGame->episode = tex[1];
                g_pCurrentGame->level[g_pCurrentGame->episode] = tex[2];
                g_SGSettings.SerialGame = 3;
				return 1;
            }
        }
	}

	GX.Client->Lock();
	NG_NetRenderBackground(pInterface, 0, mode);	
	CSP_Color(COLOR_GRAY4);
    CSP_WriteText(g_SGMenuPos.name, g_SGMenuPos.Xtitle, g_SGMenuPos.Ytitle, g_pFontMenuSml);
	RW_RenderSelection(pInterface);

	GX.Client->Unlock();
	GX.View.Flip();
	NG_UpdateColor();

	sysConHandleInput();
	RW.isLocked = sysConIsActive();
    return STUB_TaskControl();
}

static int NG_NetSelectSession(int mode)
{
    int but, ok=1;
    RW_Interface *pInterface = RW_Interface_Create(RW_VERT);
    g_cCursorMenu.Mode = 1;
    g_pCurrentGame->ship = 0;
	g_SGMenuPos.name = g_szGmT[177+7];
    but = RW_Interface_Scan(pInterface, 0, &ok, callbackNetMenu);
    if (mode)
    {
        if (ok)
        {
            GX.Client->Unlock();
            switch(but){
                case 0:
                if (sNET->CreateNewSession(g_pCurrentGame->name, 4, NULL))
                {
                    NG_MenuMessage(g_szGmT[173]);
                }
                else
                {
                    g_SGGame.IsHost=1;
                }
                break;
                case 1:
                {
                    if (sNET->JoinSession(g_SGGame.Session, NULL))
                    {
                        NG_MenuMessage(g_szGmT[174]);
                    }
                    else
                    {
                        g_SGGame.IsHost=0;
                    }
                }
                break;
                case 2:
                ok = 0;
                break;
            }
        }
    }
    RW_Interface_Release(pInterface);
    // Cancelation
    if (mode==0)
    {
        if ((g_SGGame.IsHost)&&(ok==0))
        {
            sNET->CloseSession();
        }
        if ((g_SGGame.IsHost==0)&&(g_SGSettings.SerialGame!=3))
        {
            ok = 0;
            sNET->QuitSession();
        }
    }
    return ok;
}

static void XCancel(void)
{
	NG_AudioStopTrack();
    NG_AudioPlayTrack(Ms_MENU1);
    sysMemCpy(GX.ColorTable, GX.ColorTables[0], 768);
    PAL_Full();
    return;
}

static int NG_NewGameMenu(void)
{
    int ok1=1;
    do
    {
        if ((g_SGSettings.GoToBrief==2)||NG_SelectPlayer())
        {
            int ok2=1;
            g_SGGame.IdPlayer = 0;
            g_SGGame.IsHost = 1;
            g_SGGame.numPlayer = 1;
            g_SGSettings.SerialGame = 0;
            if ((g_SGSettings.GoToBrief!=2)&&(RLX.System.Id==RLXOS_WIN32))
            {
                int m = NG_NetSelectMode();
                switch(m) {
                    case  0: break;
                    case -1: return 0;
                    case  1: g_SGSettings.SerialGame = 1; break;
                }
            }
            if (g_SGSettings.SerialGame)
            {
                sNET->SetPlayerName(g_pCurrentGame->name);
                // Host ou join
                g_SGSettings.SerialGame = 1+NG_NetSelectSession(1);
                if (g_SGSettings.SerialGame==2)
                {
                    // attente
                    g_SGSettings.SerialGame = NG_NetSelectSession(0);
                } else g_SGSettings.SerialGame = 0;
                // Quitte ou ferme la session
                if (!g_SGSettings.SerialGame)
                {
                    if (g_SGGame.IsHost) sNET->CloseSession();
                    else sNET->QuitSession();
                    return 0;
                }
            }
            do
            {
                g_SGSettings.GoToBrief = 0;
                if (NG_SelectEpisode())
                {
                    int ok3;
#if (SGTARGET ==NG_DEMO_VERSION)
                    if (NG_SelectMissionProfile())
                    {
                        return NG_SelectBriefing();
                    }
                    else
                    {
                        XCancel();
                        return 0;
                    }
#endif
                    do
                    {
                        int ok4;
                        ok4=(g_SGGame.IsHost) ? NG_SelectMap() : 1;
                        ok3=1;
                        if (g_SGGame.IsHost)
                        {
                            ok3 = 1;
                        }
                        else
                        {
                            GX.Client->Lock();
                            GX.gi.clearVideo();
                            GX.Client->Unlock();
                        }
                        if (ok4)
                        {
                            if (NG_SelectBriefing()) return 1;
                            else XCancel();
                            ok3=g_SGGame.IsHost ? 1 : 0;
                        } else {XCancel();return 0;}
                    }while(ok3!=1);
                } else {ok2=1;ok1=0;}
            }while(ok2!=1);
        } else return 0;
    }while(ok1!=1);
    return 0;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/


/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_HallOfFame(void)
*
* DESCRIPTION :
*
*/


static int RenderHallOfFame(RW_Interface *pInterface, int mode)
{
    int i, y = g_SGMenuPos.YZoneMin + RW.pFont->item[0].LY;

	GX.Client->Lock();
	
	CSP_Color(COLOR_WHITE);
	GX.csp.zoom_pset(&g_csBackground, 0, 0, GX.View.lWidth, GX.View.lHeight);
	CSP_Color(COLOR_GRAY4);
    CSP_WriteText(g_szGmT[105], g_SGMenuPos.Xtitle, g_SGMenuPos.Ytitle, g_pFontMenuLrg);
       
	for (i=0;i<10;i++, y+=g_pFontMenuSml->item[0].LY+3)
    {
		char tex[256];
    	CSP_Color((i&1 ? COLOR_GRAY4 : COLOR_GRAY2));
		CSP_WriteText(g_pBestGames[i].name, 20*2, y, g_pFontMenuSml);
        MedalTotal(tex, g_pBestGames[i].rank);
		CSP_WriteText(tex, 120*2, y, g_pRewards);
        CSP_WriteText(g_pGameItem->EI[g_pBestGames[i].episode].LI[g_pBestGames[i].level[g_pBestGames[i].episode]].name, 155*2, y, g_SGMenuPos.Font);
        sprintf(tex, "%d", (int)g_pBestGames[i].score);
        CSP_WriteText(tex, 230*2, y, g_pFontMenuSml);
    }

	RW_Zone_CreateWithText(pInterface, g_szGmT[118], g_SGMenuPos.captionX, 
		g_SGMenuPos.captionY, mode);

	RW_RenderSelection(pInterface);

	GX.Client->Unlock();
	GX.View.Flip();
	NG_UpdateColor();

	sysConHandleInput();
	RW.isLocked = sysConIsActive();
    return STUB_TaskControl();
}

static void NG_HallOfFame(void)
{
    int ok;
	RW_Interface *pInterface = RW_Interface_Create(RW_VERT);
    g_cCursorMenu.Mode = 1;
    RW_Interface_Scan(pInterface, 0, &ok, RenderHallOfFame);
    RW_Interface_Release(pInterface);
    return;
}
    
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
extern char **g_szLanguageList;
static void GetLanguageText()
{	
	sprintf(g_szLanguage, "## %s", g_szLanguageList[g_SGSettings.Language]);
}

static void ChangeLanguage()
{
	g_SGSettings.Language++;
	if (g_szLanguageList[g_SGSettings.Language] == 0)
		g_SGSettings.Language = 0;

	NG_SetLanguage(g_SGSettings.Language);
}

int NG_JoystickCalibration();

static void NG_OptionsMenu(void)
{
    int ok;
    do
    {
		GetLanguageText();
		bControlSetup = 0;
        ok = NG_ExecMainMenu(s_pMenuOptions, 0, 0xffffffff, 0);
        switch(ok)
		{
			case 0:
			ChangeLanguage();
			break;
            case 1:
            g_SGMenuPos.Help=4;
            NG_ExecSubMenu(g_szGmT[63], g_pMenuVideo, NG_RenderingSubMenu);
            g_SGMenuPos.Help=1;
            break;
            case 2:
            g_SGMenuPos.Help=3;
            NG_ExecSubMenu(g_szGmT[64], g_pMenuAudio, NG_RenderingSubMenu);
            g_SGMenuPos.Help=1;
            break;
            case 3:
			bControlSetup = 1;
            NG_ExecSubMenu(g_szGmT[65], g_pMenuControl, NG_RenderingSubMenu);
            break;            
            case 4:			
            NG_ExecSubMenu(g_szGmT[119], s_pMenuGame, NG_RenderingSubMenu);
            break;
			case 5:
            NG_ExecSubMenu(g_szGmT[101], g_pMenuKey, NG_RenderingKeybMenu);
            break;
			case 6:
			NG_JoystickCalibration();
			break;
        }
    }while(ok>=0);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void NG_MenuCredits(void)
{
    GXSPRITE sp;
	char tex[256];
    char file[256];
    char **Greets=NULL;
    int quit = 0;
    GXSPRITEGROUP *font=g_pFontMenuLrg;
    int32_t y0, f, y, j, n, LY;

    int32_t ymin = GX.View.ymin, ymax=GX.View.ymax;

	MM_heap.reset();
    {
        SYS_FILEHANDLE in;
        sprintf(tex, "%s\\voix\\e_credit.txt", ".");
        in = FIO_cur->fopen(tex, "rb");
        if (in)
        {
            Greets = array_loadtext(in, 128, -1);
            FIO_cur->fclose(in);
        }
        else
		  return;
    }
    n = array_size((const char**)Greets);
    sprintf(file, ".\\menu\\%s.png", RLX.Dev.Developper);
    NG_LoadBackground(file, &sp);
    NG_AudioPlayTrack(Ms_CREDITS);
    CSP_Color(COLOR_WHITE);
    y = GX.View.ymax;
	timer_Stop(&g_cTimer);
	timer_Start(&g_cTimer, 60, 1); // 70 fps
    do
    {
        sKEY->Update(0);
		sJOY->Update(0);
        GX.Client->Lock();
        GX.csp.zoom_pset(&sp, 0, 0, GX.View.lWidth, GX.View.lHeight);
        f = 0;
        for (y0=y, j=0;j<n;j++)
        {
            char *tx=Greets[j];
            if (tx)
            {
                if (strstr(tx, "^"))
                {
                    font=g_pFontMenuLrg;
                    CSP_Color(COLOR_WHITE);
                }
                else
                {
                    font=g_pFontMenuSml;
                    CSP_Color(COLOR_GRAY3);
                }
                LY = font->item[0].LY+2;
                if ((y0>= GX.View.ymin-LY)&&(y0<=GX.View.ymax))
                {
                    f=1;
                    CSP_WriteCenterText(tx, y0, font);
                }
                y0+=LY;
            }
        }
       
        GX.Client->Unlock();
        if ((f==0)||NG_WaitForKeyPress()) 
			quit=1;
        y--;
        GX.View.Flip();
		NG_UpdateColor();
		timer_Update(&g_cTimer);
		
    }while(!quit);
	timer_Stop(&g_cTimer);
    NG_AudioBeep(2);
    MM_heap.free(sp.data);
    array_free(Greets);
    NG_AudioStopMusic();
    GX.View.ymin = ymin;
    GX.View.ymax = ymax;
    GX.Client->Lock();
    GX.gi.clearVideo();
    GX.Client->Unlock();
    MM_heap.reset();
    GX.Client->ReleaseSprite(&sp);
    return;
}

static void NG_LoadReadMe(ReadMeStruct *r, char *filename)
{
    SYS_FILEHANDLE in = FIO_cur->fopen(filename, "rt");
	sysMemZero(r, sizeof(ReadMeStruct));
	if (in)
	{
		SYS_ASSERT(in);    
		r->text = array_loadtext(in, MAX_LINES_README_TXT, -1);
		r->maxline = array_size((const char**)r->text);
		FIO_cur->fclose(in);
	}
    return;
}

static void DestroyReadMe(ReadMeStruct *r)
{
    array_free(r->text);
    return;
}

void NG_DrawHelpFile(GXSPRITEGROUP *Font, int color2, int xz)
{
	ReadMeStruct sFile;
	char tex[256];
    int j, y, y0, LY, mX, quit=0, quitted=0;
    int ddy=0;
    MM_heap.push();
    sprintf(tex, ".\\voix\\%c_readme.txt", g_SGSettings.LangCode);
	FIO_cur = &FIO_res;
    NG_LoadReadMe(&sFile, tex);
    if (sFile.maxline)
    {
        y = GX.View.ymin;
		CSP_Color(COLOR_WHITE);
        do
        {
            if (STUB_TaskControl())
				break;

            sKEY->Update(0);
			sJOY->Update(0);

			g_SGSettings.bClearView = ((RLX.System.Id==RLXOS_DOS32)&&(RLX.V3X.Id<=1)) ? 2 : 1;
            GX.Client->Lock();
			
			CSP_Color(COLOR_WHITE);
			if (g_csBackground.data)
				GX.csp.zoom_pset(&g_csBackground, 0, 0, GX.View.lWidth, GX.View.lHeight);

			CSP_Color(COLOR_GRAY4);

            CSP_WriteText(g_szGmT[49], g_SGMenuPos.Xtitle, g_SGMenuPos.Ytitle, g_pFontMenuLrg);            
            for (y0=y, j=0;j<sFile.maxline;j++, y0+=LY)
            {
                LY = Font->item[0].LY+2;
                if ((y0>= GX.View.ymin-LY)&&(y0<=GX.View.ymax))
                {
                    char *szText = sFile.text[j];
                    CSP_DrawTextC(szText, xz, y0, color2, (GX.View.BytePerPixel > 1 ? NG_PixelFormat(255, 255, 255, 0) : 255), Font, GX.csp_cfg.put);
                }
            }
            GX.Client->Unlock();            

            mX = (y0-y)-GX.View.ymax;
            {
				int dy = sMOU->lZ;
                if (sMOU_IsHeld(1))
                    quit = 1;
                else
                    if (quit)
						quitted = 1;
                if (!dy)
                {
                    if (sKEY_IsHeld(s_up))   
						ddy= 8;

                    if (sKEY_IsHeld(s_down))
						ddy=-8;
                } 
				else 
					ddy=dy;

                if (sKEY_IsClicked(s_return) || sKEY_IsClicked(s_f1) ||  sKEY_IsClicked(s_esc))
					quitted=1;

                y+=ddy;

                if (ddy>0) 
					ddy--;

                if (ddy<0) 
					ddy++;
            }

            if (y>GX.View.ymin) 
				y=GX.View.ymin;
            
			if (y<-mX) 
				y=-mX;

            GX.View.Flip();
			NG_UpdateColor();
        }while(!quitted);
    }
    DestroyReadMe(&sFile);
    MM_heap.pop(-1);
    NG_AudioBeep(2);
    return;
}

static void NG_HelpMenu(void)
{
    int ymax, ymin;
    ymin = GX.View.ymin;
    ymax = GX.View.ymax;
    GX.View.ymin = g_SGMenuPos.YZoneMin+g_pFont->item[0].LY;
    GX.View.ymax = GX.View.ymin+g_SGMenuPos.YZoneSize-g_pFont->item[0].LY*2;
    NG_DrawHelpFile(g_SGMenuPos.Font, COLOR_GRAY, g_SGMenuPos.XZoneMin);
    GX.View.ymin = ymin;
    GX.View.ymax = ymax;
    return;
}

int NG_MainMenu(void)
{
    int but, ok=0, iItem=0;
    // Presentation
    GX.View.Flags = GX_CAPS_VSYNC| GX_CAPS_BACKBUFFERINVIDEO;
    filewad_chdir(FIO_wad, "");
    NG_ChangeScreenMode(GX.Client->SearchDisplayMode(g_SGSettings.ResolutionX, g_SGSettings.ResolutionY, g_SGSettings.ColorDepth));
    NG_LoadBackground(".\\menu\\lit_mn01.png", &g_csBackground);
    NG_InstallHandlers();
    g_SGMenuPos.Help = 1;
    RW.pFont = g_pFontMenuLrg;
    NG_LoadMenuInterface();
    CSP_Color(COLOR_WHITE);
    NG_AudioPlayTrack(Ms_MENU1);
    g_SGSettings.NextMenu = g_SGSettings.GoToBrief ? 2 : 0 ;
    g_SGSettings.DemoMode = 0;
	sysConSetLimits(0,0,GX.View.lWidth, GX.View.lHeight/2);
	sysConPrint("Loading main menu ...");
	NG_ResetColor();

	if (sJOY->numControllers && (RLX.Joy.Config == RLXCTRL_Uncalibrated))
		NG_JoystickCalibration();

    if (g_SGSettings.GoToBrief==2)
    {
        if (NG_NewGameMenu()) 
			ok=1;
        else ok=4;
        g_SGSettings.GoToBrief=0;
    }
#if (SGTARGET != NG_DEMO_VERSION)
    if (g_SGSettings.GoToBrief==1)
    {
        ok = NG_SelectMap();
        if (ok) 
			ok = NG_SelectBriefing();
        g_SGSettings.GoToBrief = 0;
        if (!ok)
            ok=4;
    }
#else
    if (g_SGSettings.GoToBrief)
    {
        g_SGSettings.GoToBrief = 0;
        ok=1;
    }
#endif
    while(ok==0)
    {
        int32_t p = MM_heap.push();
        g_uiDemoDelay = timer_sec();
        but = NG_ExecMainMenu(g_pRootMain, 0, 0xffffffff, 0);
        g_uiDemoDelay=0;
        switch(but) {
            case 0: //New Game
            if (!NG_NewGameMenu())
            {
                if (g_SGSettings.NextMenu==66)
                ok=4;
            }
            else
            {
                ok=1;
                iItem=0;
            }
            break;
            case 1: //Options
            NG_OptionsMenu();
            g_SGSettings.NextMenu=3;
            break;
            case 2: //Instr
            NG_HelpMenu();
            g_SGSettings.NextMenu=3;
            break;
            case 3: //Best of
            NG_HallOfFame();
            g_SGSettings.NextMenu=3;
            break;
            case 4: //Credits
            NG_AudioStopMusic();
            ok=3;
            break;
            case 5:
            if (g_SGSettings.DemoMode!=4) 
			{
				g_SGSettings.DemoMode = 2;
				iItem=1;
			}
            ok=1;
            break;
            case -1:
            case 6:
            if (NG_ExecMainMenu(g_pExitMenu, 0, 0xffffffff, 0)==0)
				ok=2;
            g_SGSettings.NextMenu=2;
            break;
        }
        MM_heap.pop(p);
    }
    if (!iItem)  
		GX_FadeDownPalette(0);

    GX.Client->ReleaseSprite(&g_csBackground);
    NG_ReleaseMenuInterface();
    NG_SaveGameInfo();
    NG_AudioStopMusic();
    return ok;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static void DrawValue(char *tx, int val, int val2, int mode, int l)
{
	char tex[256];
    int i = val;
	int y = g_SGMenuPos.YZoneMin + l * (RW.pFont->item[0].LY + 5);

	switch(mode) 
	{
        case 1:  sprintf(tex, "%02d:%02d", i/60, i%60); break;
        case 2:  sprintf(tex, "%d", val2); break;
        default: sprintf(tex, "%d/%d", i, val2); break;
    }    

	CSP_Color(COLOR_GRAY);
	CSP_WriteText(tx, 40 * GX.View.lWidth / 640, y, g_pFontMenuLrg);

	CSP_Color(COLOR_WHITE);
	CSP_WriteText(tex, 460 * GX.View.lWidth / 640, y, g_pFontMenuLrg);
    return;
}

static int NG_RenderEndLevelBackground(RW_Interface *p, int mode)
{
	int l = 0;

	GX.Client->Lock();

	CSP_Color(COLOR_WHITE);
	GX.csp.zoom_pset(&g_csPicture, 0, 0, GX.View.lWidth, GX.View.lHeight);
	CSP_Color(COLOR_GRAY4);
    CSP_WriteText(g_SGMenuPos.name, g_SGMenuPos.Xtitle, g_SGMenuPos.Ytitle, g_pFontMenuSml);

	DrawValue(g_szGmT[112], g_cGameStat.time_end - g_cGameStat.time_start, 0, 1, l); l++;
    DrawValue(g_szGmT[114], g_cGameStat.killed_nmy, g_cGameStat.total_nmy, 0, l); l++;    
    DrawValue(g_szGmT[116], g_cGameStat.killed_amy, g_cGameStat.total_amy, 0, l); l++;	
    
	if (g_SGSettings.SerialGame)
	{
        DrawValue("Frag", g_cGameStat.frag, g_cGameStat.frag, 2, l);
		l++;
	}
    
	if (g_cGameStat.shooted)
	{
		DrawValue(g_szGmT[191], (g_cGameStat.hitted*100L)/g_cGameStat.shooted, 100, 0, l);
		l++;
	}

	DrawValue(g_szGmT[115], g_cGameStat.bonus, g_cGameStat.total_bonus, 0, l); l++;

	RW_Zone_CreateWithText(p, g_szGmT[107], g_SGMenuPos.captionX, g_SGMenuPos.captionY, mode);
	RW_Zone_CreateWithText(p, g_szGmT[146], 300 * GX.View.lWidth / 640, g_SGMenuPos.captionY, mode);

	RW_RenderSelection(p);

	GX.Client->Unlock();
	GX.View.Flip();
	NG_UpdateColor();

	sysConHandleInput();
	RW.isLocked = sysConIsActive();
	return STUB_TaskControl();

}

int NG_EndLevel(void)
{
    int ok = 1, but = 0, bSuccess = 0;
    MM_heap.reset();
	RW.pFont = g_pFontMenuLrg;
    NG_LoadMenuInterface();
    g_SGMenuPos.name = g_pGameItem->EI[g_pCurrentGame->episode].LI[g_pCurrentGame->level[g_pCurrentGame->episode]].name;    
    
	// Check values
    if (g_cGameStat.bonus>g_cGameStat.total_bonus)  
		g_cGameStat.bonus = g_cGameStat.total_bonus;

    if (g_cGameStat.killed_nmy>g_cGameStat.total_nmy) 
		g_cGameStat.killed_nmy=g_cGameStat.total_nmy;

	if (g_cGameStat.hitted>g_cGameStat.shooted) 
		g_cGameStat.hitted=g_cGameStat.shooted;

	// Update score
	g_pCurrentGame->score+=g_pGameItem->EI[g_pCurrentGame->episode].LI[g_pCurrentGame->level[g_pCurrentGame->episode]].Bonus;
    
	NG_HighScoresUpdate();

    g_pMenu = RW_Interface_Create(RW_HORIZ);
    
    filewad_chdir(FIO_wad, "");
    NG_DrawBackgroundPic(".\\menu\\lit_mn01.png", Ms_SUCCESS, (1+2)|8);
    g_cCursorMenu.Mode = 1;
	 
    do
    {
        but = RW_Interface_Scan(g_pMenu, but, &ok, NG_RenderEndLevelBackground);
        if (STUB_TaskControl())
           break;

        if (ok)
        switch(but) 
		{
            case 0: // retry
				g_SGSettings.GoToBrief = g_SGSettings.SerialGame ? 2 : 1;
				ok=1;
            break;
            case 1: // next
				ok=2;
            break;
        }
    }while(ok==0);
	
	RW_Interface_Release(g_pMenu);
	NG_ReleaseMenuInterface();
    GX.Client->ReleaseSprite(&g_csPicture);
    NG_AudioPlaySound(NG_AudioGetByName("explode2")-1, 0);    
    NG_AudioStopMusic();

	// Update rank
	g_pCurrentGame->rank++;    

	// New level ...
	g_pCurrentGame->level[g_pCurrentGame->episode]++;
	time(&g_pCurrentGame->last_time);
    if (g_pCurrentGame->level[g_pCurrentGame->episode] >= g_pGameItem->EI[g_pCurrentGame->episode].numLevel)
    {
        g_pCurrentGame->level[g_pCurrentGame->episode] = g_pGameItem->EI[g_pCurrentGame->episode].numLevel;
        if (g_pCurrentGame->episode <= MAX_GAME_EPISODE)
        {
            NG_PlayEndEpisode(g_pCurrentGame->episode+1);
            bSuccess = 1;
        }
        
        if (g_pCurrentGame->episode<5)
        {
            g_pCurrentGame->episode++;
#if (SGTARGET ==NG_DEMO_VERSION)
            g_pCurrentGame->episode=0;
#endif
            if (g_pCurrentGame->episode>=MAX_GAME_EPISODE) 
				g_pCurrentGame->episode=0;

        }		
    }
    
#if (SGTARGET != NG_DEMO_VERSION)
    if (bSuccess)
	{		
		int i;
		for (i=0;i<MAX_GAME_EPISODE;i++)
		{
			if (g_pCurrentGame->level[i]<g_pGameItem->EI[0].numLevel)
				bSuccess = 0;
		}
		if (bSuccess)
			NG_PlayTheEnd();
	}
#endif

	NG_RosterSave();
    return ok;
}

//
// Move the Z Rotation all the way up and down

// Joystick calibration (Mac only)
int NG_JoystickCalibration()
{
	char raw[256];
	int part = 0;
	int y2 = g_SGMenuPos.YZoneMin + 40;
	int y3 = y2 + 20;
	int y4 = y2 + 40;	
	RLX.Joy.Config = RLXCTRL_Uncalibrated; // 
	
	do
    {
        if (STUB_TaskControl())
			break;

        sKEY->Update(0);
		sJOY->Update(0);
		
		CSP_Color(COLOR_WHITE);
		if (g_csBackground.data)
			GX.csp.zoom_pset(&g_csBackground, 0, 0, GX.View.lWidth, GX.View.lHeight);

		CSP_Color(COLOR_WHITE);		
		CSP_WriteText("Joystick Setup", g_SGMenuPos.Xtitle, g_SGMenuPos.Ytitle, g_pFontMenuLrg);
		
		CSP_Color(COLOR_GRAY4);

		switch(part) 
		{
			case 0:
				RLX.Joy.Config = RLXCTRL_Uncalibrated; // 
				CSP_DrawCenterText("Find Center Point", g_SGMenuPos.YZoneMin, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("Leave the handle centered, then", y2, g_pFontMenuSml, GX.csp_cfg.put);

				CSP_DrawCenterText("press a button on the controller.", y3, g_pFontMenuSml, GX.csp_cfg.put);

				RLX.Joy.J[0].MinX =
				RLX.Joy.J[0].MaxX =  sJOY->lX;

				RLX.Joy.J[0].MinY =
				RLX.Joy.J[0].MaxY =  sJOY->lY;

				RLX.Joy.J[0].MinZ =
				RLX.Joy.J[0].MaxZ =  sJOY->lZ;

				RLX.Joy.J[0].MinR =
				RLX.Joy.J[0].MaxR =  sJOY->lRx;

				sprintf(raw,"%d axis, %d buttons", sJOY->numAxes, sJOY->numButtons);
				CSP_Color(COLOR_GRAY1);
				CSP_DrawCenterText(raw, g_SGMenuPos.YZoneMax - 30, g_pFontMenuSml, GX.csp_cfg.put);
			break;

			case 1:
				CSP_DrawCenterText("Axis Calibration", g_SGMenuPos.YZoneMin, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("Move the handle in complete circles", y2, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("then press a button on the controller.", y3, g_pFontMenuSml, GX.csp_cfg.put);

				{
					int l = 180;
					int lh = l>>1;
					int cx = GX.View.lWidth/2;
					int cy = g_SGMenuPos.YZoneMax - 40 - lh;
					int dx, dy;

					GX.csp_cfg.alpha = 0x80;
					CSP_Color(COLOR_BLUE);
					GX.gi.drawShadedRect(cx-lh, cy-lh, cx+lh, cy+lh, NULL);
					GX_DrawBoxEffect3D(cx-lh, cy-lh, l, l);										

					GX.csp_cfg.alpha = 0xff;						

					RLX.Joy.J[0].MinX = min(sJOY->lX, RLX.Joy.J[0].MinX);
					RLX.Joy.J[0].MaxX = max(sJOY->lX, RLX.Joy.J[0].MaxX);
					RLX.Joy.J[0].MinY = min(sJOY->lY, RLX.Joy.J[0].MinY);
					RLX.Joy.J[0].MaxY = max(sJOY->lY, RLX.Joy.J[0].MaxY);
					{
						dx = RLX.Joy.J[0].MaxX - RLX.Joy.J[0].MinX;
						dy = RLX.Joy.J[0].MaxY - RLX.Joy.J[0].MinY;
						if (dx && dy)
						{
							int ox = ((sJOY->lX - RLX.Joy.J[0].MinX) * l / dx) - lh;
							int oy = ((sJOY->lY - RLX.Joy.J[0].MinY) * l / dy) - lh;
							int x = cx + min(max(ox, -lh), lh);
							int y = cy + min(max(oy, -lh), lh);
							GX.gi.drawVerticalLine(x, cy-lh+1, l-2, COLOR_WHITE);
							GX.gi.drawHorizontalLine(cx-lh+1, y, l-2, COLOR_WHITE);
						}
					}

				}
				sprintf(raw,"X=%d, Y=%d", sJOY->lX, sJOY->lY);
				CSP_Color(COLOR_GRAY1);
				CSP_DrawCenterText(raw, g_SGMenuPos.YZoneMax - 30, g_pFontMenuSml, GX.csp_cfg.put);
			break;
			
			case 2:
				RLX.Joy.Config = 0;
				CSP_DrawCenterText("Verify Center Point", g_SGMenuPos.YZoneMin, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("Leave the handle centered, then", y2, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("press a button on the controller.", y3, g_pFontMenuSml, GX.csp_cfg.put);

				sprintf(raw,"X=%d, Y=%d", sJOY->lX, sJOY->lY);
				CSP_Color(COLOR_GRAY1);
				CSP_DrawCenterText(raw, g_SGMenuPos.YZoneMax - 30, g_pFontMenuSml, GX.csp_cfg.put);

			break;


			case 3:
				RLX.Joy.Config = RLXCTRL_Uncalibrated; // 

				CSP_DrawCenterText("Axis Calibration", g_SGMenuPos.YZoneMin, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("Move the Z Axis", y2, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("all the way up and down", y3, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("then press a button on the controller.", y4, g_pFontMenuSml, GX.csp_cfg.put);

				{
					int l = 180;
					int lh = l>>1;
					int lq = l>>2;
					int cx = GX.View.lWidth/2;
					int cy = (3*GX.View.lHeight/4) - lq/2;
					int dz;

					GX.csp_cfg.alpha = 0x80;
					CSP_Color(COLOR_BLUE);

					GX.csp_cfg.alpha = 0xff;

					RLX.Joy.J[0].MinZ = min(sJOY->lZ, RLX.Joy.J[0].MinZ);
					RLX.Joy.J[0].MaxZ = max(sJOY->lZ, RLX.Joy.J[0].MaxZ);

					dz = (RLX.Joy.J[0].MaxZ - RLX.Joy.J[0].MinZ);
					if (dz)
					{
				        int w = (l * (sJOY->lZ - RLX.Joy.J[0].MinZ) / dz) - lh;
						w = min(max(w, -lh), lh);
						GX.csp_cfg.alpha = 0x80;
						CSP_Color(COLOR_BLUE);
						GX.gi.drawShadedRect(cx-lh, cy-lq, cx+w,  cy, NULL);						
					}    
					GX_DrawBoxEffect3D(cx-lh, cy-lq, l, lq);
				}
				sprintf(raw,"Z-axis=%d", sJOY->lZ);
				CSP_Color(COLOR_GRAY1);
				CSP_DrawCenterText(raw, g_SGMenuPos.YZoneMax - 30, g_pFontMenuSml, GX.csp_cfg.put);
			break;

			case 4:
				RLX.Joy.Config = RLXCTRL_Uncalibrated; // 

				CSP_DrawCenterText("Axis Calibration", g_SGMenuPos.YZoneMin, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("Move the Z Rotation", y2, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("all the way up and down", y3, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("then press a button on the controller.", y4, g_pFontMenuSml, GX.csp_cfg.put);

				{
					int l = 180;
					int lh = l>>1;
					int lq = l>>2;
					int cx = GX.View.lWidth/2;
					int cy = (3*GX.View.lHeight/4) - lq/2;
					int dz;

					GX.csp_cfg.alpha = 0x80;
					CSP_Color(COLOR_BLUE);

					GX.csp_cfg.alpha = 0xff;

					RLX.Joy.J[0].MinR = min(sJOY->lRx, RLX.Joy.J[0].MinR);
					RLX.Joy.J[0].MaxR = max(sJOY->lRx, RLX.Joy.J[0].MaxR);

					dz = (RLX.Joy.J[0].MaxR - RLX.Joy.J[0].MinR);
					if (dz)
					{
				        int w = (l * (sJOY->lRx - RLX.Joy.J[0].MinR) / dz) - lh;
						w = min(max(w, -lh), lh);
					
						GX.csp_cfg.alpha = 0x80;
						CSP_Color(COLOR_BLUE);
						GX.gi.drawShadedRect(cx-lh, cy-lq, cx+w,  cy, NULL);						
					}    
					GX_DrawBoxEffect3D(cx-lh, cy-lq, l, lq);
				}
				sprintf(raw,"X-axis Rotation=%d", sJOY->lRx);
				CSP_Color(COLOR_GRAY1);
				CSP_DrawCenterText(raw, g_SGMenuPos.YZoneMax - 30, g_pFontMenuSml, GX.csp_cfg.put);

			break;
			case 5:
				RLX.Joy.Config = 0; // 
				g_SGSettings.Ctrl = 2;
					
				CSP_DrawCenterText("Done!", g_SGMenuPos.YZoneMin, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("Default control is now using joystick", y2, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("Go to Keyboard menu to configure", y3, g_pFontMenuSml, GX.csp_cfg.put);
				CSP_DrawCenterText("The joystick buttons mapping.", y4, g_pFontMenuSml, GX.csp_cfg.put);
			break;
		}

		{
			int i;
			for (i=0;i<sJOY->numButtons;i++)
			{
				if (sJOY_IsClicked(i))
				{
					part++;
					if (part == 3 && sJOY->numAxes<=2)
					{
						part = 5;
					}

					if (part == 4 && sJOY->numAxes<=3)
					{
						part = 5;
					}

					NG_AudioPlaySound(NG_AudioGetByName("bloup")-1, 0);

					if (part == 5)
					{
					}
					
					break;
				}
			}

		}

		if (sKEY_IsClicked(s_esc))
		{
			break;
		}
		
        GX.View.Flip();
		NG_UpdateColor();
    }while(part<6);
	RLX.Joy.Config = 0; // 

    return 0;

}
