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
 * @file     drv_lp_tim.c
 * @brief    source file for lp tim
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
#if (RTE_LP_TIM)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    drv_isr_callback_t  isr_cb;
} lp_tim_env_t;

typedef struct {
    void            *reg;
    void            *env;
    IRQn_Type       irq_num;
    uint8_t         irq_prio;
} lp_tim_resource_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
#if (RTE_LP_TIM)
static lp_tim_env_t lp_tim_env = {
    .isr_cb = NULL,
};

static const lp_tim_resource_t lp_tim_resource = {
    .reg        = OM_LP_TIM,
    .env        = &lp_tim_env,
    .irq_num    = LP_TIMER_IRQn,
    .irq_prio   = RTE_LP_TIM_IRQ_PRIORITY,
};
#endif


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static const lp_tim_resource_t *lp_tim_get_resource(OM_LP_TIM_Type *om_lp_tim)
{
    #if (RTE_LP_TIM)
    if ((uint32_t)om_lp_tim == (uint32_t)lp_tim_resource.reg) {
        return &lp_tim_resource;
    }
    #endif

    OM_ASSERT(0);

    return NULL;
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
#if (RTE_LP_TIM_REGISTER_CALLBACK)
/**
 * @brief lp tim register callback
 *
 * @param[in] om_lp_tim      lp timer instance
 * @param[in] isr_cb         callback
 *
 * @return None
 **/
void drv_lp_tim_register_isr_callback(OM_LP_TIM_Type *om_lp_tim, drv_isr_callback_t isr_cb)
{
    const lp_tim_resource_t *resource;
    lp_tim_env_t            *env;

    resource = lp_tim_get_resource(om_lp_tim);
    if (resource == NULL) {
        return;
    }
    env = (lp_tim_env_t *)resource->env;

    env->isr_cb = isr_cb;
}
#endif

__WEAK void drv_lp_tim_isr_callback(OM_LP_TIM_Type *om_lp_tim, drv_event_t event)
{
    #if (RTE_LP_TIM_REGISTER_CALLBACK)
    const lp_tim_resource_t *resource;
    lp_tim_env_t            *env;

    resource = lp_tim_get_resource(om_lp_tim);
    if (resource == NULL) {
        return;
    }
    env = (lp_tim_env_t *)resource->env;

    if (env->isr_cb) {
        env->isr_cb(om_lp_tim, event, NULL, NULL);
    }
    #endif
}

/**
 * @brief lp tim free-running config
 *
 * @param[in] om_lp_tim     lp timer instance
 * @param[in] cfg           free-running config
 *
 * @return status, see@om_error_t
 **/
om_error_t drv_lp_tim_free_running_init(OM_LP_TIM_Type *om_lp_tim, const lp_tim_free_running_config_t *cfg)
{
    const lp_tim_resource_t *resource;

    resource = lp_tim_get_resource(om_lp_tim);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }

    // open lptimer clk and reset registers
    DRV_RCC_RESET(RCC_CLK_LP_TIM);

    // enable lptimer
    drv_lp_tim_control(om_lp_tim, LP_TIM_CONTROL_ENABLE, (void *)1);

    // config mode, top_en, prescaler
    register_set(&om_lp_tim->CTRL, MASK_3REG(LP_TIM_CTRL_REPMODE,  LP_TIM_MODE_FREE_RUNNING,
                                             LP_TIM_CTRL_CNTTOPEN, cfg->top_en,
                                             LP_TIM_CTRL_CNTPRESC, cfg->presclar));

    // set top val
    drv_lp_tim_control(om_lp_tim, LP_TIM_CONTROL_TOP_SET, (void *)(uint32_t)cfg->top_val);

    // set compare val
    om_lp_tim->COMP0 = cfg->compare_val0;
    om_lp_tim->COMP1 = cfg->compare_val1;

    // enable interrupt
    // om_lp_tim->INTE |= LP_TIM_INTE_COMP0_EN_MASK | LP_TIM_INTE_COMP1_EN_MASK | LP_TIM_INTE_UF_EN_MASK;

    return OM_ERROR_OK;
}

/**
 * @brief lp tim one shot config
 *
 * @param[in] om_lp_tim     lp timer instance
 * @param[in] cfg           one shot config
 *
 * @return status, see@om_error_t
 **/
