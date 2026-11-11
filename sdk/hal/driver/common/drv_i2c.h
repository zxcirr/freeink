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
 * @file     drv_i2c.h
 * @brief    header file for i2c
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup I2C I2C
 * @ingroup  DRIVER
 * @brief    I2C Driver for om662x
 * @details  I2C Driver for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 *
 * @example example_i2c.c
 * This is an example of how to use the i2c
 *
 */

#ifndef __DRV_I2C_H
#define __DRV_I2C_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_I2C0)
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
/// I2C Mode
typedef enum {
    /// Master Mode, 7-bit addressing mode
    I2C_MODE_MASTER       = 0,
    /// Master Mode, 10-bit addressing mode
    I2C_MODE_SMBUS_HOST   = 1,
} i2c_mode_t;

/// I2C Speed
typedef enum {
    /// Speed: 100khz
    I2C_SPEED_100K = 1,
    /// Speed: 400khz
    I2C_SPEED_400K = 2,
    /// Speed: 1Mhz
    I2C_SPEED_1M   = 3,
    /// Speed: 2Mhz
    I2C_SPEED_2M   = 4,
    /// Speed: MAX
    I2C_SPEED_MAX  = 5,
} i2c_speed_t;

/// I2C Config
typedef struct {
    i2c_mode_t  mode;
    i2c_speed_t speed;
} i2c_config_t;

/// I2C Control
typedef enum {
    /// check I2C is busy
    I2C_CONTROL_IS_BUSY         = 0U,
    /// check device is valid, argu is i2c_addr
    I2C_CONTROL_DEV_IS_VALID    = 1U,
} i2c_control_t;

/// I2C DMA Channel
typedef enum {
    /// dma channel for i2c sending
    I2C_DMA_TX_CHAN             = 0U,
    /// dma channel for i2c reception
    I2C_DMA_RX_CHAN             = 1U,
    /// dma channel for i2c sending and reception
    I2C_DMA_CHAN_ALL            = 2U,
} i2c_dma_chan_t;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief I2C initialization
 *
 * @param[in] om_i2c         Pointer to I2C
 * @param[in] cfg            Configuration for I2C
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_i2c_init(OM_I2C_Type *om_i2c, const i2c_config_t *cfg);

#if (RTE_I2C_REGISTER_CALLBACK)
/**
 *******************************************************************************
 * @brief Register event callback for transmit/receive by interrupt & dma mode
 *
 * @param[in] om_i2c         Pointer to I2C
 * @param[in] event_cb       Pointer to callback
 *******************************************************************************
 */
extern void drv_i2c_register_isr_callback(OM_I2C_Type                   *om_i2c,
                                          drv_isr_callback_t          event_cb);
#endif

/**
 *******************************************************************************
 * @brief The interrupt callback for I2C driver. It is a weak function. User should define
 *        their own callback in user file, other than modify it in the I2C driver.
 *
 * @param om_i2c            The I2C device address
 * @param event             The I2C usart event
 *                           - DRV_EVENT_COMMON_WRITE_COMPLETED
 *                           - DRV_EVENT_COMMON_READ_COMPLETED
 *                           - DRV_EVENT_COMMON_ABORT
 *                           - DRV_EVENT_COMMON_ERROR
 *                           - DRV_EVENT_COMMON_DMA2PERIPH_COMPLETED
 * @param data              The data pointer of data to be read or write
 * @param num               The data buffer valid data count
 *******************************************************************************
 */
extern __WEAK void drv_i2c_isr_callback(OM_I2C_Type *om_i2c, drv_event_t event, uint8_t *data, uint32_t num);

