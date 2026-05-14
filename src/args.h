#pragma once
#include <stdbool.h>

#define die(fmt, ...)                                                          \
  do {                                                                         \
    fprintf(stderr, fmt, ##__VA_ARGS__);                                       \
    exit(1);                                                                   \
  } while (0)

#define trace(fmt, ...)                                                        \
  do {                                                                         \
    if (verbose)                                                               \
      fprintf(stdout, fmt, ##__VA_ARGS__);                                     \
  } while (0)

extern char *prompt;
extern char *check;
extern int retries;
extern bool verbose;

void parse_args(int argc, char **argv);
void clean_args();
