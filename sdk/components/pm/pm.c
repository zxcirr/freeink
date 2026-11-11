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
 * @file     pm.c
 * @brief    power manager driver
 * @date     01. April 2020
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


/*******************************************************************************
 * INCLUDES
 */
#include "om.h"
#include "om_driver.h"
#include "trc_io.h"

/*******************************************************************************
 * MACROS
 */
#define PM_SLEEP_STATE_TABLE_SIZE   /*lint -e506 */ (PM_ID_NUM/32 + ((PM_ID_NUM%32) ? 1 : 0))
#define PM_SLEEP_STORE_CALLBACK_NUM 20
#define PM_SLEEP_CHECKER_NUM        10


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    pm_checker_priority_t priority;
    pm_checker_callback_t callback;
} pm_checker_t;

typedef struct {
    bool sleep_enable;
    bool ultra_sleep_enable;

    uint16_t min_sleep_time;

    volatile uint32_t sleep_state[PM_SLEEP_STATE_TABLE_SIZE];
    pm_checker_t checker[PM_SLEEP_CHECKER_NUM];

    pm_sleep_callback_t notify_cb;

    pm_sleep_callback_t store_cb[PM_SLEEP_STORE_CALLBACK_NUM];
} pm_env_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
static pm_env_t pm_env;


/*******************************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief  pm system enter deepsleep
 **/
__RAM_CODE
static void pm_system_enter_deepsleep(void)
{
    SystemEnterDeepSleep();

    // if icache power off in sleep, cache should be re-enable, @ref drv_icache_powerdown_in_sleep_enable()
    drv_icache_enable();
}

/**
 * @brief  pm sleep checker check
 *
 * @return status
 **/
__RAM_CODES("PM")
static pm_status_t pm_sleep_checker_check(void)
{
    int i;
    pm_status_t status = drv_pmu_sleep_status();

    if (status <= PM_STATUS_IDLE)
        return status;

    for(i = 0; i < PM_SLEEP_CHECKER_NUM; ++i) {
        if(pm_env.checker[i].callback != NULL) {
            pm_status_t checker_status = pm_env.checker[i].callback();
            status = OM_MIN(status, checker_status);
            if (status <= PM_STATUS_IDLE) {
                break;
            }
        }
    }

    return status;
}

/**
 * @brief  pm sleep state check
 *
 * @return status
 **/
__RAM_CODES("PM")
static pm_status_t pm_sleep_state_check(void)
{
    int i;
    bool all0 = true;

    for (i=0; i < PM_SLEEP_STATE_TABLE_SIZE; ++i) {
				OM_LOG_DEBUG("");
        if (pm_env.sleep_state[i]) {
            all0 = false;
            break;
        }
    }

    return all0 ? PM_STATUS_DEEP_SLEEP : PM_STATUS_IDLE;
}

/**
 * @brief  system sleep notify
 *
 * @param[in] sleep_state  sleep state
 * @param[in] power_status  power status
 **/
__RAM_CODES("PM")
static void pm_sleep_notify(pm_sleep_state_t sleep_state, pm_status_t power_status)
{
    if (sleep_state != PM_SLEEP_LEAVE_BOTTOM_HALF) {
        for (int i = 0; i < PM_SLEEP_STORE_CALLBACK_NUM; ++i) {
            if (pm_env.store_cb[i])
                pm_env.store_cb[i](sleep_state, power_status);
        }
    }

    if (pm_env.notify_cb) {
        pm_env.notify_cb(sleep_state, power_status);
    }
}

/**
 * @brief  system enter ultar sleep
 **/
static void pm_sleep_enter_ultra_sleep(void)
{
    pm_sleep_notify(PM_SLEEP_STORE, PM_STATUS_DEEP_SLEEP);

    drv_pmu_sleep_enter(PM_STATUS_DEEP_SLEEP, /*lint -e747 reboot*/true);

    while(1);
}

/**
 * @brief  system enter sleep
 **/
__RAM_CODES("PM")
static void pm_sleep_enter_common_sleep(pm_status_t power_status)
{
    pm_sleep_notify(PM_SLEEP_STORE, power_status);

    drv_pmu_sleep_enter(power_status, /*lint -e747 reboot*/false);

    pm_system_enter_deepsleep();

    drv_pmu_sleep_leave(PMU_SLEEP_LEAVE_STEP1_ON_RC32M, power_status);
    pm_sleep_notify(PM_SLEEP_RESTORE_HSI, power_status);

    drv_pmu_sleep_leave(PMU_SLEEP_LEAVE_STEP2_WAIT_XTAL32M, power_status);
    pm_sleep_notify(PM_SLEEP_RESTORE_HSE, power_status);

    drv_pmu_sleep_leave(PMU_SLEEP_LEAVE_STEP3_FINISH, power_status);

    pm_sleep_notify(PM_SLEEP_LEAVE_BOTTOM_HALF, power_status);
}

