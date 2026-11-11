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
 * @file     usart.c
 * @brief    usart driver
 * @date     27. March 2020
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
#if (RTE_USART1)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */
#define USART_MODE_X_DIV            (16)
#define USART_FIFO_BUF_SIZE         (16)

#define USART_TX_BUSY               (1U << 0)
#define USART_RX_BUSY               (1U << 4)


/*******************************************************************************
 * CONST & VARIABLES
 */
/* usart1 information */
#if (RTE_USART1)
static drv_env_t usart1_env = {
    .isr_cb        = NULL,
    .tx_num        = 0,
    .tx_cnt        = 0,
    .tx_buf        = NULL,
    .rx_num        = 0,
    .rx_cnt        = 0,
    .rx_buf        = NULL,
    .busy          = 0,
    #if (RTE_DMA)
    .dma_tx_chan = DMA_NUMBER_OF_CHANNELS,
    .dma_rx_chan = DMA_NUMBER_OF_CHANNELS,
    #endif  /* (RTE_DMA) */
};

static const drv_resource_t usart1_resource = {
    .cap      = CAP_USART1,
    .reg      = OM_USART1,
    .env      = (void *)&usart1_env,
    .irq_num  = UART1_IRQn,
    .irq_prio = RTE_USART1_IRQ_PRIORITY,
    #if (RTE_DMA)
    #if (CAP_USART1 & CAP_USART_DMA_TX_MASK)
    .dma_tx = {
        .id   = DMA_ID_USART1_TX,
        .prio = RTE_USART1_DMA_TX_PRIORITY,
    },
    #endif  /* (CAP_USART1 & CAP_USART_DMA_TX_MASK) */
    #if (CAP_USART1 & CAP_USART_DMA_RX_MASK)
    .dma_rx = {
        .id   = DMA_ID_USART1_RX,
        .prio = RTE_USART1_DMA_RX_PRIORITY,
    },
    #endif  /* (CAP_USART1 & CAP_USART_DMA_RX_MASK) */
    #endif  /* (RTE_DMA) */
};
#endif  /* RTE_USART1 */


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static const drv_resource_t *usart_get_resource(OM_USART_Type *om_usart)
{
    #if (RTE_USART1)
    if ((uint32_t)om_usart == (uint32_t)(usart1_resource.reg)) {
        return &usart1_resource;
    }
    #endif  /* (RTE_USART1) */

    return NULL;
}

static void usart_set_baudrate(OM_USART_Type *om_usart, uint32_t baudrate)
{
    uint32_t        baud_divisor;
    uint32_t        freq;

    /* Compute divisor value. Normally, we should simply return:
     *   NS16550_CLK / MODE_X_DIV / baudrate
     * but we need to round that value by adding 0.5.
     * Rounding is especially important at high baud rates.
     */
    // calculate usart divisor int and baudrate divisor
    baud_divisor = 1;
    freq = baudrate * USART_MODE_X_DIV * baud_divisor;

    // double the divisor, until the int part is under 8 bit range
    while (drv_rcc_clock_set((rcc_clk_t)(size_t)om_usart, freq) != OM_ERROR_OK) {
        freq = freq << 1;
        baud_divisor = baud_divisor << 1;
    }

    /* Baud rate setting.*/
    om_usart->LCR |= UART_LCR_DLAB_MASK;
    om_usart->DLL  = baud_divisor & 0xff;
    om_usart->DLH  = (baud_divisor >> 8) & 0xff;
    om_usart->LCR &= ~UART_LCR_DLAB_MASK;
}

#if (RTE_DMA)
static void usart_dma_tx_event_cb(void *resource, drv_event_t event, void *next_chain)
{
    drv_env_t    *env;
    uint32_t      tx_num;
    drv_event_t   drv_event;
    OM_USART_Type *om_usart;

    om_usart = (OM_USART_Type *)(((const drv_resource_t *)resource)->reg);
    env      = (drv_env_t *)(((const drv_resource_t *)resource)->env);
    tx_num   = env->tx_num;
    env->tx_num = 0U;

    switch (event) {
        case DRV_DMA_EVENT_TERMINAL_COUNT_REQUEST:
            drv_event = DRV_EVENT_COMMON_DMA2PERIPH_COMPLETED;
            break;
        case DRV_DMA_EVENT_ABORT:
            drv_event = DRV_EVENT_COMMON_ABORT;
            break;
        default:    // DRV_DMA_EVENT_ERROR
            drv_event = DRV_EVENT_COMMON_ERROR;
            OM_ASSERT(0);
            break;
    }

    drv_usart_isr_callback(om_usart, drv_event, env->tx_buf, tx_num);
}

