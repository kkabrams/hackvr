.PHONY: all install clean uninstall status

PREFIX:=/usr/local/

CFLAGS+=-Wall -pedantic -std=c99 -DGRAPHICAL -ffast-math -I$(PREFIX)/include
LDFLAGS=-L$(PREFIX)/lib

#all: hackvr_headless hackvr_x11 hackvr_opengl slowcat ### when hackvr_opengl gets useful at all I'll start including it in default build.
all: hackvr_headless hackvr_x11 slowcat nonblocktail
# hackvr_fb hackvr_freeglut slowcat

nonblocktail: LDLIBS=-lidc
nonblocktail: nonblocktail.c

hackvr_headless: LDLIBS=-lm -lidc -lhashtable
hackvr_headless: hackvr_headless.o math.o physics.o

hackvr_x11: LDLIBS=-lm -lidc -lX11 -lhashtable
hackvr_x11: hackvr_x11.o graphics_c3.o graphics_c2.o graphics_cs_x11.o math.o physics.o keyboard.o mouse_x11.o keyboard_x11.o input.o
#notice how all the targets have generic graphics objects up until a specific one.

hackvr_fb: LDLIBS=-lm -lidc -lhashtable
hackvr_fb: hackvr_fb.o graphics_c3.o graphics_c2.o graphics_cs_fb.o math.o physics.o keyboard_die.o keyboard.o mouse_die.o

hackvr_opengl: LDLIBS=-lm -lidc -lGL -lGLU -lglut -lhashtable
hackvr_opengl: hackvr_opengl.o graphics_c3.o graphics_c2_opengl.o graphics_cs_opengl.o math.o physics.o

hackvr_freeglut: LDLIBS=-lm -lidc -lGL -lGLU -lglut -lhashtable
hackvr_freeglut: hackvr_freeglut.o graphics_c3_freeglut.o math.o physics.o keyboard.o mouse_die.o keyboard_die.o

hackvr_freeglut.o: LDLIBS=-lm -lidc -lGL -lGLU -lglut -lhashtable
hackvr_freeglut.o: CFLAGS+='-DHVR_VERSION="freeglut"'

hackvr_fb.o: CFLAGS+='-DHVR_VERSION="framebuffer"'

graphics_c3_freeglut.o: LDLIBS=-lm -lidc -lGL -lGLU -lglut -lhashtable

hackvr_opengl.o: LDLIBS=-lm -lidc -lGL -lGLU -lglut -lhashtable
hackvr_opengl.o: CFLAGS+='-DHVR_VERSION="opengl"'

hackvr_x11.o: LDLIBS=-lm -lidc -lX11 -lhashtable
hackvr_x11.o: CFLAGS+='-DHVR_VERSION="x11"'

hackvr_headless.o: CFLAGS=-Wall -pedantic -std=c99 -ffast-math -I$(PREFIX)/include
hackvr_headless.o: CFLAGS+='-DHVR_VERSION="headless"'
hackvr_headless.o: LDLIBS=-lm -lidc -lhashtable

graphics_c3.o: LDLIBS=-lm -lidc -lhashtable
graphics_c2.o: LDLIBS=-lm -lidc -lhashtable

physics.o: LDLIBS=-lm -lidc -lhashtable

keyboard.o: LDLIBS=-lm -lidc -lhashtable

input.o: LDLIBS=-lm -lidc -lhashtable

graphics_cs_x11.o: LDLIBS=-lm -lidc -lX11 -lhashtable
mouse_x11.o:LDLIBS=-lX11 -lhashtable
keyboard_x11.o:LDLIBS=-lX11 -lhashtable

graphics_c2_opengl.o: LDLIBS=-lm -lidc -lGL -lGLU -lglut -lhashtable
graphics_cs_opengl.o: LDLIBS=-lm -lidc -lGL -lGLU -lglut -lhashtable
math.o: LDLIBS=-lm -lidc -lhashtable

install: all
	mkdir -p $(PREFIX)/bin
	install hackvr $(PREFIX)/bin/hackvr
	install hackvr_headless $(PREFIX)/bin/hackvr_headless
	install hackvr_x11 $(PREFIX)/bin/hackvr_x11
	install slowcat $(PREFIX)/bin/slowcat
	install nonblocktail $(PREFIX)/bin/nonblocktail
#	install hackvr_fb $(PREFIX)/bin/hackvr_fb
#	install hackvr_freeglut $(PREFIX)/bin/hackvr_freeglut

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

status:
	cmp hackvr $(PREFIX)/bin/hackvr
	cmp hackvr_x11 $(PREFIX)/bin/hackvr_x11
