/* Copyright TU Wien                                                               */
/* Licensed under the Solderpad Hardware License v2.1, see LICENSE.txt for details */
/* SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1                                */


OUTPUT_ARCH(riscv)
ENTRY(_start)

SEARCH_DIR(.)
__DYNAMIC = 0;

MEMORY
{
    boot : ORIGIN = 0x00000000, LENGTH = 0x00002000 /*   8 kB */
    ram  : ORIGIN = 0x00002000, LENGTH = 0x003FE000 /* 248 kB */
}

STACK_LEN = 0x4000; /* 16 kB */

SECTIONS
{
    .vectors 0 : {
        *(.vectors)
    } >boot


    .tohost 0x500 : {
        . = ALIGN(8);
        tohost = .;
        . += 8;
    } >boot

    .fromhost 0x508 : {
        . = ALIGN(8);
        fromhost = .;
        . += 8;
    } >boot

    .text : {
        . = ALIGN(4);
        _stext = .;
        *(.text)
        *(.text.*)
        _etext  =  .;
        __CTOR_LIST__ = .;
        LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
        *(.ctors)
        LONG(0)
        __CTOR_END__ = .;
        __DTOR_LIST__ = .;
        LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
        *(.dtors)
        LONG(0)
        __DTOR_END__ = .;
        *(.lit)
        *(.shdata)
        . = ALIGN(4);
        _endtext = .;
    } >ram

    .rodata : {
        . = ALIGN(4);
        *(.rodata);
        *(.rodata.*)
    } >ram

    .shbss : {
        . = ALIGN(4);
        *(.shbss)
    } >ram

    .data          :
    {
        . = ALIGN(4);
        __DATA_BEGIN__ = .;
        *(.data .data.*)
        *(.gnu.linkonce.d.*)
    } >ram


    .sdata          :
    {
        . = ALIGN(4);
        __SDATA_BEGIN__ = .;
        *(.sdata .sdata.*)
        *(.gnu.linkonce.s.*)
    } >ram

    . = ALIGN(4);
    PROVIDE( _edata = . );
    PROVIDE( edata = . );

    PROVIDE( _fbss = . );
    PROVIDE( __bss_start = . );
    .bss (NOLOAD) : {
        . = ALIGN(4);
        _bss_start = .;
        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
        *(COMMON)
        _bss_end = .;
    } >ram

    .user_align (NOLOAD) : {
        . = ALIGN(0x1000);
        _user_start = .;
    } >ram

    . = ALIGN(8);
    __BSS_END__ = .;
    __global_pointer$ = MIN(__SDATA_BEGIN__ + 0x800,  MAX(__DATA_BEGIN__ + 0x800, __BSS_END__ - 0x800));
    PROVIDE( _end = . );
    PROVIDE( end = . );

    .stack ORIGIN(ram) + LENGTH(ram) - STACK_LEN (NOLOAD) : {
        PROVIDE( _heap_end = . );
        . = ALIGN(16);
        _stack_start = .;
        . = . + STACK_LEN;
        . = ALIGN(16);
        stack = .;
        _stack = .;
        _stack_end = .;
        PROVIDE( _sp = . );
    } >ram
}
