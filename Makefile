CFLAGS=-Wall -pedantic -std=c99

all: hackvr slowcat

hackvr: LDFLAGS=-lm -lX11

clean:
	rm hackvr
	rm slowcat
