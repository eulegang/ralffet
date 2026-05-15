#pragma once

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
