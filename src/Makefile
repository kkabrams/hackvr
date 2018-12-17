.PHONY: all install clean uninstall

CFLAGS=-Wall -pedantic -std=c99 -DGRAPHICAL -ffast-math

#all: hackvr_headless hackvr_x11 hackvr_opengl slowcat ### when hackvr_opengl gets useful at all I'll start including it in default build.
all: hackvr_headless hackvr_x11 hackvr_fb hackvr_freeglut slowcat

hackvr_headless: LDLIBS=-lm
hackvr_headless: hackvr_headless.o math.o physics.o

hackvr_x11: LDLIBS=-lm -lX11
hackvr_x11: hackvr_x11.o graphics_c3.o graphics_c2.o graphics_cs_x11.o math.o physics.o keyboard.o mouse_x11.o keyboard_x11.o
#notice how all the targets have generic graphics objects up until a specific one.

hackvr_fb: LDLIBS=-lm
hackvr_fb: hackvr_fb.o graphics_c3.o graphics_c2.o graphics_cs_fb.o math.o physics.o keyboard_die.o keyboard.o mouse_die.o

hackvr_opengl: LDLIBS=-lm -lGL -lGLU -lglut
hackvr_opengl: hackvr_opengl.o graphics_c3.o graphics_c2_opengl.o graphics_cs_opengl.o math.o physics.o

hackvr_freeglut: LDLIBS=-lm -lGL -lGLU -lglut
hackvr_freeglut: hackvr_freeglut.o graphics_c3_freeglut.o math.o physics.o keyboard.o mouse_die.o keyboard_die.o

hackvr_freeglut.o: LDLIBS=-lm -lGL -lGLU -lglut

graphics_c3_freeglut.o: LDLIBS=-lm -lGL -lGLU -lglut

hackvr_opengl.o: LDLIBS=-lm -lGL -lGLU -lglut
hackvr_x11.o: LDLIBS=-lm -lX11

hackvr_headless.o: CFLAGS=-Wall -pedantic -std=c99 -ffast-math
hackvr_headless.o: LDLIBS=-lm

graphics_c3.o: LDLIBS=-lm
graphics_c2.o: LDLIBS=-lm
physics.o: LDLIBS=-lm
keyboard.o: LDLIBS=-lm

graphics_cs_x11.o: LDLIBS=-lm -lX11
mouse_x11.o:-lX11
keyboard_x11.o:-lX11

graphics_c2_opengl.o: LDLIBS=-lm -lGL -lGLU -lglut
graphics_cs_opengl.o: LDLIBS=-lm -lGL -lGLU -lglut
math.o: LDLIBS=-lm

install:
	install -D hackvr $(PREFIX)/bin/hackvr
	install -D hackvr_headless $(PREFIX)/bin/hackvr_headless
	install -D hackvr_x11 $(PREFIX)/bin/hackvr_x11
	install -D hackvr_fb $(PREFIX)/bin/hackvr_fb
	install -D hackvr_freeglut $(PREFIX)/bin/hackvr_freeglut
	install -D slowcat $(PREFIX)/bin/slowcat

uninstall:
	rm $(PREFIX)/bin/hackvr
	rm $(PREFIX)/bin/hackvr_headless
	rm $(PREFIX)/bin/hackvr_x11
	rm $(PREFIX)/bin/hackvr_fb
	rm $(PREFIX)/bin/slowcat

clean:
	rm -f hackvr_freeglut
	rm -f hackvr_headless
	rm -f hackvr_opengl
	rm -f hackvr_x11
	rm -f hackvr_fb
	rm -f slowcat
	rm -f *.o
