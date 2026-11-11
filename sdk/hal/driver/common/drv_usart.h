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
 * @file     drv_usart.h
 * @brief    Header file of USART HAL module.
 * @date     27. March 2020
 * @author   OnMicro SW Team
 *
 * @defgroup USART USART
 * @ingroup  DRIVER
 * @brief    USART Driver for om662x
 * @details  USART Driver for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 *
 * @example example_uart.c
 * This is an example of how to use the uart
 *
 */


#ifndef __DRV_USART_H
#define __DRV_USART_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_USART1)
#include <stdint.h>
#include "om_driver.h"

#ifdef  __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */
/// USART Hardware Flow Control
typedef enum {
    /// No hardware control
    USART_FLOW_CONTROL_NONE    = 0,
    /// Request and Clear To Send
    USART_FLOW_CONTROL_RTS_CTS = 1,
} usart_flow_control_t;

/// USART Number of Stop Bits
typedef enum {
    /// USART frame with 1 stop bit
    USART_STOP_BIT_1   = 0,
    /// USART frame with 2 stop bit
    USART_STOP_BIT_2   = 1,
    /// USART frame with 1.5 stop bit
    USART_STOP_BIT_1_5 = 2,
} usart_stop_bit_t;

/// USART Number of Data Bits
typedef enum {
    /// 5-bit long USART frame
    USART_DATA_BIT_5   = 0,
    /// 6-bit long USART frame
    USART_DATA_BIT_6   = 1,
    /// 7-bit long USART frame
    USART_DATA_BIT_7   = 2,
    /// 8-bit long USART frame
    USART_DATA_BIT_8   = 3,
} usart_data_bit_t;

/// USART Parity
typedef enum {
    /// No parity
    USART_PARITY_NONE  = 0,
    /// Odd parity
    USART_PARITY_ODD   = 1,
    /// Even parity
    USART_PARITY_EVEN  = 2,
} usart_parity_t;

/// USART DMA Channel
typedef enum {
    /// dma channel for usart sending
    USART_DMA_TX_CHAN       = 0U,
    /// dma channel for usart reception
    USART_DMA_RX_CHAN       = 1U,
    /// dma channel for usart sending and reception
    USART_DMA_CHAN_ALL      = 2U,
} usart_dma_chan_t;

/// USART config
typedef struct {
    /// Configures USART communication baud rate.
    uint32_t                baudrate;
    /// Specifies whether the hardware flow control is enabled or disabled.
    usart_flow_control_t    flow_control;
    /// Specifies the number of data bits transmitted or received in a frame.
    usart_data_bit_t        data_bit;
    /// Specifies the number of stop bits transmitted.
    usart_stop_bit_t        stop_bit;
    /// Specifies the parity mode.
    usart_parity_t          parity;
} usart_config_t;

/// USART control
typedef enum {
    USART_CONTROL_RESET             = 0U,    /*!< Reset USART. argu with NULL, return OM_ERROR_OK */
    USART_CONTROL_CLK_DISABLE       = 1U,    /*!< Disable USART clock. argu with NULL, return OM_ERROR_OK */
    USART_CONTROL_CLK_ENABLE        = 2U,    /*!< Enable USART clock. argu with NULL, return OM_ERROR_OK  */
    USART_CONTROL_BREAK             = 3U,    /*!< Nothing todo */
    USART_CONTROL_ABORT_TRANSMIT    = 4U,    /*!< Abort transmit. argu with NULL, return OM_ERROR_OK */
    USART_CONTROL_ABORT_RECEIVE     = 5U,    /*!< Abort receive, argu with NULL, return OM_ERROR_OK */
    USART_CONTROL_SET_BAUDRATE      = 6U,    /*!< Set USART baudrate, argu with uint32_t type, return OM_ERROR_OK */
    USART_CONTROL_GET_CAP           = 7U,    /*!< Get capability, argu with NULL, return capability, bit mask defined in om662x.h */
    USART_CONTROL_IS_BUSY           = 8U,    /*!< Check USART Tx is busy, argu with NULL, return 0 indicate Not Busy, else busy */
    USART_CONTROL_GET_TX_COUNT      = 9U,    /*!< Get Tx count, argu with NULL, return Transmited count in bytes */
    USART_CONTROL_GET_RX_COUNT      = 10U,   /*!< Get Rx count, argu with NULL, return Received count in bytes */
    USART_CONTROL_FLOWCTRL_ENABLE   = 11U,   /*!< Added Flow control, argu is 1: enable flowctrl, 0: disable flowctrl. */
    USART_CONTROL_IRQ_ENABLE        = 12U,   /*!< Enable USART TX/RX irq */
    USART_CONTROL_IRQ_DISABLE       = 13U,   /*!< Enable USART TX/RX irq */
} usart_control_t;


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief USART initialization
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] usart_cfg      Configuration for usart
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_usart_init(OM_USART_Type        *om_usart,
                                 const usart_config_t *usart_cfg);

