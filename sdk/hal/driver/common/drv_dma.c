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
 * @file     drv_dma.c
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
#if (RTE_DMA)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */
#define DMA_CHANNEL(ch)       (&(OM_DMA->CHAN[ch]))


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    dma_event_callback_t    event_cb[DMA_NUMBER_OF_CHANNELS];
    void                    *cb_param[DMA_NUMBER_OF_CHANNELS];
    dma_id_t                ids[DMA_NUMBER_OF_CHANNELS][2];
    uint8_t                 chan_mask;
    uint8_t                 busy_mask;
} dma_env_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
static dma_env_t dma_env = {
    .event_cb  = {NULL},
    .chan_mask = 0U,
    .busy_mask = 0U,
    .ids       = {
        {DMA_ID_INVAILD, DMA_ID_INVAILD}, {DMA_ID_INVAILD, DMA_ID_INVAILD},
        {DMA_ID_INVAILD, DMA_ID_INVAILD}, {DMA_ID_INVAILD, DMA_ID_INVAILD},
        {DMA_ID_INVAILD, DMA_ID_INVAILD}, {DMA_ID_INVAILD, DMA_ID_INVAILD},
        {DMA_ID_INVAILD, DMA_ID_INVAILD}, {DMA_ID_INVAILD, DMA_ID_INVAILD}
    },
};

static const drv_resource_t  dma_resource = {
    .cap      = DMA_NUMBER_OF_CHANNELS,
    .reg      = OM_DMA,
    .irq_num  = DMA_IRQn,
    .irq_prio = RTE_DMA_IRQ_PRIORITY,
    .env      = (void *) &dma_env,
};


/*******************************************************************************
 * LOCAL FUNCTIONS
 */


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
uint8_t drv_dma_channel_allocate(void)
{
    uint8_t chan_idx;

    OM_CRITICAL_BEGIN();
    for (chan_idx = 0U; chan_idx < DMA_NUMBER_OF_CHANNELS; chan_idx++) {
        if (dma_env.chan_mask & (1 << chan_idx)) {
            continue;
        } else {
            if (dma_env.chan_mask == 0) {       // first allocate
                DRV_RCC_RESET(RCC_CLK_DMA);
                /* Reset dma core, disable all channels */
                OM_DMA->CTRL |= DMA_RESET_MASK;

                // Clear and Enable DMA IRQ
                NVIC_SetPriority(dma_resource.irq_num, dma_resource.irq_prio);
                NVIC_ClearPendingIRQ(dma_resource.irq_num);
                NVIC_EnableIRQ(dma_resource.irq_num);
            }
            dma_env.chan_mask |= (1U << chan_idx);
            dma_env.busy_mask &= ~(1U << chan_idx);
            dma_env.ids[chan_idx][0] = DMA_ID_INVAILD;
            dma_env.ids[chan_idx][1] = DMA_ID_INVAILD;
            break;
        }
    }
    OM_CRITICAL_END();

    return chan_idx;
}

void drv_dma_channel_release(uint8_t chan_idx)
{
    OM_ASSERT(chan_idx < DMA_NUMBER_OF_CHANNELS);
    OM_CRITICAL_BEGIN();
    dma_env.busy_mask &= ~(1U << chan_idx);
    dma_env.chan_mask &= ~(1U << chan_idx);

    dma_env.ids[chan_idx][0] = DMA_ID_INVAILD;
    dma_env.ids[chan_idx][1] = DMA_ID_INVAILD;

    if (dma_env.chan_mask == 0) {
        DRV_RCC_CLOCK_ENABLE(RCC_CLK_DMA, 0U);
    }
    OM_CRITICAL_END();
}

