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
 * @file     aes_hw_reg.h
 * @brief    AES Register for OnMicro OM66xx
 * @date     21 July 2020
 * @author   OnMicro SW Team
 *
 * @ingroup  REGS
 * @brief    AES Register for OnMicro OM66xx
 * @details  common AES Register definitions
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */
#ifndef __AES_HW_REG_H
#define __AES_HW_REG_H


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
// FLUSH  (E8h)
#define AES_HW_CTRL_START_POS          0
#define AES_HW_CTRL_MODE_POS           1
#define AES_HW_CTRL_DONE_POS           4
#define AES_HW_CTRL_INTR_EN_POS        8
#define AES_HW_CTRL_START_MASK         0x00000001
#define AES_HW_CTRL_MODE_MASK          0x00000002
#define AES_HW_CTRL_DONE_MASK          0x00000010
#define AES_HW_CTRL_INTR_EN_MASK       0x00000100


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    __IO uint32_t CTRL;         // offset: 0x000
         uint32_t RESERVED[63];
    __IO uint32_t KEY[8];       // offset: 0x100
    __IO uint32_t DATA[4];      // offset: 0x120
} OM_AES_HW_Type;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif


#endif  /* __AES_HW_REG_H */


/** @} */

