all: hackvr hackvr_term

.PHONY: hackvr hackvr_term clean install uninstall all

ifeq ($(PREFIX),)
$(error the PREFIX variable is NOT set)
endif

hackvr:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

install: hackvr hackvr_term
	$(MAKE) -C src install
	cp -r bin/ $(PREFIX)/
	cp -r share/hackvr/ $(PREFIX)/share/

uninstall:
	## we'll assume PREFIX is set to the hackvr we want to uninstall
	$(MAKE) -C src uninstall
	rm -rf $(PREFIX)/share/hackvr
