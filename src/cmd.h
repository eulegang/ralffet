#pragma once

#include <stdbool.h>

typedef struct Command Command;

Command *gen_command();
bool command_push_arg(Command *self, char *arg);
bool command_push_shell(Command *self, char *shell);
void command_set_stdin(Command *self, char *stdin);

int command_run(Command *cmd);
char *command_show(Command *cmd);
