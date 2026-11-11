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
 * @file     omble_gap.h
 * @date     27. May 2021
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @addtogroup OMBLE_GAP
 * @brief GAP
 */
/// @{

#ifndef __OMBLE_GAP_H__
#define __OMBLE_GAP_H__
#include <stdint.h>
#include <stdbool.h>
#include "omble_range.h"

/// OB_GAP_EVENTS
enum OB_GAP_EVENTS {
    /// 连接成功事件，参考 @ref ob_gap_evt_connected_t
    OB_GAP_EVT_CONNECTED              = OB_GAP_EVTS_BASE,
    /// 连接断开事件，参考 @ref ob_gap_evt_disconnected_t
    OB_GAP_EVT_DISCONNECTED           = OB_GAP_EVTS_BASE + 1,
    /// 广播状态变化事件，参考 @ref ob_gap_evt_adv_state_changed_t
    OB_GAP_EVT_ADV_STATE_CHANGED      = OB_GAP_EVTS_BASE + 2,
    /// 参数更新事件，参考 @ref ob_gap_evt_conn_params_update_t
    OB_GAP_EVT_CONN_PARAMS_UPDATE     = OB_GAP_EVTS_BASE + 3,
    /// peripherial参数更新请求，参考 @ref ob_gap_evt_conn_params_request_t
    OB_GAP_EVT_CONN_PARAMS_REQUEST    = OB_GAP_EVTS_BASE + 4,
    /// 广播上报事件，参考 @ref ob_gap_evt_adv_report_t
    OB_GAP_EVT_ADV_REPORT             = OB_GAP_EVTS_BASE + 5,
    /// 链路加密状态改变事件，参考 @ref ob_gap_evt_encrypt_t
    OB_GAP_EVT_ENCRYPT                = OB_GAP_EVTS_BASE + 6,
    /// 加密请求事件，仅central角色支持，参考 @ref ob_gap_evt_sec_request_t
    OB_GAP_EVT_SEC_REQUEST            = OB_GAP_EVTS_BASE + 7,
    /// 配对请求事件，仅peripheral角色支持，参考 @ref ob_gap_evt_pairing_request_t
    OB_GAP_EVT_PAIRING_REQUEST        = OB_GAP_EVTS_BASE + 8,
    /// 配对PIN码请求事件，参考 @ref ob_gap_evt_pin_request_t
    OB_GAP_EVT_PIN_REQUEST            = OB_GAP_EVTS_BASE + 9,
    /// LTK请求，仅peripheral角色支持，参考 @ref ob_gap_evt_ltk_request_t
    OB_GAP_EVT_LTK_REQUEST            = OB_GAP_EVTS_BASE + 10,
    /// 绑定信息请求，参考 @ref ob_gap_evt_bond_info_request_t
    OB_GAP_EVT_BOND_INFO_REQUEST      = OB_GAP_EVTS_BASE + 11,
    /// 配对完成事件，参考 @ref ob_gap_evt_bonded_t
    OB_GAP_EVT_BONDED                 = OB_GAP_EVTS_BASE + 12,
    /// 配对信息上报事件（LTK/IRK），参考 @ref ob_gap_evt_bond_info_t
    OB_GAP_EVT_BOND_INFO              = OB_GAP_EVTS_BASE + 13,
    /// 超时事件（scan），参考 @ref ob_gap_evt_timeout_t
    OB_GAP_EVT_TIMEOUT                = OB_GAP_EVTS_BASE + 14,
    /// 功率变化事件，参考 @ref ob_gap_evt_power_changed_t
    OB_GAP_EVT_POWER_CHANGED          = OB_GAP_EVTS_BASE + 15,
    /// PHY变化事件，参考 @ref ob_gap_evt_phy_update_t
    OB_GAP_EVT_PHY_UPDATE             = OB_GAP_EVTS_BASE + 16,
    /// Data Length变化事件，参考 @ref ob_gap_evt_data_len_changed_t
    OB_GAP_EVT_DATA_LEN_CHANGED       = OB_GAP_EVTS_BASE + 17,
    /// 对端设备信息，参考 @ref ob_gap_evt_device_info_t
    OB_GAP_EVT_DEVICE_INFO            = OB_GAP_EVTS_BASE + 18,
    /// HCI指令错误，参考 @ref ob_gap_evt_hci_error_t
    OB_GAP_EVT_HCI_ERROR              = OB_GAP_EVTS_BASE + 0xFF,
};

