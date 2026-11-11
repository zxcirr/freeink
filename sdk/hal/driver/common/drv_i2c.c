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
 * @file     drv_i2c.c
 * @brief    driver for i2c
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
#if (RTE_I2C0)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */
#define I2C_DISABLE                 0x0U
#define I2C_ENABLE                  0x1U

#define I2C_SPEED_STANDARD          0x1U
#define I2C_SPEED_FAST              0x2U
#define I2C_SPEED_HIGH              0x3U

#define I2C_TX_BUSY                 (1U << 0)
#define I2C_RX_BUSY                 (1U << 4)

/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * CONST & VARIABLES
 */
#if (RTE_I2C0)
static drv_env_t i2c0_env = {
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
    #endif
};

static const drv_resource_t i2c0_resource = {
    .cap        = CAP_I2C0,
    .reg        = OM_I2C0,
    .env        = &i2c0_env,
    .irq_num    = I2C0_IRQn,
    .irq_prio   = RTE_I2C0_IRQ_PRIORITY,
    #if (RTE_DMA)
    .dma_tx     = {
        .id     = DMA_ID_I2C0_TX,
        .prio   = RTE_I2C0_DMA_TX_PRIORITY,
    },
    .dma_rx     = {
        .id     = DMA_ID_I2C0_RX,
        .prio   = RTE_I2C0_DMA_RX_PRIORITY,
    }
    #endif
};
#endif


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static uint32_t i2c_scl_hcnt(int                      ic_clk,
                             int                     tsymbol,
                             int                          tf,
                             int                        cond,
                             int                      offset)
{
    /*
     * DesignWare I2C core doesn't seem to have solid strategy to meet
     * the tHD;STA timing spec.  Configuring _HCNT based on tHIGH spec
     * will result in violation of the tHD;STA spec.
     */
    if (cond) {
        /*
         * Conditional expression:
         *
         *   IC_[FS]S_SCL_HCNT + (1+4+3) >= IC_CLK * tHIGH
         *
         * This is based on the manuals, and represents an ideal
         * configuration.  The resulting I2C bus speed will be
         * faster than any of the others.
         *
         * If your hardware is free from tHD;STA issue, try this one.
         */
        return (((ic_clk * tsymbol) + 5000) / 10000 - 8) + offset;
    } else {
        /*
         * Conditional expression:
         *
         *   IC_[FS]S_SCL_HCNT + 3 >= IC_CLK * (tHD;STA + tf)
         *
         * This is just experimental rule; the tHD;STA period turned
         * out to be proportinal to (_HCNT + 3).  With this setting,
         * we could meet both tHIGH and tHD;STA timing specs.
         *
         * If unsure, you'd better to take this alternative.
         *
         * The reason why we need to take into account "tf" here,
         * is the same as described in i2c_lld_scl_lcnt().
         */
        return ((ic_clk * (tsymbol + tf) + 5000) / 10000 - 3) + offset;
    }
}

static uint32_t i2c_scl_lcnt(int                     ic_clk,
                             int                       tlow,
                             int                         tf,
                             int                     offset)
{
    /*
     * Conditional expression:
     *
     *   IC_[FS]S_SCL_LCNT + 1 >= IC_CLK * (tLOW + tf)
     *
     * DW I2C core starts counting the SCL CNTs for the LOW period
     * of the SCL clock (tLOW) as soon as it pulls the SCL line.
     * In order to meet the tLOW timing spec, we need to take into
     * account the fall time of SCL signal (tf).  Default tf value
     * should be 0.3 us, for safety.
     */
    return (((ic_clk * (tlow + tf) + 5000) / 10000) - 1) + offset;
}

static const drv_resource_t *i2c_get_resource(OM_I2C_Type *om_i2c)
{
    #if (RTE_I2C0)
    if ((uint32_t)om_i2c == (uint32_t)i2c0_resource.reg) {
        return &i2c0_resource;
    }
    #endif  /* (RTE_I2C0) */

    OM_ASSERT(0);
    return NULL;
}

static uint8_t i2c_dev_is_valid(OM_I2C_Type *om_i2c, uint32_t dev_addr)
{
    uint32_t dummy;
    dummy = om_i2c->CLR_STOP_DET;

    om_i2c->ENABLE  = I2C_DISABLE;
    om_i2c->TAR     = dev_addr;
    om_i2c->CON1    = I2C_CON1_TX_ENABLE;
    om_i2c->ENABLE  = I2C_ENABLE;
    om_i2c->DATA_CMD = 0x0U;

    // wait stop
    while (!(om_i2c->RAW_INTR_STAT & I2C_INTR_STOP_DET_MASK));

    if ((om_i2c->RAW_INTR_STAT & I2C_INTR_TX_ABRT_MASK)) {
        if ((om_i2c->TX_ABRT_SOURCE & I2C_TX_ABRT_SRC_7B_ADDR_NOACK_MASK)) {
            dummy = om_i2c->CLR_TX_ABRT;
            return 1U;
        }
    }

    dummy = om_i2c->CLR_TX_ABRT;
    (void)dummy;

    return 0U;
}

