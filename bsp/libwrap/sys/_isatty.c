#include "weak_under_alias.h"
#include <unistd.h>

int __wrap_isatty(int fd) {
  if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
    return 1;

  return 0;
}
weak_under_alias(isatty);
