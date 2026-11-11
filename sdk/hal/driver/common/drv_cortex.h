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
 * @file     drv_cortex.h
 * @brief    Header file of Cortex module
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup CORTEX CORTEX
 * @ingroup  DRIVER
 * @brief    CORTEX Driver for om662x
 * @details  CORTEX Driver for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __DRV_CORTEX_H
#define __DRV_CORTEX_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#include <stdint.h>
#include "om_device.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
/**
 * @brief disable global interrupt for enter critical code
 */
#define OM_CRITICAL_BEGIN()                     \
    do {                                        \
        /*lint -save -e578 */                   \
        uint32_t _primask = __get_PRIMASK();    \
        /*lint –restore */                      \
        __disable_irq();

/**
 * @brief  restore global interrupt for exit critical code
 */
#define OM_CRITICAL_END()                       \
        if (!_primask) {                        \
            __enable_irq();                     \
        }                                       \
    } while(0)

/**
 * @brief disable global interrupt for enter critical code
 */
#define OM_CRITICAL_BEGIN_EX(irq_save_is_disabled) \
    do {                                        \
        /*lint -save -e578 */                   \
        irq_save_is_disabled = __get_PRIMASK(); \
        /*lint –restore */                      \
        __disable_irq();                        \
    } while (0)

/**
 * @brief  restore global interrupt for exit critical code
 */
#define OM_CRITICAL_END_EX(irq_save_is_disabled) \
    do {                                        \
        if (!irq_save_is_disabled) {            \
            __enable_irq();                     \
        }                                       \
    } while(0)

/**
 * @brief disable global interrupt for enter critical code
 */
#define OM_ENTER_CRITICAL_EXCEPT(irqn)                          \
    do {                                                        \
        /*lint -save -e578 */                                   \
        uint32_t __irq_save = __get_BASEPRI();                  \
        __set_BASEPRI_MAX((irqn) << (8U - __NVIC_PRIO_BITS));   \
        /*lint –restore */                                      \

/**
 * @brief  restore global interrupt for exit critical code
 */
#define OM_EXIT_CRITICAL_EXCEPT()                               \
        __set_BASEPRI(__irq_save);                              \
    } while(0)

/**
 * @brief disable global interrupt for enter critical code
 */
#define OM_ENTER_CRITICAL_EXCEPT_EX(irqn, irq_save)             \
    do {                                                        \
        /*lint -save -e578 */                                   \
        irq_save = __get_BASEPRI();                             \
        __set_BASEPRI_MAX((irqn) << (8U - __NVIC_PRIO_BITS));   \
        /*lint –restore */                                      \
    } while (0)

/**
 * @brief  restore global interrupt for exit critical code
 */
#define OM_EXIT_CRITICAL_EXCEPT_EX(irq_save)                    \
    do {                                                        \
        __set_BASEPRI(irq_save);                                \
    } while(0)

#if (RTE_CORTEX_DWT)
/// Calculation of ceil value for cycles
#define DRV_DWT_US_2_CYCLES_CEIL(us)     ((SystemCoreClock + 999999U)/ 1000U / 1000U * (us))
/// Calculation of round value for cycles
#define DRV_DWT_US_2_CYCLES_ROUND(us)    ((SystemCoreClock + 500000U) / 1000U / 1000U * (us))
/// Calculation of floor value for cycles
#define DRV_DWT_US_2_CYCLES_FLOOR(us)    (SystemCoreClock / 1000U / 1000U * (us))

/// Calculation of ceil value for cycles
#define DRV_DWT_MS_2_CYCLES_CEIL(ms )    ((SystemCoreClock + 999U)/ 1000U * (ms))
/// Calculation of round value for cycles
#define DRV_DWT_MS_2_CYCLES_ROUND(ms)    ((SystemCoreClock + 500U)/ 1000U * (ms))
/// Calculation of floor value for cycles
#define DRV_DWT_MS_2_CYCLES_FLOOR(ms)    (SystemCoreClock / 1000U * (ms))

#if (RTE_CORTEX_DWT_TIMEOUT)
/**
 *******************************************************************************
 * @brief  continus check wait_val using dwt,
 *
 * @param  wait_val wait value
 * @param  to_ms timeout value in millisecond, 0 in case of return immediately with timeout status
 * @param  ret return value, it will return OM_ERROR_OK in to_ms, else return OM_ERROR_TIMEOUT
 *******************************************************************************
 */
#define DRV_DWT_WAIT_MS_UNTIL_TO(wait_val, to_ms, ret)                         \
    do {                                                                       \
        uint32_t to;                                                           \
        ret = OM_ERROR_OK;                                                     \
        to = (uint32_t)(to_ms);                                                \
        if (to != DRV_MAX_DELAY) {                                             \
            uint32_t start, target;                                            \
            if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {                       \
                CoreDebug->DEMCR |= (1U << CoreDebug_DEMCR_TRCENA_Pos);        \
                DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;                           \
            }                                                                  \
            start  = drv_dwt_get_cycle();                                      \
            target = DRV_DWT_MS_2_CYCLES_CEIL(to);                             \
            while (wait_val) {                                                 \
                if ((drv_dwt_get_cycle() - start) > target) {                  \
                    ret = OM_ERROR_TIMEOUT;                                    \
                    break;                                                     \
                }                                                              \
            }                                                                  \
        } else {                                                               \
            while (wait_val);                                                  \
        }                                                                      \
    } while(0)

