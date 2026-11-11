/**************************************************************************//**
 * @file     startup_om662x.S
 * @brief    CMSIS-Core(M) Device Startup File for OM662X Device
 * @version  V2.0.1
 * @date     23. July 2019
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

                .syntax  unified
                .arch    armv7-m
                .eabi_attribute Tag_ABI_align_preserved, 1

                #include "features.h"

                #if defined(__ARMCC_VERSION)
                #define __INITIAL_SP     Image$$ARM_LIB_STACK$$ZI$$Limit
                .section RESET, "a"
                #elif defined(__GNUC__)
                #define __INITIAL_SP     __StackTop
                .section .vectors, "a"
                #endif

                .align   2
                .globl   __Vectors
                .globl   __Vectors_End
                .globl   __Vectors_Size
__Vectors:
                .long    __INITIAL_SP                       /*     Top of Stack */
                .long    Reset_Handler                      /*     Reset Handler */
                .long    NMI_Handler                        /* -14 NMI Handler */
                .long    HardFault_Handler                  /* -13 Hard Fault Handler */
                .long    MemManage_Handler                  /* -12 MPU Fault Handler */
                .long    BusFault_Handler                   /* -11 Bus Fault Handler */
                .long    UsageFault_Handler                 /* -10 Usage Fault Handler */
                .long    SoftFault_Handler                  /*  -9 Onmicro assign */
                .long    0xFFFFFFFF                         /*     RELEASE_SVN_REV */
                .long    __Vectors                          /*     Base Address */
                #if defined(CONFIG_XIP_FLASH_ALL)
                .long    0x46533252                         /*     RUN2SF_MAGIC_FLAG */
                #else
                .long    0xFFFFFFFF                         /*     Reserved */
                #endif
                .long    SVC_Handler                        /*  -5 SVCall Handler */
                .long    DebugMon_Handler                   /*  -4 Debug Monitor Handler */
                .long    CONFIG_ROM_ID                      /*     ROM ID */
                .long    PendSV_Handler                     /*  -2 PendSV Handler */
                .long    SysTick_Handler                    /*  -1 SysTick Handler */

                /* Interrupts */
                .long    BT_IRQHandler                     // 0
                .long    BT_WAKEUP_IRQHandler              // 1
                .long    DMA_IRQHandler                    // 2
                .long    PIN_WAKEUP_IRQHandler             // 3
                .long    TIM_IRQHandler                    // 4
                .long    OM24G_RF_IRQHandler               // 5
                .long    PMU_POF_IRQHandler                // 6
                .long    PMU_TIMER_IRQHandler              // 7
                .long    WDT_IRQHandler                    // 8
                .long    UART1_IRQHandler                  // 9
                .long    EFUSE_IRQHandler                  // 10
                .long    GPIO_IRQHandler                   // 11
                .long    ADC_IRQHandler                    // 12
                .long    I2C0_IRQHandler                   // 13
                .long    SF_IRQHandler                     // 14
                .long    SOFT0_IRQHandler                  // 15
                .long    SOFT1_IRQHandler                  // 16
                .long    SOFT2_IRQHandler                  // 17
                .long    SOFT3_IRQHandler                  // 18
                .long    SOFT4_IRQHandler                  // 19
                .long    SOFT5_IRQHandler                  // 20
                .long    SOFT6_IRQHandler                  // 21
                .long    SOFT7_IRQHandler                  // 22
                .long    VTRACK_IRQHandler                 // 23
                .long    CRY32M_RDY_IRQHandler             // 24
                .long    UART0_IRQHandler                  // 25
                .long    SPI0_IRQHandler                   // 26
                .long    CC_INTR_IRQHandler                // 27
                .long    TIM0_IRQHandler                   // 28
                .long    TIM1_IRQHandler                   // 29
                .long    TIM2_IRQHandler                   // 30
                .long    SPI1_IRQHandler                   // 31
                .long    RTC_1HZ_IRQHandler                // 32
                .long    RTC_AF_IRQHandler                 // 33
                .long    LP_TIMER_IRQHandler               // 34

__Vectors_End:
                .equ     __Vectors_Size, __Vectors_End - __Vectors
                .size    __Vectors, . - __Vectors


                .thumb
                .section .text
                .align   2

                .thumb_func
                .type    Reset_Handler, %function
                .globl   Reset_Handler
                .fnstart
Reset_Handler:
                #ifdef CONFIG_XIP_ROM_ALL
                bl       SystemExitDeepSleep
                #endif

                cpsid i
                bl       SystemInit

                #if defined(__ARMCC_VERSION)
                bl       __main
                #elif defined(__GNUC__)
                ldr      r4, =__copy_table_start__
                ldr      r5, =__copy_table_end__

