CFLAGS = -c -g -D__LINUX__
INCLUDES = -I .. -I ../../rlx32/include -I ../../rlx32/src -I ../../rlx32/src/linux -I ../../rlx32/src/renderers/opengl -I ../../rlx32/src/renderers/opengl/linux
LIBS = -lpthread -lpng -lSDL -lopenal -lGL -logg -lvorbis
OBJS = ../../rlx32/src/systools.o \
       ../../rlx32/src/gx_csp.o \
       ../../rlx32/src/v3xscene.o \
       ../../rlx32/src/iss_form.o \
       ../../rlx32/src/iss_av.o \
       ../../rlx32/src/v3xcoll.o \
       ../../rlx32/src/gx_rgb.o \
       ../../rlx32/src/syslist.o \
       ../../rlx32/src/gui_os.o \
       ../../rlx32/src/gx_init.o \
       ../../rlx32/src/syszlib.o \
       ../../rlx32/src/sysresmx.o \
       ../../rlx32/src/v3xsoft.o \
       ../../rlx32/src/iss_ogg.o \
       ../../rlx32/src/iss_cdc.o \
       ../../rlx32/src/v3x_1.o \
       ../../rlx32/src/gx_tool.o \
       ../../rlx32/src/v3xrend.o \
       ../../rlx32/src/gx_flc.o \
       ../../rlx32/src/v3x_3.o \
       ../../rlx32/src/_stub.o \
       ../../rlx32/src/v3xscen2.o \
       ../../rlx32/src/sysini.o \
       ../../rlx32/src/gx_form.o \
       ../../rlx32/src/iss_db.o \
       ../../rlx32/src/gx_alloc.o \
       ../../rlx32/src/v3x_2.o \
       ../../rlx32/src/v3xdata.o \
       ../../rlx32/src/v3xmaps.o \
       ../../rlx32/src/sysgi.o \
       ../../rlx32/src/gx_png.o \
       ../../rlx32/src/linux/systools.o \
       ../../rlx32/src/linux/systime.o \
       ../../rlx32/src/linux/snd_openal.o \
       ../../rlx32/src/linux/sysdbg.o \
       ../../rlx32/src/linux/_stublinux.o \
       ../../rlx32/src/linux/sysnet.o \
       ../../rlx32/src/linux/sysreg.o \
       ../../rlx32/src/linux/sysjoy.o \
       ../../rlx32/src/linux/syskeyb.o \
       ../../rlx32/src/linux/sysmouse.o \
       ../../rlx32/src/renderers/opengl/gl_gx.o \
       ../../rlx32/src/renderers/opengl/gl_v3x.o \
       ../../rlx32/src/renderers/opengl/linux/sdl_gl.o \
       ../../src/lt_game.o \
       ../../src/lt_weap.o \
       ../../src/lt_info.o \
       ../../src/lt_video.o \
       ../../src/lt_game2.o \
       ../../src/ccmx.o \
       ../../src/lt_menu.o \
       ../../src/lt_sound.o \
       ../../src/lt_save.o \
       ../../src/lt_reso.o \
       ../../src/lt_fx.o \
       ../../src/console.o \
       ../../src/lt_input.o \
       ../../src/lt_data.o \
       ../../src/lt_start.o \
       ../../src/lithium.o \
       ../../src/lt_st2.o \
       ../../src/Linux/preferences.o
TARGET = ../../nogravity

$(TARGET): $(OBJS)
	g++ $(OBJS) $(LIBS) -o $(TARGET)

.PHONY: clean

clean:
	rm -f $(OBJS) $(TARGET)

%.o: %.c
	gcc $(CFLAGS) $^ -o $@ $(INCLUDES)

%.o: %.cpp
	g++ $(CFLAGS) $^ -o $@ $(INCLUDES)
