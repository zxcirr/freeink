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
 * @brief    Header file of RCC HAL module
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup RCC RCC
 * @ingroup  DRIVER
 * @brief    RCC Driver for om662x
 * @details  RCC Driver for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __RCC_OM662X_H
#define __RCC_OM662X_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_RCC)
#include "om_device.h"
#include "om_driver.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * INLINE FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief RC32K update
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_rcc_rc32k_upd_rdy(void)
{
    // clear status
    do {
        OM_CPM->REG_UPD = CPM_REG_UPD_REG_UPD_STATUS_CLR_MASK;
    } while(OM_CPM->REG_UPD);

    // update
    OM_CPM->REG_UPD = CPM_REG_UPD_REG_UPD_RC32K_APB_MASK;
    while(!(OM_CPM->REG_UPD & CPM_REG_UPD_REG_UPD_RC32K_STATUS_MASK));
}


/*******************************************************************************
 * MACROS
 */
/**
 *******************************************************************************
 * @brief Enable clock for specified peripheral
 *
 * @param[in] _rcc_clk_type         rcc clock type
 * @param[in] enable                0: disable clock   not 0: enable clock
 *******************************************************************************
 */
#define DRV_RCC_CLOCK_ENABLE(_rcc_clk_type, enable)                                                 \
    do {                                                                                            \
        if (enable) {                                                                               \
            switch ((uint32_t)_rcc_clk_type) {                                                      \
                case RCC_CLK_SF:                                                                    \
                    OM_CPM->SF_CFG &= ~CPM_SF_CFG_SF_GATE_EN_MASK;                                  \
                    break;                                                                          \
                case RCC_CLK_TIM0:                                                                  \
                    OM_CPM->TIMER0_CFG &= ~CPM_TIMERX_GATE_EN_MASK;                                 \
                    break;                                                                          \
                case RCC_CLK_TIM1:                                                                  \
                    OM_CPM->TIMER1_CFG &= ~CPM_TIMERX_GATE_EN_MASK;                                 \
                    break;                                                                          \
                case RCC_CLK_TIM2:                                                                  \
                    OM_CPM->TIMER2_CFG &= ~CPM_TIMERX_GATE_EN_MASK;                                 \
                    break;                                                                          \
                case RCC_CLK_USART0:                                                                \
                    OM_CPM->UART0_CFG &= ~CPM_UARTx_CFG_UARTx_GATE_EN_MASK;                         \
                    break;                                                                          \
                case RCC_CLK_USART1:                                                                \
                    OM_CPM->UART1_CFG &= ~CPM_UARTx_CFG_UARTx_GATE_EN_MASK;                         \
                    break;                                                                          \
                case RCC_CLK_I2C0:                                                                  \
                    OM_CPM->I2C_CFG &= ~CPM_I2C_CFG_I2C_GATE_EN_MASK;                               \
                    break;                                                                          \
                case RCC_CLK_LP_TIM:                                                                \
                    OM_PMU->BASIC &= ~PMU_BASIC_LPTIM_32K_CLK_GATE_MASK;                            \
                    break;                                                                          \
                case RCC_CLK_BLE:                                                                   \
                    register_set(&OM_CPM->BLE_CFG, MASK_4REG(CPM_BLE_CFG_BLE_AHB_GATE_EN,   0,      \
                                                             CPM_BLE_CFG_BLE_MASTER_CLK_EN, 1,      \
                                                             CPM_BLE_CFG_BLE_CLK_EN,        1,      \
                                                             CPM_BLE_CFG_BLE_MAC_GATE_EN,   0));    \
                    break;                                                                          \
                case RCC_CLK_DMA:                                                                   \
                    OM_CPM->DMA_CFG &= ~CPM_DMA_CFG_DMA_GATE_EN_MASK;                               \
                    break;                                                                          \
                case RCC_CLK_AES:                                                                   \
                    OM_CPM->AES_CFG |= CPM_AES_CLK_EN_MASK;                                         \
                    break;                                                                          \
                case RCC_CLK_GPIO0:                                                                 \
                    OM_CPM->GPIO_CFG  &= ~CPM_GPIO_CFG_GPIO_GATE_EN_MASK;                           \
                    break;                                                                          \
                case RCC_CLK_PHY:                                                                   \
                    OM_CPM->PHY_CFG &= ~(CPM_PHY_CFG_PHY_16M_GATE_EN_MASK | CPM_PHY_CFG_PHY_APB_GATE_EN_MASK); \
                    break;                                                                          \
                case RCC_CLK_RNG:                                                                   \
                    OM_CPM->RNG_CFG &= ~CPM_RNG_CFG_RNG_GATE_EN_MASK;                               \
                    drv_rcc_rc32k_upd_rdy();                                                        \
                    break;                                                                          \
                case RCC_CLK_2P4:                                                                   \
                    OM_CPM->MAC_2P4_CFG &= ~CPM_2P4_CFG_MAC_2G4_GATE_EN_MASK;                       \
                    break;                                                                          \
                case RCC_CLK_DAIF:                                                                  \
                    OM_CPM->ANA_IF_APB_CFG = 0;                                                     \
                    OM_CPM->ANA_IF_CFG = 0;                                                         \
                    break;                                                                          \
                case RCC_CLK_EFUSE:                                                                 \
                    OM_CPM->EFUSE_CFG &= ~CPM_EFUSE_GATE_EN_MASK;                                   \
                    break;                                                                          \
                case RCC_CLK_SPI0:                                                                  \
                    OM_CPM->SPI0_CFG &= ~CPM_SPI0_CFG_SPI0_GATE_EN_MASK;                            \
                    break;                                                                          \
                case RCC_CLK_SPI1:                                                                  \
                    OM_CPM->SPI1_CFG &= ~CPM_SPI1_CFG_SPI1_GATE_EN_MASK;                            \
                    break;                                                                          \
                case RCC_CLK_RTC:                                                                   \
                    OM_PMU->BASIC   &= ~PMU_BASIC_RTC_CLK_GATE_MASK;                                \
                    OM_CPM->APB_CFG &= ~CPM_APB_CFG_RTC_APB_GATE_EN_MASK;                           \
                    break;                                                                          \
                default:                                                                            \
                    break;                                                                          \
            }                                                                                       \
        } else {                                                                                    \
            switch ((uint32_t)_rcc_clk_type) {                                                      \
                case RCC_CLK_SF:                                                                    \
                    OM_CPM->SF_CFG |= CPM_SF_CFG_SF_GATE_EN_MASK;                                   \
                    break;                                                                          \
                case RCC_CLK_TIM0:                                                                  \
                    OM_CPM->TIMER0_CFG |= CPM_TIMERX_GATE_EN_MASK;                                  \
                    break;                                                                          \
                case RCC_CLK_TIM1:                                                                  \
                    OM_CPM->TIMER1_CFG |= CPM_TIMERX_GATE_EN_MASK;                                  \
                    break;                                                                          \
                case RCC_CLK_TIM2:                                                                  \
                    OM_CPM->TIMER2_CFG |= CPM_TIMERX_GATE_EN_MASK;                                  \
                    break;                                                                          \
                case RCC_CLK_USART0:                                                                \
                    OM_CPM->UART0_CFG |= CPM_UARTx_CFG_UARTx_GATE_EN_MASK;                          \
                    break;                                                                          \
                case RCC_CLK_USART1:                                                                \
                    OM_CPM->UART1_CFG |= CPM_UARTx_CFG_UARTx_GATE_EN_MASK;                          \
                    break;                                                                          \
                case RCC_CLK_I2C0:                                                                  \
                    OM_CPM->I2C_CFG |= CPM_I2C_CFG_I2C_GATE_EN_MASK;                                \
                    break;                                                                          \
                case RCC_CLK_LP_TIM:                                                                \
                    OM_PMU->BASIC |= PMU_BASIC_LPTIM_32K_CLK_GATE_MASK;                             \
                    break;                                                                          \
                case RCC_CLK_BLE:                                                                   \
                    register_set(&OM_CPM->BLE_CFG, MASK_4REG(CPM_BLE_CFG_BLE_AHB_GATE_EN,   1,      \
                                                             CPM_BLE_CFG_BLE_MASTER_CLK_EN, 0,      \
                                                             CPM_BLE_CFG_BLE_CLK_EN,        0,      \
                                                             CPM_BLE_CFG_BLE_MAC_GATE_EN,   1));    \
                    break;                                                                          \
                case RCC_CLK_DMA:                                                                   \
                    OM_CPM->DMA_CFG |= CPM_DMA_CFG_DMA_GATE_EN_MASK;                                \
                    break;                                                                          \
                case RCC_CLK_AES:                                                                   \
                    OM_CPM->AES_CFG &= ~CPM_AES_CLK_EN_MASK;                                        \
                    break;                                                                          \
                case RCC_CLK_GPIO0:                                                                 \
                    OM_CPM->GPIO_CFG  |= CPM_GPIO_CFG_GPIO_GATE_EN_MASK;                            \
                    break;                                                                          \
                case RCC_CLK_PHY:                                                                   \
                    OM_CPM->PHY_CFG |= (CPM_PHY_CFG_PHY_16M_GATE_EN_MASK | CPM_PHY_CFG_PHY_APB_GATE_EN_MASK); \
                    break;                                                                          \
                case RCC_CLK_RNG:                                                                   \
                    OM_CPM->RNG_CFG |= CPM_RNG_CFG_RNG_GATE_EN_MASK;                                \
                    drv_rcc_rc32k_upd_rdy();                                                        \
                    break;                                                                          \
                case RCC_CLK_2P4:                                                                   \
                    OM_CPM->MAC_2P4_CFG |= CPM_2P4_CFG_MAC_2G4_GATE_EN_MASK;                        \
                    break;                                                                          \
                case RCC_CLK_DAIF:                                                                  \
                    OM_CPM->ANA_IF_APB_CFG = CPM_ANA_IF_APB_GATE_EN_MASK;                           \
                    OM_CPM->ANA_IF_CFG = CPM_ANA_IF_GATE_EN_MASK;                                   \
                    break;                                                                          \
                case RCC_CLK_EFUSE:                                                                 \
                    OM_CPM->EFUSE_CFG |= CPM_EFUSE_GATE_EN_MASK;                                    \
                    break;                                                                          \
                case RCC_CLK_SPI0:                                                                  \
                    OM_CPM->SPI0_CFG |= CPM_SPI0_CFG_SPI0_GATE_EN_MASK;                             \
                    break;                                                                          \
                case RCC_CLK_SPI1:                                                                  \
                    OM_CPM->SPI1_CFG |= CPM_SPI1_CFG_SPI1_GATE_EN_MASK;                             \
                    break;                                                                          \
                case RCC_CLK_RTC:                                                                   \
                    OM_PMU->BASIC   |= PMU_BASIC_RTC_CLK_GATE_MASK;                                 \
                    OM_CPM->APB_CFG |= CPM_APB_CFG_RTC_APB_GATE_EN_MASK;                            \
                default:                                                                            \
                    break;                                                                          \
            }                                                                                       \
        }                                                                                           \
    } while(0)

