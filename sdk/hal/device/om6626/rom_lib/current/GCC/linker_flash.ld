/* ----------------------------------------------------------------------------
 * Copyright (c) 2020-2030 OnMicro Limited. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of OnMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * -------------------------------------------------------------------------- */

/**
 * @file     om662x_flash.ld
 * @brief    linker script for flash version
 * @date     14. Sept  2020
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

/* Include ROM symbol */
INCLUDE  rom_symbol.ld

/*--------------------- Stack / Heap Configuration ----------------------------
  <h> Stack / General Heap / DMA Heap Configuration
    <o0> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
    <o1> General Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
  </h>
  -----------------------------------------------------------------------------*/
__STACK_SIZE = 0x800;
__GENERAL_HEAP_SIZE  = 0x400;

/* RAM SIZE: the reduced SRAM is occupied by ROM */
__RAM_SIZE = 0x00013ea0;

/* Define memory regions. */
MEMORY
{
    RAM (rwx)     : ORIGIN = 0x20000000, LENGTH = __RAM_SIZE
    RAM_RO (rwx)  : ORIGIN = 0x00200000, LENGTH = __RAM_SIZE
    FLASH (rx)    : ORIGIN = 0x00404000, LENGTH = 1012K
}

/* Define the entry point of the output file. */
ENTRY(Reset_Handler)

SECTIONS
{
    .vectors :
    {
        . = ALIGN(128);
        KEEP(*(.vectors))
    } > FLASH

    /* RAM */
    .ram_code :
    {
        . = ALIGN(32);
        __ram_code_start__ = .;
        *(.ram_code)
        . = ALIGN(4);
        *(.ram_rodata)
        . = ALIGN(32);
        __ram_code_end__ = .;
    } > RAM_RO AT > FLASH

    .text :
    {
        . = ALIGN(32);
        __code_start__ = .;
        *(.text*)
        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        *(.rodata*)
        . = ALIGN(32);
        __code_end__ = .;
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        /* ram_code and ram_rodata */
        LONG (LOADADDR(.ram_code))
        LONG (__ram_code_start__)
        LONG ((__ram_code_end__ - __ram_code_start__ + 3) >> 2)

        /* ram data */
        LONG (LOADADDR(.ram_data))
        LONG (__ram_data_start__)
        LONG ((__ram_data_end__ - __ram_data_start__ + 3) >> 2)

        __copy_table_end__ = .;
    } > FLASH

    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        /* ram bss */
        LONG (__ram_bss_start__)
        LONG ((__ram_bss_end__ - __ram_bss_start__ + 3) >> 2)
        __zero_table_end__ = .;
    } > FLASH

    .ram_non_init (NOLOAD):   /* reserved space for ram_code, NOT added any section */
    {
        . = . + SIZEOF(.ram_code);
        *(.ram_data_non_init)
    } > RAM

    .ram_data :
    {
        . = ALIGN(32);
        __ram_data_start__ = .;
        *(.data*)               /* Read-write initialized data */
        . = ALIGN(32);
        __ram_data_end__ = .;
    } > RAM AT > FLASH

    .ram_bss :
    {
        . = ALIGN(32);
        __ram_bss_start__ = .;
        *(.bss*)               /* Read-write zero initialized data */
        *(COMMON)
        . = ALIGN(32);
        __ram_bss_end__ = .;
    } > RAM

    .general_heap :
    {
        *(.ram_data_no_init)
        . = ALIGN(32);
        __general_heap_start__ = .;
        . = . + __GENERAL_HEAP_SIZE;
        . = ALIGN(4);
        __general_heap_end__ = .;
    } > RAM

    .stack :
    {
        . = ALIGN(8);
        __StackLimit = .;
        . = . + __STACK_SIZE;
        . = ALIGN(8);
        __StackTop = .;
    } > RAM

    PROVIDE(__stack = __StackTop);
}


/** @} */
