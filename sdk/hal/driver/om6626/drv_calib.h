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
 * @file     drv_calib.h
 * @brief    Header file of CALIB HAL module
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup CALIB CALIB
 * @ingroup  DRIVER
 * @brief    CALIB Driver for om662x
 * @details  CALIB Driver for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __DRV_CALIB_H
#define __DRV_CALIB_H


/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>


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


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief  calib rc32k accuracy check
 *
 * @param[in] win_32k_num  win 32k num
 *
 * @return ppm
 *******************************************************************************
 **/
extern int drv_calib_rc32k_accuracy_check(uint32_t win_32k_num);

/**
 *******************************************************************************
 * @brief calib rc32k
 *******************************************************************************
 **/
extern void drv_calib_rc32k(void);

/**
 *******************************************************************************
 * @brief calib sys rc32m
 *******************************************************************************
 **/
extern void drv_calib_sys_rc32m(void);

/**
 *******************************************************************************
 * @brief calib sys rc
 *******************************************************************************
 **/
extern void drv_calib_sys_rc(void);

/*
 *******************************************************************************
 * @brief calib rf
 *******************************************************************************
 **/
extern void drv_calib_rf(void);

/**
 *******************************************************************************
 * @brief  drv calib rf restore
 *******************************************************************************
 */
void drv_calib_rf_restore(void);

/**
 *******************************************************************************
 * @brief  drv calib sys restore
 *******************************************************************************
 */
void drv_calib_sys_restore(void);

#ifdef __cplusplus
}
#endif

#endif  /* __DRV_CALIB_H */


/** @} */

