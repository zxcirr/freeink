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
 * @file     cpm_reg.h
 * @brief
 * @date     20. Oct 2021
 * @author   OnMicro SW Team
 *
 * @defgroup DOC DOC
 * @ingroup  DOCUMENT
 * @brief    templete
 * @details  reset and clock control for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


#ifndef __CPM_REG_H
#define __CPM_REG_H


/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include "../common/common_reg.h"


/*******************************************************************************
 * MACROS
 */
// cpm rev
#define CPM_REV_CPM_REVISION_POS                                    0
#define CPM_REV_CPM_REVISION_MASK                                   (0xffffffffU << 0)

// cpm cpu cfg
#define CPM_CPU_CFG_AHB_CLK_EN_PERIPH_POS                           23
#define CPM_CPU_CFG_AHB_CLK_EN_PERIPH_MASK                          (0x1U << 23)
#define CPM_CPU_CFG_AHB_CLK_EN_RAM_POS                              22
#define CPM_CPU_CFG_AHB_CLK_EN_RAM_MASK                             (0x1U << 22)
#define CPM_CPU_CFG_AHB_CLK_MON_POS                                 21
#define CPM_CPU_CFG_AHB_CLK_MON_MASK                                (0x1U << 21)
#define CPM_CPU_CFG_CPU_DIV_COEFF_POS                               8
#define CPM_CPU_CFG_CPU_DIV_COEFF_MASK                              (0xffU << 8)
#define CPM_CPU_CFG_CPU_DIV_SEL_POS                                 2
#define CPM_CPU_CFG_CPU_DIV_SEL_MASK                                (0x1U << 2)
#define CPM_CPU_CFG_CPU_DIV_EN_POS                                  1
#define CPM_CPU_CFG_CPU_DIV_EN_MASK                                 (0x1U << 1)

// cpm apb cfg
#define CPM_APB_CFG_SYS_APB_SOFT_RESET_POS                          18
#define CPM_APB_CFG_SYS_APB_SOFT_RESET_MASK                         (0x1U << 18)
#define CPM_APB_CFG_PMU_APB_SOFT_RESET_POS                          16
#define CPM_APB_CFG_PMU_APB_SOFT_RESET_MASK                         (0x1U << 16)
#define CPM_APB_CFG_SYS_APB_AUTO_GATE_DIS_POS                       7
#define CPM_APB_CFG_SYS_APB_AUTO_GATE_DIS_MASK                      (0x1U << 7)
#define CPM_APB_CFG_CPM_APB_AUTO_GATE_DIS_POS                       6
#define CPM_APB_CFG_CPM_APB_AUTO_GATE_DIS_MASK                      (0x1U << 6)
#define CPM_APB_CFG_RTC_APB_AUTO_GATE_DIS_POS                       5
#define CPM_APB_CFG_RTC_APB_AUTO_GATE_DIS_MASK                      (0x1U << 5)
#define CPM_APB_CFG_PMU_APB_AUTO_GATE_DIS_POS                       4
#define CPM_APB_CFG_PMU_APB_AUTO_GATE_DIS_MASK                      (0x1U << 4)
#define CPM_APB_CFG_SYS_APB_GATE_EN_POS                             2
#define CPM_APB_CFG_SYS_APB_GATE_EN_MASK                            (0x1U << 2)
#define CPM_APB_CFG_RTC_APB_GATE_EN_POS                             1
#define CPM_APB_CFG_RTC_APB_GATE_EN_MASK                            (0x1U << 1)
#define CPM_APB_CFG_PMU_APB_GATE_EN_POS                             0
#define CPM_APB_CFG_PMU_APB_GATE_EN_MASK                            (0x1U << 0)

