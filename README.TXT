===================================================================
No Gravity (v1.99 Version) Source Code Release - February 13, 2005
===================================================================

 ===========================================================
1. LEGAL STUFF
-----------

"No Gravity" is a registered trademark of RealTech VR, Ltd.

"No Gravity" copyright 1996 - 2005 RealTech. All trademarks and copyrights reserved.

This is the complete source code for No Gravity version 1.98, buildable as detailed in the next section.

The code is licensed under the terms of the GPL (gnu public license).  You should read the entire license (filename "GNU.TXT" in this archive), so you understand your rights and what you can and cannot do with the source code from this release.

All of the No Gravity data files remain copyrighted and licensed by RealTech under the original terms. You can use our data for personal entertainment or educational purposes.

Please do not contact us for possible commercial exploitation of No Gravity -- we will not be interested.

Please note that this is being released without any kind of support from RealTech.

 We cannot help in getting this running and we do not guarantee that you will be able to get it to work, nor do we guarantee that it won't blow up your computer if you do try and use it.  Use at your own risk.


 ===========================================================
2. Installation


For MacOSX users, you need to do the following steps : 

Download the No Gravity (Combo Bin+Data - Mac/Linux only) package for MacOS X v10.2 or later, then double click on the .PKG file and follow the instructions. Double click on your 'Hard drive' icon and double click 'No Gravity'. 
For MacOS X 10.3 users: Be sure to have OpenAL installed (See at the bottom of the page to get it) and MacOS 10.3.9 installed. 


For Linux users, Download the No Gravity (Combo Bin+Data - Mac/Linux only) for Linux i386 package and run the autopackage, or if you are an experienced user, recompiles the CVS version as follows :


Get the source from CVS. 
cvs -d:pserver:anonymous@cvs.sourceforge.net:/cvsroot/nogravity login
cvs -z3 -d:pserver:anonymous@cvs.sourceforge.net:/cvsroot/nogravity co -P nogravity

"Bootstrap" the build environment. 
cd nogravity/src/Linux
./bootstrap

Configure the build (enabling SDL_mixer for sound). 
./configure --enable-sound=sdl_mixer (add --disable-opengl for forcing software rendering)

Build the binary. 
make


Download the No Gravity (Game Data only) for All (choose this option for Game Data), unzip the package. 

If this all succeeds, now just copy NOGRAVITY.RMX into the src/Linux directory and run ./nogravity.


For Windows users, you need to do the following steps : 
Download the No Gravity (Binaries only) for Windows 2000/XP/DirectX 8 or later and run it. 
Download the No Gravity (Game Data only) for All (choose this option for Game Data), unzip the package and copy NOGRAVITY.RMX into it C:\Program File\Realtech VR\No Gravity folder 
For Windows 2000 users, you must have at least DirectX 8 or better installed


 ===========================================================
4.  Credits

Programming : Stephane Denis
Graphics : Romuald Genevois
Music: Philippe Meisburger
Linux / SDL Port : Matt Williams
Amiga OS4 Port : Juha Niemimaki


 ===========================================================
5.  Project page
http://www.realtech-vr.com/nogravity
http://sourceforge.net/projects/nogravity


 ===========================================================
6. DEPENDENCIES
  OGG / Vorbis
  zLib
  LibPNG
  SDL
  OpenAL
  OpenGL


 ===========================================================
 7. General Gameplay Information
  
   7.1 Keyboard Controls

     Help can be available during the game by pressing F1

     7.1.1  Control 

      L                 : Lock Target
      Tab               : AfterBurner, give a boost toward
      BackSpace         : RetroBooster, give a boost backward
      Space             : Use selected item, use - and = near the backspace key
      A                 : Go to next NAV point ( Q on french keyboard ) when
                          requested. It will warp one on the next battle theatre
      0 and .           : Rolling space ship.
      + and -           : Select default speed. 
      Right Shift       : Speed up
      /                 : Full Speed
      Alt Gr            : Fire with select weapons, can be select with 1 to 5
                          when ammo is exhausted, it will change a other weapons

     7.1.2  Communications
    
      Press C for communicate with your wingmen. Select an order. For
      Attack this Target, lock a target before. For Request status, lock
      one of your wingman.  
      During network play, use T key to send messages.
 
     7.1.3  Views

      F2   : Cockpit view          F3   : Cycle view clockwise
      F4   : Cycle View            F5   : MiniCam (Rear view window)
      F6   : NAV Point view        F7   : Missile view window

     7.1.4  Details Level

     F9   : Sky On/Low/Off        
     F11  : Filtering On/Off      


     7.1.5  Miscellanous

     F1                   : Help 
     PrtScrn              : Dump a .PCX or .TGA (on c:\rt)
     I                    : HUD Display On/Off (faster for 3D acceleration)          
     T                    : Talk (Netchat)

     7.1.6   Weapon
 
     TwinLaser            : Small energy shoot
     QuadLaser            : Medium energy shoot. Used for dog fight
     OctoLaser            : Large energy shoot. Slower, but make a lot of damage
     Freezer              : Freezes any objects for few seconds
     Missile              : Missile Friend-Or-Foe. Need to lock the target
                            Excellent for dog fight
     Torpedo              : Super Missile . Quite long to lock . Not recommanded
                            for dog fight but for large enemy base.

     7.1.7  Bonus

     Ammo                 : Give more shoots for weapons
     Cell                 : Add +25%
     MegaCell             : Restore all energy
     Stealth              : Invisibility. Enemy will not attack you for 20 seconds
     SystemShield         : Invicibility !
     Cloak Device         : Warp near to the nav point
     BlackHole            : Destroy everything around you                                        

  *END OF FILE*
@

