
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
 * @file     drv_dma.h
 * @brief
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup DMA DMA
 * @ingroup  DRIVER
 * @brief    DMA Driver for om662x
 * @details  DMA Driver for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 *
 * @example example_dma.c
 * This is an example of how to use the dma
 *
 */


#ifndef __DRV_DMA_H
#define __DRV_DMA_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_DMA)
#include <stdint.h>
#include "om_device.h"
#include "om_driver.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
/// Number of DMA channels
#define DMA_NUMBER_OF_CHANNELS          (8)
/// Max trans bytes = max_trans_size * src_width
#define DMA_CHAN_MAX_TRANS_SIZE         (0x3FFFFF)
/// DMA Control Setting
#define DMA_SET_CTRL(src_addr_ctrl, dst_addr_ctrl, src_width, dst_width, src_burst_size, priority) \
          (((src_addr_ctrl)  << DMA_SRCADDRCTRL_POS)   |   \
           ((dst_addr_ctrl)  << DMA_DSTADDRCTRL_POS)   |   \
           ((src_width)      << DMA_SRCWIDTH_POS)      |   \
           ((dst_width)      << DMA_DSTWIDTH_POS)      |   \
           ((src_burst_size) << DMA_SRCBURSTSIZE_POS)  |   \
           ((priority)       << DMA_PRIORITY_POS))


/*******************************************************************************
 * TYPEDEFS
 */
/// DMA Event Callback
typedef void (*dma_event_callback_t)(void *cb_param, drv_event_t event, void *next_chain);

/// DMA Address Config
typedef enum {
    /// Address Increase
    DMA_ADDR_CTRL_INC    = 0x00,
    /// Address Decrease
    DMA_ADDR_CTRL_DEC    = 0x01,
    /// Address Fixed
    DMA_ADDR_CTRL_FIXED  = 0x02,
} dma_addr_ctrl_t;

/// DMA Transfer Width
typedef enum {
    /// 1 byte is transfered at a time
    DMA_TRANS_WIDTH_1B   = 0x00,
    /// 2 bytes are transfered at a time
    DMA_TRANS_WIDTH_2B   = 0x01,
    /// 4 bytes are transfered at a time
    DMA_TRANS_WIDTH_4B   = 0x02,
} dma_trans_width_t;

/// DMA Burst Size
typedef enum {
    /// 1-transfer per burst
    DMA_BURST_SIZE_1T     = 0x00,
    /// 2-transfer per burst
    DMA_BURST_SIZE_2T     = 0x01,
    /// 4-transfer per burst
    DMA_BURST_SIZE_4T     = 0x02,
    /// 8-transfer per burst
    DMA_BURST_SIZE_8T     = 0x03,
    /// 16-transfer per burst
    DMA_BURST_SIZE_16T    = 0x04,
    /// 32-transfer per burst
    DMA_BURST_SIZE_32T    = 0x05,
    /// 64-transfer per burst
    DMA_BURST_SIZE_64T    = 0x06,
    /// 128-transfer per burst
    DMA_BURST_SIZE_128T   = 0x07,
} dma_burst_size_t;

/// DMA Transfer Priority
typedef enum {
    /// Low Priority
    DMA_PRIORITY_LOW     = 0x00,
    /// High Priority
    DMA_PRIORITY_HIGH    = 0x01,
} dma_priority_t;

/// DMA Pheripheral Index
typedef enum {
    DMA_ID_USART1_TX    = 0,
    DMA_ID_USART1_RX    = 1,
    DMA_ID_I2C0_TX      = 2,
    DMA_ID_I2C0_RX      = 3,
    DMA_ID_SPI0_TX      = 4,
    DMA_ID_SPI0_RX      = 5,
    DMA_ID_SPI1_TX      = 6,
    DMA_ID_SPI1_RX      = 7,
    DMA_ID_TIMER0       = 8,
    DMA_ID_TIMER1       = 9,
    DMA_ID_TIMER2       = 10,
    DMA_ID_GPADC        = 11,
    /// Extended for identifying memory. Note that memory have no req/ack for DMA.
    DMA_ID_MEM          = 0xFE,

    DMA_ID_INVAILD      = 0xFF,
    DMA_ID_MAX          = 0xFF,
} dma_id_t;

