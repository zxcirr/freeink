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
 * @file     drv_cache.h
 * @brief    Header file of CACHE HAL module
 * @date     15 December 2021
 * @author   OnMicro SW Team
 *
 * @defgroup CACHE CACHE
 * @ingroup  DRIVER
 * @brief    CACHE Driver for om662x
 * @details  CACHE Driver for om662x

 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __DRV_CACHE_HW_H
#define __DRV_CACHE_HW_H


/*******************************************************************************
 * INCLUDES
 */
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
 * @brief  drv icache enable
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_icache_enable(void)
{
    OM_ICACHE->CONFIG = 0;
    OM_ICACHE->CTRL = ICACHE_CTRL_CEN_MASK;
    while((OM_ICACHE->STATUS & ICACHE_STATUS_CSTS_MASK) == 0);
}

/**
 *******************************************************************************
 * @brief  drv icache disable
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_icache_disable(void)
{
    OM_ICACHE->CONFIG = ICACHE_CONFIG_GCLKDIS_MASK;
    OM_ICACHE->CTRL = 0;
    while((OM_ICACHE->STATUS & ICACHE_STATUS_CSTS_MASK) != 0);
}

/**
 *******************************************************************************
 * @brief  drv icache invalidate
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_icache_invalidate(void)
{
    OM_ICACHE->MAINT0 = ICACHE_MAINT0_INVALL_MASK;
}

/**
 *******************************************************************************
 * @brief  drv icache power off in sleep
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_icache_powerdown_in_sleep_enable(bool enable)
{
    REGW(&OM_PMU->PSO_PM, MASK_1REG(PMU_PSO_PM_ICACHE_POWER_ON, enable?0:1));
}

#ifdef  __cplusplus
}
#endif

#endif /* __DRV_AES_HW_H */


/** @} */