/**
 *******************************************************************************
 * @brief Enable peripheral clock and reset peripheral
 *
 * @param[in] _rcc_clk_type         rcc clock type
 *******************************************************************************
 */
#define DRV_RCC_RESET(_rcc_clk_type)                                                            \
    do {                                                                                        \
        DRV_RCC_CLOCK_ENABLE(_rcc_clk_type, 1U);                                                \
        switch((uint32_t)_rcc_clk_type) {                                                       \
            case RCC_CLK_SF:                                                                    \
                OM_CPM->SF_CFG |= CPM_SF_CFG_SF_SOFT_RESET_MASK;                                \
                while(OM_CPM->SF_CFG & CPM_SF_CFG_SF_SOFT_RESET_MASK);                          \
                break;                                                                          \
            case RCC_CLK_TIM0:                                                                  \
                OM_CPM->TIMER0_CFG |= CPM_TIMERX_SOFT_RESET_MASK;                               \
                while(OM_CPM->TIMER0_CFG & CPM_TIMERX_SOFT_RESET_MASK);                         \
                break;                                                                          \
            case RCC_CLK_TIM1:                                                                  \
                OM_CPM->TIMER1_CFG |= CPM_TIMERX_SOFT_RESET_MASK;                               \
                while(OM_CPM->TIMER1_CFG & CPM_TIMERX_SOFT_RESET_MASK);                         \
                break;                                                                          \
            case RCC_CLK_TIM2:                                                                  \
                OM_CPM->TIMER2_CFG |= CPM_TIMERX_SOFT_RESET_MASK;                               \
                while(OM_CPM->TIMER2_CFG & CPM_TIMERX_SOFT_RESET_MASK);                         \
                break;                                                                          \
            case RCC_CLK_USART0:                                                                \
                OM_CPM->UART0_CFG |= CPM_UARTx_CFG_UARTx_SOFT_RESET_MASK;                       \
                while(OM_CPM->UART0_CFG & CPM_UARTx_CFG_UARTx_SOFT_RESET_MASK);                 \
                break;                                                                          \
            case RCC_CLK_USART1:                                                                \
                OM_CPM->UART1_CFG |= CPM_UARTx_CFG_UARTx_SOFT_RESET_MASK;                       \
                while(OM_CPM->UART1_CFG & CPM_UARTx_CFG_UARTx_SOFT_RESET_MASK);                 \
                break;                                                                          \
            case RCC_CLK_I2C0:                                                                  \
                OM_CPM->I2C_CFG |= CPM_I2C_CFG_I2C_SOFT_RESET_MASK;                             \
                while(OM_CPM->I2C_CFG & CPM_I2C_CFG_I2C_SOFT_RESET_MASK);                       \
                break;                                                                          \
            case RCC_CLK_LP_TIM:                                                                \
                register_set(&OM_PMU->MISC_CTRL, MASK_2REG(PMU_MISC_CTRL_LPTIM_APB_SOFT_RESET, 0,   \
                                                           PMU_MISC_CTRL_LPTIM_CLK_SFOT_RESET, 1)); \
                DRV_DELAY_US(40);                                                                  \
                register_set(&OM_PMU->MISC_CTRL, MASK_2REG(PMU_MISC_CTRL_LPTIM_APB_SOFT_RESET, 1,   \
                                                           PMU_MISC_CTRL_LPTIM_CLK_SFOT_RESET, 0)); \
                break;                                                                          \
            case RCC_CLK_BLE:                                                                   \
                OM_CPM->BLE_CFG |= CPM_BLE_CFG_BLE_AHB_SOFT_RESET_MASK;                         \
                OM_CPM->BLE_CFG &= ~CPM_BLE_CFG_BLE_AHB_SOFT_RESET_MASK;                        \
                break;                                                                          \
            case RCC_CLK_DMA:                                                                   \
                OM_CPM->DMA_CFG |= CPM_DMA_CFG_DMA_SOFT_RESET_MASK;                             \
                while(OM_CPM->DMA_CFG & CPM_DMA_CFG_DMA_SOFT_RESET_MASK);                       \
                break;                                                                          \
            case RCC_CLK_AES:                                                                   \
                OM_CPM->AES_CFG |= CPM_AES_SOFT_RESET_MASK;                                     \
                while(OM_CPM->AES_CFG & CPM_AES_SOFT_RESET_MASK);                               \
                break;                                                                          \
            case RCC_CLK_GPIO0:                                                                 \
                OM_CPM->GPIO_CFG  |= CPM_GPIO_CFG_GPIO_SOFT_RESET_MASK;                         \
                while(OM_CPM->GPIO_CFG & CPM_GPIO_CFG_GPIO_SOFT_RESET_MASK);                    \
                break;                                                                          \
            case RCC_CLK_PHY:                                                                   \
                OM_CPM->PHY_CFG |= CPM_PHY_CFG_PHY_SOFT_RESET_MASK;                             \
                while(OM_CPM->PHY_CFG & CPM_PHY_CFG_PHY_SOFT_RESET_MASK);                       \
                break;                                                                          \
            case RCC_CLK_RNG:                                                                   \
                OM_CPM->RNG_CFG |= CPM_RNG_CFG_RNG_SOFT_RESET_MASK;                             \
                while(OM_CPM->RNG_CFG & CPM_RNG_CFG_RNG_SOFT_RESET_MASK);                       \
                break;                                                                          \
            case RCC_CLK_2P4:                                                                   \
                OM_CPM->MAC_2P4_CFG |= CPM_2P4_CFG_MAC_2G4_SOFT_RESET_MASK;                     \
                while(OM_CPM->MAC_2P4_CFG & CPM_2P4_CFG_MAC_2G4_SOFT_RESET_MASK);               \
                break;                                                                          \
            case RCC_CLK_DAIF:                                                                  \
                OM_CPM->ANA_IF_APB_CFG |= CPM_ANA_IF_APB_SOFT_RESET_MASK;                       \
                while(OM_CPM->ANA_IF_APB_CFG & CPM_ANA_IF_APB_SOFT_RESET_MASK);                 \
                break;                                                                          \
            case RCC_CLK_EFUSE:                                                                 \
                OM_CPM->EFUSE_CFG |= CPM_EFUSE_SOFT_RESET_MASK;                                 \
                while(OM_CPM->EFUSE_CFG & CPM_EFUSE_SOFT_RESET_MASK);                           \
                break;                                                                          \
            case RCC_CLK_SPI0:                                                                  \
                OM_CPM->SPI0_CFG |= CPM_SPI0_CFG_SPI0_SOFT_RESET_MASK;                          \
                while(OM_CPM->SPI0_CFG & CPM_SPI0_CFG_SPI0_SOFT_RESET_MASK);                    \
                break;                                                                          \
            case RCC_CLK_SPI1:                                                                  \
                OM_CPM->SPI1_CFG |= CPM_SPI1_CFG_SPI1_SOFT_RESET_MASK;                          \
                while(OM_CPM->SPI0_CFG & CPM_SPI1_CFG_SPI1_SOFT_RESET_MASK);                    \
                break;                                                                          \
            default:                                                                            \
                break;                                                                          \
        }                                                                                       \
    } while(0)

