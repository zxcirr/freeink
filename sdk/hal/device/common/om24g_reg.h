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
 * @file     gpio_reg.h
 * @brief    GPIO Register for OnMicro OM66xx
 * @date     21 July 2020
 * @author   OnMicro SW Team
 *
 * @ingroup  REGS
 * @brief    GPIO Register for OnMicro OM66xx
 * @details  common GPIO Register definitions
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */
#ifndef __OM24G_REG_H
#define __OM24G_REG_H


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

#define OM24G_STATUS_RX_DR             0x80
#define OM24G_STATUS_TX_DS             0x40
#define OM24G_STATUS_MAX_RT            0x20
#define OM24G_STATUS_TIME_OUT          0x10
#define OM24G_STATUS_CRC_ERR           0x01

// PKTCTRL0 (00h)
#define OM24G_PKTCTRL0_PRIM_RX_POS          0
#define OM24G_PKTCTRL0_STRUCT_SEL_POS       1
#define OM24G_PKTCTRL0_ACK_TX_POSITION_POS  2
#define OM24G_PKTCTRL0_ADDR_CHK_POS         4
#define OM24G_PKTCTRL0_RX_0_1_RVS_POS       6
#define OM24G_PKTCTRL0_TX_0_1_RVS_POS       7
#define OM24G_PKTCTRL0_TIMESTAMP_POS        8
#define OM24G_PKTCTRL0_FORCE_CAL_POS        10
#define OM24G_PKTCTRL0_MAC_SEL_POS          11
#define OM24G_PKTCTRL0_CE_H_THRE_POS        12
#define OM24G_PKTCTRL0_NUM_HDR_BITS_POS     16

#define OM24G_PKTCTRL0_PRIM_RX_MASK          0x00000001
#define OM24G_PKTCTRL0_STRUCT_SEL_MASK       0x00000002
#define OM24G_PKTCTRL0_ACK_TX_POSITION_MASK  0x0000000C
#define OM24G_PKTCTRL0_ADDR_CHK_MASK         0x00000030
#define OM24G_PKTCTRL0_RX_0_1_RVS_MASK       0x00000040
#define OM24G_PKTCTRL0_TX_0_1_RVS_MASK       0x00000080
#define OM24G_PKTCTRL0_TIMESTAMP_MASK        0x00000100
#define OM24G_PKTCTRL0_FORCE_CAL_MASK        0x00000400
#define OM24G_PKTCTRL0_MAC_SEL_MASK          0x00000800
#define OM24G_PKTCTRL0_CE_H_THRE_MASK        0x0000F000
#define OM24G_PKTCTRL0_NUM_HDR_BITS_MASK     0x003F0000
        
// FB_PKTCTRL (04h)
#define OM24G_FB_PKTCTRL_NUM_ADDR1_BITS_POS   0
#define OM24G_FB_PKTCTRL_NUM_LEN_BITS_POS     8
#define OM24G_FB_PKTCTRL_ADDR1_POS_POS        16
#define OM24G_FB_PKTCTRL_LEN_POS_POS          24
#define OM24G_FB_PKTCTRL_ADDR1_LOC_POS        29

#define OM24G_FB_PKTCTRL_NUM_ADDR1_BITS_MASK  0x0000000F
#define OM24G_FB_PKTCTRL_NUM_LEN_BITS_MASK    0x00001F00
#define OM24G_FB_PKTCTRL_ADDR1_POS_MASK       0x001F0000
#define OM24G_FB_PKTCTRL_LEN_POS_MASK         0x1F000000
#define OM24G_FB_PKTCTRL_ADDR1_LOC_MASK       0x60000000

// MAC_EN(08h)
#define OM24G_CLK_EN_POS                    0

#define OM24G_CLK_EN_MASK                   0x00000001

// DYNPD (0Ch)
#define OM24G_DPL_P0_POS                    0
#define OM24G_DPL_P1_POS                    1
#define OM24G_DPL_P2_POS                    2
#define OM24G_DPL_P3_POS                    3
#define OM24G_DPL_P4_POS                    4
#define OM24G_DPL_P5_POS                    5
#define OM24G_DPL_P6_POS                    6
#define OM24G_DPL_P7_POS                    7

