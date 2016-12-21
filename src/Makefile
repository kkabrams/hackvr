### if you want X11: CFLAGS=-Wall -pedantic -std=c99 -DGRAPHICAL
### and add graphics.o to hackvr build target.
### fuck it. I'll fix it later.
CFLAGS=-Wall -pedantic -std=c99 -DGRAPHICAL

all: hackvr slowcat

hackvr: LDLIBS=-lm -lX11
hackvr: hackvr.o graphics.o

# x11.o: LDLIBS=-lm -lX11

hackvr.o: LDLIBS=-lm -lX11

graphics.o: LDLIBS=-lm -lX11

clean:
	rm hackvr
	rm slowcat
	rm *.o
