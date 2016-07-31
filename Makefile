all: crc32combine crc32trim-leading crc32trim-trailing

crc32combine:
	gcc -Wall -o crc32combine crc32combine.c -lz

crc32trim-leading:
	gcc -Wall -o crc32trim-leading crc32trim_leading.c -lz

crc32trim-trailing:
	gcc -Wall -o crc32trim-trailing crc32trim_trailing.c -lcrc32trim

clean:
	rm -f crc32combine crc32trim-leading crc32trim-trailing
