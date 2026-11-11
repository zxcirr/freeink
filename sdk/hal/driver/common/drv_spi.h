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
 * @file     drv_spi.h
 * @brief    Header file of SPI HAL module
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup SPI SPI
 * @ingroup  DRIVER
 * @brief    SPI Driver for om662x
 * @details  SPI Driver for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 *
 * @example example_spi.c
 * This is an example of how to use the spi
 *
 */

#ifndef __DRV_SPI_H
#define __DRV_SPI_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_SPI0 || RTE_SPI1)
#include <stdint.h>
#include "om_driver.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
#ifndef SPI_CS_AUTO_CONTROL
    #define SPI_CS_AUTO_CONTROL     1
#endif

#if (SPI_CS_AUTO_CONTROL)
    #define SPI_CSN_LOW(OM_SPI)                                                 \
        do {                                                                    \
            (OM_SPI)->CSNCTRL = SPI_CSNCTRL_CS_MODE_MASK;                        \
        } while(0)

    #define SPI_CSN_HIGH(OM_SPI)                                                    \
        do {                                                                        \
            (OM_SPI)->CSNCTRL = SPI_CSNCTRL_CS_MODE_MASK +  SPI_CSNCTRL_CS_GPO_MASK;  \
        } while(0)
#else
    #define SPI_CSN_LOW(OM_SPI)
    #define SPI_CSN_HIGH(OM_SPI)
#endif


/*******************************************************************************
 * TYPEDEFS
 */
/// SPI Role
typedef enum {
    /// Slave
    SPI_ROLE_SLAVE  = 0,
    /// Master
    SPI_ROLE_MASTER = 1,
} spi_role_t;

/// Valid level of CS
typedef enum {
    /// Valid level of CS is Low
    SPI_CS_LOW      = 0,
    /// Valid level of CS is High
    SPI_CS_HIGH     = 1,
} spi_cs_t;

/// SPI Mode
typedef enum {
    /// MODE0: the idle time is low and the sampling starts from the first hop edge
    SPI_MODE_0      = 0,
    /// MODE1: the idle time is low and the sampling starts from the second hop edge
    SPI_MODE_1      = 1,
    /// MODE2: the idle time is high and the sampling starts from the first hop edge
    SPI_MODE_2      = 2,
    /// MODE3: the idle time is high and the sampling starts from the second hop edge
    SPI_MODE_3      = 3,

    SPI_MODE_NUM,
} spi_mode_t;

/// SPI Wire
typedef enum {
    /// 3 wire: CS<->CS, CLK<->CLK, DO<->DO
    SPI_WIRE_3      = 0,
    /// 4 wire: CS<->CS, CLK<->CLK, DO<->DI, DI<->DO
    SPI_WIRE_4      = 1,
} spi_wire_t;

/// SPI MSB LSB Transmission
typedef enum {
    /// data transfers start from LSB bit
    SPI_LSB_FIRST  = 0,
    /// data transfers start from MSB bit
    SPI_MSB_FIRST  = 1,
} spi_first_t;

/// SPI DMA Channel
typedef enum {
    /// dma channel for spi sending
    SPI_DMA_TX_CHAN         = 0U,
    /// dma channel for spi reception
    SPI_DMA_RX_CHAN         = 1U,
    /// dma channel for spi sending and reception
    SPI_DMA_CHAN_ALL        = 2U,
} spi_dma_chan_t;

/// SPI Control
typedef enum {
    SPI_CONTROL_CLK_DISABLE                 = 0U,    /**< Disable clock SPI controller, argu is NULL, return OM_ERROR_OK */
    SPI_CONTROL_CLK_ENABLE                  = 1U,    /**< Enable clock SPI controller, argu is NULL, return OM_ERROR_OK */
    SPI_CONTROL_RESET                       = 2U,    /**< Reseet SPI controller, argu is NULL, return OM_ERROR_OK */
    SPI_CONTROL_GET_CLK                     = 3U,    /**< Get SPI controller clock, argu is NULL, return uint32_t */
    SPI_CONTROL_GET_RX_COUNT                = 4U,    /**< Get Rx count, argu is NULL, return range in [0, 0xFFFF] */
    SPI_CONTROL_GET_TX_COUNT                = 5U,    /**< Get Tx count, argu is NULL, return range in [0, 0xFFFF] */
    SPI_CONTROL_IS_BUSY                     = 6U,    /**< Check SPI controller is busy. argu is NULL, return 1 indicate is busy, 0 not busy */
    SPI_CONTROL_DLY_SAMPLE_FE_SET           = 7U,    /**< When argu is NULL, disable falling edge delay sample and vice versa **/
    SPI_CONTROL_DLY_SAMPLE_CYCLE_NUM_SET    = 8U,    /**< Set delay sample cycle num, argu is in the range [0, 3] **/
    SPI_CONTROL_SET_FREQUENCY               = 9U,    /**< Set SPI controller clock, argu is uint32_t, return OM_ERROR_OK */
    SPI_CONTROL_SET_MODE                    = 10U,    /**< Set SPI controller mode, argu is uint8_t, return OM_ERROR_OK */
    SPI_CONTROL_CSN_LOW                     = 11U,   /**< Set SPI controller CSn low, argu is NULL, return OM_ERROR_OK */
    SPI_CONTROL_CSN_HIGH                    = 12U,   /**< Set SPI controller CSn high, argu is NULL, return OM_ERROR_OK */
    SPI_CONTROL_SET_BIT_ORDER               = 13U,   /**< Set SPI controller bit order, argu is uint8_t, return OM_ERROR_OK */

} spi_control_t;

