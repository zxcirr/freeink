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
 * @file     drv_usart_ex.h
 * @brief    Header file of USART Extended HAL module
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup USART_EX USART EX
 * @ingroup  DRIVER
 * @brief    USART Extended Driver for om662x
 * @details  USART Extended Driver for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 *
 * @example example_uart_ex.c
 * This is an example of how to use the extended uart
 *
 */

#ifndef __DRV_USART_EX_H
#define __DRV_USART_EX_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_USART0)
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
/// USART Extended control
typedef enum {
    /// set tx_data bit8 level in mode2,mode3, set bit8 to 1 if argu0 not NULL and vice versa
    USART_EX_CTRL_TB8_LEVEL_SET      = 0U,
    /// return rx data bit8 level in mode2,mode3 or return stop bit level in mode1
    USART_EX_CTRL_RB8_LEVEL_GET      = 1U,
    /// set usart mode, argu0 is USART_EX_MODE1, USART_EX_MODE2 or USART_EX_MODE3, argu1 is baudrate.
    USART_EX_CTRL_MODE_BAUDRATE_SET  = 2U,
    /// enable multi-processor communication, receive data only when the data bit8 == 1
    USART_EX_CTRL_MULT_PROCESSOR_EN  = 3U,
    /// disable multi-processor communication, receive data regardless of whether the data bit8 is 1 or not
    USART_EX_CTRL_MULT_PROCESSOR_DIS = 4U,
    /// set the usart reload register, only 10 bits are used.
    USART_EX_CTRL_TIM_RELAOD_SET     = 5U,
} usart_ex_control_t;

/// USART Extended Mode
typedef enum {
    /// 8bit usart, the farme is 1bit start + 8bit data + 1bit stop, baud rate is variable
    USART_EX_MODE1  = 1,
    /// 9bit usart, the farme is 1bit start + 8bit data + 1bit programmable_bit + 1bit stop,
    /// when sending, bit8(programming bit) is controled by TB8, when receving, bit8 is saved in RB8, baudrate is fixed
    USART_EX_MODE2  = 2,
    /// 9bit usart, the difference with MODE2 is that the MODE3 baudrate is variable
    USART_EX_MODE3  = 3,
} usart_ex_mode_t;

/// USART Extended Configuration
typedef struct {
    uint32_t baudrate;
} usart_ex_config_t;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief config usart, this function enable MODE1 by default: 8bit data, 1bit stop
 *        bit,no parity bit, no flow control.
 *
 * @param[in] om_usart:  pointer to usart
 * @param[in] usart_cfg: configuration
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_usart_ex_init(OM_USART_EX_Type *om_usart, const usart_ex_config_t *usart_cfg);

#if (RTE_USART_REGISTER_CALLBACK)
/**
 *******************************************************************************
 * @brief register callback function.
 *
 * @param[in] om_usart: pointer to usart
 * @param[in] cb:       callback function
 *
 *******************************************************************************
 */
extern void drv_usart_ex_register_isr_callback(OM_USART_EX_Type *om_usart, drv_isr_callback_t cb);
#endif

/**
 *******************************************************************************
 * @brief The interrupt callback for UART EX driver. It is a weak function. User should define
 *        their own callback in user file, other than modify it in the UART EX driver.
 *
 * @param om_usart          The USART EX device address
 * @param event             The driver usart event
 *                           - DRV_EVENT_COMMON_READ_COMPLETED
 * @param data              The data pointer of data to be read
 * @param num               The data buffer valid data count
 *******************************************************************************
 */
__WEAK void drv_usart_ex_isr_callback(OM_USART_EX_Type *om_usart, drv_event_t event, uint8_t *data, uint16_t num);

/**
 *******************************************************************************
 * @brief Transmit number of bytes from USART by block mode. NOTE: cannot send
 *        and receive at the same time
 *
 * @param[in] om_usart:      pointer to usart
 * @param[in] data:          pointer to data buffer
 * @param[in] num:           num of data to be sent
 * @param[in] timeout_ms     time out(ms)
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_usart_ex_write(OM_USART_EX_Type *om_usart, const uint8_t *data, uint16_t num, uint32_t timeout_ms);

/**
 *******************************************************************************
 * @brief control usart
 *
 * @param[in] om_usart: pointer to usart
 * @param[in] control:  ctrl cmd
 * @param[in] argu0:    ctrl param0
 * @param[in] argu1:    ctrl param1
 *
 * @return : control status
 *******************************************************************************
 */
extern void *drv_usart_ex_control(OM_USART_EX_Type *om_usart, usart_ex_control_t control, void *argu0, void *argu1);

/**
 *******************************************************************************
 * @brief usart_ex interrupt service routine
 *
 * @param[in] om_usart: pointer to usart
 *
 *******************************************************************************
 */
extern void drv_usart_ex_isr(OM_USART_EX_Type *om_usart);


#ifdef __cplusplus
}
#endif

#endif /* RTE_USART0 */

#endif  /* __DRV_USART_EX_H */


/** @} */