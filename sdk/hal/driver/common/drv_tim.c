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
 * @file     drv_tim.c
 * @brief    driver for gp_timer
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
#if (RTE_TIM0 || RTE_TIM1 || RTE_TIM2)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */
#define TIM_NUM                 3
#define TIM_ARR_MAX             0xFFFF
#define TIM_PSC_MAX             0xFFFF


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    drv_isr_callback_t      isr_cb;
    #if (RTE_DMA)
    uint8_t                 dma_chan;
    tim_chan_t              tim_chan;
    #endif  /* (RTE_DMA) */
} tim_env_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
#if (RTE_TIM0)
static tim_env_t tim0_env = {
    .isr_cb     = NULL,
    #if (RTE_DMA)
    .dma_chan   = DMA_NUMBER_OF_CHANNELS,
    .tim_chan   = TIM_CHAN_ALL,
    #endif  /* RTE_DMA */
};

static const drv_resource_t tim0_resource = {
    .cap        = CAP_TIM0,
    .reg        = OM_TIM0,
    .env        = &tim0_env,
    .irq_num    = TIM0_IRQn,
    .irq_prio   = RTE_TIM0_IRQ_PRIORITY,
    #if (RTE_DMA)
    .dma_rx = {
        .id     = DMA_ID_TIMER0,
        .prio   = RTE_TIM0_DMA_PRIORITY,
    },
    .dma_tx = {
        .id     = DMA_ID_TIMER0,
        .prio   = RTE_TIM0_DMA_PRIORITY,
    },
    #endif  /* RTE_DMA */
};
#endif

#if (RTE_TIM1)
static tim_env_t tim1_env = {
    .isr_cb     = NULL,
    #if (RTE_DMA)
    .dma_chan   = DMA_NUMBER_OF_CHANNELS,
    .tim_chan   = TIM_CHAN_ALL,
    #endif  /* RTE_DMA */
};

static const drv_resource_t tim1_resource = {
    .cap        = CAP_TIM1,
    .reg        = OM_TIM1,
    .env        = &tim1_env,
    .irq_num    = TIM1_IRQn,
    .irq_prio   = RTE_TIM1_IRQ_PRIORITY,
    #if (RTE_DMA)
    .dma_rx = {
        .id     = DMA_ID_TIMER1,
        .prio   = RTE_TIM1_DMA_PRIORITY,
    },
    .dma_tx = {
        .id     = DMA_ID_TIMER1,
        .prio   = RTE_TIM1_DMA_PRIORITY,
    },
    #endif  /* RTE_DMA */
};
#endif

#if (RTE_TIM2)
static tim_env_t tim2_env = {
    .isr_cb     = NULL,
    #if (RTE_DMA)
    .dma_chan   = DMA_NUMBER_OF_CHANNELS,
    .tim_chan   = TIM_CHAN_ALL,
    #endif  /* RTE_DMA */
};

static const drv_resource_t tim2_resource = {
    .cap        = CAP_TIM2,
    .reg        = OM_TIM2,
    .env        = &tim2_env,
    .irq_num    = TIM2_IRQn,
    .irq_prio   = RTE_TIM2_IRQ_PRIORITY,
    #if (RTE_DMA)
    .dma_rx = {
        .id     = DMA_ID_TIMER2,
        .prio   = RTE_TIM2_DMA_PRIORITY,
    },
    .dma_tx = {
        .id     = DMA_ID_TIMER2,
        .prio   = RTE_TIM2_DMA_PRIORITY,
    },
    #endif  /* RTE_DMA */
};
#endif


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
/**
 * @brief tim get resource
 *
 * @param[in] om_tim      timer instance
 *
 * @return timer resource pointer
 **/
static const drv_resource_t *tim_get_resource(OM_TIM_Type *om_tim)
{
    #if (RTE_TIM0)
    if ((uint32_t)om_tim == (uint32_t)tim0_resource.reg) {
        return &tim0_resource;
    }
    #endif

    #if (RTE_TIM1)
    if ((uint32_t)om_tim == (uint32_t)tim1_resource.reg) {
        return &tim1_resource;
    }
    #endif

    #if (RTE_TIM2)
    if ((uint32_t)om_tim == (uint32_t)tim2_resource.reg) {
        return &tim2_resource;
    }
    #endif

    OM_ASSERT(0);

    return NULL;
}

/**
 * @brief timer calcurate prescaler and auto reload register
 *
 * @param[in] om_tim        timer instance
 * @param[in] period_us     us set
 *
 * @return true: set success, false: set fail.
 **/
static bool tim_calc_psc_arr(OM_TIM_Type *om_tim, uint32_t period_us)
{
    uint32_t clk_mhz;
    uint32_t psc, arr, factor;

    clk_mhz = drv_rcc_clock_get((rcc_clk_t)(uint32_t)om_tim) / 1000000;
    factor  = period_us / (TIM_ARR_MAX + 1);

    psc = clk_mhz * (factor + 1);
    arr = period_us / (factor + 1);

    if (arr > TIM_ARR_MAX || psc > TIM_PSC_MAX) {
        return false;
    }

    om_tim->PSC = psc - 1;
    om_tim->ARR = arr - 1;

    return true;
}

#if (RTE_DMA)
static void drv_tim_dma_event_callback(void *resource, drv_event_t event, void *next_chain)
{
    tim_env_t   *env;
    OM_TIM_Type *om_tim;

    env = (tim_env_t *)((const drv_resource_t *)resource)->env;
    if (env == NULL) {
        return;
    }
    om_tim = (OM_TIM_Type *)((const drv_resource_t *)resource)->reg;

    if (event == DRV_DMA_EVENT_TERMINAL_COUNT_REQUEST) {
        if (env->isr_cb) {
            env->isr_cb(om_tim, DRV_EVENT_TIMER_DMA_COMPLETE, next_chain, NULL);
        }
    } else if (event == DRV_DMA_EVENT_ABORT) {
        return;
    } else {
        OM_ASSERT(0);
    }
}
#endif  /* RTE_DMA */


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 * @brief tim initialization
 *
 * @param[in] om_tim      timer instance
 *
 * @return status, see@om_error_t
 **/
om_error_t drv_tim_init(OM_TIM_Type *om_tim)
{
    const drv_resource_t    *resource;
    tim_env_t               *env;

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }

    DRV_RCC_RESET((rcc_clk_t)(size_t)resource->reg);

    env = (tim_env_t *)resource->env;
    env->isr_cb = NULL;

    #if (RTE_DMA)
    if (resource->cap & CAP_TIM_DMA_MASK) {
        env->tim_chan = TIM_CHAN_ALL;
        env->dma_chan = DMA_NUMBER_OF_CHANNELS;
    }
    #endif  /* RTE_DMA */

    NVIC_ClearPendingIRQ(resource->irq_num);
    NVIC_SetPriority(resource->irq_num, resource->irq_prio);
    NVIC_EnableIRQ(resource->irq_num);

    return OM_ERROR_OK;
}

/**
 * @brief tim uninitialization
 *
 * @param[in] om_tim      timer instance
 *
 * @return status, see@om_error_t
 **/
om_error_t drv_tim_uninit(OM_TIM_Type *om_tim)
{
    const drv_resource_t *resource;

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }

    DRV_RCC_CLOCK_ENABLE((rcc_clk_t)(size_t)resource->reg, 0U);

    NVIC_DisableIRQ(resource->irq_num);

    return OM_ERROR_OK;
}