static void usart_dma_rx_event_cb(void *resource, drv_event_t event, void *next_chain)
{
    drv_env_t   *env;
    uint32_t     rx_num;
    drv_event_t  drv_event;
    OM_USART_Type *om_usart;

    if(resource == NULL) {
        return;
    }
    om_usart    = (OM_USART_Type *)(((const drv_resource_t *)resource)->reg);
    env         = (drv_env_t *)(((const drv_resource_t *)resource)->env);
    rx_num      = env->rx_num;
    env->rx_num = 0U;

    switch (event) {
        case DRV_DMA_EVENT_TERMINAL_COUNT_REQUEST:
            drv_event = DRV_EVENT_COMMON_READ_COMPLETED;
            break;
        case DRV_DMA_EVENT_ABORT:
            drv_event = DRV_EVENT_COMMON_ABORT;
            break;
        default:    // DRV_DMA_EVENT_ERROR
            drv_event = DRV_EVENT_COMMON_ERROR;
            OM_ASSERT(0);
            break;
    }

    drv_usart_isr_callback(om_usart, drv_event, env->rx_buf, rx_num);
}
#endif  /* (RTE_DMA) */

static drv_event_t usart_rx_line_int_handler(OM_USART_Type *om_usart)
{
    uint32_t            lsr;
    drv_event_t         event;

    event = DRV_EVENT_COMMON_NONE;
    lsr   = om_usart->LSR & UART_LSR_LINE_STATUS;

    // OverRun error
    if (lsr & UART_LSR_OE) {
        event |= DRV_EVENT_COMMON_RX_OVERFLOW;
    }
    // Parity error
    if (lsr & UART_LSR_PE) {
        event |= DRV_EVENT_USART_RX_PARITY_ERROR;
    }
    // Break detected
    if (lsr & UART_LSR_BI) {
        event |= DRV_EVENT_USART_RX_BREAK;
    }
    // Framing error
    if (lsr & UART_LSR_FE) {
        event |= DRV_EVENT_USART_RX_FRAME_ERROR;
    }

    return event;
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief USART initialization
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] usart_cfg      Configuration for usart
 *
 * @return status:
 *    - OM_ERROR_OK:         Nothing more to do
 *    - others:               No
 *******************************************************************************
 */
om_error_t drv_usart_init(OM_USART_Type *om_usart, const usart_config_t *usart_cfg)
{
    const drv_resource_t   *resource;
    drv_env_t              *env;

    OM_ASSERT(usart_cfg);
    drv_usart_control(om_usart, USART_CONTROL_RESET, NULL);
    resource = usart_get_resource(om_usart);
    if((resource == NULL) || (usart_cfg == NULL)) {
        return OM_ERROR_PARAMETER;
    }
    env = (drv_env_t *)(resource->env);

    // frame format
    {
        uint32_t lcr = 0U;

        // usart Data bits
        switch (usart_cfg->data_bit) {
            case USART_DATA_BIT_5:
                lcr |= UART_LCR_DLS_5;
                break;
            case USART_DATA_BIT_6:
                lcr |= UART_LCR_DLS_6;
                break;
            case USART_DATA_BIT_7:
                lcr |= UART_LCR_DLS_7;
                break;
            case USART_DATA_BIT_8:
                lcr |= UART_LCR_DLS_8;
                break;
            default:
                lcr = 0;
                OM_ASSERT(0);
                break;
        }
        // usart Parity
        switch (usart_cfg->parity) {
            case USART_PARITY_EVEN:
                lcr |= (UART_LCR_PEN_MASK | UART_LCR_EPS);
                break;
            case USART_PARITY_ODD:
                lcr |= UART_LCR_PEN_MASK;
                lcr &= ~UART_LCR_EPS;
                break;
            case USART_PARITY_NONE:
                lcr &= ~(UART_LCR_PEN_MASK | UART_LCR_EPS);
                break;
            default:
                OM_ASSERT(0);
                break;
        }
        // usart Stop bits
        switch (usart_cfg->stop_bit) {
            case USART_STOP_BIT_1_5:
            case USART_STOP_BIT_2:
                lcr |= UART_LCR_STOP_2B;
                break;
            case USART_STOP_BIT_1:
                lcr |= UART_LCR_STOP_1B;
                break;
            default:
                OM_ASSERT(0);
                break;
        }

        om_usart->LCR = lcr;
        // usart Baudrate
        usart_set_baudrate(om_usart, usart_cfg->baudrate);
    }

    // usart Flow control (RTS and CTS lines are only available on usart1)
    switch (usart_cfg->flow_control) {
        case USART_FLOW_CONTROL_NONE:
            om_usart->MCR = 0U;
            break;
        case USART_FLOW_CONTROL_RTS_CTS:
            if (resource->cap & CAP_USART_CTS_RTS_FLOW_CONTROL_MASK) {
                // Auto RTS -- Becomes active when the following occurs:
                //   - Auto Flow Control is selected during configuration
                //   - RTS (MCR[1] bit and MCR[5]bit are both set)
                om_usart->MCR = UART_MCR_RTS | UART_MCR_AFCE;
            } else {
                return OM_ERROR_PARAMETER;
            }
            break;
        default:
            OM_ASSERT(0);
            break;
    }

    // disable all interrupt
    om_usart->IER = 0U;

    // Configure FIFO Control register
    // Set trigger level
    om_usart->FCR = (UART_FCR_RST_RCVR) | (UART_FCR_RST_XMIT)
                    | (UART_FCR_FIFO_EN) | (UART_FCR_TRIGGER_REC_FIFO_1B)
                    | (UART_FCR_TRIGGER_TRANS_FIFO_0B);

    #if (RTE_DMA)
    env->dma_tx_chan = DMA_NUMBER_OF_CHANNELS;
    env->dma_rx_chan = DMA_NUMBER_OF_CHANNELS;
    #endif  /* (RTE_DMA) */

    // Clear and Enable usart IRQ
    NVIC_ClearPendingIRQ(resource->irq_num);
    NVIC_SetPriority(resource->irq_num, resource->irq_prio);
    NVIC_EnableIRQ(resource->irq_num);

    env->isr_cb = NULL;
    env->busy     = 0U;
    return OM_ERROR_OK;
}

