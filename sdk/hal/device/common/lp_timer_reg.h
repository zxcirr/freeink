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
 * @file     lp_timer_reg.h
 * @brief    register file for lptimer
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __LP_TIMER_REG_H
#define __LP_TIMER_REG_H


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
// LPTIMER_EN
#define LP_TIM_EN_POS                                           0
#define LP_TIM_EN_MASK                                          (0x1U << 0)

// LPTIMER_CTRL
#define LP_TIM_CTRL_CNTPRESC_POS                                12
#define LP_TIM_CTRL_CNTPRESC_MASK                               (0xfU << 12)
#define LP_TIM_CTRL_CNTTOPEN_POS                                9
#define LP_TIM_CTRL_CNTTOPEN_MASK                               (0x1U << 9)
#define LP_TIM_CTRL_BUFTOP_POS                                  8
#define LP_TIM_CTRL_BUFTOP_MASK                                 (0x1U << 8)
#define LP_TIM_CTRL_OPOL1_POS                                   7
#define LP_TIM_CTRL_OPOL1_MASK                                  (0x1U << 7)
#define LP_TIM_CTRL_OPOL0_POS                                   6
#define LP_TIM_CTRL_OPOL0_MASK                                  (0x1U << 6)
#define LP_TIM_CTRL_UFOA1_POS                                   4
#define LP_TIM_CTRL_UFOA1_MASK                                  (0x3U << 4)
#define LP_TIM_CTRL_UFOA0_POS                                   2
#define LP_TIM_CTRL_UFOA0_MASK                                  (0x3U << 2)
#define LP_TIM_CTRL_REPMODE_POS                                 0
#define LP_TIM_CTRL_REPMODE_MASK                                (0x3U << 0)

// LPTIMER_CMD
#define LP_TIM_CMD_CTO1_POS                                     4
#define LP_TIM_CMD_CTO1_MASK                                    (0x1U << 4)
#define LP_TIM_CMD_CTO0_POS                                     3
#define LP_TIM_CMD_CTO0_MASK                                    (0x1U << 3)
#define LP_TIM_CMD_CLEAR_POS                                    2
#define LP_TIM_CMD_CLEAR_MASK                                   (0x1U << 2)
#define LP_TIM_CMD_STOP_POS                                     1
#define LP_TIM_CMD_STOP_MASK                                    (0x1U << 1)
#define LP_TIM_CMD_START_POS                                    0
#define LP_TIM_CMD_START_MASK                                   (0x1U << 0)

// LPTIMER_STAT
#define LP_TIM_STAT_RUNNING_POS                                 0
#define LP_TIM_STAT_RUNNING_MASK                                (0x1U << 0)

// LPTIMER_CNT
#define LP_TIM_CNT_POS                                          0
#define LP_TIM_CNT_MASK                                         (0xffffU << 0)

// LPTIMER_COMP0
#define LP_TIM_COMP0_POS                                        0
#define LP_TIM_COMP0_MASK                                       (0xffffU << 0)

// LPTIMER_COMP1
#define LP_TIM_COMP1_POS                                        0
#define LP_TIM_COMP1_MASK                                       (0xffffU << 0)

// LPTIMER_TOP
#define LP_TIM_TOP_POS                                          0
#define LP_TIM_TOP_MASK                                         (0xffffU << 0)

// LPTIMER_TOPBUFF
#define LP_TIM_TOPBUFF_POS                                      0
#define LP_TIM_TOPBUFF_MASK                                     (0xffffU << 0)

// LPTIMER_REP0
#define LP_TIM_REP0_POS                                         0
#define LP_TIM_REP0_MASK                                        (0xffU << 0)

// LPTIMER_REP1
#define LP_TIM_REP1_POS                                         0
#define LP_TIM_REP1_MASK                                        (0xffU << 0)