.L_loop0:
                cmp      r4, r5
                bge      .L_loop0_done
                ldr      r1, [r4]              /* source address */
                ldr      r2, [r4, #4]          /* destination address */
                ldr      r3, [r4, #8]          /* word count */
                lsls     r3, r3, #2            /* byte count */

.L_loop0_0:
                subs     r3, #4                /* decrement byte count */
                ittt     ge
                ldrge    r0, [r1, r3]
                strge    r0, [r2, r3]
                bge      .L_loop0_0

                adds     r4, #12
                b        .L_loop0
.L_loop0_done:

                ldr      r3, =__zero_table_start__
                ldr      r4, =__zero_table_end__

.L_loop2:
                cmp      r3, r4
                bge      .L_loop2_done
                ldr      r1, [r3]                /* destination address */
                ldr      r2, [r3, #4]            /* word count */
                lsls     r2, r2, #2              /* byte count */
                movs     r0, 0

.L_loop2_0:
                subs     r2, #4                  /* decrement byte count */
                itt      ge
                strge    r0, [r1, r2]
                bge      .L_loop2_0

                adds     r3, #8
                b        .L_loop2
.L_loop2_done:
                bl       SystemInitPost
                bl      _start
                #endif

                .fnend
                .size    Reset_Handler, . - Reset_Handler


                #if defined(__ARMCC_VERSION)
/* From __main */
                .thumb_func
                .type    $Sub$$main, %function
                .globl   $Sub$$main
                .fnstart
$Sub$$main:
                bl       SystemInitPost
                bl       $Super$$main
                .fnend
                #endif


#if 0  /* In drv_isr.c */
/* The default macro is not used for HardFault_Handler
 * because this results in a poor debug illusion.
 */
                .thumb_func
                .type    HardFault_Handler, %function
                .weak    HardFault_Handler
                .fnstart
HardFault_Handler:
                b        .
                .fnend
                .size    HardFault_Handler, . - HardFault_Handler

                .thumb_func
                .type    Default_Handler, %function
                .weak    Default_Handler
                .fnstart
Default_Handler:
                b        .
                .fnend
                .size    Default_Handler, . - Default_Handler

/* Macro to define default exception/interrupt handlers.
 * Default handler are weak symbols with an endless loop.
 * They can be overwritten by real handlers.
 */
                .macro   Set_Default_Handler  Handler_Name
                .weak    \Handler_Name
                .set     \Handler_Name, Default_Handler
                .endm


/* Default exception/interrupt handler */
                Set_Default_Handler  NMI_Handler
                Set_Default_Handler  MemManage_Handler
                Set_Default_Handler  BusFault_Handler
                Set_Default_Handler  UsageFault_Handler
                Set_Default_Handler  SoftFault_Handler
                Set_Default_Handler  SVC_Handler
                Set_Default_Handler  DebugMon_Handler
                Set_Default_Handler  PendSV_Handler
                Set_Default_Handler  SysTick_Handler

                Set_Default_Handler  BT_IRQHandler
                Set_Default_Handler  BT_WAKEUP_IRQHandler
                Set_Default_Handler  DMA_IRQHandler
                Set_Default_Handler  PIN_WAKEUP_IRQHandler
                Set_Default_Handler  TIM_IRQHandler
                Set_Default_Handler  OM24G_RF_IRQHandler
                Set_Default_Handler  PMU_POF_IRQHandler
                Set_Default_Handler  PMU_TIMER_IRQHandler
                Set_Default_Handler  WDT_IRQHandler
                Set_Default_Handler  UART1_IRQHandler
                Set_Default_Handler  EFUSE_IRQHandler
                Set_Default_Handler  GPIO_IRQHandler
                Set_Default_Handler  ADC_IRQHandler
                Set_Default_Handler  I2C0_IRQHandler
                Set_Default_Handler  SF_IRQHandler
                Set_Default_Handler  SOFT0_IRQHandler
                Set_Default_Handler  SOFT1_IRQHandler
                Set_Default_Handler  SOFT2_IRQHandler
                Set_Default_Handler  SOFT3_IRQHandler
                Set_Default_Handler  SOFT4_IRQHandler
                Set_Default_Handler  SOFT5_IRQHandler
                Set_Default_Handler  SOFT6_IRQHandler
                Set_Default_Handler  SOFT7_IRQHandler
                Set_Default_Handler  VTRACK_IRQHandler
                Set_Default_Handler  CRY32M_RDY_IRQHandler
                Set_Default_Handler  UART0_IRQHandler
                Set_Default_Handler  SPI0_IRQHandler
                Set_Default_Handler  CC_INTR_IRQHandler
                Set_Default_Handler  TIM0_IRQHandler
                Set_Default_Handler  TIM1_IRQHandler
                Set_Default_Handler  TIM2_IRQHandler
                Set_Default_Handler  SPI1_IRQHandler
                Set_Default_Handler  RTC_1HZ_IRQHandler
                Set_Default_Handler  RTC_AF_IRQHandler
                Set_Default_Handler  LP_TIMER_IRQHandler
#endif

                .end
