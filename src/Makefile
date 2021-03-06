.PHONY: all install clean uninstall status

PREFIX:=/usr/local/

### if you didn't install them after building, you can still point at them directly.
### right now, the header, and shared library are in their base dir, not lib or include subdirs.
LIBIDC_LIB_PATH:=$(PREFIX)/lib
LIBIDC_INCLUDE_PATH:=$(PREFIX)/include
LIBHASHTABLE_LIB_PATH:=$(PREFIX)/lib
LIBHASHTABLE_INCLUDE_PATH:=$(PREFIX)/include

BASE_CFLAGS=-g -Wall -pedantic -std=c99 -ffast-math -I$(PREFIX)/include -I$(LIBIDC_INCLUDE_PATH) -I$(LIBHASHTABLE_INCLUDE_PATH)

override CFLAGS+=$(BASE_CFLAGS)
override CFLAGS+=-DGRAPHICAL

override LDFLAGS+=-L$(PREFIX)/lib -L$(LIBIDC_LIB_PATH) -L$(LIBHASHTABLE_LIB_PATH)

#all: hackvr_headless hackvr_x11 hackvr_opengl slowcat ### when hackvr_opengl gets useful at all I'll start including it in default build.
all: hackvr_svg hackvr_fb hackvr_xdie hackvr_headless hackvr_x11 slowcat nonblocktail hackvr_xdiewii
# hackvr_fb hackvr_freeglut slowcat

hackvr_xdiewii: override LDLIBS+=-lm -lidc -lX11 -lhashtable
hackvr_xdiewii: hackvr_xdiewii.o graphics_c3.o graphics_c2.o graphics_cs_x11.o math.o physics.o keyboard.o mouse_wii.o keyboard_x11.o

hackvr_xdie: override LDLIBS+=-lm -lidc -lX11 -lhashtable
hackvr_xdie: hackvr_xdie.o graphics_c3.o graphics_c2.o graphics_cs_x11.o math.o physics.o keyboard.o mouse_die.o keyboard_die.o

nonblocktail: override LDLIBS+=-lidc
nonblocktail: nonblocktail.c

hackvr_headless: override LDLIBS+=-lm -lidc -lhashtable
hackvr_headless: hackvr_headless.o math.o physics.o

hackvr_x11: override LDLIBS+=-lm -lidc -lX11 -lhashtable
hackvr_x11: hackvr_x11.o graphics_c3.o graphics_c2.o graphics_cs_x11.o math.o physics.o keyboard.o mouse_x11.o keyboard_x11.o

hackvr_svg: override LDLIBS+=-lm -lidc -lhashtable
hackvr_svg: hackvr_svg.o graphics_c3.o graphics_c2.o graphics_cs_svg.o math.o physics.o keyboard.o keyboard_die.o mouse_die.o

#notice how all the targets have generic graphics objects up until a specific one.

hackvr_fb: override LDLIBS+=-lm -lidc -lhashtable
hackvr_fb: hackvr_fb.o graphics_c3.o graphics_c2.o graphics_cs_fb.o math.o physics.o keyboard_die.o keyboard.o mouse_die.o

hackvr_opengl: override LDLIBS+=-lm -lidc -lGL -lGLU -lglut -lhashtable
hackvr_opengl: hackvr_opengl.o graphics_c3.o graphics_c2_opengl.o graphics_cs_opengl.o math.o physics.o

hackvr_freeglut: override LDLIBS+=-lm -lidc -lGL -lGLU -lglut -lhashtable
hackvr_freeglut: hackvr_freeglut.o graphics_c3_freeglut.o math.o physics.o keyboard.o mouse_die.o keyboard_die.o

hackvr_fb.o: override CFLAGS+='-DHVR_VERSION="framebuffer"'
hackvr_x11.o: override CFLAGS+='-DHVR_VERSION="x11"'
hackvr_xdie.o: override CFLAGS+='-DHVR_VERSION="x11+die"'
hackvr_xdiewii.o: override CFLAGS+='-DHVR_VERSION="x11+kdie+wii"'
hackvr_svg.o: override CFLAGS+='-DHVR_VERSION="svg"'
hackvr_headless.o: override CFLAGS=$(BASE_CFLAGS)
hackvr_headless.o: override CFLAGS+='-DHVR_VERSION="headless"'

install: all
	mkdir -p $(PREFIX)/bin
	install -t $(PREFIX)/bin hackvr_headless
	install -t $(PREFIX)/bin hackvr_x11
	install -t $(PREFIX)/bin slowcat
	install -t $(PREFIX)/bin nonblocktail
	install -t $(PREFIX)/bin hackvr_xdie
	install -t $(PREFIX)/bin hackvr_fb
	install -t $(PREFIX)/bin hackvr_svg
	install -t $(PREFIX)/bin hackvr_xdiewii
#	install -t $(PREFIX)/bin hackvr_freeglut

uninstall:
	rm $(PREFIX)/bin/hackvr
	rm $(PREFIX)/bin/hackvr_headless
	rm $(PREFIX)/bin/hackvr_x11
	rm $(PREFIX)/bin/slowcat
	rm $(PREFIX)/bin/nonblocktail
	rm $(PREFIX)/bin/hackvr_xdie
	rm $(PREFIX)/bin/hackvr_fb
#	rm $(PREFIX)/bin/hackvr_freeglut

clean:
#	do *not* rm hackvr. it is a script which doesn't actually belong in this dir so...
	rm -f hackvr_headless
	rm -f hackvr_x11
	rm -f slowcat
	rm -f nonblocktail
	rm -f hackvr_xdie
#	rm -f hackvr_opengl
	rm -f hackvr_fb
#	rm -f hackvr_freeglut
	rm -f *.o

status:
	cmp hackvr $(PREFIX)/bin/hackvr
	cmp hackvr_x11 $(PREFIX)/bin/hackvr_x11
