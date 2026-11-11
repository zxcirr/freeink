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
 * @file     dma_reg.h
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

#ifndef __DMA_REG_H
#define __DMA_REG_H


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
//ChCtrl
#define DMA_ENABLE_POS                              0
#define DMA_INTTCMASK_POS                           1
#define DMA_INTERRMASK_POS                          2
#define DMA_INTABTMASK_POS                          3
#define DMA_DSTREQSEL_POS                           4
#define DMA_SRCREQSEL_POS                           8
#define DMA_DSTADDRCTRL_POS                         12
#define DMA_SRCADDRCTRL_POS                         14
#define DMA_DSTMODE_POS                             16
#define DMA_SRCMODE_POS                             17
#define DMA_DSTWIDTH_POS                            18
#define DMA_SRCWIDTH_POS                            20
#define DMA_SRCBURSTSIZE_POS                        22
#define DMA_PRIORITY_POS                            29
#define DMA_ENABLE_MASK                             0x00000001
#define DMA_INTTCMASK_MASK                          0x00000002
#define DMA_INTERRMASK_MASK                         0x00000004
#define DMA_INTABTMASK_MASK                         0x00000008
#define DMA_DSTREQSEL_MASK                          0x000000F0
#define DMA_SRCREQSEL_MASK                          0x00000F00
#define DMA_DSTADDRCTRL_MASK                        0x00003000
#define DMA_SRCADDRCTRL_MASK                        0x0000C000
#define DMA_DSTMODE_MASK                            0x00010000
#define DMA_SRCMODE_MASK                            0x00020000
#define DMA_DSTWIDTH_MASK                           0x000C0000
#define DMA_SRCWIDTH_MASK                           0x00300000
#define DMA_SRCBURSTSIZE_MASK                       0x01C00000
#define DMA_PRIORITY_MASK                           0x20000000
//IDREV
#define DMA_REVMINJOR_POS                           0
#define DMA_REVMAJOR_POS                            4
#define DMA_ID_POS                                  12
#define DMA_REVMINJOR_MASK                          0x0000000F
#define DMA_REVMAJOR_MASK                           0x00000FF0
#define DMA_ID_MASK                                 0xFFFFF000
//DMACFG
#define DMA_CHANNELNUM_POS                          0
#define DMA_FIFIODEPTH_POS                          4
#define DMA_REQNUM_POS                              10
#define DMA_REQSYNC_POS                             30
#define DMA_CHAINXFR_POS                            31
#define DMA_CHANNELNUM_MASK                         0x0000000F
#define DMA_FIFIODEPTH_MASK                         0x000003F0
#define DMA_REQNUM_MASK                             0x00007C00
#define DMA_REQSYNC_MASK                            0x40000000
#define DMA_CHAINXFR_MASK                           0x80000000
//DMACTRL
#define DMA_RESET_POS                               0
#define DMA_RESET_MASK                              0x00000001
//Status
#define DMA_ERROR_POS                               0
#define DMA_ABORT_POS                               8
#define DMA_TC_POS                                  16
#define DMA_ERROR_MASK                              0x000000FF
#define DMA_ABORT_MASK                              0x0000FF00
#define DMA_TC_MASK                                 0x00FF0000

// DMA Interrupt Status register
#define DMA_INT_STATUS_ERROR_POS(chan_idx)          (chan_idx)
#define DMA_INT_STATUS_ERROR_MASK(chan_idx)         (1U << chan_idx)
#define DMA_INT_STATUS_ABORT_POS(chan_idx)          (8U + (chan_idx))
#define DMA_INT_STATUS_ABORT_MASK(chan_idx)         (1U << (8 + (chan_idx)))      /* abort status, one bit per channel */
#define DMA_INT_STATUS_TC_POS(chan_idx)             (16U + (chan_idx))
#define DMA_INT_STATUS_TC_MASK(chan_idx)            (1U << (16U + (chan_idx)))    /* terminal count status of DMA channels, one bit per channel */
#define DMA_INT_STATUS_ALL(chan_idx)                (DMA_INT_STATUS_ERROR_MASK(chan_idx) |  \
                                                     DMA_INT_STATUS_ABORT_MASK(chan_idx) |   \
                                                     DMA_INT_STATUS_TC_MASK(chan_idx))



/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    __IO uint32_t CTRL;          // offset address 0x44+n*0x14, channel n control register
    __IO uint32_t SRC_ADDR;      // offset address 0x48+n*0x14, channel n source address register
    __IO uint32_t DST_ADDR;      // offset address 0x4C+n*0x14, channel n destination address register
    __IO uint32_t TRANS_SIZE;    // offset address 0x50+n*0x14, channel n transfer size register
    __IO uint32_t LL_PTR;        // offset address 0x54+n*0x14, channel n linker list pointer register
} __IO OM_DMA_CHAN_Type;

typedef struct {
         uint8_t  Reserved00_1c[0x1c - 0x00 + 4];
    __O  uint32_t CTRL;                              // offset:0x20
         uint8_t  Reserved24_2c[0x2c - 0x24 + 4];
    __IO uint32_t INT_STATUS;                        // offset:0x30
    __I  uint32_t CHAN_EN;                           // offset:0x34
         uint8_t  Reserved38_3c[0x3c - 0x38 + 4];
    __O  uint32_t CHAN_ABORT;                        // offset:0x40
         OM_DMA_CHAN_Type CHAN[8];                   // offset:0x44
    __O  uint32_t LLP_SHADOW[8];                     // offset address 0xE4. when transfer is completed, llp_shadow will be NULL
} OM_DMA_Type;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif

#endif  /* __DMA_REG_H */


/** @} */