// cpm reg upd
#define CPM_REG_UPD_REG_UPD_32K_STATUS_POS                          6
#define CPM_REG_UPD_REG_UPD_32K_STATUS_MASK                         (0x1U << 6)
#define CPM_REG_UPD_REG_UPD_RC32K_STATUS_POS                        5
#define CPM_REG_UPD_REG_UPD_RC32K_STATUS_MASK                       (0x1U << 5)
#define CPM_REG_UPD_REG_UPD_STATUS_CLR_POS                          3
#define CPM_REG_UPD_REG_UPD_STATUS_CLR_MASK                         (0x1U << 3)
#define CPM_REG_UPD_REG_UPD_32K_APB_POS                             2
#define CPM_REG_UPD_REG_UPD_32K_APB_MASK                            (0x1U << 2)
#define CPM_REG_UPD_REG_UPD_RC32K_APB_POS                           1
#define CPM_REG_UPD_REG_UPD_RC32K_APB_MASK                          (0x1U << 1)

// cpm sf cfg
#define CPM_SF_CFG_SF_DIV_COEFF_POS                                 8
#define CPM_SF_CFG_SF_DIV_COEFF_MASK                                (0xffU << 8)
#define CPM_SF_CFG_SF_SOFT_RESET_POS                                4
#define CPM_SF_CFG_SF_SOFT_RESET_MASK                               (0x1U << 4)
#define CPM_SF_CFG_SF_DIV_SEL_POS                                   2
#define CPM_SF_CFG_SF_DIV_SEL_MASK                                  (0x1U << 2)
#define CPM_SF_CFG_SF_DIV_EN_POS                                    1
#define CPM_SF_CFG_SF_DIV_EN_MASK                                   (0x1U << 1)
#define CPM_SF_CFG_SF_GATE_EN_POS                                   0
#define CPM_SF_CFG_SF_GATE_EN_MASK                                  (0x1U << 0)

// cpm timerx
#define CPM_TIMERX_SOFT_RESET_POS                                   4
#define CPM_TIMERX_SOFT_RESET_MASK                                  (0x1U << 4)
#define CPM_TIMERX_APB_AUTO_GATE_DIS_POS                            3
#define CPM_TIMERX_APB_AUTO_GATE_DIS_MASK                           (0x1U << 3)
#define CPM_TIMERX_GATE_EN_POS                                      0
#define CPM_TIMERX_GATE_EN_MASK                                     (0x1U << 0)

// cpm uartx cfg
#define CPM_UARTx_CFG_UARTx_DIV_COEFF_FRC_POS                       24
#define CPM_UARTx_CFG_UARTx_DIV_COEFF_FRC_MASK                      (0xffU << 24)
#define CPM_UARTx_CFG_UARTx_DIV_COEFF_INT_POS                       8
#define CPM_UARTx_CFG_UARTx_DIV_COEFF_INT_MASK                      (0x1ffU << 8)
#define CPM_UARTx_CFG_UARTx_SOFT_RESET_POS                          4
#define CPM_UARTx_CFG_UARTx_SOFT_RESET_MASK                         (0x1U << 4)
#define CPM_UARTx_CFG_UARTx_DIV_SEL_POS                             2
#define CPM_UARTx_CFG_UARTx_DIV_SEL_MASK                            (0x1U << 2)
#define CPM_UARTx_CFG_UARTx_DIV_EN_POS                              1
#define CPM_UARTx_CFG_UARTx_DIV_EN_MASK                             (0x1U << 1)
#define CPM_UARTx_CFG_UARTx_GATE_EN_POS                             0
#define CPM_UARTx_CFG_UARTx_GATE_EN_MASK                            (0x1U << 0)

// cpm i2c cfg
#define CPM_I2C_CFG_I2C_SOFT_RESET_POS                              4
#define CPM_I2C_CFG_I2C_SOFT_RESET_MASK                             (0x1U << 4)
#define CPM_I2C_CFG_I2C_APB_AUTO_GATE_DIS_POS                       3
#define CPM_I2C_CFG_I2C_APB_AUTO_GATE_DIS_MASK                      (0x1U << 3)
#define CPM_I2C_CFG_I2C_GATE_EN_POS                                 0
#define CPM_I2C_CFG_I2C_GATE_EN_MASK                                (0x1U << 0)

