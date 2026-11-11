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
 * @file     drv_wdt.h
 * @brief
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup WDT WDT
 * @ingroup  DRIVER
 * @brief    Watch Dog Timer Driver for om662x
 * @details  Watch Dog Timer Driver for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 *
 * @example example_wdt.c
 * This is an example of how to use wdt
 */

#ifndef __DRV_WDT_H
#define __DRV_WDT_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_WDT)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"

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
 * @brief wdt register callback
 *
 * @param[in] isr_cb         callback
 **/
extern void drv_wdt_register_isr_callback(drv_isr_callback_t isr_cb);

/**
 *******************************************************************************
 * @brief Kick watchdog
 *******************************************************************************
 */
extern void drv_wdt_keep_alive(void);

/**
 *******************************************************************************
 * @brief wdt interrupt service routine
 *
 *******************************************************************************
 */
extern void drv_wdt_isr(void);

/**
 *******************************************************************************
 * @brief Watchdog initialization, enable APB clock, and would not clear reset flag
 *
 * @param[in] timeout_ms     Timeout in unit ms, range in [0, 8191992] ms
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_wdt_init(uint32_t timeout_ms)
{
    extern void drv_wdt_enable(uint32_t timeout_ms);
    drv_wdt_enable(timeout_ms);

    if (timeout_ms == 0U) {
        while(OM_PMU->WDT_STATUS & PMU_WDT_STATUS_LD_WDT_KR_STATUS_MASK);
        OM_PMU->WDT_KR_CFG = 0x6666;
    }
}

#ifdef __cplusplus
}
#endif

#endif  /* RTE_WDT */

#endif  /* __DRV_WDT_H */


/** @} */