#if (RTE_USART_REGISTER_CALLBACK)
/**
 *******************************************************************************
 * @brief Register event callback for transmit/receive by interrupt & dma mode
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] event_cb       Pointer to callback
 *
 *******************************************************************************
 */
void drv_usart_register_isr_callback(OM_USART_Type *om_usart, drv_isr_callback_t cb)
{
    const drv_resource_t *resource;
    drv_env_t            *env;

    resource = usart_get_resource(om_usart);
    if(resource == NULL) {
        return;
    }
    env = (drv_env_t *)(resource->env);
    env->isr_cb = cb;
}
#endif

__WEAK void drv_usart_isr_callback(OM_USART_Type *om_usart, drv_event_t event, uint8_t *data, uint32_t num)
{
    #if (RTE_USART_REGISTER_CALLBACK)
    const drv_resource_t *resource;
    drv_env_t            *env;

    resource = usart_get_resource(om_usart);
    if(resource == NULL) {
        return;
    }
    env = (drv_env_t *)(resource->env);

    if (env->isr_cb != NULL) {
        env->isr_cb(om_usart, event, data, (void *)num);
    }
    #endif
}

/**
 *******************************************************************************
 * @brief Transmit number of bytes from USART by block mode
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] data           Pointer where data to transmit from USART
 * @param[in] num            Number of data bytes to transmit
 *
 * @return status:
 *    - OM_ERROR_OK:         Transmit done
 *    - others:              No
 *******************************************************************************
 */
om_error_t drv_usart_write(OM_USART_Type  *om_usart,
                            const uint8_t      *data,
                            uint16_t             num,
                            uint32_t      timeout_ms)
{
    const drv_resource_t  *resource;
    drv_env_t             *env;
    om_error_t             error;

    // check input parameter
    OM_ASSERT(num != 0U);
    resource = usart_get_resource(om_usart);
    if(resource == NULL) {
        return OM_ERROR_PARAMETER;
    }

    env = (drv_env_t *)(resource->env);
    // Save transmit buffer info
    env->tx_buf = (uint8_t *)data;
    env->tx_num = num;
    env->tx_cnt = 0U;

    for (uint16_t i = 0; i < env->tx_num; i++) {
        DRV_WAIT_MS_UNTIL_TO(!(om_usart->LSR & UART_LSR_THRE), timeout_ms, error);
        if (error != OM_ERROR_OK) {
            return error;
        }
        om_usart->THR = env->tx_buf[env->tx_cnt];
        env->tx_cnt++;
    }

    // wait for fifo/hold reg and shift reg empty
    while (!(om_usart->LSR & UART_LSR_TEMT));

    return OM_ERROR_OK;
}

