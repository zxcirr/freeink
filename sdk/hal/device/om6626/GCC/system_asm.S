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
 * @file     system_om6681a_gnu.S
 * @brief
 * @date     26 Jan. 2022
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
#include "features.h"

/*******************************************************************************
 * MACROS
 */

.syntax unified

/**
 * System sleep status:
 *
 * [0:29]  wakeup resume address
 * [30]    0: jump to __SYSTEM_CPU_SUSPEND_RESUME  1: jump to [0:29] address
 * [31]    0: power up reset flag  1: sleep reset flag
 * */
#define __SYSTEM_CPU_STATUS     0x400E00F0
#define __SYSTEM_PMU_STATE_REG  0X400E0000

/* SCR */
#define __SYSTEM_CPU_SCR        0xE000ED10

/*******************************************************************************
 * SECTION .data_aon_ram
 */

/* .rodata */
.section  .rodata

/* REGs save address table */
.align 4
__SYSTEM_REG_SAVE_ADDR:
    .long   0xE000E100  /* NVIC->ISER (EXTERNAL_IRQn_Num / 32) */
    .long   2           /* NVIC->ISER[0] ~ NVIC->ISER[1] */
    .long   0xE000E400  /* NVIC->IP (EXTERNAL_IRQn_Num / 4)  */
    .long   9           /* NVIC->IP[0] ~ NVIC->IP[8] */
    .long   0xE000ED08  /* SCB->VTOR */
    .long   0xE000ED88  /* SCB->CPACR */
__SYSTEM_REG_SAVE_ADDR_END:


/* .bss, writable section */
.section  .data

/* REG save buffer */
.align 4
__SYSTEM_CPU_REG_SAVE_ADDR:
    .long   0  /* 0  CONTROL */
    .long   0  /* 4  MSP */
    .long   0  /* 8  PSP */
    .long   0  /* 12 reserved */
__SYSTEM_CPU_REG_SAVE_ADDR_END:


/*******************************************************************************
 * SECTION .ramtext
 */

/* .ramtext, executable section */
.section .text

.global SystemEnterDeepSleep
.type SystemEnterDeepSleep, %function
.global SystemExitDeepSleep
.type SystemExitDeepSleep, %function
.global SystemFromRomExitDeepSleepEnable
.type SystemFromRomExitDeepSleepEnable, %function
.global SystemRunTo
.type SystemRunTo, %function

/* void SystemEnterDeepSleep(void) */
SystemEnterDeepSleep:

    /* Push register to stack */
    PUSH {R0-R12, LR}

    /*
     * Push REGs {
     *
     * void push_regs(uint32_t start, uint32_t stop)
     * {
     *     uint32_t num;
     *     uint32_t addr;
     *     for (; start<stop; start+=4) {
     *         addr = *(uint32_t *)start;
     *         num = 1;
     *         if ((start+4)<stop && *(uint32_t *)(start+4)<0x00001000) {
     *             num = *(uint32_t *)(start+4);
     *             start += 4;
     *         }
     *         for (int j=0; j<num; ++j) {
     *             push_it(*(__IO uint32_t *)(addr + j*4));
     *         }
     *     }
     * }
     */
    LDR R4, =__SYSTEM_REG_SAVE_ADDR
    LDR R6, =__SYSTEM_REG_SAVE_ADDR_END
__SYSTEM_REG_SAVE_LOOP_0:
    CMP R4, R6
    BCC __SYSTEM_REG_SAVE_LOOP_1
    B __SYSTEM_REG_SAVE_LOOP_END