#define OM24G_DPL_P0_MASK                   0x00000001
#define OM24G_DPL_P1_MASK                   0x00000002
#define OM24G_DPL_P2_MASK                   0x00000004
#define OM24G_DPL_P3_MASK                   0x00000008
#define OM24G_DPL_P4_MASK                   0x00000010
#define OM24G_DPL_P5_MASK                   0x00000020
#define OM24G_DPL_P6_MASK                   0x00000040
#define OM24G_DPL_P7_MASK                   0x00000080

//FEATURE  (10h)
#define OM24G_EN_ACK_PAY_POS                0
#define OM24G_EN_DPL_POS                    1

#define OM24G_EN_ACK_PAY_MASK               0x00000001
#define OM24G_EN_DPL_MASK                   0x00000002

//INT_MASK (14h)
#define OM24G_MASK_CRC_ERR_POS               0
#define OM24G_MASK_BCC_MATCH_POS             1
#define OM24G_MASK_SYNC0_DET_POS             2
#define OM24G_MASK_SYNC1_DET_POS             3
#define OM24G_MASK_DEV_MATCH_POS             4
#define OM24G_MASK_MAX_RT_POS                5
#define OM24G_MASK_TX_DS_POS                 6
#define OM24G_MASK_RX_DR_POS                 7

#define OM24G_MASK_CRC_ERR_MASK              0x00000001
#define OM24G_MASK_BCC_MATCH_MASK            0x00000002
#define OM24G_MASK_SYNC0_DET_MASK            0x00000004
#define OM24G_MASK_SYNC1_DET_MASK            0x00000008
#define OM24G_MASK_DEV_MATCH_MASK            0x00000010
#define OM24G_MASK_MAX_RT_MASK               0x00000020
#define OM24G_MASK_TX_DS_MASK                0x00000040
#define OM24G_MASK_RX_DR_MASK                0x00000080

//INT_ST(18h)
#define OM24G_INT_CRC_ERR_POS               0
#define OM24G_INT_BCC_MATCH_POS             1
#define OM24G_INT_SYNC0_DET_POS             2
#define OM24G_INT_SYNC1_DET_POS             3
#define OM24G_INT_DEV_MATCH_POS             4
#define OM24G_INT_MAX_RT_POS                5
#define OM24G_INT_TX_DS_POS                 6
#define OM24G_INT_RX_DR_POS                 7

#define OM24G_INT_CRC_ERR_MASK              0x00000001
#define OM24G_INT_BCC_MATCH_MASK            0x00000002
#define OM24G_INT_SYNC0_DET_MASK            0x00000004
#define OM24G_INT_SYNC1_DET_MASK            0x00000008
#define OM24G_INT_DEV_MATCH_MASK            0x00000010
#define OM24G_INT_MAX_RT_MASK               0x00000020
#define OM24G_INT_TX_DS_MASK                0x00000040
#define OM24G_INT_RX_DR_MASK                0x00000080

//EN_AA(20h)
#define OM24G_ENAA_P0_POS                   0
#define OM24G_ENAA_P1_POS                   1
#define OM24G_ENAA_P2_POS                   2
#define OM24G_ENAA_P3_POS                   3
#define OM24G_ENAA_P4_POS                   4
#define OM24G_ENAA_P5_POS                   5
#define OM24G_ENAA_P6_POS                   6
#define OM24G_ENAA_P7_POS                   7
#define OM24G_ENAA_P0_MASK                  0x00000001
#define OM24G_ENAA_P1_MASK                  0x00000002
#define OM24G_ENAA_P2_MASK                  0x00000004
#define OM24G_ENAA_P3_MASK                  0x00000008
#define OM24G_ENAA_P4_MASK                  0x00000010
#define OM24G_ENAA_P5_MASK                  0x00000020
#define OM24G_ENAA_P6_MASK                  0x00000040
#define OM24G_ENAA_P7_MASK                  0x00000080

