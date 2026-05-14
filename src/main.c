#include "args.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  parse_args(argc, argv);

  int attempt = 0;
  int status = 0;

  while (attempt < retries) {
    trace("attempt: %d\n", attempt);

    if (attempt) {
      trace("ralphing again!");
    }

    trace("running \"%s\" ...\n", check);
    status = system(check);
    trace("\"%s\" resulted in %d\n", check, status);

    if (!status) {
      break;
    }

    status = system(prompt);

    if (status)
      die("failed to prompt");

    attempt++;
  }

  clean_args();
}
