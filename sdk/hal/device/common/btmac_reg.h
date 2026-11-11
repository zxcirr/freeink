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
 * @file     btmac_reg.h
 * @brief    BTMAC Register for OnMicro OM66xx
 * @date     21 July 2020
 * @author   OnMicro SW Team
 *
 * @ingroup  REGS
 * @brief    BTMAC Register for OnMicro OM66xx
 * @details  common BTMAC Register definitions
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */
#ifndef __BTMAC_REG_H
#define __BTMAC_REG_H


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
         uint32_t RESERVE1[136];
    __IO uint32_t EM_CLOCK_EN;                // offset: 0x220
} OM_BTMAC_Type;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif


#endif  /* __BTMAC_REG_H */

/** @} */