enum ob_adv_properties_bits {
    OB_ADV_PROP_BIT0_CONNECTALBE      = 1 << 0, ///< 可连接
    OB_ADV_PROP_BIT1_SCANNABLE        = 1 << 1, ///< 响应scan request
    OB_ADV_PROP_BIT2_DIRECTED_ADV     = 1 << 2, ///< 直接广播
    OB_ADV_PROP_BIT3_HIGH_DUTY_CYCLE  = 1 << 3, ///< 高速直接广播
    OB_ADV_PROP_BIT4_LEGACY_ADV       = 1 << 4, ///< 传统广播
    OB_ADV_PROP_BIT5_OMIT_ADV_ADDRESS = 1 << 5, ///< 匿名广播
    OB_ADV_PROP_BIT6_INCLUDE_TX_POWER = 1 << 6, ///< 包含Tx Power
};

/// adv广播属性
enum ob_adv_properties {
    OB_ADV_PROP_LEGACY_IND               = 0x13, ///< 传统广播
    OB_ADV_PROP_LEGACY_DIRECT_IND_LOW    = 0x15, ///< 低速直连传统广播
    OB_ADV_PROP_LEGACY_DIRECT_IND_HIGH   = 0x1D, ///< 高速直连传统广播
    OB_ADV_PROP_LEGACY_SCAN_IND          = 0x12, ///< 不可连接传统广播
    OB_ADV_PROP_LEGACY_NONCONN_IND       = 0x10, ///< 不可连接不可扫描传统广播
    OB_ADV_PROP_EXT_CONN_NONSCAN         = 0x01, ///< 不可扫描扩展广播
    OB_ADV_PROP_EXT_NONCONN_SCAN         = 0x02, ///< 不可连接可扫描扩展广播
    OB_ADV_PROP_EXT_NONCONN_NONSCAN      = 0x00, ///< 不可连接不可扫描扩展广播
    OB_ADV_PROP_EXT_ANONYMOUS            = 0x20, ///< 匿名扩展广播
};

/// 广播白名单设置
enum ob_adv_filter_policy {
    OB_ADV_FILTER_NONE,
    OB_ADV_FILTER_SCAN,
    OB_ADV_FILTER_CONN,
    OB_ADV_FILTER_CONN_SCAN,
};

/// 连接白名单设置
enum ob_conn_filter_policy {
    OB_CONN_FILTER_NOT_USE,
    OB_CONN_FILTER_USE,
};

/// 扫描白名单设置
enum ob_scan_filter_policy {
    OB_SCAN_FILTER_BASIC_UNFILTER,
    OB_SCAN_FILTER_BASIC_FILTER,
    OB_SCAN_FILTER_EXT_UNFILTER,
    OB_SCAN_FILTER_EXT_FILTER,
};

/// 广播信道
enum ob_adv_channel {
    OB_ADV_CH_37  = 0x01,
    OB_ADV_CH_38  = 0x02,
    OB_ADV_CH_39  = 0x04,
    OB_ADV_CH_ALL = 0x07,
};

/// 广播地址类型
enum ob_adv_addr_type {
    OB_ADV_ADDR_TYPE_PUBLIC,
    OB_ADV_ADDR_TYPE_RANDOM,
};

/// 广播PHY
enum ob_adv_phy {
    OB_ADV_PHY_1M = 0x01,
    OB_ADV_PHY_2M = 0x02,
    OB_ADV_PHY_CODED = 0x03,
};

/// 绑定信息类型
enum ob_bond_info_type {
    OB_BOND_INFO_LTK,
    OB_BOND_INFO_IRK,
};

/// 配对pin 类型
enum ob_smp_io_capability {
    OB_SMP_IOCAP_DIS_ONLY, // DisplayOnly
    OB_SMP_IOCAP_DISP_YN,  // DisplayYesNo
    OB_SMP_IOCAP_KBD_ONLY, // KeyboardOnly
    OB_SMP_IOCAP_NONE,     // NoInputNoOutput
    OB_SMP_IOCAP_KDB_DISP, // KeyboardDisplay
};

/// 配对pin 类型
enum ob_smp_pin_type {
    OB_SMP_PIN_TYPE_NONE,     // 无显示,无输入
    OB_SMP_PIN_TYPE_OOB_REQ,  // 请求oob输入
    OB_SMP_PIN_TYPE_DIS_YN,   // 显示pin码,并请求确认
    OB_SMP_PIN_TYPE_PK_REQ,   // 请求pin输入
    OB_SMP_PIN_TYPE_DIS,      // 显示pin码
};

