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
 * @file     drv_usart_ex.c
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
#if (RTE_USART0)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */
typedef struct {
    drv_isr_callback_t isr_cb;
} drv_usart_ex_env_t;

typedef struct {
    void            *reg;
    void            *env;
    IRQn_Type       irq_num;
    uint8_t         irq_prio;
} drv_usart_ex_resource_t;


/*******************************************************************************
 * TYPEDEFS
 */
#if (RTE_USART0)
static drv_usart_ex_env_t usart0_env = {
    .isr_cb        = NULL,
};

static const drv_usart_ex_resource_t usart0_resource = {
    .reg      = OM_USART0,
    .env      = (void *)&usart0_env,
    .irq_num  = UART0_IRQn,
    .irq_prio = RTE_USART0_IRQ_PRIORITY,
};
#endif


/*******************************************************************************
 * CONST & VARIABLES
 */


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static const drv_usart_ex_resource_t *usart_ex_get_resource(OM_USART_EX_Type *om_usart)
{
    #if (RTE_USART0)
    if ((uint32_t)om_usart == (uint32_t)(usart0_resource.reg)) {
        return &usart0_resource;
    }
    #endif /* RTE_USART0 */

    return NULL;
}

static void usart_ex_config_mode_baudrate(OM_USART_EX_Type *om_usart, usart_ex_mode_t mode, uint32_t baudrate)
{
    uint16_t   baud_divisor;

    // config mode
    register_set(&om_usart->CON, MASK_1REG(UART_EX_CON_MODE, mode));

    // config baudrate
    switch (mode) {
        case USART_EX_MODE1:
        case USART_EX_MODE3:
            if (drv_rcc_clock_get(RCC_CLK_MAIN) >= 64000000 && baudrate <= 19200) {
                baud_divisor = 4;
            } else {
                baud_divisor = 1;
            }

            om_usart->RELL = baud_divisor & 0xff;
            om_usart->RELH = (baud_divisor >> 8) & 0xff;

            drv_rcc_clock_set((rcc_clk_t)(uint32_t)om_usart, baud_divisor * baudrate * 16);
            break;
        case USART_EX_MODE2:
            drv_rcc_clock_set((rcc_clk_t)(uint32_t)om_usart, baudrate * 32);
            break;
        default:
            OM_ASSERT(0);
            break;
    }
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief config usart
 *
 * @param[in] om_usart: pointer to usart
 * @param[in] usart_cfg: config
 *
 * @return
 *    - OM_ERROR_OK: config success
 *    - others: config failed
 *******************************************************************************
 */
om_error_t drv_usart_ex_init(OM_USART_EX_Type *om_usart, const usart_ex_config_t *usart_cfg)
{
    const drv_usart_ex_resource_t *resource;

    OM_ASSERT(usart_cfg);

    resource = usart_ex_get_resource(om_usart);
    if(resource == NULL){
        return OM_ERROR_PARAMETER;
    }

    DRV_RCC_RESET((rcc_clk_t)(uint32_t)om_usart);

    usart_ex_config_mode_baudrate(om_usart, USART_EX_MODE1, usart_cfg->baudrate);

    drv_usart_ex_control(om_usart, USART_EX_CTRL_MULT_PROCESSOR_EN, NULL, NULL);

    register_set(&om_usart->CON, MASK_2REG(UART_EX_CON_SRX_EN,    1,
                                           UART_EX_CON_RX_INT_EN, 1));

    NVIC_ClearPendingIRQ(resource->irq_num);
	NVIC_SetPriority(resource->irq_num, resource->irq_prio);
	NVIC_EnableIRQ(resource->irq_num);

    return OM_ERROR_OK;
}

#if (RTE_USART_REGISTER_CALLBACK)
/**
 *******************************************************************************
 * @brief register callback function.
 *
 * @param[in] om_usart: pointer to usart
 * @param[in] cb: callback function
 *
 *******************************************************************************
 */
void drv_usart_ex_register_isr_callback(OM_USART_EX_Type *om_usart, drv_isr_callback_t cb)
{
    const drv_usart_ex_resource_t *resource;
    drv_usart_ex_env_t            *env;

    resource = usart_ex_get_resource(om_usart);
    if(resource == NULL) {
        return;
    }
    env = (drv_usart_ex_env_t *)(resource->env);

    env->isr_cb = cb;
}
#endif

__WEAK void drv_usart_ex_isr_callback(OM_USART_EX_Type *om_usart, drv_event_t event, uint8_t *data, uint16_t num)
{
    #if (RTE_USART_REGISTER_CALLBACK)
    const drv_usart_ex_resource_t *resource;
    drv_usart_ex_env_t            *env;

    resource = usart_ex_get_resource(om_usart);
    if(resource == NULL) {
        return;
    }
    env = (drv_usart_ex_env_t *)(resource->env);

    if (env->isr_cb != NULL) {
        env->isr_cb(om_usart, event, data, (void *)(uint32_t)num);
    }
    #endif
}

/**
 *******************************************************************************
 * @brief Transmit number of bytes from USART by block mode. NOTE: cannot send
 *        and receive at the same time
 *
 * @param[in] om_usart: pointer to usart
 * @param[in] data: pointer to data buffer
 * @param[in] num: num of data to be sent
 *
 * @return
 *    - OM_ERROR_OK:send successfully
 *    - others: send failed
 *******************************************************************************
 */
om_error_t drv_usart_ex_write(OM_USART_EX_Type *om_usart, const uint8_t *data, uint16_t num, uint32_t timeout_ms)
{
    const drv_usart_ex_resource_t  *resource;
    uint16_t tx_cnt;
    uint16_t tx_num;
    om_error_t error;

    OM_ASSERT(num != 0U);
    resource = usart_ex_get_resource(om_usart);
    if(resource == NULL) {
        return OM_ERROR_PARAMETER;
    }

    tx_num = num;
    tx_cnt = 0U;

    error = OM_ERROR_OK;
    while (tx_cnt < tx_num) {
        om_usart->THR = data[tx_cnt];
        DRV_WAIT_MS_UNTIL_TO(!(om_usart->CON & UART_EX_CON_TX_COMPLETED_MASK), timeout_ms, error);
        om_usart->CON &= ~UART_EX_CON_TX_COMPLETED_MASK;
        tx_cnt++;
    }

    return error;
}

/**
 *******************************************************************************
 * @brief control usart
 *
 * @param[in] om_usart: pointer to usart
 * @param[in] control: ctrl cmd
 * @param[in] argu: ctrl param
 *
 * @return : control status
 *******************************************************************************
 */
void *drv_usart_ex_control(OM_USART_EX_Type *om_usart, usart_ex_control_t control, void *argu0, void *argu1)
{
    const drv_usart_ex_resource_t *resource;
    uint32_t              ret;

    ret = (uint32_t )OM_ERROR_OK;
    resource = usart_ex_get_resource(om_usart);
    if (resource == NULL) {
        return (void *)OM_ERROR_PARAMETER;
    }

    OM_CRITICAL_BEGIN();
    switch (control) {
        case USART_EX_CTRL_TB8_LEVEL_SET:
            if (argu0) {
                om_usart->CON |= UART_EX_CON_TB8_MASK;
            } else {
                om_usart->CON &= ~UART_EX_CON_TB8_MASK;
            }
            break;
        case USART_EX_CTRL_RB8_LEVEL_GET:
            ret = om_usart->CON & UART_EX_CON_RB8_MASK ? 1U : 0U;
            break;
        case USART_EX_CTRL_MODE_BAUDRATE_SET:
            usart_ex_config_mode_baudrate(om_usart, (usart_ex_mode_t)(uint32_t)argu0, (uint32_t)argu1);
            break;
        case USART_EX_CTRL_MULT_PROCESSOR_EN:
            om_usart->CON |= UART_EX_CON_MC_EN_MASK;
            break;
        case USART_EX_CTRL_MULT_PROCESSOR_DIS:
            om_usart->CON &= ~UART_EX_CON_MC_EN_MASK;
            break;
        case USART_EX_CTRL_TIM_RELAOD_SET:
            om_usart->RELL = (uint32_t)argu0 & 0xff;
            om_usart->RELH = ((uint32_t)argu0 >> 8) & 0xff;
            break;
    }
    OM_CRITICAL_END();

    return (void *)ret;
}

/**
 *******************************************************************************
 * @brief usart_ex interrupt service routine
 *
 * @param[in] om_usart: pointer to usart
 *
 *******************************************************************************
 */
void drv_usart_ex_isr(OM_USART_EX_Type *om_usart)
{
    uint8_t ch;
    const drv_usart_ex_resource_t  *resource;

    resource = usart_ex_get_resource(om_usart);
    if(resource == NULL) {
        return;
    }

    if (om_usart->CON & UART_EX_CON_RX_COMPLETED_MASK) {
        om_usart->CON &= ~UART_EX_CON_RX_COMPLETED_MASK;

        ch = om_usart->RBR;
        drv_usart_ex_isr_callback(om_usart, DRV_EVENT_COMMON_READ_COMPLETED, &ch, 1);
    }
}


#endif /* (RTE_USART0) */

/** @} */