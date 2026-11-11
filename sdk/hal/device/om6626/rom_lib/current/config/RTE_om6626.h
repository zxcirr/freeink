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
 * @file     RTE_om6626.h
 * @brief    RTE device for om662x
 * @date     26. Aug. 2021
 * @author   OnMicro SW Team
 *
 * @defgroup DOC DOC
 * @ingroup  DOCUMENT
 * @brief    templete
 * @details  RTE device for om6626
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __RTE_OM6626_H
#define __RTE_OM6626_H

/*******************************************************************************
 * MACROS
 */
// <o.0> RTE_CORTEX_DWT
#define RTE_CORTEX_DWT                                      1
// <o.0> RTE_CORTEX_DWT_DELAY
#define RTE_CORTEX_DWT_DELAY                                1
// <o.0> RTE_CORTEX_DWT_TIMEOUT
#define RTE_CORTEX_DWT_TIMEOUT                              1

// <o.0> RTE_RCC
#define RTE_RCC                                             1

// <o.0> RTE_ISR
#define RTE_ISR                                             1

// <o.0> RTE_PIN
#define RTE_PIN                                             1

// <o.0> RTE_PMU
#define RTE_PMU                                             1
// <o.0> RTE_PMU_WAKEUP_PIN_IRQ_PRIORITY
#define RTE_PMU_WAKEUP_PIN_IRQ_PRIORITY                     4

// <o.0> RTE_PMU_TIMER_REGISTER_CALLBACK
#define RTE_PMU_TIMER_REGISTER_CALLBACK                     1
// <o.0> RTE_PMU_TIMER
#define RTE_PMU_TIMER                                       1
// <o.0> RTE_PMU_TIMER_IRQ_PRIORITY
#define RTE_PMU_TIMER_IRQ_PRIORITY                          15

// <o.0> RTE_USART_REGISTER_CALLBACK
#define RTE_USART_REGISTER_CALLBACK                         1
// <o.0> RTE_USART0
#define RTE_USART0                                          1
// <o.0> RTE_USART0_IRQ_PRIORITY
#define RTE_USART0_IRQ_PRIORITY                             3
// <o.0> RTE_USART1
#define RTE_USART1                                          1
// <o.0> RTE_USART1_IRQ_PRIORITY
#define RTE_USART1_IRQ_PRIORITY                             3
// <o.0> RTE_USART1_DMA_TX_PRIORITY
#define RTE_USART1_DMA_TX_PRIORITY                          1
// <o.0> RTE_USART1_DMA_RX_PRIORITY
#define RTE_USART1_DMA_RX_PRIORITY                          1

// <o.0> RTE_GPIO_REGISTER_CALLBACK
#define RTE_GPIO_REGISTER_CALLBACK                          1
// <o.0> RTE_GPIO0
#define RTE_GPIO0                                           1
// <o.0> RTE_GPIO0_IRQ_PRIORITY
#define RTE_GPIO0_IRQ_PRIORITY                              4

// <o.0> RTE_SF_BASE
#define RTE_SF_BASE                                         1
// <o.0> RTE_SF_BASE_IRQ_PRIORITY
#define RTE_SF_BASE_IRQ_PRIORITY                            4
// <o.0> RTE_SF_BASE_USING_ROM_SYMBOL
#define RTE_SF_BASE_USING_ROM_SYMBOL                        1
// <o.0> RTE_SF
#define RTE_SF                                              1
// <o.0> RTE_SF_BUSY_ALLOW_IRQ_PRIORIT
#define RTE_SF_BUSY_ALLOW_IRQ_PRIORIT                       1
// <o.0> RTE_SF_USING_ROM_SYMBOL
#define RTE_SF_USING_ROM_SYMBOL                             0
// <o.0> RTE_SF_SYS
#define RTE_SF_SYS                                          1
// <o.0> RTE_SF_SYS_USING_ROM_SYMBOL
#define RTE_SF_SYS_USING_ROM_SYMBOL                         0

// <o.0> RTE_EFUSE
#define RTE_EFUSE                                           1

// <o.0> RTE_AES_HW
#define RTE_AES_HW                                          1

// <o.0> RTE_OM24G
#define RTE_OM24G                                           1
// <o.0> RTE_OM24G_IRQ_PRIORITY
#define RTE_OM24G_IRQ_PRIORITY                              2
// <o.0> RTE_OM24G_TI2640
#define RTE_OM24G_TI2640                                    0
// <o.0> RTE_OM24G_OM6621X
#define RTE_OM24G_OM6621X                                   1
// <o.0> RTE_OM24G_SILICONLAB
#define RTE_OM24G_SILICONLAB                                0
// <o.0> RTE_OM24G_NORDIC
#define RTE_OM24G_NORDIC                                    0

// <o.0> RTE_DMA
#define RTE_DMA                                             1
// <o.0> RTE_DMA_IRQ_PRIORITY
#define RTE_DMA_IRQ_PRIORITY                                3

