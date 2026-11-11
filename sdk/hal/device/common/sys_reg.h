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
 * @file     sys_reg.h
 * @brief    system register
 * @date     15. December 2021
 * @author   OnMicro SW Team
 *
 * @defgroup DOC DOC
 * @ingroup  DOCUMENT
 * @brief    templete
 * @details  system register
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __SYS_REG_H
#define __SYS_REG_H


/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include "common_reg.h"


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct
{
    __I  uint32_t REV_ID;               // offset:0x00
    __I  uint32_t CHIP_ID;              // offset:0x04
         uint32_t RESERVE1[3];
    __IO uint32_t SYS_TICK;             // offset:0x14
    __IO uint32_t GPIO_POWER_UP_STATUS; // offset:0x18
    __IO uint32_t GPIO_POWER_UP_STATUS_1;//offset:0x1C
    __IO uint32_t RST_32K_OSC_CTRL;     // offset:0x20
         uint32_t RESERVE3[5];
    __IO uint32_t MON;                  // offset:0x38
    __IO uint32_t USB_CTRL;             // offset:0x3C
    __I  uint32_t CHRGR_STAT;           // offset:0x40
    __IO uint32_t SOFT_INT_SET;         // offset:0x44
    __IO uint32_t SOFT_INT_CLR;         // offset:0x48
         uint32_t RESERVE5[13];
    __IO uint32_t PINMUX[8];            // offset:0x80  (Auto Restore)
} OM_SYS_Type;


/*******************************************************************************
 * MACROS
 */