/**
 * @brief tim register callback
 *
 * @param[in] om_tim      timer instance
 * @param[in] isr_cb      callback to register
 *
 * @return None
 **/
void drv_tim_register_event_callback(OM_TIM_Type *om_tim, drv_isr_callback_t isr_cb)
{
    const drv_resource_t *resource;
    tim_env_t            *env;

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return;
    }
    env = (tim_env_t *)resource->env;

    env->isr_cb = isr_cb;
}

/**
 * @brief General Purpose Timer Start
 *
 * @param[in] om_tim      tim instance
 * @param[in] config      general timer config
 *
 * @return status, see@om_error_t
 **/
om_error_t drv_tim_gp_start(OM_TIM_Type *om_tim, const tim_gp_config_t *cfg)
{
    const drv_resource_t *resource;

    OM_ASSERT(cfg);

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }

    if (!tim_calc_psc_arr(om_tim, cfg->period_us)) {
        return OM_ERROR_OUT_OF_RANGE;
    }

    // use edge-aligned mode, upcounter, ARR register is buffered
    register_set(&om_tim->CR1, MASK_4REG(TIM_CR1_ARPE, 1,
                                         TIM_CR1_OPM,  0,
                                         TIM_CR1_CMS,  0,
                                         TIM_CR1_DIR,  0));
    om_tim->DIER = 0;
    om_tim->CNT  = 0;
    om_tim->SR   = 0;

    // set CCxS input as default (ICx->TIx)
    register_set(&om_tim->CCMR1, MASK_2REG(TIM_CCMR1_CC1S, 1,
                                           TIM_CCMR1_CC2S, 1));
    register_set(&om_tim->CCMR2, MASK_2REG(TIM_CCMR2_CC3S, 1,
                                           TIM_CCMR2_CC4S, 1));
    // enable update interrupt
    om_tim->DIER |= TIM_DIER_UIE_MASK;

    // start
    om_tim->CR1 |= TIM_CR1_CEN_MASK;

    return OM_ERROR_OK;
}

/**
 * @brief General Purpose Timer stop
 *
 * @param[in] om_tim      timer instance
 *
 * @return None
 **/
void drv_tim_gp_stop(OM_TIM_Type *om_tim)
{
    const drv_resource_t *resource;

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return;
    }

    om_tim->CR1 &= ~TIM_CR1_CEN_MASK;
}

/**
 * @brief tim pwm output start
 *
 * @param[in] om_tim      timer instance
 * @param[in] config      tim_pwm_output_config_t
 *
 * @return status see@om_error_t
 **/