/// 连接角色
enum ob_gap_role {
    OB_GAP_ROLE_CENTRAL,      ///< central
    OB_GAP_ROLE_PHERIPHERAL,  ///< peripheral
};

/// 分发密钥类型
enum ob_smp_distribution {
    OB_SMP_DIST_BIT_ENC_KEY = 0x01, ///< EncKey
    OB_SMP_DIST_BIT_ID_KEY  = 0x02, ///< IdKey
};

/// 超时原因
enum ob_gap_timeout_source_t {
    OB_GAP_TOUT_SCAN, ///< 扫描超时
    OB_GAP_TOUT_CONN_PARAM_UPDATE, ///< 参数更新超时
};

/// 设备信息类型
enum ob_dev_info_type {
    OB_DEV_INFO_PEER_VERSION,      ///< 对端设备版本信息
    OB_DEV_INFO_PEER_LE_FEATURE,   ///< 对端设备feature信息
    OB_DEV_INFO_LOCAL_VERSION,     ///< 当前设备版本信息
    OB_DEV_INFO_LOCAL_LE_FEATURE,  ///< 当前设备feature信息
    OB_DEV_INFO_RSSI,              ///< 指定连接的RSSI
    OB_DEV_INFO_CHANNEL_MAP,       ///< 指定连接的ChannelMap信息
};

#define OB_GAP_ADDR_LEN   6   ///< 地址长度
#define OB_GAP_RANDOM_LEN 8   ///< random值长度
#define OB_GAP_KEY_LEN    16  ///< key长度
#define OB_GAP_ADV_TX_POWER_NO_AVA 0x7F ///< 发射功率不可用

/// GAP地址
typedef struct ob_gap_addr {
    uint8_t addr_type;              ///< See @ref ob_adv_addr_type.
    uint8_t addr[OB_GAP_ADDR_LEN];  ///< 48-bit address, LSB format.
} ob_gap_addr_t;

/// 广播参数
typedef struct ob_adv_param {
    uint32_t prim_intv_min;   ///< unit of 0.625ms
    uint32_t prim_intv_max;   ///< unit of 0.625ms
    uint16_t adv_properties;  ///< @ref ob_adv_properties
    uint8_t own_addr_type;    ///< @ref ob_adv_addr_type
    uint8_t prim_ch_map;      ///< @ref ob_adv_channel
    ob_gap_addr_t *peer_addr; ///< peer address
    uint8_t filter_policy;    ///< @ref ob_adv_filter_policy
    uint8_t prim_phy;         ///< @ref ob_adv_phy, OB_ADV_PHY_2M not support in primary phy
    uint8_t secd_phy;         ///< @ref ob_adv_phy
    int8_t tx_pwr;            ///< -127 ~ +20, 0x7F: Host has no preference
    uint16_t timeout;         ///< 超时时间 单位 10ms，0表示无超时时间
    uint8_t *local_addr;      ///< 本地地址
} ob_adv_param_t;

/// 广播数据/扫描数据
typedef struct ob_data {
    uint8_t *data;       ///< 数据
    int len;             ///< 数据长度
} ob_data_t;

/// 连接参数
typedef struct {
    uint16_t conn_intv;      ///< 连接间隔
    uint16_t latency_max;    ///< latency最大值
    uint16_t timeout;        ///< 连接超时时间
} ob_gap_conn_params_t;

/// 绑定信息
typedef struct {
    uint8_t type; ///< @ref ob_bond_info_type
    union {
        /// Identical Address和IRK
        struct {
            ob_gap_addr_t id_addr;
            uint8_t irk[OB_GAP_KEY_LEN];
        } id_info;
        /// EDIV, Random和LTK
        struct {
            uint16_t ediv;
            uint8_t random[OB_GAP_RANDOM_LEN];
            uint8_t ltk[OB_GAP_KEY_LEN];
        } enc_info;
    };
} ob_bond_info_t;

/// 回复的pin码
typedef struct {
    uint8_t type;                      ///< @ref ob_smp_pin_type
    union {
        uint8_t oob[OB_GAP_KEY_LEN];   ///< OOB数据
        uint32_t passkey;              ///< 按键数据
    };
} ob_smp_pin_t;