/**
 * @brief  system enter deep sleep
 **/
static void pm_sleep_enter_deep_sleep(void)
{
    pm_sleep_enter_common_sleep(PM_STATUS_DEEP_SLEEP);
}

/**
 * @brief  system enter deep sleep
 **/
__RAM_CODES("PM")
static void pm_sleep_enter_light_sleep(void)
{
    pm_sleep_enter_common_sleep(PM_STATUS_SLEEP);
}

/*******************************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief  system sleep
 *
 * @param[in] status  status
 **/
__RAM_CODES("PM")
void pm_sleep(pm_status_t status)
{
    /* IRQ has been disabled */

    switch(status) {
        case PM_STATUS_SLEEP:
            TRC_IO(TRC_IO_PM_SLEEP, 1);
            pm_sleep_enter_light_sleep();
            TRC_IO(TRC_IO_PM_SLEEP, 0);
            break;

        case PM_STATUS_DEEP_SLEEP:
            if (pm_env.ultra_sleep_enable) {
                TRC_IO(TRC_IO_PM_ULTRA_SLEEP, 1);
                pm_sleep_enter_ultra_sleep();
                TRC_IO(TRC_IO_PM_ULTRA_SLEEP, 0);
            } else {
                TRC_IO(TRC_IO_PM_DEEP_SLEEP, 1);
                pm_sleep_enter_deep_sleep();
                TRC_IO(TRC_IO_PM_DEEP_SLEEP, 0);
            }
            break;

        case PM_STATUS_IDLE:
            TRC_IO(TRC_IO_PM_IDLE, 1);
            __WFI();
            TRC_IO(TRC_IO_PM_IDLE, 0);
            break;

        default:
            break;
    }
}

/**
 * @brief  system sleep min time set
 *
 * @param[in] time  32k tick
 **/
void pm_sleep_min_time_set(uint16_t tick)
{
    pm_env.min_sleep_time = tick;
}

/**
 *******************************************************************************
 * @brief  pm sleep min time get
 *
 * @return  32k tick
 *******************************************************************************
 */
__RAM_CODES("PM")
uint32_t pm_sleep_min_time_get(void)
{
    return pm_env.min_sleep_time;
}

/**
 *******************************************************************************
 * @brief  pm sleep ultra sleep mode enable
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
void pm_sleep_ultra_sleep_mode_enable(bool enable)
{
    pm_env.ultra_sleep_enable = enable;
}

/**
 *******************************************************************************
 * @brief  pm sleep ultra sleep mode enable
 *
 * @return  is enabled
 *******************************************************************************
 */
bool pm_sleep_ultra_sleep_mode_is_enabled(void)
{
    return pm_env.ultra_sleep_enable;
}

/**
 *******************************************************************************
 * @brief  pm sleep enable
 *
 * @param[in] enable  enable
 *******************************************************************************
 **/
void pm_sleep_enable(bool enable)
{
    pm_env.sleep_enable = enable;
}

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
void pm_sleep_prevent(pm_id_t id)
{
    uint32_t group = (int)id / 32;
    uint32_t mask  = 1u << ((int)id % 32);

    OM_CRITICAL_BEGIN();
    pm_env.sleep_state[group] |= mask;
    OM_CRITICAL_END();
}

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
void pm_sleep_allow(pm_id_t id)
{
    uint32_t group  = (int)id / 32;
    uint32_t mask_n = ~(1u << ((int)id % 32));

    OM_CRITICAL_BEGIN();
    pm_env.sleep_state[group] &= mask_n;
    OM_CRITICAL_END();
}

/**
 *******************************************************************************
 * @brief  pm sleep checker register
 *
 * @param[in] priority  priority
 * @param[in] checker_cb  checker cb
 *******************************************************************************
 **/
