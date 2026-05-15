#include <ctype.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

size_t imprint_embedded(char *buf, char *content, size_t cap) {
  char *it = content;
  size_t len = 0;

  while (*it++ == '#') {
    if (*it == '!') {
      while (*it != '\n') {
        it++;
      }
      it++;
    } else {
      while (isspace(*it)) {
        it++;
      }

      char *hare = it;

      while (*hare != '\n')
        hare++;
      hare++;

      size_t sub = hare - it;

      len += sub;
      memcpy(buf, it, sub);
      buf += sub;
    }
  }

  return len;
}

char *extract_embed(char *file) {
  int fd = open(file, O_CLOEXEC | O_RDONLY);

  if (fd == -1)
    return NULL;

  off_t filelen = lseek(fd, 0, SEEK_END);
  size_t cap = 4 * getpagesize();

  char *buf =
      mmap(0, cap, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0);

  if (buf == MAP_FAILED) {
    close(fd);
    return NULL;
  }

  char *content = mmap(0, filelen, PROT_READ, MAP_PRIVATE, fd, 0);

  close(fd);
  if (content == MAP_FAILED) {
    return NULL;
  }

  size_t len = 0;
  if ((len = imprint_embedded(buf, content, cap)) > cap) {
    return NULL;
  }

  return buf;
}
