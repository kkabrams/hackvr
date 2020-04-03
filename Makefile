PREFIX:=/usr/local

all: hackvr

.PHONY: hackvr clean install uninstall all

hackvr:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

install: hackvr
	$(MAKE) -C src install
	install -t $(PREFIX)/bin bin/*
	install -t $(PREFIX)/share/applications share/applications/*
	cp -r share/hackvr $(PREFIX)/share/hackvr

uninstall:
	## we'll assume PREFIX is set to the hackvr we want to uninstall
	$(MAKE) -C src uninstall
	rm -f $(PREFIX)/share/applications/hackvr
	rm -rf $(PREFIX)/share/hackvr
