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
 * @file     drv_spi.c
 * @brief
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_SPI0 || RTE_SPI1)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */
#define SPI_TX_DUMMY                0xFFU

#define SPI_TX_BUSY                 (1U << 0)
#define SPI_RX_BUSY                 (1U << 4)


/*******************************************************************************
 * TYPEDEFS
 */
typedef enum {
    SENDING =   0,
    RECVING =   1,

    INVALID =   0xFF,
} wire3_trans_state_t;

typedef struct {
    drv_isr_callback_t           isr_cb;
    uint16_t                     tx_num;
    uint16_t                     tx_cnt;
    uint8_t                     *tx_buf;
    uint16_t                     rx_num;
    uint16_t                     rx_cnt;
    uint8_t                     *rx_buf;
    uint8_t                      busy;
    #if (RTE_DMA)
    uint8_t                      dma_tx_chan;
    uint8_t                      dma_rx_chan;
    #endif  /* (RTE_DMA) */
    wire3_trans_state_t          wire3_trans_state;
    uint8_t                      ignore_next_dma_tx_cb;
} spi_drv_env_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
#if (RTE_SPI0)
static spi_drv_env_t spi0_env = {
    .isr_cb         = NULL,
    .tx_num         = 0U,
    .tx_cnt         = 0U,
    .tx_buf         = NULL,
    .rx_num         = 0U,
    .rx_cnt         = 0U,
    .rx_buf         = NULL,
    .busy           = 0U,
    #if (RTE_DMA)
    .dma_tx_chan    = DMA_NUMBER_OF_CHANNELS,
    .dma_rx_chan    = DMA_NUMBER_OF_CHANNELS,
    #endif  /* RTE_DMA */
    .wire3_trans_state = INVALID,
    .ignore_next_dma_tx_cb = 0U,
};

static const drv_resource_t spi0_resource = {
    .cap        = CAP_SPI0,
    .reg        = OM_SPI0,
    .env        = &spi0_env,
    .irq_num    = SPI0_IRQn,
    .irq_prio   = RTE_SPI0_IRQ_PRIORITY,
    #if (RTE_DMA)
    .dma_tx = {
        .id     = DMA_ID_SPI0_TX,
        .prio   = RTE_SPI0_DMA_TX_PRIORITY,
    },
    .dma_rx = {
        .id     = DMA_ID_SPI0_RX,
        .prio   = RTE_SPI0_DMA_RX_PRIORITY,
    }
    #endif /* RTE_DMA */
};
#endif  /* RTE_SPI0 */

#if (RTE_SPI1)
static spi_drv_env_t spi1_env = {
    .isr_cb         = NULL,
    .tx_num         = 0U,
    .tx_cnt         = 0U,
    .tx_buf         = NULL,
    .rx_num         = 0U,
    .rx_cnt         = 0U,
    .rx_buf         = NULL,
    .busy           = 0U,
    #if (RTE_DMA)
    .dma_tx_chan    = DMA_NUMBER_OF_CHANNELS,
    .dma_rx_chan    = DMA_NUMBER_OF_CHANNELS,
    #endif  /* RTE_DMA */
    .wire3_trans_state = INVALID,
    .ignore_next_dma_tx_cb = 0U,
};

static const drv_resource_t spi1_resource = {
    .cap        = CAP_SPI1,
    .reg        = OM_SPI1,
    .env        = &spi1_env,
    .irq_num    = SPI1_IRQn,
    .irq_prio   = RTE_SPI1_IRQ_PRIORITY,
    #if (RTE_DMA)
    .dma_tx = {
        .id     = DMA_ID_SPI1_TX,
        .prio   = RTE_SPI1_DMA_TX_PRIORITY,
    },
    .dma_rx = {
        .id     = DMA_ID_SPI1_RX,
        .prio   = RTE_SPI1_DMA_RX_PRIORITY,
    }
    #endif /* RTE_DMA */
};
#endif  /* RTE_SPI1 */


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static const drv_resource_t *spi_get_resource(OM_SPI_Type *om_spi)
{
    #if (RTE_SPI0)
    if ((uint32_t)om_spi == (uint32_t)spi0_resource.reg) {
        return &spi0_resource;
    }
    #endif  /* RTE_SPI0 */
    #if (RTE_SPI1)
    if ((uint32_t)om_spi == (uint32_t)spi1_resource.reg) {
        return &spi1_resource;
    }
    #endif  /* RTE_SPI1 */

    OM_ASSERT(0);
    return NULL;
}

#if (RTE_DMA)
static void spi_dma_rx_event_cb(void *resource, drv_event_t event, void *next_chain)
{
    spi_wire_t wire;
    spi_role_t role;
    spi_drv_env_t *env;
    OM_SPI_Type *om_spi;
    uint16_t rx_num;
    drv_event_t  drv_event;

    if (resource == NULL) {
        OM_ASSERT(0);
    }

    env  = (spi_drv_env_t *)(((const drv_resource_t *)resource)->env);
    om_spi  = (OM_SPI_Type *)(((const drv_resource_t *)resource)->reg);
    wire = om_spi->CTRL & SPI_CTRL_BIDIRECT_DATA_MASK ? SPI_WIRE_3 : SPI_WIRE_4;
    role = om_spi->CTRL & SPI_CTRL_MASTER_EN_MASK ? SPI_ROLE_MASTER : SPI_ROLE_SLAVE;

    rx_num = env->rx_num;
    env->rx_num = 0U;

    switch (event) {
        case DRV_DMA_EVENT_TERMINAL_COUNT_REQUEST:
            drv_event = DRV_EVENT_COMMON_READ_COMPLETED;
            if (wire == SPI_WIRE_3) {
                om_spi->CTRL &= ~SPI_CTRL_ACTIVE_DO_ENL_MASK;
                env->wire3_trans_state = SENDING;
                if (role == SPI_ROLE_SLAVE) {
                    if (env->tx_num) {
                        register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 0,
                                                              SPI_CTRL_TX_FIFO_EN, 1));
                        drv_dma_channel_enable(env->dma_tx_chan, (uint32_t)(&om_spi->WDATA), (uint32_t)(env->tx_buf), env->tx_num);
                        env->busy |= SPI_TX_BUSY;
                    } else {
                        // om_spi->CTRL |= SPI_CTRL_ACTIVE_DO_ENL_MASK;
                        env->wire3_trans_state = RECVING;

                        env->busy &= ~SPI_TX_BUSY;
                    }
                }
            }
            break;
        case DRV_DMA_EVENT_ABORT:
            drv_event = DRV_EVENT_COMMON_ABORT;
            break;
        default:
            drv_event = DRV_EVENT_COMMON_ERROR;
            OM_ASSERT(0);
            break;
    }

    env->busy &= ~SPI_RX_BUSY;

    drv_spi_isr_callback(om_spi, drv_event, env->rx_buf, rx_num);

    if (env->rx_num == 0U) {
        if (env->busy == 0) {
            if (role == SPI_ROLE_MASTER) {
                SPI_CSN_HIGH(om_spi);
            }
        }
    }
}

