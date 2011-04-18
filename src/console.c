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
#include <string.h>
#include <stdarg.h>
#include "_rlx32.h"

#include "sysresmx.h"
#include "sysctrl.h"
#include "systime.h"
#include "systools.h"

#include "gx_struc.h"
#include "gx_csp.h"
#include "gx_rgb.h"

#include "console.h"

struct sys_console_item
{
	char						text[MAX_SYS_CONSOLE_TEXT];
	struct sys_console_item	*	prev;
	struct sys_console_item	*	next;
};

struct sys_console_cmd
{
    struct sys_console_cmd	*	next;
    char						szCmd[MAX_SYS_CONSOLE_TEXT];
    PFV3XCNLCMD					pfFunc;
};

struct sys_console_cvar
{
    struct sys_console_cvar	 *	next;
    char						szVar[MAX_SYS_CONSOLE_TEXT];
    void					 *  pData;
};

static struct sys_console_item	*	m_pFirst;
static struct sys_console_item	*	m_pLast;
static struct sys_console_item	*	m_pCurrent;
static struct sys_console_cmd	*	m_pFirstCmd;

static struct sys_console_cvar	*	m_pFirstCvar;
static struct sys_console_cmd	*	m_pLastCmd;
static struct sys_console_cvar	*	m_pLastCvar;

static struct sys_console_item		m_pInput;

static int							m_nNumLines;
static int							m_nNumSpaces;
static int							m_scanCode;
static int							m_bActive;

static int							m_x, m_y, m_w, m_h;
static int							m_cHd, m_cH;

static GXSPRITEGROUP			*	m_pFont;

void sysConClear()
{
	struct sys_console_item *p = m_pFirst;
	while(p)
	{
		struct sys_console_item *pp = p->next;
		free(p);
		p = pp;
	}
	m_pFirst = 0;
	m_pLast = 0;
	m_pCurrent = 0;
}

void sysConPrint(const char *newmsg, ...)
{
	struct sys_console_item *p;
    va_list		argptr;
    char		msg[512];
    va_start (argptr,newmsg);
    vsprintf (msg, newmsg, argptr);
    va_end (argptr);

#ifdef _DEBUG
    SYS_Debug("%s\n", msg);
#endif

	p = (struct sys_console_item*) malloc (sizeof(struct sys_console_item));
	p->prev = 0;
	p->next = 0;
	sysStrnCpy(p->text, msg, MAX_SYS_CONSOLE_TEXT);

	if (!m_pFirst)
		m_pFirst = m_pLast = p;
	else
	if (m_pLast)
	{
		m_pLast->next = p;
		p->prev = m_pLast;
		m_pLast = p;
	}
	m_pCurrent = m_pLast;
}

int sysConIsActive()
{
	return !!m_bActive;
}

void sysConToggle()
{
	if (!m_cHd)
	{
		m_cHd = 16;
		m_bActive = TRUE;
	}
	else
		m_cHd = -m_cHd;
}