om_error_t drv_tim_pwm_output_start(OM_TIM_Type *om_tim, const tim_pwm_output_config_t *cfg)
{
    const drv_resource_t *resource;
    tim_env_t            *env;
    uint32_t              tim_clk, psc;
    uint32_t              ccer = 0;
    uint32_t              trans_size_byte;

    OM_ASSERT(cfg);

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }
    env = (tim_env_t *)resource->env;

    // calculate prescaler val
    tim_clk = drv_rcc_clock_get((rcc_clk_t)(size_t)resource->reg);
    psc     = (tim_clk / cfg->cnt_freq) - 1;
    if (psc > TIM_PSC_MAX) {
        return OM_ERROR_OUT_OF_RANGE;
    }

    // update shadow register
    if (!(om_tim->CR1 & TIM_CR1_CEN_MASK)) {
        om_tim->DIER    = 0;
        om_tim->CR1     = TIM_CR1_ARPE_MASK;
        om_tim->PSC     = psc;
        om_tim->ARR     = cfg->period_cnt - 1;
    }

    // channel config
    for (uint8_t chan_id = TIM_CHAN_1; chan_id < TIM_CHAN_ALL; chan_id++) {
        switch (chan_id) {
            case TIM_CHAN_1:
                if (cfg->chan[chan_id].en) {
                    // pwm mode 1, upcounter, ccrx preload
                    register_set(&om_tim->CCMR1, MASK_3REG(TIM_CCMR1_OC1M,  6,
                                                           TIM_CCMR1_OC1PE, 1,
                                                           TIM_CCMR1_CC1S,  0));
                    // compare output enable,set pol
                    register_set(&ccer, MASK_2REG(TIM_CCER_CC1E, 1,
                                                  TIM_CCER_CC1P, cfg->chan[chan_id].cfg.pol));
                    // set output compare val
                    om_tim->CCR[chan_id] = cfg->chan[chan_id].cfg.oc_val;
                } else {
                    // set CCnS input
                    register_set(&om_tim->CCMR1, MASK_1REG(TIM_CCMR1_CC1S, 1));
                    // disable compare
                    ccer &= ~TIM_CCER_CC1E_MASK;
                }
                break;
            case TIM_CHAN_2:
                if (cfg->chan[chan_id].en) {
                    // pwm mode 1, upcounter, ccrx preload
                    register_set(&om_tim->CCMR1, MASK_3REG(TIM_CCMR1_OC2M,  6,
                                                           TIM_CCMR1_OC2PE, 1,
                                                           TIM_CCMR1_CC2S,  0));
                    // compare output enable,set pol
                    register_set(&ccer, MASK_2REG(TIM_CCER_CC2E, 1,
                                                  TIM_CCER_CC2P, cfg->chan[chan_id].cfg.pol));
                    // set output compare val
                    om_tim->CCR[chan_id] = cfg->chan[chan_id].cfg.oc_val;
                } else {
                    // set CCnS input
                    register_set(&om_tim->CCMR1, MASK_1REG(TIM_CCMR1_CC2S, 1));
                    // disable compare
                    ccer &= ~TIM_CCER_CC2E_MASK;
                }
                break;
            case TIM_CHAN_3:
                if (cfg->chan[chan_id].en) {
                    // pwm mode 1, upcounter, ccrx preload
                    register_set(&om_tim->CCMR2, MASK_3REG(TIM_CCMR2_OC3M,  6,
                                                           TIM_CCMR2_OC3PE, 1,
                                                           TIM_CCMR2_CC3S,  0));
                    // compare output enable,set pol
                    register_set(&ccer, MASK_2REG(TIM_CCER_CC3E, 1,
                                                  TIM_CCER_CC3P, cfg->chan[chan_id].cfg.pol));
                    // set output compare val
                    om_tim->CCR[chan_id] = cfg->chan[chan_id].cfg.oc_val;
                } else {
                    // set CCnS input
                    register_set(&om_tim->CCMR2, MASK_1REG(TIM_CCMR2_CC3S, 1));
                    // disable compare
                    ccer &= ~TIM_CCER_CC3E_MASK;
                }
                break;
            case TIM_CHAN_4:
                if (cfg->chan[chan_id].en) {
                    // pwm mode 1, upcounter, ccrx preload
                    register_set(&om_tim->CCMR2, MASK_3REG(TIM_CCMR2_OC4M,  6,
                                                           TIM_CCMR2_OC4PE, 1,
                                                           TIM_CCMR2_CC4S,  0));
                    // compare output enable,set pol
                    register_set(&ccer, MASK_2REG(TIM_CCER_CC4E, 1,
                                                  TIM_CCER_CC4P, cfg->chan[chan_id].cfg.pol));
                    // set output compare val
                    om_tim->CCR[chan_id] = cfg->chan[chan_id].cfg.oc_val;
                } else {
                    // set CCnS input
                    register_set(&om_tim->CCMR2, MASK_1REG(TIM_CCMR2_CC4S, 1));
                    // disable compare
                    ccer &= ~TIM_CCER_CC4E_MASK;
                }
                break;
            default:
                break;
        }
    }

    om_tim->CCER = ccer;

    // set dma, only one channel use dma
    #if (RTE_DMA)
    if (resource->cap & CAP_TIM_DMA_MASK && cfg->dma_cfg.en) {
        dma_config_t dma_config;

        OM_ASSERT(cfg->dma_cfg.chain);
        OM_ASSERT(cfg->dma_cfg.tim_chan != TIM_CHAN_ALL);

        // dma config param
        dma_config.channel_ctrl = DMA_SET_CTRL(DMA_ADDR_CTRL_INC, DMA_ADDR_CTRL_FIXED,
                                               DMA_TRANS_WIDTH_2B, DMA_TRANS_WIDTH_2B, DMA_BURST_SIZE_1T,
                                               resource->dma_tx.prio ? DMA_PRIORITY_HIGH : DMA_PRIORITY_LOW);
        dma_config.src_id       = DMA_ID_MEM;
        dma_config.dst_id       = (dma_id_t)resource->dma_tx.id;
        dma_config.event_cb     = drv_tim_dma_event_callback;
        dma_config.cb_param     = (void *)resource;
        dma_config.chain_trans  = NULL;
        dma_config.chain_trans_num = 0U;

        trans_size_byte = cfg->dma_cfg.chain[0].size_byte;
        if (cfg->dma_cfg.chain[0].ll_ptr != NULL) {
            dma_config.chain_trans = &cfg->dma_cfg.chain[0];

            uint32_t index = 0;
            do {
                dma_config.chain_trans_num++;
                dma_config.chain_trans[index].dst_addr = (uint32_t)&om_tim->CCR[cfg->dma_cfg.tim_chan];
                if (cfg->dma_cfg.chain[index].ll_ptr == NULL
                        || cfg->dma_cfg.chain[index].ll_ptr == &cfg->dma_cfg.chain[0]) {
                    break;
                }
                index++;
            } while (true);
            dma_config.chain_trans_num--;
            dma_config.chain_trans = &cfg->dma_cfg.chain[1];
        } else {
            dma_config.chain_trans = NULL;
        }

        drv_dma_channel_config(env->dma_chan, &dma_config);

        // need to cfg chain[0] in circular list
        if (dma_config.chain_trans_num) {
            uint8_t src_width = 1 << register_get(&dma_config.channel_ctrl, MASK_POS(DMA_SRCWIDTH));
            cfg->dma_cfg.chain[0].trans_size = cfg->dma_cfg.chain[0].size_byte / src_width;
            cfg->dma_cfg.chain[0].ctrl = cfg->dma_cfg.chain[1].ctrl;
        }

        // trigger dma request enable
        om_tim->DIER |= TIM_DIER_UDE_MASK;

        // disable output compare preload when in dma mode
        switch (cfg->dma_cfg.tim_chan) {
            case TIM_CHAN_1:
                om_tim->CCMR1 &= ~TIM_CCMR1_OC1PE_MASK;
                break;
            case TIM_CHAN_2:
                om_tim->CCMR1 &= ~TIM_CCMR1_OC2PE_MASK;
                break;
            case TIM_CHAN_3:
                om_tim->CCMR2 &= ~TIM_CCMR2_OC3PE_MASK;
                break;
            case TIM_CHAN_4:
                om_tim->CCMR2 &= ~TIM_CCMR2_OC4PE_MASK;
                break;
            default:
                OM_ASSERT(0);
                break;
        }

        // enable dma channel
        drv_dma_channel_enable(env->dma_chan, (uint32_t)&om_tim->CCR[cfg->dma_cfg.tim_chan], (uint32_t)cfg->dma_cfg.chain[0].src_addr, trans_size_byte);
    }
    #endif  /* RTE_DMA */

    if (!(om_tim->CR1 & TIM_CR1_CEN_MASK)) {
        om_tim->SR  = 0;
        om_tim->EGR |= TIM_EGR_UG_MASK;
        while (!(om_tim->SR & TIM_SR_UIF_MASK));

        om_tim->CNT = 0;
        om_tim->CR2 = 0;
        om_tim->SR  = 0;

        // open oc and ocn output
        om_tim->BDTR |= TIM_BDTR_MOE_MASK|cfg->dead_time;
        if (env->isr_cb) {
            om_tim->DIER |= TIM_DIER_UIE_MASK;
        }

        // start
        om_tim->CR1 |= TIM_CR1_CEN_MASK;
    }

    return OM_ERROR_OK;
}

/**
 * @brief tim pwm complementary output start
 *
 * @param[in] om_tim      timer instance
 * @param[in] config      tim_pwm_complementary_output_config_t
 *
 * @return status see@om_error_t
 **/
