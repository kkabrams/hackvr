all: hackvr hackvr_term

.PHONY: hackvr hackvr_term clean install uninstall all

hackvr:
	$(MAKE) -C src all

hackvr_term:
	$(MAKE) -C hackvr_term all

clean:
	$(MAKE) -C src clean
	$(MAKE) -C hackvr_term clean

install:
	$(MAKE) -C src install
	$(MAKE) -C hackvr_term install
	install -D font/font.hackvr $(PREFIX)/share/hackvr/font.hackvr

uninstall:
	$(MAKE) -C src uninstall
	$(MAKE) -C hackvr_term uninstall
