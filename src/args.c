#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <linux/limits.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>

#include <sys/mman.h>
#include <unistd.h>

#include "args.h"
#include "cmd.h"
#include "embedded.h"
#include "util.h"

#define PROMPT_ARG 1
#define CHECK_ARG 2
#define INTERP_ARG 4
#define EMBEDDED_PROMPT_ARG 8
#define FILE_ARG 16

void embedded_arg_check(Command **cmd, char *interp, char *file);
void embedded_arg_prompt(Command **cmd, char *harness, char *file);
void arg_prompt(Command **cmd, char *harness, char *prompt);
void arg_check(Command **cmd, char *interp, char *expr);

Args parse_args(int argc, char **argv) {
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

  int flags = 0;

  char *_prompt = NULL;
  char *_check = NULL;
  char *interp = NULL;
  bool verbose = false;
  int retries = 3;
  char *harness = getenv("RALFFET_HARNESS");

  int c;
  while ((c = getopt_long(argc, argv, "vhp:r:c:I:P", long_options, NULL)) !=
         -1) {
    switch (c) {
    case 'v':
      verbose = true;
      break;

    case 'h':
      printf("ralffet -p <prompt>\n");
      exit(0);

    case 'p': {
      if (flags & PROMPT_ARG)
        die("prompt specified twice\n");

      _prompt = optarg;
      flags |= PROMPT_ARG;
    } break;

    case 'c': {
      if (flags & CHECK_ARG)
        die("check specified twice\n");

      _check = optarg;
      flags |= CHECK_ARG;
    } break;

    case 'P':
      if (flags & EMBEDDED_PROMPT_ARG)
        die("check already specified\n");

      flags |= EMBEDDED_PROMPT_ARG;
      break;

    case 'I': {
      if (flags & INTERP_ARG)
        die("check and interp may not both be used\n");

      interp = optarg;
      flags |= INTERP_ARG;

    } break;

    case 'r': {
      if ((retries = atoi(optarg)) == 0 && !strcmp(optarg, "0")) {
        fprintf(stderr, "invalid retries parameter\n");
        exit(1);
      }
    } break;
    }
  }

  char *file;
  if (optind < argc) {
    file = argv[optind++];
    flags |= FILE_ARG;
  }

  if (optind < argc)
    die("extra arguments provided\n");

  if (!harness)
    die("ai harness not defined\n");

  if (flags & INTERP_ARG && flags & CHECK_ARG) {
    die("can not have intermpreter and check\n");
  }

  if (flags & PROMPT_ARG && flags & EMBEDDED_PROMPT_ARG) {
    die("a prompt can not be specified and embedded\n");
  }

  if (flags & FILE_ARG && flags & CHECK_ARG) {
    die("can not use check expression and check file");
  }

  if (flags & EMBEDDED_PROMPT_ARG && !(flags & FILE_ARG)) {
    die("A embed prompt needs a file\n");
  }

  Command *prompt = NULL;
  Command *check = NULL;

  if (!interp) {
    interp = getenv("SHELL");

    if (!interp) {
      interp = "/bin/bash";
    }
  }

  if (flags & FILE_ARG) {
    embedded_arg_check(&check, interp, file);
    if (flags & EMBEDDED_PROMPT_ARG) {
      embedded_arg_prompt(&prompt, harness, file);
    } else {
      arg_prompt(&prompt, harness, _prompt);
    }
  } else {

    arg_prompt(&prompt, harness, _prompt);
  }

  if (!check)
    die("check program not specififed\n");

  if (!prompt)
    die("prompt invocation not specififed\n");

  return (Args){
      .verbose = verbose,
      .retries = retries,
      .check = check,
      .prompt = prompt,
  };
}
void embedded_arg_check(Command **cmd, char *interp, char *file) {
  Command *_cmd = gen_command();

  command_push_shell(_cmd, interp);

  if (!command_push_arg(_cmd, file))
    return;

  *cmd = _cmd;
}

void embedded_arg_prompt(Command **cmd, char *harness, char *file) {
  Command *_cmd = gen_command();

  if (!command_push_shell(_cmd, harness))
    return;

  if (!command_push_arg(_cmd, extract_embed(file)))
    return;

  *cmd = _cmd;
}

void arg_prompt(Command **cmd, char *harness, char *prompt) {
  Command *_cmd = gen_command();

  if (!command_push_shell(_cmd, harness))
    return;

  if (!command_push_arg(_cmd, prompt))
    return;

  *cmd = _cmd;
}

void arg_check(Command **cmd, char *interp, char *expr) {
  Command *_cmd = gen_command();

  if (!command_push_shell(_cmd, interp))
    return;

  if (!command_push_arg(_cmd, "-c"))
    return;

  if (!command_push_arg(_cmd, expr))
    return;

  *cmd = _cmd;
}
