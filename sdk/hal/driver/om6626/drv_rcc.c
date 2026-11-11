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
 * @file     drv_rcc.h
 * @brief    reset and clock configuration
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
#if (RTE_RCC)
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
    rcc_clk_t       rcc_clk;
    __IO uint32_t   *rcc_reg;
} rcc_reg_table_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
uint32_t SystemCoreClock = 32 * 1000 * 1000;

static const rcc_reg_table_t rcc_reg_table[] = {
    {RCC_CLK_CPU,           &OM_CPM->CPU_CFG},
    {RCC_CLK_USART0,        &OM_CPM->UART0_CFG},
    {RCC_CLK_USART1,        &OM_CPM->UART1_CFG},
    {RCC_CLK_SF,            &OM_CPM->SF_CFG},
};


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static __IO uint32_t *rcc_clk_search(rcc_clk_t rcc_clk)
{
    if ((rcc_clk == RCC_CLK_DMA) || (rcc_clk == RCC_CLK_GPIO0) || (rcc_clk == RCC_CLK_TIM0) ||
        (rcc_clk == RCC_CLK_TIM1) || (rcc_clk == RCC_CLK_TIM2) || (rcc_clk == RCC_CLK_SPI0) ||
        (rcc_clk == RCC_CLK_SPI1)) {
        rcc_clk = RCC_CLK_CPU;
    }

    for (uint32_t i = 0; i < (sizeof(rcc_reg_table) / sizeof(rcc_reg_table[0])); i++) {
        if (rcc_reg_table[i].rcc_clk == rcc_clk) {
            return rcc_reg_table[i].rcc_reg;
        }
    }

    return NULL;
}

