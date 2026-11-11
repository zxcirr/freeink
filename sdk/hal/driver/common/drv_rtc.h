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
 * @file     drv_rtc.h
 * @brief    Header file of RTC HAL module
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup RTC RTC
 * @ingroup  DRIVER
 * @brief    RTC Driver for om662x
 * @details  RTC Driver for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 *
 * @example example_rtc.c
 * This is an example of how to use the rtc
 *
 */

#ifndef __DRV_RTC_H
#define __DRV_RTC_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_RTC)
#include <stdint.h>
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
/// RTC Alarm Event Type
typedef enum {
    /// alarm for event0
    RTC_ALARM_EVENT0    = 0U,
    /// alarm for event1
    RTC_ALARM_EVENT1    = 1U,
    /// alarm for event2
    RTC_ALARM_EVENT2    = 2U,

    RTC_ALARM_EVENT_MAX,
} rtc_alarm_event_t;

/// RTC Control
typedef enum {
    RTC_CONTROL_RESET,                      /*!< enable second(1hz) interrupt */
    RTC_CONTROL_ACCU_SET,                   /*!< set accu reg, argu is value */
} rtc_control_t;

/// RTC time
typedef struct {
    int         tm_sec;                     /*!< seconds after the minute (0-59) */
    int         tm_min;                     /*!< minutes after the hour (0-59) */
    int         tm_hour;                    /*!< hours since midnight (0-23) */
    int         tm_mday;                    /*!< day of the month (1-31) */
    int         tm_mon;                     /*!< months since January (0-11) */
    int         tm_year;                    /*!< years since 1900 */
    int         tm_wday;                    /*!< days since Sunday (0-6) */
    int         tm_yday;                    /*!< days since January 1 (0-365) */
    int         tm_isdst;                   /*!< Daylight Saving Time flag */
    int64_t     __TM_GMTOFF;
    const char *__TM_ZONE;
} rtc_tm_t;

/// RTC second interrupt callback
typedef void (*drv_rtc_second_isr_callback_t)(const rtc_tm_t *tm);

/// RTC alarm interrupt callback
typedef void (*drv_rtc_alarm_isr_callback_t)(rtc_alarm_event_t event, const rtc_tm_t *tm);


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief RTC initialize
 *
 * @param[in] om_rtc: pointer to rtc
 *
 *******************************************************************************
 */
extern om_error_t drv_rtc_init(OM_RTC_Type *om_rtc);

/**
 *******************************************************************************
 * @brief Register second isr callback
 *
 * @param[in] om_rtc: pointer to rtc
 * @param[in] cb:     second callback fucntion
 *
 *******************************************************************************
 */
extern void drv_rtc_register_second_isr_callback(OM_RTC_Type *om_rtc,
                                                 drv_rtc_second_isr_callback_t cb);

/**
 *******************************************************************************
 * @brief Register alarm isr callback
 *
 * @param[in] om_rtc: pointer to rtc
 * @param[in] cb:     alarm callback fucntion
 *
 *******************************************************************************
 */
extern void drv_rtc_register_alarm_isr_callback(OM_RTC_Type *om_rtc,
                                                drv_rtc_alarm_isr_callback_t cb);

/**
 *******************************************************************************
 * @brief Set current time
 *
 * @param[in] om_rtc: pointer to rtc
 * @param[in] tm:     pointer to time configuration
 *
 *******************************************************************************
 */
extern void drv_rtc_timer_set(OM_RTC_Type *om_rtc, rtc_tm_t *tm);

/**
 *******************************************************************************
 * @brief Get current time
 *
 * @param[in] om_rtc: pointer to rtc
 * @param[out] tm:    pointer to time configuration
 *
 *******************************************************************************
 */
extern void drv_rtc_timer_get(OM_RTC_Type *om_rtc, rtc_tm_t *tm);

/**
 *******************************************************************************
 * @brief Set alarm time
 *
 * @param[in] om_rtc: pointer to rtc
 * @param[in] event:  alarm event
 * @param[in] tm:     pointer to time configuration
 *
 *******************************************************************************
 */
void drv_rtc_alarm_set(OM_RTC_Type *om_rtc, rtc_alarm_event_t event, rtc_tm_t *tm);

/**
 *******************************************************************************
 * @brief Get alarm time
 *
 * @param[in] om_rtc: pointer to rtc
 * @param[in] event:  alarm event
 * @param[in] tm:     pointer to time configuration
 *
 *******************************************************************************
 */
extern void drv_rtc_alarm_get(OM_RTC_Type *om_rtc, rtc_alarm_event_t event, rtc_tm_t *tm);

/**
 *******************************************************************************
 * @brief RTC control
 *
 * @param[in] om_rtc:   pointer to rtc
 * @param[in] control:  control command
 * @param[in] argu:     argument
 *
 * @return control status
 *******************************************************************************
 */
extern void *drv_rtc_control(OM_RTC_Type *om_rtc, rtc_control_t control, void *argu);

/**
 *******************************************************************************
 * @brief RTC second interrupt service routine
 *
 * @param[in] om_rtc: pointer to rtc
 *
 *******************************************************************************
 */
extern void drv_rtc_second_isr(OM_RTC_Type *om_rtc);

/**
 *******************************************************************************
 * @brief RTC alarm interrupt service routine
 *
 * @param[in] om_rtc: pointer to rtc
 *
 *******************************************************************************
 */
extern void drv_rtc_alarm_isr(OM_RTC_Type *om_rtc);


#ifdef __cplusplus
}
#endif

#endif  /* (RTE_RTC) */

#endif  /* __DRV_RTC_H */


/** @} */