static void spi_dma_tx_event_cb(void *resource, drv_event_t event, void *next_chain)
{
    spi_wire_t wire;
    spi_role_t role;
    spi_drv_env_t *env;
    OM_SPI_Type *om_spi;
    uint16_t tx_num;
    drv_event_t  drv_event;
    uint8_t ignore = 0;

    if (resource == NULL) {
        OM_ASSERT(0);
    }

    env  = (spi_drv_env_t *)(((const drv_resource_t *)resource)->env);
    om_spi  = (OM_SPI_Type *)(((const drv_resource_t *)resource)->reg);
    wire = om_spi->CTRL & SPI_CTRL_BIDIRECT_DATA_MASK ? SPI_WIRE_3 : SPI_WIRE_4;
    role = om_spi->CTRL & SPI_CTRL_MASTER_EN_MASK ? SPI_ROLE_MASTER : SPI_ROLE_SLAVE;

    if (env->ignore_next_dma_tx_cb) {
        env->ignore_next_dma_tx_cb = 0U;
        ignore = 1;
    }

    tx_num = env->tx_num;
    env->tx_num = 0U;

    switch (event) {
        case DRV_DMA_EVENT_TERMINAL_COUNT_REQUEST:
            drv_event = DRV_EVENT_COMMON_DMA2PERIPH_COMPLETED;
            while (om_spi->STAT & SPI_STAT_TX_BYTE_CNT_MASK); // wait transmit truely over
            if (!ignore) {
                if (wire == SPI_WIRE_3) {
                    om_spi->CTRL |= SPI_CTRL_RX_CLR_FIFO_MASK;
                    om_spi->CTRL &= ~SPI_CTRL_RX_CLR_FIFO_MASK;
                    if (role == SPI_ROLE_MASTER) {
                        if (env->rx_num) {
                            env->tx_num = env->rx_num;

                            om_spi->CTRL |= SPI_CTRL_ACTIVE_DO_ENL_MASK;
                            env->wire3_trans_state = RECVING;

                            register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 1,
                                                                  SPI_CTRL_TX_FIFO_EN, 1));
                            drv_dma_channel_enable(env->dma_rx_chan, (uint32_t)(env->rx_buf), (uint32_t)(&om_spi->RDATA), env->rx_num);
                            drv_dma_channel_enable(env->dma_tx_chan, (uint32_t)(&om_spi->WDATA), (uint32_t)(env->rx_buf), env->tx_num);
                            env->busy |= SPI_RX_BUSY;
                            env->ignore_next_dma_tx_cb = 1U;
                        } else {
                            // om_spi->CTRL &= ~SPI_CTRL_ACTIVE_DO_ENL_MASK;
                            env->wire3_trans_state = SENDING;

                            env->busy &= ~SPI_RX_BUSY;
                        }
                    }
                }
            }
            break;
        case DRV_DMA_EVENT_ABORT:
            drv_event = DRV_EVENT_COMMON_ABORT;
            break;
        default:
            drv_event = DRV_EVENT_COMMON_ERROR;
            OM_ASSERT(0);
            break;
    }

    if (env->tx_num == 0U) {
        env->busy &= ~SPI_TX_BUSY;
    }

    if (!ignore) {
        drv_spi_isr_callback(om_spi, drv_event, env->tx_buf, tx_num);
    }

    if (env->tx_num == 0U) {
        if (env->busy == 0) {
            if (role == SPI_ROLE_MASTER) {
                SPI_CSN_HIGH(om_spi);
            }
        }
    }
}
#endif

static om_error_t spi_4wire_transfer(const drv_resource_t   *resource,
                                     const uint8_t          *tx_data,
                                     uint16_t                tx_num,
                                     uint8_t                *rx_data,
                                     uint16_t                rx_num,
                                     uint32_t                timeout_ms)
{
    OM_SPI_Type            *om_spi;
    volatile uint8_t        dummy;
    spi_drv_env_t          *env;
    spi_role_t              role;
    uint16_t                fifo_size;
    uint16_t                tx_fifo_cnt;
    uint16_t                rx_fifo_cnt;
    uint32_t                timeout_cycle;
    uint32_t                start_cycle;

    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    if((tx_num == 0) && (rx_num != 0)) {
        tx_num = rx_num;
    }
    if((rx_num == 0) && (tx_num != 0)) {
        rx_num = tx_num;
    }

    env    = (spi_drv_env_t *)resource->env;
    om_spi = (OM_SPI_Type *)resource->reg;
    role   = om_spi->CTRL & SPI_CTRL_MASTER_EN_MASK ? SPI_ROLE_MASTER : SPI_ROLE_SLAVE;
    fifo_size = register_get(&resource->cap, MASK_POS(CAP_SPI_FIFO_LEVEL));

    env->tx_buf = (uint8_t *)tx_data;
    env->tx_num = tx_num;
    env->tx_cnt = 0U;
    env->rx_buf = rx_data;
    env->rx_num = rx_num;
    env->rx_cnt = 0U;

    register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 1,
                                          SPI_CTRL_TX_FIFO_EN, 1));

    // 1. start transfer
    if (role == SPI_ROLE_MASTER) {
        SPI_CSN_LOW(om_spi);
    }

    om_spi->CTRL |= SPI_CTRL_RX_CLR_FIFO_MASK;
    om_spi->CTRL &= ~SPI_CTRL_RX_CLR_FIFO_MASK;    // clear rx fifo and status

    DRV_DWT_DELAY_CALC_INIT();
    (void)__delay_cycle;
    (void)__delay_us;
    timeout_cycle = DRV_DWT_MS_2_CYCLES_ROUND(timeout_ms);
    start_cycle = drv_dwt_get_cycle();
    while (env->rx_cnt < env->rx_num) {
        if (timeout_ms != DRV_MAX_DELAY) {
            if (drv_dwt_get_cycle() - start_cycle > timeout_cycle) {
                return OM_ERROR_TIMEOUT;
            }
        }
        // 1.1 transmit
        if (env->tx_cnt < env->tx_num) {
            tx_fifo_cnt = register_get(&om_spi->STAT, MASK_POS(SPI_STAT_TX_BYTE_CNT));
            if (tx_fifo_cnt < fifo_size - 1) {
                om_spi->WDATA = tx_data ? tx_data[env->tx_cnt] : SPI_TX_DUMMY;
                env->tx_cnt++;
            }
        }
        // 1.2 receive
        rx_fifo_cnt = register_get(&om_spi->STAT, MASK_POS(SPI_STAT_RX_BYTE_CNT));
        if (rx_fifo_cnt) {
            if (rx_data) {
                rx_data[env->rx_cnt] = om_spi->RDATA;
            } else {
                dummy = om_spi->RDATA;
            }
            env->rx_cnt++;
        }
    }

    if (role == SPI_ROLE_MASTER) {
        SPI_CSN_HIGH(om_spi);
    }

    (void)dummy;
    return OM_ERROR_OK;
}

