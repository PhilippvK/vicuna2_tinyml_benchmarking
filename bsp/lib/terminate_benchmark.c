#include "terminate_benchmark.h"

#define TOHOST_ADDR 0x500

static volatile unsigned int *tohost = (unsigned int *)TOHOST_ADDR;

void sim_exit(int code) {
    *tohost = code;
    while (1); // wait for simulator to stop
}

void benchmark_success()
{
    sim_exit(0);
}

void benchmark_failure()
{
    sim_exit(123);
}