om_error_t drv_lp_tim_one_shot_init(OM_LP_TIM_Type *om_lp_tim, const lp_tim_one_shot_config_t *cfg)
{
    const lp_tim_resource_t *resource;

    resource = lp_tim_get_resource(om_lp_tim);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }

    // open lptimer clk and reset registers
    DRV_RCC_RESET(RCC_CLK_LP_TIM);

    // enable lptimer
    drv_lp_tim_control(om_lp_tim, LP_TIM_CONTROL_ENABLE, (void *)1);

    // config mode, top_en, prescaler
    register_set(&om_lp_tim->CTRL, MASK_3REG(LP_TIM_CTRL_REPMODE,  LP_TIM_MODE_ONE_SHOT,
                                             LP_TIM_CTRL_CNTTOPEN, cfg->top_en,
                                             LP_TIM_CTRL_CNTPRESC, cfg->presclar));

    // set top val
    drv_lp_tim_control(om_lp_tim, LP_TIM_CONTROL_TOP_SET, (void *)(uint32_t)cfg->top_val);
    // set rep0 val
    drv_lp_tim_control(om_lp_tim, LP_TIM_CONTROL_REP0_SET, (void *)(uint32_t)cfg->rep0_val);

    // set compare val
    om_lp_tim->COMP0 = cfg->compare_val0;
    om_lp_tim->COMP1 = cfg->compare_val1;

    // enable interrupt
    // om_lp_tim->INTE |= LP_TIM_INTE_COMP0_EN_MASK | LP_TIM_INTE_COMP1_EN_MASK |
    //                    LP_TIM_INTE_UF_EN_MASK | LP_TIM_INTE_REP0_EN_MASK;

    return OM_ERROR_OK;
}

/**
 * @brief lp tim buffered config
 *
 * @param[in] om_lp_tim     lp timer instance
 * @param[in] cfg           buffered config
 *
 * @return status, see@om_error_t
 **/
om_error_t drv_lp_tim_buffered_init(OM_LP_TIM_Type *om_lp_tim, const lp_tim_buffered_config_t *cfg)
{
    const lp_tim_resource_t *resource;

    resource = lp_tim_get_resource(om_lp_tim);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }

    // open lptimer clk and reset registers
    DRV_RCC_RESET(RCC_CLK_LP_TIM);

    // enable lptimer
    drv_lp_tim_control(om_lp_tim, LP_TIM_CONTROL_ENABLE, (void *)1);

    // config mode, top_en, prescaler
    register_set(&om_lp_tim->CTRL, MASK_4REG(LP_TIM_CTRL_REPMODE,  LP_TIM_MODE_BUFFERED,
                                             LP_TIM_CTRL_CNTTOPEN, cfg->top_en,
                                             LP_TIM_CTRL_BUFTOP,   cfg->buftop_en,
                                             LP_TIM_CTRL_CNTPRESC, cfg->presclar));
    // set top val
    drv_lp_tim_control(om_lp_tim, LP_TIM_CONTROL_TOP_SET, (void *)(uint32_t)cfg->top_val);
    // set topbuff
    om_lp_tim->TOPBUFF = cfg->buftop_val;
    // set rep0,rep1 val
    drv_lp_tim_control(om_lp_tim, LP_TIM_CONTROL_REP0_SET, (void *)(uint32_t)cfg->rep0_val);
    drv_lp_tim_control(om_lp_tim, LP_TIM_CONTROL_REP1_SET, (void *)(uint32_t)cfg->rep1_val);

    // set compare val
    om_lp_tim->COMP0 = cfg->compare_val0;
    om_lp_tim->COMP1 = cfg->compare_val1;

    // enable interrupt
    // om_lp_tim->INTE |= LP_TIM_INTE_COMP0_EN_MASK | LP_TIM_INTE_COMP1_EN_MASK |
    //                    LP_TIM_INTE_UF_EN_MASK | LP_TIM_INTE_REP0_EN_MASK;

    return OM_ERROR_OK;
}

/**
 * @brief lp tim double config
 * NOTE: digital bug:if clear rep0/rep1's interrup flag, the out0/1 will continue
 * output, now just support output the max times of repetition
 *
 * @param[in] om_lp_tim     lp timer instance
 * @param[in] cfg           double config
 *
 * @return status, see@om_error_t
 **/
