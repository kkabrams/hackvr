### if you want X11: CFLAGS=-Wall -pedantic -std=c99 -DGRAPHICAL
### and add graphics.o to hackvr build target.
### fuck it. I'll fix it later.
CFLAGS=-Wall -pedantic -std=c99 -DGRAPHICAL -ffast-math
#CFLAGS=-std=c99 -DGRAPHICAL -ffast-math

all: hackvr slowcat

#for x11
hackvr: LDLIBS=-lm -lX11
#for opengl
#hackvr: LDLIBS=-lm -lGL -lGLU -lglut

hackvr: hackvr.o graphics.o graphics_x11.o
#hackvr: hackvr.o graphics.o graphics_opengl.o

graphics_opengl.o: LDLIBS=-lm -lGL -lGLU -lglut

graphics_x11.o: LDLIBS=-lm -lX11

hackvr.o: LDLIBS=-lm -lX11

graphics.o: LDLIBS=-lm -lX11

clean:
	rm -f hackvr
	rm -f slowcat
	rm -f *.o