om_error_t drv_tim_pwm_complementary_output_start(OM_TIM_Type *om_tim, tim_pwm_complementary_output_config_t *cfg)
{
    const drv_resource_t *resource;
    tim_env_t            *env;
    uint32_t              tim_clk, psc;
    uint32_t              ccer = 0;
    uint32_t              trans_size_byte;

    OM_ASSERT(cfg);

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }
    env = (tim_env_t *)resource->env;

    // calculate prescaler val
    tim_clk = drv_rcc_clock_get((rcc_clk_t)(size_t)resource->reg);
    psc     = (tim_clk / cfg->cnt_freq) - 1;
    if (psc > TIM_PSC_MAX) {
        return OM_ERROR_OUT_OF_RANGE;
    }

    // update shadow register
    if (!(om_tim->CR1 & TIM_CR1_CEN_MASK)) {
        om_tim->DIER    = 0;
        om_tim->CR1     = TIM_CR1_ARPE_MASK;
        om_tim->PSC     = psc;
        om_tim->ARR     = cfg->period_cnt - 1;
    }

    // channel config
    for (uint8_t chan_id = TIM_CHAN_1; chan_id < TIM_CHAN_ALL; chan_id++) {
        switch (chan_id) {
            case TIM_CHAN_1:
                if (cfg->chan[chan_id].en) {
                    // pwm mode 1, upcounter, ccrx preload
                    register_set(&om_tim->CCMR1, MASK_3REG(TIM_CCMR1_OC1M,  6,
                                                           TIM_CCMR1_OC1PE, 1,
                                                           TIM_CCMR1_CC1S,  0));

                    register_set(&ccer, MASK_4REG(TIM_CCER_CC1E, 1,TIM_CCER_CC1NE, 1,
                                                  TIM_CCER_CC1P, cfg->chan[chan_id].cfg.pol,TIM_CCER_CC1NP, cfg->chan[chan_id].cfg.pol));
                    // set output compare val
                    om_tim->CCR[chan_id] = cfg->chan[chan_id].cfg.oc_val;
                } else {
                    // set CCnS input
                    register_set(&om_tim->CCMR1, MASK_1REG(TIM_CCMR1_CC1S, 1));
                    // disable compare
                    ccer &= ~TIM_CCER_CC1E_MASK;
                }
                break;
            case TIM_CHAN_2:
                if (cfg->chan[chan_id].en) {
                    // pwm mode 1, upcounter, ccrx preload
                    register_set(&om_tim->CCMR1, MASK_3REG(TIM_CCMR1_OC2M,  6,
                                                           TIM_CCMR1_OC2PE, 1,
                                                           TIM_CCMR1_CC2S,  0));
                    // compare output enable,set pol
                    register_set(&ccer, MASK_4REG(TIM_CCER_CC2E, 1,TIM_CCER_CC2NE, cfg->chan[chan_id].cfg.complementary_output_enable,
                                                  TIM_CCER_CC2P, cfg->chan[chan_id].cfg.pol,TIM_CCER_CC2NP, cfg->chan[chan_id].cfg.pol));
                    // set output compare val
                    om_tim->CCR[chan_id] = cfg->chan[chan_id].cfg.oc_val;
                } else {
                    // set CCnS input
                    register_set(&om_tim->CCMR1, MASK_1REG(TIM_CCMR1_CC2S, 1));
                    // disable compare
                    ccer &= ~TIM_CCER_CC2E_MASK;
                }
                break;
            case TIM_CHAN_3:
                if (cfg->chan[chan_id].en) {
                    // pwm mode 1, upcounter, ccrx preload
                    register_set(&om_tim->CCMR2, MASK_3REG(TIM_CCMR2_OC3M,  6,
                                                           TIM_CCMR2_OC3PE, 1,
                                                           TIM_CCMR2_CC3S,  0));
                    // compare output enable,set pol
                    register_set(&ccer, MASK_4REG(TIM_CCER_CC3E, 1,TIM_CCER_CC3NE, cfg->chan[chan_id].cfg.complementary_output_enable,
                                                  TIM_CCER_CC3P, cfg->chan[chan_id].cfg.pol,TIM_CCER_CC3NP, cfg->chan[chan_id].cfg.pol));
                    // set output compare val
                    om_tim->CCR[chan_id] = cfg->chan[chan_id].cfg.oc_val;
                } else {
                    // set CCnS input
                    register_set(&om_tim->CCMR2, MASK_1REG(TIM_CCMR2_CC3S, 1));
                    // disable compare
                    ccer &= ~TIM_CCER_CC3E_MASK;
                }
                break;
            case TIM_CHAN_4:
                if (cfg->chan[chan_id].en) {
                    // pwm mode 1, upcounter, ccrx preload
                    register_set(&om_tim->CCMR2, MASK_3REG(TIM_CCMR2_OC4M,  6,
                                                           TIM_CCMR2_OC4PE, 1,
                                                           TIM_CCMR2_CC4S,  0));
                    // compare output enable,set pol
                    register_set(&ccer, MASK_2REG(TIM_CCER_CC4E, 1,
                                                  TIM_CCER_CC4P, cfg->chan[chan_id].cfg.pol));
                    // set output compare val
                    om_tim->CCR[chan_id] = cfg->chan[chan_id].cfg.oc_val;
                } else {
                    // set CCnS input
                    register_set(&om_tim->CCMR2, MASK_1REG(TIM_CCMR2_CC4S, 1));
                    // disable compare
                    ccer &= ~TIM_CCER_CC4E_MASK;
                }
                break;
            default:
                break;
        }
    }

    om_tim->CCER = ccer;

    // set dma, only one channel use dma
    #if (RTE_DMA)
    if (resource->cap & CAP_TIM_DMA_MASK && cfg->dma_cfg.en) {
        dma_config_t dma_config;

        OM_ASSERT(cfg->dma_cfg.chain);
        OM_ASSERT(cfg->dma_cfg.tim_chan != TIM_CHAN_ALL);

        // dma config param
        dma_config.channel_ctrl = DMA_SET_CTRL(DMA_ADDR_CTRL_INC, DMA_ADDR_CTRL_FIXED,
                                               DMA_TRANS_WIDTH_2B, DMA_TRANS_WIDTH_2B, DMA_BURST_SIZE_1T,
                                               resource->dma_tx.prio ? DMA_PRIORITY_HIGH : DMA_PRIORITY_LOW);
        dma_config.src_id       = DMA_ID_MEM;
        dma_config.dst_id       = (dma_id_t)resource->dma_tx.id;
        dma_config.event_cb     = drv_tim_dma_event_callback;
        dma_config.cb_param     = (void *)resource;
        dma_config.chain_trans  = NULL;
        dma_config.chain_trans_num = 0U;

        trans_size_byte = cfg->dma_cfg.chain[0].size_byte;
        if (cfg->dma_cfg.chain[0].ll_ptr != NULL) {
            dma_config.chain_trans = &cfg->dma_cfg.chain[0];

            uint32_t index = 0;
            do {
                dma_config.chain_trans_num++;
                dma_config.chain_trans[index].dst_addr = (uint32_t)&om_tim->CCR[cfg->dma_cfg.tim_chan];
                if (cfg->dma_cfg.chain[index].ll_ptr == NULL
                        || cfg->dma_cfg.chain[index].ll_ptr == &cfg->dma_cfg.chain[0]) {
                    break;
                }
                index++;
            } while (true);
            dma_config.chain_trans_num--;
            dma_config.chain_trans = &cfg->dma_cfg.chain[1];
        } else {
            dma_config.chain_trans = NULL;
        }

        drv_dma_channel_config(env->dma_chan, &dma_config);

        // need to cfg chain[0] in circular list
        if (dma_config.chain_trans_num) {
            uint8_t src_width = 1 << register_get(&dma_config.channel_ctrl, MASK_POS(DMA_SRCWIDTH));
            cfg->dma_cfg.chain[0].trans_size = cfg->dma_cfg.chain[0].size_byte / src_width;
            cfg->dma_cfg.chain[0].ctrl = cfg->dma_cfg.chain[1].ctrl;
        }

        // trigger dma request enable
        om_tim->DIER |= TIM_DIER_UDE_MASK;

        // disable output compare preload when in dma mode
        switch (cfg->dma_cfg.tim_chan) {
            case TIM_CHAN_1:
                om_tim->CCMR1 &= ~TIM_CCMR1_OC1PE_MASK;
                break;
            case TIM_CHAN_2:
                om_tim->CCMR1 &= ~TIM_CCMR1_OC2PE_MASK;
                break;
            case TIM_CHAN_3:
                om_tim->CCMR2 &= ~TIM_CCMR2_OC3PE_MASK;
                break;
            case TIM_CHAN_4:
                om_tim->CCMR2 &= ~TIM_CCMR2_OC4PE_MASK;
                break;
            default:
                OM_ASSERT(0);
                break;
        }

        // enable dma channel
        drv_dma_channel_enable(env->dma_chan, (uint32_t)&om_tim->CCR[cfg->dma_cfg.tim_chan], (uint32_t)cfg->dma_cfg.chain[0].src_addr, trans_size_byte);
    }
    #endif  /* RTE_DMA */

    if (!(om_tim->CR1 & TIM_CR1_CEN_MASK)) {
        om_tim->SR  = 0;
        om_tim->EGR |= TIM_EGR_UG_MASK;
        while (!(om_tim->SR & TIM_SR_UIF_MASK));

        om_tim->CNT = 0;
        om_tim->CR2 = 0;
        om_tim->SR  = 0;

        // open oc and ocn output
		om_tim->BDTR &= 0xFF00;
        om_tim->BDTR |= TIM_BDTR_MOE_MASK|cfg->dead_time;
        if (env->isr_cb) {
            om_tim->DIER |= TIM_DIER_UIE_MASK;
        }

        // start
        om_tim->CR1 |= TIM_CR1_CEN_MASK;
    }

    return OM_ERROR_OK;
}

