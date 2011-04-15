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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "sysresmx.h"
#include "sysctrl.h"
#include "systools.h"
#include "systime.h"
//
#include "gx_struc.h"
#include "gx_csp.h"
#include "gx_tools.h"
#include "gx_init.h"
#include "gx_rgb.h"
//
#include "syslist.h"
#include "gui_os.h"

RW_SYSTEM  RW;

RW_Interface *RW_Interface_Create(int mode)
{
    RW_Interface *p = (RW_Interface *) MM_heap.malloc(sizeof(RW_Interface));
    p->numItem = 0;
	p->keymode = (enum RW_KEY_MODE)mode;
    p->item = MM_CALLOC(1, RW_Button);
    return p;
}

void RW_Interface_Release(RW_Interface *pInt)
{
    MM_heap.free(pInt->item);
	if (pInt->parent)
		MM_heap.free(pInt->parent);
    MM_heap.free(pInt);
}

static void RW_Zone_Alloc(RW_Interface *pInt)
{
    pInt->item = ( RW_Button *) MM_heap.realloc(pInt->item, sizeof( RW_Button  ) * (pInt->numItem+2) );
}

void RW_Zone_CreateWithSize(RW_Interface *pInt, int x, int y, int lx, int ly)
{
    RW_Zone_Alloc(pInt);
    pInt->item[pInt->numItem].X=x;
    pInt->item[pInt->numItem].Y=y;
    pInt->item[pInt->numItem].LX=lx;
    pInt->item[pInt->numItem].LY=ly;
    pInt->numItem++;
}

void RW_Zone_CreateWithButton(RW_Interface *pInt, RW_Button *But)
{
    RW_Zone_CreateWithSize(pInt, But->X, But->Y, But->LX, But->LY);
}

void RW_Zone_CreateWithText(RW_Interface *pInt, char *szText, int x, int y, int bAllocate)
{
    RW_Button area;
    area.X = x;
    area.Y = y;
    area.LX = (short)(CSPG_TxLen(szText, RW.pFont) + 6);
    area.LY = (short)(RW.pFont->item[0].LY + 6);

	CSP_WriteText(szText, area.X + 4, area.Y + 3, RW.pFont);

    if (!bAllocate)
	{
		area.X+=2;
		area.LX-=3;
		area.Y+=2;
		area.LY-=3;
        RW_Zone_CreateWithButton(pInt, &area);
	}
}

static int IsOverlapping(RW_Interface *pInt, int i)
{
    return (
    ((int32_t)sMOU->x>=GX.View.xmin+pInt->item[i].X)
    && ((int32_t)sMOU->x<=pInt->item[i].X+pInt->item[i].LX)
    && ((int32_t)sMOU->y>=GX.View.ymin +pInt->item[i].Y)
    && ((int32_t)sMOU->y<=pInt->item[i].Y+pInt->item[i].LY)
    );
}

static int CheckMouseOverlap(RW_Interface *pInt, int OldF)
{
    int f=OldF, i=0;
    RW.insideMouse = 0;
    do
    {
        if (IsOverlapping(pInt, i)) {f=i;RW.insideMouse=1;}
        i++;
    }while (!((f!= OldF) || (i>pInt->numItem-1)));
    return f;
}

#define deadZone     16382L
#define isjoyright  (sJOY->lX>32767L+deadZone)
#define isjoyleft   (sJOY->lX<32767L-deadZone)
#define isjoyup     (sJOY->lY<32767L-deadZone)
#define isjoydown   (sJOY->lY>32767L+deadZone)

static int ScanKeys(RW_Interface *pInt, int *pCurSel, int *pPrevSel)
{
    RW.dx = 0;
	RW.dy = 0;
	RW.ch = 0;

	if (sKEY_IsClicked(s_return))
		RW.ch = 13;

    if (sKEY_IsClicked(s_esc))
		RW.ch = 27;

	RW.dx = sKEY_IsClicked(s_right) - sKEY_IsClicked(s_left);
    RW.dy = sKEY_IsClicked(s_down) - sKEY_IsClicked(s_up);

	if (sJOY->numControllers)
	{
		static unsigned to;
		unsigned tc = timer_ms();
		if (tc > to)
		{
			to = tc + 125;
			if (!RW.dx)
				RW.dx = isjoyright ? 1 : (isjoyleft ? -1 : 0);

			if (!RW.dy)
				RW.dy = isjoydown ? 1 : (isjoyup ? -1 : 0);
		}

		if (sJOY_IsClicked(2))
		{
			RW.ch = 13;
		}

		if (sJOY_IsClicked(1))
		{
			RW.ch = 27;
		}
	}

	if ((RW.ch)||(RW.dx)||(RW.dy))
    {
        RW.isMouseHidden = 1;
        switch(pInt->keymode)
		{
            case RW_BOTH:
			{
				if (RW.dx)
					(*pCurSel)+=RW.dx;
				else
					(*pCurSel)+=RW.dy;
			}
			break;
            case RW_HORIZ:
			{
				(*pCurSel)+=RW.dx;
			}
            break;
            case RW_VERT:
			{
				(*pCurSel)+=RW.dy;
			}
            break;
        }
    }
	else
	{
		if (sMOU->lX || sMOU->lY)
			RW.isMouseHidden = 0;

	}

	if (*pCurSel<0)
	{
		(*pCurSel)=pInt->numItem-1;
		(*pPrevSel)= -1;
	}

	if (*pCurSel>=pInt->numItem)
	{
		(*pCurSel)=0;
		(*pPrevSel)= -1;
	}

	if (((RW.dx)||(RW.dy))&&(!RW.isMouseHidden))
    {
		sMOU->SetPosition(
			pInt->item[*pCurSel].X+(pInt->item[*pCurSel].LX>>1),
			pInt->item[*pCurSel].Y+(pInt->item[*pCurSel].LY>>1)
		);
    }
    return RW.ch;
}