//EN_RXADDR(24h)
#define OM24G_ERX_P0_POS                   0
#define OM24G_ERX_P1_POS                   1
#define OM24G_ERX_P2_POS                   2
#define OM24G_ERX_P3_POS                   3
#define OM24G_ERX_P4_POS                   4
#define OM24G_ERX_P5_POS                   5
#define OM24G_ERX_P6_POS                   6
#define OM24G_ERX_P7_POS                   7
#define OM24G_ERX_P0_MASK                  0x00000001
#define OM24G_ERX_P1_MASK                  0x00000002
#define OM24G_ERX_P2_MASK                  0x00000004
#define OM24G_ERX_P3_MASK                  0x00000008
#define OM24G_ERX_P4_MASK                  0x00000010
#define OM24G_ERX_P5_MASK                  0x00000020
#define OM24G_ERX_P6_MASK                  0x00000040
#define OM24G_ERX_P7_MASK                  0x00000080

// FA_PKTCTRL(28h)
#define OM24G_FA_PKTCTRL_SETUP_AW_POS      0
#define OM24G_FA_PKTCTRL_BP_RX_ADDR_POS    1
#define OM24G_FA_PKTCTRL_NO_ACK_POS        2
#define OM24G_FA_PKTCTRL_EN_DYN_ACK_POS    3

#define OM24G_FA_PKTCTRL_SETUP_AW_MASK     0x00000001
#define OM24G_FA_PKTCTRL_BP_RX_ADDR_MASK   0x00000002
#define OM24G_FA_PKTCTRL_NO_ACK_MASK       0x00000004
#define OM24G_FA_PKTCTRL_EN_DYN_ACK_MASK   0x00000008

// FA_SETUP_RETR(2Ch)
#define OM24G_FA_SETUP_RETR_ARC_POS        0
#define OM24G_FA_SETUP_RETR_ARD_POS        4
#define OM24G_FA_SETUP_RETR_ARC_MASK       0x0000000F
#define OM24G_FA_SETUP_RETR_ARD_MASK       0x000000F0

// PREAMBLE (30h)
#define OM24G_PREAMBLE_POS                 0
#define OM24G_PREAMBLE_MASK                0x000000FF

// PREAMBLE_LEN (34h)
#define OM24G_PREAMBLE_LEN_POS             0
#define OM24G_PREAMBLE_LEN_MASK            0x0000000F

// PRE_GUARD (3Ch)
#define OM24G_PREGRD_CNT_POS              0
#define OM24G_PREGRD_EN_POS               4
#define OM24G_TAIL_CTL_POS                5
#define OM24G_GUARD_EN_POS                10
#define OM24G_TAIL_PATERN_POS             12
#define OM24G_PREGRD_CNT_MASK             0x0000000F
#define OM24G_PREGRD_EN_MASK              0x00000010
#define OM24G_TAIL_CTL_MASK               0x000003E0
#define OM24G_GUARD_EN_MASK               0x00000400
#define OM24G_TAIL_PATERN_MASK            0x00003000


//// PRE_GUARD (3Ch)
//#define OM24G_PREGRD_CNT_POS              0
//#define OM24G_PREGRD_EN_POS               4
//#define OM24G_TAIL_CTL_POS                5
//#define OM24G_GUARD_EN_POS                8
//#define OM24G_PREGRD_CNT_MASK             0x0000000F
//#define OM24G_PREGRD_EN_MASK              0x00000010
//#define OM24G_TAIL_CTL_MASK               0x000000E0
//#define OM24G_GUARD_EN_MASK               0x00000100

// SYNC_WORD0 (40h)
#define OM24G_SYNC_WORD0_POS              0
#define OM24G_SYNC_WORD0_MASK             0xFFFFFFFF

// SYNC_WORD1 (44h)
#define OM24G_SYNC_WORD1_POS              0
#define OM24G_SYNC_WORD1_MASK             0xFFFFFFFF

// TX_ADDR (48h)
#define OM24G_TX_ADDR_POS                 0
#define OM24G_TX_ADDR_MASK                0x000000FF

