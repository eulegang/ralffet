#pragma once
#include "cmd.h"
#include <stdbool.h>

typedef struct {
  Command *check;
  Command *prompt;
  int retries;
  bool verbose;
} Args;

Args parse_args(int argc, char **argv);
