CC = gcc
CFLAGS = -Wall -Wcast-align -Wcast-qual -Wimplicit -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow
ifndef PREFIX
PREFIX = /usr/local
endif

include VERSION

EXTRA_CFLAGS = '-DCRC32TRIM_UTILS_VERSION="$(CRC32TRIM_UTILS_VERSION)"'

all: crc32combine crc32trim-leading crc32trim-trailing

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

crc32combine: crc32combine.o
	$(CC) $(CFLAGS) -o $@ $^ -lz

crc32trim-leading: crc32trim_leading.o
	$(CC) $(CFLAGS) -o $@ $^ -lz -lcrc32trim

crc32trim-trailing: crc32trim_trailing.o
	$(CC) $(CFLAGS) -o $@ $^ -lz -lcrc32trim

.PHONY: clean
clean:
	rm -f crc32combine crc32combine.o crc32trim-leading crc32trim_leading.o crc32trim-trailing crc32trim_trailing.o

.PHONY: install
install: crc32combine crc32trim-leading crc32trim-trailing
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install $^ $(DESTDIR)$(PREFIX)/bin/

.PHONY: uninstall
uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/crc32combine
	rm $(DESTDIR)$(PREFIX)/bin/crc32trim-leading
	rm $(DESTDIR)$(PREFIX)/bin/crc32trim-trailing
