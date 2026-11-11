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
 * @file     drv_common.h
 * @brief    Header file for driver common defination
 * @date     3 Feb 2020
 * @author   OnMicro SW Team
 *
 * @defgroup DRIVER_COMMON DRIVER_COMMON
 * @ingroup  DRIVER
 * @brief    Common Typedefs for Driver
 * @details  Common Typedefs for Driver
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


#ifndef __DRV_COMMON_H
#define __DRV_COMMON_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#include <stdint.h>
#include "om_device.h"


#ifdef  __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */

/// Invalid TIME
#define DRV_INVALID_TIME           0xFFFFFFFF
/// Invalid TEMPERATURE
#define DRV_INVALID_TEMPERATURE    0xFF

/// Peripheral driver timeout max delay
#define DRV_MAX_DELAY              (0xFFFFFFFFU/2 - 1)

/// BIT Mask defined
#define BITMASK(n)                 (1U << (n))

/// Used for begin in driver isr
#define DRV_IRQ_BEGIN()                                                         \
    do {                                                                        \
        /* TODO [os specific code] */                                           \

/// Used for end in driver isr
#define DRV_IRQ_END()                                                           \
        /* TODO [os specific code] */                                           \
    } while(0)


/*******************************************************************************
 * TYPEDEFS
 */
/// General peripheral driver event, [0-15]： driver common event: [0-3] common error event, [4-8] common event
typedef enum {
    DRV_EVENT_COMMON_NONE                   = 0,
    DRV_EVENT_COMMON_ERROR                  = (1U << 0),               /**< Common error */
    DRV_EVENT_COMMON_ABORT                  = (1U << 1),               /**< Abort transmit/receive/read/write event */

    DRV_EVENT_COMMON_GENERAL                = (1U << 4),               /**< used for General event */
    DRV_EVENT_COMMON_WRITE_COMPLETED        = (1U << 5),               /**< write completed, from system to peripheral direction */
    DRV_EVENT_COMMON_TRANSMIT_COMPLETED     = (1U << 5),               /**< transmit completed, from system to peripheral direction */
    DRV_EVENT_COMMON_READ_COMPLETED         = (1U << 6),               /**< read completed, from peripheral to system direction */
    DRV_EVENT_COMMON_RECEIVE_COMPLETED      = (1U << 6),               /**< read completed, from peripheral to system direction */
    DRV_EVENT_COMMON_TRANSFER_COMPLETED     = (1U << 5) + (1U << 6),   /**< read and write completed, such as SPI peripheral */
    DRV_EVENT_COMMON_RX_OVERFLOW            = (1U << 7),               /**< Received FIFO overflow event */
    DRV_EVENT_COMMON_DMA2PERIPH_COMPLETED   = (1U << 8),               /**< system DMA to peripheral completed event, from system to peripheral direction */
    DRV_EVENT_I2C_TIMEOUT                   = (1U << 9),               /**< I2C timeout event */
    DRV_EVENT_I2C_TXADDR_NACK               = (1U << 10),              /**< I2C tx addr nack event  */
    DRV_EVENT_I2C_RXADDR_NACK               = (1U << 11),              /**< I2C rx addr nack event  */
    DRV_EVENT_I2C_TXDATA_NACK               = (1U << 12),              /**< I2C tx data nack event  */
    DRV_EVENT_I2C_RXDATA_UNDER              = (1U << 13),              /**< I2C rx data not exist event  */

    /*!< DMA */
    DRV_DMA_EVENT_TERMINAL_COUNT_REQUEST    = (1U << 16),
    DRV_DMA_EVENT_ERROR                     = (1U << 17),
    DRV_DMA_EVENT_ABORT                     = (1U << 18),
    /*!< USART */
    DRV_EVENT_USART_RX_TIMEOUT              = (1U << 16),
    DRV_EVENT_USART_RX_BREAK                = (1U << 17),
    DRV_EVENT_USART_RX_FRAME_ERROR          = (1U << 18),
    DRV_EVENT_USART_RX_PARITY_ERROR         = (1U << 19),
    DRV_EVENT_USART_CTS                     = (1U << 20),
    DRV_EVENT_USART_DSR                     = (1U << 21),
    DRV_EVENT_USART_DCD                     = (1U << 22),
    DRV_EVENT_USART_RI                      = (1U << 23),
    /*!< TIMER */
    DRV_EVENT_TIMER_UPDATE                  = (1U << 16),
    DRV_EVENT_TIMER_CC1                     = (1U << 17),
    DRV_EVENT_TIMER_CC2                     = (1U << 18),
    DRV_EVENT_TIMER_CC3                     = (1U << 19),
    DRV_EVENT_TIMER_CC4                     = (1U << 20),
    DRV_EVENT_TIMER_TRIGGER                 = (1U << 21),
    DRV_EVENT_TIMER_CASCADE_UPDATE          = (1U << 22),
    DRV_EVENT_TIMER_CC1_OVERFLOW            = (1U << 24),
    DRV_EVENT_TIMER_CC2_OVERFLOW            = (1U << 25),
    DRV_EVENT_TIMER_CC3_OVERFLOW            = (1U << 26),
    DRV_EVENT_TIMER_CC4_OVERFLOW            = (1U << 27),
    DRV_EVENT_TIMER_DMA_COMPLETE            = (1U << 28),
    /*!< LP TIMER */
    DRV_EVENT_LP_TIMER_COMP0                = (1U << 16),
    DRV_EVENT_LP_TIMER_COMP1                = (1U << 17),
    DRV_EVENT_LP_TIMER_UNDER_FLOW           = (1U << 18),
    DRV_EVENT_LP_TIMER_REP0                 = (1U << 19),
    DRV_EVENT_LP_TIMER_REP1                 = (1U << 20),
} drv_event_t;