static om_error_t spi_3wire_master_transfer(const drv_resource_t            *resource,
                                            const uint8_t                   *tx_data,
                                            uint16_t                         tx_num,
                                            uint8_t                         *rx_data,
                                            uint16_t                         rx_num,
                                            uint32_t                     timeout_ms)
{
    OM_SPI_Type            *om_spi;
    spi_drv_env_t          *env;
    volatile uint8_t        dummy;
    uint16_t                tx_dummy_cnt;
    uint16_t                fifo_size;
    uint16_t                tx_fifo_cnt;
    uint16_t                rx_fifo_cnt;
    om_error_t              error;
    uint32_t                timeout_cycle;
    uint32_t                start_cycle;

    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (spi_drv_env_t *)resource->env;
    om_spi = (OM_SPI_Type *)resource->reg;
    fifo_size = register_get(&resource->cap, MASK_POS(CAP_SPI_FIFO_LEVEL));

    env->tx_num = tx_num;
    env->tx_cnt = 0U;
    env->rx_num = rx_num;
    env->rx_cnt = 0U;

    register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 0,
                                          SPI_CTRL_TX_FIFO_EN, 1));

    // 1. start transfer
    SPI_CSN_LOW(om_spi);
    // 1.1 switch SPI_DO to be driven
    om_spi->CTRL &= ~SPI_CTRL_ACTIVE_DO_ENL_MASK;
    env->wire3_trans_state = SENDING;
    // 1.2 transmit
    DRV_DWT_DELAY_CALC_INIT();
    (void)__delay_cycle;
    (void)__delay_us;
    timeout_cycle = DRV_DWT_MS_2_CYCLES_ROUND(timeout_ms);
    start_cycle = drv_dwt_get_cycle();
    while (env->tx_cnt < env->tx_num) {
        if (timeout_ms != DRV_MAX_DELAY) {
            if (drv_dwt_get_cycle() - start_cycle > timeout_cycle) {
                return OM_ERROR_TIMEOUT;
            }
        }
        tx_fifo_cnt = register_get(&om_spi->STAT, MASK_POS(SPI_STAT_TX_BYTE_CNT));
        if (tx_fifo_cnt < fifo_size - 1) {
            om_spi->WDATA = tx_data ? tx_data[env->tx_cnt] : SPI_TX_DUMMY;
            env->tx_cnt++;
        }
    }
    // 1.3 wait transmit done
    DRV_DWT_WAIT_MS_UNTIL_TO(om_spi->STAT & SPI_STAT_TX_BYTE_CNT_MASK, timeout_ms, error);
    if (error != OM_ERROR_OK) {
        return error;
    }
    // 1.4 switch SPI_DO to HIGH-Z and clear rx fifo
    om_spi->CTRL |= SPI_CTRL_RX_CLR_FIFO_MASK;
    om_spi->CTRL &= ~SPI_CTRL_RX_CLR_FIFO_MASK;
    om_spi->CTRL |= SPI_CTRL_ACTIVE_DO_ENL_MASK;
    env->wire3_trans_state = RECVING;
    DRV_DELAY_US(5);

    register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 1,
                                          SPI_CTRL_TX_FIFO_EN, 1));

    // 1.5 receive
    tx_dummy_cnt = env->rx_cnt;
    start_cycle = drv_dwt_get_cycle();
    while (env->rx_cnt < env->rx_num) {
        if (timeout_ms != DRV_MAX_DELAY) {
            if (drv_dwt_get_cycle() - start_cycle > timeout_cycle) {
                return OM_ERROR_TIMEOUT;
            }
        }
        // 1.5.1 use tx dummy for generating clk to slave
        if (tx_dummy_cnt < env->rx_num) {
            tx_fifo_cnt = register_get(&om_spi->STAT, MASK_POS(SPI_STAT_TX_BYTE_CNT));
            if (tx_fifo_cnt < fifo_size - 1) {
                om_spi->WDATA = SPI_TX_DUMMY;
            }
            tx_dummy_cnt++;
        }
        DRV_DWT_WAIT_MS_UNTIL_TO(om_spi->STAT & SPI_STAT_TX_BYTE_CNT_MASK, timeout_ms, error);
        if (error != OM_ERROR_OK) {
            return error;
        }
        // 1.5.2 receive data
        rx_fifo_cnt = register_get(&om_spi->STAT, MASK_POS(SPI_STAT_RX_BYTE_CNT));
        if (rx_fifo_cnt) {
            if (rx_data) {
                rx_data[env->rx_cnt] = om_spi->RDATA;
            } else {
                dummy = om_spi->RDATA;
            }
            env->rx_cnt++;
        }
    }
    DRV_DELAY_US(5);
    // 1.6 switch SPI_DO to be driven
    om_spi->CTRL &= ~SPI_CTRL_ACTIVE_DO_ENL_MASK;
    env->wire3_trans_state = SENDING;

    SPI_CSN_HIGH(om_spi);
    (void)dummy;
    return OM_ERROR_OK;
}

