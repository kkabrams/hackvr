.PHONY: all install clean uninstall

CFLAGS=-Wall -pedantic -std=c99 -DGRAPHICAL -ffast-math

#all: hackvr_headless hackvr_x11 hackvr_opengl slowcat ### when hackvr_opengl gets useful at all I'll start including it in default build.
all: hackvr_headless hackvr_x11 hackvr_fb slowcat

hackvr_headless: LDLIBS=-lm
hackvr_headless: hackvr_headless.o math.o

hackvr_x11: LDLIBS=-lm -lX11
hackvr_x11: hackvr_x11.o graphics_c3.o graphics_c2.o graphics_cs_x11.o math.o
#notice how all the targets have generic graphics objects up until a specific one.

hackvr_fb: LDLIBS=-lm
hackvr_fb: hackvr_fb.o graphics_c3.o graphics_c2.o graphics_cs_fb.o math.o

hackvr_opengl: LDLIBS=-lm -lGL -lGLU -lglut
hackvr_opengl: hackvr_opengl.o graphics_c3.o graphics_c2_opengl.o graphics_cs_opengl.o math.o

hackvr_opengl.o: LDLIBS=-lm -lGL -lGLU -lglut
hackvr_x11.o: LDLIBS=-lm -lX11

hackvr_headless.o: CFLAGS=-Wall -pedantic -std=c99 -ffast-math
hackvr_headless.o: LDLIBS=-lm

graphics_c3.o: LDLIBS=-lm
graphics_c2.o: LDLIBS=-lm

graphics_cs_x11.o: LDLIBS=-lm -lX11

graphics_c2_opengl.o: LDLIBS=-lm -lGL -lGLU -lglut
graphics_cs_opengl.o: LDLIBS=-lm -lGL -lGLU -lglut
math.o: LDLIBS=-lm

install:
	install hackvr $(PREFIX)/bin/hackvr
	install hackvr_headless $(PREFIX)/bin/hackvr_headless
	install hackvr_x11 $(PREFIX)/bin/hackvr_x11
	install hackvr_fb $(PREFIX)/bin/hackvr_fb
	install slowcat $(PREFIX)/bin/slowcat

uninstall:
	rm $(PREFIX)/bin/hackvr
	rm $(PREFIX)/bin/hackvr_headless
	rm $(PREFIX)/bin/hackvr_x11
	rm $(PREFIX)/bin/hackvr_fb
	rm $(PREFIX)/bin/slowcat

clean:
	rm -f hackvr
	rm -f hackvr_headless
	rm -f hackvr_opengl
	rm -f hackvr_x11
	rm -f hackvr_fb
	rm -f slowcat
	rm -f *.o
