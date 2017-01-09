CFLAGS=-Wall -pedantic -std=c99 -DGRAPHICAL -ffast-math

all: hackvr_headless hackvr_x11 hackvr_opengl slowcat

hackvr_headless: LDLIBS=-lm
hackvr_headless: hackvr_headless.o math.o

hackvr_x11: LDLIBS=-lm -lX11
hackvr_x11: hackvr_x11.o graphics.o graphics_x11.o math.o

hackvr_opengl: LDLIBS=-lm -lGL -lGLU -lglut
hackvr_opengl: hackvr_opengl.o graphics.o graphics_opengl.o math.o

hackvr_opengl.o: LDLIBS=-lm -lGL -lGLU -lglut
hackvr_x11.o: LDLIBS=-lm -lX11

hackvr_headless.o: CFLAGS=-Wall -pedantic -std=c99 -ffast-math
hackvr_headless.o: LDLIBS=-lm

graphics.o: LDLIBS=-lm
graphics_x11.o: LDLIBS=-lm -lX11
graphics_opengl.o: LDLIBS=-lm -lGL -lGLU -lglut
math.o: LDLIBS=-lm

clean:
	rm -f hackvr
	rm -f hackvr_headless
	rm -f hackvr_opengl
	rm -f hackvr_x11
	rm -f slowcat
	rm -f *.o
