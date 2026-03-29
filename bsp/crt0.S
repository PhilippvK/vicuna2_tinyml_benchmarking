# Copyright TU Wien
# Licensed under the Solderpad Hardware License v2.1, see LICENSE.txt for details
# SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1


    .section .vectors, "ax", @progbits
    .option norvc;

    # redirect all interrupts to the execption wrapper
    .rept 31
    nop
    .endr
    j exc_wrapper

    # the reset vector is redirected to _start
    j _start



    .section .text

# entry point
    .global _start
    .type   _start, @function
_start:

    # Initialize global pointer
    .option push
    .option norelax
    la      gp, __global_pointer$
    .option pop

    # initialize stack pointer
    la sp, _stack

    # Clear the bss segment
    # memset(void* str, int c, size_t n);
    # memset(__bss_start, 0, __bss_start - _end);
    la      a0, __bss_start
    li      a1, 0
    la      a2, _end
    sub     a2, a2, a0
    call    memset

    # Register global termination functions
    # to be called upon exit
    la      a0, __libc_fini_array
    call    atexit

    # Run global initialization functions
    call    __libc_init_array

    # jump to main (argc = 0, argv = NULL, envp = NULL)
    li      a0, 0
    li      a1, 0
    li      a2, 0 # set envp = NULL just in case
    call    main

    tail    exit


# default exception handler (infinite loop)
    .weak exception_handler
    .type exception_handler, @function
exception_handler:
    j exception_handler


# wrapper for exception handlers (saves registers and calls a handler function)
exc_wrapper:
    # save registers:
    addi sp, sp, -80
    sw ra, 76(sp)
    sw s0, 72(sp)
    addi s0, sp, 80
    sw t0, 68(sp)
    sw t1, 64(sp)
    sw t2, 60(sp)
    sw t3, 56(sp)
    sw t4, 52(sp)
    sw t5, 48(sp)
    sw t6, 44(sp)
    sw a0, 40(sp)
    sw a1, 36(sp)
    sw a2, 32(sp)
    sw a3, 28(sp)
    sw a4, 24(sp)
    sw a5, 20(sp)
    sw a6, 16(sp)
    sw a7, 12(sp)

    # save mepc, mie and mstatus:
    csrr t0, mie
    csrr t1, mstatus
    csrr a1, mepc
    sw t0, 0(sp)
    sw t1, 4(sp)
    sw a1, 8(sp)

    # call exception handler (a0 = mcause, a1 = mepc, a2 = mtval, a3 = fp)
    # void _trap_handler_c(unsigned long mcause, unsigned long mepc)
    csrr a0, mcause
    csrr a1, mepc
    call    _trap_handler_c

    # restore mepc, mie and mstatus (restoring mstatus disables interrupts):
    lw t0, 0(sp)
    lw t1, 4(sp)
    lw t2, 8(sp)
    csrw mie, t0
    csrw mstatus, t1
    csrw mepc, t2

    # restore registers:
    lw a7, 12(sp)
    lw a6, 16(sp)
    lw a5, 20(sp)
    lw a4, 24(sp)
    lw a3, 28(sp)
    lw a2, 32(sp)
    lw a1, 36(sp)
    lw a0, 40(sp)
    lw t6, 44(sp)
    lw t5, 48(sp)
    lw t4, 52(sp)
    lw t3, 56(sp)
    lw t2, 60(sp)
    lw t1, 64(sp)
    lw t0, 68(sp)
    lw s0, 72(sp)
    lw ra, 76(sp)
    addi sp, sp, 80
    mret
