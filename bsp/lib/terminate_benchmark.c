#include "terminate_benchmark.h"

extern volatile uint64_t tohost;

void sim_exit(int code) {
    tohost = code;
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