static om_error_t spi_3wire_slave_transfer(const drv_resource_t            *resource,
                                           const uint8_t                   *tx_data,
                                           uint16_t                         tx_num,
                                           uint8_t                         *rx_data,
                                           uint16_t                         rx_num,
                                           uint32_t                     timeout_ms)
{
    OM_SPI_Type            *om_spi;
    spi_drv_env_t          *env;
    volatile uint8_t        dummy;
    uint16_t                fifo_size;
    uint16_t                tx_fifo_cnt;
    uint16_t                rx_fifo_cnt;
    om_error_t              error;
    uint32_t                timeout_cycle;
    uint32_t                start_cycle;

    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    om_spi = (OM_SPI_Type *)resource->reg;
    env    = (spi_drv_env_t *)resource->env;
    fifo_size = register_get(&resource->cap, MASK_POS(CAP_SPI_FIFO_LEVEL));

    env->tx_num = tx_num;
    env->tx_cnt = 0U;
    env->rx_num = rx_num;
    env->rx_cnt = 0U;

    register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 1,
                                          SPI_CTRL_TX_FIFO_EN, 0));

    // 1. switch SPI DO to HIGH-Z
    om_spi->CTRL |= SPI_CTRL_RX_CLR_FIFO_MASK;
    om_spi->CTRL &= ~SPI_CTRL_RX_CLR_FIFO_MASK;
    om_spi->CTRL |= SPI_CTRL_ACTIVE_DO_ENL_MASK;
    env->wire3_trans_state = RECVING;
    // 2. receive
    DRV_DWT_DELAY_CALC_INIT();
    (void)__delay_cycle;
    (void)__delay_us;
    timeout_cycle = DRV_DWT_MS_2_CYCLES_ROUND(timeout_ms);
    start_cycle = drv_dwt_get_cycle();
    while (env->rx_cnt < env->rx_num) {
        if (timeout_ms != DRV_MAX_DELAY) {
            if (drv_dwt_get_cycle() - start_cycle > timeout_cycle) {
                return OM_ERROR_TIMEOUT;
            }
        }
        rx_fifo_cnt = register_get(&om_spi->STAT, MASK_POS(SPI_STAT_RX_BYTE_CNT));
        if (rx_fifo_cnt) {
            if (rx_data) {
                rx_data[env->rx_cnt] = om_spi->RDATA;
            } else {
                dummy = om_spi->RDATA;
            }
            env->rx_cnt++;
        }
    }
    DRV_DELAY_US(2);
    register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 0,
                                          SPI_CTRL_TX_FIFO_EN, 1));
    // 3. switch SPI_DO to be driven
    om_spi->CTRL &= ~SPI_CTRL_ACTIVE_DO_ENL_MASK;
    env->wire3_trans_state = SENDING;
    // om_spi->WDATA = 0xFF; //10K 1 byte invalid data is required, Over 10K is not required
    // 4. fill tx fifo, then wait for master's clk
    start_cycle = drv_dwt_get_cycle();
    while (env->tx_cnt < env->tx_num) {
        if (timeout_ms != DRV_MAX_DELAY) {
            if (drv_dwt_get_cycle() - start_cycle > timeout_cycle) {
                return OM_ERROR_TIMEOUT;
            }
        }
        tx_fifo_cnt = register_get(&om_spi->STAT, MASK_POS(SPI_STAT_TX_BYTE_CNT));
        if (tx_fifo_cnt < fifo_size - 1) {
            om_spi->WDATA = tx_data ? tx_data[env->tx_cnt] : SPI_TX_DUMMY;
            env->tx_cnt++;
        }
    }
    // 5. wait transmit done
    DRV_DWT_WAIT_MS_UNTIL_TO(om_spi->STAT & SPI_STAT_TX_BYTE_CNT_MASK, timeout_ms, error);
    if (error != OM_ERROR_OK) {
        return error;
    }
    DRV_DELAY_US(5);
    // 6. switch SPI DO to HIGH-Z
    om_spi->CTRL |= SPI_CTRL_ACTIVE_DO_ENL_MASK;
    env->wire3_trans_state = RECVING;

    (void)dummy;
    return OM_ERROR_OK;
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
om_error_t drv_spi_init(OM_SPI_Type *om_spi, spi_config_t *cfg)
{
    const drv_resource_t *resource;
    spi_drv_env_t  *env;
    uint32_t    clk;

    OM_ASSERT(om_spi);
    if (cfg == NULL) {
        spi_config_t cfg_default = {
            .freq = 10000U,
            .mode = SPI_MODE_0,
            .role = SPI_ROLE_MASTER,
            .wire = SPI_WIRE_4,
            .first_bit = SPI_MSB_FIRST,
            .cs_valid = SPI_CS_LOW,
        };
        cfg = &cfg_default;
    }

    resource = spi_get_resource(om_spi);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    DRV_RCC_RESET((rcc_clk_t)(size_t)resource->reg);
    clk = drv_rcc_clock_get((rcc_clk_t)(size_t)resource->reg);

    env = (spi_drv_env_t *)resource->env;

    SPI_CSN_HIGH(om_spi);

    register_set(&om_spi->CTRL, MASK_3REG(SPI_CTRL_SOFT_RST,    1,
                                          SPI_CTRL_RX_CLR_FIFO, 1,
                                          SPI_CTRL_TX_CLR_FIFO, 1));
    {
        uint32_t ctrl_reg = 0;

        // use cs as chip enable
        ctrl_reg &= ~SPI_CTRL_USE_RDY_OUT_MASK;
        // config clock divider in master
        if (cfg->role == SPI_ROLE_MASTER) {
            uint16_t div;

            OM_ASSERT(cfg->freq);
            OM_ASSERT(cfg->freq <= clk / 2);
            div     = clk / cfg->freq;
            register_set(&ctrl_reg, MASK_3REG(SPI_CTRL_CLK_DIVIDER, div / 2 - 1,
                                              SPI_CTRL_MASTER_EN, 1,
                                              SPI_CTRL_MASTER_CE_AT_END, 1));
            if (cfg->cs_valid == SPI_CS_HIGH) {
                ctrl_reg &= ~SPI_CTRL_MASTER_CE_AT_END_MASK;
            }
        }

        // config mode
        switch (cfg->mode) {
            case SPI_MODE_0:
                register_set(&ctrl_reg, MASK_2REG(SPI_CTRL_MODE,       0,
                                                  SPI_CTRL_INVERT_CLK, 0));
                break;
            case SPI_MODE_1:
                register_set(&ctrl_reg, MASK_2REG(SPI_CTRL_MODE,       1,
                                                  SPI_CTRL_INVERT_CLK, 0));
                break;
            case SPI_MODE_2:
                register_set(&ctrl_reg, MASK_2REG(SPI_CTRL_MODE,       0,
                                                  SPI_CTRL_INVERT_CLK, 1));
                break;
            case SPI_MODE_3:
                register_set(&ctrl_reg, MASK_2REG(SPI_CTRL_MODE,       1,
                                                  SPI_CTRL_INVERT_CLK, 1));
                break;
            default:
                OM_ASSERT(0);
                break;
        }

        // config wire 3 or 4
        if (cfg->wire == SPI_WIRE_3) {
            register_set(&ctrl_reg, MASK_2REG(SPI_CTRL_BIDIRECT_DATA,   1,
                                              SPI_CTRL_INACTIVE_DO_ENL, 1));
        }

        // config first bit
        if (cfg->first_bit == SPI_MSB_FIRST) {
            ctrl_reg |= SPI_CTRL_MSB_FIRST_MASK;
        }

        register_set(&ctrl_reg, MASK_4REG(SPI_CTRL_SOFT_RST,        0,
                                          SPI_CTRL_RX_TRIG_LEVEL,   0,
                                          SPI_CTRL_RX_CLR_FIFO,     0,
                                          SPI_CTRL_TX_CLR_FIFO,     0));

        om_spi->CTRL = ctrl_reg;
    }

    // config wire3 initial transfer state
    if (cfg->wire == SPI_WIRE_3) {
        if (cfg->role == SPI_ROLE_MASTER) {
            om_spi->CTRL &= ~SPI_CTRL_ACTIVE_DO_ENL_MASK;
            env->wire3_trans_state = SENDING;
        } else {
            om_spi->CTRL |= SPI_CTRL_ACTIVE_DO_ENL_MASK;
            env->wire3_trans_state = RECVING;
        }
    } else {
        env->wire3_trans_state = INVALID;
    }

    // disable tx empty and rx trigger interrupt
    register_set(&om_spi->STAT, MASK_2REG(SPI_STAT_TX_EMPTY_INT_EN, 0,
                                          SPI_STAT_RX_TRIG_INT_EN,  0));

    NVIC_ClearPendingIRQ(resource->irq_num);
    NVIC_SetPriority(resource->irq_num, resource->irq_prio);
    NVIC_EnableIRQ(resource->irq_num);

    env->isr_cb = NULL;

    return OM_ERROR_OK;
}

#if (RTE_SPI_REGISTER_CALLBACK)
void drv_spi_register_isr_callback(OM_SPI_Type *om_spi, drv_isr_callback_t isr_cb)
{
    const drv_resource_t *resource;
    spi_drv_env_t        *env;

    resource = spi_get_resource(om_spi);
    if(resource != NULL) {
        env = (spi_drv_env_t *)(resource->env);
        env->isr_cb = isr_cb;
    }
}
#endif

__WEAK void drv_spi_isr_callback(OM_SPI_Type *om_spi, drv_event_t event, uint8_t *data, uint32_t num)
{
    #if (RTE_SPI_REGISTER_CALLBACK)
    const drv_resource_t *resource;
    spi_drv_env_t        *env;

    resource = spi_get_resource(om_spi);
    if (resource == NULL) {
        return;
    }
    env = (spi_drv_env_t *)(resource->env);

    if (env->isr_cb) {
        env->isr_cb(om_spi, event, data, (void *)num);
    }
    #endif
}