void sysConHandleInput()
{
	m_cH+=m_cHd;
	if (m_cH < 0)
	{
		m_cH = 0;
		m_bActive = FALSE;
		m_cHd = 0;
	}

	if (m_cH > m_h)
	{
		m_cH = m_h;
	}

    if (sKEY_IsClicked(s_tilda))
    {
		sysConToggle();
        return;
    }

    // If the console is not active, there's no need to process input
    if (!sysConIsActive())
		return;

	if (sKEY->scanCode!=m_scanCode)
	{
		m_scanCode = sKEY->scanCode;
	//	sKEY->scanCode = 0;

		switch(sKEY->scanCode)
		{
			case s_space:
			{
				if(strlen(m_pInput.text) < MAX_SYS_CONSOLE_TEXT-2)
				{
					strncat(m_pInput.text, " ", 1);
					m_nNumSpaces++;
				}
			}
			break;
			case s_delete:
			case s_backspace:
				{
					size_t tmp = strlen(m_pInput.text);
					if(tmp > 0)
					{
						if( m_pInput.text[tmp - 1] == ' ' )
						{
							m_pInput.text[tmp - 1] = '\0';
							m_nNumSpaces--;
						}
						else
						{
							m_pInput.text[tmp - 1] = '\0';
						}
					}
				}
				break;
				case s_tilda: //ignore
				{
				}break;
			//case s_numreturn:
			case s_return:
				{
					//If console_buffer[0] strlen() != 0
					//1. Push the m_pInput.text unto the console_buffer
					//2. parse the text

					sysConPrint("%s",m_pInput.text);
					m_nNumSpaces = 0;
					sysConParse(m_pInput.text);

					sysMemZero(m_pInput.text, MAX_SYS_CONSOLE_TEXT);

				}break;
			case s_down:
				{
					if(m_pCurrent->next != NULL)
					{
						m_pCurrent = m_pCurrent->next;
					}
				}break;
			case s_pagedown:
				{
					int i;
					for(i = 0; i < m_nNumLines; i++)
					{
						if(m_pCurrent->next != NULL)
						{
							m_pCurrent = m_pCurrent->next;
						}
						else
						{
							break;
						}
					}
				}break;
			case s_up:
				{
					if (m_pCurrent)
					if (m_pCurrent->prev != NULL)
					{
						m_pCurrent = m_pCurrent->prev;
					}
				}break;
			case s_pageup:
				{
					int i;
					for(i = 0; i < m_nNumLines; i++)
					{
						if(m_pCurrent->prev != NULL)
						{
							m_pCurrent = m_pCurrent->prev;
						}
						else
						{
							break;
						}
					}
				}break;
			case s_end:
				{
					m_pCurrent = m_pLast;
				}break;

			case s_home:
				{
					m_pCurrent = m_pFirst;
				}break;
			case s_esc:
				{
					m_bActive = 0;
				}break;
			default:
				{
					size_t l = strlen(m_pInput.text);
					if(l < MAX_SYS_CONSOLE_TEXT-2)
					{
						char lastKey = sKEY->charCode;
						if(lastKey)
						{
							m_pInput.text[l] = lastKey;
							m_pInput.text[l+1] = 0;
						}
					}
				}
				break;
			}
		}
}

void sysConSave(const char *szFilename)
{
	SYS_FILEHANDLE fp = FIO_std.fopen(szFilename, "wt");
	if (fp)
	{
		struct sys_console_item *p = m_pFirst;
		while(p)
		{
			FIO_std.fwrite(p->text, strlen(p->text), 1, fp);
			FIO_std.fwrite("\n",2, 1, fp);
			p = p->next;
		}
		sysConPrint("File saved.");
	}
	else
		sysConPrint("Can't write file");
}

int sysConBindCmd(const char *szCmd, PFV3XCNLCMD pfFunc)
{
	struct sys_console_cmd *p = (struct sys_console_cmd*) malloc(sizeof(struct sys_console_cmd ));
	p->next = 0;
	sysStrnCpy(p->szCmd, szCmd, MAX_SYS_CONSOLE_TEXT);
	p->pfFunc = pfFunc;

	if (!m_pFirstCmd)
		m_pFirstCmd = m_pLastCmd = p;
	else
	if (m_pLastCmd)
	{
		m_pLastCmd->next = p;
		m_pLastCmd = p;
	}
	return 0;
}

int sysConBindCVar(const char *szVar, const void *pData)
{
	struct sys_console_cvar *p = (struct sys_console_cvar*) malloc(sizeof(struct sys_console_cvar));
	p->next = 0;
	sysStrnCpy(p->szVar, szVar, MAX_SYS_CONSOLE_TEXT);
	p->pData = (void*)pData;

	if (!m_pFirstCvar)
		m_pFirstCvar = m_pLastCvar = p;
	else
	if (m_pLastCvar)
	{
		m_pLastCvar->next = p;
		m_pLastCvar = p;
	}
	return 0;
}

int onClear(char *parms)
{
    sysConClear();
	UNUSED(parms);
	return 0;
}

static int onConDump(char *parms)
{
	if (parms)
		sysConSave(parms);
	return 0;
}

static int onClose(char *parms)
{
	m_bActive = 0;
	UNUSED(parms);
	return 0;
}