/**
 *******************************************************************************
 * @brief ana if clock open
 *
 *******************************************************************************
 */
#define DRV_RCC_ANA_CLK_ENABLE_NOIRQ()                                                          \
    do {                                                                                        \
        uint32_t __daif_clk_is_dis = OM_CPM->ANA_IF_CFG;                                        \
        if (__daif_clk_is_dis) {                                                                \
            DRV_RCC_CLOCK_ENABLE(RCC_CLK_DAIF, 1U);                                             \
        }                                                                                       \

/**
 *******************************************************************************
 * @brief ana if clock restore
 *
 *******************************************************************************
 */
#define DRV_RCC_ANA_CLK_RESTORE_NOIRQ()                                                         \
        if (__daif_clk_is_dis) {                                                                \
            DRV_RCC_CLOCK_ENABLE(RCC_CLK_DAIF, 0U);                                             \
        }                                                                                       \
    } while(0)                                                                                  \

/**
 *******************************************************************************
 * @brief ana if clock open in the critical section
 *
 *******************************************************************************
 */
#define DRV_RCC_ANA_CLK_ENABLE()        OM_CRITICAL_BEGIN(); DRV_RCC_ANA_CLK_ENABLE_NOIRQ()

/**
 *******************************************************************************
 * @brief ana if clock restore in the critical section
 *
 *******************************************************************************
 */
