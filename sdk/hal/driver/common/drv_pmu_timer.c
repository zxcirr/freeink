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
 * @file     pmu_timer.c
 * @brief    the driver of pmu_timer
 * @date     3 Feb 2023
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
#include "RTE_OM662x.h"
#if (RTE_PMU_TIMER)
#include "om_device.h"
#include "om_driver.h"

/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * CONST & VARIABLES
 */
pmu_timer_env_t pmu_timer_env = {
    .isr0_cb = NULL,
    .isr1_cb = NULL,
};

static const drv_resource_t pmu_timer_resource = {
    .cap      = 0U,
    .reg      = OM_PMU,
    .env      = &pmu_timer_env,
    .irq_num  = PMU_TIMER_IRQn,
    .irq_prio = RTE_PMU_TIMER_IRQ_PRIORITY,
};

/*******************************************************************************
 * LOCAL FUNCTIONS
 */

#if (RTE_PM)
/**
 *******************************************************************************
 * @brief  pmu timer pm checker handler
 *
 * @return status
 *******************************************************************************
 */
__RAM_CODES("PM")
static pm_status_t pmu_timer_pm_checker_handler(void)
{
    pm_status_t status = PM_STATUS_DEEP_SLEEP;

    if (OM_PMU->TIMER_CTRL & PMU_TIMER_CTRL_PMU_TIMER_INT_VAL0_EN_MASK) {
        if (drv_pmu_timer_left_time_get(PMU_TIMER_TRIG_VAL0) < pm_sleep_min_time_get()) {
            return PM_STATUS_IDLE;
        } else {
            status = PM_STATUS_SLEEP;
        }
    }

    if (OM_PMU->TIMER_CTRL & PMU_TIMER_CTRL_PMU_TIMER_INT_VAL1_EN_MASK) {
        if (drv_pmu_timer_left_time_get(PMU_TIMER_TRIG_VAL1) < pm_sleep_min_time_get()) {
            return PM_STATUS_IDLE;
        } else {
            status = PM_STATUS_SLEEP;
        }
    }

    return status;
}
#endif

/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief initialize pmu_timer
 *
 * @return None
 *******************************************************************************
 */
void drv_pmu_timer_init(void)
{
    OM_PMU_Type *pmu_timer;

    pmu_timer = (OM_PMU_Type *)pmu_timer_resource.reg;

    // Clear and Enable pmu timer IRQ
    NVIC_ClearPendingIRQ(pmu_timer_resource.irq_num);
    NVIC_SetPriority(pmu_timer_resource.irq_num, pmu_timer_resource.irq_prio);
    NVIC_EnableIRQ(pmu_timer_resource.irq_num);

    // start count
    register_set1(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_EN_MASK);

    #if (RTE_PM)
    pm_sleep_checker_callback_register(PM_CHECKER_PRIORITY_NORMAL, pmu_timer_pm_checker_handler);
    #endif  /* (RTE_PM) */
}

#if (RTE_PMU_TIMER_REGISTER_CALLBACK)
/**
 *******************************************************************************
 * @brief pmu timer register callback
 *
 * @param[in] cb    callback
 *
 * @return None
 *******************************************************************************
 */
void drv_pmu_timer_register_isr_callback(pmu_timer_trig_t trig_type, drv_isr_callback_t cb)
{
    pmu_timer_env_t *env;

    env = (pmu_timer_env_t *)(pmu_timer_resource.env);

    if (trig_type == PMU_TIMER_TRIG_VAL0) {
        env->isr0_cb = cb;
    } else if (trig_type == PMU_TIMER_TRIG_VAL1) {
        env->isr1_cb = cb;
    }
}
#endif

