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
 * @file     drv_rtc.c
 * @brief    driver for rtc
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
#if (RTE_RTC)
#include <stddef.h>
#include <time.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    drv_rtc_second_isr_callback_t second_cb;
    drv_rtc_alarm_isr_callback_t alarm_event_cb;
} drv_rtc_env_t;

typedef struct {
    void            *reg;
    void            *env;
    IRQn_Type       second_irq_num;
    uint8_t         second_irq_prio;
    IRQn_Type       alarm_irq_num;
    uint8_t         alarm_irq_prio;
} drv_rtc_resource_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
#if (RTE_RTC)
static drv_rtc_env_t rtc_env = {
    .second_cb          = NULL,
    .alarm_event_cb    = NULL,
};

static const drv_rtc_resource_t rtc_resource = {
    .reg                = OM_RTC,
    .env                = &rtc_env,
    .second_irq_num     = RTC_1HZ_IRQn,
    .second_irq_prio    = RTE_RTC_1HZ_IRQ_PRIORITY,
    .alarm_irq_num      = RTC_AF_IRQn,
    .alarm_irq_prio     = RTE_RTC_AF_IRQ_PRIORITY,
};
#endif


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
/**
 * @brief  rtc read SR
 *
 * @return value of SR
 **/
static const drv_rtc_resource_t *rtc_get_resource(OM_RTC_Type *om_rtc)
{
    #if (RTE_RTC)
    if ((uint32_t)om_rtc == (uint32_t)rtc_resource.reg) {
        return &rtc_resource;
    }
    #endif  /*(RTE_RTC)*/

    OM_ASSERT(0);
    return NULL;
}

/**
 * @brief  read SR
 *
 * @param[in] om_rtc    the pointer to rtc
 * @param[in] value     value
 **/
static uint32_t rtc_read_SR(OM_RTC_Type *om_rtc)
{
    uint32_t t;

    // DIGITAL BUG: try 2 times to workaround
    do
    {
        t = om_rtc->SR;
    } while (t != om_rtc->SR);

    return t;
}

/**
 * @brief  rtc write CR
 *
 * @param[in] om_rtc    the pointer to rtc
 * @param[in] value     value
 **/
static void rtc_write_CR(OM_RTC_Type *om_rtc, uint32_t value)
{
    om_rtc->CR = value;
}

/**
 * @brief  rtc write GR
 *
 * @param[in] om_rtc    the pointer to rtc
 * @param[in] value     value
 **/
static void rtc_write_GR(OM_RTC_Type *om_rtc, uint32_t value)
{
    om_rtc->GR |=  RTC_LOCK_MASK;
    while(!(om_rtc->GR & RTC_LOCK_SYNC_MASK));

    om_rtc->GR = value;

    om_rtc->GR &= ~RTC_LOCK_MASK;
    while(om_rtc->GR & RTC_LOCK_SYNC_MASK);
}

/**
 * @brief  rtc write SR
 *
 * @param[in] om_rtc    the pointer to rtc
 * @param[in] value     value
 **/
void rtc_write_SR(OM_RTC_Type *om_rtc, uint32_t value)
{
    while(om_rtc->CR & RTC_SR_INI_SYNC_MASK);
    om_rtc->SR = value;
}

/**
 * @brief  rtc write SAR0
 *
 * @param[in] om_rtc        the pointer to rtc
 * @param[in] event         alarm event
 * @param[in] value         value
 **/
static void rtc_write_SAR(OM_RTC_Type *om_rtc, rtc_alarm_event_t event, uint32_t value)
{
    OM_ASSERT(event < RTC_ALARM_EVENT_MAX);

    switch (event) {
        case RTC_ALARM_EVENT0:
            om_rtc->CR &= ~RTC_AE_0_MASK;
            while (om_rtc->CR & RTC_AE_0_SYNC_MASK);
            om_rtc->SAR0 = value;
            om_rtc->CR |= RTC_AE_0_MASK;
            while (!(om_rtc->CR & RTC_AE_0_SYNC_MASK));
            break;
        case RTC_ALARM_EVENT1:
            om_rtc->CR &= ~RTC_AE_1_MASK;
            while (om_rtc->CR & RTC_AE_1_SYNC_MASK);
            om_rtc->SAR1 = value;
            om_rtc->CR |= RTC_AE_1_MASK;
            while (!(om_rtc->CR & RTC_AE_1_SYNC_MASK));
            break;
        case RTC_ALARM_EVENT2:
            om_rtc->CR &= ~RTC_AE_2_MASK;
            while (om_rtc->CR & RTC_AE_2_SYNC_MASK);
            om_rtc->SAR2 = value;
            om_rtc->CR |= RTC_AE_2_MASK;
            while (!(om_rtc->CR & RTC_AE_2_SYNC_MASK));
            break;
        default:
            break;
    }
}

