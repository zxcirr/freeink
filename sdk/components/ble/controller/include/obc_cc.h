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
 * DISCLAIMED. IN NO EVENT AESLL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * -------------------------------------------------------------------------- */

/**
 * @file     obc_cc.h
 * @brief    obc_cc
 * @date     15 December 2021
 * @author   OnMicro SW Team
 *
 * @defgroup OBC_CC OBC_CC
 * @ingroup  OBC
 * @brief    Onmicro BLE controller config
 * @details  Onmicro BLE controller config

 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __SC_H__
#define __SC_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>
#include "features.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/// stack config
typedef struct
{
    /// LP clock (32k) drift (ppm)
    uint16_t    lpclk_drift;
    /// Wakeup time
    uint8_t     pre_wakeup_time;
    /// Prog delay
    uint8_t     llt_prog_delay;
    /// Min prog delay
    uint8_t     llt_min_prog_delay;
    /// Min sleep space
    uint8_t     min_sleep_space;
    /// Use fixed p256key
    bool        dbg_fixed_p256_key;
    /// Channel selection algorithm #2
    bool        chsel2;
    /// Connection move enable
    bool        con_move_en;
    /// Enable channel assessment usage when building channel maps
    bool        ch_ass_en;
    /// Extended scanning
    bool        ext_scan;
    /// Default Scan event duration (in 31.25us)
    uint16_t    scan_evt_dur_dft;

#ifdef CONFIG_LE_LL_CODED_PHY
    bool        coded_phy_500k;
#endif
}obc_cc_t;

/*********************************************************************
 * EXTERN VARIABLES
 */
extern obc_cc_t obcc;

/*********************************************************************
 * EXTERN FUNCTIONS
 */


#ifdef __cplusplus
}
#endif

#endif

/** @} */

