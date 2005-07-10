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
#ifndef __RLXPARAMETERS
#define __RLXPARAMETERS

#define __RLXVERSION__ 0x170

#if defined __BEOS__
    #include <byteorder.h>
    #if B_HOST_IS_BENDIAN
        #define __BIG_ENDIAN__
    #endif
    
#elif defined __APPLE__ && defined __MACH__
	#define __MACOS__
    #define __C32__

#elif defined __amigaos4__ || defined __MORPHOS__
    #define __C32__
    #ifndef __BIG_ENDIAN__
    #define __BIG_ENDIAN__
    #endif
#endif

#ifdef HAVE_CONFIG_H
    #include "autoconfig.h"
#endif

#endif