/**
 * @brief  check if rtc is running
 *
 **/
static bool rtc_is_running(void)
{
    return (OM_RTC->CR & RTC_EN_SYNC_MASK);
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
om_error_t drv_rtc_init(OM_RTC_Type *om_rtc)
{
    const drv_rtc_resource_t    *resource;
    drv_rtc_env_t               *env;

    // open RTC power and clock
    OM_PMU->PSO_PM |= PMU_PSO_PM_RTC_POWER_ON_MASK;
    // wait RTC power on
    while (!(OM_PMU->PSO_PM & PMU_PSO_PM_RTC_POWER_STATUS_MASK));
    DRV_RCC_CLOCK_ENABLE(RCC_CLK_RTC, 1U);

    resource = rtc_get_resource(om_rtc);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }
    env = (drv_rtc_env_t *)(resource->env);
    env->second_cb      = NULL;
    env->alarm_event_cb = NULL;

    NVIC_DisableIRQ(resource->second_irq_num);
    NVIC_ClearPendingIRQ(resource->second_irq_num);
    NVIC_SetPriority(resource->second_irq_num, resource->second_irq_prio);

    NVIC_DisableIRQ(resource->alarm_irq_num);
    NVIC_ClearPendingIRQ(resource->alarm_irq_num);
    NVIC_SetPriority(resource->alarm_irq_num, resource->alarm_irq_prio);

    if (rtc_is_running()) {
        return OM_ERROR_BUSY;
    }
    // RTC reset (!!! Must power on firstly, then reset it)
    OM_PMU->MISC_CTRL &= ~PMU_MISC_CTRL_RTC_SOFT_RESET_MASK;
    OM_PMU->MISC_CTRL |= PMU_MISC_CTRL_RTC_APB_SOFT_RESET_MASK;
    while (OM_PMU->MISC_CTRL & PMU_MISC_CTRL_RTC_APB_SOFT_RESET_MASK);

    // Calibration RTC
    //rtc_write_GR(om_rtc, om_rtc->GR & (~RTC_LOCK_MASK));
    rtc_write_GR(om_rtc, (om_rtc->GR & ~RTC_NC1HZ_MASK) | (drv_rcc_clock_get(RCC_CLK_RTC) - 1));
    rtc_write_GR(om_rtc, om_rtc->GR | RTC_LOCK_MASK);

    return OM_ERROR_OK;
}

void drv_rtc_register_second_isr_callback(OM_RTC_Type *om_rtc, drv_rtc_second_isr_callback_t cb)
{
    const drv_rtc_resource_t *resource;
    drv_rtc_env_t            *env;

    resource = rtc_get_resource(om_rtc);
    if (resource) {
        env = (drv_rtc_env_t *)resource->env;
        env->second_cb = cb;
        if (cb) {
            NVIC_EnableIRQ(resource->second_irq_num);
            rtc_write_CR(om_rtc, om_rtc->CR | RTC_1HZ_IE_MASK);
        } else {
            rtc_write_CR(om_rtc, om_rtc->CR & (~RTC_1HZ_IE_MASK));
            NVIC_DisableIRQ(resource->second_irq_num);
            NVIC_ClearPendingIRQ(resource->alarm_irq_num);
        }
    }
}

void drv_rtc_register_alarm_isr_callback(OM_RTC_Type *om_rtc, drv_rtc_alarm_isr_callback_t cb)
{
    const drv_rtc_resource_t *resource;
    drv_rtc_env_t            *env;

    resource = rtc_get_resource(om_rtc);
    if (resource) {
        env = (drv_rtc_env_t *)resource->env;
        env->alarm_event_cb = cb;

        if (cb) {
            NVIC_EnableIRQ(resource->alarm_irq_num);
        } else {
            NVIC_DisableIRQ(resource->alarm_irq_num);
            NVIC_ClearPendingIRQ(resource->alarm_irq_num);
        }
    }
}

