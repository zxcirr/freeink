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
 * @file     uart_ex_reg.h
 * @brief
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __UART_EX_REG_H
#define __UART_EX_REG_H


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
// con
#define UART_EX_CON_TX_INT_EN_POS                           9
#define UART_EX_CON_TX_INT_EN_MASK                          (0x1U << 9)
#define UART_EX_CON_RX_INT_EN_POS                           8
#define UART_EX_CON_RX_INT_EN_MASK                          (0x1U << 8)
#define UART_EX_CON_MODE_POS                                6
#define UART_EX_CON_MODE_MASK                               (0x3U << 6)
#define UART_EX_CON_MC_EN_POS                               5
#define UART_EX_CON_MC_EN_MASK                              (0x1U << 5)
#define UART_EX_CON_SRX_EN_POS                              4
#define UART_EX_CON_SRX_EN_MASK                             (0x1U << 4)
#define UART_EX_CON_TB8_POS                                 3
#define UART_EX_CON_TB8_MASK                                (0x1U << 3)
#define UART_EX_CON_RB8_POS                                 2
#define UART_EX_CON_RB8_MASK                                (0x1U << 2)
#define UART_EX_CON_TX_COMPLETED_POS                        1
#define UART_EX_CON_TX_COMPLETED_MASK                       (0x1U << 1)
#define UART_EX_CON_RX_COMPLETED_POS                        0
#define UART_EX_CON_RX_COMPLETED_MASK                       (0x1U << 0)

// RELL
#define UART_EX_RELL_TIML_POS                               0
#define UART_EX_RELL_TIML_MASK                              (0xFFU << 0)

// RELH
#define UART_EX_RELL_TIMH_POS                               0
#define UART_EX_RELL_TIMH_MASK                              (0xFFU << 0)


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    __IO uint32_t CON;          // offset : 0x00
    union {
        __I uint32_t RBR;
        __O uint32_t THR;
    };                          // offset : 0x04
    __IO uint32_t RELL;         // offset : 0x08
    __IO uint32_t RELH;         // offset : 0x0C
} OM_USART_EX_Type;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif

#endif  /* __UART_EX_REG_H */


/** @} */
