#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wordexp.h>

#include <sys/mman.h>
#include <sys/wait.h>

#define CAP 16 * getpagesize()

size_t command_resolve(const char *name, char *buf);

struct Command {
  char *stdin;
  size_t len;
  char *ptr[32];
};

Command *gen_command() {
  Command *cmd =
      mmap(0, CAP, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0);

  if (cmd == MAP_FAILED)
    return NULL;

  cmd->stdin = NULL;
  cmd->len = 0;

  return cmd;
}

void command_set_stdin(Command *self, char *stdin) { self->stdin = stdin; }

int command_run(Command *self) {

  int pid = fork();

  if (pid == 0) {
    int res = execvp(self->ptr[0], self->ptr);

    if (res != 0) {
      abort();
    }

    exit(0);
  } else {
    int status;
    printf("pid: %d\n", pid);
    int res = waitpid(pid, &status, 0);
    printf("stauts: %d\n", status);

    if (res == -1)
      perror("waiting");
    return WEXITSTATUS(status);
  }
}

char *command_show(Command *self) {
  if (self->len >= sizeof(self->ptr) / sizeof(char *))
    return false;

  char *cur = self->ptr[self->len - 1];

  size_t lastlen = strlen(cur);
  char *rest = cur + lastlen + 1;

  if (*rest != 0)
    return rest;

  char *base = (char *)(self + 1);
  size_t len = rest - base;

  memcpy(rest, base, len);

  for (size_t i = 0; i + 1 < len; i++) {
    if (rest[i] == 0) {
      rest[i] = ' ';
    }
  }

  return rest;
}

bool command_push_arg(Command *self, char *arg) {
  char *base = (char *)(self + 1);
  size_t len = 0;

  if (!self->len) {
    if ((len = strlcpy(base, arg, CAP - sizeof(Command))) >
        CAP - sizeof(Command)) {
      return false;
    }

    self->ptr[self->len++] = base;
  } else {
    if (self->len >= sizeof(self->ptr) / sizeof(char *))
      return false;

    char *cur = self->ptr[self->len - 1];

    size_t lastlen = strlen(cur);
    char *rest = cur + lastlen + 1;
    size_t buflen = CAP - (rest - (char *)self);

    if ((len = strlcpy(rest, arg, buflen)) > buflen) {
      return false;
    }

    self->ptr[self->len++] = rest;
  }

  return true;
}

size_t command_resolve(const char *name, char *buf) {
  if (!name)
    return 0;

  size_t cap = CAP - sizeof(Command);

  switch (*name) {
  case '/':
  case '.':
    return strlcpy(buf, name, cap);
  }

  char *path = getenv("PATH");
  char *dir = strtok(path, ":");
  size_t len = 0;
  bool done = false;

  while (dir != NULL && !done) {
    *buf = 0;
    if ((len = strlcpy(buf, dir, cap)) > cap) {
      dir = strtok(NULL, ":");
      continue;
    };

    if (len != 0 && buf[len - 1] != '/') {
      buf[len++] = '/';
    }

    len += strlcpy(buf + len, name, cap - len);

    if (len > cap) {
      dir = strtok(NULL, ":");
      continue;
    }

    if (access(buf, X_OK) != 0) {
      memset(buf, 0, len);
      dir = strtok(NULL, ":");
      continue;
    }

    done = true;

    break;
  }

  if (!done) {
    return 0;
  } else {
    return len;
  }
}

bool command_push_shell(Command *self, char *shell) {
  if (!shell)
    return true;

  wordexp_t word;
  size_t len = 0;

  if ((len = wordexp(shell, &word, 0)) != 0)
    return false;

  for (size_t i = 0; i < word.we_wordc; i++) {
    command_push_arg(self, word.we_wordv[i]);
  }

  return true;
}
