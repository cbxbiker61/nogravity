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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "_rlx32.h"

#include "systools.h"
#include "sysresmx.h"
#include "gx_struc.h"
#include "gx_init.h"
#include "gx_tools.h"
#include "gx_csp.h"
#include "gx_rgb.h"
#include "gx_flc.h"
#include "gx_cdc.h"
#include "_rlx.h"

IMG_Codec IMG_FormList[]={
    {".png", PNG_unpack}, 
	{NULL, NULL}
};

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int IMG_LoadFp(char *filename, SYS_FILEHANDLE fp, GXSPRITE *sp)
*
* DESCRIPTION :  
*
*/
int IMG_LoadFp(const char *filename, SYS_FILEHANDLE fp, GXSPRITE *sp)
{
    int bpp, op=1;
    GXSPRITE spp;
    IMG_Codec *upk=IMG_FormList;
    if (!sp) 
	{
		sp=&spp;
		op=0;
	}
    sp->data = NULL;
    sp->handle = NULL;
    if (RLX.Video.Config&RLXVIDEO_ConvertImg8bit) 
		op|=2;
    while (upk->ext!=NULL)
    {
        if (strstr(filename, upk->ext))
        {
            sp->data = upk->unpack(fp, &sp->LX, &sp->LY, &bpp, GX.ColorTable, op);
			SYS_ASSERT(sp->data);
            return bpp;
        }
        upk++;
    }
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int IMG_LoadFn(char *filename, char *res, GXSPRITE *sp)
*
* DESCRIPTION :
*
*/
int IMG_LoadFn(const char *filename, GXSPRITE *sp)
{
    SYS_FILEHANDLE fp;
    int bpp;
    FIO_cur=&FIO_res;
    fp = FIO_cur->fopen(filename, "rb");
    if (fp!=NULL)
    {
        bpp = IMG_LoadFp(filename, fp, sp);
        FIO_cur->fclose(fp);
        if ((!sp)||(sp->data!=NULL)) return bpp;
    }
    if (sp) sp->data = NULL;
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void Load_Background(char *filename, int options)
*
* DESCRIPTION :
*
*/
void IMG_LoadFnSprite(char *file, GXSPRITE *sp, int options)
{    
	int32_t dp = GX.View.BytePerPixel;

    if (options==1)
    {
        IMG_LoadFn(file, sp);
        return;
    }

    if (options&2)
    {		
        u_int8_t *p = NULL;
        int act = MM_heap.active, vp;
        MM_heap.active = 0;

        vp = IMG_LoadFn(file, sp);
		if (!vp) 
			return;

        if (vp==4)
			vp=1;

        if (vp>=8) 
			vp>>=3;

        if (options&4) 
			CSP_Resize(sp, GX.View.lWidth, GX.View.lHeight, vp);

        p = sp->data;
        MM_heap.active = act;
        if (options&8)
            sp->data = (u_int8_t*)RGB_SmartConverter(NULL, GX.ColorTable, dp, 
												 sp->data, GX.ColorTable, vp, sp->LX*sp->LY);            
        else
	        CSP_AntiAliasRGB(sp, dp, vp);
    }
    else
    {
        if (!IMG_LoadFn(file, sp)) 
			return;

        if (options&4)
			CSP_Resize(sp, GX.View.lWidth, GX.View.lHeight, dp);
    }
    return;
}
