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
 * DISCLAIMED. IN NO EVENT AESLL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * -------------------------------------------------------------------------- */

/**
 * @file     drv_aes_hw.h
 * @brief    Header file of AES HAL module
 * @date     15 December 2021
 * @author   OnMicro SW Team
 *
 * @defgroup AES AES
 * @ingroup  DRIVER
 * @brief    AES256 Driver for om662x
 * @details  AES256 Driver for om662x

 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 *
 * @example example_aes.c
 * This is an example of how to use the aes
 *
 */

#ifndef __DRV_AES_HW_H
#define __DRV_AES_HW_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_AES_HW)
#include <stddef.h>
#include <stdint.h>

#ifdef  __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief  aes128 encrypt
 *
 * @param[in] key  key 16
 * @param[in] input  input 16
 * @param[out] output  output 16
 *******************************************************************************
 */
extern void drv_aes128_hw_encrypt(const uint8_t key[16], const uint8_t input[16], uint8_t output[16]);

/**
 *******************************************************************************
 * @brief  aes256 encrypt
 *
 * @param[in] key  key 32
 * @param[in] input  input 16
 * @param[out] output  output 16
 *******************************************************************************
 */
extern void drv_aes256_hw_encrypt(const uint8_t key[32], const uint8_t input[16], uint8_t output[16]);


#ifdef  __cplusplus
}
#endif

#endif  /* RTE_AES_HW */

#endif /* __DRV_AES_HW_H */


/** @} */