/**
 *******************************************************************************
 * @brief I2C master start sending data to slave device by block mode.
 *
 * @param[in]   om_i2c       Pointer to I2C
 * @param[in]   dev_addr     Slave device address
 * @param[in]   tx_data      Pointer to buffer with data to send
 * @param[in]   tx_num       Number of data items to send
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_i2c_master_write(OM_I2C_Type                      *om_i2c,
                                       uint16_t                        dev_addr,
                                       const uint8_t                   *tx_data,
                                       uint32_t                          tx_num,
                                       uint32_t                     timeout_ms);

/**
 *******************************************************************************
 * @brief I2C master start sending data to slave device by interrupt mode.
 *
 * @param[in]   om_i2c       Pointer to I2C
 * @param[in]   dev_addr     Slave device address
 * @param[in]   tx_data      Pointer to buffer with data to send
 * @param[in]   tx_num       Number of data items to send
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_i2c_master_write_int(OM_I2C_Type                  *om_i2c,
                                           uint16_t                    dev_addr,
                                           const uint8_t               *tx_data,
                                           uint32_t                      tx_num,
                                           uint32_t                 timeout_ms);

#if (RTE_DMA)
/**
 *******************************************************************************
 * @brief Allocate dma channel for i2c
 *
 * @param[in] om_i2c    Pointer for I2C
 * @param[in] channel   I2C rx/tx channel
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_i2c_dma_channel_allocate(OM_I2C_Type *om_i2c, i2c_dma_chan_t channel);

/**
 *******************************************************************************
 * @brief Release dma channel for i2c
 *
 * @param[in] om_i2c    Pointer for I2C
 * @param[in] channel   I2C rx/tx channel
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_i2c_dma_channel_release(OM_I2C_Type *om_i2c, i2c_dma_chan_t channel);

/**
 *******************************************************************************
 * @brief I2C master start sending data to slave device by interrupt dma mode.
 *
 * @param[in]   om_i2c       Pointer to I2C
 * @param[in]   dev_addr     Slave device address
 * @param[in]   tx_data      Pointer to buffer with data to send
 * @param[in]   tx_num       Number of data items to send
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_i2c_master_write_dma(OM_I2C_Type                  *om_i2c,
                                           uint16_t                    dev_addr,
                                           const uint8_t               *tx_data,
                                           uint32_t                      tx_num,
                                           uint32_t                 timeout_ms);
#endif

/**
 *******************************************************************************
 * @brief I2C master start receiving data from slave device by block mode.
 *
 * @param[in]   om_i2c       Pointer to I2C
 * @param[in]   dev_addr     Slave device address
 * @param[in]   tx_data      Pointer to buffer with data to send
 * @param[in]   tx_num       Number of data items to send
 * @param[out]  rx_data      Pointer to buffer with data to receive
 * @param[in]   rx_num       Number of data items to receive
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_i2c_master_read(OM_I2C_Type                       *om_i2c,
                                      uint16_t                         dev_addr,
                                      const uint8_t                    *tx_data,
                                      uint16_t                           tx_num,
                                      uint8_t                          *rx_data,
                                      uint16_t                           rx_num,
                                      uint32_t                      timeout_ms);
/**
 *******************************************************************************
 * @brief I2C master start receiving data from slave device by interrupt mode.
 *
 * @param[in]   om_i2c       Pointer to I2C
 * @param[in]   dev_addr     Slave device address
 * @param[in]   tx_data      Pointer to buffer with data to send
 * @param[in]   tx_num       Number of data items to send(tx_num <= 16)
 * @param[out]  rx_data      Pointer to buffer with data to receive
 * @param[in]   rx_num       Number of data items to receive
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_i2c_master_read_int(OM_I2C_Type                   *om_i2c,
                                          uint16_t                     dev_addr,
                                          const uint8_t                *tx_data,
                                          uint16_t                       tx_num,
                                          uint8_t                      *rx_data,
                                          uint16_t                       rx_num,
                                          uint32_t                  timeout_ms);
#if (RTE_DMA)
/**
 *******************************************************************************
 * @brief I2C master start receiving data from slave device by dma mode.
 *
 * @param[in]   om_i2c       Pointer to I2C
 * @param[in]   dev_addr     Slave device address
 * @param[in]   tx_data      Pointer to buffer with data to send
 * @param[in]   tx_num       Number of data items to send
 * @param[out]  rx_data      Pointer to buffer with data to receive
 * @param[in]   rx_num       Number of data items to receive
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_i2c_master_read_dma(OM_I2C_Type                   *om_i2c,
                                          uint16_t                     dev_addr,
                                          const uint8_t                *tx_data,
                                          uint16_t                       tx_num,
                                          uint8_t                      *rx_data,
                                          uint16_t                       rx_num,
                                          uint32_t                  timeout_ms);
#endif

/**
 *******************************************************************************
 * @brief Control i2c
 *
 * @param[in] om_i2c      Pointer to I2C
 * @param[in] control:    Control command
 * @param[in] argu:       Control argument
 *
 * @return control status
 *******************************************************************************
 */
extern void *drv_i2c_control(OM_I2C_Type *om_i2c, i2c_control_t control, void *argu);

/**
 *******************************************************************************
 * @brief I2c interrupt service routine
 *
 * @param[in]   om_i2c       Pointer to I2C
 *******************************************************************************
 */
extern void drv_i2c_isr(OM_I2C_Type *om_i2c);

/**
 *******************************************************************************
 * @brief Get I2C TX ABRT SOURCE
 *
 * @param[in] om_i2c        Pointer to I2C
 *
 * @return I2C TX ABRT SOURCE value
 *******************************************************************************
 */
__STATIC_FORCEINLINE uint32_t drv_i2c_get_tx_abrt(OM_I2C_Type *om_i2c)
{
    uint32_t result;
    uint32_t state;
    result = (om_i2c->RAW_INTR_STAT & I2C_INTR_TX_ABRT_MASK)&&(om_i2c->TX_ABRT_SOURCE & (I2C_TX_ABRT_SRC_7B_ADDR_NOACK_MASK | I2C_TX_ABRT_SRC_10ADDR1_NOACK_MASK | I2C_TX_ABRT_SRC_10ADDR2_NOACK_MASK | I2C_TX_ABRT_SRC_TXDATA_NOACK_MASK));
    state = (result != 0) ? 1 : 0;
    return (state);

}

/**
 *******************************************************************************
 * @brief Get I2C TIMEOUT RAW INTR STAT
 *
 * @param[in] om_i2c        Pointer to I2C
 *
 * @return I2C TIMEOUT RAW INTR STAT value
 *******************************************************************************
 */
__STATIC_FORCEINLINE uint32_t drv_i2c_get_timeout_rawstate(OM_I2C_Type *om_i2c)
{
    uint32_t state;
    state = ((om_i2c->RAW_INTR_STAT & I2C_INTR_TIME_OUT_MASK) != 0) ? 1 : 0;
    return (state);
}

/**
 *******************************************************************************
 * @brief Get I2C RX UNDER RAW INTR STAT
 *
 * @param[in] om_i2c        Pointer to I2C
 *
 * @return I2C RX UNDER RAW INTR STAT value
 *******************************************************************************
 */
__STATIC_FORCEINLINE uint32_t drv_i2c_get_rxunder_rawstate(OM_I2C_Type *om_i2c)
{
    uint32_t state;
    state = ((om_i2c->RAW_INTR_STAT & I2C_INTR_RX_UNDER_MASK) != 0) ? 1 : 0;
    return (state);
}

#ifdef __cplusplus
}
#endif


#endif  /* RTE_I2C0 */

#endif  /* __DRV_I2C_H */


/** @} */