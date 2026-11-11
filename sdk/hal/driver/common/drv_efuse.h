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
 * @file     drv_efuse.h
 * @brief    Header file of EFUSE HAL module
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup EFUSE EFUSE
 * @ingroup  DRIVER
 * @brief    EFUSE Driver for om662x
 * @details  EFUSE Driver for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 *
 * @example example_efuse.c
 * This is an example of how to write and read efuse
 *
 */

#ifndef __DRV_EFUSE_H
#define __DRV_EFUSE_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_EFUSE)
#include <stdint.h>
#include "om_driver.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
/// Total is 256bits
#define EFUSE_TOTAL_SIZE            32
/// The last 128bit is CP calibration data, don't touch it.
#define EFUSE_USER_SECTION_SIZE     16

/*******************************************************************************
 * TYPEDEFS
 */
/// Program forbid
typedef enum {
    /// not forbid
    DATA_PROG_FORBID_NOT      = 0x00,
    /// program forbid bits form 0   to 127
    DATA_PROG_FORBID_127_0    = 0x40,
    /// program forbid bits form 128 to 253
    DATA_PROG_FORBID_253_128  = 0x80,
    /// program forbid bits form 0   to 253
    DATA_PROG_FORBID_253_0    = 0xC0,
} efuse_program_forbid_t;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief  efuse open
 *******************************************************************************
 **/
void drv_efuse_init(void);

/**
 *******************************************************************************
 * @brief  drv efuse write
 *
 * @note
 * The default value of efuse is 0, and any bit can be changed from 0 to 1,
 * multiple write operations can be performed.
 * The last 128bit is CP calibration data, don't touch it.
 *
 * @param[in] addr  addr
 * @param[in] data  data
 * @param[in] length  length
 *******************************************************************************
 */
void drv_efuse_write(uint32_t addr, const void *data, uint32_t length);

/**
 *******************************************************************************
 * @brief  drv efuse read
 *
 * @param[in] addr  addr
 * @param[in] data  data
 * @param[in] length  length
 *******************************************************************************
 */
void drv_efuse_read(uint32_t addr, void *data, uint32_t length);

/**
 *******************************************************************************
 * @brief  drv efuse iflash encrypt fetch
 *******************************************************************************
 */
void drv_efuse_iflash_encrypt_uid_fetch(void);

#ifdef __cplusplus
}
#endif

#endif  /* RTE_GPIO0 */

#endif  /* __DRV_GPIO_H */


/** @} */