#if (RTE_USART_REGISTER_CALLBACK)
/**
 *******************************************************************************
 * @brief Register event callback for transmit/receive by interrupt & dma mode
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] cb             Pointer to callback
 *******************************************************************************
 */
extern void drv_usart_register_isr_callback(OM_USART_Type            *om_usart,
                                            drv_isr_callback_t              cb);
#endif

/**
 *******************************************************************************
 * @brief The interrupt callback for UART driver. It is a weak function. User should define
 *        their own callback in user file, other than modify it in the UART driver.
 *
 * @param om_usart          The USART device address
 * @param event             The driver usart event
 *                           - DRV_EVENT_COMMON_WRITE_COMPLETED
 *                           - DRV_EVENT_COMMON_READ_COMPLETED
 *                           - DRV_EVENT_USART_RX_TIMEOUT
 *                           - DRV_EVENT_COMMON_RX_OVERFLOW
 *                           - DRV_EVENT_USART_RX_PARITY_ERROR
 *                           - DRV_EVENT_USART_RX_BREAK
 *                           - DRV_EVENT_USART_RX_FRAME_ERROR
 * @param data              The data pointer of data to be read or write
 * @param num               The data buffer valid data count
 *******************************************************************************
 */
extern __WEAK void drv_usart_isr_callback(OM_USART_Type *om_usart, drv_event_t event, uint8_t *data, uint32_t num);

/**
 *******************************************************************************
 * @brief Transmit number of bytes from USART by block mode
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] data           Pointer to data buffer
 * @param[in] num            Number of data bytes to be sent
 * @param[in] timeout_ms     time out(ms)
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_usart_write(OM_USART_Type *om_usart,
                                  const uint8_t     *data,
                                  uint16_t           num,
                                  uint32_t      timeout_ms);

/**
 *******************************************************************************
 * @brief Transmit number of bytes from USART by interrupt mode
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] data           Pointer to data buffer
 * @param[in] num            Number of data bytes to be sent
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_usart_write_int(OM_USART_Type *om_usart,
                                      const uint8_t     *data,
                                      uint16_t            num);

#if (RTE_DMA)
/**
 *******************************************************************************
 * @brief Allocate usart dma channel
 *
 * @param[in] om_usart    Pointer to usart
 * @param[in] channel     USART rx/tx channel
 *
 * @return errno
 *******************************************************************************
 */
om_error_t drv_usart_dma_channel_allocate(OM_USART_Type *om_usart, usart_dma_chan_t channel);

/**
 *******************************************************************************
 * @brief Release usart dma channel
 *
 * @param[in] om_usart    Pointer to usart
 * @param[in] channel     USART rx/tx channel
 *
 * @return errno
 *******************************************************************************
 */
om_error_t drv_usart_dma_channel_release(OM_USART_Type *om_usart, usart_dma_chan_t channel);

/**
 *******************************************************************************
 * @brief Transmit number of bytes from USART by DMA mode
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] data           Pointer to data buffer
 * @param[in] num            Number of data bytes to be sent
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_usart_write_dma(OM_USART_Type *om_usart, const uint8_t *data, uint16_t num);
#endif  /* (RTE_DMA) */

/**
 *******************************************************************************
 * @brief Prepare receive number of bytes by block mode
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] data           Pointer to data buffer
 * @param[in] num            Number of data bytes to be received
 * @param[in] timeout_ms     time out(ms)
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_usart_read(OM_USART_Type *om_usart, uint8_t *data, uint16_t num, uint32_t timeout_ms);

/**
 *******************************************************************************
 * @brief Prepare receive number of bytes by interrupt mode, if num is 0, usart would
 *        receive data until drv_usart_control with USART_CONTROL_ABORT_RECEIVE is called.
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] data           Pointer to data buffer
 * @param[in] num            Number of data bytes to be received
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_usart_read_int(OM_USART_Type *om_usart, uint8_t *data, uint16_t num);

#if (RTE_DMA)
/**
 *******************************************************************************
 * @brief Prepare receive number of bytes by DMA mode
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] data           Pointer to data buffer
 * @param[in] num            Number of data bytes to be received
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_usart_read_dma(OM_USART_Type *om_usart, uint8_t *data, uint16_t num);
#endif  /* (RTE_DMA) */

/**
 *******************************************************************************
 * @brief Control USART interface.
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] control        Operation
 * @param[in] argu           Operation argument
 *
 * @return status:           Control status
 *******************************************************************************
 */
extern void *drv_usart_control(OM_USART_Type *om_usart, usart_control_t control, void *argu);

/**
 *******************************************************************************
 * @brief usart interrupt service routine
 *
 * @param[in] om_usart       Pointer to USART
 *
 *******************************************************************************
 */
extern void drv_usart_isr(OM_USART_Type *om_usart);


#ifdef  __cplusplus
}
#endif

#endif  /* (RTE_USART1) */

#endif /* __DRV_USART_H */


/** @} */

