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
 * @file     drv_lp_tim.h
 * @brief    Header file of LP timer module
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup LPTIM LPTIM
 * @ingroup  DRIVER
 * @brief    LP TIM Driver for om662x
 * @details  LP TIM Driver for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 *
 * @example example_lp_timer.c
 * This is an example of how to use the lp timer
 *
 */

#ifndef __DRV_LP_TIM_H
#define __DRV_LP_TIM_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_LP_TIM)
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
/// LP Timer Control
typedef enum {
    LP_TIM_CONTROL_ENABLE,                  /*!< When argu is NULL, disable lptimer, and vice versa */
    LP_TIM_CONTROL_START,                   /*!< Start LP Timer */
    LP_TIM_CONTROL_STOP,                    /*!< Stop LP Timer */
    LP_TIM_CONTROL_CLEAR,                   /*!< Clear LP Timer counter, and load top val */
    LP_TIM_CONTROL_CTO0,                    /*!< Clear toggle out0 to its idle state */
    LP_TIM_CONTROL_CTO1,                    /*!< Clear toggle out1 to its idle state */
    LP_TIM_CONTROL_CNT_SET,                 /*!< Set counter val, val is in range [0~0xFFFF] */
    LP_TIM_CONTROL_TOP_SET,                 /*!< Set top val, val is in range [0~0xFFFF] */
    LP_TIM_CONTROL_REP0_SET,                /*!< Set rep0 val, val is in range [0~0xFF] */
    LP_TIM_CONTROL_REP1_SET,                /*!< Set rep1 val, val is in range [0~0xFF] */
    LP_TIM_CONTROL_POWER_IN_SLEEP,          /*!< When argu is not NULL, power on lptimer in sleep, and vice versa */
    LP_TIM_CONTROL_INT_EN,                  /*!< Control enable or disable lp timer interrupt, argu is the interrupt en combinations */
} lp_tim_control_t;

/// LP Timer Reption Mode
typedef enum {
    /// Free Running Mode: run until it is stopped
    LP_TIM_MODE_FREE_RUNNING    = 0U,
    /// One Shot Mode: run as long as REP0 != 0
    LP_TIM_MODE_ONE_SHOT        = 1U,
    /// Buffered Mode: run as long as REP0 != 0, REP1 will be loaded into REP0 once if REP1 != 0
    LP_TIM_MODE_BUFFERED        = 2U,
    /// Double Mode: run as long as REP0 != 0 or REP1 != 0
    LP_TIM_MODE_DOUBLE          = 3U,
} lp_tim_rep_mode_t;

/// Decide whether to load TOP val into Counter on each underflow
typedef enum {
    /// Load 0xFFFFFF into Counter
    LP_TIM_TOP_DISABLE          = 0U,
    /// Load TOP val into Counter
    LP_TIM_TOP_ENABLE           = 1U,
} lp_tim_top_en_t;

/// Decide whether to load TOPBUF val into TOP when REP0 is about to decrement to 0
typedef enum {
    /// BUFTOP is used
    LP_TIM_BUFTOP_DISABLE       = 0U,
    /// BUFTOP is not used
    LP_TIM_BUFTOP_ENABLE        = 1U,
} lp_tim_buftop_en_t;

/// LP Timer Clock Frequency Division
typedef enum {
    LP_TIM_PRESC_DIV1           = 0U,
    LP_TIM_PRESC_DIV2           = 1U,
    LP_TIM_PRESC_DIV4           = 2U,
    LP_TIM_PRESC_DIV8           = 3U,
    LP_TIM_PRESC_DIV16          = 4U,
    LP_TIM_PRESC_DIV32          = 5U,
    LP_TIM_PRESC_DIV64          = 6U,
    LP_TIM_PRESC_DIV128         = 7U,
    LP_TIM_PRESC_DIV256         = 8U,
} lp_tim_presc_t;

