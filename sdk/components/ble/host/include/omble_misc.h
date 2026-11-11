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
 * @file     omble_misc.h
 * @date     27. May 2021
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @addtogroup OMBLE_MISC
 * @brief MISC
 */
/// @{

#ifndef __OMBLE_MISC_H__
#define __OMBLE_MISC_H__
#include <stdint.h>
#include "omble_range.h"

/// omble 参数类型
enum ob_opt_cfg_type {
    OB_OPT_CFG_TYPE_GAP_MAX_EXT_ADV_EVENT                   = 0,
    OB_OPT_CFG_TYPE_GAP_ADV_SECD_MAX_SKIP                   = 1,
    OB_OPT_CFG_TYPE_GAP_ADV_SCAN_REQ_NTF_EN                 = 2,
    OB_OPT_CFG_TYPE_GAP_CE_LENGTH                           = 3,
    OB_OPT_CFG_TYPE_GAP_PIN_NONE_REQ_ENABLE                 = 4,
    OB_OPT_CFG_TYPE_SMP_PIN_CODE                            = 5,
    OB_OPT_CFG_TYPE_SMP_PRIVATE_KEY                         = 6,
};

/// omble gap_ce_length
struct gap_ce_length {
    uint16_t min; ///< ce length 最小值
    uint16_t max; ///< ce length 最大值
};

/// omble 参数结构体
typedef union {
    uint8_t gap_max_ext_adv_event;      ///< 扩展广播最大广播事件发送数
    uint8_t gap_max_adv_secd_max_skip;  ///< 扩展广播最大广播skip数
    uint8_t gap_adv_scan_req_ntf_en;    ///< 是否开启扩展广播中的scan notify事件
    struct gap_ce_length gap_ce_length; ///< 配置ce length 参数
    uint8_t gap_pin_none_req_enable;    ///< 是否在NO_INPUT_NO_OUTPUT模式配对时上报OB_GAP_EVT_PIN_REQUEST消息
    struct {                            ///< 配对时的pin code, 只写
        uint8_t conn_idx;
        int pin_code;
    } smp_pin_code;
    uint8_t smp_private_key[32];        ///< Private key, 只写
} ob_opt_cfg_t;

#endif /* __OMBLE_MISC_H__ */

/// @}