om_error_t drv_dma_channel_config(uint8_t chan_idx, const dma_config_t *config)
{
    OM_DMA_CHAN_Type    *dma_ch;
    dma_id_t            id = DMA_ID_INVAILD;
    uint32_t            channel_ctrl = config->channel_ctrl;
    uint8_t             src_width;
    uint8_t             dst_width;
    uint8_t             src_burst_size;

    src_width      = 1 << register_get(&channel_ctrl, MASK_POS(DMA_SRCWIDTH));
    dst_width      = 1 << register_get(&channel_ctrl, MASK_POS(DMA_DSTWIDTH));
    src_burst_size = 1 << register_get(&channel_ctrl, MASK_POS(DMA_SRCBURSTSIZE));

    OM_ASSERT(OM_IS_ALIGN(src_width * src_burst_size, dst_width)); // burst_bytes must be aligned to dst_width
    OM_ASSERT(chan_idx < DMA_NUMBER_OF_CHANNELS);

    if (dma_env.busy_mask & (1U << chan_idx)) {
        return OM_ERROR_BUSY;
    }

    // src dst id config
    if (config->src_id != DMA_ID_MEM) {
        id = config->src_id;
        for (uint8_t i = 0; i < DMA_NUMBER_OF_CHANNELS; i++) {              // check conflict
            if (i == chan_idx) {
                continue;
            }
            if ((dma_env.ids[i][0] == id) || (dma_env.ids[i][1] == id)) {
                return OM_ERROR_PARAMETER;
            }
        }

        dma_env.ids[chan_idx][0] = id;
        register_set(&channel_ctrl, MASK_2REG(DMA_SRCMODE,    1,
                                              DMA_SRCREQSEL,  id));
    }
    if (config->dst_id != DMA_ID_MEM) {
        id = config->dst_id;
        for (uint8_t i = 0; i < DMA_NUMBER_OF_CHANNELS; i++) {
            if (i == chan_idx) {
                continue;
            }
            if ((dma_env.ids[i][0] == id) || (dma_env.ids[i][1] == id)) {
                return OM_ERROR_PARAMETER;
            }
        }

        dma_env.ids[chan_idx][1] = id;
        register_set(&channel_ctrl, MASK_2REG(DMA_DSTMODE,    1,
                                              DMA_DSTREQSEL,  id));
    }

    // Save callback pointer
    dma_env.event_cb[chan_idx] = config->event_cb;
    dma_env.cb_param[chan_idx] = config->cb_param;

    dma_ch = DMA_CHANNEL(chan_idx);

    dma_ch->CTRL = channel_ctrl;

    // Link list partital config
    if (config->chain_trans_num) {
        dma_ch->LL_PTR = (uint32_t)(config->chain_trans);
        for (uint8_t i = 0; i < config->chain_trans_num; i++) {
            OM_ASSERT(OM_IS_ALIGN(config->chain_trans[i].size_byte, src_width)); // total trans bytes must be aligned to src,dst width
            OM_ASSERT(OM_IS_ALIGN(config->chain_trans[i].size_byte, dst_width));
            OM_ASSERT(OM_IS_ALIGN(config->chain_trans[i].src_addr, src_width));
            OM_ASSERT(OM_IS_ALIGN(config->chain_trans[i].dst_addr, dst_width));

            config->chain_trans[i].trans_size = config->chain_trans[i].size_byte / src_width;
            config->chain_trans[i].ctrl       = channel_ctrl | DMA_ENABLE_MASK;  // link list item's enable bit does not affect
        }
    } else {
        dma_ch->LL_PTR = 0U;
    }

    return OM_ERROR_OK;
}

om_error_t drv_dma_channel_enable(uint8_t chan_idx, uint32_t dst_addr, uint32_t src_addr, uint32_t total_trans_byte)
{
    OM_DMA_CHAN_Type    *dma_ch;
    uint8_t             src_width;
    uint8_t             dst_width;
    uint8_t             src_burst_size;

    // Check if channel is valid
    OM_ASSERT(chan_idx < DMA_NUMBER_OF_CHANNELS);

    // Check if channel is busy
    if (dma_env.busy_mask & (1U << chan_idx)) {
        return OM_ERROR_BUSY;
    }

    // Check if channel is allocated
    if (dma_env.chan_mask & (1U << chan_idx)) {
        OM_CRITICAL_BEGIN();
        dma_env.busy_mask |= (1U << chan_idx);
        OM_CRITICAL_END();
    } else {
        return OM_ERROR_RESOURCES;
    }

    dma_ch = DMA_CHANNEL(chan_idx);

    src_width      = 1 << register_get(&dma_ch->CTRL, MASK_POS(DMA_SRCWIDTH));
    dst_width      = 1 << register_get(&dma_ch->CTRL, MASK_POS(DMA_DSTWIDTH));
    src_burst_size = 1 << register_get(&dma_ch->CTRL, MASK_POS(DMA_SRCBURSTSIZE));

    // burst size bytes check
    OM_ASSERT(OM_IS_ALIGN(src_width * src_burst_size, dst_width));
    // sources address must be aligned to the source transfer size
    // destination address must be aligned to the destination trfansfer size.
    OM_ASSERT(OM_IS_ALIGN(src_addr, src_width));
    OM_ASSERT(OM_IS_ALIGN(dst_addr, dst_width));
    // transfer size check
    OM_ASSERT(total_trans_byte != 0U);
    OM_ASSERT(OM_IS_ALIGN(total_trans_byte, src_width));
    OM_ASSERT(OM_IS_ALIGN(total_trans_byte, dst_width));
    OM_ASSERT(total_trans_byte / src_width <= DMA_CHAN_MAX_TRANS_SIZE);

    dma_ch->SRC_ADDR   = src_addr;
    dma_ch->DST_ADDR   = dst_addr;
    dma_ch->TRANS_SIZE = total_trans_byte / src_width;

    dma_ch->CTRL |= DMA_ENABLE_MASK;

    return OM_ERROR_OK;
}