int RW_Interface_Scan(RW_Interface *pInt, int pCurSel, int *ok, PFRWCALLBACK pfCallback)
{
	int key = 0;
    int pPrevSel = pCurSel, pushed = 0, pushOn = -1;

	if ((sMOU->numControllers)&&(!RW.isMouseHidden))
		pCurSel = CheckMouseOverlap(pInt, pPrevSel);

    *ok = 1;

	RW.current = pCurSel;
	pfCallback(pInt, 0);

    do
    {
		int ret;
		sKEY->Update(0);
		sJOY->Update(0);
		sMOU->Update(0);
		if (!RW.isLocked)
		{
			key = ScanKeys(pInt, &pCurSel, &pPrevSel);
			if (sMOU->numControllers && !RW.isMouseHidden)
			{
				pCurSel = CheckMouseOverlap(pInt, pCurSel);

				if ( sMOU_IsHeld(0) )
				{
					if (IsOverlapping(pInt, pCurSel))
					{
						pushed = 1;
						pushOn = pCurSel;
					}
				}

				if ( sMOU_IsReleased(0) )
				{
					if ( pushed && IsOverlapping(pInt, pushOn))
						pushed = 2;
					else
						pushed = 0;
				}

				if (pushed == 2)
				{
					if (IsOverlapping(pInt, pCurSel))
						key = 13;
				}
			}
		}

		RW.ch = key;
		RW.current = pCurSel;

		ret = pfCallback(pInt, 1);

		if (ret == -1)
			key = 27;

		if (ret == 1)
			key = 13;

		if (key==27)
			*ok = 0;

    }while (!((key==13) || (key==27)));

	return RW.current;
}

void RW_Create(GXSPRITEGROUP *pFont)
{
    sysMemZero(&RW, sizeof(RW));
	RW.pFont = pFont;
}

void RW_Release(void)
{
}

int RW_InputText(RW_Interface *pInt, char *t, int numChar, PFRWCALLBACK pfCallback)
{
    uint8_t finish = 0;
	int m_nNumSpaces = 0;
	char text[256], tmp[256];
	char m_scanCode = sKEY->scanCode;
	SYS_ASSERT(numChar<255);
    sysStrnCpy(text, t, 255);

	RW.current = 0;
	pfCallback(pInt, 0);

    do
    {
		sKEY->Update(0);
		sJOY->Update(0);

		if (sKEY->scanCode!=m_scanCode)
		{
			m_scanCode = sKEY->scanCode;

			switch(sKEY->scanCode)
			{
				case s_space:
				{
					if(strlen(text) < (size_t)(numChar-2))
					{
						strncat(text, " ", 1);
						m_nNumSpaces++;
					}
				}
				break;
				case s_delete:
				case s_backspace:
					{
						size_t tmp = strlen(text);
						if(tmp > 0)
						{
							if( text[tmp - 1] == ' ' )
							{
								text[tmp - 1] = '\0';
								m_nNumSpaces--;
							}
							else
							{
								text[tmp - 1] = '\0';
							}
						}
					}
					break;
				case s_esc:
					{
						finish = 2;
					}break;

				case s_return:
					{
						m_nNumSpaces = 0;
						finish = 1;

					}break;
				default:
					{
						size_t l = strlen(text);
						if(l < (size_t)(numChar-2))
						{
							char lastKey = sKEY->charCode;
							if(lastKey)
							{
								text[l] = lastKey;
								text[l+1] = 0;
							}
						}
					}
					break;
				}
			}

		sprintf(tmp, "%s%c", text, timer_ms()%1000<500 ? '_' : ' ');
		RW.szInputText = tmp;
		pfCallback(pInt, 2);

    }while(!finish);

	if (finish == 1)
		sysStrnCpy(t, text, numChar);
    return finish;
}

RW_Interface * RW_Interface_GetFp(SYS_FILEHANDLE in)
{
    RW_Interface *pInterface = RW_Interface_Create(0);
    char SPChead[4];
    GXSPRITEFORMAT spl;
    unsigned int i;
    unsigned short int max;
    FIO_cur->fread(SPChead, sizeof(char), 4, in);
    FIO_cur->fread(&max, sizeof(unsigned short int), 1, in);
#ifdef __BIG_ENDIAN__
    BSWAP16(&max, 1);
#endif
    for (i=0;i<max;i++)
    {
        FIO_cur->fread(&spl, sizeof(GXSPRITEFORMAT), 1, in);
#ifdef __BIG_ENDIAN__
        BSWAP16((uint16_t*)&spl.a, 4);
#endif
        RW_Zone_CreateWithSize(pInterface, spl.a, spl.b, spl.c-spl.a+1, spl.d-spl.b+1);
    }
    return pInterface;
}

RW_Interface * RW_Interface_GetFn(char *filename)
{
    SYS_FILEHANDLE in;
    RW_Interface *p = 0;
    in = FIO_cur->fopen(filename, "rb");
    if (in)
	{
		p = RW_Interface_GetFp(in);
		FIO_cur->fclose(in);
	}
    return p;
}


void RW_Interface_BuildTree(RW_Interface *spc)
{
    int i;
    RW_Button *root, *but;
	for (i=0, but=spc->item;i<spc->numItem;i++, but++)
    {
        if (spc->parent[i]<255)
        {
            root = spc->item + spc->parent[i];
            but->X-=root->X;
            but->Y-=root->Y;
            but->parent = root;
        }
        else but->parent = NULL;
    }
}

