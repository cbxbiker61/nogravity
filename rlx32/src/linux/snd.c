//-------------------------------------------------------------------------
/*
Copyright (C) 2005 - Matt Williams

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

Linux/SDL Port: 2005 - Matt Williams
*/
//-------------------------------------------------------------------------

#include "_rlx32.h"

/* Determine the right sound driver based on the available libraries. */
#if defined(HAVE_AL_ALC_H) && defined(HAVE_AL_AL_H) && defined(HAVE_LIBOPENAL)
    #include "snd_openal.c"
#elif defined(HAVE_SDL_SDL_MIXER_H) && defined(HAVE_LIBSDL_MIXER)
    #include "snd_sdlmixer.c"
#else
    #include "snd_none.c"
#endif