// cpm le timer
#define CPM_LE_TIMER_SOFT_RESET_POS                                 4
#define CPM_LE_TIMER_SOFT_RESET_MASK                                (0x1U << 4)
#define CPM_LE_TIMER_APB_AUTO_GATE_DIS_POS                          3
#define CPM_LE_TIMER_APB_AUTO_GATE_DIS_MASK                         (0x1U << 3)
#define CPM_LE_TIMER_GATE_EN_POS                                    0
#define CPM_LE_TIMER_GATE_EN_MASK                                   (0x1U << 0)

// cpm ble cfg
#define CPM_BLE_CFG_BLE_MAC_GATE_EN_POS                             9
#define CPM_BLE_CFG_BLE_MAC_GATE_EN_MASK                            (0x1U << 9)
#define CPM_BLE_CFG_BLE_AHB_SOFT_RESET_POS                          7
#define CPM_BLE_CFG_BLE_AHB_SOFT_RESET_MASK                         (0x1U << 7)
#define CPM_BLE_CFG_BLE_CLK_EN_POS                                  2
#define CPM_BLE_CFG_BLE_CLK_EN_MASK                                 (0x1U << 2)
#define CPM_BLE_CFG_BLE_MASTER_CLK_EN_POS                           1
#define CPM_BLE_CFG_BLE_MASTER_CLK_EN_MASK                          (0x1U << 1)
#define CPM_BLE_CFG_BLE_AHB_GATE_EN_POS                             0
#define CPM_BLE_CFG_BLE_AHB_GATE_EN_MASK                            (0x1U << 0)

// cpm cpu tclk cfg
#define CPM_CPU_TCLK_GATE_EN_POS                                    0
#define CPM_CPU_TCLK_GATE_EN_MASK                                   (0x1U << 0)

// cpm ahb cfg
#define CPM_AHB_CFG_RAM_AUTO_GATE_EN_POS                            16
#define CPM_AHB_CFG_RAM_AUTO_GATE_EN_MASK                           (0x1U << 16)
#define CPM_AHB_CFG_AHB_SOFT_RESET_POS                              4
#define CPM_AHB_CFG_AHB_SOFT_RESET_MASK                             (0x1U << 4)

// cpm dma cfg
#define CPM_DMA_CFG_DMA_SOFT_RESET_POS                              4
#define CPM_DMA_CFG_DMA_SOFT_RESET_MASK                             (0x1U << 4)
#define CPM_DMA_CFG_DMA_GATE_EN_POS                                 0
#define CPM_DMA_CFG_DMA_GATE_EN_MASK                                (0x1U << 0)

// cpm ram cfg
#define CPM_RAM_CFG_PLL_RAM_SEL_POS                                 4
#define CPM_RAM_CFG_PLL_RAM_SEL_MASK                                (0x1U << 4)
#define CPM_RAM_CFG_RAM2_GATE_EN_POS                                2
#define CPM_RAM_CFG_RAM2_GATE_EN_MASK                               (0x1U << 2)
#define CPM_RAM_CFG_RAM1_GATE_EN_POS                                1
#define CPM_RAM_CFG_RAM1_GATE_EN_MASK                               (0x1U << 1)
#define CPM_RAM_CFG_RAM0_GATE_EN_POS                                0
#define CPM_RAM_CFG_RAM0_GATE_EN_MASK                               (0x1U << 0)

// cpm aes cfg
#define CPM_AES_SOFT_RESET_POS                                      4
#define CPM_AES_SOFT_RESET_MASK                                     (0x1U << 4)
#define COM_AES_CLK_SEL_POS                                         1
#define COM_AES_CLK_SEL_MASK                                        (0x1U << 1)
#define CPM_AES_CLK_EN_POS                                          0
#define CPM_AES_CLK_EN_MASK                                         (0x1U << 0)

// cpm gpio cfg
#define CPM_GPIO_CFG_GPIO_SOFT_RESET_POS                            4
#define CPM_GPIO_CFG_GPIO_SOFT_RESET_MASK                           (0x1U << 4)
#define CPM_GPIO_CFG_GPIO_GATE_EN_POS                               0
#define CPM_GPIO_CFG_GPIO_GATE_EN_MASK                              (0x1U << 0)