om_error_t drv_lp_tim_double_init(OM_LP_TIM_Type *om_lp_tim, const lp_tim_double_config_t *cfg)
{
    const lp_tim_resource_t *resource;

    resource = lp_tim_get_resource(om_lp_tim);
    if (resource == NULL) {
        return OM_ERROR_RESOURCES;
    }

    // open lptimer clk and reset registers
    DRV_RCC_RESET(RCC_CLK_LP_TIM);

    // enable lptimer
    drv_lp_tim_control(om_lp_tim, LP_TIM_CONTROL_ENABLE, (void *)1);

    // config mode, top_en, prescaler
    register_set(&om_lp_tim->CTRL, MASK_3REG(LP_TIM_CTRL_REPMODE,  LP_TIM_MODE_DOUBLE,
                                             LP_TIM_CTRL_CNTTOPEN, cfg->top_en,
                                             LP_TIM_CTRL_CNTPRESC, cfg->presclar));
    // set top val
    drv_lp_tim_control(om_lp_tim, LP_TIM_CONTROL_TOP_SET, (void *)(uint32_t)cfg->top_val);
    // set rep0,rep1 val
    drv_lp_tim_control(om_lp_tim, LP_TIM_CONTROL_REP0_SET, (void *)(uint32_t)cfg->rep0_val);
    drv_lp_tim_control(om_lp_tim, LP_TIM_CONTROL_REP1_SET, (void *)(uint32_t)cfg->rep1_val);

    // set compare val
    om_lp_tim->COMP0 = cfg->compare_val0;
    om_lp_tim->COMP1 = cfg->compare_val1;

    // enable interrupt
    // om_lp_tim->INTE |= LP_TIM_INTE_COMP0_EN_MASK | LP_TIM_INTE_COMP1_EN_MASK |
    //                    LP_TIM_INTE_UF_EN_MASK | LP_TIM_INTE_REP0_EN_MASK | LP_TIM_INTE_REP1_EN_MASK;


    return OM_ERROR_OK;
}

/**
 * @brief lp tim out config
 *
 * @param[in] om_lp_tim     lp timer instance
 * @param[in] chan_outx     output channel(0,1)
 * @param[in] cfg           out config
 *
 * @return status, see@om_error_t
 **/
om_error_t drv_lp_tim_outx_config(OM_LP_TIM_Type *om_lp_tim, lp_tim_chan_out_t chan_outx, const lp_tim_out_config_t *cfg)
{
    if (chan_outx == LP_TIM_CHAN_OUT0) {
        register_set(&om_lp_tim->CTRL, MASK_2REG(LP_TIM_CTRL_OPOL0, cfg->pol,
                                                 LP_TIM_CTRL_UFOA0, cfg->action));
    } else if (chan_outx == LP_TIM_CHAN_OUT1) {
        register_set(&om_lp_tim->CTRL, MASK_2REG(LP_TIM_CTRL_OPOL1, cfg->pol,
                                                 LP_TIM_CTRL_UFOA1, cfg->action));
    } else {
        return OM_ERROR_PARAMETER;
    }

    return OM_ERROR_OK;
}

/**
 * @brief lp tim control
 *
 * @param[in] om_lp_tim     lp timer instance
 * @param[in] ctrl          control command
 * @param[in] argu          argument
 *
 * @return None
 **/