/// LP Timer Channel
typedef enum {
    /// Channel 0
    LP_TIM_CHAN_OUT0,
    /// Channel 1
    LP_TIM_CHAN_OUT1,
} lp_tim_chan_out_t;

/// IDLE Level Defination
typedef enum {
    /// Define outx idle level to low
    LP_TIM_POL_IDLE_LOW         = 0U,
    /// Define outx idle level to high
    LP_TIM_POL_IDLE_HIGH        = 1U,
} lp_tim_out_pol_t;

/// LP Timer UnderFlow Out Action Defination
typedef enum {
    /// OUTx is held at its idle value
    LP_TIM_UFOA_NONE            = 0U,
    /// OUTx is toggled on COMPx match, DIGITAL BUG: the pin state eventually returns to the IDLE state
    LP_TIM_UFOA_TOGGLE          = 1U,
    /// OUTx is held active for one LP Timer clock cycle on COMP0/1 match, then return to its IDLE state
    LP_TIM_UFOA_PULSE           = 2U,
    /// OUTx is set to IDLE on CNT underflow, and active on compare match COMP0/1
    LP_TIM_UFOA_PWM             = 3U,
} lp_tim_underflow_out_action_t;

/// LP Timer interrupt enable type
typedef enum {
    LP_TIM_INTE_COMP0_EN   = (0x1U << 0),
    LP_TIM_INTE_COMP1_EN   = (0x1U << 1),
    LP_TIM_INTE_UF_EN      = (0x1U << 2),
    LP_TIM_INTE_REP0_EN    = (0x1U << 3),
    LP_TIM_INTE_REP1_EN    = (0x1U << 4),
} lp_tim_int_en_t;

/// LP Timer OUT Configuration
typedef struct {
    /// OUT Polarity
    lp_tim_out_pol_t                pol;
    /// OUT Action
    lp_tim_underflow_out_action_t   action;
} lp_tim_out_config_t;

/// LP Timer Free Running Configuration
typedef struct {
    /// Frequency Division
    lp_tim_presc_t      presclar;
    /// Use TOP val or not
    lp_tim_top_en_t     top_en;
    /// TOP val
    uint16_t            top_val;
    /// Output Compare value0
    uint16_t            compare_val0;
    /// Output Compare value1
    uint16_t            compare_val1;
} lp_tim_free_running_config_t;

/// LP Timer One Shot Configuration
typedef struct {
    /// Frequency Division
    lp_tim_presc_t      presclar;
    /// Reption value0
    uint8_t             rep0_val;
    /// Use TOP val or not
    lp_tim_top_en_t     top_en;
    /// TOP val
    uint16_t            top_val;
    /// Output Compare value0
    uint16_t            compare_val0;
    /// Output Compare value1
    uint16_t            compare_val1;
} lp_tim_one_shot_config_t;

/// LP Timer Buffered Configuration
typedef struct {
    /// Frequency Division
    lp_tim_presc_t      presclar;
    /// Reption value0
    uint8_t             rep0_val;
    /// Reption value1
    uint8_t             rep1_val;
    /// Use TOP val or not
    lp_tim_top_en_t     top_en;
    /// TOP val
    uint16_t            top_val;
    /// Use BUFTOP val or not
    lp_tim_buftop_en_t  buftop_en;
    /// BUFTOP val
    uint16_t            buftop_val;
    /// Output Compare value0
    uint16_t            compare_val0;
    /// Output Compare value1
    uint16_t            compare_val1;
} lp_tim_buffered_config_t;

/// LP Timer Double Configuration
typedef struct {
    /// Frequency Division
    lp_tim_presc_t      presclar;
    /// Reption value0
    uint8_t             rep0_val;
    /// Reption value1
    uint8_t             rep1_val;
    /// Use TOP val or not
    lp_tim_top_en_t     top_en;
    /// TOP val
    uint16_t            top_val;
    /// Output Compare value0
    uint16_t            compare_val0;
    /// Output Compare value1
    uint16_t            compare_val1;
} lp_tim_double_config_t;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
#if (RTE_LP_TIM_REGISTER_CALLBACK)
/**
 * @brief LP Timer register callback
 *
 * @param[in] om_lp_tim      Pointer to LP Timer
 * @param[in] isr_cb         callback
 *
 **/