// cpm phy cfg
#define CPM_PHY_CFG_PHY_SOFT_RESET_POS                              4
#define CPM_PHY_CFG_PHY_SOFT_RESET_MASK                             (0x1U << 4)
#define CPM_PHY_CFG_PHY_APB_AUTO_GATE_DIS_POS                       3
#define CPM_PHY_CFG_PHY_APB_AUTO_GATE_DIS_MASK                      (0x1U << 3)
#define CPM_PHY_CFG_PHY_16M_GATE_EN_POS                             1
#define CPM_PHY_CFG_PHY_16M_GATE_EN_MASK                            (0x1U << 1)
#define CPM_PHY_CFG_PHY_APB_GATE_EN_POS                             0
#define CPM_PHY_CFG_PHY_APB_GATE_EN_MASK                            (0x1U << 0)

// cpm rng cfg
#define CPM_RNG_CFG_RNG_SOFT_RESET_POS                              4
#define CPM_RNG_CFG_RNG_SOFT_RESET_MASK                             (0x1U << 4)
#define CPM_RNG_CFG_RNG_GATE_EN_POS                                 0
#define CPM_RNG_CFG_RNG_GATE_EN_MASK                                (0x1U << 0)

// cpm status read
#define CPM_STATUS_READ_CPU_CLK_SYNC_DONE_POS                       4
#define CPM_STATUS_READ_CPU_CLK_SYNC_DONE_MASK                      (0x1U << 4)
#define CPM_STATUS_READ_UART1_CLK_SYNC_DONE_POS                     3
#define CPM_STATUS_READ_UART1_CLK_SYNC_DONE_MASK                    (0x1U << 3)
#define CPM_STATUS_READ_UART0_CLK_SYNC_DONE_POS                     2
#define CPM_STATUS_READ_UART0_CLK_SYNC_DONE_MASK                    (0x1U << 2)
#define CPM_STATUS_READ_SF_CLK_SYNC_DONE_POS                        1
#define CPM_STATUS_READ_SF_CLK_SYNC_DONE_MASK                       (0x1U << 1)
#define CPM_STATUS_READ_MAIN_CLK_SYNC_DONE_POS                      0
#define CPM_STATUS_READ_MAIN_CLK_SYNC_DONE_MASK                     (0x1U << 0)

// cpm ana if apb cfg
#define CPM_ANA_IF_APB_SOFT_RESET_POS                               4
#define CPM_ANA_IF_APB_SOFT_RESET_MASK                              (0x1U << 4)
#define CPM_ANA_IF_APB_AUTO_GATE_DIS_POS                            3
#define CPM_ANA_IF_APB_AUTO_GATE_DIS_MASK                           (0x1U << 3)
#define CPM_ANA_IF_APB_GATE_EN_POS                                  0
#define CPM_ANA_IF_APB_GATE_EN_MASK                                 (0x1U << 0)

// cpm 2p4 cfg
#define CPM_2P4_CFG_MAC_2G4_SOFT_RESET_POS                          4
#define CPM_2P4_CFG_MAC_2G4_SOFT_RESET_MASK                         (0x1U << 4)
#define CPM_2P4_CFG_MAC_2G4_AUTO_GATE_DIS_POS                       3
#define CPM_2P4_CFG_MAC_2G4_AUTO_GATE_DIS_MASK                      (0x1U << 3)
#define CPM_2P4_CFG_MAC_2G4_GATE_EN_POS                             0
#define CPM_2P4_CFG_MAC_2G4_GATE_EN_MASK                            (0x1U << 0)

// cpm ana if cfg
#define CPM_ANA_IF_GATE_EN_POS                                      0
#define CPM_ANA_IF_GATE_EN_MASK                                     (0x1U << 0)

// cpm efuse cfg
#define CPM_EFUSE_SOFT_RESET_POS                                    4
#define CPM_EFUSE_SOFT_RESET_MASK                                   (0x1U << 4)
#define CPM_EFUSE_APB_AUTO_GATE_DIS_POS                             3
#define CPM_EFUSE_APB_AUTO_GATE_DIS_MASK                            (0x1U << 3)
#define CPM_EFUSE_GATE_EN_POS                                       0
#define CPM_EFUSE_GATE_EN_MASK                                      (0x1U << 0)

