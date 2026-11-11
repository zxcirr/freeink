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
 * @file     pm.h
 * @brief    Power Manager driver
 * @date     01. April 2020
 * @author   OnMicro SW Team
 *
 * @defgroup PM Power Manager
 * @ingroup  COMPONENTS
 * @brief    Power Manager Driver
 * @details  Power Manager Driver
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __PM_H
#define __PM_H


/*******************************************************************************
 * INCLUDES
 */
#include "om.h"


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
/// PM ID defined
typedef enum {
    PM_ID_BLE,      ///< Bluetooth
    PM_ID_24G,      ///< 2.4G
    PM_ID_GPADC,    ///< GPADC
    PM_ID_RTC,      ///< RTC
    PM_ID_WDT,      ///< Watch dag
    PM_ID_DMA,      ///< DMA
    PM_ID_TIM0,     ///< TIM0
    PM_ID_TIM1,     ///< TIM1
    PM_ID_TIM2,     ///< TIM2
    PM_ID_USART0,   ///< USART0
    PM_ID_USART1,   ///< USART1
    PM_ID_I2C0,     ///< I2C0
    PM_ID_I2C1,     ///< I2C1
    PM_ID_I2S0,     ///< I2S0
    PM_ID_SPI0,     ///< SPI0
    PM_ID_SPI1,     ///< SPI1

    PM_ID_SF0,      ///< SF0
    PM_ID_SF1,      ///< SF0
    PM_ID_SF2,      ///< SF0

    PM_ID_TEST,     ///< TEST
    PM_ID_SHELL,    ///< SHELL
    PM_ID_USER0 = 32+28, ///< For user 0
    PM_ID_USER1 = 32+29, ///< For user 1
    PM_ID_USER2 = 32+30, ///< For user 2
    PM_ID_USER3 = 32+31, ///< For user 3
    PM_ID_NUM,           ///< Number of PM ID
} pm_id_t;

/// PM checker priority
typedef enum {
    PM_CHECKER_PRIORITY_LOWEST,     ///< Lowest
    PM_CHECKER_PRIORITY_LOW,        ///< Low
    PM_CHECKER_PRIORITY_NORMAL,     ///< Normal
    PM_CHECKER_PRIORITY_HIGH,       ///< High
    PM_CHECKER_PRIORITY_HIGHEST,    ///< Highest
} pm_checker_priority_t;

/// PM status
typedef enum {
    /// All modules are alive, and CPU run with full speed
    PM_STATUS_ACTIVE,
    /// All modules are alive, but CPU clock is gating
    PM_STATUS_IDLE,
    /// Power down most of module(CPU, Peripheral, etc),
    /// but 32K is alive, only gpio and sleep-timer can wake up chip
    PM_STATUS_SLEEP,
    /// Power down most of module(CPU, Peripheral, etc),
    /// 32K is not alive, only gpio can wake up chip
    PM_STATUS_DEEP_SLEEP,
} pm_status_t;

/// Sleep state
typedef enum {
    /// Event with sleep entering.
    /// The output of IO can be controlled.
    PM_SLEEP_ENTRY,
    /// Event with sleep leaving, CHIP is working on 'High Speed Internal Clock (RC32M)'.
    /// The output of IO can not be controlled.
    PM_SLEEP_LEAVE_TOP_HALF_HSI,
    /// Event with sleep leaving, CHIP is working on 'High Speed External Clock (XTAL32M)'.
    /// The output of IO can not be controlled.
    PM_SLEEP_LEAVE_TOP_HALF,
    /// Event with sleep leaving, CHIP is working on 'High Speed External Clock (XTAL32M)'.
    /// The output of IO can be controlled.
    PM_SLEEP_LEAVE_BOTTOM_HALF,

    /// Alias for PM_SLEEP_ENTRY, just for driver REG store
    PM_SLEEP_STORE = PM_SLEEP_ENTRY,
    /// Alias for PM_SLEEP_LEAVE_TOP_HALF_HSI, just for driver REG restore
    PM_SLEEP_RESTORE_HSI = PM_SLEEP_LEAVE_TOP_HALF_HSI,
    /// Alias for PM_SLEEP_LEAVE_TOP_HALF, just for driver REG restore
    PM_SLEEP_RESTORE_HSE = PM_SLEEP_LEAVE_TOP_HALF,
} pm_sleep_state_t;