/// DMA chain transfer, preload the next descriptor before the interrupt is generated
typedef struct {
    /// DMA Control
    uint32_t              ctrl;
    /// Source Address
    uint32_t              src_addr;
    /// Destination Address
    uint32_t              dst_addr;
    union {
        /// total number of transfered bytes
        uint32_t          size_byte;
        /// transSize = sizebyte / src_width
        uint32_t          trans_size;
    };
    /// Pointer to chain transfer item
    void                 *ll_ptr;
} dma_chain_trans_t;

/// DMA Configuration
typedef struct {
    /// Channel Ctrl
    uint32_t                 channel_ctrl;
    /// Pheripheral Source Index
    dma_id_t                 src_id;
    /// Pheripheral Destination Index
    dma_id_t                 dst_id;
    /// DMA Event Callback
    dma_event_callback_t     event_cb;
    /// Callback Parameter
    void                     *cb_param;
    /// Pointer to chain transfer item
    dma_chain_trans_t        *chain_trans;
    /// Number of chain transfer item
    uint8_t                  chain_trans_num;
} dma_config_t;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Allocate a new channel
 *
 * @returns:
 *          [0, DMA_NUMBER_OF_CHANNELS-1]:  DMA channel allocated
 *          DMA_NUMBER_OF_CHANNELS:         No DMA channel can be allocated
 *
 *******************************************************************************
 */
extern uint8_t drv_dma_channel_allocate(void);

/**
 *******************************************************************************
 * @brief Release a channel
 *
 * @param[in]   chan_idx        Channel index [0,DMA_NUMBER_OF_CHANNELS-1]
 *
 *******************************************************************************
 */
extern void drv_dma_channel_release(uint8_t chan_idx);

/**
 *******************************************************************************
 * @brief Configure DMA channel for next transfer
 * dma interrupt is enabled by default
 *
 * @param[in]   chan_idx        Channel index [0,DMA_NUMBER_OF_CHANNELS-1]
 * @param[in]   config          Channel config
 *
 * @return: errno
 *******************************************************************************
 */
extern om_error_t drv_dma_channel_config(uint8_t chan_idx, const dma_config_t *config);

/**
 *******************************************************************************
 * @brief Enable DMA channel for next transfer
 *
 * @param[in]   chan_idx        Channel index [0,DMA_NUMBER_OF_CHANNELS-1]
 * @param[in]   src_addr        Source address
 * @param[in]   dst_addr        Destination address
 * @param[in]   size            Amount of data to transfer in byte
 *
 * @return: errno
 *******************************************************************************
 */
extern om_error_t drv_dma_channel_enable(uint8_t chan_idx, uint32_t dst_addr, uint32_t src_addr, uint32_t size);

/**
 *******************************************************************************
 * @brief Disable DMA channel
 *
 * @param[in]   chan_idx        Channel index [0,DMA_NUMBER_OF_CHANNELS-1]
 *
 * @return: errno
 *******************************************************************************
 */
extern om_error_t drv_dma_channel_disable(uint8_t chan_idx);

/**
 *******************************************************************************
 * @brief Check if DMA channel is busy
 *
 * @param[in]   chan_idx        Channel index [0,DMA_NUMBER_OF_CHANNELS-1]
 *
 * @return:
 *         1          Channel is busy
 *         0          Channel is idle
 *******************************************************************************
 */
extern uint8_t drv_dma_channel_is_busy(uint8_t chan_idx);

/**
 *******************************************************************************
 * @brief Get the number of left data
 *
 * @param[in]   chan_idx        Channel index [0,DMA_NUMBER_OF_CHANNELS-1]
 *
 * @return:     left count
 *******************************************************************************
 */
extern uint32_t drv_dma_channel_get_left_count(uint8_t chan_idx);

/**
 *******************************************************************************
 * @brief Set DMA link list ptr
 *
 * @param[in] chan_idx   the channel index
 * @param[in] ptr        the link list ptr
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_dam_channel_set_ptr(uint8_t chan_idx, void *ptr)
{
    OM_DMA_CHAN_Type *dma_chan;

    dma_chan = &(OM_DMA->CHAN[chan_idx]);
    dma_chan->LL_PTR = (uint32_t)ptr;
}

/**
 *******************************************************************************
 * @brief dma interrupt service routine
 *
 *******************************************************************************
 */
extern void drv_dma_isr(void);

#ifdef __cplusplus
}
#endif

#endif   /* RTE_DMA */

#endif  /* __DRV_DMA_H */


/** @} */