/**
 * @brief pwm output stop
 *
 * @param[in] om_tim      timer instance
 * @param[in] channel     the tiemr channel, if TIMER_CHANNEL_ALL, stop the TIMx.
 *
 * @return None
 **/
void drv_tim_pwm_output_stop(OM_TIM_Type *om_tim, tim_chan_t channel)
{
    const drv_resource_t *resource;
    tim_env_t            *env;

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return;
    }
    env = (tim_env_t *)resource->env;

    if (channel == TIM_CHAN_ALL) {
        om_tim->CCR[0] = 0;
        om_tim->CCR[1] = 0;
        om_tim->CCR[2] = 0;
        om_tim->CCR[3] = 0;
        om_tim->CCER   = 0;
        om_tim->CCMR1  = 0;
        om_tim->CCMR2  = 0;
        #if (RTE_DMA)
        if (resource->cap & CAP_TIM_DMA_MASK && env->tim_chan != TIM_CHAN_ALL) {
            drv_dma_channel_disable(env->dma_chan);
        }
        #endif  /* RTE_DMA */
    } else {
        // disable compare val
        om_tim->CCR[channel] = 0;

        // disable compare output en
        switch (channel) {
            case TIM_CHAN_1:
                om_tim->CCER  &= ~(TIM_CCER_CC1E_MASK | TIM_CCER_CC1P_MASK);
                om_tim->CCMR1 &= ~(TIM_CCMR1_OC1M_MASK | TIM_CCMR1_OC1PE_MASK);
                break;
            case TIM_CHAN_2:
                om_tim->CCER  &= ~(TIM_CCER_CC2E_MASK | TIM_CCER_CC2P_MASK);
                om_tim->CCMR1 &= ~(TIM_CCMR1_OC2M_MASK | TIM_CCMR1_OC2PE_MASK);
                break;
            case TIM_CHAN_3:
                om_tim->CCER  &= ~(TIM_CCER_CC3E_MASK | TIM_CCER_CC3P_MASK);
                om_tim->CCMR2 &= ~(TIM_CCMR2_OC3M_MASK | TIM_CCMR2_OC3PE_MASK);
                break;
            case TIM_CHAN_4:
                om_tim->CCER  &= ~(TIM_CCER_CC4E_MASK | TIM_CCER_CC4P_MASK);
                om_tim->CCMR2 &= ~(TIM_CCMR2_OC4M_MASK | TIM_CCMR2_OC4PE_MASK);
                break;
            default:
                break;
        }
        #if (RTE_DMA)
        if (resource->cap & CAP_TIM_DMA_MASK && env->tim_chan == channel) {
            drv_dma_channel_disable(env->dma_chan);
        }
        #endif  /* RTE_DMA */
    }

    // check disable all channel
    if (!(om_tim->CCER & (TIM_CCER_CC1E_MASK | TIM_CCER_CC2E_MASK | TIM_CCER_CC3E_MASK | TIM_CCER_CC4E_MASK))) {
        // disable counter
        om_tim->CR1 &= ~TIM_CR1_CEN_MASK;
    }
}

/**
 * @brief Force PWM channel output
 *
 * @param[in] om_tim      timer instance
 * @param[in] channel     channel index
 * @param[in] level       force output level
 *
 * @return None
 **/
void drv_tim_pwm_force_output(OM_TIM_Type *om_tim, tim_chan_t channel, tim_force_level_t level)
{
    switch (channel) {
        case TIM_CHAN_1:
            register_set(&om_tim->CCMR1, MASK_1REG(TIM_CCMR1_OC1M, level));
            break;
        case TIM_CHAN_2:
            register_set(&om_tim->CCMR1, MASK_1REG(TIM_CCMR1_OC2M, level));
            break;
        case TIM_CHAN_3:
            register_set(&om_tim->CCMR2, MASK_1REG(TIM_CCMR2_OC3M, level));
            break;
        case TIM_CHAN_4:
            register_set(&om_tim->CCMR2, MASK_1REG(TIM_CCMR2_OC4M, level));
            break;
        default:
            break;
    }
}

/**
 * @brief the timer capture mode start
 *
 * @param[in] om_tim        timer instance, NOTE:only tim0 support capature in 6626
 * @param[in] config        timer_capture_config_t
 *
 * @return status, see@om_error_t
 **/