// SYNC_WORD_SEL (4Ch)
#define OM24G_SYNC_WORD_SEL_POS           0
#define OM24G_SYNC_WORD_SEL_MASK          0x00000001

// RX_ADDR_P0 (50h)
#define OM24G_RX_ADDR_P0_POS              0
#define OM24G_RX_ADDR_P0_MASK             0x000000FF

// RX_ADDR_P1 (54h)
#define OM24G_RX_ADDR_P1_POS              0
#define OM24G_RX_ADDR_P1_MASK             0x000000FF

// RX_ADDR_P2 (58h)
#define OM24G_RX_ADDR_P2_POS              0
#define OM24G_RX_ADDR_P2_MASK             0x000000FF

// RX_ADDR_P3 (5Ch)
#define OM24G_RX_ADDR_P3_POS              0
#define OM24G_RX_ADDR_P3_MASK             0x000000FF

// RX_ADDR_P4 (60h)
#define OM24G_RX_ADDR_P4_POS              0
#define OM24G_RX_ADDR_P4_MASK             0x000000FF

// RX_ADDR_P5 (64h)
#define OM24G_RX_ADDR_P5_POS              0
#define OM24G_RX_ADDR_P5_MASK             0x000000FF

// RX_ADDR_P6 (68h)
#define OM24G_RX_ADDR_P6_POS              0
#define OM24G_RX_ADDR_P6_MASK             0x000000FF

// RX_ADDR_P7 (6Ch)
#define OM24G_RX_ADDR_P7_POS              0
#define OM24G_RX_ADDR_P7_MASK             0x000000FF

// RF_DR(70h)
#define OM24G_RF_DR_TX_RF_DR_POS           0
#define OM24G_EN_TX_ARB_POS                2
#define OM24G_N_REP_POS                    4
#define OM24G_RF_DR_RX_RF_DR_POS           8
#define OM24G_EN_RX_ARB_POS                10
#define OM24G_N_AVR_POS                    12

#define OM24G_RF_DR_TX_RF_DR_MASK          0x00000003
#define OM24G_EN_TX_ARB_MASK               0x00000004
#define OM24G_N_REP_MASK                   0x000000F0
#define OM24G_RF_DR_RX_RF_DR_MASK          0x00000300
#define OM24G_EN_RX_ARB_MASK               0x00000400
#define OM24G_N_AVR_MASK                   0x0000F000

// RF_PD_AHEAD(74h)
#define OM24G_RF_PD_AHEAD_POS              0
#define OM24G_RF_PD_AHEAD_EN_POS           8

#define OM24G_RF_PD_AHEAD_MASK             0x0000007F
#define OM24G_RF_PD_AHEAD_EN_MASK          0x00000100

// ACK_PIPE_N (78h)
#define OM24G_ACK_PIPE_NO_POS             0
#define OM24G_ACK_PIPE_NO_MASK            0x00000007

// RX_P_NO (80h)
#define OM24G_RX_P_NO_POS                 0
#define OM24G_RX_P_NO_MASK                0x00000007

// OBSERVE_TX (84h)
#define OM24G_ARC_CNT_POS                 0
#define OM24G_PLOS_CNT_POS                4
#define OM24G_PLOS_CNT_CLR_POS            8
#define OM24G_ARC_CNT_MASK                0x0000000F
#define OM24G_PLOS_CNT_MASK               0x000000F0
#define OM24G_PLOS_CNT_CLR_MASK           0x00000100

// CRCCFG (88h)
#define OM24G_CRC_LEN_POS                 0
#define OM24G_CRC_EN_POS                  2
#define OM24G_CRC_MODE_POS                3

#define OM24G_CRC_LEN_MASK                0x00000003
#define OM24G_CRC_EN_MASK                 0x00000004
#define OM24G_CRC_MODE_MASK               0x00000008

// CRCPOLY (8Ch)
#define OM24G_CRCPOLY_POS                 0
#define OM24G_CRCPOLY_MASK                0xFFFFFFFF