#define DRV_RCC_ANA_CLK_RESTORE()       DRV_RCC_ANA_CLK_RESTORE_NOIRQ(); OM_CRITICAL_END()


/*******************************************************************************
 * TYPEDEFS
 */
/// Clock Source Definition
typedef enum {
    RCC_CLK_MAIN        = 0x0100U,
    RCC_CLK_CPU         = 0x0200U,

    RCC_CLK_SF          = OM_SF_BASE,
    RCC_CLK_TIM0        = OM_TIM0_BASE,
    RCC_CLK_TIM1        = OM_TIM1_BASE,
    RCC_CLK_TIM2        = OM_TIM2_BASE,
    RCC_CLK_USART0      = OM_USART0_BASE,
    RCC_CLK_USART1      = OM_USART1_BASE,
    RCC_CLK_I2C0        = OM_I2C0_BASE,
    RCC_CLK_LP_TIM      = OM_LP_TIM_BASE,
    RCC_CLK_BLE         = OM_BB_BASE,
    RCC_CLK_DMA         = OM_DMA_BASE,
    RCC_CLK_AES         = OM_AES_HW_BASE,
    RCC_CLK_GPIO0       = OM_GPIO_BASE,
    RCC_CLK_PHY         = OM_PHY_BASE,
    RCC_CLK_RNG         = OM_RNG_BASE,
    RCC_CLK_DAIF        = OM_DAIF_BASE,
    RCC_CLK_2P4         = OM_RF_2_4G_BASE,
    RCC_CLK_EFUSE       = OM_EFUSE_BASE,
    RCC_CLK_SPI0        = OM_SPI0_BASE,
    RCC_CLK_SPI1        = OM_SPI1_BASE,
    RCC_CLK_RTC         = OM_RTC_BASE,
} rcc_clk_t;


/*******************************************************************************
 * EXTERN VARIABLES
 */
/// CPU clock
extern uint32_t SystemCoreClock;


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Initialize system clock
 *
 *******************************************************************************
 */
extern void drv_rcc_init(void);

/**
 *******************************************************************************
 * @brief Get clock frequency for specified clock source
 *
 * @param[in] rcc_clk        Clock source
 *
 * @return clock frequency
 *******************************************************************************
 */
extern uint32_t drv_rcc_clock_get(rcc_clk_t rcc_clk);

/**
 *******************************************************************************
 * @brief Set clock frequency for specified clock source
 *
 * @param[in] rcc_clk        Clock source
 * @param[in] freq           Clock frequency, unit Hz
 *
 * @note if freq <= 64, freq will be considered as frequency division
 *
 * @return error information
 *******************************************************************************
 */
extern om_error_t drv_rcc_clock_set(rcc_clk_t rcc_clk, uint32_t freq);



#ifdef __cplusplus
}
#endif

#endif  /* RTE_RCC */

#endif  /* __RCC_OM662X_H */


/** @} */