om_error_t drv_tim_capture_start(OM_TIM_Type *om_tim, const tim_capture_config_t *cfg)
{
    const drv_resource_t *resource;
    tim_env_t            *env;
    uint32_t              tim_clk, psc;
    uint32_t              trans_size_byte;
    uint32_t              ccer = 0;
    om_error_t            error;

    OM_ASSERT(cfg);

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }
    env = (tim_env_t *)resource->env;

    if (!(resource->cap & CAP_TIM_CAPTURE_MASK)) {
        return OM_ERROR_UNSUPPORTED;
    }

    if (om_tim->CR1 & TIM_CR1_CEN_MASK) {
        return OM_ERROR_BUSY;
    }

    tim_clk = drv_rcc_clock_get((rcc_clk_t)(size_t)resource->reg);
    psc     = (tim_clk / cfg->cnt_freq) - 1;
    if (psc > TIM_PSC_MAX) {
        return OM_ERROR_OUT_OF_RANGE;
    }

    om_tim->DIER  = 0;
    om_tim->CR1   = 0;
    om_tim->PSC   = psc;
    om_tim->ARR   = TIM_ARR_MAX;
    om_tim->CNT   = 0;
    om_tim->SR    = 0;

    register_set(&om_tim->CCMR1, MASK_2REG(TIM_CCMR1_CC1S, 1,
                                           TIM_CCMR1_CC2S, 1));
    register_set(&om_tim->CCMR2, MASK_2REG(TIM_CCMR2_CC3S, 1,
                                           TIM_CCMR2_CC4S, 1));

    /* capture enable and polarities setup */
    for (tim_chan_t chan_id = TIM_CHAN_1; chan_id < TIM_CHAN_ALL; chan_id++) {
        switch (chan_id) {
            case TIM_CHAN_1:
                if (cfg->chan[chan_id].en) {
                    ccer |= TIM_CCER_CC1E_MASK;
                    if (cfg->chan[chan_id].pol == TIM_CAPTURE_POLARITY_RISING_EDGE) {
                        ccer &= ~TIM_CCER_CC1P_MASK;
                    } else {
                        ccer |= TIM_CCER_CC1P_MASK;
                    }
                    om_tim->DIER |= TIM_DIER_CC1IE_MASK;
                }
                break;
            case TIM_CHAN_2:
                if (cfg->chan[chan_id].en) {
                    ccer |= TIM_CCER_CC2E_MASK;
                    if (cfg->chan[chan_id].pol == TIM_CAPTURE_POLARITY_RISING_EDGE) {
                        ccer &= ~TIM_CCER_CC2P_MASK;
                    } else {
                        ccer |= TIM_CCER_CC2P_MASK;
                    }
                    om_tim->DIER |= TIM_DIER_CC2IE_MASK;
                }
                break;
            case TIM_CHAN_3:
                if (cfg->chan[chan_id].en) {
                    ccer |= TIM_CCER_CC3E_MASK;
                    if (cfg->chan[chan_id].pol == TIM_CAPTURE_POLARITY_RISING_EDGE) {
                        ccer &= ~TIM_CCER_CC3P_MASK;
                    } else {
                        ccer |= TIM_CCER_CC3P_MASK;
                    }
                    om_tim->DIER |= TIM_DIER_CC3IE_MASK;
                }
                break;
            case TIM_CHAN_4:
                if (cfg->chan[chan_id].en) {
                    ccer |= TIM_CCER_CC4E_MASK;
                    if (cfg->chan[chan_id].pol == TIM_CAPTURE_POLARITY_RISING_EDGE) {
                        ccer &= ~TIM_CCER_CC4P_MASK;
                    } else {
                        ccer |= TIM_CCER_CC4P_MASK;
                    }
                    om_tim->DIER |= TIM_DIER_CC4IE_MASK;
                }
                break;
            default:
                break;
        }
    }

    #if (RTE_DMA)
    if (resource->cap & CAP_TIM_DMA_MASK && cfg->dma_cfg.en) {
        dma_config_t dma_cfg;

        OM_ASSERT(cfg->dma_cfg.chain);
        OM_ASSERT(cfg->dma_cfg.tim_chan != TIM_CHAN_ALL);

        dma_cfg.channel_ctrl    = DMA_SET_CTRL(DMA_ADDR_CTRL_FIXED, DMA_ADDR_CTRL_INC,
                                               DMA_TRANS_WIDTH_2B, DMA_TRANS_WIDTH_2B, DMA_BURST_SIZE_1T,
                                               resource->dma_rx.prio ? DMA_PRIORITY_HIGH : DMA_PRIORITY_LOW);
        dma_cfg.src_id          = (dma_id_t)resource->dma_rx.id;
        dma_cfg.dst_id          = DMA_ID_MEM;
        dma_cfg.event_cb        = drv_tim_dma_event_callback;
        dma_cfg.cb_param        = (void *)resource;
        dma_cfg.chain_trans     = NULL;
        dma_cfg.chain_trans_num = 0U;

        trans_size_byte = cfg->dma_cfg.chain[0].size_byte;
        if (cfg->dma_cfg.chain[0].ll_ptr != NULL) {
            dma_cfg.chain_trans = &cfg->dma_cfg.chain[0];

            uint32_t index = 0;
            do {
                dma_cfg.chain_trans_num++;
                dma_cfg.chain_trans[index].src_addr = (uint32_t)&om_tim->CCR[cfg->dma_cfg.tim_chan];
                if (cfg->dma_cfg.chain[index].ll_ptr == NULL
                        || cfg->dma_cfg.chain[index].ll_ptr == &cfg->dma_cfg.chain[0]) {
                    break;
                }
                index++;
            } while (true);
            dma_cfg.chain_trans_num--;
            dma_cfg.chain_trans = &cfg->dma_cfg.chain[1];
        } else {
            dma_cfg.chain_trans = NULL;
        }

        error = drv_dma_channel_config(env->dma_chan, &dma_cfg);
        if (error != OM_ERROR_OK) {
            return error;
        }

        // need to cfg chain[0] in circular list
        if (dma_cfg.chain_trans_num) {
            uint8_t src_width = 1 << register_get(&dma_cfg.channel_ctrl, MASK_POS(DMA_SRCWIDTH));
            cfg->dma_cfg.chain[0].trans_size = cfg->dma_cfg.chain[0].size_byte / src_width;
            cfg->dma_cfg.chain[0].ctrl = cfg->dma_cfg.chain[1].ctrl;
        }

        switch (cfg->dma_cfg.tim_chan) {
            case TIM_CHAN_1:
                om_tim->DIER |= TIM_DIER_CC1DE_MASK;
                break;
            case TIM_CHAN_2:
                om_tim->DIER |= TIM_DIER_CC2DE_MASK;
                break;
            case TIM_CHAN_3:
                om_tim->DIER |= TIM_DIER_CC3DE_MASK;
                break;
            case TIM_CHAN_4:
                om_tim->DIER |= TIM_DIER_CC4DE_MASK;
                break;
            default:
                break;
        }

        env->tim_chan = cfg->dma_cfg.tim_chan;

        drv_dma_channel_enable(env->dma_chan, (uint32_t)cfg->dma_cfg.chain[0].dst_addr, (uint32_t)&om_tim->CCR[cfg->dma_cfg.tim_chan], trans_size_byte);
    }
    #endif

    om_tim->CR1  |= TIM_CR1_CEN_MASK;
    om_tim->DIER |= TIM_DIER_UIE_MASK;
    om_tim->CCER |= ccer;

    return OM_ERROR_OK;
}

/**
 * @brief timer capture mode stop
 *
 * @param[in] om_tim        timer instance
 * @param[in] channel       the tiemr channel, if TIMER_CHANNEL_ALL, stop the TIMx.
 *
 * @return None
 **/
void drv_tim_capture_stop(OM_TIM_Type *om_tim, tim_chan_t channel)
{
    const drv_resource_t *resource;
    tim_env_t            *env;

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return;
    }
    env = (tim_env_t *)resource->env;

    if (channel == TIM_CHAN_ALL) {
        om_tim->CCR[0] = 0;
        om_tim->CCR[1] = 0;
        om_tim->CCR[2] = 0;
        om_tim->CCR[3] = 0;
        om_tim->CCER   = 0;
        om_tim->CCMR1  = 0;
        om_tim->CCMR2  = 0;

        #if (RTE_DMA)
        if (resource->cap & CAP_TIM_DMA_MASK && env->tim_chan != TIM_CHAN_ALL) {
            drv_dma_channel_disable(env->dma_chan);
        }
        #endif  /* RTE_DMA */
    } else {
        // disable compare val
        om_tim->CCR[channel] = 0;

        // disable compare output en
        switch (channel) {
            case TIM_CHAN_1:
                om_tim->CCER  &= ~(TIM_CCER_CC1E_MASK | TIM_CCER_CC1P_MASK);
                om_tim->CCMR1 &= ~(TIM_CCMR1_OC1M_MASK | TIM_CCMR1_OC1PE_MASK);
                break;
            case TIM_CHAN_2:
                om_tim->CCER  &= ~(TIM_CCER_CC2E_MASK | TIM_CCER_CC2P_MASK);
                om_tim->CCMR1 &= ~(TIM_CCMR1_OC2M_MASK | TIM_CCMR1_OC2PE_MASK);
                break;
            case TIM_CHAN_3:
                om_tim->CCER  &= ~(TIM_CCER_CC3E_MASK | TIM_CCER_CC3P_MASK);
                om_tim->CCMR2 &= ~(TIM_CCMR2_OC3M_MASK | TIM_CCMR2_OC3PE_MASK);
                break;
            case TIM_CHAN_4:
                om_tim->CCER  &= ~(TIM_CCER_CC4E_MASK | TIM_CCER_CC4P_MASK);
                om_tim->CCMR2 &= ~(TIM_CCMR2_OC4M_MASK | TIM_CCMR2_OC4PE_MASK);
                break;
            default:
                break;
        }
        #if (RTE_DMA)
        if (resource->cap & CAP_TIM_DMA_MASK && env->tim_chan == channel) {
            drv_dma_channel_disable(env->dma_chan);
        }
        #endif  /* RTE_DMA */
    }

    // check disable all channel
    if (!(om_tim->CCER & (TIM_CCER_CC1E_MASK | TIM_CCER_CC2E_MASK | TIM_CCER_CC3E_MASK | TIM_CCER_CC4E_MASK))) {
        // disable counter
        om_tim->CR1 &= ~TIM_CR1_CEN_MASK;
    }
}

