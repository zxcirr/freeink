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
 * @file     shell_cmd.c
 * @brief    shell system command
 * @date     30. Sept  2021
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


/*******************************************************************************
 * INCLUDES
 */
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "shell_cmd.h"
#include "shell.h"

#ifdef CONFIG_SHELL

/*******************************************************************************
 * CONST & VARIABLES
 */
static void cmd_mem32r(int argc, char *argv[]);
static void cmd_mem32w(int argc, char *argv[]);

const shell_cmd_t shell_cmd [] = {
    { "mem32r",  cmd_mem32r,  "read                        Usage: mem32r addr [count]/[start_bit end_bit]"    },
    { "mem32w",  cmd_mem32w,  "write                       Usage: mem32w addr [data]/[start_bit end_bit data]"},
    //ADD other command
    { NULL,        NULL,        NULL                                                                  },     /* donot deleted */
};


/*******************************************************************************
 * MACROS
 */
#define    SWAP(a, b)           do {                                           \
                                    uint32_t swap;                             \
                                    swap = a;                                  \
                                    a = b;                                     \
                                    b = swap;                                  \
                                } while(0)


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static void cmd_mem32r(int argc, char *argv[])
{
    volatile uint32_t *mem_ptr;
    uint32_t address;
    uint32_t cnt;
    uint32_t i;
    uint32_t start_bit;
    uint32_t end_bit;
    uint32_t bit_len;

    start_bit = 0U;
    end_bit = 0U;
    if(argc == 1) {             //mem32r addr
        cnt = 1;
        bit_len = 0;
    } else if (argc == 2) {     //mem32 addr count
        cnt = strtoul(argv[1], NULL, 16);
        bit_len = 0;
    } else if (argc == 3) {     //mem32 addr start_bit end_bit
        cnt = 1;
        start_bit = (strtoul(argv[1], NULL, 10)) % 32;
        end_bit = strtoul(argv[2], NULL, 10) % 32;
        if (start_bit > end_bit) {
            SWAP(start_bit, end_bit);
        }
        bit_len = start_bit - end_bit + 1U;
    } else {
        shell_printf("Usage: mem32r address count\n");
        return;
    }

    address   = strtoul(argv[0], NULL, 0);
    mem_ptr = (volatile uint32_t *)address;
    for(i = 0; i < cnt; i++) {
        shell_printf("\n0x%08X:\t", (address+i*4));
        shell_printf("0x%08X ", mem_ptr[i]);
    }

    // mem32 addr start_bit end_bit
    if (bit_len) {
        uint32_t bit_val;
        uint32_t mask;

        /*lint -save -e644*/
        mask = ((end_bit - start_bit) == 31) ? 0xFFFFFFFFU : ((1u << bit_len) - 1);
        bit_val = ((*(volatile uint32_t *)address) >> start_bit) & mask;
        shell_printf("[%d, %d]'s value is 0x%08X\n\n", end_bit, start_bit, bit_val);
        /*lint -restore*/
    }
    shell_printf("\n\n");
}

static void cmd_mem32w(int argc, char *argv[])
{
    uint32_t address;
    uint32_t wdata;
    uint32_t start_bit;
    uint32_t end_bit;
    uint32_t bit_len;

    address = strtoul(argv[0], NULL, 0);
    if (argc == 2) {                 // mem32w addr data
        wdata = strtoul(argv[1], NULL, 0);
        *((volatile uint32_t *)address) = wdata;
        shell_printf("[0x%08X] set 0x%08x completed\n\n", address, wdata);
    } else if (argc == 4) {         // mem32w addr start_bit end_bit data
        uint32_t mask;
        uint32_t mem_data;

        start_bit = strtoul(argv[1], NULL, 0) % 32;
        end_bit   = strtoul(argv[2], NULL, 0) % 32;
        wdata = strtoul(argv[3], NULL, 16);

        if (start_bit > end_bit) {
            SWAP(start_bit, end_bit);
        }
        bit_len = end_bit - start_bit + 1;
        mask = ((end_bit - start_bit) == 31) ? 0xFFFFFFFFU : (((1u << bit_len) - 1) << start_bit);
        mem_data = *((volatile uint32_t *)address);
        *((volatile uint32_t *)address) = (mem_data & (~mask)) | ((wdata << start_bit) & mask);

        shell_printf("0x%08x [%d, %d] set 0x%08x completed\n\n", address, start_bit, end_bit, wdata);
    } else {
        shell_printf("Usage: mem32w address value\n");
    }
    shell_printf("\n\n");
}

#endif

/** @} */
