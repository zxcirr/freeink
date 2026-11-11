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
 * @file     obc_llt.h
 * @brief    obc_llt
 * @date     15 December 2021
 * @author   OnMicro SW Team
 *
 * @defgroup OBLLT OBLLT
 * @ingroup  Peripheral
 * @brief    OBLLT Driver
 * @details  OBLLT Driver

 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __OB_LLT_H
#define __OB_LLT_H

#ifdef  __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */
#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 * MACROS
 */
#define OBC_LLT_INVALID_TIME     0xFFFFFFFF
#define OBC_LLT_AUTO_TIME        0xFFFFFFFF

/*******************************************************************************
 * TYPEDEFS
 */
/// Event type
typedef enum {
    /// Event begin
    OBC_LLT_EVENT_BEGIN,
    /// Event end
    OBC_LLT_EVENT_END,
    /// Event external ISR
    OBC_LLT_EVENT_EXTERNAL_ISR,
    /// Event timed trigger
    OBC_LLT_EVENT_TRIGGER,
} obc_llt_event_t;

/// status
typedef enum {
    /// Maintain system event schedule
    OBC_LLT_STATUS_CONTINUE,
    /// Indicate current event can be ended.
    OBC_LLT_STATUS_END,
} obc_llt_status_t;

/// LLT mode
typedef enum {
    /// on time
    OBC_LLT_MODE_ONTIME,
    /// not on time
    OBC_LLT_MODE_NOT_ONTIME,
    /// background
    OBC_LLT_MODE_BACKGROUND,
} obc_llt_mode_t;

/**
 *******************************************************************************
 * @brief  ob llt event callback
 *
 * @param[in] event  event
 * @param[in] cur_time  cur time
 *
 * @return  @ref obc_llt_event_callback_t
 *******************************************************************************
 */
typedef obc_llt_status_t (* obc_llt_event_callback_t)(obc_llt_event_t event, uint32_t cur_time, uint32_t *next_trigger_time);

/// Params
typedef struct {
    /// Mode
    obc_llt_mode_t mode;
    /// Priority init
    uint8_t prio_init;
    /// Priority increase
    uint8_t prio_inc;

    /// Interval min in 625us
    uint32_t interval_min;
    /// Interval max in 625us
    uint32_t interval_max;
    /// Duration min in 625us
    uint32_t duration_min;
    /// Duration max in 625us
    uint32_t duration_max;

    // Event callback
    obc_llt_event_callback_t event_cb;
} obc_llt_params_t;


/// base llt
typedef struct {
    /// Timer for LLT
    uint32_t llt_timer[9];
    /// Timer for tigger
    uint32_t trigger_timer[3];

    /// Priority init
    uint8_t prio_init;
    /// Priority increase
    uint8_t prio_inc;
    /// Mode
    obc_llt_mode_t mode;
    /// act_id
    uint8_t act_id;
    /// is event begin
    bool is_event_begin;
    /// is event stop
    bool is_event_stop;
    /// count
    uint16_t count;

    /// offset in 31.25us
    uint32_t offset;
    /// interval in 31.25us
    uint32_t interval;
    /// duration in 31.25us
    uint32_t duration;
    /// end_time in 31.25us
    uint32_t end_time;

    /// Event callback
    obc_llt_event_callback_t event_cb;
} obc_llt_t;


/*******************************************************************************
 * EXTERN FUNCTIONS
 */

/**
 *******************************************************************************
 * @brief  ob llt init
 *
 * @param[in] llt  llt
 *******************************************************************************
 */
void obc_llt_init(obc_llt_t *llt);

/**
 *******************************************************************************
 * @brief  ob llt start
 *
 * @param[in] llt  llt
 *******************************************************************************
 */
void obc_llt_start(obc_llt_t *llt);

/**
 *******************************************************************************
 * @brief  ob llt stop
 *
 * @param[in] llt  llt
 *******************************************************************************
 */
void obc_llt_stop(obc_llt_t *llt);

/**
 *******************************************************************************
 * @brief  ob llt params set
 *
 * @param[in] llt  llt
 * @param[in] params  params
 *******************************************************************************
 */
void obc_llt_params_set(obc_llt_t *llt, const obc_llt_params_t *params);

/**
 *******************************************************************************
 * @brief  ob llt timer set
 *
 * @param[in] llt  llt
 * @param[in] time  time
 *******************************************************************************
 */
void obc_llt_timer_set(obc_llt_t *llt, uint32_t time);

/**
 *******************************************************************************
 * @brief  ob llt external isr
 *
 * @param[in] llt  llt
 *******************************************************************************
 */
void obc_llt_external_isr(obc_llt_t *llt);

/**
 *******************************************************************************
 * @brief  obc llt time
 *
 * @return current time
 *******************************************************************************
 */
uint32_t obc_llt_time(void);

#ifdef  __cplusplus
}
#endif

#endif /* __OB_LLT_H */


/** @} */