// CRCINIT (90h)
#define OM24G_CRCINIT_POS                 0
#define OM24G_CRCINIT_MASK                0xFFFFFFFF

// CRCSKIP (94h)
#define OM24G_CRC_SKIP_SYNC_POS           0
#define OM24G_CRC_SKIP_LEN_POS            1
#define OM24G_CRC_SKIP_ADDR_POS           2
#define OM24G_CRC_SKIP_SYNC_MASK          0x00000001
#define OM24G_CRC_SKIP_LEN_MASK           0x00000002
#define OM24G_CRC_SKIP_ADDR_MASK          0x00000004

// WHITECFG  (A0h)
#define OM24G_WHITE_EN_POS                0
#define OM24G_WHITE_SKIP_HEADER_POS       1
#define OM24G_WHITE_SKIP_ADDR_POS         2
#define OM24G_WHITE_SKIP_CRC_POS          3
#define OM24G_WHITE_EN_MASK               0x00000001
#define OM24G_WHITE_SKIP_HEADER_MASK      0x00000002
#define OM24G_WHITE_SKIP_ADDR_MASK        0x00000004
#define OM24G_WHITE_SKIP_CRC_MASK         0x00000008

// WHITESEL (A4h)
#define OM24G_WHITE_SEL_POS               0
#define OM24G_WHITE_SEL_MASK              0x00000007

// WHITESEED (A8h)
#define OM24G_WHITE_SEED_POS              0
#define OM24G_WHITE_SEED_MASK             0x0000FFFF

// WHITEOUT (ACh)
#define OM24G_WHITE_OUT_POS               0
#define OM24G_WHITE_OUT_MASK              0x0000000F

// DMA_CMD (B0h)
#define OM24G_DMA_CMD_POS                 0
#define OM24G_DMA_CMD_MASK                0x0000FFFF

// DMA_TX_LEN (B4h)
#define OM24G_DMA_TX_LEN_POS              0
#define OM24G_DMA_TX_LEN_MASK             0x0000FFFF

// RX_DYN_LEN (B8h)
#define OM24G_RX_DYN_LEN_POS              0
#define OM24G_RX_DYN_LEN_MASK             0x0000FFFF

// DMA_TX_ADDR (BCh)
#define OM24G_DMA_TX_ADDR_POS             0
#define OM24G_DMA_TX_ADDR_MASK            0x0001FFFF

// DMA_RX_ADDR (C0h)
#define OM24G_DMA_RX_ADDR_POS             0
#define OM24G_DMA_RX_ADDR_MASK            0x0001FFFF

// PACKET_LEN (C4h)
#define OM24G_PACKET_LEN_POS              0
#define OM24G_PACKET_LEN_MASK             0x0000FFFF

// BCC (C8h)
#define OM24G_BCC_CNT_POS                 0
#define OM24G_BCC_CNT_MASK                0x0000FFFF

// TIMESTAMP_RT (D4h)
#define OM24G_TS_VALUE_RT_POS             0
#define OM24G_TS_VALUE_RT_MASK            0xFFFFFFFF

// TIMESTAMP_TRIGER (D8h)
#define OM24G_TS_TRIGER_POS               0
#define OM24G_TS_TRIGER_MASK              0xFFFFFFFF

// TIMESTAMP_CFG (DCh)
#define OM24G_TS_CNT_EN_POS               0
#define OM24G_TS_TRIGER_EN_POS            1
#define OM24G_TS_CNT_EN_MASK              0x00000001
#define OM24G_TS_TRIGER_EN_MASK           0x00000002

// SETUP_VALUE (E0h)
#define OM24G_RX_SETUP_VALUE_POS          0
#define OM24G_TX_SETUP_VALUE_POS          8
#define OM24G_RX_TM_CNT_POS               16
#define OM24G_RX_SETUP_VALUE_MASK         0x000000FF
#define OM24G_TX_SETUP_VALUE_MASK         0x0000FF00
#define OM24G_RX_TM_CNT_MASK              0x00FF0000


