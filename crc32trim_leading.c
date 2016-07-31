/* crc32trim_leading.c

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

    sscanf(argv[1], "%lx", &crcAB);
    sscanf(argv[2], "%lx", &crcA);
    sscanf(argv[3],  "%d", &lenB);

    crcB = crc32_combine(crcA, crcAB, lenB);

    printf("%lx\n", crcB);

    return 0;
}
