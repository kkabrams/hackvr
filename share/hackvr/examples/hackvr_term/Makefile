.PHONY: all clean install uninstall

all: hackvr_term

hackvr_term.o: CFLAGS=-pedantic -Wall
libtmt/tmt.o: CFLAGS=-pedantic

hackvr_term: hackvr_term.o libtmt/tmt.o

clean:
	rm *.o
	rm */*.o
	rm hackvr_term

install:
	install hackvr_term $(PREFIX)/bin/hackvr_term

uninstall:
	[ -e $(PREFIX)/bin/hackvr_term ] && rm -i $(PREFIX)/bin/hackvr_term || echo "nothing to uninstall"