static om_error_t rcc_clk_usart_div_set(__IO uint32_t *rcc_reg, uint32_t freq)
{
    uint32_t div_x256;
    uint32_t int_div, frc_div;

    div_x256 = (uint32_t)((((uint64_t)drv_rcc_clock_get(RCC_CLK_MAIN)) << 8) / freq);
    int_div = div_x256 >> 8U;
    if (int_div > (CPM_UARTx_CFG_UARTx_DIV_COEFF_INT_MASK >> CPM_UARTx_CFG_UARTx_DIV_COEFF_INT_POS)) {
        return OM_ERROR_OUT_OF_RANGE;
    }
    frc_div = div_x256 & 0xFFU;

    if (int_div < 2) {
        register_set(rcc_reg, MASK_2REG(CPM_UARTx_CFG_UARTx_DIV_SEL,       0,
                                        CPM_UARTx_CFG_UARTx_DIV_EN,        1));
    } else {
        register_set(rcc_reg, MASK_4REG(CPM_UARTx_CFG_UARTx_DIV_COEFF_INT, int_div,
                                        CPM_UARTx_CFG_UARTx_DIV_COEFF_FRC, frc_div,
                                        CPM_UARTx_CFG_UARTx_DIV_SEL,       1,
                                        CPM_UARTx_CFG_UARTx_DIV_EN,        1));
    }

    return OM_ERROR_OK;
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
void drv_rcc_init(void)
{
    SystemCoreClock = drv_rcc_clock_get(RCC_CLK_CPU);
}


uint32_t drv_rcc_clock_get(rcc_clk_t rcc_clk)
{
    uint32_t div;

    switch (rcc_clk) {
        case RCC_CLK_I2C0:
        case RCC_CLK_MAIN:
            if (OM_PMU->MISC_CTRL & PMU_MISC_CTRL_MAIN_CLK_SEL_MASK) {
                if (OM_PMU->XTAL32M_CNS0 & PMU_XTAL32M_CNS0_SEL_CPUCLK_MASK) {
                    return 64 * 1000 * 1000;  // use rc32m_x2 or xtal32m_x2
                } else {
                    return 32 * 1000 * 1000; // use xtal32m
                }
            } else {
                return 32 * 1000 * 1000; // use rc32m
            }
            break;  /*lint !e527 */
        case RCC_CLK_RTC:
            return 32768U;
            break;
        default:
            break;
    }

    __IO uint32_t *rcc_reg = rcc_clk_search(rcc_clk);
    if (!rcc_reg) {
        OM_ASSERT(0);
        return 0U;
    }

    switch (rcc_clk) {
        case RCC_CLK_DMA:
        case RCC_CLK_GPIO0:
        case RCC_CLK_TIM0:
        case RCC_CLK_TIM1:
        case RCC_CLK_TIM2:
        case RCC_CLK_SPI0:
        case RCC_CLK_SPI1:
        case RCC_CLK_CPU:
            if (*rcc_reg & CPM_CPU_CFG_CPU_DIV_SEL_MASK) {
                div = REGR(rcc_reg, MASK_POS(CPM_CPU_CFG_CPU_DIV_COEFF));
            } else {
                div = 1;
            }
            return drv_rcc_clock_get(RCC_CLK_MAIN) / div;
            break;  /*lint !e527 */

        case RCC_CLK_SF:
            if (*rcc_reg & CPM_SF_CFG_SF_GATE_EN_MASK) {                   // is gated
                return 0U;
            } else if (!(*rcc_reg & CPM_SF_CFG_SF_DIV_SEL_MASK)) {         // not select divider
                return drv_rcc_clock_get(RCC_CLK_MAIN);
            } else if (*rcc_reg & CPM_SF_CFG_SF_DIV_EN_MASK) {             // select divider and enable
                div = register_get(rcc_reg, MASK_POS(CPM_SF_CFG_SF_DIV_COEFF));
                return drv_rcc_clock_get(RCC_CLK_MAIN) / div;
            } else {                                                       // select divider but not enable
                return 0U;
            }
            break;  /*lint !e527 */

        default:
            break;
    }

    return 0U;
}


om_error_t drv_rcc_clock_set(rcc_clk_t rcc_clk, uint32_t freq)
{
    om_error_t error = OM_ERROR_OK;
    uint32_t div;
    uint32_t main_clk;

    main_clk =  drv_rcc_clock_get(RCC_CLK_MAIN);
    OM_ASSERT(freq != 0U);
    OM_ASSERT(freq <= main_clk);

    if(freq <= 64) {
        div = freq;
    } else {
        div = main_clk / freq;
    }

    __IO uint32_t *rcc_reg = rcc_clk_search(rcc_clk);
    switch (rcc_clk) {
        case RCC_CLK_CPU:
            if (div < 2) {
                register_set(&OM_CPM->CPU_CFG, MASK_2REG(CPM_CPU_CFG_CPU_DIV_SEL, 0, CPM_CPU_CFG_CPU_DIV_EN, 1));
                SystemCoreClock = main_clk;
            } else {
                register_set(&OM_CPM->CPU_CFG, MASK_3REG(CPM_CPU_CFG_CPU_DIV_COEFF, div, CPM_CPU_CFG_CPU_DIV_SEL, 1,
                                                         CPM_CPU_CFG_CPU_DIV_EN, 1));
                SystemCoreClock = main_clk / ((OM_CPM->CPU_CFG & CPM_CPU_CFG_CPU_DIV_COEFF_MASK) >> CPM_CPU_CFG_CPU_DIV_COEFF_POS);
            }
            while(!(OM_CPM->STATUS_READ & CPM_STATUS_READ_CPU_CLK_SYNC_DONE_MASK));
            break;
        case RCC_CLK_SF:
            if (div < 2) {
                register_set(&OM_CPM->SF_CFG, MASK_2REG(CPM_SF_CFG_SF_DIV_SEL, 0, CPM_SF_CFG_SF_DIV_EN, 1));
            } else {
                register_set(&OM_CPM->SF_CFG, MASK_3REG(CPM_SF_CFG_SF_DIV_COEFF, div, CPM_SF_CFG_SF_DIV_SEL, 1,
                                                        CPM_SF_CFG_SF_DIV_EN, 1));
            }
            while(!(OM_CPM->STATUS_READ & CPM_STATUS_READ_SF_CLK_SYNC_DONE_MASK));
            break;
        case RCC_CLK_USART0:
            error = rcc_clk_usart_div_set(rcc_reg, freq);
            if (error == OM_ERROR_OK) {
                while(!(OM_CPM->STATUS_READ & CPM_STATUS_READ_UART0_CLK_SYNC_DONE_MASK));
            }
            break;
        case RCC_CLK_USART1:
            error = rcc_clk_usart_div_set(rcc_reg, freq);
            if (error == OM_ERROR_OK) {
                while(!(OM_CPM->STATUS_READ & CPM_STATUS_READ_UART1_CLK_SYNC_DONE_MASK));
            }
            break;
        default:
            break;
    }

    return error;
}

#endif /* RTE_RCC */

/** @} */