void drv_rtc_timer_set(OM_RTC_Type *om_rtc, rtc_tm_t *tm)
{
    if (tm) {
        time_t time_sec = mktime((struct tm *)tm);
        // set second counter register
        rtc_write_SR(om_rtc, (uint32_t)time_sec);
        // enable rtc, NOTE: this will take effect in 2 rtc clk
        rtc_write_CR(om_rtc, om_rtc->CR | RTC_CE_MASK);
    }
}

void drv_rtc_timer_get(OM_RTC_Type *om_rtc, rtc_tm_t *tm)
{
    time_t time_sec = rtc_read_SR(om_rtc);
    memcpy(tm, localtime(&time_sec), sizeof(rtc_tm_t));  /*lint !e668 */
}

void drv_rtc_alarm_set(OM_RTC_Type *om_rtc, rtc_alarm_event_t event, rtc_tm_t *tm)
{
    uint32_t alarm_en_mask;
    OM_ASSERT(event < RTC_ALARM_EVENT_MAX);

    alarm_en_mask = (1 << (RTC_AE_0_POS + (uint32_t)event)) | (1 <<(RTC_AIE_0_POS + (uint32_t)event));
    if (tm) {
        time_t time_sec = mktime((struct tm *)tm);
        rtc_write_SAR(om_rtc, event, (uint32_t)time_sec);
        rtc_write_CR(om_rtc, om_rtc->CR | alarm_en_mask);
    } else {
        rtc_write_CR(om_rtc, om_rtc->CR & (~alarm_en_mask));
    }
}

void drv_rtc_alarm_get(OM_RTC_Type *om_rtc, rtc_alarm_event_t event, rtc_tm_t *tm)
{
    time_t time_sec;

    OM_ASSERT(event < RTC_ALARM_EVENT_MAX);

    switch (event) {
        case RTC_ALARM_EVENT0:
            time_sec = om_rtc->SAR0;
            break;
        case RTC_ALARM_EVENT1:
            time_sec = om_rtc->SAR1;
            break;
        case RTC_ALARM_EVENT2:
            time_sec = om_rtc->SAR2;
            break;
        default:
            break;
    }

    memcpy(tm, localtime(&time_sec), sizeof(rtc_tm_t));
}

void *drv_rtc_control(OM_RTC_Type *om_rtc, rtc_control_t control, void *argu)
{
    OM_CRITICAL_BEGIN();
    switch (control) {
        case RTC_CONTROL_RESET:
            OM_PMU->MISC_CTRL &= ~PMU_MISC_CTRL_RTC_SOFT_RESET_MASK;
            OM_PMU->MISC_CTRL |= PMU_MISC_CTRL_RTC_APB_SOFT_RESET_MASK;
            while (OM_PMU->MISC_CTRL & PMU_MISC_CTRL_RTC_APB_SOFT_RESET_MASK);
            break;
        case RTC_CONTROL_ACCU_SET:
            om_rtc->ACCU = (uint32_t)argu;
            break;
        default:
            break;
    }
    OM_CRITICAL_END();

    return (void *)OM_ERROR_OK;
}

void drv_rtc_second_isr(OM_RTC_Type *om_rtc)
{
    rtc_tm_t  tm;
    uint32_t  status;

    DRV_IRQ_BEGIN();
    status = om_rtc->CR;
    if (status & RTC_1HZ_WAKE_MASK) {
        rtc_write_CR(om_rtc, status | RTC_1HZ_CLR_MASK);
        if (rtc_env.second_cb) {
            drv_rtc_timer_get(om_rtc, &tm);
            rtc_env.second_cb(&tm);
        }
    }
    DRV_IRQ_END();
}

void drv_rtc_alarm_isr(OM_RTC_Type *om_rtc)
{
    rtc_tm_t  tm;
    uint32_t  status;

    DRV_IRQ_BEGIN();
    status = om_rtc->CR;
    for (rtc_alarm_event_t i = RTC_ALARM_EVENT0; i < RTC_ALARM_EVENT_MAX; i++) {
        if ((status >> ((uint32_t)i + RTC_AF_0_WAKE_POS)) & 1U) {
            rtc_write_CR(om_rtc, status | (1U << (RTC_AF_0_CLR_POS + (uint32_t)i)));
            if (rtc_env.alarm_event_cb) {
                drv_rtc_timer_get(om_rtc, &tm);
                rtc_env.alarm_event_cb(i, &tm);
            }
        }
    }
    DRV_IRQ_END();
}

#endif  /* RTE_RTC */

/** @} */