om_error_t drv_spi_transfer(OM_SPI_Type                      *om_spi,
                            const uint8_t                    *tx_data,
                            uint16_t                          tx_num,
                            uint8_t                          *rx_data,
                            uint16_t                          rx_num,
                            uint32_t                      timeout_ms)
{
    const drv_resource_t   *resource;
    uint32_t                ctrl_reg;
    om_error_t              error;
    spi_role_t              role;
    spi_wire_t              wire;

    resource = spi_get_resource(om_spi);
    if(resource == NULL) {
        return OM_ERROR_PARAMETER;
    }

    ctrl_reg = om_spi->CTRL;
    role = ctrl_reg & SPI_CTRL_MASTER_EN_MASK ? SPI_ROLE_MASTER : SPI_ROLE_SLAVE;
    wire = ctrl_reg & SPI_CTRL_BIDIRECT_DATA_MASK ? SPI_WIRE_3 : SPI_WIRE_4;

    if (wire == SPI_WIRE_4) {
        error = spi_4wire_transfer(resource, tx_data, tx_num, rx_data, rx_num, timeout_ms);
    } else {
        if (role == SPI_ROLE_MASTER) {
            error = spi_3wire_master_transfer(resource, tx_data, tx_num, rx_data, rx_num, timeout_ms);
        } else {
            error = spi_3wire_slave_transfer(resource, tx_data, tx_num, rx_data, rx_num, timeout_ms);
        }
    }

    return error;
}

om_error_t drv_spi_transfer_int(OM_SPI_Type                    *om_spi,
                                const uint8_t                  *tx_data,
                                uint16_t                        tx_num,
                                uint8_t                        *rx_data,
                                uint16_t                        rx_num)
{
    const drv_resource_t  *resource;
    spi_drv_env_t         *env;
    spi_wire_t             wire;
    spi_role_t             role;
    uint8_t                fifo_depth, fifo_size;

    // 1. check parameter
    resource = spi_get_resource(om_spi);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }

    env = (spi_drv_env_t *)(resource->env);
    wire = om_spi->CTRL & SPI_CTRL_BIDIRECT_DATA_MASK ? SPI_WIRE_3 : SPI_WIRE_4;
    role = om_spi->CTRL & SPI_CTRL_MASTER_EN_MASK ? SPI_ROLE_MASTER : SPI_ROLE_SLAVE;
    fifo_size = register_get(&resource->cap, MASK_POS(CAP_SPI_FIFO_LEVEL));
    fifo_depth = fifo_size;

    if (wire == SPI_WIRE_4) {
        if((tx_num == 0) && (rx_num != 0)) {
            tx_num = rx_num;
        }
        if((rx_num == 0) && (tx_num != 0)) {
            rx_num = tx_num;
        }
    }

    // 2. start transmit
    env->tx_buf = (uint8_t *)tx_data;
    env->tx_num = tx_num;
    env->tx_cnt = 0U;
    env->rx_buf = rx_data;
    env->rx_num = rx_num;
    env->rx_cnt = 0U;

    if (role == SPI_ROLE_MASTER) {
        SPI_CSN_LOW(om_spi);
    }

    OM_CRITICAL_BEGIN();
    // reset interrupt_en state
    // om_spi->STAT &= ~(SPI_STAT_TX_EMPTY_INT_EN_MASK | SPI_STAT_RX_TRIG_INT_EN_MASK);
    om_spi->STAT = 0U;

    // The speed of 32m master SPI is faster than that of CPU.
    // Enabling interrupt will cause failure to enter interrupt function after FIFO is filled.
    if (role == SPI_ROLE_MASTER) {
        om_spi->STAT |= SPI_STAT_TX_EMPTY_INT_EN_MASK;
    } else {
        om_spi->STAT |= SPI_STAT_RX_TRIG_INT_EN_MASK;
    }

    if (wire == SPI_WIRE_3) {
        if (role == SPI_ROLE_SLAVE) {
            if (rx_num) {
                register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 1,
                                                      SPI_CTRL_TX_FIFO_EN, 0));
                om_spi->CTRL |= SPI_CTRL_RX_CLR_FIFO_MASK;
                om_spi->CTRL &= ~SPI_CTRL_RX_CLR_FIFO_MASK;    // clear rx fifo and status
                om_spi->CTRL |= SPI_CTRL_ACTIVE_DO_ENL_MASK;
                env->wire3_trans_state = RECVING;
            } else {
                om_spi->CTRL |= SPI_CTRL_RX_CLR_FIFO_MASK;
                om_spi->CTRL &= ~SPI_CTRL_RX_CLR_FIFO_MASK;    // clear rx fifo and status
                om_spi->CTRL &= ~SPI_CTRL_ACTIVE_DO_ENL_MASK;
                om_spi->STAT |= SPI_STAT_TX_EMPTY_INT_EN_MASK;
                om_spi->STAT &= ~SPI_STAT_RX_TRIG_INT_EN_MASK;
                env->wire3_trans_state = SENDING;
            }
        } else {
            if (tx_num) {
                om_spi->CTRL &= ~SPI_CTRL_ACTIVE_DO_ENL_MASK;
                env->wire3_trans_state = SENDING;
            } else {
                om_spi->CTRL |= SPI_CTRL_RX_CLR_FIFO_MASK;
                om_spi->CTRL &= ~SPI_CTRL_RX_CLR_FIFO_MASK;    // clear rx fifo and status
                om_spi->CTRL |= SPI_CTRL_ACTIVE_DO_ENL_MASK;
                env->wire3_trans_state = RECVING;

                register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 1,
                                                      SPI_CTRL_TX_FIFO_EN, 1));

                uint16_t tx_dummy_cnt = env->rx_cnt;
                while((tx_dummy_cnt < env->rx_num) && fifo_depth
                        && (register_get(&om_spi->STAT, MASK_POS(SPI_STAT_TX_BYTE_CNT)) < fifo_size)) {
                om_spi->WDATA = SPI_TX_DUMMY;
                tx_dummy_cnt++;
                fifo_depth--;
                }
            }
        }
    }

    if ((wire == SPI_WIRE_4) || (env->wire3_trans_state == SENDING)) {
        if (wire == SPI_WIRE_4) {
            register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 1,
                                                  SPI_CTRL_TX_FIFO_EN, 1));
        } else {
            register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 0,
                                                  SPI_CTRL_TX_FIFO_EN, 1));
        }
        while ((env->tx_cnt < env->tx_num) && fifo_depth) {
            om_spi->WDATA = env->tx_buf ? env->tx_buf[env->tx_cnt] : SPI_TX_DUMMY;
            env->tx_cnt++;
            fifo_depth--;
        }
    }
    env->busy |= SPI_RX_BUSY | SPI_TX_BUSY;
    OM_CRITICAL_END();

    return OM_ERROR_OK;
}

