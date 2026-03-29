#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "terminate_benchmark.h"

#define INSTRUCTION_ADDRESS_MISALIGNED 0
#define INSTRUCTION_ACCESS_FAULT 1
#define INSTRUCTION_PAGE_FAULT 12

#define LOAD_ADDRESS_MISALIGNED 4
#define LOAD_ACCESS_FAULT 5
#define LOAD_PAGE_FAULT 13

#define STORE_AMO_ADDRESS_MISALIGNED 6
#define STORE_AMO_ACCESS_FAULT 7
#define STORE_AMO_PAGE_FAULT 15

#define ILLEGAL_INSTRUCTION 2
#define BREAKPOINT 3

#define ECALL_FROM_U_MODE 8
#define ECALL_FROM_S_MODE 9
#define ECALL_FROM_M_MODE 11

#define ERROR_HALT(msg)                           \
    uart_printf("EXCEPTION: %s at %08X\n", msg, mepc); \
    sim_exit(-1);

void _trap_handler_c(uint32_t mcause, uint32_t mepc)
{
    switch (mcause)
    {
    case BREAKPOINT:
        return; // do nothing

    case INSTRUCTION_ADDRESS_MISALIGNED:
        ERROR_HALT("Instruction address misaligned");
    case INSTRUCTION_ACCESS_FAULT:
        ERROR_HALT("Instruction access fault");
    case INSTRUCTION_PAGE_FAULT:
        ERROR_HALT("Instruction page fault");

    case LOAD_ADDRESS_MISALIGNED:
        ERROR_HALT("Load address misaligned");
    case LOAD_ACCESS_FAULT:
        ERROR_HALT("Load access fault");
    case LOAD_PAGE_FAULT:
        ERROR_HALT("Load page fault");

    case STORE_AMO_ADDRESS_MISALIGNED:
        ERROR_HALT("Store/AMO address misaligned");
    case STORE_AMO_ACCESS_FAULT:
        ERROR_HALT("Store/AMO access fault");
    case STORE_AMO_PAGE_FAULT:
        ERROR_HALT("Store/AMO page fault");

    case ILLEGAL_INSTRUCTION:
        ERROR_HALT("Illegal instruction");

    case ECALL_FROM_U_MODE:
    case ECALL_FROM_S_MODE:
    case ECALL_FROM_M_MODE:
        ERROR_HALT("ECALL");

    default:
        uart_printf("CAUSE: %u\n", mcause);
        ERROR_HALT("Unhandled cause");
    }
}

// Return global reent struct if any newlib functions are linked to bootloader
struct _reent *__getreent(void)
{
    return _GLOBAL_REENT;
}

void __cxa_pure_virtual() {
  // You can optionally put a breakpoint or error handler here
  while (1) {} // or abort(), if available
}