/// 扩展广播状态
enum ob_gap_adv_state {
    OB_GAP_ADV_ST_STARTED,              ///< 广播已开启
    OB_GAP_ADV_ST_STOPPED_BY_USER,      ///< 用户手动关闭广播
    OB_GAP_ADV_ST_STOPPED_BY_CONNECTED, ///< 连接成功
    OB_GAP_ADV_ST_STOPPED_BY_TIMEOUT,   ///< 广播超时
    OB_GAP_ADV_ST_STOPPED_BY_EVENT,     ///< 达到了Max_Extended_Advertising_Events计数
    OB_GAP_ADV_ST_STOPPED_UNEXPECTED,   ///< 未知原因关闭了广播
};

/// 发射功率等级标记
enum ob_gap_tx_pwr_level_flag {
    OB_GAP_TX_PWR_LEVEL_MIN = 0x01,              ///< 发射功率达到最小值
    OB_GAP_TX_PWR_LEVEL_MAX = 0x02,              ///< 发射功率达到最大值
};

/// 广播数据状态
enum ob_gap_report_state {
    OB_GAP_REPORT_ST_COMPLETE,          ///< Complete
    OB_GAP_REPORT_ST_MORE_DATA,         ///< Incomplete, more data to come
    OB_GAP_REPORT_ST_TRUNCATED,         ///< Incomplete, data truncated, no more to come
};

/// Event structure for @ref OB_GAP_EVT_CONNECTED.
typedef struct {
    uint8_t role;                       ///< 连接角色，参考@ref ob_gap_role
    uint8_t adv_idx;                    ///< 当前连接对应的广播index，若role为Central则该参数无效
    ob_gap_addr_t peer_addr;            ///< 对方地址，参考@ref ob_gap_addr_t
    ob_gap_conn_params_t conn_params;   ///< 连接参数，参考@ref ob_gap_conn_params_t
} ob_gap_evt_connected_t;

/// Event structure for @ref OB_GAP_EVT_DISCONNECTED.
typedef struct {
    uint8_t reason;                     ///< 断开原因
} ob_gap_evt_disconnected_t;

/// Event structure for @ref OB_GAP_EVT_ADV_STATE_CHANGED.
typedef struct {
    uint8_t adv_idx;                   ///< 广播index
    uint8_t state;                     ///< 广播状态，参考@ref ob_gap_adv_state
} ob_gap_evt_adv_state_changed_t;

/// Event structure for @ref OB_GAP_EVT_CONN_PARAMS_UPDATE.
typedef struct {
    uint16_t conn_intv_min;  ///< 连接间隔最小色值
    uint16_t conn_intv_max;  ///< 连接间隔最大值
    uint16_t latency_max;    ///< latency最大值
    uint16_t timeout;        ///< 连接超时时间
} ob_gap_evt_conn_params_update_t;

/// Event structure for @ref OB_GAP_EVT_CONN_PARAMS_REQUEST.
typedef struct {
    uint16_t conn_intv_min;  ///< 连接间隔最小值
    uint16_t conn_intv_max;  ///< 连接间隔最大值
    uint16_t latency_max;    ///< latency最大值
    uint16_t timeout;        ///< 连接超时时间
} ob_gap_evt_conn_params_request_t;

/// 广播report类型
typedef struct {
    uint8_t connectable : 1;    ///< 可连接广播标记
    uint8_t scannable   : 1;    ///< 可扫描广播标记
    uint8_t direct      : 1;    ///< 直连广播标记
    uint8_t scan_resp   : 1;    ///< scan response数据标记
    uint8_t legacy_adv  : 1;    ///< Legacy广播标记
    uint8_t data_state  : 2;    ///< 数据状态，参考 @ref ob_gap_report_state
    uint8_t rsv         : 1;    ///< reserved
} ob_gap_report_event_type_t;

/// Event structure for @ref OB_GAP_EVT_ADV_REPORT.
typedef struct {
    ob_gap_report_event_type_t report_type;     ///< 广播类型 @ref ob_gap_report_event_type_t
    ob_gap_addr_t addr;                         ///< 广播地址
    ob_gap_addr_t direct_adv_addr;              ///< 直接广播地址
    uint8_t prim_phy;                           ///< @ref ob_adv_phy
    uint8_t secd_phy;                           ///< @ref ob_adv_phy
    uint8_t adv_sid;                            ///< 0x00~0x0F, 0xFF means No ADI field provided
    uint16_t padv_intv;                         ///< 0x0000: No periodic advertising, else Time = N*1.25ms
    int8_t tx_pwr;                              ///< 发射功率，若值为@ref OB_GAP_ADV_TX_POWER_NO_AVA 则表示不可用
    int8_t rssi;                                ///< 若值为@ref OB_GAP_ADV_TX_POWER_NO_AVA 则表示不可用
    const uint8_t *data;                        ///< 广播数据
    uint8_t data_len;                           ///< 广播长度
} ob_gap_evt_adv_report_t;