#if (RTE_DMA)
om_error_t drv_spi_dma_channel_allocate(OM_SPI_Type *om_spi, spi_dma_chan_t channel)
{
    const drv_resource_t    *resource;
    spi_drv_env_t           *env;

    OM_ASSERT(channel <= SPI_DMA_CHAN_ALL);

    resource = spi_get_resource(om_spi);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (spi_drv_env_t *)(resource->env);

    if (channel == SPI_DMA_RX_CHAN) {
        if (env->dma_rx_chan >= DMA_NUMBER_OF_CHANNELS) {
            env->dma_rx_chan = drv_dma_channel_allocate();
            if (env->dma_rx_chan >= DMA_NUMBER_OF_CHANNELS) {
                return OM_ERROR_RESOURCES;
            }
        }
    } else if (channel == SPI_DMA_TX_CHAN) {
        if (env->dma_tx_chan >= DMA_NUMBER_OF_CHANNELS) {
            env->dma_tx_chan = drv_dma_channel_allocate();
            if (env->dma_tx_chan >= DMA_NUMBER_OF_CHANNELS) {
                return OM_ERROR_RESOURCES;
            }
        }
    } else if (channel == SPI_DMA_CHAN_ALL) {
        if (env->dma_rx_chan >= DMA_NUMBER_OF_CHANNELS) {
            env->dma_rx_chan = drv_dma_channel_allocate();
            if (env->dma_rx_chan >= DMA_NUMBER_OF_CHANNELS) {
                return OM_ERROR_RESOURCES;
            }
        }
        if (env->dma_tx_chan >= DMA_NUMBER_OF_CHANNELS) {
            env->dma_tx_chan = drv_dma_channel_allocate();
            if (env->dma_tx_chan >= DMA_NUMBER_OF_CHANNELS) {
                return OM_ERROR_RESOURCES;
            }
        }
    }

    return OM_ERROR_OK;
}

om_error_t drv_spi_dma_channel_release(OM_SPI_Type *om_spi, spi_dma_chan_t channel)
{
    const drv_resource_t    *resource;
    spi_drv_env_t           *env;

    OM_ASSERT(channel <= SPI_DMA_CHAN_ALL);

    resource = spi_get_resource(om_spi);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (spi_drv_env_t *)(resource->env);

    if (channel == SPI_DMA_RX_CHAN) {
        drv_dma_channel_release(env->dma_rx_chan);
        env->dma_rx_chan = DMA_NUMBER_OF_CHANNELS;
    } else if (channel == SPI_DMA_TX_CHAN) {
        drv_dma_channel_release(env->dma_tx_chan);
        env->dma_tx_chan = DMA_NUMBER_OF_CHANNELS;
    } else if (channel == SPI_DMA_CHAN_ALL) {
        drv_dma_channel_release(env->dma_rx_chan);
        env->dma_rx_chan = DMA_NUMBER_OF_CHANNELS;
        drv_dma_channel_release(env->dma_tx_chan);
        env->dma_tx_chan = DMA_NUMBER_OF_CHANNELS;
    }

    return OM_ERROR_OK;
}

om_error_t drv_spi_transfer_dma(OM_SPI_Type                     *om_spi,
                                const uint8_t                  *tx_data,
                                uint16_t                         tx_num,
                                uint8_t                        *rx_data,
                                uint16_t                         rx_num)
{
    const drv_resource_t    *resource;
    spi_drv_env_t           *env;
    dma_config_t             dma_config;
    om_error_t               error;
    spi_wire_t               wire;
    spi_role_t               role;

    OM_ASSERT(tx_data != NULL);
    OM_ASSERT(rx_data != NULL);
    OM_ASSERT((tx_num != 0U) || (rx_num != 0U));

    resource = spi_get_resource(om_spi);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }

    env = (spi_drv_env_t *)(resource->env);
    wire = om_spi->CTRL & SPI_CTRL_BIDIRECT_DATA_MASK ? SPI_WIRE_3 : SPI_WIRE_4;
    role = om_spi->CTRL & SPI_CTRL_MASTER_EN_MASK ? SPI_ROLE_MASTER : SPI_ROLE_SLAVE;

    if (wire == SPI_WIRE_4) {
        if((tx_num == 0) && (rx_num != 0)) {
            tx_num = rx_num;
        }
        if((rx_num == 0) && (tx_num != 0)) {
            rx_num = tx_num;
        }
    }

    om_spi->DMACR   = SPI_DMACR_RDMAE_MASK | SPI_DMACR_TDMAE_MASK;
    om_spi->DMATDLR = 0x1D;     // dma_tx_req is asserted when data in TxFifo <= 0x1D
    om_spi->DMARDLR = 0X00;     // dama_rx_req is asserted when data in RxFifo >= 1
    env->tx_buf = (uint8_t *)tx_data;
    env->tx_num = tx_num;
    env->tx_cnt = 0U;
    env->rx_buf = rx_data;
    env->rx_num = rx_num;
    env->rx_cnt = 0U;

    // config wire3 initial transfer state
    if (wire == SPI_WIRE_3) {
        if (role == SPI_ROLE_MASTER) {
            om_spi->CTRL &= ~SPI_CTRL_ACTIVE_DO_ENL_MASK;
            env->wire3_trans_state = SENDING;
        } else {
            om_spi->CTRL |= SPI_CTRL_ACTIVE_DO_ENL_MASK;
            env->wire3_trans_state = RECVING;
        }
    }

    // RX using dma
    if (resource->cap & CAP_SPI_GPDMA_RX_MASK) {
        dma_config.channel_ctrl  = DMA_SET_CTRL(DMA_ADDR_CTRL_FIXED, DMA_ADDR_CTRL_INC,
                                                DMA_TRANS_WIDTH_1B,  DMA_TRANS_WIDTH_1B,
                                                DMA_BURST_SIZE_1T, (resource->dma_rx.prio) ? DMA_PRIORITY_HIGH : DMA_PRIORITY_LOW);
        dma_config.src_id        = (dma_id_t)resource->dma_rx.id;
        dma_config.dst_id        = DMA_ID_MEM;
        dma_config.event_cb      = spi_dma_rx_event_cb;
        dma_config.cb_param      = (void *)resource;
        dma_config.chain_trans   = NULL;
        dma_config.chain_trans_num = 0U;

        error = drv_dma_channel_config(env->dma_rx_chan, &dma_config);
        if (error) {
            return error;
        }

        OM_CRITICAL_BEGIN();
        if ((wire == SPI_WIRE_4) || (env->wire3_trans_state == RECVING)) {
            if (rx_num) {
                if (wire == SPI_WIRE_4) {
                    register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 1,
                                                          SPI_CTRL_TX_FIFO_EN, 1));
                } else {
                    register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 1,
                                                          SPI_CTRL_TX_FIFO_EN, 0));
                }
                om_spi->CTRL |= SPI_CTRL_RX_CLR_FIFO_MASK;
                om_spi->CTRL &= ~SPI_CTRL_RX_CLR_FIFO_MASK;
                error = drv_dma_channel_enable(env->dma_rx_chan, (uint32_t)rx_data, (uint32_t)(&om_spi->RDATA), rx_num);
                if (error == OM_ERROR_OK) {
                    env->busy |= SPI_RX_BUSY;
                } else {
                    return error;
                }
            } else {
                if (env->wire3_trans_state == RECVING) {
                    om_spi->CTRL &= ~SPI_CTRL_ACTIVE_DO_ENL_MASK;
                    env->wire3_trans_state = SENDING;
                    env->busy &= ~SPI_RX_BUSY;
                }
            }
        }
        OM_CRITICAL_END();
    } else {
        return OM_ERROR_UNSUPPORTED;
    }

    // TX using dma
    if (resource->cap & CAP_SPI_GPDMA_TX_MASK) {
        dma_config.channel_ctrl  = DMA_SET_CTRL(DMA_ADDR_CTRL_INC, DMA_ADDR_CTRL_FIXED,
                                                DMA_TRANS_WIDTH_1B, DMA_TRANS_WIDTH_1B,
                                                DMA_BURST_SIZE_1T, (resource->dma_tx.prio) ? DMA_PRIORITY_HIGH : DMA_PRIORITY_LOW);
        dma_config.src_id        = DMA_ID_MEM;
        dma_config.dst_id        = (dma_id_t)resource->dma_tx.id;
        dma_config.event_cb      = spi_dma_tx_event_cb;
        dma_config.cb_param      = (void *)resource;
        dma_config.chain_trans   = NULL;
        dma_config.chain_trans_num = 0U;

        error = drv_dma_channel_config(env->dma_tx_chan, &dma_config);
        if (error) {
            return error;
        }

        if (role == SPI_ROLE_MASTER) {
            SPI_CSN_LOW(om_spi);
        }

        OM_CRITICAL_BEGIN();
        if ((wire == SPI_WIRE_4) || (env->wire3_trans_state == SENDING)) {
            if (tx_num) {
                if (wire == SPI_WIRE_4) {
                    register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 1,
                                                          SPI_CTRL_TX_FIFO_EN, 1));
                } else {
                    register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 0,
                                                          SPI_CTRL_TX_FIFO_EN, 1));
                }
                error = drv_dma_channel_enable(env->dma_tx_chan, (uint32_t)(&om_spi->WDATA), (uint32_t)tx_data, tx_num);
                if (error == OM_ERROR_OK) {
                    env->busy |= SPI_TX_BUSY;
                } else {
                    return error;
                }
            } else {
                if (env->wire3_trans_state == SENDING) {
                    om_spi->CTRL |= SPI_CTRL_RX_CLR_FIFO_MASK;
                    om_spi->CTRL &= ~SPI_CTRL_RX_CLR_FIFO_MASK;
                    om_spi->CTRL |= SPI_CTRL_ACTIVE_DO_ENL_MASK;
                    env->wire3_trans_state = RECVING;

                    register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 1,
                                                          SPI_CTRL_TX_FIFO_EN, 1));

                    env->tx_num = env->rx_num;
                    drv_dma_channel_enable(env->dma_rx_chan, (uint32_t)(env->rx_buf), (uint32_t)(&om_spi->RDATA), env->rx_num);
                    drv_dma_channel_enable(env->dma_tx_chan, (uint32_t)(&om_spi->WDATA), (uint32_t)(env->rx_buf), env->tx_num);
                    env->ignore_next_dma_tx_cb = 1U;
                    env->busy |= SPI_RX_BUSY | SPI_TX_BUSY;
                }
            }
        }
        OM_CRITICAL_END();
    } else {
        return OM_ERROR_UNSUPPORTED;
    }

    return OM_ERROR_OK;
}
#endif