#if (RTE_DMA)
static void i2c_dma_rx_event_cb(void            *resource,
                                drv_event_t      event,
                                void            *next_chain)
{
    drv_env_t   *env;
    OM_I2C_Type *om_i2c;
    uint32_t     rx_num;
    drv_event_t  drv_event;

    if (resource == NULL) {
        return;
    }
    env     = (drv_env_t *)((const drv_resource_t *)resource)->env;
    om_i2c  = (OM_I2C_Type *)((const drv_resource_t *)resource)->reg;
    rx_num  = env->rx_num;
    env->rx_num = 0U;

    switch (event) {
        case DRV_DMA_EVENT_TERMINAL_COUNT_REQUEST:
            drv_event = DRV_EVENT_COMMON_READ_COMPLETED;
            break;
        case DRV_DMA_EVENT_ABORT:
            drv_event = DRV_EVENT_COMMON_ABORT;
            break;
        default:
            drv_event = DRV_EVENT_COMMON_ERROR;
            OM_ASSERT(0);
            break;
    }

    env->busy &= ~I2C_RX_BUSY;

    drv_i2c_isr_callback(om_i2c, drv_event, env->rx_buf, rx_num);
}

static void i2c_dma_tx_event_cb(void            *resource_temp,
                                drv_event_t      event,
                                void            *next_chain)
{
    drv_env_t   *env;
    OM_I2C_Type *om_i2c;
    uint32_t     tx_num;
    drv_event_t  drv_event;
    const drv_resource_t *resource;

    resource = (const drv_resource_t *)resource_temp;
    if (resource == NULL) {
        return;
    }
    env     = (drv_env_t *)resource->env;
    om_i2c  = (OM_I2C_Type *)resource->reg;
    tx_num  = env->tx_num;
    env->tx_num = 0U;

    switch (event) {
        case DRV_DMA_EVENT_TERMINAL_COUNT_REQUEST:
            drv_event = DRV_EVENT_COMMON_DMA2PERIPH_COMPLETED;
            while ((!(om_i2c->STATUS & I2C_STATUS_TFE_MASK)) && (!(om_i2c->RAW_INTR_STAT & I2C_INTR_TIME_OUT_MASK)) && (!((om_i2c->RAW_INTR_STAT & I2C_INTR_TX_ABRT_MASK)&&(om_i2c->TX_ABRT_SOURCE & (I2C_TX_ABRT_SRC_7B_ADDR_NOACK_MASK | I2C_TX_ABRT_SRC_10ADDR1_NOACK_MASK | I2C_TX_ABRT_SRC_10ADDR2_NOACK_MASK | I2C_TX_ABRT_SRC_TXDATA_NOACK_MASK)))));


            if (env->rx_cnt < env->rx_num) {
                om_i2c->CON1 = (env->rx_num | I2C_CON1_RX_ENABLE | I2C_CON1_READBYTES_UPDATE);
                drv_dma_channel_enable(env->dma_rx_chan, (uint32_t)env->rx_buf, (uint32_t)&om_i2c->DATA_CMD, env->rx_num);
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

    env->busy &= ~I2C_TX_BUSY;

    if (!(env->busy & I2C_RX_BUSY)) {
        drv_i2c_isr_callback(om_i2c, drv_event, env->tx_buf, tx_num);
    }
}
#endif


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
om_error_t drv_i2c_init(OM_I2C_Type *om_i2c, const i2c_config_t *cfg)
{
    const drv_resource_t  *resource;
    drv_env_t             *env;
    uint32_t               clk;

    OM_ASSERT(cfg);
    resource = i2c_get_resource(om_i2c);
    if ((resource == NULL) || (cfg == NULL)) {
        return OM_ERROR_PARAMETER;
    }

    DRV_RCC_RESET((uint32_t)om_i2c);
    env = (drv_env_t *)resource->env;

    om_i2c->ENABLE = I2C_DISABLE;
    switch (cfg->mode) {
        case I2C_MODE_MASTER:
            om_i2c->CON = I2C_CON_MASTER_MODE_MASK | I2C_CON_RESTART_EN_MASK;
            break;
        case I2C_MODE_SMBUS_HOST:
            om_i2c->CON = I2C_CON_MASTER_MODE_MASK | I2C_CON_10BITADDR_MASTER_MASK | I2C_CON_RESTART_EN_MASK;
            break;
        default:
            OM_ASSERT(0);
            return OM_ERROR_PARAMETER;
    }

    clk = drv_rcc_clock_get(RCC_CLK_I2C0);
    switch (cfg->speed) {
        case I2C_SPEED_100K:
            /* set standard and fast speed deviders for high/low periods */
            /* Standard-mode @100K period=10us */
            om_i2c->SS_SCL_HCNT = i2c_scl_hcnt(clk / 1000,
                                               40, /* tHD;STA = tHIGH = 4.0 us */
                                               3,  /* tf = 0.3 us */
                                               0,  /* 0: default, 1: Ideal */
                                               0); /* No offset */
            om_i2c->SS_SCL_LCNT = i2c_scl_lcnt(clk / 1000,
                                               47, /* tLOW = 4.7 us */
                                               3,  /* tf = 0.3 us */
                                               0); /* No offset */
            /* Standard mode clock_div calculate: Tlow/Thigh = 1/1.*/
            /* Sets the Maximum Rise Time for standard mode.*/
            register_set(&om_i2c->CON, MASK_1REG(I2C_CON_SPEED, I2C_SPEED_STANDARD));
            break;
        case I2C_SPEED_400K:
            /* Fast-mode @400K period=2.5us */
            om_i2c->FS_SCL_HCNT = i2c_scl_hcnt(clk / 1000,
                                               6, /* tHD;STA = tHIGH = 0.6 us */
                                               3, /* tf = 0.3 us */
                                               0, /* 0: default, 1: Ideal */
                                               0); /* No offset */
            om_i2c->FS_SCL_LCNT = i2c_scl_lcnt(clk / 1000,
                                               13, /* tLOW = 1.3 us */
                                               3, /* tf = 0.3 us */
                                               0); /* No offset */
            /* Sets the Maximum Rise Time for fast mode.*/
            register_set(&om_i2c->CON, MASK_1REG(I2C_CON_SPEED, I2C_SPEED_FAST));
            break;
        case I2C_SPEED_1M:
            /* High-mode @1M period=1us */
            om_i2c->HS_SCL_HCNT = i2c_scl_hcnt(clk / 1000,
                                               2, /* tHD;STA = tHIGH = 0.2 us */
                                               3, /* tf = 0.3 us */
                                               0, /* 0: default, 1: Ideal */
                                               0); /* No offset */
            om_i2c->HS_SCL_LCNT = i2c_scl_lcnt(clk / 1000,
                                               2, /* tLOW = 0.2 us */
                                               3, /* tf = 0.3 us */
                                               0); /* No offset */
            /* Sets the Maximum Rise Time for high mode.*/
            register_set(&om_i2c->CON, MASK_1REG(I2C_CON_SPEED, I2C_SPEED_HIGH));
            break;
        case I2C_SPEED_2M:
            /* High-mode @2M period=0.5us,need to set CPU clk 64M */
            om_i2c->HS_SCL_HCNT = i2c_scl_hcnt(clk / 1000,
                                               1, /* tHD;STA = tHIGH = 0.1 us */
                                               2, /* tf = 0.2 us */
                                               0, /* 0: default, 1: Ideal */
                                               0); /* No offset */
            om_i2c->HS_SCL_LCNT = i2c_scl_lcnt(clk / 1000,
                                               1, /* tLOW = 0.1 us */
                                               1, /* tf = 0.1 us */
                                               0); /* No offset */
            /* Sets the Maximum Rise Time for high mode.*/
            register_set(&om_i2c->CON, MASK_1REG(I2C_CON_SPEED, I2C_SPEED_HIGH));
            break;
        case I2C_SPEED_MAX:
            /* High-mode max,speed=clk/(i2c_scl_hcnt(min6)+i2c_scl_lcnt(min8)+9),if cpu clk 64M ,then SPI max speed 2.78M */
            om_i2c->HS_SCL_HCNT = 6;
            om_i2c->HS_SCL_LCNT = 8;
            /* Sets the Maximum Rise Time for high mode.*/
            register_set(&om_i2c->CON, MASK_1REG(I2C_CON_SPEED, I2C_SPEED_HIGH));
            break;
        default:
            OM_ASSERT(0);
            return OM_ERROR_PARAMETER;
    }

    #if (RTE_DMA)
    env->dma_tx_chan = DMA_NUMBER_OF_CHANNELS;
    env->dma_rx_chan = DMA_NUMBER_OF_CHANNELS;
    #endif

    NVIC_ClearPendingIRQ(resource->irq_num);
    NVIC_SetPriority(resource->irq_num, resource->irq_prio);
    NVIC_EnableIRQ(resource->irq_num);

    // mask all the interrupt
    om_i2c->INTR_MASK = 0;
	om_i2c->INTR_MASK |= I2C_INTR_TIME_OUT_MASK;
    /* tx fifo has 15 bytes or below then trigger the tx empty interrupt.*/
    om_i2c->TX_TL = 15U;
    /* rx fifo has received one byte then trigger the rx full interrupt.*/
    om_i2c->RX_TL = 0U;

    env->isr_cb = NULL;

    return OM_ERROR_OK;
}

#if (RTE_I2C_REGISTER_CALLBACK)
void drv_i2c_register_isr_callback(OM_I2C_Type                          *om_i2c,
                                   drv_isr_callback_t                        cb)
{
    const drv_resource_t  *resource;
    drv_env_t             *env;

    resource = i2c_get_resource(om_i2c);
    if (resource) {
        env = (drv_env_t *)resource->env;
        env->isr_cb = cb;
    }
}
#endif

__WEAK void drv_i2c_isr_callback(OM_I2C_Type *om_i2c, drv_event_t event, uint8_t *data, uint32_t num)
{
    #if (RTE_I2C_REGISTER_CALLBACK)
    const drv_resource_t  *resource;
    drv_env_t             *env;

    resource = i2c_get_resource(om_i2c);
    if (resource == NULL) {
        return;
    }
    env = (drv_env_t *)resource->env;

    if (env->isr_cb) {
        env->isr_cb(om_i2c, event, data, (void *)num);
    }
    #endif
}

om_error_t drv_i2c_master_write(OM_I2C_Type                             *om_i2c,
                                uint16_t                               dev_addr,
                                const uint8_t                          *tx_data,
                                uint32_t                                 tx_num,
                                uint32_t                             timeout_ms)
{
    uint16_t len;
    uint16_t fifo_padding;
    uint16_t fifo_size;
    uint16_t offset;
	uint32_t main_clk;
	uint32_t timeout_count;
    const drv_resource_t *resource;

    om_i2c->ENABLE      = I2C_DISABLE;
    om_i2c->TAR         = dev_addr;
    om_i2c->CON1        = I2C_CON1_TX_ENABLE;
    om_i2c->ENABLE      = I2C_ENABLE;

    resource = i2c_get_resource(om_i2c);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }

	NVIC_DisableIRQ(resource->irq_num);
	om_i2c->INTR_MASK = 0;
    om_i2c->INTR_MASK |= I2C_INTR_TIME_OUT_MASK;

	main_clk =  drv_rcc_clock_get(RCC_CLK_MAIN);//RCC_CLK_I2C0
	timeout_count=timeout_ms * (main_clk / 1000);//max=33554ms
	if (timeout_count == 0) {
		om_i2c->TIMEOUT = 0;
	} else {
	    om_i2c->TIMEOUT = (I2C_TIMEOUT_EN_TIMEOUT_MASK | timeout_count);
	}

    offset = 0U;
    fifo_size = register_get(&resource->cap, MASK_POS(CAP_I2C_FIFO_LEVEL));
    OM_CRITICAL_BEGIN();
    while (tx_num && (!drv_i2c_get_timeout_rawstate(om_i2c)) && (!drv_i2c_get_tx_abrt(om_i2c))) {
        fifo_padding = fifo_size - om_i2c->TXFLR;
        len = tx_num > fifo_padding ? fifo_padding : tx_num;
        for (uint32_t i = 0; i < len; i++) {
            om_i2c->DATA_CMD = tx_data[offset + i];
        }
        while ((om_i2c->TXFLR > om_i2c->TX_TL) && (!drv_i2c_get_timeout_rawstate(om_i2c)) && (!drv_i2c_get_tx_abrt(om_i2c)));

        offset += len;
        tx_num -= len;
    }
    OM_CRITICAL_END();
    while (
        (!(om_i2c->STATUS & I2C_STATUS_TFE_MASK) || (om_i2c->STATUS & I2C_STATUS_MST_ACTIVITY_MASK)) &&
        (!drv_i2c_get_timeout_rawstate(om_i2c)) &&
        (!drv_i2c_get_tx_abrt(om_i2c))
    ){};

	while(!((drv_i2c_get_timeout_rawstate(om_i2c) && (om_i2c->INTR_MASK == 2303)) || (drv_i2c_get_timeout_rawstate(om_i2c) == 0)));
	om_i2c->INTR_MASK = 0;
    om_i2c->INTR_MASK |= I2C_INTR_TIME_OUT_MASK;
    NVIC_ClearPendingIRQ(resource->irq_num);
    NVIC_SetPriority(resource->irq_num, resource->irq_prio);
    NVIC_EnableIRQ(resource->irq_num);
	if (om_i2c->RAW_INTR_STAT & I2C_INTR_TIME_OUT_MASK) {
	    return OM_ERROR_TIMEOUT;
	}

    return OM_ERROR_OK;
}

om_error_t drv_i2c_master_write_int(OM_I2C_Type                         *om_i2c,
                                    uint16_t                           dev_addr,
                                    const uint8_t                      *tx_data,
                                    uint32_t                             tx_num,
                                    uint32_t                         timeout_ms)
{
    const drv_resource_t *resource;
    drv_env_t            *env;
    uint32_t              fifo_size;
	uint32_t main_clk;
	uint32_t timeout_count;

    resource = i2c_get_resource(om_i2c);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (drv_env_t *)resource->env;
    fifo_size = register_get(&resource->cap, MASK_POS(CAP_I2C_FIFO_LEVEL));

    env->tx_buf = (uint8_t *)tx_data;
    env->tx_num = tx_num;
    env->tx_cnt = 0U;

    om_i2c->ENABLE = I2C_DISABLE;
    om_i2c->TAR    = dev_addr;
    om_i2c->CON1   = I2C_CON1_TX_ENABLE;
    om_i2c->ENABLE = I2C_ENABLE;

    OM_CRITICAL_BEGIN();
    while (fifo_size && env->tx_cnt < env->tx_num) {
        om_i2c->DATA_CMD = env->tx_buf[env->tx_cnt];
        env->tx_cnt++;
        fifo_size--;
    }

    env->busy |= I2C_TX_BUSY;

	om_i2c->INTR_MASK = 0;
    om_i2c->INTR_MASK |= I2C_INTR_TX_EMPTY_MASK | I2C_INTR_TX_ABRT_MASK;
	om_i2c->INTR_MASK &= ~(I2C_INTR_TIME_OUT_MASK);

	main_clk =  drv_rcc_clock_get(RCC_CLK_MAIN);//RCC_CLK_I2C0
	timeout_count=timeout_ms * (main_clk / 1000);//max=33554ms
	if (timeout_count == 0) {
		om_i2c->TIMEOUT = 0;
	} else {
	    om_i2c->TIMEOUT = (I2C_TIMEOUT_EN_TIMEOUT_MASK | timeout_count);
	}
    OM_CRITICAL_END();

    return OM_ERROR_OK;
}

#if (RTE_DMA)
om_error_t drv_i2c_dma_channel_allocate(OM_I2C_Type *om_i2c, i2c_dma_chan_t channel)
{
    const drv_resource_t  *resource;
    drv_env_t             *env;

    OM_ASSERT(channel <= I2C_DMA_CHAN_ALL);

    resource = i2c_get_resource(om_i2c);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (drv_env_t *)resource->env;

    if (channel == I2C_DMA_RX_CHAN) {
        if (env->dma_rx_chan >= DMA_NUMBER_OF_CHANNELS) {
            env->dma_rx_chan = drv_dma_channel_allocate();
            if (env->dma_rx_chan >= DMA_NUMBER_OF_CHANNELS) {
                return OM_ERROR_RESOURCES;
            }
        }
    } else if (channel == I2C_DMA_TX_CHAN) {
        if (env->dma_tx_chan >= DMA_NUMBER_OF_CHANNELS) {
            env->dma_tx_chan = drv_dma_channel_allocate();
            if (env->dma_tx_chan >= DMA_NUMBER_OF_CHANNELS) {
                return OM_ERROR_RESOURCES;
            }
        }
    } else if (channel == I2C_DMA_CHAN_ALL) {
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

om_error_t drv_i2c_dma_channel_release(OM_I2C_Type *om_i2c, i2c_dma_chan_t channel)
{
    const drv_resource_t  *resource;
    drv_env_t             *env;

    resource = i2c_get_resource(om_i2c);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (drv_env_t *)resource->env;

    OM_ASSERT(channel <= I2C_DMA_CHAN_ALL);

    if (channel == I2C_DMA_RX_CHAN) {
        drv_dma_channel_release(env->dma_rx_chan);
        env->dma_rx_chan = DMA_NUMBER_OF_CHANNELS;
    } else if (channel == I2C_DMA_TX_CHAN) {
        drv_dma_channel_release(env->dma_tx_chan);
        env->dma_tx_chan = DMA_NUMBER_OF_CHANNELS;
    } else if (channel == I2C_DMA_CHAN_ALL) {
        drv_dma_channel_release(env->dma_rx_chan);
        env->dma_rx_chan = DMA_NUMBER_OF_CHANNELS;
        drv_dma_channel_release(env->dma_tx_chan);
        env->dma_tx_chan = DMA_NUMBER_OF_CHANNELS;
    }

    return OM_ERROR_OK;
}

om_error_t drv_i2c_master_write_dma(OM_I2C_Type                         *om_i2c,
                                    uint16_t                           dev_addr,
                                    const uint8_t                      *tx_data,
                                    uint32_t                             tx_num,
                                    uint32_t                         timeout_ms)
{
    const drv_resource_t  *resource;
    drv_env_t             *env;
    om_error_t             error;
	uint32_t main_clk;
	uint32_t timeout_count;

    OM_ASSERT(tx_num);
    resource = i2c_get_resource(om_i2c);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (drv_env_t *)resource->env;

	om_i2c->INTR_MASK = 0;
	om_i2c->INTR_MASK |= I2C_INTR_TIME_OUT_MASK;

	main_clk =  drv_rcc_clock_get(RCC_CLK_MAIN);//RCC_CLK_I2C0
	timeout_count=timeout_ms * (main_clk / 1000);//max=33554ms
	if (timeout_count == 0) {
	    om_i2c->TIMEOUT = 0;
	} else {
	    om_i2c->TIMEOUT = (I2C_TIMEOUT_EN_TIMEOUT_MASK | timeout_count);
	}

    if (resource->cap & CAP_I2C_DMA_TX_MASK) {
        dma_config_t dma_cfg;

        dma_cfg.channel_ctrl = DMA_SET_CTRL(DMA_ADDR_CTRL_INC,
                                            DMA_ADDR_CTRL_FIXED,
                                            DMA_TRANS_WIDTH_1B,
                                            DMA_TRANS_WIDTH_1B,
                                            DMA_BURST_SIZE_1T,
                                            resource->dma_tx.prio ? DMA_PRIORITY_HIGH : DMA_PRIORITY_LOW);
        dma_cfg.src_id       = DMA_ID_MEM;
        dma_cfg.dst_id       = (dma_id_t)resource->dma_tx.id;
        dma_cfg.chain_trans  = NULL;
        dma_cfg.chain_trans_num = 0U;
        dma_cfg.event_cb     = i2c_dma_tx_event_cb;
        dma_cfg.cb_param     = (void *)resource;

        env->tx_buf          = (uint8_t *)tx_data;
        env->tx_num          = tx_num;
        env->tx_cnt          = 0U;

        om_i2c->DMA_CR       |= I2C_DMA_CR_TDMAE_MASK;
        om_i2c->DMA_TDLR     = 10U;     // when txfifo entries <= 10, assert dma_tx_req
        om_i2c->ENABLE       = I2C_DISABLE;
        om_i2c->TAR          = dev_addr;
        om_i2c->CON1         = I2C_CON1_TX_ENABLE;

        error = drv_dma_channel_config(env->dma_tx_chan, &dma_cfg);
        if (error != OM_ERROR_OK) {
            return error;
        }

        OM_CRITICAL_BEGIN();
        error = drv_dma_channel_enable(env->dma_tx_chan, (uint32_t)&om_i2c->DATA_CMD, (uint32_t)env->tx_buf, env->tx_num);
        if (error == OM_ERROR_OK) {
            env->busy |= I2C_TX_BUSY;
        } else {
            return error;
        }
        OM_CRITICAL_END();

        om_i2c->ENABLE = I2C_ENABLE;

        return error;
    } else {
        return OM_ERROR_UNSUPPORTED;
    }
}
#endif

om_error_t drv_i2c_master_read(OM_I2C_Type                              *om_i2c,
                               uint16_t                                dev_addr,
                               const uint8_t                           *tx_data,
                               uint16_t                                  tx_num,
                               uint8_t                                 *rx_data,
                               uint16_t                                  rx_num,
                               uint32_t                              timeout_ms)
{
	uint32_t main_clk;
	uint32_t timeout_count;
    const drv_resource_t  *resource;

    resource = i2c_get_resource(om_i2c);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }

    om_i2c->ENABLE      = I2C_DISABLE;
    om_i2c->TAR         = dev_addr;
    om_i2c->CON1        = I2C_CON1_TX_ENABLE;
    om_i2c->ENABLE      = I2C_ENABLE;

	NVIC_DisableIRQ(resource->irq_num);
	om_i2c->INTR_MASK = 0;
    om_i2c->INTR_MASK |= I2C_INTR_TIME_OUT_MASK;

	main_clk =  drv_rcc_clock_get(RCC_CLK_MAIN);//RCC_CLK_I2C0
	timeout_count=timeout_ms * (main_clk / 1000);//max=33554ms
	if (timeout_count == 0) {
		om_i2c->TIMEOUT = 0;
	} else {
	    om_i2c->TIMEOUT = (I2C_TIMEOUT_EN_TIMEOUT_MASK | timeout_count);
	}
    OM_CRITICAL_BEGIN();
    for (uint32_t i = 0; i < tx_num; i++) {
        while ((!(om_i2c->STATUS & I2C_STATUS_TFNF_MASK)) && (!drv_i2c_get_timeout_rawstate(om_i2c)) && (!drv_i2c_get_tx_abrt(om_i2c)));
        om_i2c->DATA_CMD = tx_data[i];
    }
    while ((!(om_i2c->STATUS & I2C_STATUS_TFE_MASK)) && (!drv_i2c_get_timeout_rawstate(om_i2c)) && (!drv_i2c_get_tx_abrt(om_i2c)));

    om_i2c->CON1 = (rx_num | I2C_CON1_RX_ENABLE | I2C_CON1_READBYTES_UPDATE);
    for (uint32_t i = 0; i < rx_num; i++) {
        while ((!(om_i2c->STATUS & I2C_STATUS_RFNE_MASK)) && (!drv_i2c_get_timeout_rawstate(om_i2c)) && (!drv_i2c_get_tx_abrt(om_i2c)) && (!drv_i2c_get_rxunder_rawstate(om_i2c)));

        rx_data[i] = om_i2c->DATA_CMD;
    }
    while (((om_i2c->STATUS & I2C_STATUS_RFNE_MASK) || (om_i2c->STATUS & I2C_STATUS_MST_ACTIVITY_MASK)) && (!drv_i2c_get_timeout_rawstate(om_i2c)) && (!drv_i2c_get_tx_abrt(om_i2c)) && (!drv_i2c_get_rxunder_rawstate(om_i2c)));

    OM_CRITICAL_END();

	while(!((drv_i2c_get_timeout_rawstate(om_i2c) && (om_i2c->INTR_MASK == 2303)) || (drv_i2c_get_timeout_rawstate(om_i2c) == 0)));
	om_i2c->INTR_MASK = 0;
    om_i2c->INTR_MASK |= I2C_INTR_TIME_OUT_MASK;
    NVIC_ClearPendingIRQ(resource->irq_num);
    NVIC_SetPriority(resource->irq_num, resource->irq_prio);
    NVIC_EnableIRQ(resource->irq_num);
	if (om_i2c->RAW_INTR_STAT & I2C_INTR_TIME_OUT_MASK) {
	    return OM_ERROR_TIMEOUT;
	}

    return OM_ERROR_OK;
}

om_error_t drv_i2c_master_read_int(OM_I2C_Type                          *om_i2c,
                                   uint16_t                            dev_addr,
                                   const uint8_t                       *tx_data,
                                   uint16_t                              tx_num,
                                   uint8_t                             *rx_data,
                                   uint16_t                              rx_num,
                                   uint32_t                          timeout_ms)
{
    const drv_resource_t *resource;
    drv_env_t            *env;
    uint32_t              fifo_size;
	uint32_t main_clk;
	uint32_t timeout_count;

    resource = i2c_get_resource(om_i2c);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (drv_env_t *)resource->env;
    fifo_size = register_get(&resource->cap, MASK_POS(CAP_I2C_FIFO_LEVEL));

    env->tx_buf = (uint8_t *)tx_data;
    env->tx_num = tx_num;
    env->tx_cnt = 0U;
    env->rx_buf = (uint8_t *)rx_data;
    env->rx_num = rx_num;
    env->rx_cnt = 0U;

    om_i2c->ENABLE = I2C_DISABLE;
    om_i2c->TAR    = dev_addr;
    om_i2c->CON1   = I2C_CON1_TX_ENABLE;
    om_i2c->ENABLE = I2C_ENABLE;

    OM_CRITICAL_BEGIN();
    while (fifo_size && env->tx_cnt < env->tx_num) {
        om_i2c->DATA_CMD = env->tx_buf[env->tx_cnt];
        env->tx_cnt++;
        fifo_size--;
    }

    env->busy |= I2C_TX_BUSY | I2C_RX_BUSY;

	om_i2c->INTR_MASK = 0;
    om_i2c->INTR_MASK |= I2C_INTR_TX_EMPTY_MASK | I2C_INTR_TX_ABRT_MASK | I2C_INTR_RX_UNDER_MASK;
	om_i2c->INTR_MASK &= ~(I2C_INTR_TIME_OUT_MASK);

	main_clk =  drv_rcc_clock_get(RCC_CLK_MAIN);//RCC_CLK_I2C0
	timeout_count=timeout_ms * (main_clk / 1000);//max=33554ms
	if (timeout_count == 0) {
		om_i2c->TIMEOUT = 0;
	} else {
	    om_i2c->TIMEOUT = (I2C_TIMEOUT_EN_TIMEOUT_MASK | timeout_count);
	}
    OM_CRITICAL_END();

    return OM_ERROR_OK;
}

#if (RTE_DMA)
om_error_t drv_i2c_master_read_dma(OM_I2C_Type                          *om_i2c,
                                   uint16_t                            dev_addr,
                                   const uint8_t                       *tx_data,
                                   uint16_t                              tx_num,
                                   uint8_t                             *rx_data,
                                   uint16_t                              rx_num,
                                   uint32_t                          timeout_ms)
{
    const drv_resource_t *resource;
    drv_env_t            *env;
    om_error_t            error;
	uint32_t main_clk;
	uint32_t timeout_count;

    OM_ASSERT(rx_num);

    resource = i2c_get_resource(om_i2c);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (drv_env_t *)resource->env;

	om_i2c->INTR_MASK = 0;
	om_i2c->INTR_MASK |= I2C_INTR_TIME_OUT_MASK;

	main_clk =  drv_rcc_clock_get(RCC_CLK_MAIN);//RCC_CLK_I2C0
	timeout_count=timeout_ms * (main_clk / 1000);//max=33554ms
	if (timeout_count == 0) {
	    om_i2c->TIMEOUT = 0;
	} else {
		om_i2c->TIMEOUT = (I2C_TIMEOUT_EN_TIMEOUT_MASK | timeout_count);
	}


    if (resource->cap & CAP_I2C_DMA_RX_MASK) {
        om_i2c->ENABLE          = I2C_DISABLE;
        om_i2c->TAR             = dev_addr;

        dma_config_t dma_rx_cfg;

        dma_rx_cfg.channel_ctrl = DMA_SET_CTRL(DMA_ADDR_CTRL_FIXED,
                                               DMA_ADDR_CTRL_INC,
                                               DMA_TRANS_WIDTH_1B,
                                               DMA_TRANS_WIDTH_1B,
                                               DMA_BURST_SIZE_1T,
                                               resource->dma_rx.prio ? DMA_PRIORITY_HIGH : DMA_PRIORITY_LOW);
        dma_rx_cfg.src_id       = (dma_id_t)resource->dma_rx.id;
        dma_rx_cfg.dst_id       = DMA_ID_MEM;
        dma_rx_cfg.chain_trans  = NULL;
        dma_rx_cfg.chain_trans_num = 0U;
        dma_rx_cfg.event_cb     = i2c_dma_rx_event_cb;
        dma_rx_cfg.cb_param     = (void *)resource;

        env->tx_buf         = (uint8_t *)tx_data;
        env->tx_num         = tx_num;
        env->tx_cnt         = 0U;
        env->rx_buf         = (uint8_t *)rx_data;
        env->rx_num         = rx_num;
        env->rx_cnt         = 0U;

        om_i2c->DMA_CR      |= I2C_DMA_CR_RDMAE_MASK;
        om_i2c->DMA_RDLR    = 0U;     // when rxfifo entries >= 1, assert dma_rx_req
        om_i2c->CON1        = I2C_CON1_RX_ENABLE;

        error = drv_dma_channel_config(env->dma_rx_chan, &dma_rx_cfg);
        if (error != OM_ERROR_OK) {
            return error;
        }

        if (env->tx_num == 0U) {
            om_i2c->CON1 = (env->rx_num | I2C_CON1_RX_ENABLE | I2C_CON1_READBYTES_UPDATE);
            error = drv_dma_channel_enable(env->dma_rx_chan, (uint32_t)env->rx_buf, (uint32_t)&om_i2c->DATA_CMD, env->rx_num);
            if (error == OM_ERROR_OK) {
                env->busy |= I2C_RX_BUSY;
            }
            om_i2c->ENABLE = I2C_ENABLE;
        } else {
            if (resource->cap & CAP_I2C_DMA_TX_MASK) {
                dma_config_t          dma_tx_cfg;

                dma_tx_cfg.channel_ctrl    = DMA_SET_CTRL(DMA_ADDR_CTRL_INC,
                                                          DMA_ADDR_CTRL_FIXED,
                                                          DMA_TRANS_WIDTH_1B,
                                                          DMA_TRANS_WIDTH_1B,
                                                          DMA_BURST_SIZE_1T,
                                                          resource->dma_tx.prio ? DMA_PRIORITY_HIGH : DMA_PRIORITY_LOW);
                dma_tx_cfg.src_id          = DMA_ID_MEM;
                dma_tx_cfg.dst_id          = (dma_id_t)resource->dma_tx.id;
                dma_tx_cfg.chain_trans     = NULL;
                dma_tx_cfg.chain_trans_num = 0U;
                dma_tx_cfg.event_cb        = i2c_dma_tx_event_cb;
                dma_tx_cfg.cb_param        = (void *)resource;

                om_i2c->DMA_CR       |= I2C_DMA_CR_TDMAE_MASK;
                om_i2c->DMA_TDLR     = 10U;     // when txfifo entries <= 10, assert dma_tx_req
                om_i2c->CON1         = I2C_CON1_TX_ENABLE;

                error = drv_dma_channel_config(env->dma_tx_chan, &dma_tx_cfg);
                if (error != OM_ERROR_OK) {
                    return error;
                }

                error = drv_dma_channel_enable(env->dma_tx_chan, (uint32_t)&om_i2c->DATA_CMD, (uint32_t)env->tx_buf, env->tx_num);
                if (error == OM_ERROR_OK) {
                    env->busy |= I2C_TX_BUSY | I2C_RX_BUSY;
                } else {
                    return error;
                }

                om_i2c->ENABLE = I2C_ENABLE;
            }
        }
    } else {
        return OM_ERROR_UNSUPPORTED;
    }

    return OM_ERROR_OK;
}
#endif

void *drv_i2c_control(OM_I2C_Type *om_i2c, i2c_control_t control, void *argu)
{
    const drv_resource_t    *resource;
    uint32_t                 ret;
    drv_env_t               *env;

    resource = i2c_get_resource(om_i2c);
    if (resource == NULL) {
        return (void *)OM_ERROR_PARAMETER;
    }
    env = (drv_env_t *)resource->env;
    ret = (uint32_t)OM_ERROR_OK;

    OM_CRITICAL_BEGIN();
    switch (control) {
        case I2C_CONTROL_IS_BUSY:
            ret = env->busy;
            break;
        case I2C_CONTROL_DEV_IS_VALID:
            ret = i2c_dev_is_valid(om_i2c, (uint32_t)argu);
            break;
        default:
            break;
    }
    OM_CRITICAL_END();

    return (void *)ret;
}

void drv_i2c_isr(OM_I2C_Type *om_i2c)
{
    uint16_t fifo_size;
    uint16_t fifo_padding;
    uint16_t len;
    uint32_t int_status;
    uint32_t int_status_source;
    uint32_t int_status_clr;
    drv_env_t *env;
    const drv_resource_t *resource;

    resource = i2c_get_resource(om_i2c);
    if (resource == NULL) {
        return;
    }
    env = (drv_env_t *)resource->env;

    DRV_IRQ_BEGIN();
    fifo_size = register_get(&resource->cap, MASK_POS(CAP_I2C_FIFO_LEVEL));
    int_status = om_i2c->INTR_STAT;
    int_status_source = om_i2c->TX_ABRT_SOURCE;
    if (int_status & I2C_INTR_TIME_OUT_MASK) {
        int_status_clr = om_i2c->CLR_TIME_OUT;
		if ((env->busy & I2C_TX_BUSY) == 0x1) {
			env->busy &= ~I2C_TX_BUSY;
		}
		if ((env->busy & I2C_RX_BUSY) == 0x10) {
			env->busy &= ~I2C_RX_BUSY;
		}
		drv_i2c_isr_callback(om_i2c, DRV_EVENT_I2C_TIMEOUT, NULL, 0);
    }
    if ((int_status & I2C_INTR_TX_ABRT_MASK)&&(int_status_source & (I2C_TX_ABRT_SRC_7B_ADDR_NOACK_MASK | I2C_TX_ABRT_SRC_10ADDR1_NOACK_MASK | I2C_TX_ABRT_SRC_10ADDR2_NOACK_MASK | I2C_TX_ABRT_SRC_TXDATA_NOACK_MASK))) {
		int_status_clr = om_i2c->CLR_TX_ABRT;
        if ((env->busy & I2C_TX_BUSY) == 0x1) {
			env->busy &= ~I2C_TX_BUSY;
		}
		if ((env->busy & I2C_RX_BUSY) == 0x10) {
			env->busy &= ~I2C_RX_BUSY;
		}
		if (int_status_source & (I2C_TX_ABRT_SRC_7B_ADDR_NOACK_MASK | I2C_TX_ABRT_SRC_10ADDR1_NOACK_MASK | I2C_TX_ABRT_SRC_10ADDR2_NOACK_MASK)) {
			drv_i2c_isr_callback(om_i2c, DRV_EVENT_I2C_TXADDR_NACK | DRV_EVENT_I2C_RXADDR_NACK, NULL, int_status_source);
		}
		if (int_status_source & I2C_TX_ABRT_SRC_TXDATA_NOACK_MASK) {
			drv_i2c_isr_callback(om_i2c, DRV_EVENT_I2C_TXDATA_NACK, NULL, int_status_source);
		}
    }
    if (int_status & I2C_INTR_RX_UNDER_MASK) {
        int_status_clr = om_i2c->CLR_RX_UNDER;
		if ((env->busy & I2C_RX_BUSY) == 0x10) {
			env->busy &= ~I2C_RX_BUSY;
			drv_i2c_isr_callback(om_i2c, DRV_EVENT_I2C_RXDATA_UNDER, NULL, 0);
		}

    }

    if (int_status & I2C_INTR_TX_EMPTY_MASK) {
        if (env->tx_cnt == env->tx_num) {
            om_i2c->INTR_MASK &= ~I2C_INTR_TX_EMPTY_MASK;
            while (!(om_i2c->STATUS & I2C_STATUS_TFE_MASK));

            env->busy &= ~I2C_TX_BUSY;

            if (!(env->busy & I2C_RX_BUSY)) {
                drv_i2c_isr_callback(om_i2c, DRV_EVENT_COMMON_WRITE_COMPLETED, env->tx_buf, env->tx_cnt);
            }

            // enable receive
            if (env->rx_cnt < env->rx_num) {
                om_i2c->CON1 = (env->rx_num | I2C_CON1_RX_ENABLE | I2C_CON1_READBYTES_UPDATE);
                om_i2c->INTR_MASK |= I2C_INTR_RX_FULL_MASK;
            }
        } else {
            fifo_padding = fifo_size - om_i2c->TXFLR;
            len = env->tx_num - env->tx_cnt;
            len = len > fifo_padding ? fifo_padding : len;
            for (uint32_t i = 0; i < len; i++) {
                om_i2c->DATA_CMD = env->tx_buf[env->tx_cnt++];
            }
        }
    }

    if (int_status & I2C_INTR_RX_FULL_MASK) {
        while (om_i2c->STATUS & I2C_STATUS_RFNE_MASK && env->rx_cnt < env->rx_num) {
            env->rx_buf[env->rx_cnt++] = om_i2c->DATA_CMD;
        }
        if (env->rx_cnt == env->rx_num) {
            om_i2c->INTR_MASK &= ~I2C_INTR_RX_FULL_MASK;
            env->busy &= ~I2C_RX_BUSY;

            drv_i2c_isr_callback(om_i2c, DRV_EVENT_COMMON_READ_COMPLETED, env->rx_buf, env->rx_cnt);
        }
    }

    (void)int_status_clr;
    DRV_IRQ_END();
}


#endif  /* RTE_I2C0 */

/** @} */