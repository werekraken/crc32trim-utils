/* crc32trim_trailing.c

  Copyright (C) 2016 Matt Cover

  For conditions of distribution and use, see LICENSE.
*/

#include <stdio.h>
#include "crc32trim.h"

int main(
    int argc,
    char *argv[]
) {
    unsigned long crcA;
    unsigned long crcAB;
    unsigned long crcB;
    int lenB;

    sscanf(argv[1], "%lx", &crcAB);
    sscanf(argv[2], "%lx", &crcB);
    sscanf(argv[3],  "%d", &lenB);

    crcA = crc32_trim_trailing(crcAB, crcB, lenB);

    printf("%lx\n", crcA);

    return 0;
}
