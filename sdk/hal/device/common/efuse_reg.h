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
 * @file     efuse_reg.h
 * @brief    EFUSE Register for OnMicro OM66xx
 * @date     24 November 2021
 * @author   OnMicro SW Team
 *
 * @ingroup  REGS
 * @brief    EFUSE Register for OnMicro OM66xx
 * @details  common EFUSE Register definitions
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */
#ifndef __EFUSE_REG_H
#define __EFUSE_REG_H


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
// EFUSE program enable PROGRAM_ENABLE (offset 0xF00)
#define EFUSE_PROGRAM_ENABLE_POS                         0
#define EFUSE_PROGRAM_ENABLE_MASK                       (1U << 0)

// EFUSE program start PROGRAM_START (offset 0xF04)
#define EFUSE_PROGRAM_START_POS                          0U
#define EFUSE_PROGRAM_START_MASK                        (1U << 0)

// EFUSE avdd selected AVDD_ENABLE_SELECT (offset 0xF08)
#define EFUSE_AVDD_TIMING_NS                            (10 * 1000)

// EFUSE program address PROGRAM_ADDRESS (offset 0xF0C)
#define EFUSE_PROGRAM_ADDRESS_POS                        0U
#define EFUSE_PROGRAM_ADDRESS_MASK                       0x1F

// EFUSE program config0 PROGRAM_CFG0 (offset 0xF18)
#define EFUSE_PROGRAM_CFG0_L16_NS                        1000U
#define EFUSE_PROGRAM_CFG0_H16_NS                        100U

// EFUSE program config1 PROGRAM_CFG1 (offset 0xF1C)
#define EFUSE_PROGRAM_CFG1_L16_NS                        11900U
#define EFUSE_PROGRAM_CFG1_H16_NS                        10000U

// EFUSE program config2 PROGRAM_CFG2 (offset 0xF20)
#define EFUSE_PROGRAM_CFG2_L16_NS                        50U

// EFUSE read config READ_CFG (offset 0xF24)
#define EFUSE_READ_CFG_L8_NS                             40U
#define EFUSE_READ_CFG_L16_NS                            75U
#define EFUSE_READ_CFG_H24_NS                            10U
#define EFUSE_READ_CFG_H32_NS                            100U
#define EFUSE_READ_CFG_T_RD_POS                          0U
#define EFUSE_READ_CFG_T_AEN_RD_POS                      8U
#define EFUSE_READ_CFG_T_SR_A_POS                        16U
#define EFUSE_READ_CFG_T_SR_RD_POS                       24U

// EFUSE program interrupt PROGRAM_INTR (offset 0xF28)
#define EFUSE_PROGRAM_INTR_DISABLE                       0U
#define EFUSE_PROGRAM_INTR_ENABLE                       (1U << 0)
#define EFUSE_PROGRAM_INTR_CLR_POS                       1U
#define EFUSE_PROGRAM_INTR_CLR_MASK                     (1U << 1)
#define EFUSE_PROGRAM_INTR_POS                           2U
#define EFUSE_PROGRAM_INTR_MASK                         (1U << 2)

// EFUSE status STATUS (offset 0xF2C)
#define EFUSE_STATUS_CTRL_STATE_POS                      0U
#define EFUSE_STATUS_CTRL_STATE_MASK                    (1U << 0)
#define EFUSE_STATUS_PROGRAM_EN_POS                      1U
#define EFUSE_STATUS_PROGRAM_EN_MASK                    (1U << 1)
#define EFUSE_STATUS_ERROR_SET_PROG_POS                  2U
#define EFUSE_STATUS_ERROR_SET_PROG_MASK                (1U << 2)
#define EFUSE_STATUS_ERROR_SET_READ_POS                  3U
#define EFUSE_STATUS_ERROR_SET_READ_MASK                (1U << 3)
#define EFUSE_STATUS_CTRL_STATE_DEBUG_POS                8U
#define EFUSE_STATUS_CTRL_STATE_DEBUG_MASK              (0xFU << 8)

// CTRL
#define EFUSE_CTRL_PMU_PROG_FORBID_POS                  2
#define EFUSE_CTRL_PMU_PROG_FORBID_MASK                 (0x3U << 2)
#define EFUSE_CTRL_UID_READ_POS                         1
#define EFUSE_CTRL_UID_READ_MASK                        (0x1U << 1)
#define EFUSE_CTRL_PROGRAM_FORBID_POS                   0
#define EFUSE_CTRL_PROGRAM_FORBID_MASK                  (0x1U << 0)


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    __I  uint8_t   READ_DATA[32];   // offset:0x00
         uint8_t   RSVD[0xF00-32];
    __IO uint32_t  PROGRAM_ENABLE;  // offset:0xF00
    __IO uint32_t  PROGRAM_START;   // offset:0xF04
    __IO uint32_t  AVDD_TIMING_CFG; // offset:0xF08
    __IO uint32_t  PROGRAM_ADDRESS; // offset:0xF0C
    __IO uint32_t  PROGRAM_DATA;    // offset:0xF10
    __IO uint32_t  CTRL;            // offset:0xF14
    __IO uint32_t  PROGRAM_CFG0;    // offset:0xF18
    __IO uint32_t  PROGRAM_CFG1;    // offset:0xF1C
    __IO uint32_t  PROGRAM_CFG2;    // offset:0xF20
    __IO uint32_t  READ_CFG;        // offset:0xF24
    __IO uint32_t  PROGRAM_INTR;    // offset:0xF28
    __I  uint32_t  STATUS;          // offset:0xF2C
} OM_EFUSE_Type;

/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif


#endif  /* __EFUSE_REG_H */


/** @} */