/**
 * @brief timer pwm input mode start
 *
 * @param[in] om_tim        timer instance
 * @param[in] config        pwm input mode config
 *
 * @return status, see@om_error_t
 **/
om_error_t drv_tim_pwm_input_start(OM_TIM_Type *om_tim, const tim_pwm_input_config_t *cfg)
{
    const drv_resource_t *resource;
    uint32_t tim_clk, psc;

    OM_ASSERT(cfg);

    if (om_tim->CR1 & TIM_CR1_CEN_MASK) {
        return OM_ERROR_BUSY;
    }

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }

    tim_clk = drv_rcc_clock_get((rcc_clk_t)(size_t)resource->reg);
    psc     = (tim_clk / cfg->cnt_freq) - 1;
    if (psc > TIM_PSC_MAX) {
        return OM_ERROR_OUT_OF_RANGE;
    }

    om_tim->DIER = 0;
    om_tim->CR1 |= TIM_CR1_URS_MASK;
    om_tim->PSC  = psc;
    om_tim->ARR  = TIM_ARR_MAX;
    om_tim->CNT  = 0;
    om_tim->SR   = 0;

    // IC1->TI1, IC2->TI1
    register_set(&om_tim->CCMR1, MASK_2REG(TIM_CCMR1_CC1S, 1,
                                           TIM_CCMR1_CC2S, 2));
    register_set(&om_tim->CCMR2, MASK_2REG(TIM_CCMR2_CC3S, 1,
                                           TIM_CCMR2_CC4S, 2));
    // CC1 falling, CC2 rising edge
    register_set(&om_tim->CCER, MASK_2REG(TIM_CCER_CC1P, 1,
                                          TIM_CCER_CC2P, 0));
    // select TRGI use TI1FP1, slave mode->reset mode
    register_set(&om_tim->SMCR, MASK_2REG(TIM_SMCR_TS,  5,
                                          TIM_SMCR_SMS, 4));
    // enable CC1,CC2 irq and update event
    om_tim->DIER |= TIM_DIER_CC1IE_MASK | TIM_DIER_CC2IE_MASK | TIM_DIER_UIE_MASK;
    // start
    om_tim->CR1 |= TIM_CR1_CEN_MASK;
    // enable capture
    om_tim->CCER |= TIM_CCER_CC1E_MASK | TIM_CCER_CC2E_MASK;

    return OM_ERROR_OK;
}

/**
 * @brief timer pwm input mode stop
 *
 * @param[in] om_tim      timer instance
 *
 * @return None
 **/
void drv_tim_pwm_input_stop(OM_TIM_Type *om_tim)
{
    const drv_resource_t *resource;

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return;
    }

    om_tim->CR1     = 0;
    om_tim->DIER    = 0;
    om_tim->SR      = 0;
    om_tim->SMCR    = 0;
    om_tim->CCER    = 0;
    om_tim->CCMR1   = 0;
    om_tim->CCMR2   = 0;

}
/**
 * @brief General Purpose Timer 32bit Start
 *
 * @param[in] om_tim              tim instance
 * @param[in] om_timer_slave      slave tim instance
 * @param[in] cfg                 general 32bit timer config
 *
 * @return status, see@om_error_t
 **/
om_error_t drv_tim_gp_32bit_start(OM_TIM_Type *om_tim, OM_TIM_Type *om_timer_slave, tim_32bits_config_t *cfg)
{
    const drv_resource_t *resource;
    uint32_t              tim_clk, psc;
	uint32_t factor, clock_mhz, arr, slave_arr;

    OM_ASSERT(cfg);

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }
    // calculate prescaler val
    tim_clk = drv_rcc_clock_get((rcc_clk_t)(size_t)resource->reg);
	clock_mhz = tim_clk / TIM_1M;

	/*set timer irq*/
	if (cfg->callback_uif || cfg->callback_cas_uif) {
		om_tim->DIER = 0;

        //drv_tim_init(om_tim);
		if (cfg->callback_uif) {
			om_tim->DIER |= TIM_DIER_UIE_MASK;
		}
		om_tim->DIER |= TIM_DIER_CASIE_MASK;
    }

	om_timer_slave->RCR = cfg->delay_period;
	factor = cfg->time / (TIM_32BITS_MAX + 1);
	psc = clock_mhz * (factor + 1);
	arr = cfg->time / (factor + 1);
    if (psc > TIM_PSC_MAX) {
        return OM_ERROR_OUT_OF_RANGE;
    }

	slave_arr = arr / TIM_ARR_MAX;
	om_tim->ARR = arr - slave_arr * TIM_ARR_MAX - 1;
	om_timer_slave->ARR = slave_arr;
	om_tim->PSC = psc - 1;
	om_timer_slave->PSC = psc - 1;

	om_tim->CR2  = 0x20;/* mode select is 010 update envet as the triger output*/
	om_tim->SMCR = 0x8;/*Enable Cascade function*/
	om_timer_slave->SMCR = 0xBF;/*om_timer_slave Enable Cascade function and tiger select is 011 ITR3 internal triger register3 and slave mode select is 111 extern clock mode */

	if (cfg->counter == DOWN_COUNT) {
		om_tim->CR1 = TIM_CR1_DIR_MASK;
		om_tim->EGR = TIM_EGR_UG_MASK;
		om_tim->SR = 0;

		om_timer_slave->CR1 = TIM_CR1_DIR_MASK;
		om_timer_slave->EGR = TIM_EGR_UG_MASK;
		om_timer_slave->SR = 0;
	}
	// start
	om_tim->CR1 |= TIM_CR1_CEN_MASK;
	om_timer_slave->CR1 |= TIM_CR1_CEN_MASK;
    return OM_ERROR_OK;
}

/**
 * @brief tim pwm 32bit output start
 *
 * @param[in] om_tim              timer instance
 * @param[in] om_timer_slave      slave tim instance
 * @param[in] cfg                 general 32bit timer config
 *
 * @return status see@om_error_t
 **/