void *drv_spi_control(OM_SPI_Type *om_spi, spi_control_t control, void *argu)
{
    uint32_t ret;
    uint16_t div;
    uint32_t clk;
    uint32_t freq;
    uint8_t  mode;
    uint8_t  bit_order;
    const drv_resource_t  *resource;
    spi_drv_env_t             *env;

    resource = spi_get_resource(om_spi);
    if(resource == NULL) {
        return (void *)OM_ERROR_PARAMETER;
    }
    env = (spi_drv_env_t *)(resource->env);
    ret = (uint32_t)OM_ERROR_OK;

    OM_CRITICAL_BEGIN();
    switch (control) {
        case SPI_CONTROL_CLK_DISABLE:
            do {
                #if (RTE_SPI0)
                if ((uint32_t)om_spi == (uint32_t)(spi0_resource.reg)) {
                    DRV_RCC_CLOCK_ENABLE(RCC_CLK_SPI0, 0U);
                    break;
                }
                #endif  /* RTE_SPI0 */
                #if (RTE_SPI1)
                if ((uint32_t)om_spi == (uint32_t)(spi1_resource.reg)) {
                    DRV_RCC_CLOCK_ENABLE(RCC_CLK_SPI1, 0U);
                    break;
                }
                #endif  /* RTE_SPI1 */
            } while(0);
            break;
        case SPI_CONTROL_CLK_ENABLE:
            do {
                #if (RTE_SPI0)
                if ((uint32_t)om_spi == (uint32_t)(spi0_resource.reg)) {
                    DRV_RCC_CLOCK_ENABLE(RCC_CLK_SPI0, 1U);
                    break;
                }
                #endif  /* RTE_SPI0 */
                #if (RTE_SPI1)
                if ((uint32_t)om_spi == (uint32_t)(spi1_resource.reg)) {
                    DRV_RCC_CLOCK_ENABLE(RCC_CLK_SPI1, 1U);
                    break;
                }
                #endif  /* RTE_SPI1 */
            } while(0);
            break;
        case SPI_CONTROL_RESET:
            do {
                #if (RTE_SPI0)
                if ((uint32_t)om_spi == (uint32_t)(spi0_resource.reg)) {
                    DRV_RCC_RESET(RCC_CLK_SPI0);
                    break;
                }
                #endif  /* RTE_SPI0 */
                #if (RTE_SPI1)
                if ((uint32_t)om_spi == (uint32_t)(spi1_resource.reg)) {
                    DRV_RCC_RESET(RCC_CLK_SPI1);
                    break;
                }
                #endif  /* RTE_SPI1 */
            } while(0);
            break;
        case SPI_CONTROL_GET_CLK:
            ret = drv_rcc_clock_get((rcc_clk_t)(size_t)resource->reg);
            break;
        case SPI_CONTROL_GET_RX_COUNT:
            ret = (uint32_t)(env->rx_cnt);
            break;
        case SPI_CONTROL_GET_TX_COUNT:
            ret = (uint32_t)(env->tx_cnt);
            break;
        case SPI_CONTROL_IS_BUSY:
            ret = env->busy;
            break;
        case SPI_CONTROL_DLY_SAMPLE_FE_SET:
            if (argu) {
                om_spi->DELAY_SAMPLE |= SPI_DELAY_SAMPLE_FE_EN_MASK;
            } else {
                om_spi->DELAY_SAMPLE &= ~SPI_DELAY_SAMPLE_FE_EN_MASK;
            }
            break;
        case SPI_CONTROL_DLY_SAMPLE_CYCLE_NUM_SET:
            register_set(&om_spi->DELAY_SAMPLE, MASK_1REG(SPI_DELAY_SAMPLE_CYCLE_NUM, (uint32_t)argu));
            break;
         case SPI_CONTROL_SET_FREQUENCY:
            clk = drv_rcc_clock_get((rcc_clk_t)(size_t)resource->reg);
            freq = (uint32_t)argu;
            div = clk / (freq);
            om_spi->CTRL &= ~SPI_CTRL_CLK_DIVIDER_MASK;
            om_spi->CTRL |= (int)((div / 2 - 1) << SPI_CTRL_CLK_DIVIDER_POS);
            break;
        case SPI_CONTROL_SET_MODE:
            mode = (uint8_t)(uint32_t)argu;
            om_spi->CTRL &= ~SPI_CTRL_MODE_MASK;
            om_spi->CTRL &= ~SPI_CTRL_INVERT_CLK_MASK;
            switch (mode) {
                case SPI_MODE_1:
                    om_spi->CTRL |= (SPI_CTRL_MODE_1 << SPI_CTRL_MODE_POS);
                    break;
                case SPI_MODE_2:
                    om_spi->CTRL |= (SPI_CTRL_CLK_HIGH_WHEN_IDLE << SPI_CTRL_INVERT_CLK_POS);
                    break;
                case SPI_MODE_3:
                    om_spi->CTRL |= (SPI_CTRL_MODE_1 << SPI_CTRL_MODE_POS) | (SPI_CTRL_CLK_HIGH_WHEN_IDLE << SPI_CTRL_INVERT_CLK_POS);
                    break;
                case SPI_MODE_0:
                    break;
                default:
                    break;
            }
            break;
        case SPI_CONTROL_CSN_LOW:
            om_spi->CSNCTRL = SPI_CSNCTRL_CS_MODE_MASK;
            break;
        case SPI_CONTROL_CSN_HIGH:
            om_spi->CSNCTRL = SPI_CSNCTRL_CS_MODE_MASK +  SPI_CSNCTRL_CS_GPO_MASK;
            break;
        case SPI_CONTROL_SET_BIT_ORDER:
            bit_order = (uint8_t)(uint32_t)argu;
            if (bit_order == SPI_MSB_FIRST) {
                om_spi->CTRL |= SPI_CTRL_MSB_FIRST_MASK;
            } else {
                om_spi->CTRL &= ~SPI_CTRL_MSB_FIRST_MASK;
            }
            break;
        default:
            break;
    }
    OM_CRITICAL_END();

    return (void *)ret;
}