// ENDIAN  (E4h)
#define OM24G_EDIBIT_CRC_POS              0
#define OM24G_EDIBIT_PL_POS               1
#define OM24G_EDIBIT_SW_POS               3
#define OM24G_EDIBIT_ADDR_POS             4
#define OM24G_EDIBIT_HDR_POS              5
#define OM24G_EDIBYTE_CRC_POS             8
#define OM24G_EDIBYTE_SW_POS              9
#define OM24G_EDIBIT_CRC_MASK             0x00000001
#define OM24G_EDIBIT_PL_MASK              0x00000002
#define OM24G_EDIBIT_SW_MASK              0x00000008
#define OM24G_EDIBIT_ADDR_MASK            0x00000010
#define OM24G_EDIBIT_HDR_MASK             0x00000020
#define OM24G_EDIBYTE_CRC_MASK            0x00000100
#define OM24G_EDIBYTE_SW_MASK             0X00000200
// FLUSH  (E8h)
#define OM24G_TX_FIFO_FLUSH_POS           0
#define OM24G_RX_FIFO_FLUSH_POS           4
#define OM24G_TX_FIFO_FLUSH_MASK          0x00000001
#define OM24G_RX_FIFO_FLUSH_MASK          0x00000010

// TESTCTRL  (ECh)
#define OM24G_TEST_PAT_POS                0
#define OM24G_TEST_PAT_EN_POS             8
#define OM24G_PSUDO_RND_POS               9
#define OM24G_CONT_WAVE_POS               10
#define OM24G_FORCE_CRC_ERR_POS           11
#define OM24G_TEST_PAT_MASK               0x000000FF
#define OM24G_TEST_PAT_EN_MASK            0x00000100
#define OM24G_PSUDO_RND_MASK              0x00000200
#define OM24G_CONT_WAVE_MASK              0x00000400
#define OM24G_FORCE_CRC_ERR_MASK          0x00000800

// STATE (F0h)
#define OM24G_MAIN_STATE_POS              0
#define OM24G_CRC_OK_POS                  6
#define OM24G_TX_FIFO_EMPTY_POS           7
#define OM24G_MAIN_STATE_MASK             0x0000003F
#define OM24G_CRC_OK_MASK                 0x00000040
#define OM24G_TX_FIFO_EMPTY_MASK          0x00000080

// DBG_FREQ (F4h)
#define OM24G_FREQ_POS                    0
#define OM24G_FREQ_MASK                   0x00000FFF

#define OM24G_CE_LOW()                                                         \
    do {                                                                       \
        REGW0(&OM_24G->MAC_EN, OM24G_CLK_EN_MASK);                             \
    } while(0)

#define OM24G_CE_HIGH()                                                        \
    do {                                                                       \
        REGW1(&OM_24G->MAC_EN, OM24G_CLK_EN_MASK);                             \
    } while(0)

#define OM24G_FLUSH_TX_FIFO()                                                  \
    do {                                                                       \
        REGW1(&OM_24G->FLUSH, OM24G_TX_FIFO_FLUSH_MASK);                       \
    } while(0)

#define OM24G_FLUSH_RX_FIFO()                                                  \
    do {                                                                       \
        REGW1(&OM_24G->FLUSH, OM24G_RX_FIFO_FLUSH_MASK);                       \
    } while(0)

#define OM24G_CLEAR_ALL_IRQ()                                                  \
    do {                                                                       \
        REGW1(&OM_24G->INT_ST, 0XFF);                                          \
    } while(0)
