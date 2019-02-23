/* crc32trim_leading.c
 *
 * Copyright (c) 2016-present Matt Cover
 * All rights reserved.
 *
 * This source code is licensed under the zlib license found in the
 * LICENSE file in the root directory of this source tree.
 */

#define _GNU_SOURCE // for GNU basename()

#include <crc32trim.h> // for crc32_trim_leading()
#include <getopt.h>    // for getopt_long(), optind, option, optopt
#include <stdbool.h>   // for bool, false, true
#include <stdio.h>     // for fprintf(), NULL, printf(), puts(), stderr,
                       //     sprintf()
#include <stdlib.h>    // for free(), malloc(), strtol(), strtoul()
#include <string.h>    // for GNU basename()
#include <zlib.h>      // for crc32(), crc32combine(), Z_NULL

#define EXIT_OK          0
#define EXIT_FAIL        1
#define EXIT_FAIL_OPTION 2

#define BASE_SIXTEEN 16
#define BASE_TEN     10

#define MAX_SHORT_BADOPT_LENGTH 2

static const struct option long_options[] = {
  {"help",          no_argument, NULL, 'h'},
  {"split-lengths", no_argument, NULL, 's'},
  {"version",       no_argument, NULL, 'V'},
  {0, 0, NULL, 0}
};

char *program_basename;

static void usage(void) {
  puts("Usage:");
  printf(
    "  %s [OPTIONS] <crcAB> [<crcA lengthB> ...]\n\n",
    program_basename
  );

  puts(
    "Options:\n"
    "  -h, [--help]                 # Display this help and exit.\n"
    "  -s, [--split-lengths]        # When trimming recursively, the default length\n"
    "                               #   syntax can be a bit awkward (e.g. crcABC\n"
    "                               #   crcA lenBC crcB lenC), but is proper. This\n"
    "                               #   option provides an alternative which may be\n"
    "                               #   more friendly in some cases (e.g. crcABC\n"
    "                               #   crcA lenB crcB lenC).\n"
    "  -V, [--version]              # Display the version and exit.\n"
  );
}

static void usage_short(void) {
  fprintf(
    stderr,
    "Usage: %s [--help] [--version] <crcAB> [<crcA> <lengthB> ...]\n",
    program_basename
  );
}