om_error_t drv_dma_channel_disable(uint8_t chan_idx)
{
    // Check if channel is valid
    OM_ASSERT(chan_idx < DMA_NUMBER_OF_CHANNELS);

    // abort current transfer
    OM_DMA->CHAN_ABORT          = (1U << chan_idx);
    DMA_CHANNEL(chan_idx)->CTRL &= ~DMA_ENABLE_MASK;

    // Clear Channel active flag
    OM_CRITICAL_BEGIN();
    dma_env.busy_mask &= ~(1U << chan_idx);
    OM_CRITICAL_END();

    return OM_ERROR_OK;
}

uint8_t drv_dma_channel_is_busy(uint8_t chan_idx)
{
    // Check if channel is valid
    OM_ASSERT(chan_idx < DMA_NUMBER_OF_CHANNELS);

    if (dma_env.busy_mask & (1U << chan_idx)) {
        return 1U;
    } else {
        return 0U;
    }
}

uint32_t drv_dma_channel_get_left_count(uint8_t chan_idx)
{
    OM_DMA_CHAN_Type *dma_ch;
    uint8_t  src_width;

    OM_ASSERT(chan_idx < DMA_NUMBER_OF_CHANNELS);

    dma_ch = DMA_CHANNEL(chan_idx);
    src_width = 1 << register_get(&dma_ch->CTRL, MASK_POS(DMA_SRCWIDTH));

    return (dma_ch->TRANS_SIZE * src_width);
}

void drv_dma_isr(void)
{
    uint32_t             ch;
    OM_DMA_CHAN_Type     *dma_ch;
    drv_event_t          event = DRV_EVENT_COMMON_NONE;
    uint32_t             int_status;

    DRV_IRQ_BEGIN();
    while (OM_DMA->INT_STATUS) {
        int_status = OM_DMA->INT_STATUS;
        for (ch = 0; ch < DMA_NUMBER_OF_CHANNELS; ch++) {
            if (int_status & DMA_INT_STATUS_ALL(ch)) {
                dma_ch = DMA_CHANNEL(ch);
                if (int_status & DMA_INT_STATUS_TC_MASK(ch)) {  // Terminal count request interrupt
                    // Clear interrupt flag
                    OM_DMA->INT_STATUS = DMA_INT_STATUS_TC_MASK(ch);
                    if (!OM_DMA->LLP_SHADOW[ch]) {
                        dma_env.busy_mask &= ~(1U << ch);
                    }
                    if (OM_DMA->LLP_SHADOW[ch] && (dma_ch->CTRL & 0x01) == 0x0) {
                        dma_env.busy_mask &= ~(1U << ch);
                    }
                    event |= DRV_DMA_EVENT_TERMINAL_COUNT_REQUEST;
                }
                if (int_status & DMA_INT_STATUS_ERROR_MASK(ch)) {  // Error interrupt
                    dma_ch->CTRL = 0U;
                    // Clear interrupt flag
                    OM_DMA->INT_STATUS = DMA_INT_STATUS_ERROR_MASK(ch);
                    event |= DRV_DMA_EVENT_ERROR;
                    dma_env.busy_mask &= ~(1U << ch);
                }
                if (int_status & DMA_INT_STATUS_ABORT_MASK(ch)) {  // abort interrupt
                    dma_ch->CTRL = 0U;
                    // Clear interrupt flag
                    OM_DMA->INT_STATUS = DMA_INT_STATUS_ABORT_MASK(ch);
                    event |= DRV_DMA_EVENT_ABORT;
                    dma_env.busy_mask &= ~(1U << ch);
                }

                // Signal Event
                if (dma_env.event_cb[ch]) {
                    dma_env.event_cb[ch](dma_env.cb_param[ch], event, (void *)(OM_DMA->LLP_SHADOW[ch]));
                }
            }
        }
    }
    DRV_IRQ_END();
}

#endif /* RTE_DMA */

/** @} */