/**
 * @brief general driver isr event callback prototype
 *
 * @param[in] om_reg: pointer to peripheral registers base(DMA channel base for dma peripheral)
 * @param[in] event:  driver event
 * @param[in] param0: refer to sepcific scenarios
 * @param[in] param1: refer to sepcific scenarios
 */
typedef void (*drv_isr_callback_t)(void *om_reg, drv_event_t event, void *param0, void *param1);

/// General peripheral driver environment
typedef struct {
    drv_isr_callback_t           isr_cb;          /**< event callback                      */
    uint16_t                     tx_num;          /**< Total number of data to be send     */
    uint16_t                     tx_cnt;          /**< Count of data sent*/
    uint8_t                     *tx_buf;          /**< Pointer to out data buffer, system to peripheral direction */
    uint16_t                     rx_num;          /**< Total number of data to be received, peripheral to system direction */
    uint16_t                     rx_cnt;          /**< Count of data received, peripheral to system direction */
    uint8_t                     *rx_buf;          /**< Pointer to in data buffer            */
    volatile uint8_t             busy;            /**< Indicate device busy or not          */
    #if (RTE_DMA)
    uint8_t                      dma_tx_chan;     /**< peripheral dma tx channel, from system to peripheral direction */
    uint8_t                      dma_rx_chan;     /**< peripheral dma rx channel, from peripheral to system direction */
    #endif  /* (RTE_DMA) */
} drv_env_t;

#if (RTE_DMA)
/// DMA info for driver
typedef struct {
    uint8_t                      id;                /**< dma_id         */
    uint8_t                      prio;              /**< dma_priority_t */
} drv_dma_t;
#endif  /* (RTE_DMA) */

/// Peripheral resource description
typedef struct {
    uint32_t                     cap;               /**< capabilities               */
    void                        *reg;               /**< peripheral registers base  */
    void                        *env;               /**< peripheral environment     */
    IRQn_Type                    irq_num;           /**< peripheral IRQn_Type       */
    uint8_t                      irq_prio;          /**< peripheral irq priority    */
    #if (RTE_DMA)
    drv_dma_t                    dma_tx;          /**< peripheral dma tx info, from system to peripheral direction */
    drv_dma_t                    dma_rx;          /**< peripheral dma rx info, form peripheral to system direction */
    #endif  /* (RTE_DMA) */
} drv_resource_t;


#ifdef __cplusplus
}
#endif

#endif  /* __DRV_COMMON_H */


/** @} */