/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    __IO uint32_t PKTCTRL0;                     // offset:0x00
    __IO uint32_t FB_PKTCTRL;                   // offset:0x04
    __IO uint32_t MAC_EN;                       // offset:0x08
    __IO uint32_t DYNPD;                        // offset:0x0C
    __IO uint32_t FEATURE;                      // offset:0x10
    __IO uint32_t INT_MASK;                     // offset:0x14
    __IO uint32_t INT_ST;                       // offset:0x18
         uint32_t RESERVED1;                    // offset:0x1C
    __IO uint32_t EN_AA;                        // offset:0x20
    __IO uint32_t EN_RXADDR;                    // offset:0x24
    __IO uint32_t FA_PKTCTRL;                   // offset:0x28
    __IO uint32_t FA_SETUP_RETR;                // offset:0x2C
    __IO uint32_t FA_OBSERVE_TX;                // offset:0x30
    __IO uint32_t PREAMBLE;                     // offset:0x34
    __IO uint32_t PREAMBLE_LEN;                 // offset:0x38
    __IO uint32_t PRE_GUARD;                    // offset:0x3C
    __IO uint32_t SYNC_WORD0;                   // offset:0x40
    __IO uint32_t SYNC_WORD1;                   // offset:0x44
    __IO uint32_t TX_ADDR;                      // offset:0x48
    __IO uint32_t SYNC_WORD_SEL;                // offset:0x4C
    __IO uint32_t RX_ADDR_P0;                   // offset:0x50
    __IO uint32_t RX_ADDR_P1;                   // offset:0x54
    __IO uint32_t RX_ADDR_P2;                   // offset:0x58
    __IO uint32_t RX_ADDR_P3;                   // offset:0x5C
    __IO uint32_t RX_ADDR_P4;                   // offset:0x60
    __IO uint32_t RX_ADDR_P5;                   // offset:0x64
    __IO uint32_t RX_ADDR_P6;                   // offset:0x68
    __IO uint32_t RX_ADDR_P7;                   // offset:0x6C
    __IO uint32_t RF_DR;                        // offset:0x70
    __IO uint32_t RF_PD_AHEAD;                  // offset:0x74
    __IO uint32_t RX_P_NO;                      // offset:0x78
         uint32_t RESERVED4;                    // offset:0x7C
    __IO uint32_t CRCCFG;                       // offset:0x80
    __IO uint32_t CRCPOLY;                      // offset:0x84
    __IO uint32_t CRCINIT;                      // offset:0x88
    __IO uint32_t CRCSKIP;                      // offset:0x8C
    __IO uint32_t RXCRC;                        // offset:0x90
    __IO uint32_t WHITECFG;                     // offset:0x94
    __IO uint32_t WHITESEL;                     // offset:0x98
    __IO uint32_t WHITESEED;                    // offset:0x9C
    __IO uint32_t WHITEOUT;                     // offset:0xA0
    __IO uint32_t DMA_CMD;                      // offset:0xA4
    __IO uint32_t DMA_TX_LEN;                   // offset:0xA8
         uint32_t RESERVED6[3];
    __IO uint32_t DMA_TX_ADDR;                  // offset:0xB8
         uint32_t RESERVED7[2];
    __IO uint32_t DMA_RX_ADDR;                  // offset:0xC4
    __IO uint32_t RX_DYN_LEN;                   // offset:0xC8
    __IO uint32_t PACKET_LEN;                   // offset:0xCC
    __IO uint32_t BCC;                          // offset:0xD0
    __IO uint32_t TIMESTAMP_RT;                 // offset:0xD4
    __I  uint32_t TIMESTAMP_TRIGER;             // offset:0xD8
    __IO uint32_t TIMESTAMP_CFG;                // offset:0xDC
    __IO uint32_t SETUP_VALUE;                  // offset:0xE0
    __IO uint32_t ENDIAN;                       // offset:0xE4
    __IO uint32_t FLUSH;                        // offset:0xE8
    __IO uint32_t TESTCTRL;                     // offset:0xEC
    __I  uint32_t STATE;                        // offset:0xF0
    __IO uint32_t FB_HEADER;                    // offset:0xF4
    __IO uint32_t PMU_CNT_32K;                  // offset:0xF8
    __IO uint32_t PMU_CNT_4M;                   // offset:0xFC
    __IO uint32_t ACK_MODE;                     // offset:0x100
         uint32_t RESERVED8[7];
    __IO uint32_t FPGA_TX_INDEX;                // offset:0x120
    __IO uint32_t DBG_SPI0;                     // offset:0x124
    __IO uint32_t DBG_SPI1;                     // offset:0x128
    
} OM_24G_Type;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif


#endif  /* __GPIO_REG_H */


/** @} */