om_error_t drv_tim_pwm_32bit_output_start(OM_TIM_Type *om_tim, OM_TIM_Type *om_timer_slave, tim_32bits_config_t *cfg)
{
    const drv_resource_t *resource;
    uint32_t              tim_clk, psc;

	uint32_t clock_mhz, arr, slave_arr, ccr, slave_ccr;

    OM_ASSERT(cfg);

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }

    // calculate prescaler val
    tim_clk = drv_rcc_clock_get((rcc_clk_t)(size_t)resource->reg);
	clock_mhz = tim_clk / TIM_1M;

    // update shadow register
    if (!(om_tim->CR1 & TIM_CR1_CEN_MASK)) {
        om_tim->DIER    = 0;
        om_tim->CR1     = TIM_CR1_ARPE_MASK;
    }

	/*set timer irq*/
	if (cfg->callback_uif || cfg->callback_cas_uif) {
		om_tim->DIER = 0;

        drv_tim_init(om_tim);

		if (cfg->callback_uif) {
			om_tim->DIER |= TIM_DIER_UIE_MASK;
		}

		om_tim->DIER |= TIM_DIER_CASIE_MASK;
    }

	om_timer_slave->RCR = cfg->delay_period;
	psc = clock_mhz;
	arr = (TIM_1M / cfg->freq);
    if (psc > TIM_PSC_MAX) {
        return OM_ERROR_OUT_OF_RANGE;
    }

	for (uint8_t i = 0; i < 4; i++) {
		ccr = (arr * cfg->duty[i]) / 100;
		if (cfg->counter == DOWN_COUNT) {
			if (ccr > 0) {
				ccr -= 1;
			} else {
				return false;	// When downcount can't output duty=0% pwm.
			}
		}
		slave_ccr = ccr / TIM_ARR_MAX;

		om_tim->CCER = 0x1555;
		om_tim->CCMR1 = 0x6060;
		om_tim->CCMR2 = 0x6060;
		om_tim->BDTR = 0x8000;

		om_timer_slave->CCER = 0x1555;
		om_timer_slave->CCMR1 = 0x6060;
		om_timer_slave->CCMR2 = 0x6060;
		om_timer_slave->BDTR = 0x8000;

		om_tim->CCR[i] = ccr - slave_ccr * TIM_ARR_MAX;
		om_timer_slave->CCR[i] = slave_ccr;
	}
	slave_arr = arr / TIM_ARR_MAX;
	om_tim->ARR = arr - slave_arr * TIM_ARR_MAX - 1;
	om_timer_slave->ARR = slave_arr;
	om_tim->PSC = psc - 1;
	om_timer_slave->PSC = psc - 1;

	om_tim->CR2  = 0x20;
	om_tim->SMCR = 0x8;
	om_timer_slave->SMCR = 0xBF;

	/*Enable all channel for test*/
	if (cfg->run_mode == PWM_32BITS) {
		/*Enable all channel output CCXE CCXNE(capture/compare X channel output enable)*/
		om_tim->CCER = 0x1555;
		/*OC2M and OC1M(output compare 1 mode) set to PWM mode1*/
		om_tim->CCMR1 = 0x6060;
		/*OC4M and OC3M(output compare 3 mode) set to PWM mode1*/
		om_tim->CCMR2 = 0x6060;
		/*MON (main output enable)*/
		om_tim->BDTR = 0x8000;

		om_timer_slave->CCER = 0x1555;
		om_timer_slave->CCMR1 = 0x6060;
		om_timer_slave->CCMR2 = 0x6060;
		om_timer_slave->BDTR = 0x8000;
	}

	if (cfg->counter == DOWN_COUNT) {
		om_tim->CR1 = TIM_CR1_DIR_MASK;
		om_tim->EGR = TIM_EGR_UG_MASK;
		om_tim->SR = 0;

		om_timer_slave->CR1 = TIM_CR1_DIR_MASK;
		om_timer_slave->EGR = TIM_EGR_UG_MASK;
		om_timer_slave->SR = 0;
	}
	// start
	om_tim->CR1 |= TIM_CR1_CEN_MASK;
	om_timer_slave->CR1 |= TIM_CR1_CEN_MASK;
    return OM_ERROR_OK;
}

/**
 * @brief timer interrupt service routine
 *
 * @param[in] om_tim      timer instance
 *
 * @return None
 **/
void drv_tim_isr(OM_TIM_Type *om_tim)
{
    const drv_resource_t *resource;
    tim_env_t            *env;
    uint32_t              status;
    uint32_t              event;

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return;
    }
    env = (tim_env_t *)resource->env;

    DRV_IRQ_BEGIN();
    // get status and clear
    status = om_tim->SR;
    om_tim->SR = 0;

    if (status & TIM_SR_UIF_MASK) {
        if (env->isr_cb) {
            env->isr_cb(om_tim, DRV_EVENT_TIMER_UPDATE, NULL, NULL);
        }
    }
    if (status & TIM_SR_CASUIF_MASK) {
        if (env->isr_cb) {
            env->isr_cb(om_tim, DRV_EVENT_TIMER_CASCADE_UPDATE, NULL, NULL);
        }
    }
    if (status & TIM_SR_TIF_MASK || status & (TIM_SR_CC1OF_MASK | TIM_SR_CC2OF_MASK | TIM_SR_CC3OF_MASK | TIM_SR_CC4OF_MASK)) {
        event = (status & (TIM_SR_TIF_MASK | TIM_SR_CC1OF_MASK| TIM_SR_CC2OF_MASK| TIM_SR_CC3OF_MASK| TIM_SR_CC4OF_MASK)) << 15;
        if (env->isr_cb) {
            env->isr_cb(om_tim, (drv_event_t)event, NULL, NULL);
        }
    }

    // CCx capture irq, pwm input must use CC1 and CC2
    if (status & TIM_SR_CC1IF_MASK) {
        if (env->isr_cb) {
            env->isr_cb(om_tim, DRV_EVENT_TIMER_CC1, (void *)om_tim->CCR[0], (void *)1);
        }
    }
    if (status & TIM_SR_CC2IF_MASK) {
        if (env->isr_cb) {
            env->isr_cb(om_tim, DRV_EVENT_TIMER_CC2, (void *)om_tim->CCR[1], (void *)1);
        }
    }
    if (status & TIM_SR_CC3IF_MASK) {
        if (env->isr_cb) {
            env->isr_cb(om_tim, DRV_EVENT_TIMER_CC3, (void *)om_tim->CCR[2], (void *)1);
        }
    }
    if (status & TIM_SR_CC4IF_MASK) {
        if (env->isr_cb) {
            env->isr_cb(om_tim, DRV_EVENT_TIMER_CC4, (void *)om_tim->CCR[3], (void *)1);
        }
    }
    DRV_IRQ_END();
}

#if (RTE_DMA)
om_error_t drv_tim_dma_channel_allocate(OM_TIM_Type *om_tim)
{
    const drv_resource_t  *resource;
    tim_env_t             *env;

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (tim_env_t *)resource->env;

    if (env->dma_chan >= DMA_NUMBER_OF_CHANNELS) {
        env->dma_chan = drv_dma_channel_allocate();
        if (env->dma_chan >= DMA_NUMBER_OF_CHANNELS) {
            return OM_ERROR_RESOURCES;
        }
    }

    return OM_ERROR_OK;
}

om_error_t drv_tim_dma_channel_release(OM_TIM_Type *om_tim)
{
    const drv_resource_t  *resource;
    tim_env_t             *env;

    resource = tim_get_resource(om_tim);
    if (resource == NULL) {
        return OM_ERROR_PARAMETER;
    }
    env = (tim_env_t *)resource->env;

    drv_dma_channel_release(env->dma_chan);
    env->dma_chan   = DMA_NUMBER_OF_CHANNELS;
    env->tim_chan   = TIM_CHAN_ALL;

    return OM_ERROR_OK;
}
#endif

#endif  /* (RTE_TIM0 || RTE_TIM1 || RTE_TIM2) */

/** @} */