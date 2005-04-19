===================================================================
No Gravity (v1.98 Version) Source Code Release - February 13, 2005
===================================================================

LEGAL STUFF
-----------

"No Gravity" is a registered trademark of RealTech VR, Ltd. 

"No Gravity" copyright 1996 - 2005 RealTech. All trademarks and copyrights reserved.

This is the complete source code for No Gravity version 1.98, buildable as detailed in the next section.

The code is licensed under the terms of the GPL (gnu public license).  You should read the entire license (filename "GNU.TXT" in this archive), so you understand your rights and what you can and cannot do with the source code from this release.

All of the No Gravity data files remain copyrighted and licensed by RealTech under the original terms. You can use our data for personal entertainment or educational purposes. 

Please do not contact us for possible commercial exploitation of No Gravity -- we will not be interested.

Please note that this is being released without any kind of support from RealTech. 

 We cannot help in getting this running and we do not guarantee that you will be able to get it to work, nor do we guarantee that it won't blow up your computer if you do try and use it.  Use at your own risk.


CREDITS
-----------
Programming : Stephane Denis
Graphics : Romuald Genevois
Music: Philippe Meisburger


======================================================
HOW TO COMPILE THE SOURCE CODE (by Stephane Denis)
======================================================

Before compiling you need the following packages

- Zlib 
- libPNG 
- oggvorbis
- the game data (NOGRAVITY.RMX 37,127 KB)

The projects depends of it.

There are four projects available for
- Visual Studio 2003 
- XCode 1.5 
- BeIDE
- autoconf/automake

The source code 
is in three parts:
- The game code.
- The engine code.
- The renderer.

The game code depends of the engine code and is completely hardware and OS independant.

The engine code is in several parts:
- One OS independant part (/rlx32/src)
- An OS dependant part for each platform (Win32, MacOS X and BeOS)
- The renderers for OpenGL and a software renderer implementation. 
  (There is a DX6 renderer but it is quite broken).
  
You need to compile the game code and engine code.
Then compile the OS dependant part.

After that, you need to build a rendererer. (on PC, it's a dynamic library, on MacOS, it's built-in (Because it's only support OpenGL)).

The software renderer is written in C. It is completely OS independant.

BeOS:
You don't need to recompile zlib, png and oggvorbis. there is libz.so, png.so and ogg.so and vorbis.so available
which are fine.
The output is

/bin/nogravity
/bin/nogravity.rmx
/bin/lib/sg_soft.so

The software renderer is relying on BDirectWindow class.
