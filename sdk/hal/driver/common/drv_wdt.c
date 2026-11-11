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
 * @file     drv_wdt.c
 * @brief
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
#if (RTE_WDT)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    drv_isr_callback_t      event_cb;
} wdt_env_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
static wdt_env_t wdt_env = {
    .event_cb   = NULL,
};


/*******************************************************************************
 * LOCAL FUNCTIONS
 */


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 * @brief wdt register callback
 *
 * @param[in] isr_cb         callback
 *
 * @return None
 **/
void drv_wdt_register_isr_callback(drv_isr_callback_t isr_cb)
{
    wdt_env.event_cb = isr_cb;
    if (isr_cb != NULL) {
        // Enable WDT cpu en int
        REGW1(&OM_PMU->WDT_STATUS, PMU_WDT_STATUS_WDT_INT_CPU_EN_MASK);

        // Enable WDT NVIC int
        NVIC_ClearPendingIRQ(WDT_IRQn);
        NVIC_SetPriority(WDT_IRQn, RTE_WDT_IRQ_PRIORITY);
        NVIC_EnableIRQ(WDT_IRQn);
    } else {
        REGW0(&OM_PMU->WDT_STATUS, PMU_WDT_STATUS_WDT_INT_CPU_EN_MASK);
        NVIC_DisableIRQ(WDT_IRQn);
    }
}

/**
 * @brief watch dog keepalive
 *
 * @return None
 **/
void drv_wdt_keep_alive(void)
{
    OM_PMU->WDT_KR_CFG = 0xAAAA;
}

/**
 * @brief enable watch dog
 *
 * @param[in] timeout_ms  timeout with second
 **/
void drv_wdt_enable(uint32_t timeout_ms)
{
    REGW0(&OM_PMU->WDT_STATUS, PMU_WDT_STATUS_WDT_INT_CPU_EN_MASK);
    if (timeout_ms) {
        while(OM_PMU->WDT_STATUS & PMU_WDT_STATUS_LD_WDT_KR_STATUS_MASK);
        OM_PMU->WDT_RLR_CFG = timeout_ms * 128 / 1000;
        OM_PMU->WDT_KR_CFG = 0x5555;
        while(OM_PMU->WDT_STATUS & PMU_WDT_STATUS_LD_WDT_KR_STATUS_MASK);
        OM_PMU->WDT_KR_CFG = 0xAAAA;
    }
}

/**
 *******************************************************************************
 * @brief wdt interrupt service routine
 *
 *******************************************************************************
 */
void drv_wdt_isr(void)
{
    DRV_IRQ_BEGIN();

    DRV_DELAY_MS(8);
    REGW1(&OM_PMU->WDT_STATUS, PMU_WDT_STATUS_WDT_FLAG_CLR_MASK);

    if(wdt_env.event_cb) {
        wdt_env.event_cb(NULL, DRV_EVENT_COMMON_GENERAL, NULL, NULL);
    }

    DRV_IRQ_END();
}


#endif /* RTE_WDT*/

/** @} */