/// Event structure for @ref OB_GAP_EVT_ENCRYPT.
typedef struct {
    bool encrypted;       ///< 链路当前加密状态
} ob_gap_evt_encrypt_t;

/// SMP Authentication requirements flags
typedef struct {
    uint8_t bond_flags : 2;  ///< 是否绑定
    uint8_t mitm       : 1;  ///< 是否支持MITM
    uint8_t sc         : 1;  ///< 是否支持安全配对
    uint8_t rsv        : 4;  ///< reserved
} ob_gap_auth_t;

/// Event structure for @ref OB_GAP_EVT_SEC_REQUEST.
typedef struct {
    ob_gap_auth_t auth; ///< 请求安全属性及绑定信息，参考 @ref ob_gap_auth_t
} ob_gap_evt_sec_request_t;

/// Event structure for @ref OB_GAP_EVT_PAIRING_REQUEST.
typedef struct {
    uint8_t io_capability;              ///< io_capability, @ref ob_smp_io_capability
    uint8_t oob_data_flag;              ///< oob_data_flag
    ob_gap_auth_t authreq;              ///< authreq
    uint8_t initiator_key_distribution; ///< initiator_key_distribution @ref ob_smp_distribution
    uint8_t responder_key_distribution; ///< responder_key_distribution @ref ob_smp_distribution
} ob_gap_evt_pairing_request_t;

/// Event structure for @ref OB_GAP_EVT_PIN_REQUEST.
typedef struct {
    uint8_t type; ///< @ref ob_smp_pin_type
    uint32_t pin_code; ///< only valid if type == OB_SMP_PIN_TYPE_DIS
} ob_gap_evt_pin_request_t;

/// Event structure for @ref OB_GAP_EVT_LTK_REQUEST.
typedef struct {
    uint16_t ediv; ///< EDIV
    uint8_t random[OB_GAP_RANDOM_LEN]; ///< RANDOM
} ob_gap_evt_ltk_request_t;

/// Event structure for @ref OB_GAP_EVT_BOND_INFO_REQUEST.
typedef struct {
    uint8_t type; ///< @ref ob_bond_info_type
} ob_gap_evt_bond_info_request_t;

/// Event structure for @ref OB_GAP_EVT_BONDED.
typedef struct {
    uint32_t status;     ///< 绑定状态
    ob_gap_auth_t auth; ///< 绑定成功信息，参考@ref ob_gap_auth_t
} ob_gap_evt_bonded_t;

/// Event structure for @ref OB_GAP_EVT_BOND_INFO.
typedef struct {
    uint8_t type; ///< @ref ob_bond_info_type
    union {
        /// Identical Address和IRK
        struct {
            ob_gap_addr_t id_addr;
            uint8_t irk[OB_GAP_KEY_LEN];
        } id_info;
        /// EDIV, Random和LTK
        struct {
            uint16_t ediv;
            uint8_t random[OB_GAP_RANDOM_LEN];
            uint8_t ltk[OB_GAP_KEY_LEN];
        } enc_info;
    };
} ob_gap_evt_bond_info_t;

/// Event structure for @ref OB_GAP_EVT_TIMEOUT.
typedef struct {
    uint8_t source; ///< 超时原因，参考@ref ob_gap_timeout_source_t
} ob_gap_evt_timeout_t;

/// Event structure for @ref OB_GAP_EVT_POWER_CHANGED.
typedef struct {
    uint8_t side;                   ///< 本方或对方发射功率更新
    uint8_t phy;                    ///< 更新功率对应的PHY
    int8_t tx_pwr;                  ///< 更新后的功率，若值为@ref OB_GAP_ADV_TX_POWER_NO_AVA 则表示不可用
    uint8_t tx_pwr_level_flag;      ///< 是否达到最大或最小功率，参考 @ref ob_gap_tx_pwr_level_flag
    int8_t tx_pwr_delta;            ///< 功率变化，若值为@ref OB_GAP_ADV_TX_POWER_NO_AVA 则表示不可用
} ob_gap_evt_power_changed_t;

