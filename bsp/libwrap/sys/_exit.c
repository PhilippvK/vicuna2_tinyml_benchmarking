/* See LICENSE of license details. */

#include "weak_under_alias.h"
#include <stdint.h>
#include <unistd.h>
#include "terminate_benchmark.h"

void __wrap_exit(int code) {
    sim_exit(code);
    for(;;)
        ;
}
weak_under_alias(exit);