/// SPI Configuration
typedef struct {
    /// Specifies SPI clock frequency, NOTE: spi_clock must <= cpu_clock / 2
    uint32_t        freq;
    /// Specifies SPI Mode
    spi_mode_t      mode;
    /// Specifies SPI Role
    spi_role_t      role;
    /// Specifies SPI Wire
    spi_wire_t      wire;
    /// Specifies whether data transfers start from LSB or MSB bit
    spi_first_t     first_bit;
    /// Specifies CS valid level
    spi_cs_t        cs_valid;
} spi_config_t;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief SPI initialization
 *
 * @param[in] om_spi         Pointer to SPI
 * @param[in] cfg            Configuration for SPI
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_spi_init(OM_SPI_Type *om_spi, spi_config_t *cfg);

#if (RTE_SPI_REGISTER_CALLBACK)
/**
 *******************************************************************************
 * @brief Register callback for SPI transfer
 *
 * @param[in] om_spi         Pointer to SPI
 * @param[in] event_cb       Pointer to callback
 *******************************************************************************
 */
extern void drv_spi_register_isr_callback(OM_SPI_Type                 *om_spi,
                                            drv_isr_callback_t        event_cb);
#endif

/**
 *******************************************************************************
 * @brief The interrupt callback for SPI driver. It is a weak function. User should define
 *        their own callback in user file, other than modify it in the SPI driver.
 *
 * @param om_spi            The SPI device address
 * @param event             The driver usart event
 *                           - DRV_EVENT_COMMON_TRANSFER_COMPLETED
 *                           - DRV_EVENT_COMMON_READ_COMPLETED
 *                           - DRV_EVENT_COMMON_ABORT
 *                           - DRV_EVENT_COMMON_ERROR
 *                           - DRV_EVENT_COMMON_DMA2PERIPH_COMPLETED
 * @param data              The data pointer of data to be read or write
 * @param num               The data buffer valid data count
 *******************************************************************************
 */
extern __WEAK void drv_spi_isr_callback(OM_SPI_Type *om_spi, drv_event_t event, uint8_t *data, uint32_t num);

/**
 *******************************************************************************
 * @brief Start sending data to SPI transmitter by block mode.
 *
 * @param[in]   om_spi   Pointer to SPI
 * @param[in]   tx_data  Pointer to buffer with data to send to master SPI transmitter
 * @param[in]   tx_num   Number of data items to send
 * @param[in]   rx_data  Pointer to buffer with data to send to master SPI receiver
 * @param[in]   rx_num   Number of data items to receive
 * @param[in]   timeout_ms   timeout
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_spi_transfer(OM_SPI_Type                      *om_spi,
                                   const uint8_t                   *tx_data,
                                   uint16_t                          tx_num,
                                   uint8_t                         *rx_data,
                                   uint16_t                          rx_num,
                                   uint32_t                      timeout_ms);

/**
 *******************************************************************************
 * @brief Start sending data to SPI transmitter by interrupt mode.
 *
 * @param[in]   om_spi   Pointer to SPI
 * @param[in]   tx_data  Pointer to buffer with data to send to master SPI transmitter
 * @param[in]   tx_num   Number of data items to send
 * @param[in]   rx_data  Pointer to buffer with data to send to master SPI receiver
 * @param[in]   rx_num   Number of data items to receive
 *
 * @return errno
 *******************************************************************************
 */

extern om_error_t drv_spi_transfer_int(OM_SPI_Type                      *om_spi,
                                       const uint8_t                   *tx_data,
                                       uint16_t                          tx_num,
                                       uint8_t                         *rx_data,
                                       uint16_t                          rx_num);

#if (RTE_DMA)
/**
 *******************************************************************************
 * @brief Allocate spi dma channel
 *
 * @param[in] om_spi    Pointer to spi
 * @param[in] channel   SPI rx/tx channel
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_spi_dma_channel_allocate(OM_SPI_Type *om_spi, spi_dma_chan_t channel);

/**
 *******************************************************************************
 * @brief Allocate spi dma channel
 *
 * @param[in] om_spi    Pointer to spi
 * @param[in] channel   SPI rx/tx channel
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_spi_dma_channel_release(OM_SPI_Type *om_spi, spi_dma_chan_t channel);

/**
 *******************************************************************************
 * @brief Start sending data to SPI transmitter by dma mode.
 *
 * @param[in]   om_spi   Pointer to SPI
 * @param[in]   tx_data  Pointer to buffer with data to send to master SPI transmitter
 * @param[in]   tx_num   Number of data items to send
 * @param[in]   rx_data  Pointer to buffer with data to send to master SPI receiver
 * @param[in]   rx_num   Number of data items to receive
 *
 * @return errno
 *******************************************************************************
 */
extern om_error_t drv_spi_transfer_dma(OM_SPI_Type                  *om_spi,
                                       const uint8_t               *tx_data,
                                       uint16_t                      tx_num,
                                       uint8_t                     *rx_data,
                                       uint16_t                      rx_num);
#endif

/**
 *******************************************************************************
 * @brief Control the SPI
 *
 * @param[in] om_spi         Pointer to SPI
 * @param[in] control        Control options
 * @param[in] argu           argument for control options
 *
 * @return control status
 *******************************************************************************
 */
extern void *drv_spi_control(OM_SPI_Type *om_spi, spi_control_t control, void *argu);

/**
 *******************************************************************************
 * @brief spi interrupt service routine
 *
 * @param[in] om_spi         Pointer to SPI
 *******************************************************************************
 */
extern void drv_spi_isr(OM_SPI_Type *om_spi);


#ifdef __cplusplus
}
#endif

#endif  /* (RTE_SPI0 || RTE_SPI1) */

#endif  /* __DRV_SPI_H */


/** @} */