void *drv_lp_tim_control(OM_LP_TIM_Type *om_lp_tim, lp_tim_control_t ctrl, void *argu)
{
    uint32_t ret;
    const lp_tim_resource_t *resource;

    resource = lp_tim_get_resource(om_lp_tim);
    if (resource == NULL) {
        return (void *)(uint32_t)OM_ERROR_RESOURCES;
    }

    ret = (uint32_t)OM_ERROR_OK;

    OM_CRITICAL_BEGIN();
    switch (ctrl) {
        case  LP_TIM_CONTROL_ENABLE:
            if (argu) {
                om_lp_tim->EN |= LP_TIM_EN_MASK;
            } else {
                om_lp_tim->EN &= ~LP_TIM_EN_MASK;
            }
            break;
        case LP_TIM_CONTROL_START:
			while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_START_MASK);
            om_lp_tim->CMD |= LP_TIM_CMD_START_MASK;
            while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_START_MASK);
            break;
        case LP_TIM_CONTROL_STOP:
		    if(om_lp_tim->STAT & LP_TIM_STAT_RUNNING_MASK){
                om_lp_tim->CMD |= LP_TIM_CMD_STOP_MASK;
		        while (om_lp_tim->STAT & LP_TIM_STAT_RUNNING_MASK);
			}
            break;
        case LP_TIM_CONTROL_CLEAR:
			while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_CLEAR_MASK);
			om_lp_tim->CMD |= (LP_TIM_CMD_CLEAR_MASK|LP_TIM_CMD_START_MASK);
            while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_CLEAR_MASK);
            break;
        case LP_TIM_CONTROL_CTO0:
			while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_CTO0_MASK);
            om_lp_tim->CMD |= (LP_TIM_CMD_CTO0_MASK|LP_TIM_CMD_START_MASK);
            while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_CTO0_MASK);
            break;
        case LP_TIM_CONTROL_CTO1:
			while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_CTO1_MASK);
            om_lp_tim->CMD |= (LP_TIM_CMD_CTO1_MASK|LP_TIM_CMD_START_MASK);
            while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_CTO1_MASK);
            break;
        case LP_TIM_CONTROL_CNT_SET:
            while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_CNT_BUSY_MASK);
            om_lp_tim->CNT = (uint32_t)argu;
            while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_CNT_BUSY_MASK);
            break;
        case LP_TIM_CONTROL_TOP_SET:
            while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_TOP_BUSY_MASK);
            om_lp_tim->TOP = (uint32_t)argu;
            while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_TOP_BUSY_MASK);
            break;
        case LP_TIM_CONTROL_REP0_SET:
            while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_REP0_BUSY_MASK);
            om_lp_tim->REP0 = (uint32_t)argu;
            while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_REP0_BUSY_MASK);
            break;
        case LP_TIM_CONTROL_REP1_SET:
            while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_REP1_BUSY_MASK);
            om_lp_tim->REP1 = (uint32_t)argu;
            while (om_lp_tim->SYNCBUSY & LP_TIM_SYNCBUSY_REP1_BUSY_MASK);
            break;
        case LP_TIM_CONTROL_POWER_IN_SLEEP:
            if (argu) {
                OM_PMU->PSO_PM |= PMU_PSO_PM_LPTIM_POWER_ON_MASK;
            } else {
                OM_PMU->PSO_PM &= ~PMU_PSO_PM_LPTIM_POWER_ON_MASK;
            }
            break;
        case LP_TIM_CONTROL_INT_EN:
            if (argu) {
                NVIC_ClearPendingIRQ(resource->irq_num);
                NVIC_SetPriority(resource->irq_num, resource->irq_prio);
                NVIC_EnableIRQ(resource->irq_num);
            } else {
                NVIC_DisableIRQ(resource->irq_num);
                NVIC_ClearPendingIRQ(resource->irq_num);
            }
            om_lp_tim->INTE = (uint32_t)argu;
            break;
        default:
            break;
    }
    OM_CRITICAL_END();

    return (void *)ret;
}

/**
 * @brief lp tim interrupt service routine
 *
 * @param[in] om_lp_tim     lp timer instance
 *
 * @return None
 **/
void drv_lp_tim_isr(OM_LP_TIM_Type *om_lp_tim)
{
    const lp_tim_resource_t *resource;
    uint32_t                 status;
    uint32_t                 event;
    lp_tim_rep_mode_t        mode;

    resource = lp_tim_get_resource(om_lp_tim);
    if (resource == NULL) {
        return;
    }

    DRV_IRQ_BEGIN();
    // get status and clear
    status = om_lp_tim->INTF & LP_TIM_INTF_ALL_MASK;
    mode = register_get(&om_lp_tim->CTRL, MASK_POS(LP_TIM_CTRL_REPMODE));

    if (mode == LP_TIM_MODE_DOUBLE) {
        if ((status & LP_TIM_INTF_REP0_FLG_MASK) != 0 && (status & LP_TIM_INTF_REP1_FLG_MASK) != 0) {
            om_lp_tim->INTF = status;
        } else {
            om_lp_tim->INTF = status & ~(LP_TIM_INTF_REP0_FLG_MASK | LP_TIM_INTF_REP1_FLG_MASK);
        }
    } else {
        om_lp_tim->INTF = status;
    }

    if (status) {
        event = status << 16;
        drv_lp_tim_isr_callback(om_lp_tim, (drv_event_t)event);
    }
    DRV_IRQ_END();
}


#endif  /* (RTE_LP_TIM) */

/** @} */