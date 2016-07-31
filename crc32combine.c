/* crc32combine.c

  Copyright (C) 2016 Matt Cover

  For conditions of distribution and use, see LICENSE.
*/

#include <stdio.h>
#include "zlib.h"

int main(
    int argc,
    char *argv[]
) {
    unsigned long crcA;
    unsigned long crcAB;
    unsigned long crcB;
    int lenB;

    sscanf(argv[1], "%lx", &crcA);
    sscanf(argv[2], "%lx", &crcB);
    sscanf(argv[3],  "%d", &lenB);

    crcAB = crc32_combine(crcA, crcB, lenB);

    printf("%lx\n", crcAB);

    return 0;
}
