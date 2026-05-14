#include "args.h"
#include <getopt.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>

char *prompt = NULL;
char *check = NULL;
int retries = 3;
bool verbose = false;

char **shellwords(char *);

#define CLEAN_CHECK 1
static int clean_flags = 0;

void parse_args(int argc, char **argv) {
  static struct option long_options[] = {
      {"verbose", no_argument, 0, 'v'},
      {
          "help",
          no_argument,
          0,
          'h',
      },
      {"prompt", required_argument, 0, 'p'},
      {"retries", required_argument, 0, 'r'},
      {"check", required_argument, 0, 'c'},
      {"interp", required_argument, 0, 'I'},
      {0, 0, 0, 0},
  };

  int check_flags = 0;

  int c;
  while ((c = getopt_long(argc, argv, "vhp:r:c:I:", long_options, NULL)) !=
         -1) {
    switch (c) {
    case 'v':
      verbose = true;
      break;

    case 'h':
      printf("ralffet -p <prompt>\n");
      exit(0);

    case 'p': {
      if (prompt != NULL)
        die("prompt specified twice\n");

      prompt = optarg;
    } break;

    case 'c': {
      if (check_flags)
        die("check and interp may not both be used\n");

      if (check != NULL)
        die("check specified twice\n");

      check = optarg;
      check_flags |= 1;

    } break;

    case 'I': {
      if (check_flags)
        die("check and interp may not both be used\n");

      if (check != NULL)
        die("interp specified twice\n");

      check = optarg;
      check_flags |= 2;

    } break;

    case 'r': {
      if ((retries = atoi(optarg)) == 0 && !strcmp(optarg, "0")) {
        fprintf(stderr, "invalid retries parameter\n");
        exit(1);
      }
    } break;
    }
  }

  if (check_flags & 2) {
    if (optind < argc) {
      char *buf = malloc(4096);
      clean_flags |= CLEAN_CHECK;

      strlcat(buf, check, 4096);
      strlcat(buf, " ", 4096);
      strlcat(buf, argv[optind++], 4096);
      check = buf;
    } else {
      fprintf(stderr, "interp used without file\n");
      exit(1);
    }
  }

  if (optind < argc)
    die("extra arguments provided\n");

  if (!check)
    die("check program not specififed\n");

  if (!prompt)
    die("prompt invocation not specififed\n");
}

void clean_args() {
  if (clean_flags & CLEAN_CHECK)
    free(check);
}
