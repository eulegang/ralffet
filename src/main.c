#include "args.h"
#include "cmd.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  Args args = parse_args(argc, argv);
  bool verbose = args.verbose;

  trace("check: %s\n", command_show(args.check));
  trace("prompt: %s\n", command_show(args.prompt));

  int attempt = 0;
  int status = 0;

  while (attempt < args.retries) {
    trace("attempt: %d\n", attempt);

    if (attempt) {
      trace("ralphing again!\n");
    }

    trace("running \"%s\" ...\n", command_show(args.check));
    status = command_run(args.check);
    trace("\"%s\" resulted in %d\n", command_show(args.check), status);

    if (!status) {
      break;
    }

    trace("running \"%s\" ...\n", command_show(args.prompt));
    status = command_run(args.prompt);

    if (status)
      die("failed to prompt\n");

    attempt++;
  }

  if (attempt == args.retries)
    exit(2);
}