/**
 *******************************************************************************
 * @brief  pm checker callback t
 *
 * @return @ref pm_status_t
 *******************************************************************************
 **/
typedef pm_status_t (*pm_checker_callback_t) (void);

/**
 *******************************************************************************
 * @brief sleep state event callback
 *
 * @param[in] sleep_state  current sleep state @ref pm_sleep_state_t
 * @param[in] power_status  power status, only PM_STATUS_SLEEP and PM_STATUS_DEEP_SLEEP are valid @ref pm_status_t
 *******************************************************************************
 **/
typedef void (*pm_sleep_callback_t)(pm_sleep_state_t sleep_state, pm_status_t power_status);


/*******************************************************************************
 * EXTERN FUNCTIONS
 */

/**
 *******************************************************************************
 * @brief  system sleep min time set
 *
 * @param[in] time  32k tick
 *******************************************************************************
 **/
extern void pm_sleep_min_time_set(uint16_t time);

/**
 *******************************************************************************
 * @brief  pm sleep min time get
 *
 * @return  32k tick
 *******************************************************************************
 */
extern uint32_t pm_sleep_min_time_get(void);

/**
 *******************************************************************************
 * @brief  pm sleep ultra sleep mode enable
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
extern void pm_sleep_ultra_sleep_mode_enable(bool enable);

/**
 *******************************************************************************
 * @brief  pm sleep ultra sleep mode enable
 *
 * @return  is enabled
 *******************************************************************************
 */
extern bool pm_sleep_ultra_sleep_mode_is_enabled(void);

/**
 *******************************************************************************
 * @brief  pm sleep enable or disable
 *
 * @param[in] enable  enable
 *******************************************************************************
 **/
extern void pm_sleep_enable(bool enable);

/**
 *******************************************************************************
 * @brief  pm sleep prevent
 *
 * @param[in] id  id, reference @ref pm_id_t
 *                If I2C0 need to prevent system into sleep mode, id = PM_ID_I2C0;
 *                If I2C2 need to prevent system into sleep mode, id = PM_ID_I2C2;
 *                ...
 *
 *******************************************************************************
 **/
extern void pm_sleep_prevent(pm_id_t id);

/**
 *******************************************************************************
 * @brief  pm sleep allow
 *
 * @param[in] id  id, reference @ref pm_id_t
 *                If I2C0 need to allow system into sleep mode, id = PM_ID_I2C0;
 *                If I2C2 need to allow system into sleep mode, id = PM_ID_I2C2;
 *                ...
 *
 *******************************************************************************
 **/
extern void pm_sleep_allow(pm_id_t id);

/**
 *******************************************************************************
 * @brief  pm sleep checker register
 *
 * @param[in] priority  priority
 * @param[in] checker_cb  checker cb
 *******************************************************************************
 **/
extern void pm_sleep_checker_callback_register(pm_checker_priority_t priority, pm_checker_callback_t checker_cb);

/**
 *******************************************************************************
 * @brief  system sleep notify user callback register
 *
 * @param[in] notify_cb  sleep notify cb
 *******************************************************************************
 **/
extern void pm_sleep_notify_user_callback_register(pm_sleep_callback_t notify_cb);

/**
 *******************************************************************************
 * @brief  system sleep store restore callback register
 *
 * @param[in] store_cb  sleep callback
 *******************************************************************************
 **/
extern void pm_sleep_store_restore_callback_register(pm_sleep_callback_t store_cb);

/**
 *******************************************************************************
 * @brief  pm sleep check
 *
 * @return sleep status
 *******************************************************************************
 */
extern pm_status_t pm_sleep_check(void);

/**
 *******************************************************************************
 * @brief  system sleep
 *
 * @param[in] status  status
 *******************************************************************************
 **/
extern void pm_sleep(pm_status_t status);

/**
 *******************************************************************************
 * @brief  pm power manage
 *******************************************************************************
 **/
extern void pm_power_manage(void);

/**
 *******************************************************************************
 * @brief  pm init
 *******************************************************************************
 */
extern void pm_init(void);

/**
 *******************************************************************************
 * @brief  dump pm information
 *******************************************************************************
 */
extern void pm_dump(void *printf_dump_func);


#ifdef  __cplusplus
}
#endif

#endif  /* __PM_H */


/** @} */