int main(int argc, char **argv) {
  program_basename = basename(argv[0]);

  int c;
  char *badopt = NULL;
  bool split_lengths = false;

  /*
   * optstring must start with ":" so ':' is returned for a missing option
   * argument. Otherwise '?' is returned for both invalid option and missing
   * option argument.
   */
  while ((c = getopt_long(argc, argv, ":hsV", long_options, 0)) != -1) {
    switch (c) {
      case 'h':
        usage();
        return EXIT_OK;

      case 's':
        split_lengths = true;
        break;

      case 'V':
        printf("%s version %s\n", program_basename, CRC32TRIM_UTILS_VERSION);
        return EXIT_OK;

      case ':':  /* missing option argument */
        fprintf(
          stderr,
          "%s: Option '%s' requires an argument.\n",
          program_basename,
          argv[optind-1]
        );
        usage_short();
        return EXIT_FAIL_OPTION;

      case '?':  /* invalid option */
      default:   /* invalid option in optstring */
        /*
         * optopt is NULL for long options. optind is the index of the next
         * argv to be processed. It is therefore tempting to use argv[optind-1]
         * to retrieve the invalid option, be it short or long.
         *
         * This method works with long options and with non-bundled short
         * options. However, it fails under certain conditions with bundled
         * short options.
         *
         * When the first invalid option in the bundle is also the last option
         * in the bundle, argv[optind-1] works.
         *
         * When the first invalid option in the bundle is not also the last
         * option in the bundle, optind is not incremented. This makes sense
         * since the bundle argv still needs further processing.
         *
         * Relying solely on optind leaves us with two difficult to distinguish
         * possibilities.
         *   1. The last or only short option in argv[optind-1] was invalid and
         *      optind was incremented.
         *   2. Any short option other than the last in argv[optind] was
         *      invalid and optind was not incremented.
         *
         * As previously mentioned, optopt is NULL for long options. It is also
         * reliable for finding the invalid short option with one caveat; we
         * need to keep our optstring clean. If we arrive here via the default
         * case optopt will be NULL and we'll have a corner case for the
         * invalid short option mistakenly in optstring.
         *
         * As long as we keep optstring clean, we can use optopt for short
         * options and argv[optind-1] for long options.
         */
        if (optopt) {
          badopt = malloc(MAX_SHORT_BADOPT_LENGTH + 1);
          sprintf(badopt, "-%c", optopt);
        } else {
          badopt = argv[optind-1];
        }
        fprintf(
          stderr,
          "%s: Unrecognized option '%s'.\n",
          program_basename,
          badopt
        );
        if (optopt) {
          free(badopt);
        }
        usage_short();
        return EXIT_FAIL_OPTION;
    }
  }

  char *endptr = NULL;
  unsigned long crcAB = crc32(0L, Z_NULL, 0);
  unsigned long crcA = crc32(0L, Z_NULL, 0);
  long lenB = 0L;

  if ((argc - optind) % 2 == 0) {
    fprintf(
      stderr,
      "%s: Invalid number of arguments supplied.\n",
      program_basename
    );
    usage_short();
    return EXIT_FAIL_OPTION;
  }

  crcAB = strtoul(argv[optind], &endptr, BASE_SIXTEEN);
  if (*endptr) {
    fprintf(
      stderr,
      "%s: Invalid crc32 '%s'.\n",
      program_basename,
      argv[optind]
    );
    usage_short();
    return EXIT_FAIL_OPTION;
  }
  optind++;

  if (split_lengths) {
    /*
     * Combine all to-be-trimmed leading crcs into crcA.
     */
    crcA = strtoul(argv[optind], &endptr, BASE_SIXTEEN);
    if (*endptr) {
      fprintf(
        stderr,
        "%s: Invalid crc32 '%s'.\n",
        program_basename,
        argv[optind]
      );
      usage_short();
      return EXIT_FAIL_OPTION;
    }
    optind++;

    /*
     * This is just our crcB for the combine operations, not the final crcB.
     */
    unsigned long crcB = crc32(0L, Z_NULL, 0);

    for (; optind + 1 < argc; optind++) {
      /*
       * Associated lengths are first.
       */
      lenB = strtol(argv[optind], &endptr, BASE_TEN);
      if (*endptr) {
        fprintf(
          stderr,
          "%s: Invalid length '%s'.\n",
          program_basename,
          argv[optind]
        );
        usage_short();
        return EXIT_FAIL_OPTION;
      }
      optind++;

      crcB = strtoul(argv[optind], &endptr, BASE_SIXTEEN);
      if (*endptr) {
        fprintf(
          stderr,
          "%s: Invalid crc32 '%s'.\n",
          program_basename,
          argv[optind]
        );
        usage_short();
        return EXIT_FAIL_OPTION;
      }

      /*
       * crcA ends up holding crcAB for the combine operations.
       */
      crcA = crc32_combine(crcA, crcB, lenB);
    }

    /*
     * We have one final length to collect.
     */
    lenB = strtol(argv[optind], &endptr, BASE_TEN);
    if (*endptr) {
      fprintf(
        stderr,
        "%s: Invalid length '%s'.\n",
        program_basename,
        argv[optind]
      );
      usage_short();
      return EXIT_FAIL_OPTION;
    }

    /*
     * Now we can do our trim leading.
     */
    crcAB = crc32_trim_leading(crcAB, crcA, lenB);
  } else {
    for (; optind < argc; optind++) {
      crcA = strtoul(argv[optind], &endptr, BASE_SIXTEEN);
      if (*endptr) {
        fprintf(
          stderr,
          "%s: Invalid crc32 '%s'.\n",
          program_basename,
          argv[optind]
        );
        usage_short();
        return EXIT_FAIL_OPTION;
      }
      optind++;

      lenB = strtol(argv[optind], &endptr, BASE_TEN);
      if (*endptr) {
        fprintf(
          stderr,
          "%s: Invalid length '%s'.\n",
          program_basename,
          argv[optind]
        );
        usage_short();
        return EXIT_FAIL_OPTION;
      }

      crcAB = crc32_trim_leading(crcAB, crcA, lenB);
    }
  }

  /*
   * crcAB holds crcB in the end.
   */
  printf("%08lx\n", crcAB);

  return EXIT_OK;
}