extern void drv_lp_tim_register_isr_callback(OM_LP_TIM_Type *om_lp_tim, drv_isr_callback_t isr_cb);
#endif

/**
 *******************************************************************************
 * @brief The interrupt callback for LP Timer driver. It is a weak function. User should define
 *        their own callback in user file, other than modify it in the LP Timer driver.
 *
 * @param om_lp_tim         The LP Timer device address
 * @param event             The driver usart event
 *                           - DRV_EVENT_LP_TIMER_COMP0
                             - DRV_EVENT_LP_TIMER_COMP1
                             - DRV_EVENT_LP_TIMER_UNDER_FLOW
                             - DRV_EVENT_LP_TIMER_REP0
                             - DRV_EVENT_LP_TIMER_REP1
 *******************************************************************************
 */
extern __WEAK void drv_lp_tim_isr_callback(OM_LP_TIM_Type *om_lp_tim, drv_event_t event);

/**
 * @brief LP Timer free-running config
 *
 * @param[in] om_lp_tim     Pointer to LP Timer
 * @param[in] cfg           free-running config
 *
 * @return errno
 **/
extern om_error_t drv_lp_tim_free_running_init(OM_LP_TIM_Type *om_lp_tim, const lp_tim_free_running_config_t *cfg);

/**
 * @brief LP Timer one shot config
 *
 * @param[in] om_lp_tim     Pointer to LP Timer
 * @param[in] cfg           one shot config
 *
 * @return errno
 **/
extern om_error_t drv_lp_tim_one_shot_init(OM_LP_TIM_Type *om_lp_tim, const lp_tim_one_shot_config_t *cfg);

/**
 * @brief LP Timer buffered config
 *
 * @param[in] om_lp_tim     Pointer to LP Timer
 * @param[in] cfg           buffered config
 *
 * @return errno
 **/
extern om_error_t drv_lp_tim_buffered_init(OM_LP_TIM_Type *om_lp_tim, const lp_tim_buffered_config_t *cfg);

/**
 * @brief LP Timer double config
 * @note Digital Bug: if clear rep0/rep1's interrup flag, the out0/1 will continue
 * output, now just support output the max times of repetition
 *
 * @param[in] om_lp_tim     Pointer to LP Timer
 * @param[in] cfg           double config
 *
 * @return errno
 **/
extern om_error_t drv_lp_tim_double_init(OM_LP_TIM_Type *om_lp_tim, const lp_tim_double_config_t *cfg);

/**
 * @brief LP Timer out config
 *
 * @param[in] om_lp_tim     Pointer to LP Timer
 * @param[in] chan_outx     output channel(0,1)
 * @param[in] cfg           out config
 *
 * @return errno
 **/
extern om_error_t drv_lp_tim_outx_config(OM_LP_TIM_Type *om_lp_tim, lp_tim_chan_out_t chan_outx, const lp_tim_out_config_t *cfg);

/**
 * @brief LP Timer control
 *
 * @param[in] om_lp_tim     Pointer to LP Timer
 * @param[in] ctrl          control command
 * @param[in] argu          argument
 *
 * @return control status
 *
 **/
extern void *drv_lp_tim_control(OM_LP_TIM_Type *om_lp_tim, lp_tim_control_t ctrl, void *argu);

/**
 * @brief lp tim interrupt service routine
 *
 * @param[in] om_lp_tim     Pointer to LP Timer
 **/
extern void drv_lp_tim_isr(OM_LP_TIM_Type *om_lp_tim);


#ifdef __cplusplus
}
#endif

#endif  /* (RTE_LP_TIM) */

#endif  /* __DRV_LP_TIM_H */


/** @} */