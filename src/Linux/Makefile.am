if USE_OPENGL
RENDERER = ../../rlx32/src/renderers/opengl/gl_gx.cpp \
           ../../rlx32/src/renderers/opengl/gl_v3x.cpp \
           ../../rlx32/src/renderers/opengl/linux/sdl_gl.cpp
else
RENDERER = ../../rlx32/src/renderers/v3x_ref/z16_256.c \
           ../../rlx32/src/renderers/v3x_ref/r16_256.c \
           ../../rlx32/src/renderers/v3x_ref/r08_256.c\
           ../../rlx32/src/renderers/v3x_ref/z08_256.c \
           ../../rlx32/src/renderers/v3x_ref/r16_128.c \
           ../../rlx32/src/renderers/v3x_ref/z32_256.c \
           ../../rlx32/src/renderers/v3x_ref/r08_128.c \
           ../../rlx32/src/renderers/v3x_ref/r32_256.c \
           ../../rlx32/src/renderers/v3x_ref/v3xref.c \
           ../../rlx32/src/renderers/gfx_ref/sp8bit.c \
           ../../rlx32/src/renderers/gfx_ref/sp16bit.c \
           ../../rlx32/src/renderers/gfx_ref/gr32bit.c \
           ../../rlx32/src/renderers/gfx_ref/gx_drv.c \
           ../../rlx32/src/renderers/gfx_ref/gr16bit.c \
           ../../rlx32/src/renderers/gfx_ref/gr8bit.c \
           ../../rlx32/src/renderers/gfx_ref/sp32bit.c \
           ../../rlx32/src/linux/gx_sdl.cpp
endif

nogravitydir = $(prefix)/games/nogravity
nogravity_PROGRAMS = nogravity
nogravity_DATA = ../../GNU.TXT ../../README.TXT
nogravity_SOURCES = ../../rlx32/src/systools.c \
                    ../../rlx32/src/gx_csp.c \
                    ../../rlx32/src/v3xscene.c \
                    ../../rlx32/src/iss_form.c \
                    ../../rlx32/src/iss_av.c \
                    ../../rlx32/src/v3xcoll.c \
                    ../../rlx32/src/gx_rgb.c \
                    ../../rlx32/src/syslist.c \
                    ../../rlx32/src/gui_os.c \
                    ../../rlx32/src/gx_init.c \
                    ../../rlx32/src/syszlib.c \
                    ../../rlx32/src/sysresmx.c \
                    ../../rlx32/src/v3xsoft.c \
                    ../../rlx32/src/iss_ogg.c \
                    ../../rlx32/src/iss_cdc.c \
                    ../../rlx32/src/v3x_1.c \
                    ../../rlx32/src/gx_tool.c \
                    ../../rlx32/src/v3xrend.c \
                    ../../rlx32/src/gx_flc.c \
                    ../../rlx32/src/v3x_3.c \
                    ../../rlx32/src/_stub.cpp \
                    ../../rlx32/src/v3xscen2.c \
                    ../../rlx32/src/sysini.c \
                    ../../rlx32/src/gx_form.c \
                    ../../rlx32/src/iss_db.c \
                    ../../rlx32/src/gx_alloc.c \
                    ../../rlx32/src/v3x_2.c \
                    ../../rlx32/src/v3xdata.c \
                    ../../rlx32/src/v3xmaps.c \
                    ../../rlx32/src/sysgi.c \
                    ../../rlx32/src/gx_png.c \
                    ../../rlx32/src/linux/systool2.c \
                    ../../rlx32/src/linux/systime.c \
                    ../../rlx32/src/linux/snd.c \
                    ../../rlx32/src/linux/sysdbg.c \
                    ../../rlx32/src/linux/_stublinux.cpp \
                    ../../rlx32/src/linux/sysnet.c \
                    ../../rlx32/src/linux/sysreg.c \
                    ../../rlx32/src/linux/sysjoy.c \
                    ../../rlx32/src/linux/syskeyb.c \
                    ../../rlx32/src/linux/sysmouse.c \
                    ../../src/lt_game.c \
                    ../../src/lt_weap.c \
                    ../../src/lt_info.c \
                    ../../src/lt_video.c \
                    ../../src/lt_game2.c \
                    ../../src/ccmx.c \
                    ../../src/lt_menu.c \
                    ../../src/lt_sound.c \
                    ../../src/lt_save.c \
                    ../../src/lt_reso.c \
                    ../../src/lt_fx.c \
                    ../../src/console.c \
                    ../../src/lt_input.c \
                    ../../src/lt_data.c \
                    ../../src/lt_start.c \
                    ../../src/lithium.c \
                    ../../src/lt_st2.c \
                    ../../src/Linux/preferences.c \
                    $(RENDERER)
AM_CPPFLAGS = -I ../../rlx32/include -I ../../rlx32/src -I ../../rlx32/src/linux -I ../../rlx32/src/renderers/opengl -I ../../rlx32/src/renderers/opengl/linux -D__LINUX__