// cpm spi0 cfg
#define CPM_SPI0_CFG_SPI0_SOFT_RESET_POS                            4
#define CPM_SPI0_CFG_SPI0_SOFT_RESET_MASK                           (0x1U << 4)
#define CPM_SPI0_CFG_SPI0_APB_AUTO_GATE_DIS_POS                     3
#define CPM_SPI0_CFG_SPI0_APB_AUTO_GATE_DIS_MASK                    (0x1U << 3)
#define CPM_SPI0_CFG_SPI0_GATE_EN_POS                               0
#define CPM_SPI0_CFG_SPI0_GATE_EN_MASK                              (0x1U << 0)

// cpm spi1 cfg
#define CPM_SPI1_CFG_SPI1_SOFT_RESET_POS                            4
#define CPM_SPI1_CFG_SPI1_SOFT_RESET_MASK                           (0x1U << 4)
#define CPM_SPI1_CFG_SPI1_APB_AUTO_GATE_DIS_POS                     3
#define CPM_SPI1_CFG_SPI1_APB_AUTO_GATE_DIS_MASK                    (0x1U << 3)
#define CPM_SPI1_CFG_SPI1_GATE_EN_POS                               0
#define CPM_SPI1_CFG_SPI1_GATE_EN_MASK                              (0x1U << 0)

// cpm sleep cfg
#define CPM_SLEEP_GATE_EN_POS                                       0
#define CPM_SLEEP_GATE_EN_MASK                                      (0x1U << 0)


/*******************************************************************************
 * TYPEDEFS
 */
/* CPM register file definitions */
typedef struct {
    __IO uint32_t REV;              // offset:0x00
    __IO uint32_t CPU_CFG;          // offset:0x04  (Auto Restore)
    __IO uint32_t APB_CFG;          // offset:0x08  (Auto Restore)
    __IO uint32_t REG_UPD;          // offset:0x0c
    __IO uint32_t SF_CFG;           // offset:0x10  (Auto Restore)
    __IO uint32_t TIMER0_CFG;       // offset:0x14
    __IO uint32_t TIMER1_CFG;       // offset:0x18
    __IO uint32_t TIMER2_CFG;       // offset:0x1c
    __IO uint32_t UART0_CFG;        // offset:0x20
    __IO uint32_t UART1_CFG;        // offset:0x24
    __IO uint32_t I2C_CFG;          // offset:0x28
    __IO uint32_t LE_TIMER_CFG;     // offset:0x2C
    __IO uint32_t BLE_CFG;          // offset:0x30
    __IO uint32_t CPU_TCLK_CFG;     // offset:0x34
    __IO uint32_t AHB_CFG;          // offset:0x38
    __IO uint32_t DMA_CFG;          // offset:0x3c
    __IO uint32_t RAM_CFG;          // offset:0x40  (Auto Restore)
    __IO uint32_t AES_CFG;          // offset:0x44
    __IO uint32_t GPIO_CFG;         // offset:0x48  (Auto Restore)
    __IO uint32_t PHY_CFG;          // offset:0x4c
    __IO uint32_t RNG_CFG;          // offset:0x50
         uint32_t Reserved_54;
    __IO uint32_t STATUS_READ;      // offset:0x58
    __IO uint32_t ANA_IF_APB_CFG;   // offset:0x5c
    __IO uint32_t MAC_2P4_CFG;      // offset:0x60
    __IO uint32_t ANA_IF_CFG;       // offset:0x64
    __IO uint32_t EFUSE_CFG;        // offset:0x68
    __IO uint32_t SPI0_CFG;         // offset:0x6C
    __IO uint32_t SPI1_CFG;         // offset:0x70
         uint32_t Reserved_74;
    __IO uint32_t SLEEP_CFG;        // offset:0x78
} OM_CPM_Type;


#endif  /* __CPM_REG_H*/


/** @} */
