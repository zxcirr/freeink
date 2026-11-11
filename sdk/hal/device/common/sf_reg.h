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
 * @file     sf_reg.h
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

#ifndef __SF_REG_H
#define __SF_REG_H


/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include "common_reg.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
// SF CPM
#define CPM_SF_DIV_COEFF_POS                8
#define CPM_SF_SOFT_RESET_POS               4
#define CPM_SF_DIV_SEL_POS                  2
#define CPM_SF_DIV_EN_POS                   1
#define CPM_SF_GATE_EN_POS                  0
#define CPM_SF_DIV_COEFF_MASK               0x0000FF00
#define CPM_SF_SOFT_RESET_MASK              0x00000010
#define CPM_SF_DIV_SEL_MASK                 0x00000004
#define CPM_SF_DIV_EN_MASK                  0x00000002
#define CPM_SF_GATE_EN_MASK                 0x00000001

// CONFIGURATION.CTRL
#define DRV_SF_CTRL_DMA_SEL_POS          27
#define DRV_SF_CTRL_BYTE_RW_EN_POS       26
#define DRV_SF_CTRL_ALGIN_3BYTE_EN_POS   25
#define DRV_SF_CTRL_DMA_DIRECTY_POS      24
#define DRV_SF_CTRL_LCD_RD_EN_POS        23
#define DRV_SF_CTRL_RGB_MODE_POS         21
#define DRV_SF_CTRL_LCD_SPI_CTRL_POS     18
#define DRV_SF_CTRL_WIDTH_POS            16
#define DRV_SF_CTRL_FE_DLY_SAMPLE_POS    14
#define DRV_SF_CTRL_DLY_SAMPLE_POS       12
#define DRV_SF_CTRL_DLYX_SAMPLE_POS      12
#define DRV_SF_CTRL_BP_CLOCK_DIV_POS     10
#define DRV_SF_CTRL_CPOL_POS             9
#define DRV_SF_CTRL_CPHA_POS             8
#define DRV_SF_CTRL_MODE_POS             8
#define DRV_SF_CTRL_CLOCK_DIV_POS        0
#define DRV_SF_CTRL_DMA_SEL_MASK         0x08000000
#define DRV_SF_CTRL_BYTE_RW_EN_MASK      0x04000000
#define DRV_SF_CTRL_ALGIN_3BYTE_EN_MASK  0x02000000
#define DRV_SF_CTRL_DMA_DIRECTY_MASK     0x01000000
#define DRV_SF_CTRL_LCD_RD_EN_MASK       0x00800000
#define DRV_SF_CTRL_RGB_MODE_MASK        0x00600000
#define DRV_SF_CTRL_LCD_SPI_CTRL_MASK    0x001C0000
#define DRV_SF_CTRL_WIDTH_MASK           0x00030000
#define DRV_SF_CTRL_DLYX_SAMPLE_MASK     0x00007000
#define DRV_SF_CTRL_FE_DLY_SAMPLE_MASK   0x00004000
#define DRV_SF_CTRL_DLY_SAMPLE_MASK      0x00003000
#define DRV_SF_CTRL_BP_CLOCK_DIV_MASK    0x00000400
#define DRV_SF_CTRL_CPOL_MASK            0x00000200
#define DRV_SF_CTRL_CPHA_MASK            0x00000100
#define DRV_SF_CTRL_MODE_MASK            0x00000300
#define DRV_SF_CTRL_CLOCK_DIV_MASK       0x000000FF
// CONFIGURATION.CS
#define DRV_SF_CS_RECOVER_POS            24
#define DRV_SF_CS_HOLD_POS               16
#define DRV_SF_CS_SETUP_POS              8
#define DRV_SF_CS_POL_POS                0
#define DRV_SF_CS_RECOVER_MASK           0xFF000000
#define DRV_SF_CS_HOLD_MASK              0x00FF0000
#define DRV_SF_CS_SETUP_MASK             0x0000FF00
#define DRV_SF_CS_POL_MASK               0x00000001
// SW_SPI_CFG0_REG
#define DRV_SF_SW_CFG0_DUMMY_CYCLE_CNT_POS     24
#define DRV_SF_SW_CFG0_CMD_P1_BIT_CNT_POS      16
#define DRV_SF_SW_CFG0_CMD_P1_BUS_WIDTH_POS    12
#define DRV_SF_SW_CFG0_CMD_P0_BUS_WIDTH_POS    8
#define DRV_SF_SW_CFG0_CMD_P0_BIT_CNT_POS      0
#define DRV_SF_SW_CFG0_DUMMY_CYCLE_CNT_MASK    0xFF000000
#define DRV_SF_SW_CFG0_CMD_P1_BIT_CNT_MASK     0x003F0000
#define DRV_SF_SW_CFG0_CMD_P1_BUS_WIDTH_MASK   0x00003000
#define DRV_SF_SW_CFG0_CMD_P0_BUS_WIDTH_MASK   0x00000300
#define DRV_SF_SW_CFG0_CMD_P0_BIT_CNT_MASK     0x0000007F
// SW_SPI_CFG1_REG
#define DRV_SF_SW_CFG1_SW_CFG_EN_POS           31
#define DRV_SF_SW_CFG1_BUF_WIDTH_BYTES_POS     24
#define DRV_SF_SW_CFG1_SDATA_BUS_WIDTH_POS     20
#define DRV_SF_SW_CFG1_SDATA_BYTE_CNT_POS      0
#define DRV_SF_SW_CFG1_SW_CFG_EN_MASK          0x80000000
#define DRV_SF_SW_CFG1_BUF_WIDTH_BYTES_MASK    0x07000000
#define DRV_SF_SW_CFG1_SDATA_BUS_WIDTH_MASK    0x00300000
#define DRV_SF_SW_CFG1_SDATA_BYTE_CNT_MASK     0x000FFFFF
// DELAY_CTRL
#define DRV_SF_DELAY_EXTERNAL_POS              4
#define DRV_SF_DELAY_INNER_POS                 0
#define DRV_SF_DELAY_EXTERNAL_MASK             0x000000F0
#define DRV_SF_DELAY_INNER_MASK                0x0000000F
// CIPHER_CTRL
#define DRV_SF_CIPHER_DECIPHER_EN_POS           1
#define DRV_SF_CIPHER_CIPHER_EN_POS             0
#define DRV_SF_CIPHER_DECIPHER_EN_MASK          0x00000002
#define DRV_SF_CIPHER_CIPHER_EN_MASK            0x00000001