/// Event structure for @ref OB_GAP_EVT_PHY_UPDATE.
typedef struct {
    uint8_t status;                 ///< 更新状态，为0时表示更新成功，否则为更新失败
    uint8_t tx_phy;                 ///< TX PHY for this connection，参考 @ref ob_gap_phys_t
    uint8_t rx_phy;                 ///< RX PHY for this connection，参考 @ref ob_gap_phys_t
} ob_gap_evt_phy_update_t;

/// Event structure for @ref OB_GAP_EVT_DATA_LEN_CHANGED.
typedef struct {
    uint16_t max_tx_octets;   ///< 最大发送字节数
    uint16_t max_rx_octets;   ///< 最大接收字节数
    uint16_t max_tx_time_us;  ///< 最大发送时间（单位：us）
    uint16_t max_rx_time_us;  ///< 最大接收时间（单位：us）
} ob_gap_evt_data_len_changed_t;

/// Event structure for @ref OB_GAP_EVT_DEVICE_INFO.
typedef struct {
    uint8_t type;                     ///< 信息类型 @ref ob_dev_info_type
    union {
        /// bluetooth version
        struct {
            uint8_t version;              ///< 蓝牙version
            uint8_t subversion;           ///< 蓝牙subversion
            uint16_t company_identifier;  ///< company_identifier
        } version;
        /// device feature
        uint8_t features[8];
        /// RSSI
        int8_t rssi;
        /// channel map
        uint8_t channel_map[5];
    };
} ob_gap_evt_device_info_t;

/// Event structure for @ref OB_GAP_EVT_HCI_ERROR.
typedef struct {
    uint16_t hci_opcode;     ///< 出错时对应的hci opcode
    uint8_t status;          ///< 出错时的HCI status状态
} ob_gap_evt_hci_error_t;

/// GAP 消息结构体
typedef struct {
    uint8_t conn_idx; ///< 连接 index
    union {
        ob_gap_evt_connected_t              connected;             ///< 连接成功事件参数
        ob_gap_evt_disconnected_t           disconnected;          ///< 连接断开事件参数
        ob_gap_evt_adv_state_changed_t      adv_state_changed;     ///< 广播状态变化事件参数
        ob_gap_evt_conn_params_update_t     conn_params_update;    ///< 参数更新事件参数
        ob_gap_evt_conn_params_request_t    conn_params_request;   ///< 参数更新请求参数
        ob_gap_evt_adv_report_t             adv_report;            ///< 广播上报事件参数
        ob_gap_evt_encrypt_t                encrypt;               ///< 链路加密状态改变事件参数
        ob_gap_evt_sec_request_t            sec_request;           ///< 加密请求事件参数
        ob_gap_evt_pairing_request_t        pairing_request;       ///< 配对请求事件参数
        ob_gap_evt_pin_request_t            pin_request;           ///< 配对PIN码请求事件参数
        ob_gap_evt_ltk_request_t            ltk_request;           ///< LTK请求参数
        ob_gap_evt_bond_info_request_t      bond_info_request;     ///< 绑定信息请求参数
        ob_gap_evt_bonded_t                 bonded;                ///< 配对完成事件参数
        ob_gap_evt_bond_info_t              bond_info;             ///< 配对信息上报事件（LTK/IRK）参数
        ob_gap_evt_timeout_t                timeout;               ///< 超时事件参数
        ob_gap_evt_power_changed_t          power_changed;         ///< 功率变化事件参数
        ob_gap_evt_phy_update_t             phy_updated;           ///< PHY变化事件参数
        ob_gap_evt_data_len_changed_t       data_len_changed;      ///< Data Length变化事件参数
        ob_gap_evt_device_info_t            device_info;           ///< 设备信息参数
        ob_gap_evt_hci_error_t              hci_error;             ///< HCI指令错误参数
    };
} omble_gap_evt_t;