__WEAK void drv_pmu_timer_isr_callback(OM_PMU_Type *om_pmu_timer, pmu_timer_trig_t trig_type, drv_event_t event)
{
#ifdef PMU_TIMER_DEBUG
    pmu_timer_env.record[trig_type].cur_cnt_when_overflow = drv_pmu_timer_cnt_get();
#endif

    #if (RTE_PMU_TIMER_REGISTER_CALLBACK)
    pmu_timer_env_t *env;

    env = (pmu_timer_env_t *)(pmu_timer_resource.env);

    if (trig_type == PMU_TIMER_TRIG_VAL0) {
        if (env->isr0_cb) {
            env->isr0_cb(OM_PMU, event, NULL, NULL);
        }
    } else if (trig_type == PMU_TIMER_TRIG_VAL1) {
        if (env->isr1_cb) {
            env->isr1_cb(OM_PMU, event, NULL, NULL);
        }
    }
    #endif
}

/**
 *******************************************************************************
 * @brief get pmu timer left time(ms)
 *
 * @param[in] trig_type    trigger type
 *
 * @return
 *  - 0xFFFFFFFF : invalid time
 *  - others : left time(tick)
 *******************************************************************************
 */
__RAM_CODES("PM")
uint32_t drv_pmu_timer_left_time_get(pmu_timer_trig_t trig_type)
{
    uint32_t int_is_en;
    uint32_t left_tick;
    uint32_t timer_set;
    uint32_t timer_cnt;

    int_is_en = (uint32_t)drv_pmu_timer_control(trig_type, PMU_TIMER_CONTROL_GET_TRIG_INT_STATE, NULL);

    if (int_is_en) {
        timer_set = (uint32_t)drv_pmu_timer_control(trig_type, PMU_TIMER_CONTROL_GET_TIMER_VAL, NULL);
        timer_cnt = drv_pmu_timer_cnt_get();

        if (timer_set > timer_cnt) {
            left_tick = timer_set - timer_cnt;
        } else {
            left_tick = PMU_TIMER_MAX_TICK + timer_set - timer_cnt;
        }
        left_tick = left_tick > PMU_TIMER_MAX_DELAY ? 0U : left_tick;

        return left_tick;
    }
    return DRV_INVALID_TIME;
}


/**
 *******************************************************************************
 * @brief pmu timer interrupt service routine
 *
 *******************************************************************************
 */
void drv_pmu_timer_isr(void)
{
    __IO uint32_t status;
    OM_PMU_Type *pmu_timer;

    pmu_timer = (OM_PMU_Type *)pmu_timer_resource.reg;

    while (1) {
        status = pmu_timer->TIMER_CTRL;

        // while(1){}: prevent digital lost INT0 irq when INT1 irq is also pending
        if ((status & (PMU_TIMER_CTRL_PMU_TIMER_INT0_MASK|PMU_TIMER_CTRL_PMU_TIMER_INT1_MASK)) == 0)
            break;

        if (status & PMU_TIMER_CTRL_PMU_TIMER_INT0_MASK) {
            // clear interrupt status
            // Fix: when 2 pmutimers are running as same time, stop one, may lead another stoped issue.
            REGW0(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_INT_VAL0_EN_MASK);
            REGW1(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_INT_VAL0_CLR_MASK);
            REGW0(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_INT_VAL0_CLR_MASK);

            // callback
            drv_pmu_timer_isr_callback(OM_PMU, PMU_TIMER_TRIG_VAL0, DRV_EVENT_COMMON_GENERAL);
        }

        if (status & PMU_TIMER_CTRL_PMU_TIMER_INT1_MASK) {
            // clear interrupt status
            REGW0(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_INT_VAL1_EN_MASK);
            REGW1(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_INT_VAL1_CLR_MASK);
            REGW0(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_INT_VAL1_CLR_MASK);

            // callback
            drv_pmu_timer_isr_callback(OM_PMU, PMU_TIMER_TRIG_VAL1, DRV_EVENT_COMMON_GENERAL);
        }
    }
}


#endif  /* RTE_PMU_TIMER */


/** @} */
