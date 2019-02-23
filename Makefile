CC = gcc
CFLAGS = -Wall -Wcast-align -Wcast-qual -Wimplicit -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow
PREFIX = /usr/local

include VERSION

EXTRA_CFLAGS = '-DCRC32TRIM_UTILS_VERSION="$(CRC32TRIM_UTILS_VERSION)"'

all: crc32combine crc32trim-leading crc32trim-trailing

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

crc32combine: crc32combine.o
	$(CC) $(CFLAGS) -o $@ $^ -lz

crc32trim-leading:
	gcc -Wall -o crc32trim-leading crc32trim_leading.c -lz

crc32trim-trailing:
	gcc -Wall -o crc32trim-trailing crc32trim_trailing.c -lcrc32trim

.PHONY: clean
clean:
	rm -f crc32combine crc32combine.o crc32trim-leading crc32trim-trailing