void pm_sleep_checker_callback_register(pm_checker_priority_t priority, pm_checker_callback_t checker_cb)
{
    int n, i, j;

    for(n = 0; n < PM_SLEEP_CHECKER_NUM; ++n) {
        if(pm_env.checker[n].callback == NULL) {
            // sort and insert
            for (i = 0; i < n; ++i) {
                // check priority
                if (pm_env.checker[i].priority < priority) {
                    // move
                    for (j = n; j > i; --j) {
                        pm_env.checker[j] = pm_env.checker[j-1];
                    }
                    // insert
                    pm_env.checker[i].priority = priority;
                    pm_env.checker[i].callback = checker_cb;
                    return;
                }
            }

            pm_env.checker[n].priority = priority;
            pm_env.checker[n].callback = checker_cb;
            return;

        } else if (pm_env.checker[n].callback == checker_cb) {
            return;
        }
    }

    OM_ASSERT(0);
}

/**
 * @brief  system sleep notify user callback register
 *
 * @param[in] notify_cb  sleep notify cb
 **/
void pm_sleep_notify_user_callback_register(pm_sleep_callback_t notify_cb)
{
    pm_env.notify_cb = notify_cb;
}

/**
 * @brief  system sleep store restore callback register
 *
 * @param[in] sleep_enter_cb  sleep enter cb
 * @param[in] sleep_leave_cb  sleep leave cb
 **/
void pm_sleep_store_restore_callback_register(pm_sleep_callback_t store_cb)
{
    int i;

    for (i = 0; i < PM_SLEEP_STORE_CALLBACK_NUM; ++i) {
        if (pm_env.store_cb[i] == NULL) {
            pm_env.store_cb[i] = store_cb;
            return;

        } else if (pm_env.store_cb[i] == store_cb) {
            return;
        }
    }

    OM_ASSERT(0);
}

/**
 *******************************************************************************
 * @brief  pm sleep check
 *
 * @return sleep status
 *******************************************************************************
 */
__RAM_CODES("PM")
pm_status_t pm_sleep_check(void)
{
    pm_status_t status;

    // 1st. check pm self sleep state
    status = pm_sleep_state_check();
		//OM_LOG_DEBUG("first check status is %d\n", status);
    // 2nd. check registed sleep checker
    if (status > PM_STATUS_IDLE) {
        TRC_IO(TRC_IO_PM_CHECKER, 1);
        status = pm_sleep_checker_check();
        TRC_IO(TRC_IO_PM_CHECKER, 0);
    }
		//OM_LOG_DEBUG("second check status is %d\n", status);
    return status;
}

/**
 *******************************************************************************
 * @brief  pm power manage
 *******************************************************************************
 **/
__RAM_CODES("PM")
void pm_power_manage(void)
{
    pm_status_t status;

    // 1st. check pm sleep
    status = pm_sleep_check();
		
    // 2th. check sleep enable
    if (!pm_env.sleep_enable) {
        status = OM_MIN(PM_STATUS_IDLE, status);
    }
	
    pm_sleep(status);
}

/**
 *******************************************************************************
 * @brief  pm init
 *******************************************************************************
 */
void pm_init(void)
{
    pm_env.min_sleep_time = PMU_TIMER_MS2TICK(3);

//#ifdef CONFIG_SYSTEM_FROM_ROM_EXIT_DEEPSLEEP_ENABLE
    SystemFromRomExitDeepSleepEnable();
//#endif
}

/**
 *******************************************************************************
 * @brief  dump pm information
 *******************************************************************************
 */
void pm_dump(void *printf_dump_func)
{
    void (*__printf)(const char *format, ...) = /*lint -e{611}*/ (void (*)(const char *format, ...))printf_dump_func;

    if (__printf == NULL) {
        return;
    }
    // pm prevent status
    __printf("[PM] prevent status:");
    for (uint8_t i = 0; i < PM_SLEEP_STATE_TABLE_SIZE; i++) {
        __printf("0x%08X ", pm_env.sleep_state[i]);
    }
    __printf("\n");

    // pm sleep enable
    __printf("[PM] sleep enable:%s  ultra sleep enable:%s\n",
                            pm_env.sleep_enable ? "yes" : "no",
                            pm_env.ultra_sleep_enable ? "yes" : "no"
            );
    // checker
    __printf("[PM] checker:  ");
    for (uint8_t i = 0; i < PM_SLEEP_CHECKER_NUM; i++) {
        if (pm_env.checker[i].callback != NULL) {
            __printf("cb_addr:0x%08X(%d) ", pm_env.checker[i].callback, pm_env.checker[i].priority);
        }
    }
    __printf("\n");
    // store cb
    __printf("[PM] store_cb: ");
    for (uint8_t i = 0; i < PM_SLEEP_STORE_CALLBACK_NUM; i++) {
        if (pm_env.store_cb[i] != NULL) {
            __printf("cb_addr:0x%08X ", pm_env.store_cb[i]);
        }
    }
    __printf("\n");
}

/** @} */