static int onCvarList(char *parms)
{
	struct sys_console_cvar *p = m_pFirstCvar;
	int n = 0;
	while(p)
	{
		sysConPrint(p->szVar);
		p = p->next;
		n++;
	}
	UNUSED(parms);
	sysConPrint("%d var(s) found", n);
	return 0;
}

static int onCmdList(char *parms)
{
	struct sys_console_cmd *p = m_pFirstCmd;
	int n = 0;
	while(p)
	{
		sysConPrint(p->szCmd);
		p = p->next;
		n++;
	}
	sysConPrint("%d command(s) found", n);
	UNUSED(parms);
	return 0;
}

void sysConCreate()
{
	m_pFirst = 0;
	m_pLast = 0;
	m_pCurrent = 0;
	m_pFirstCmd = 0;
    m_pFirstCvar = 0;
	m_pLastCmd = 0;
	m_pLastCvar = 0;

	m_nNumLines = 0;
	m_nNumSpaces = 0;
	m_scanCode = 0;
	m_bActive = 0;

	m_x = 0;
    m_y = 0;
    m_w = 0;
    m_h = 0;
	m_cHd = 0;
    m_cH = 0;

	sysMemZero(&m_pInput, sizeof(m_pInput));
	sysConBindCmd("/clear", onClear);
	sysConBindCmd("/close", onClose);
	sysConBindCmd("/cmdlist", onCmdList);
	sysConBindCmd("/cvarlist", onCvarList);
	sysConBindCmd("/condump", onConDump);
}

int sysConParse(char *cmd)
{
	struct sys_console_cmd *p = m_pFirstCmd;

	char seps[] = " ";
	char *token;

	if (!cmd)
		return -1;

	token = strtok( cmd, seps );
	if (!token)
		return -2;

	while (p)
	{
		if (!strcmp(p->szCmd, token))
		{
			return p->pfFunc(strtok( NULL , seps ));
		}
		p = p->next;
	}
	return -1;
}

void sysConSetLimits(int x, int y, int w, int h)
{
	m_x = x;
	m_y = y;
	m_w = w;
	m_h = h;
	if (m_pFont)
	{
		m_nNumLines = (int)( h / m_pFont->item[0].LY);
	}
}

void sysConRender()
{
	int m_left = m_x+10;
	int m_top = m_y+10;
//	int m_right = m_x+m_w-10;
	int m_bottom = m_y+m_cH-10;

	int x = m_left;
	int y = m_bottom - m_pFont->item[0].LY;

	intptr_t cl = RGB_PixelFormat(128,128,128);
	if (!sysConIsActive())
		return;

	CSP_Color(0);
	CSP_Alpha(128);
	GX.gi.drawShadedRect(m_x, m_y, m_w, m_cH, (void*)cl);

	CSP_Color(0xffffffff);

	{
		char szOut[512];
		sprintf(szOut,"%s%c", m_pInput.text, (timer_ms()%1000 < 500) ? '_' : ' ');

		{
			int v = m_pFont->item[0].LY + 1;
			int n = 1; //m_pFont->getJustified(szOut, (int)x, (int)y, (int)(m_right-m_left));
			if (y>=m_top)
			{

				CSP_DrawText(szOut, x, y, m_pFont, GX.csp_cfg.put);
				// m_pFont->printJustified(szOut, (int)x, (int)y, (int)(m_right-m_left));
				y-= n * v;
			}

			{
				struct sys_console_item *p = m_pCurrent;

				while(p)
				{
					//CSP_DrawText(p->text, x, y, m_pFont, GX.csp_cfg.put);
					// n = m_pFont->getJustified(p->text, (int)x, (int)y, (int)(m_right-m_left));
					y-= n * v;
					if (y<m_top)
						break;

					CSP_DrawText(p->text, x, y, m_pFont, GX.csp_cfg.put);
					// m_pFont->printJustified(p->text, (int)x, (int)y, (int)(m_right-m_left));
					p = p->prev;

				}
			}
		}
	}
}

void sysConSetFont(GXSPRITEGROUP *font)
{
	m_pFont = font;
}

