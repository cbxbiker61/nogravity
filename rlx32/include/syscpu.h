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

#ifndef __SYSCPU_H
#define __SYSCPU_H
typedef struct {
    union __v{
	struct _version {
	    unsigned stepping :  4;
	    unsigned model    :  4;
	    unsigned family   :  4;
	    unsigned pad2     : 20;
	}version;
	unsigned eax;
    };
    union __f{
        struct _features {
            unsigned pad0              :  7;
            unsigned hasCMPXCHG8B      :  1;
            unsigned hasRDPMC          :  4;
            unsigned pad1              :  4;
            unsigned hasCMOV           :  1;
            unsigned pad2              :  5;

            unsigned hasMMXextensions  :  1;
            unsigned hasFXsave         :  1;
            unsigned hasXMMXextensions :  1;
            unsigned pad               :  7;
        }features;
        unsigned edx;
    };
}PM_CPUID_Informations;

typedef struct {
    char                  vendorName[16];
    char                  name[16+12];
    PM_CPUID_Informations info;
    unsigned              hasnoCpuid;
    unsigned		  pad;
}SYSCPUID;

__extern_c
    void PM_CPUID_GetInfo(SYSCPUID *cpu);
__end_extern_c

#endif

