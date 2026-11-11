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
 * @file     rtc_reg.h
 * @brief    RTC Register for OnMicro OM66xx
 * @date     21 July 2020
 * @author   OnMicro SW Team
 *
 * @ingroup  REGS
 * @brief    RTC Register for OnMicro OM66xx
 * @details  common RTC Register definitions
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */
#ifndef __RTC_REG_H
#define __RTC_REG_H


/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include "common_reg.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
// RTC CR
#define RTC_CE_POS                        0
#define RTC_CE_MASK                       (1U << 0)
#define RTC_AE_0_POS                      1
#define RTC_AE_0_MASK                     (1U << 1)
#define RTC_AE_1_POS                      2
#define RTC_AE_1_MASK                     (1U << 2)
#define RTC_AE_2_POS                      3
#define RTC_AE_2_MASK                     (1U << 3)
#define RTC_AIE_0_POS                     4
#define RTC_AIE_0_MASK                    (1U << 4)
#define RTC_AIE_1_POS                     5
#define RTC_AIE_1_MASK                    (1U << 5)
#define RTC_AIE_2_POS                     6
#define RTC_AIE_2_MASK                    (1U << 6)
#define RTC_AF_0_CLR_POS                  7
#define RTC_AF_0_CLR_MASK                 (1U << 7)
#define RTC_AF_1_CLR_POS                  8
#define RTC_AF_1_CLR_MASK                 (1U << 8)
#define RTC_AF_2_CLR_POS                  9
#define RTC_AF_2_CLR_MASK                 (1U << 9)
#define RTC_1HZ_IE_POS                    10
#define RTC_1HZ_IE_MASK                   (1U << 10)
#define RTC_1HZ_CLR_POS                   11
#define RTC_1HZ_CLR_MASK                  (1U << 11)
#define RTC_SR_INI_SYNC_POS               12
#define RTC_SR_INI_SYNC_MASK              (1U << 12)
#define RTC_AF_WAKE_POS                   13U
#define RTC_AF_WAKE_MASK                  (7U << 13)
#define RTC_AF_0_WAKE_POS                 13
#define RTC_AF_0_WAKE_MASK                (1U << 13)
#define RTC_AF_1_WAKE_POS                 14
#define RTC_AF_1_WAKE_MASK                (1U << 14)
#define RTC_AF_2_WAKE_POS                 15
#define RTC_AF_2_WAKE_MASK                (1U << 15)
#define RTC_1HZ_WAKE_POS                  16
#define RTC_1HZ_WAKE_MASK                 (1U << 16)
#define RTC_EN_SYNC_POS                   23
#define RTC_EN_SYNC_MASK                  (1U << 23)
#define RTC_AE_0_SYNC_POS                 24
#define RTC_AE_0_SYNC_MASK                (1U << 24)
#define RTC_AE_1_SYNC_POS                 25
#define RTC_AE_1_SYNC_MASK                (1U << 25)
#define RTC_AE_2_SYNC_POS                 26
#define RTC_AE_2_SYNC_MASK                (1U << 26)
#define RTC_AIE_0_SYNC_POS                27
#define RTC_AIE_0_SYNC_MASK               (1U << 27)
#define RTC_AIE_1_SYNC_POS                28
#define RTC_AIE_1_SYNC_MASK               (1U << 28)
#define RTC_AIE_2_SYNC_POS                29
#define RTC_AIE_2_SYNC_MASK               (1U << 29)

// RTC GR
#define RTC_LOCK_POS                      31
#define RTC_LOCK_MASK                     (0x1U << 31)
#define RTC_LOCK_SYNC_POS                 30
#define RTC_LOCK_SYNC_MASK                (0x1U << 30)
#define RTC_NC1HZ_POS                     0
#define RTC_NC1HZ_MASK                    (0xFFFFU << 0)


/*******************************************************************************
 * TYPEDEFS
 */
// OM_RTC_Type
typedef struct
{
    __IO uint32_t CR;           // offset:0x00
    __IO uint32_t SR;           // offset:0x04
    __IO uint32_t SAR0;         // offset:0x08
    __IO uint32_t GR;           // offset:0x0C
         uint32_t Reserved1;
    __IO uint32_t SAR1;         // offset:0x14
    __IO uint32_t SAR2;         // offset:0x18
    __IO uint32_t ACCU;         // offset:0x1C
} OM_RTC_Type;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif


#endif  /* __RTC_REG_H */


/** @} */
