#include "weak_under_alias.h"
#include <stdint.h>
#include <unistd.h>
#include "uart.h"

extern ssize_t _bsp_write(int, const void *, size_t);
ssize_t __wrap_write(int fd, const void *ptr, size_t len) {
  // TODO: check fd is stdout or stderr!
  uart_write(len, ptr);
  return len;
}
weak_under_alias(write);