// <o.0> RTE_SPI_REGISTER_CALLBACK
#define RTE_SPI_REGISTER_CALLBACK                           1
// <o.0> RTE_SPI0
#define RTE_SPI0                                            1
// <o.0> RTE_SPI0_IRQ_PRIORITY
#define RTE_SPI0_IRQ_PRIORITY                               3
// <o.0> RTE_SPI0_DMA_TX_PRIORITY
#define RTE_SPI0_DMA_TX_PRIORITY                            0
// <o.0> RTE_SPI0_DMA_RX_PRIORITY
#define RTE_SPI0_DMA_RX_PRIORITY                            0
// <o.0> RTE_SPI1
#define RTE_SPI1                                            1
// <o.0> RTE_SPI1_IRQ_PRIORITY
#define RTE_SPI1_IRQ_PRIORITY                               3
// <o.0> RTE_SPI1_DMA_TX_PRIORITY
#define RTE_SPI1_DMA_TX_PRIORITY                            0
// <o.0> RTE_SPI1_DMA_RX_PRIORITY
#define RTE_SPI1_DMA_RX_PRIORITY                            0

// <o.0> RTE_I2C_REGISTER_CALLBACK
#define RTE_I2C_REGISTER_CALLBACK                           1
// <o.0> RTE_I2C0
#define RTE_I2C0                                            1
// <o.0> RTE_I2C0_IRQ_PRIORITY
#define RTE_I2C0_IRQ_PRIORITY                               3
// <o.0> RTE_I2C0_DMA_TX_PRIORITY
#define RTE_I2C0_DMA_TX_PRIORITY                            0
// <o.0> RTE_I2C0_DMA_RX_PRIORITY
#define RTE_I2C0_DMA_RX_PRIORITY                            0

// <o.0> RTE_RTC
#define RTE_RTC                                             1
// <o.0> RTE_RTC_1HZ_IRQ_PRIORITY
#define RTE_RTC_1HZ_IRQ_PRIORITY                            6
// <o.0> RTE_RTC_AF_IRQ_PRIORITY
#define RTE_RTC_AF_IRQ_PRIORITY                             6

// <o.0> RTE_RADIO
#define RTE_RADIO                                           1

// <o.0> RTE_TRNG
#define RTE_TRNG                                            1

// <o.0> RTE_GPADC
#define RTE_GPADC                                           1
// <o.0> RTE_GPADC_IRQ_PRIORITY
#define RTE_GPADC_IRQ_PRIORITY                              3

// <o.0> RTE_SYSTEM_USING_ROM_SYMBOL
#define RTE_SYSTEM_USING_ROM_SYMBOL                         1

// <o.0> RTE_TIM0
#define RTE_TIM0                                            1
// <o.0> RTE_TIM0_IRQ_PRIORITY
#define RTE_TIM0_IRQ_PRIORITY                               3
// <o.0> RTE_TIM0_DMA_PRIORITY
#define RTE_TIM0_DMA_PRIORITY                               0
// <o.0> RTE_TIM1
#define RTE_TIM1                                            1
// <o.0> RTE_TIM1_IRQ_PRIORITY
#define RTE_TIM1_IRQ_PRIORITY                               3
// <o.0> RTE_TIM1_DMA_PRIORITY
#define RTE_TIM1_DMA_PRIORITY                               0
// <o.0> RTE_TIM2
#define RTE_TIM2                                            1
// <o.0> RTE_TIM2_IRQ_PRIORITY
#define RTE_TIM2_IRQ_PRIORITY                               3
// <o.0> RTE_TIM2_DMA_PRIORITY
#define RTE_TIM2_DMA_PRIORITY                               0

// <o.0> RTE_RNG
#define RTE_RNG                                             1

// <o.0> RTE_LP_TIM_REGISTER_CALLBACK
#define RTE_LP_TIM_REGISTER_CALLBACK                        1
// <o.0> RTE_LP_TIM
#define RTE_LP_TIM                                          1
// <o.0> RTE_LP_TIM_IRQ_PRIORITY
#define RTE_LP_TIM_IRQ_PRIORITY                             6

// <o.0> RTE_WDT
#define RTE_WDT                                             1
// <o.0> RTE_WDT_IRQ_PRIORITY
#define RTE_WDT_IRQ_PRIORITY                                2


/*******************************************************************************
 * RTE for `./components/`
 * The modules below are in `./components/`, but are also managed by `RTE`
 */
// <o.0> RTE_PM
#define RTE_PM                                              1
// <o.0> RTE_PM_USING_ROM_SYMBOL
#define RTE_PM_USING_ROM_SYMBOL                             0
// <o.0> RTE_PM_USING_ROM_ENV_SYMBOL
#define RTE_PM_USING_ROM_ENV_SYMBOL                         0

// <o.0> RTE_BLE_IRQ_PRIORITY
#define RTE_BLE_IRQ_PRIORITY                                2
// <o.0> RTE_BLE_WAKEUP_IRQ_PRIORITY
#define RTE_BLE_WAKEUP_IRQ_PRIORITY                         1

#endif  /* __RTE_OM6626_H */


/** @} */

