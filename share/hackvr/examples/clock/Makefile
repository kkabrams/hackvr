PREFIX:=/usr/local

.PHONY: all install

hackvr-clock: run.pre generate.py
	./generate.py | cat run.pre /dev/stdin > hackvr-clock
	chmod +x hackvr-clock
all: hackvr-clock

install: all
	install -t $(PREFIX)/bin hackvr-clock