/**@brief 设置默认地址
 * @note 地址必须在无广播和连接的状态下设置
 * @param[in]  addr_type    地址类型，参考 @ref ob_adv_addr_type.
 * @param[in]  addr         设备地址
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_addr_set(uint8_t addr_type, const uint8_t addr[OB_GAP_ADDR_LEN]);

/**@brief 获取默认地址
 * @param[in]   addr_type  地址类型，参考 @ref ob_adv_addr_type.
 * @param[out]  addr       地址指针
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_addr_get(uint8_t addr_type, uint8_t addr[OB_GAP_ADDR_LEN]);
/**@brief 设置白名单
 * @param[in]  fa_addrs   filter accept地址指针组
 * @param[in]  len        数组长度
 * @note 只能在无广播使能的状态下进行调用
 * @note 若fa_addrs为NULL则清除当前Filter Accept列表内容
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_accept_list_set(const ob_gap_addr_t *fa_addrs, uint8_t len);
/**@brief 开启指定index的广播
 * @param[in]  adv_idx        广播index
 * @param[in]  adv_param      广播参数，若为NULL则只更新当前广播的广播数据
 * @param[in]  adv_data       广播数据
 * @param[in]  scan_rsp_data  扫描数据
 * @note 若只更新参数，则该广播必须为已开启状态
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_adv_start(uint8_t adv_idx, ob_adv_param_t *adv_param, ob_data_t *adv_data, ob_data_t *scan_rsp_data);
/**@brief 关闭指定index的广播
 * @param[in]  adv_idx        广播index
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_adv_stop(uint8_t adv_idx);

/**@brief 参数更新
 * @param[in]  conn_idx       连接index
 * @param[in]  p_conn_params  连接参数
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_conn_param_update(uint8_t conn_idx, ob_gap_conn_params_t const *p_conn_params);

/**@brief 断开连接
 * @param[in]  conn_idx        连接index
 * @param[in]  hci_status_code 断开原因
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_disconnect(uint8_t conn_idx, uint8_t hci_status_code);

/**@brief 配对/绑定请求
 * @param[in]  conn_idx       连接index
 * @param[in]  auth           auth
 * @note 只有peripheral角色可以调用此接口
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_security_request(uint8_t conn_idx, ob_gap_auth_t auth);

/// GAP SMP配对参数结构体
typedef struct {
    uint8_t io_capability;              ///< io_capability, @ref ob_smp_io_capability
    uint8_t oob_data_flag;              ///< oob_data_flag
    ob_gap_auth_t authreq;              ///< authreq
    uint8_t initiator_key_distribution; ///< initiator_key_distribution @ref ob_smp_distribution
    uint8_t responder_key_distribution; ///< responder_key_distribution @ref ob_smp_distribution
} ob_pairing_param_t;

/**@brief 配对/绑定请求
 * @param[in]  conn_idx       连接index
 * @param[in]  request        配对请求参数
 * @note 只有central角色可以调用此接口
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_pairing_request(uint8_t conn_idx, ob_pairing_param_t *request);

/**@brief 配对/绑定请求
 * @param[in]  conn_idx       连接index
 * @param[in]  response       配对响应参数
 * @note 只有peripheral角色可以调用此接口
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_pairing_response(uint8_t conn_idx, ob_pairing_param_t *response);

/**@brief 链路加密请求
 * @param[in]  conn_idx       连接index
 * @param[in]  ediv           EDIV
 * @param[in]  rand           random value
 * @param[in]  ltk            Long Term Key
 * @note 只有central角色可以调用此接口，用于发起链路加密流程
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_encrypt(uint8_t conn_idx, uint16_t ediv, uint8_t *rand, uint8_t *ltk);

/**@brief 链路加密响应
 * @param[in]  conn_idx       连接index
 * @param[in]  ltk            Long Term Key
 * @note 若ltk为 NULL时表示LTK key missing
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_ltk_response(uint8_t conn_idx, uint8_t *ltk);

/**@brief 配对信息响应
 * @param[in]  conn_idx       连接index
 * @param[in]  bond_info      配对信息
 * @note 该接口用于回复OB_GAP_EVT_BOND_INFO_REQUEST消息
 * @note 若ltk为 NULL时表示LTK key missing
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_bond_info_response(uint8_t conn_idx, ob_bond_info_t *bond_info);

/**@brief PIN码响应
 * @param[in]  conn_idx       连接index
 * @param[in]  accept         是否接受配对操作
 * @param[in]  pin_info       需要响应的PIN码
 * @note 该接口用于响应OOB_GAP_EVT_PAIRING_REQUEST事件
 * @note 在pin_info::type为OB_SMP_PIN_TYPE_PK_REQ/OB_SMP_PIN_TYPE_OOB_REQ时用于回复相应的PIN码
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_pin_response(uint8_t conn_idx, uint8_t accept, ob_smp_pin_t *pin_info);

/**@brief 获取当前加密状态
 * @param[in]  conn_idx       连接index
 * @param[out] state          true/false，链路是否已加密
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_get_encrypt_state(uint8_t conn_idx, uint8_t *state);

/// ob_gap_phys
typedef struct {
    uint8_t phy_1m    : 1; ///< 1M phy
    uint8_t phy_2m    : 1; ///< 2M phy
    uint8_t phy_coded : 1; ///< Coded phy
    uint8_t rsv       : 5; ///< reserved
} ob_gap_phys_t;

/**@brief phy更新请求
 * @param[in]  conn_idx       连接index
 * @param[in]  tx_phys        链路发送PHY的建议值，参考@ref ob_gap_phys_t
 * @param[in]  rx_phys        链路接收PHY的建议值，参考@ref ob_gap_phys_t
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_phy_update(uint8_t conn_idx, ob_gap_phys_t tx_phys, ob_gap_phys_t rx_phys);

/**@brief 更新data length
 * @param[in]  conn_idx       连接index
 * @param[out] tx_octets      建议最大的数据字节长度
 * @param[out] tx_time        建议最大的数据传输时间（单位微秒）
 * @note tx_octets的范围是  0x001B ~ 0x00FB
 * @note tx_time的范围是  0x0148 ~ 0x4290
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_data_length_update(uint8_t conn_idx, uint16_t tx_octets, uint16_t tx_time);

/**@brief Set Host Channel Classification
 * @param[in]  channel_map  低37 bit对应通道号: field bad = 0, unknown = 1
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_host_channel_map_update(const uint8_t channel_map[5]);

/// ob_scanning_param
typedef struct {
    uint8_t scan_type;   ///< is Active Scanning
    uint16_t interval;   ///< 扫描间隔
    uint16_t window;     ///< 扫描窗口
} ob_scanning_param_t;

/// ob_scan_param
typedef struct {
    uint8_t addr_type;               ///< See @ref ob_adv_addr_type
    uint8_t filter_policy;           ///< @ref ob_scan_filter_policy
    uint16_t timeout;                ///< 超时时间，单位10毫秒
    ob_scanning_param_t *scan_1m;    ///< 1M phy 扫描参数
    ob_scanning_param_t *scan_coded; ///< Coded phy 扫描参数
} ob_scan_param_t;

/**@brief 开启扫描
 * @param[in]  param       扫描参数
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_scan_start(ob_scan_param_t *param);

/**@brief 停止扫描
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_scan_stop(void);

/// ob_conn_phy_param
typedef struct {
    uint16_t scan_intv;      ///< 扫描间隔
    uint16_t scan_wind;      ///< 扫描窗口
    uint16_t conn_intv_min;  ///< 连接间隔最小色值
    uint16_t conn_intv_max;  ///< 连接间隔最大值
    uint16_t latency_max;    ///< latency最大值
    uint16_t timeout;        ///< 连接超时时间
} ob_conn_phy_param_t;

/// ob_conn_param
typedef struct {
    uint8_t addr_type;               ///< ob_adv_addr_type
    uint8_t filter_policy;           ///< @ref ob_conn_filter_policy
    ob_gap_addr_t peer_addr;         ///< 连接地址
    ob_conn_phy_param_t *scan_1m;    ///< 1M phy 连接参数
    ob_conn_phy_param_t *scan_2m;    ///< 2M phy 连接参数
    ob_conn_phy_param_t *scan_coded; ///< Coded phy 扫描参数
} ob_conn_param_t;
/**@brief 创建连接
 * @param[in]  param       连接参数
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_connect(ob_conn_param_t *param);

/**@brief 取消连接
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gap_connect_cancel(void);

/**@brief 获取设备信息
 * @param[in]  type        信息类型，参考@ref ob_dev_info_type
 * @param[in]  conn_idx    若type为OB_DEV_INFO_LOCAL_XXX则该参数无效
 * @return 执行结果，参考@ref ob_error
 * @note 返回的设备信息会通过@ref OB_GAP_EVT_DEVICE_INFO 事件上报
 */
uint32_t ob_gap_get_device_info(uint8_t type, uint8_t conn_idx);

/**@brief 根据指定IRK生成RPA
 * @param[in]   irk       用于生成RPA的IRK
 * @param[out]  rpa       生成的RPA
 */
void ob_gap_rpa_gen(const uint8_t irk[16], uint8_t rpa[6]);

/**@brief 检查指定的RPA和IRK是否匹配
 * @param[in]   irk       与RPA对应的IRK
 * @param[out]  rpa       需要检查的RPA
 * @return 是否匹配
 * @note 若给定的RPA类型错误则也返回false
 */
bool ob_gap_rpa_check(const uint8_t irk[16], const uint8_t rpa[6]);

#endif /* __OMBLE_GAP_H__ */

/// @}
