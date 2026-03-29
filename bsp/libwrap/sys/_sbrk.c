#include <stddef.h>
#include "weak_under_alias.h"

void *__wrap_sbrk(ptrdiff_t incr)
{
  extern char _end[];
  extern char _heap_end[];
  static char *curbrk = _end;

  if ((curbrk + incr < _end) || (curbrk + incr > _heap_end))
    return (void*)- 1;

  curbrk += incr;
  return curbrk - incr;
}

weak_under_alias(sbrk);