/**
 *******************************************************************************
 * @brief  continus check wait_val using dwt,
 *
 * @param  wait_val wait value
 * @param  to_us timeout value in macrosecond, 0 in case of return immediately with timeout status
 * @param  ret return value, it will return OM_ERROR_OK in to_us, else return OM_ERROR_TIMEOUT
 *******************************************************************************
 */
#define DRV_DWT_WAIT_US_UNTIL_TO(wait_val, to_us, ret)                         \
    do {                                                                       \
        uint32_t to;                                                           \
        ret = OM_ERROR_OK;                                                     \
        to = (uint32_t)(to_us);                                                \
        if (to != DRV_MAX_DELAY) {                                             \
            uint32_t start, target;                                            \
            if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {                       \
                CoreDebug->DEMCR |= (1U << CoreDebug_DEMCR_TRCENA_Pos);        \
                DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;                           \
            }                                                                  \
            start  = drv_dwt_get_cycle();                                      \
            target = DRV_DWT_US_2_CYCLES_CEIL(to);                             \
            while (wait_val) {                                                 \
                if ((drv_dwt_get_cycle() - start) > target) {                  \
                    ret = OM_ERROR_TIMEOUT;                                    \
                    break;                                                     \
                }                                                              \
            }                                                                  \
        } else {                                                               \
            while (wait_val);                                                  \
        }                                                                      \
    } while(0)

#endif  /* RTE_CORTEX_DWT_TIMEOUT */

/// Calculate BEGIN-END delay time (init)
#define DRV_DWT_DELAY_CALC_INIT() \
    uint32_t __delay_cycle, __delay_us; \
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; \
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk
/// Calculate BEGIN-END delay time (BEGIN)
#define DRV_DWT_DELAY_CALC_BEGIN() \
     __delay_cycle = DWT->CYCCNT;
/// Calculate BEGIN-END delay time (END with us)
#define DRV_DWT_DELAY_CALC_END() \
    (__delay_cycle = DWT->CYCCNT - __delay_cycle, __delay_us = __delay_cycle / (SystemCoreClock/1000000) )
/// Calculate BEGIN-END delay time (END with cycle)
#define DRV_DWT_DELAY_CALC_CYCLE_END() \
    (__delay_cycle = DWT->CYCCNT - __delay_cycle, __delay_cycle)


/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */

/**
 *******************************************************************************
 * @brief delay some cycles using DWT timer
 *
 * @param cycles Delay cycles
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_dwt_delay_cycles(uint32_t cycles)
{
    if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
        CoreDebug->DEMCR |= (1U << CoreDebug_DEMCR_TRCENA_Pos);
        // enable DWT peripheral, enable Cortex-M DWT Cycle counter
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }

    uint32_t start_cnt = DWT->CYCCNT;
    while ((uint32_t)(DWT->CYCCNT - start_cnt) <= cycles);
}

/**
 *******************************************************************************
 * @brief delay microseconds using DWT timer
 *
 * @param us            Delay microseconds, SystemCoreClock < 64MHz,
 *                      so us must be less than 2^32 / 64 = 2^(32-6) = 2^26
 *                      max 0x4000000/1000(ms)/1000(s) = 67.11s
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_dwt_delay_us(uint32_t us)
{
    uint32_t cycles;

    cycles = DRV_DWT_US_2_CYCLES_CEIL(us);
    drv_dwt_delay_cycles(cycles);
}

/**
 *******************************************************************************
 * @brief delay milliseconds using DWT timer
 *
 * @param ms            Delay milliseconds, SystemCoreClock < 64MHz,
 *                      max delay time (2^32 / 64)/1000(ms)/1000(s) = 67.11s
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_dwt_delay_ms(uint32_t ms)
{
    uint32_t cycles;

    cycles = DRV_DWT_MS_2_CYCLES_CEIL(ms);
    drv_dwt_delay_cycles(cycles);
}

/**
 *******************************************************************************
 * @brief Get current DWT cycle count
 *
 * @return Current DWT count
 *******************************************************************************
 */
__STATIC_FORCEINLINE uint32_t drv_dwt_get_cycle(void)
{
    return DWT->CYCCNT;
}

#endif  /* RTE_CORTEX_DWT */


/**
 *******************************************************************************
 * @brief  drv irq is ext pending
 *
 * @return pending ?
 *******************************************************************************
 */
__STATIC_FORCEINLINE bool drv_irq_is_any_ext_pending(void)
{
    uint32_t nvic_reg_num = EXTERNAL_IRQn_Num/32 + ((EXTERNAL_IRQn_Num%32) ? 1 : 0) ;
    uint32_t i;

    for (i=0; i<nvic_reg_num; ++i) {
        // NVIC_GetPendingIRQ & NVIC_GetEnableIRQ
        if (NVIC->ISPR[i] & NVIC->ISER[i])
            return true;
    }

    return false;
}

#ifdef __cplusplus
}
#endif

#endif  /* __DRV_CORTEX_H */


/** @} */