/**
 *******************************************************************************
 * @brief Transmit number of bytes from USART by interrupt mode
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] data           Pointer where data to transmit from USART
 * @param[in] num            Number of data bytes to transmit
 *
 * @return status:
 *    - OM_ERROR_OK:         Begin to transmit
 *    - others:               No
 *******************************************************************************
 */
om_error_t drv_usart_write_int(OM_USART_Type                 *om_usart,
                                const uint8_t                     *data,
                                uint16_t                            num)
{
    const drv_resource_t   *resource;
    drv_env_t              *env;

    // check input parameter
    OM_ASSERT(num != 0U);
    resource = usart_get_resource(om_usart);
    if(resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (drv_env_t *)(resource->env);

    // Save transmit buffer info
    env->tx_buf = (uint8_t *)data;
    env->tx_num = num;
    env->tx_cnt = 0U;

    // support programmable THRE
    #if 0
    // Check if FIFO is enabled
    if (om_usart->IIR & UART_IIR_FE) {
        // Enable programmable THRE interrupt, then THRE indicated FIFO full
        om_usart->IER |= UART_IER_P_THREI;
        // Enable transmit holding register empty interrupt
        OM_CRITICAL_BEGIN();
        while (!(om_usart->LSR & UART_LSR_THRE) && (env->tx_cnt < env->tx_num)) {
            om_usart->THR = env->tx_buf[env->tx_cnt++];
        }

        #if (RTE_PM)
        pm_sleep_prevent(resource->pm_id);
        #endif  /* (RTE_PM) */

        om_usart->IER |= UART_IER_THREI;
        OM_CRITICAL_END();
    } else {
    }
    #endif
    {
        OM_CRITICAL_BEGIN();
        while ((om_usart->LSR & UART_LSR_THRE) && (env->tx_cnt < env->tx_num)) {
            om_usart->THR = env->tx_buf[env->tx_cnt++];
        }
        // Enable transmit holding register empty interrupt
        om_usart->IER |= UART_IER_THREI;
        OM_CRITICAL_END();
    }

    return OM_ERROR_OK;
}

#if (RTE_DMA)
om_error_t drv_usart_dma_channel_allocate(OM_USART_Type *om_usart, usart_dma_chan_t channel)
{
    const drv_resource_t   *resource;
    drv_env_t              *env;

    OM_ASSERT(channel <= USART_DMA_CHAN_ALL);

    resource = usart_get_resource(om_usart);
    if(resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (drv_env_t *)(resource->env);

    if (channel == USART_DMA_RX_CHAN) {
        if (env->dma_rx_chan >= DMA_NUMBER_OF_CHANNELS) {
            env->dma_rx_chan = drv_dma_channel_allocate();
            if (env->dma_rx_chan >= DMA_NUMBER_OF_CHANNELS) {
                return OM_ERROR_RESOURCES;
            }
        }
    } else if (channel == USART_DMA_TX_CHAN) {
        if (env->dma_tx_chan >= DMA_NUMBER_OF_CHANNELS) {
            env->dma_tx_chan = drv_dma_channel_allocate();
            if (env->dma_tx_chan >= DMA_NUMBER_OF_CHANNELS) {
                return OM_ERROR_RESOURCES;
            }
        }
    } else if (channel == USART_DMA_CHAN_ALL) {
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

om_error_t drv_usart_dma_channel_release(OM_USART_Type *om_usart, usart_dma_chan_t channel)
{
    const drv_resource_t   *resource;
    drv_env_t              *env;

    OM_ASSERT(channel <= USART_DMA_CHAN_ALL);

    resource = usart_get_resource(om_usart);
    if(resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (drv_env_t *)(resource->env);

    if (channel == USART_DMA_RX_CHAN) {
        drv_dma_channel_release(env->dma_rx_chan);
        env->dma_rx_chan = DMA_NUMBER_OF_CHANNELS;
    } else if (channel == USART_DMA_TX_CHAN) {
        drv_dma_channel_release(env->dma_tx_chan);
        env->dma_tx_chan = DMA_NUMBER_OF_CHANNELS;
    } else if (channel == USART_DMA_CHAN_ALL) {
        drv_dma_channel_release(env->dma_rx_chan);
        env->dma_rx_chan = DMA_NUMBER_OF_CHANNELS;
        drv_dma_channel_release(env->dma_tx_chan);
        env->dma_tx_chan = DMA_NUMBER_OF_CHANNELS;
    }

    return OM_ERROR_OK;
}

/**
 *******************************************************************************
 * @brief Transmit number of bytes from USART by DMA mode
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] data           Pointer where data to transmit from USART
 * @param[in] num            Number of data bytes to USAFT transmiter
 *
 * @return status:
 *    - OM_ERROR_OK:         Begin to transmit
 *    - others:              No
 *******************************************************************************
 */
om_error_t drv_usart_write_dma(OM_USART_Type             *om_usart,
                               const uint8_t                 *data,
                               uint16_t                        num)
{
    const drv_resource_t   *resource;
    drv_env_t              *env;
    om_error_t              error;

    // check input parameter
    OM_ASSERT(num != 0U);
    resource = usart_get_resource(om_usart);
    if(resource == NULL) {
        return OM_ERROR_PARAMETER;
    }

    if (resource->cap & CAP_USART_DMA_TX_MASK) {
        dma_config_t dma_config;

        env = (drv_env_t *)(resource->env);

        // Save transmit buffer info
        env->tx_buf = (uint8_t *)data;
        env->tx_cnt = 0U;

        dma_config.channel_ctrl     = DMA_SET_CTRL(DMA_ADDR_CTRL_INC, DMA_ADDR_CTRL_FIXED,
                                                   DMA_TRANS_WIDTH_1B, DMA_TRANS_WIDTH_1B,
                                                   DMA_BURST_SIZE_1T, (resource->dma_tx.prio) ? DMA_PRIORITY_HIGH : DMA_PRIORITY_LOW);
        dma_config.src_id           = DMA_ID_MEM;
        dma_config.dst_id           = (dma_id_t)resource->dma_tx.id;
        dma_config.event_cb         = usart_dma_tx_event_cb;
        dma_config.cb_param         = (void *)resource;
        dma_config.chain_trans      = NULL;
        dma_config.chain_trans_num  = 0U;

        error = drv_dma_channel_config(env->dma_tx_chan, &dma_config);
        if (error) {
            return error;
        }

        OM_CRITICAL_BEGIN();
        error = drv_dma_channel_enable(env->dma_tx_chan, (uint32_t)(&(om_usart->THR)), (uint32_t)data, num);
        if (error == OM_ERROR_OK) {
            env->tx_num = num;
        }
        OM_CRITICAL_END();

        return error;
    } else {
        return OM_ERROR_UNSUPPORTED;
    }
}
#endif  /* (RTE_DMA) */

/**
 *******************************************************************************
 * @brief Prepare receive number of bytes by block mode
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] data           Pointer where data to receive from USART
 * @param[in] num            Number of data bytes from USART receiver
 *
 * @return status:
 *    - OM_ERROR_OK:         receive done
 *    - others:              No
 *******************************************************************************
 */
om_error_t drv_usart_read(OM_USART_Type             *om_usart,
                           uint8_t                      *data,
                           uint16_t                       num,
                           uint32_t                 timeout_ms)
{
    const drv_resource_t    *resource;
    drv_env_t               *env;
    om_error_t               error;

    // check input parameter
    OM_ASSERT((data != NULL) && (num != 0U));
    resource = usart_get_resource(om_usart);
    if(resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (drv_env_t *)(resource->env);
    env->rx_cnt = 0U;
    env->rx_num = num;
    env->rx_buf = (uint8_t *)data;

    for (uint16_t i = 0; i < env->rx_num; i++) {
        DRV_WAIT_MS_UNTIL_TO(!(om_usart->LSR & UART_LSR_DR), timeout_ms, error);
        if (error != OM_ERROR_OK) {
            return error;
        }
        env->rx_buf[env->rx_cnt] = om_usart->RBR;
        env->rx_cnt++;
    }

    return OM_ERROR_OK;
}

/**
 *******************************************************************************
 * @brief Prepare receive number of bytes by interrupt mode, if num is 0, usart would
 *        receive data until drv_usart_control with USART_CONTROL_ABORT_RECEIVE is called.
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] data           Pointer where data to receive from USART
 * @param[in] num            Number of data bytes from USART receiver
 *
 * @return status:
 *    - OM_ERROR_OK:         Begin to receive
 *    - others:              No
 *******************************************************************************
 */
om_error_t drv_usart_read_int(OM_USART_Type              *om_usart,
                               uint8_t                        *data,
                               uint16_t                         num)
{
    const drv_resource_t   *resource;
    drv_env_t              *env;

    // OM_ASSERT((num == 0 && data == NULL) || (num != 0 && data != NULL));
    resource = usart_get_resource(om_usart);
    if(resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env         = (drv_env_t *)(resource->env);
    env->rx_cnt = 0U;
    env->rx_num = num;
    env->rx_buf = (uint8_t *)data;

    OM_CRITICAL_BEGIN();
    // Enable Rx interrupts
    om_usart->IER |= (UART_IER_RLSI | UART_IER_RDI);
    OM_CRITICAL_END();

    return OM_ERROR_OK;
}

#if (RTE_DMA)
/**
 *******************************************************************************
 * @brief Prepare receive number of bytes by DMA mode
 *
 * @param[in] om_usart       Pointer to USART
 * @param[in] data           Pointer where data to receive from USART
 * @param[in] num            Number of data bytes from USART receiver
 *
 * @return status:
 *    - OM_ERROR_OK:         Begin to receive
 *    - others:              No
 *******************************************************************************
 */
om_error_t drv_usart_read_dma(OM_USART_Type             *om_usart,
                               uint8_t                       *data,
                               uint16_t                        num)
{
    const drv_resource_t   *resource;
    drv_env_t              *env;
    om_error_t              error;

    // check input parameter
    OM_ASSERT((data != NULL) && (num != 0U));
    resource = usart_get_resource(om_usart);
    if(resource == NULL) {
        return OM_ERROR_PARAMETER;
    }

    // Allocate & Configure channel
    if (resource->cap & CAP_USART_DMA_RX_MASK) {
        dma_config_t dma_config;

        // check usart is init & poweron
        env = (drv_env_t *)(resource->env);
        env->rx_cnt = 0U;
        env->rx_buf = (uint8_t *)data;

        dma_config.channel_ctrl     = DMA_SET_CTRL(DMA_ADDR_CTRL_FIXED, DMA_ADDR_CTRL_INC,
                                                   DMA_TRANS_WIDTH_1B, DMA_TRANS_WIDTH_1B,
                                                   DMA_BURST_SIZE_1T, (resource->dma_rx.prio) ? DMA_PRIORITY_HIGH : DMA_PRIORITY_LOW);
        dma_config.src_id           = (dma_id_t)resource->dma_rx.id;
        dma_config.dst_id           = DMA_ID_MEM;
        dma_config.event_cb         = usart_dma_rx_event_cb;
        dma_config.cb_param         = (void *)resource;
        dma_config.chain_trans      = NULL;
        dma_config.chain_trans_num  = 0U;

        error = drv_dma_channel_config(env->dma_rx_chan, &dma_config);
        if (error) {
            return error;
        }

        OM_CRITICAL_BEGIN();
        error = drv_dma_channel_enable(env->dma_rx_chan, (uint32_t)data, (uint32_t)(&(om_usart->RBR)), num);
        if (error == OM_ERROR_OK) {
            env->rx_num = num;
        }
        OM_CRITICAL_END();

        return error;
    } else {
        return OM_ERROR_UNSUPPORTED;
    }
}
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
void *drv_usart_control(OM_USART_Type *om_usart, usart_control_t control, void *argu)
{
    const drv_resource_t *resource;
    drv_env_t            *env;
    uint32_t              ret;

    ret = (uint32_t)OM_ERROR_OK;
    resource = usart_get_resource(om_usart);
    if(resource == NULL) {
        return (void*)OM_ERROR_PARAMETER;
    }
    env = (drv_env_t *)(resource->env);

    OM_CRITICAL_BEGIN();
    switch (control) {
        case USART_CONTROL_RESET:
            do {
                #if (RTE_USART1)
                if ((uint32_t)om_usart == (uint32_t)(usart1_resource.reg)) {
                    DRV_RCC_RESET(RCC_CLK_USART1);
                    break;
                }
                #endif  /* (RTE_USART1) */
            } while(0);
            break;
        case USART_CONTROL_CLK_DISABLE:
            do {
                #if (RTE_USART1)
                if ((uint32_t)om_usart == (uint32_t)(usart1_resource.reg)) {
                    DRV_RCC_CLOCK_ENABLE(RCC_CLK_USART1, 0U);
                    break;
                }
                #endif  /* (RTE_USART1) */
            } while(0);
            break;

        case USART_CONTROL_CLK_ENABLE:
            do {
               #if (RTE_USART1)
                if ((uint32_t)om_usart == (uint32_t)(usart1_resource.reg)) {
                    DRV_RCC_CLOCK_ENABLE(RCC_CLK_USART1, 1U);
                    break;
                }
                #endif  /* (RTE_USART1) */
            } while(0);
            break;

        // Abort Send
        case USART_CONTROL_ABORT_TRANSMIT:
            {
                #if (RTE_DMA)
                if ((resource->cap & CAP_USART_DMA_TX_MASK) && (env->dma_tx_chan < DMA_NUMBER_OF_CHANNELS)) {
                    drv_dma_channel_disable(env->dma_tx_chan);
                    env->dma_tx_chan = DMA_NUMBER_OF_CHANNELS;
                }
                #endif  /* (RTE_DMA) */

                // Interrupt mode
                om_usart->IER &= ~UART_IER_THREI;   // Disable transmit holding register empty interrupt
                // Transmit FIFO reset
                om_usart->FCR = (UART_FCR_RST_XMIT)
                                | (UART_FCR_FIFO_EN) | (UART_FCR_TRIGGER_REC_FIFO_1B)
                                | (UART_FCR_TRIGGER_TRANS_FIFO_0B);
            }
            break;

        // Abort receive
        case USART_CONTROL_ABORT_RECEIVE:
            {
                // DMA mode - disable DMA channel
                #if (RTE_DMA)
                if ((resource->cap & CAP_USART_DMA_RX_MASK) && (env->dma_rx_chan < DMA_NUMBER_OF_CHANNELS)) {
                    drv_dma_channel_disable(env->dma_rx_chan);
                    env->dma_rx_chan = DMA_NUMBER_OF_CHANNELS;
                }
                #endif  /* (RTE_DMA) */

                // Interrupt mode
                om_usart->IER &= ~(UART_IER_RDI | UART_IER_RLSI);  // Disable receive data available interrupt
                // Receive FIFO reset
                om_usart->FCR = (UART_FCR_RST_RCVR)
                                | (UART_FCR_FIFO_EN) | (UART_FCR_TRIGGER_REC_FIFO_1B)
                                | (UART_FCR_TRIGGER_TRANS_FIFO_0B);
            }
            break;
        case USART_CONTROL_SET_BAUDRATE:
            usart_set_baudrate(om_usart, (uint32_t)argu);
            break;
        case USART_CONTROL_GET_CAP:
            ret = resource->cap;
            break;
        case USART_CONTROL_IS_BUSY:
            ret = (env->busy != 0U);
            break;
        case USART_CONTROL_GET_TX_COUNT:
            #if (RTE_DMA)
            if ((resource->cap & CAP_USART_DMA_TX_MASK) && (env->dma_tx_chan < DMA_NUMBER_OF_CHANNELS) && drv_dma_channel_is_busy(env->dma_tx_chan)) {
                ret = env->tx_num - drv_dma_channel_get_left_count(env->dma_tx_chan);
            } else {
                ret = env->tx_cnt;
            }
            #else
            ret = env->tx_cnt;
            #endif  /* (RTE_DMA) */
            break;
        case USART_CONTROL_GET_RX_COUNT:
            #if (RTE_DMA)
            if ((resource->cap & CAP_USART_DMA_RX_MASK) && (env->dma_rx_chan < DMA_NUMBER_OF_CHANNELS) && drv_dma_channel_is_busy(env->dma_rx_chan)) {
                ret = env->rx_num - drv_dma_channel_get_left_count(env->dma_rx_chan);
            } else {
                ret = env->rx_cnt;
            }
            #else
            ret = env->rx_cnt;
            #endif  /* (RTE_DMA) */
            break;
        case USART_CONTROL_FLOWCTRL_ENABLE:
            // usart Flow control (RTS and CTS lines are only available on usart1)
            if (resource->cap & CAP_USART_CTS_RTS_FLOW_CONTROL_MASK) {
                om_usart->MCR = ((uint32_t)argu) ? (UART_MCR_RTS | UART_MCR_AFCE) : 0U;
            } else {
                ret = OM_ERROR_UNSUPPORTED;
            }
            break;
        case USART_CONTROL_IRQ_ENABLE:
            if((uint32_t)argu) {
                // TX
                om_usart->IER |= UART_IER_THREI;
            } else {
                // RX
                om_usart->IER |= (UART_IER_RLSI | UART_IER_RDI);
            }
            break;
        case USART_CONTROL_IRQ_DISABLE:
            if((uint32_t)argu) {
                // TX
                om_usart->IER &= ~UART_IER_THREI;
            } else {
                // RX
                om_usart->IER &= ~(UART_IER_RDI | UART_IER_RLSI);
            }
            break;
        default:
            break;
    }

    OM_CRITICAL_END();

    return (void *)ret;
}

/**
 *******************************************************************************
 * @brief usart interrupt service routine
 *
 * @param[in] om_usart       Pointer to USART
 *
 *******************************************************************************
 */
void drv_usart_isr(OM_USART_Type *om_usart)
{
    drv_event_t     event;
    drv_env_t       *env;
    const drv_resource_t  *resource;
    uint8_t         int_status;

    resource   = usart_get_resource(om_usart);
    if (resource == NULL) {
        return;
    }

    DRV_IRQ_BEGIN();
    env        = (drv_env_t *)(resource->env);
    int_status = (om_usart->IIR & UART_IIR_INT_ID_MASK);

    switch (int_status) {
        // Receive data avaliable & Character timeout indication
        case UART_IIR_RDI:
        case UART_IIR_CTI:
            event = DRV_EVENT_COMMON_NONE;

            // When rx_num is 0, usart will automatically receive data as long as
            // interrupt triggered, and interrupt will not be closed. If usart need
            // to be stopped, please use drv_usart_control() with USART_CONTROL_ABORT_RECEIVE.
            // When rx_num is not 0, usart will receive data with expected length
            if (env->rx_num == 0) {
                uint8_t buf[USART_FIFO_BUF_SIZE];
                uint8_t cnt = 0;

                // receive all available data
                while ((om_usart->LSR & UART_LSR_DR) && (cnt < sizeof(buf))) {
                    buf[cnt++] = om_usart->RBR;
                }
                // Character time-out indicator
                if (int_status == UART_IIR_CTI) {
                    // Signal RX Time-out event, if not all requested data received
                    event |= DRV_EVENT_USART_RX_TIMEOUT;
                }

                event |= DRV_EVENT_COMMON_READ_COMPLETED;
                drv_usart_isr_callback(om_usart, event, buf, cnt);
            } else {
                while (om_usart->LSR & UART_LSR_DR) {
                    env->rx_buf[env->rx_cnt] = om_usart->RBR;
                    env->rx_cnt++;
                    // Check if requested amount of data is received
                    if (env->rx_cnt == env->rx_num) {
                        // Check RX line interrupt for errors
                        event = (drv_event_t)(DRV_EVENT_COMMON_READ_COMPLETED | usart_rx_line_int_handler(om_usart));

                        om_usart->IER &= ~(UART_IER_RLSI | UART_IER_RDI);
                        break;
                    }
                }

                // Character time-out indicator
                if (int_status == UART_IIR_CTI) {
                    // Signal RX Time-out event, if not all requested data received
                    if (env->rx_cnt != env->rx_num) {
                        event |= DRV_EVENT_USART_RX_TIMEOUT;
                    }
                }
                drv_usart_isr_callback(om_usart, event, env->rx_buf, env->rx_cnt);
            }
            break;

        // Transmit holding register empty
        case UART_IIR_THREI:
            {
                // Check if all data is transmitted
                if (env->tx_num == env->tx_cnt) {
                    // Disable THRE interrupt
                    om_usart->IER &= ~UART_IER_THREI;

                    // send tx complete event
                    drv_usart_isr_callback(om_usart, DRV_EVENT_COMMON_WRITE_COMPLETED, env->tx_buf, env->tx_cnt);
                } else {
                    // support programmable THRE
                    #if 0
                    // Check if FIFO is enabled
                    if (om_usart->IIR & UART_IIR_FE) {
                        while (!(om_usart->LSR & UART_LSR_THRE) && (env->tx_num != env->tx_cnt)) {
                            // Write data to Tx FIFO
                            om_usart->THR = env->tx_buf[env->tx_cnt];
                            env->tx_cnt++;
                        }
                    } else {
                    }
                    #endif
                    {
                        while ((om_usart->LSR & UART_LSR_THRE) && (env->tx_num != env->tx_cnt)) {
                            // Write data to Tx FIFO
                            om_usart->THR = env->tx_buf[env->tx_cnt];
                            env->tx_cnt++;
                        }
                    }
                }
            }
            break;
        // Receive line status
        case UART_IIR_RLSI:
            event = usart_rx_line_int_handler(om_usart);

            drv_usart_isr_callback(om_usart, event, env->rx_buf, 0U);
            break;

        default:
            break;
    }
    DRV_IRQ_END();
}


#endif  /* (RTE_USART1) */


/** @} */


