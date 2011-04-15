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
#include <windows.h>
#include "_rlx32.h"
#include "_rlx.h"
#include "sysini.h"
#include "systools.h"
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void STUB_RegistryWrite(STUB_Registry *regs)
*
* DESCRIPTION : Sauvegarde de la base de registre
*
*/
int STUB_RegistryWrite(STUB_Registry *regs)
{
#ifndef RLXIOREADONLY
    FILE *in;
	char tex[256];
    sprintf(tex, "%s\\%s", regs->IniPath, regs->IniFilename);
    in = fopen(tex, "wt");
    if (in)
    {
		 fprintf(in, "// RLX Interface settings v3.10\n");
         fprintf(in, "\n[System]\n");
         fprintf(in, "Processor=%d\n", regs->System.Processor);
         fprintf(in, "Config=%x\n",    regs->System.Config);
         fprintf(in, "\n[Audio]\n");
         fprintf(in, "Config=%x\n",    regs->Audio.Config);
         fprintf(in, "WaveDeviceId=%d\n", regs->Audio.WaveDeviceId);
         fprintf(in, "MidiDeviceId=%d\n", regs->Audio.MidiDeviceId);
         fprintf(in, "SamplingRate=%d\n", regs->Audio.SamplingRate);
         fprintf(in, "ChannelToMix=%d\n", regs->Audio.ChannelToMix);
         fprintf(in, "\n[Video]\n");
         fprintf(in, "Config=%x\n",    regs->Video.Config);
         fprintf(in, "Id=%d\n",        regs->Video.Id);
         fprintf(in, "\n[3D]\n");
         fprintf(in, "Config=%x\n",    regs->V3X.Config);
		 fprintf(in, "Id=%d\n",        regs->V3X.Id);
		 fprintf(in, "TextureQuality=%d\n", regs->V3X.DefaultResize);
		 fprintf(in, "\n[Controller]\n");
		 fprintf(in, "Config=%x\n",    regs->Control.Config);
		 fprintf(in, "Id=%d\n",        regs->Control.Id);

		fclose(in);
		return 1;
    }
    else
		return 0;
#else
    UNUSED(regs);
    return 0;
#endif
}