__SYSTEM_REG_SAVE_LOOP_1:
    MOV R2, R4
    LDR R7, [R2], #4
    CMP R2, R6
    BCS __SYSTEM_REG_SAVE_LOOP_4
    LDR R5, [R4, #4]
    CMP R5, #4096
    BCS __SYSTEM_REG_SAVE_LOOP_4
    MOV R4, R2
__SYSTEM_REG_SAVE_LOOP_2:
    ADD R5, R7, R5, LSL #2
__SYSTEM_REG_SAVE_LOOP_3:
    CMP R5, R7
    BNE __SYSTEM_REG_SAVE_LOOP_5
    ADDS R4, #4
    B __SYSTEM_REG_SAVE_LOOP_0
__SYSTEM_REG_SAVE_LOOP_4:
    MOVS R5, #1
    B __SYSTEM_REG_SAVE_LOOP_2
__SYSTEM_REG_SAVE_LOOP_5:
    LDR R0, [R7], #4
    PUSH {R0}
    B __SYSTEM_REG_SAVE_LOOP_3
__SYSTEM_REG_SAVE_LOOP_END:
    /* } */

    /* R0 is buffer base */
    LDR R0, =__SYSTEM_CPU_REG_SAVE_ADDR

    /* Save CONTROL */
    MRS R1, CONTROL
    STR R1, [R0, #0]

    /* Save MSP */
    MRS R1, MSP
    STR R1, [R0, #4]

    /* Save PSP */
    MRS R1, PSP
    STR R1, [R0, #8]

    /* __SYSTEM_CPU_STATUS |= (1<<31) */
    LDR	R0, =__SYSTEM_CPU_STATUS
    LDR R1, [R0]
    ORR R1, R1, #0x80000000
    STR R1, [R0]

    /* set deep sleep flag */
    LDR	R0, =__SYSTEM_CPU_SCR
    MOV R1, #4
    STR R1, [R0]

    /* Good Night */
    WFI

    /*
     * Wait PMU ready: Check PMU_STATE (PMU_BASIC[31:27]) equal to 7
     *
     * That prevent the problem:
     *   When the PMU is going through the sleep process, an interrupt causes
     *   the CPU to work, but the PMU has not yet woken up.
     */
    LDR R0, =__SYSTEM_PMU_STATE_REG
__SYSTEM_REG_PMU_STATE_CHECK:
    LDR R1, [R0]
    LSR R1, R1, #27
    CMP R1, #7
    BNE __SYSTEM_REG_PMU_STATE_CHECK

    /* Resume Place */
__SYSTEM_CPU_SUSPEND_RESUME:

    /* Disable IRQ */
    CPSID I

    /* clear deep sleep flag */
    LDR	R0, =__SYSTEM_CPU_SCR
    MOV R1, #0
    STR R1, [R0]

    /* R0 is buffer base */
    LDR R0, =__SYSTEM_CPU_REG_SAVE_ADDR

    /* Restore CONTROL */
    LDR R1, [R0, #0]
    MSR CONTROL, R1

    /* Restore MSP */
    LDR R1, [R0, #4]
    MSR MSP, R1

    /* Restore PSP */
    LDR R1, [R0, #8]
    MSR PSP, R1

    /* __SYSTEM_CPU_STATUS &= ~(1<<31) */
    LDR R0, =__SYSTEM_CPU_STATUS
    LDR R1, [R0]
    /* BIC: dest = op_1 AND (~op_2) */
    BIC R1, R1, #0x80000000
    STR R1, [R0]

    /*
     * pop REGs {
     *
     * void pop_regs(uint32_t start, uint32_t stop)
     * {
     *     uint32_t num;
     *     uint32_t addr;
     *     for (stop-=4; stop>=start; stop-=4) {
     *         num = 1;
     *         if (*(uint32_t *)(stop)<0x00001000) {
     *             num = *(uint32_t *)(stop);
     *             stop -= 4;
     *         }
     *         addr = *(uint32_t *)stop;
     *         for (int j=num-1; j>=0; --j) {
     *             pop_it((addr + j*4));
     *         }
     *     }
     * }
     */
    LDR R6, =__SYSTEM_REG_SAVE_ADDR
    LDR R4, =__SYSTEM_REG_SAVE_ADDR_END
    SUBS R4, R4, #4
__SYSTEM_REG_RESTORE_LOOP_0:
    CMP R4, R6
    BCS __SYSTEM_REG_RESTORE_LOOP_1
    B __SYSTEM_REG_RESTORE_LOOP_END
__SYSTEM_REG_RESTORE_LOOP_1:
    LDR R5, [R4, #0]
    CMP R5, #4096
    ITE CC
    SUBCC R4, #4
    MOVCS R5, #1
    LDR R7, [R4, #0]
__SYSTEM_REG_RESTORE_LOOP_2:
    SUBS R5, #1
    BCS __SYSTEM_REG_RESTORE_LOOP_3
    SUBS R4, #4
    B __SYSTEM_REG_RESTORE_LOOP_0
__SYSTEM_REG_RESTORE_LOOP_3:
    ADD R0, R7, R5, LSL #2
    POP {R1}
    STR R1, [R0]
    B __SYSTEM_REG_RESTORE_LOOP_2
__SYSTEM_REG_RESTORE_LOOP_END:
    /* } */

    /* Pop register */
    POP {R0-R12, PC}


/* void SystemExitDeepSleep(void) */
SystemExitDeepSleep:

    /* if ((__SYSTEM_CPU_STATUS & (1<<31)) == 0) {goto __SYSTEM_CPU_POWER_UP_RESET} */
    LDR R0, =__SYSTEM_CPU_STATUS
    LDR R1, [R0]
    CMP	R1, #0
    /* BGE: >=0, MSB=0 */
    BGE __SYSTEM_CPU_POWER_UP_RESET

    /* if ((__SYSTEM_CPU_STATUS & (1<<30)) == 0) {goto __SYSTEM_CPU_SUSPEND_RESUME} */
    LSLS R0, R1, #1
    /* BPL: MSB=0 */
    BPL __SYSTEM_CPU_SUSPEND_RESUME

    /* else {goto (__SYSTEM_CPU_STATUS & ~0xC0000000)} */
    BIC R0, R1, #0xc0000000
    BX R0
    B .

__SYSTEM_CPU_POWER_UP_RESET:
    /* Normal system reset */
    BX LR


/* void SystemFromRomExitDeepSleepEnable(void) */
SystemFromRomExitDeepSleepEnable:

    /* __SYSTEM_CPU_STATUS = __SYSTEM_CPU_SUSPEND_RESUME | (1<<30) | 1 */
    LDR R1, =__SYSTEM_CPU_SUSPEND_RESUME
    ORR R1, R1, #0x40000000
    ORR R1, R1, #0x00000001
    LDR	R0, =__SYSTEM_CPU_STATUS
    STR R1, [R0]
    BX LR


/* void SystemRunTo(uint32_t vector_base) */
SystemRunTo:

    CPSID I
    LDR R1, [R0, #0]
    LDR R2, [R0, #4]
    MSR MSP, R1
    BX  R2
    B   .

    .end