// REV_ID
#define SYS_REV_ANA_POS                             0
#define SYS_REV_SOFT_POS                            4
#define SYS_REV_CHIP_POS                            8
#define SYS_REV_FPGA_FLAG_POS                       0
#define SYS_REV_OTP_CHECK_POS                       15
#define SYS_REV_ANA_MASK                            0x0000000F
#define SYS_REV_SOFT_MASK                           0x000000F0
#define SYS_REV_CHIP_MASK                           0x00000F00
#define SYS_REV_FPGA_FLAG_MASK                      0x00000FFF
#define SYS_REV_BONDING_OTP_MASK                    0x00008000
#define SYS_REV_SOFT_MAIN_VERSION                   0
#define SYS_REV_SOFT_RAM_VERSION                    1
#define SYS_REV_SOFT_SFLASH_VERSION                 2
#define SYS_REV_FPGA_FLAG                           0x555
#define SYS_IS_FPGA()                               ((OM_SYS->REV_ID & SYS_REV_FPGA_FLAG_MASK) == SYS_REV_FPGA_FLAG)
#define SYS_IS_A3()                                 ((OM_SYS->REV_ID & SYS_REV_CHIP_MASK) == 0x00000200)
// GPIO_POWER_UP_STATUS
#define SYS_GPIO_STAUS_POWERDOWN_MASK               0x80000000
// SOFT_INT
#define SYS_SOFT_INI_0_MASK                         0x00000001
#define SYS_SOFT_INI_1_MASK                         0x00000002
#define SYS_SOFT_INI_2_MASK                         0x00000004
#define SYS_SOFT_INI_3_MASK                         0x00000008
#define SYS_SOFT_INI_4_MASK                         0x00000010
#define SYS_SOFT_INI_5_MASK                         0x00000020
#define SYS_SOFT_INI_6_MASK                         0x00000040
#define SYS_SOFT_INI_7_MASK                         0x00000080
// SYS_TICK
#define SYS_CPU_SYS_TICK_CON_POS                    0
#define SYS_CPU_SYS_TICK_UPD_POS                    26
#define SYS_CPU_SYS_TICK_CON_MASK                   0x03FFFFFF
#define SYS_CPU_SYS_TICK_UPD_MASK                   0x04000000
// RST_32K_OSC_CTRL
#define SYS_CRY32M_DIV_EN_POS                       12
#define SYS_CLK_32K_DIV_RST_TRIG_POS                11
#define SYS_RST_32K_RC_TRIG_POS                     10
#define SYS_RST_32K_RC_TIME_POS                     0
#define SYS_CRY32M_DIV_EN_MASK                      0x00001000
#define SYS_CLK_32K_DIV_RST_TRIG_MASK               0x00000800
#define SYS_RST_32K_RC_TRIG_MASK                    0x00000400
#define SYS_RST_32K_RC_TIME_MASK                    0x000003FF
// MON
#define SYS_MON_CPM(n)                              (0x0000 + n)
#define SYS_MON_TIMER0(n)                           (0x0100 + n)
#define SYS_MON_TIMER1(n)                           (0x0200 + n)
#define SYS_MON_TIMER2(n)                           (0x0300 + n)
#define SYS_MON_SFLASH(n)                           (0x0400 + n)
#define SYS_MON_KPP(n)                              (0x0500 + n)
#define SYS_MON_SPI0(n)                             (0x0600 + n)
#define SYS_MON_SPI1(n)                             (0x0700 + n)
#define SYS_MON_WATCHDOG(n)                         (0x0800 + n)
#define SYS_MON_APB(n)                              (0x0900 + n)
#define SYS_MON_AHB(n)                              (0x0A00 + n)
#define SYS_MON_UART0(n)                            (0x0B00 + n)
#define SYS_MON_UART1(n)                            (0x0C00 + n)
#define SYS_MON_BASEBAND(n)                         (0x0D00 + n)
#define SYS_MON_PHY(n)                              (0x0E00 + n)
//#define SYS_MON_PHY(n)                              (0x0F00 + n)
#define SYS_MON_DAIF(n)                             (0x1000 + n)
#define SYS_MON_PMU(n)                              (0x1100 + n)
#define SYS_MON_QDEC(n)                             (0x1200 + n)
#define SYS_MON_RNG(n)                              (0x1300 + n)
// Simulation
#define SYS_SIMULATION_FLAG                         (*(__IO uint32_t *)0x40000c0c)
// USB_CTRL
#define SYS_USB_OTG_CID_EN_POS                      28
#define SYS_USB_OTG_CID_POS                         24
#define SYS_USB_VBUS_STATE_POS                      20
#define SYS_USB_PD_VBUS_DET_POS                     16
#define SYS_USB_BUS_ACTIVE_SYNC_POS                 12
#define SYS_USB_BUS_MON_EN_POS                      8
#define SYS_USB_SESS_CTRL_EN_POS                    7
#define SYS_USB_SESS_CTRL_POS                       4
#define SYS_USB_TEST_MODE_POS                       0
#define SYS_USB_OTG_CID_EN_MASK                     0x10000000
#define SYS_USB_OTG_CID_MASK                        0x01000000
#define SYS_USB_VBUS_STATE_MASK                     0x00700000
#define SYS_USB_PD_VBUS_DET_MASK                    0x00010000
#define SYS_USB_BUS_ACTIVE_SYNC_MASK                0x00001000
#define SYS_USB_BUS_MON_EN_MASK                     0x00000100
#define SYS_USB_SESS_CTRL_EN_MASK                   0x00000080
#define SYS_USB_SESS_CTRL_MASK                      0x00000070
#define SYS_USB_TEST_MODE_MASK                      0x00000001
// CHRGR_STAT
#define SYS_CHRGR_FINISH_MASK                       0x00000001
#define SYS_CHRGR_CHARGING_LARGE0_TRICKLE1_MASK     0x00000002
#define SYS_CHRGR_INSERT_DETECT_MASK                0x00000004
// PINMUX
#define SYS_PINMUX_MASK(f,p)                        (0x3F<<(p)),((f)<<(p))
// PINMUX0
#define SYS_PINMUX_GPIO_DIN_HOLD_SEL_MASK           0x00000080
#define SYS_PINMUX_SYSPLL_GT_CPUCLK_HW_CTRL_MASK    0x00800000


#endif  /* __SYS_REG_H */


/** @} */