void drv_spi_isr(OM_SPI_Type *om_spi)
{
    const drv_resource_t  *resource;
    spi_drv_env_t         *env;
    spi_wire_t             wire;
    spi_role_t             role;
    uint8_t                fifo_depth, fifo_size;
    volatile uint8_t       dummy;

    resource   = spi_get_resource(om_spi);
    if (resource == NULL) {
        return;
    }

    env  = (spi_drv_env_t *)(resource->env);
    wire = om_spi->CTRL & SPI_CTRL_BIDIRECT_DATA_MASK ? SPI_WIRE_3 : SPI_WIRE_4;
    role = om_spi->CTRL & SPI_CTRL_MASTER_EN_MASK ? SPI_ROLE_MASTER : SPI_ROLE_SLAVE;
    fifo_size  = register_get(&resource->cap, MASK_POS(CAP_SPI_FIFO_LEVEL));
    fifo_depth = fifo_size;

    DRV_IRQ_BEGIN();
    // clear SPI interrupt status
    om_spi->STAT |= SPI_STAT_SPI_INT_MASK;

    // 1. receive
    if ((wire == SPI_WIRE_4) || (env->wire3_trans_state == RECVING)) {
        while (om_spi->STAT & SPI_STAT_RX_NOT_EMPTY_MASK) {
            if (env->rx_cnt < env->rx_num) {
                if (env->rx_buf) {
                    env->rx_buf[env->rx_cnt] = om_spi->RDATA;
                } else {
                    dummy = om_spi->RDATA;
                }
                env->rx_cnt++;
            } else {
                break;
            }
        }
    }

    // 2. check current state
    // 2.1 transfer finished
    if ((env->rx_cnt == env->rx_num) && (env->tx_cnt == env->tx_num) && !(om_spi->STAT & SPI_STAT_TX_BYTE_CNT_MASK)) {
        om_spi->STAT &= ~(SPI_STAT_TX_EMPTY_INT_EN_MASK | SPI_STAT_RX_TRIG_INT_EN_MASK);    // disable interrupt
        if (env->wire3_trans_state == SENDING) {
            env->wire3_trans_state = RECVING;
        } else if (env->wire3_trans_state == RECVING) {
            env->wire3_trans_state = SENDING;
        }
        if (role == SPI_ROLE_MASTER) {
            SPI_CSN_HIGH(om_spi);
        }
        env->busy &= ~(SPI_RX_BUSY | SPI_TX_BUSY);
        drv_spi_isr_callback(om_spi, DRV_EVENT_COMMON_TRANSFER_COMPLETED, env->rx_buf, env->rx_cnt);
        goto _exit;
    }
    // 2.2 wire3 switch between send and reveive
    if (wire == SPI_WIRE_3) {
        if ((role == SPI_ROLE_MASTER) && (env->tx_cnt == env->tx_num)) {
            if (env->wire3_trans_state == SENDING) {
                om_spi->CTRL |= SPI_CTRL_RX_CLR_FIFO_MASK;
                om_spi->CTRL &= ~SPI_CTRL_RX_CLR_FIFO_MASK;    // clear rx fifo and status
                om_spi->CTRL |= SPI_CTRL_ACTIVE_DO_ENL_MASK;
                env->wire3_trans_state = RECVING;

                register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 1,
                                                      SPI_CTRL_TX_FIFO_EN, 1));
            }
            // 2.2.1 wire3 master receive:generate clk for slave
            uint16_t tx_dummy_cnt = env->rx_cnt;
            while((tx_dummy_cnt < env->rx_num) && fifo_depth
                        && (register_get(&om_spi->STAT, MASK_POS(SPI_STAT_TX_BYTE_CNT)) < fifo_size)) {
                om_spi->WDATA = SPI_TX_DUMMY;
                tx_dummy_cnt++;
                fifo_depth--;
            }
        } else if ((role == SPI_ROLE_SLAVE) && (env->rx_cnt == env->rx_num) && (env->wire3_trans_state == RECVING)) {
            om_spi->CTRL |= SPI_CTRL_RX_CLR_FIFO_MASK;
            om_spi->CTRL &= ~SPI_CTRL_RX_CLR_FIFO_MASK;    // clear rx fifo and status
            om_spi->CTRL &= ~SPI_CTRL_ACTIVE_DO_ENL_MASK;
            om_spi->STAT |= SPI_STAT_TX_EMPTY_INT_EN_MASK;
            om_spi->STAT &= ~SPI_STAT_RX_TRIG_INT_EN_MASK;
            env->wire3_trans_state = SENDING;

            register_set(&om_spi->CTRL, MASK_2REG(SPI_CTRL_RX_FIFO_EN, 0,
                                                  SPI_CTRL_TX_FIFO_EN, 1));
        }
    }

    // 2.3 sending
    if ((wire == SPI_WIRE_4) || (env->wire3_trans_state == SENDING)) {
        uint8_t cnt_to_send = fifo_size - register_get(&om_spi->STAT, MASK_POS(SPI_STAT_TX_BYTE_CNT));
        if (cnt_to_send == fifo_size) { // check if tx_fifo is really empty
            while((env->tx_cnt < env->tx_num) && fifo_depth && cnt_to_send > 0) {
                om_spi->WDATA = env->tx_buf ? env->tx_buf[env->tx_cnt] : SPI_TX_DUMMY;
                env->tx_cnt++;
                fifo_depth--;
                cnt_to_send--;
            }
        }
    }

_exit:
    (void)dummy;
    DRV_IRQ_END();
}


#endif  /* (RTE_SPI0 || RTE_SPI1) */

/** @} */

