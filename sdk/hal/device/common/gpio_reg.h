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
 * @file     gpio_reg.h
 * @brief    GPIO Register for OnMicro OM66xx
 * @date     21 July 2020
 * @author   OnMicro SW Team
 *
 * @ingroup  REGS
 * @brief    GPIO Register for OnMicro OM66xx
 * @details  common GPIO Register definitions
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */
#ifndef __GPIO_REG_H
#define __GPIO_REG_H


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


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    __I  uint32_t DATA;                         // offset:0x00
    __IO uint32_t DATAOUT;                      // offset:0x04  (Auto Restore)
         uint32_t RESERVED0[2];
    __IO uint32_t OUTENSET;                     // offset:0x10  (Auto Restore)
    __IO uint32_t OUTENCLR;                     // offset:0x14
    __IO uint32_t ALTFUNCSET;                   // offset:0x18
    __IO uint32_t ALTFUNCCLR;                   // offset:0x1C
    __IO uint32_t INTENSET;                     // offset:0x20  (Auto Restore)
    __IO uint32_t INTENCLR;                     // offset:0x24
    __IO uint32_t INTTYPESET;                   // offset:0x28  (Auto Restore)
    __IO uint32_t INTTYPECLR;                   // offset:0x2C
    __IO uint32_t INTPOLSET;                    // offset:0x30  (Auto Restore)
    __IO uint32_t INTPOLCLR;                    // offset:0x34
    __IO uint32_t INTSTATUS;                    // offset:0x38
         uint32_t RESERVED1;
    __IO uint32_t INTBOTHSET;                   // offset:0x40  (Auto Restore)
    __IO uint32_t INTBOTHCLR;                   // offset:0x44
         uint8_t  RESERVED3[0x1000-0x0048];

    __IO uint32_t MASK_0_7[0x100];              // offset:0x1000
    __IO uint32_t MASK_8_15[0x100];             // offset:0x1400
    __IO uint32_t MASK_16_23[0x100];            // offset:0x1800
    __IO uint32_t MASK_24_31[0x100];            // offset:0x1C00
} OM_GPIO_Type;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif


#endif  /* __GPIO_REG_H */


/** @} */