// LPTIMER_INTF
#define LP_TIM_INTF_REP1_FLG_POS                                4
#define LP_TIM_INTF_REP1_FLG_MASK                               (0x1U << 4)
#define LP_TIM_INTF_REP0_FLG_POS                                3
#define LP_TIM_INTF_REP0_FLG_MASK                               (0x1U << 3)
#define LP_TIM_INTF_UF_FLG_POS                                  2
#define LP_TIM_INTF_UF_FLG_MASK                                 (0x1U << 2)
#define LP_TIM_INTF_COMP1_FLG_POS                               1
#define LP_TIM_INTF_COMP1_FLG_MASK                              (0x1U << 1)
#define LP_TIM_INTF_COMP0_FLG_POS                               0
#define LP_TIM_INTF_COMP0_FLG_MASK                              (0x1U << 0)
#define LP_TIM_INTF_ALL_POS                                     0
#define LP_TIM_INTF_ALL_MASK                                    (0x1FU << 0)

// LPTIMER_INTE
#define LP_TIM_INTE_REP1_EN_POS                                 4
#define LP_TIM_INTE_REP1_EN_MASK                                (0x1U << 4)
#define LP_TIM_INTE_REP0_EN_POS                                 3
#define LP_TIM_INTE_REP0_EN_MASK                                (0x1U << 3)
#define LP_TIM_INTE_UF_EN_POS                                   2
#define LP_TIM_INTE_UF_EN_MASK                                  (0x1U << 2)
#define LP_TIM_INTE_COMP1_EN_POS                                1
#define LP_TIM_INTE_COMP1_EN_MASK                               (0x1U << 1)
#define LP_TIM_INTE_COMP0_EN_POS                                0
#define LP_TIM_INTE_COMP0_EN_MASK                               (0x1U << 0)

// LPTIMER_SYNCBUSY
#define LP_TIM_SYNCBUSY_CTO1_POS                                8
#define LP_TIM_SYNCBUSY_CTO1_MASK                               (0x1U << 8)
#define LP_TIM_SYNCBUSY_CTO0_POS                                7
#define LP_TIM_SYNCBUSY_CTO0_MASK                               (0x1U << 7)
#define LP_TIM_SYNCBUSY_CLEAR_POS                               6
#define LP_TIM_SYNCBUSY_CLEAR_MASK                              (0x1U << 6)
#define LP_TIM_SYNCBUSY_STOP_POS                                5
#define LP_TIM_SYNCBUSY_STOP_MASK                               (0x1U << 5)
#define LP_TIM_SYNCBUSY_START_POS                               4
#define LP_TIM_SYNCBUSY_START_MASK                              (0x1U << 4)
#define LP_TIM_SYNCBUSY_REP1_BUSY_POS                           3
#define LP_TIM_SYNCBUSY_REP1_BUSY_MASK                          (0x1U << 3)
#define LP_TIM_SYNCBUSY_REP0_BUSY_POS                           2
#define LP_TIM_SYNCBUSY_REP0_BUSY_MASK                          (0x1U << 2)
#define LP_TIM_SYNCBUSY_TOP_BUSY_POS                            1
#define LP_TIM_SYNCBUSY_TOP_BUSY_MASK                           (0x1U << 1)
#define LP_TIM_SYNCBUSY_CNT_BUSY_POS                            0
#define LP_TIM_SYNCBUSY_CNT_BUSY_MASK                           (0x1U << 0)


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    __IO uint32_t EN;                   // offset: 0x00
    __IO uint32_t CTRL;                 // offset: 0x04
    __IO uint32_t CMD;                  // offset: 0x08
    __I  uint32_t STAT;                 // offset: 0x0C
    __IO uint32_t CNT;                  // offset: 0x10
    __IO uint32_t COMP0;                // offset: 0x14
    __IO uint32_t COMP1;                // offset: 0x18
    __IO uint32_t TOP;                  // offset: 0x1C
    __IO uint32_t TOPBUFF;              // offset: 0x20
    __IO uint32_t REP0;                 // offset: 0x24
    __IO uint32_t REP1;                 // offset: 0x28
    __IO uint32_t INTF;                 // offset: 0x2C
    __IO uint32_t INTE;                 // offset: 0x30
    __I  uint32_t SYNCBUSY;             // offset: 0x34
} OM_LP_TIM_Type;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif

#endif  /* __LP_TIMER_REG_H */


/** @} */
