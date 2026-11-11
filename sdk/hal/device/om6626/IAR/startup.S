;/**************************************************************************//**
; * @file     startup_ARMCM4.s
; * @brief    CMSIS Core Device Startup File for
; *           ARMCM33 Device
; * @version  V5.3.1
; * @date     09. July 2018
; ******************************************************************************/
;/*
; * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

                #include "features.h"

                MODULE   ?cstartup

                ;; Forward declaration of sections.
                SECTION  CSTACK:DATA:NOROOT(3)

                SECTION  .intvec:CODE:NOROOT(2)

                EXTERN   NMI_Handler                       ; -14 NMI Handler
                EXTERN   HardFault_Handler                 ; -13 Hard Fault Handler
                EXTERN   MemManage_Handler                 ; -12 MPU Fault Handler
                EXTERN   BusFault_Handler                  ; -11 Bus Fault Handler
                EXTERN   UsageFault_Handler                ; -10 Usage Fault Handler
                EXTERN   SoftFault_Handler                 ;  -9 Onmicro assign
                EXTERN   SVC_Handler                       ;  -5 SVCall Handler
                EXTERN   DebugMon_Handler                  ;  -4 Debug Monitor Handler
                EXTERN   PendSV_Handler                    ;  -2 PendSV Handler
                EXTERN   SysTick_Handler                   ;  -1 SysTick Handler
                EXTERN   BT_IRQHandler                     ; 0
                EXTERN   BT_WAKEUP_IRQHandler              ; 1
                EXTERN   DMA_IRQHandler                    ; 2
                EXTERN   PIN_WAKEUP_IRQHandler             ; 3
                EXTERN   TIM_IRQHandler                    ; 4
                EXTERN   OM24G_RF_IRQHandler               ; 5
                EXTERN   PMU_POF_IRQHandler                ; 6
                EXTERN   PMU_TIMER_IRQHandler              ; 7
                EXTERN   WDT_IRQHandler                    ; 8
                EXTERN   UART1_IRQHandler                  ; 9
                EXTERN   EFUSE_IRQHandler                  ; 10
                EXTERN   GPIO_IRQHandler                   ; 11
                EXTERN   ADC_IRQHandler                    ; 12
                EXTERN   I2C0_IRQHandler                   ; 13
                EXTERN   SF_IRQHandler                     ; 14
                EXTERN   SOFT0_IRQHandler                  ; 15
                EXTERN   SOFT1_IRQHandler                  ; 16
                EXTERN   SOFT2_IRQHandler                  ; 17
                EXTERN   SOFT3_IRQHandler                  ; 18
                EXTERN   SOFT4_IRQHandler                  ; 19
                EXTERN   SOFT5_IRQHandler                  ; 20
                EXTERN   SOFT6_IRQHandler                  ; 21
                EXTERN   SOFT7_IRQHandler                  ; 22
                EXTERN   VTRACK_IRQHandler                 ; 23
                EXTERN   CRY32M_RDY_IRQHandler             ; 24
                EXTERN   UART0_IRQHandler                  ; 25
                EXTERN   SPI0_IRQHandler                   ; 26
                EXTERN   CC_INTR_IRQHandler                ; 27
                EXTERN   TIM0_IRQHandler                   ; 28
                EXTERN   TIM1_IRQHandler                   ; 29
                EXTERN   TIM2_IRQHandler                   ; 30
                EXTERN   SPI1_IRQHandler                   ; 31
                EXTERN   RTC_1HZ_IRQHandler                ; 32
                EXTERN   RTC_AF_IRQHandler                 ; 33
                EXTERN   LP_TIMER_IRQHandler               ; 34

                PUBLIC   __vector_table
                PUBLIC   __Vectors
                PUBLIC   __Vectors_End
                PUBLIC   __Vectors_Size

                DATA

__vector_table
                DCD    sfe(CSTACK)                       ;     Top of Stack
                DCD    Reset_Handler                     ;     Reset Handler
                DCD    NMI_Handler                       ; -14 NMI Handler
                DCD    HardFault_Handler                 ; -13 Hard Fault Handler
                DCD    MemManage_Handler                 ; -12 MPU Fault Handler
                DCD    BusFault_Handler                  ; -11 Bus Fault Handler
                DCD    UsageFault_Handler                ; -10 Usage Fault Handler
                DCD    SoftFault_Handler                 ;  -9 Onmicro assign
                DCD    0xFFFFFFFF                        ;     RELEASE_SVN_REV
                DCD    __vector_table                    ;     Base Address
                #if defined(CONFIG_XIP_FLASH_ALL)        ;
                DCD    0x46533252                        ;     RUN2SF_MAGIC_FLAG
                #else
                DCD    0xFFFFFFFF                        ;     Reserved */
                #endif
                DCD    SVC_Handler                       ;  -5 SVCall Handler
                DCD    DebugMon_Handler                  ;  -4 Debug Monitor Handler
                DCD    CONFIG_ROM_ID                     ;     ROM ID
                DCD    PendSV_Handler                    ;  -2 PendSV Handler
                DCD    SysTick_Handler                   ;  -1 SysTick Handler

                /* Interrupts */
                DCD    BT_IRQHandler                     ; 0
                DCD    BT_WAKEUP_IRQHandler              ; 1
                DCD    DMA_IRQHandler                    ; 2
                DCD    PIN_WAKEUP_IRQHandler             ; 3
                DCD    TIM_IRQHandler                    ; 4
                DCD    OM24G_RF_IRQHandler               ; 5
                DCD    PMU_POF_IRQHandler                ; 6
                DCD    PMU_TIMER_IRQHandler              ; 7
                DCD    WDT_IRQHandler                    ; 8
                DCD    UART1_IRQHandler                  ; 9
                DCD    EFUSE_IRQHandler                  ; 10
                DCD    GPIO_IRQHandler                   ; 11
                DCD    ADC_IRQHandler                    ; 12
                DCD    I2C0_IRQHandler                   ; 13
                DCD    SF_IRQHandler                     ; 14
                DCD    SOFT0_IRQHandler                  ; 15
                DCD    SOFT1_IRQHandler                  ; 16
                DCD    SOFT2_IRQHandler                  ; 17
                DCD    SOFT3_IRQHandler                  ; 18
                DCD    SOFT4_IRQHandler                  ; 19
                DCD    SOFT5_IRQHandler                  ; 20
                DCD    SOFT6_IRQHandler                  ; 21
                DCD    SOFT7_IRQHandler                  ; 22
                DCD    VTRACK_IRQHandler                 ; 23
                DCD    CRY32M_RDY_IRQHandler             ; 24
                DCD    UART0_IRQHandler                  ; 25
                DCD    SPI0_IRQHandler                   ; 26
                DCD    CC_INTR_IRQHandler                ; 27
                DCD    TIM0_IRQHandler                   ; 28
                DCD    TIM1_IRQHandler                   ; 29
                DCD    TIM2_IRQHandler                   ; 30
                DCD    SPI1_IRQHandler                   ; 31
                DCD    RTC_1HZ_IRQHandler                ; 32
                DCD    RTC_AF_IRQHandler                 ; 33
                DCD    LP_TIMER_IRQHandler               ; 34

__Vectors_End
__Vectors       EQU      __vector_table
__Vectors_Size  EQU      __Vectors_End - __Vectors



                THUMB

                SECTION  .text:CODE:REORDER:NOROOT(2)

                EXTERN   __iar_program_start
                EXTERN   SystemInit
                EXTERN   SystemInitPost

; Reset Handler
Reset_Handler
                #ifdef CONFIG_XIP_ROM_ALL
                BL  SystemExitDeepSleep
                #endif

                cpsid i

                BL  SystemInit

                LDR R0, =__iar_program_start
                BX  R0

; __low_level_init will auto called by IAR cstartup
                EXTERN  __iar_data_init3
                PUBLIC  __low_level_init
__low_level_init
                PUSH {LR}
                ; call IAR table copy function.
                BL  __iar_data_init3
                BL  SystemInitPost
                MOV R0, #0   ; return 0
                POP  {PC}


                END

