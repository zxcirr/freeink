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
 * @file     evt_timer.h
 * @brief    event timer
 * @date     01. April 2020
 * @author   OnMicro SW Team
 *
 * @defgroup EVT_TIMER Event timer
 * @ingroup  COMPONENTS
 * @brief    event timer that based on pmu_timer and evt
 * @details  event timer that based on pmu_timer and evt
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __EVT_TIMER_H
#define __EVT_TIMER_H


/*******************************************************************************
 * INCLUDES
 */

#include <stdbool.h>
#include "om_list.h"
#include "om_driver.h"
#include "evt.h"

#ifdef  __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
/// Max tick: 32bit, 30.52us (1/32768)s
#define EVT_TIMER_MAX_TICK              PMU_TIMER_MAX_TICK
/// Max delay tick
#define EVT_TIMER_MAX_DELAY             (EVT_TIMER_MAX_TICK/2 - 1)
/// unit transmite 1ms to 30.52us
#define EVT_TIMER_MS2TICK(time)         PMU_TIMER_MS2TICK(time)
/// unit transmite 30.52us to 1ms
#define EVT_TIMER_TICK2MS(time)         PMU_TIMER_TICK2MS(time)

/*******************************************************************************
 * TYPEDEFS
 */
/// Timer mode for one shot or repeat
typedef enum
{
    /// Only run once
    EVT_TIMER_ONE_SHOT,
    /// Repeat until stop it
    EVT_TIMER_REPEAT,
} evt_timer_mode_t;

/// @cond
/// timer object
typedef struct
{
    om_list_node_t hdr;

    uint32_t delay_tick;
    void *cb;
    evt_timer_mode_t mode;
    void *param;

    uint32_t time;
} evt_timer_t;
/// @endcond

/**
 * @brief software timer expire callback
 *
 * @param[in] timer  Timer object
 * @param[in] param  Parameter with evt_timer_set()
 *
 * @return None
 **/
typedef void (*evt_timer_callback_t)(evt_timer_t *timer, void *param);

/*******************************************************************************
 * EXTERN FUNCTIONS
 */

/// @cond
/**
 ****************************************************************************************
 * @brief timer init
 *
 * @return None
 ****************************************************************************************
 **/
void evt_timer_init(void);
/// @endcond

/**
 ****************************************************************************************
 * @brief Setup a software timer with tick
 *
 * @param[in] timer  the timer object (must be static or global variable)
 * @param[in] delay_tick  its uint is 1/32768s, about 30.52us, max is "EVT_MAX_TIMER_DELAY"
 * @param[in] mode  one shot or repeat
 * @param[in] callback  expire callback
 * @param[in] param  params
 *
 * @return false: Set timer fail;
 *         true: Set timer success
 ****************************************************************************************
 **/
bool evt_timer_set_tick(evt_timer_t *timer, uint32_t delay_tick, evt_timer_mode_t mode,
        evt_timer_callback_t callback, void *param);

/**
 ****************************************************************************************
 * @brief Setup a software timer with millisecond
 *
 * @param[in] timer  the timer object (must be static or global variable)
 * @param[in] delay  its uint is 1ms, max is "CO_TIME_SYS2MS(CO_MAX_TIMER_DELAY)"
 * @param[in] mode  one shot or repeat
 * @param[in] callback  expire callback
 * @param[in] param  params
 *
 * @return false: Set timer fail;
 *         true: Set timer success
 ****************************************************************************************
 **/
bool evt_timer_set(evt_timer_t *timer, uint32_t delay, evt_timer_mode_t mode,
        evt_timer_callback_t callback, void *param);

/**
 ****************************************************************************************
 * @brief delete a timer
 *
 * @param[in] timer  the timer object (must be static or global variable)
 ****************************************************************************************
 **/
void evt_timer_del(evt_timer_t *timer);

/**
 ****************************************************************************************
 * @brief timer dump
 *
 * @note If this function called in co_timer expired callback, the expired timer don't dump
 *
 * @param[in] printf_dump_func  dump function, link printf
 ****************************************************************************************
 **/
void evt_timer_dump(void *printf_dump_func);

#ifdef  __cplusplus
}
#endif

#endif  /* __EVT_H */

/** @} */