/*******************************************************************************
 * TYPEDEFS
 */
typedef struct
{
    __I  uint32_t INTR_STATUS;              // offset:0x00
    __IO uint32_t RAW_INTR_STATUS;          // offset:0x04
    __IO uint32_t INTR_MASK;                // offset:0x08
    __IO uint32_t COMMAND;                  // offset:0x0C
    __IO uint32_t COMMAND_DATA0_REG;        // offset:0x10
    __IO uint32_t COMMAND_DATA1_REG;        // offset:0x14
    __IO uint32_t READ0_REG;                // offset:0x18
    __IO uint32_t READ1_REG;                // offset:0x1C
    __IO uint32_t ADDRESS_REG;              // offset:0x20
    __IO uint32_t READ_OPCODE_REG;          // offset:0x24  (Auto Restore)
    struct {
        __IO uint32_t CTRL;                 // offset:0x28,0x30  (Auto Restore)
        __IO uint32_t CS;                   // offset:0x2C,0x34
    } CONFIGURATION[2];
    __IO uint32_t TRANS_REMAP_REG;          // offset:0x38
    __IO uint32_t WP_HOLD_REG;              // offset:0x3C
    __IO uint32_t SW_SPI_CFG0_REG;          // offset:0x40
    __IO uint32_t SW_SPI_CFG1_REG;          // offset:0x44
         uint32_t RESERVED0;                // offset:0x48
    __IO uint32_t DBG;                      // offset:0x4C
    __IO uint32_t DELAY_CTRL;               // offset:0x50  (Auto Restore)
    __IO uint32_t CIPHER_CTRL;              // offset:0x54  (Auto Restore)
} OM_SF_Type;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif

#endif  /* __SF_REG_H */


/** @} */
