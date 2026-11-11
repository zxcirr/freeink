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
 * @file     omsh_apis.h
 * @brief    Header file of omesh APIs
 * @date     01. JAN. 2023
 * @author   OnMicro SW Team
 *
 * @defgroup MESH API
 * @ingroup  MESH
 * @brief    Header file of omesh APIs
 * @details  Header file of omesh APIs

 * @version
 * Version 1.0
 *  - Initial release
 *
 *
 * @{
 */

#ifndef __OMSH_APIS_H__
#define __OMSH_APIS_H__

#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * INCLUDES
 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "features.h"


/*******************************************************************************
 * DEFINES
 */
/// Authentication data OOB length
#ifndef MESH_OOB_AUTH_DATA_LEN
#define MESH_OOB_AUTH_DATA_LEN      (16)
#endif

/// Device uuid length
#ifndef MESH_DEV_UUID_LEN
#define MESH_DEV_UUID_LEN           (16)
#endif

/// Key length
#ifndef MESH_KEY_LEN
#define MESH_KEY_LEN                (16)
#endif

/// TODO: Mesh NVDS first tag entry, Modify it need synchronize it with the library
#ifndef NVDS_TAG_MESH_FIRST
#define NVDS_TAG_MESH_FIRST         (0xB0)
#endif

/// Mesh NVDS last tag entry
#ifndef NVDS_TAG_MESH_LAST
#define NVDS_TAG_MESH_LAST          (0xFF)
#endif

/// Mesh NVDS get tag entry index
#ifndef MESH_TB_GET_NVDS_TAG
#define MESH_TB_GET_NVDS_TAG(x)     (NVDS_TAG_MESH_FIRST + x)
#endif


/*******************************************************************************
 * LOG MOULE DEFINE
 */
#ifndef __LOG
/// Defines various sources for logging messages.
#define LOG_SRC_API                 (1 <<  0)       /**< Receive logs from the abstraction layer.       */
#define LOG_SRC_BEARER              (1 <<  1)       /**< Receive logs from the bearer layer.            */
#define LOG_SRC_NETWORK             (1 <<  2)       /**< Receive logs from the network layer.           */
#define LOG_SRC_LTRANSPORT          (1 <<  3)       /**< Receive logs from the lower transport layer.   */
#define LOG_SRC_UTRANSPORT          (1 <<  4)       /**< Receive logs from the upper transport layer.   */
#define LOG_SRC_ACCESS              (1 <<  5)       /**< Receive logs from the access layer.            */
#define LOG_SRC_APP                 (1 <<  6)       /**< Receive logs from the app layer.               */
#define LOG_SRC_PROV                (1 <<  7)       /**< Receive logs from the provisioning module.     */
#define LOG_SRC_FRIEND              (1 <<  8)       /**< Receive logs from the friend layer.            */
#define LOG_SRC_LPN                 (1 <<  9)       /**< Receive logs from the LPN layer.               */
#define LOG_SRC_MODEL               (1 <<  10)      /**< Receive logs from the model layer.             */
#define LOG_SRC_PROXY               (1 <<  11)      /**< Receive logs from the proxy layer.             */
#define LOG_SRC_GATT                (1 <<  12)      /**< Receive logs from the debug use.               */

/// Defines possible criticality levels for logged messages.
#define LOG_LEVEL_ASSERT            (0)             /**< Log level for assertions                       */
#define LOG_LEVEL_ERROR             (1)             /**< Log level for error messages.                  */
#define LOG_LEVEL_WARN              (2)             /**< Log level for warning messages.                */
#define LOG_LEVEL_REPORT            (3)             /**< Log level for report messages.                 */
#define LOG_LEVEL_INFO              (4)             /**< Log level for information messages.            */
#define LOG_LEVEL_PTS               (5)             /**< Log level for PTS messages.                    */
#define LOG_LEVEL_DBG1              (6)             /**< Log level for debug messages (debug level 1).  */
#define LOG_LEVEL_DBG2              (7)             /**< Log level for debug messages (debug level 2).  */
#define LOG_LEVEL_DBG3              (8)             /**< Log level for debug messages (debug level 3).  */


#if CONFIG_BLE_MESH_LOG_EN
extern uint32_t g_om_log_mask;
extern int32_t g_om_log_level;
extern void log_printf(uint32_t source, uint32_t timestamp, const char *format, ...);
extern uint32_t omesh_port_sys_time_get(void);

#define __LOG(source, level, ...)                                                   \
    if ((source & g_om_log_mask) && level <= g_om_log_level) {                      \
        log_printf(source, omesh_port_sys_time_get(), __VA_ARGS__);                 \
    }

#else
#define __LOG(...)
#endif /* CONFIG_BLE_MESH_LOG_EN */

#endif /* __LOG */


/*******************************************************************************
 * TYPEDEFS ENUMERATE
 */
/// Mesh error type enumerate
typedef enum {
    /// No Error
    OMESH_ERROR_NO_ERROR,
    /// Invalid length
    OMESH_ERROR_INVALID_LEN,
    /// Invalid params
    OMESH_ERROR_INVALID_PARAMS,
    /// Can't execute when unproved
    OMESH_ERROR_INVALID_STATE,
    /// Insufficient resources
    OMESH_ERROR_INSUFF_RESOURCE,
    /// Model register Fail
    OMESH_ERROR_MDL_REG_FAIL,
    /// Model publish data Fail
    OMESH_ERROR_MDL_PUB_FAIL,
    /// Default TTL set Fail
    OMESH_ERROR_TTL_SET_FAIL,

    /// Device Key Update Fail
    OMESH_ERROR_DEV_KEY_UPD_FAIL,

    /// APP Key ADD Fail
    OMESH_ERROR_APP_KEY_ADD_FAIL,
    /// APP Key DEL Fail
    OMESH_ERROR_APP_KEY_DEL_FAIL,
    /// APP Key GET Fail
    OMESH_ERROR_APP_KEY_GET_FAIL,
    /// Cannot Update APP KEY
    OMESH_ERROR_APP_KEY_UPD_FAIL,
    /// Node Publish Need Least One APP Key
    OMESH_ERROR_APP_KEY_NUM_OV,
    /// Cannot Find APP KEY ID
    OMESH_ERROR_FIND_APP_KEY_FAIL,

    /// Network key ADD Fail
    OMESH_ERROR_NET_KEY_ADD_FAIL,
    /// APP Key DEL Fail
    OMESH_ERROR_NET_KEY_DEL_FAIL,
    /// Network Key GET Fail
    OMESH_ERROR_NET_KEY_GET_FAIL,
    /// Network Key Update Fail
    OMESH_ERROR_NET_KEY_UPD_FAIL,
    /// Node Only Support Netkey One
    OMESH_ERROR_NET_KEY_NUM_OV,
    /// Cannot Find NET KEY ID
    OMESH_ERROR_FIND_NET_KEY_FAIL,

    /// Node Publish Check Binding Fail
    OMESH_ERROR_CHECK_BIND_FAIL,

    /// Set Subscription Address Fail
    OMESH_ERROR_ADD_SUBS_ADDR_FAIL,
    /// Get Subscription Address Fail
    OMESH_ERROR_GET_SUBS_ADDR_FAIL,
    /// Delete Subscription Address Fail
    OMESH_ERROR_DEL_SUBS_ADDR_FAIL,
} omesh_error_t;

/// State of the provisioning
enum omesh_prov_state
{
    /// Provisioning started - procedure started by a provisioner
    OMESH_PROV_STARTED,
    /// Provisioning succeed
    OMESH_PROV_SUCCEED,
    /// Provisioning failed
    OMESH_PROV_FAILED,
};


/*******************************************************************************
 * TYPEDEFS STRUCTURE
 */
/// Local Identifier
typedef uint8_t omesh_lid_t;

/// Typedef of api buf type
typedef void omesh_apis_buf_t;

/// structure of a list element header
struct omesh_api_list_hdr
{
    /// Pointer to next omesh_list_hdr
    struct omesh_api_list_hdr *next;
};

/// simplify type name of list element header
typedef struct omesh_api_list_hdr omesh_api_list_hdr_t;

/// structure of a list
struct omesh_api_list
{
    /// pointer to first element of the list
    struct omesh_api_list_hdr *first;

    /// pointer to the last element
    struct omesh_api_list_hdr *last;
};

/// simplify type name of list
typedef struct omesh_api_list omesh_api_list_t;

/// Mesh Job element structure
typedef void (*omesh_djob_cb)(void *p_env);

typedef struct omesh_djob {
    /// Pointer to environment that will be used as callback parameter.
    void *p_env;

    /// Callback to execute in background context
    omesh_djob_cb cb;
} omesh_djob_t;

/// Mesh node identifier structures
typedef struct omesh_node_id {
    /// UUID
    uint8_t uuid[MESH_DEV_UUID_LEN];

    /// CID
    uint16_t company_id;

    /// PID
    uint16_t product_id;

    /// VID
    uint16_t version_id;
} omesh_node_id_t;

/// Mesh network layer parameters structures
typedef struct omesh_net_params {
    /// Default TTL, range: [2-127]
    uint8_t send_ttl;

    /// Retransmit number of network layer, range: [1,8]
    uint8_t net_transmit_cnt;

    /// Retransimt step of network layer (1 = 0.625ms), range: [0x20-0x4000]
    uint16_t net_transmit_step;
} omesh_net_params_t;

/// Proxy connectable advertising control values
typedef enum {
    /// Stop connectable advertising
    OMESH_PROXY_ADV_CTL_STOP = 0,

    /// Start connectable advertising with Node Identity (duration = 60s)
    OMESH_PROXY_ADV_CTL_START_NODE,

    /// Start connectable advertising with Network ID (duration = 60s)
    OMESH_PROXY_ADV_CTL_START_NET,
} omesh_proxy_adv_ctl_t;

/// Mesh auto provision structures
typedef struct omesh_auto_prov {
    /// Device Key
    uint8_t  dev_key[MESH_KEY_LEN];

    /// Network Key
    uint8_t  net_key[MESH_KEY_LEN];

    /// Network Key Index
    uint16_t net_key_id;

    /// Key state flags bitmask
    uint8_t  flags;

    /// Current Value of the IV index
    uint32_t iv;

    /// Unicast Address of the primary element
    uint16_t unicast_addr;
} omesh_auto_prov_t;

/// Mesh provision authenticate data structures
typedef uint8_t omesh_prov_auth_data_t[MESH_OOB_AUTH_DATA_LEN];

/// Mesh Provisioning Authentication Data Response
typedef struct omesh_api_prov_auth_data_cfm {
    /// 1, Accept pairing request, 0 reject
    uint8_t  accept;

    /// Authentication data size (<= requested size else pairing automatically rejected)
    uint8_t  auth_size;

    /// Authentication data (LSB for a number or array of bytes)
    uint8_t  auth_data[16];
} omesh_api_prov_auth_data_cfm_t;

/// Mesh Provisioning Parameters Response
typedef struct omesh_api_prov_param_cfm {
    /// Device UUID
    uint8_t  dev_uuid[MESH_DEV_UUID_LEN];

    /// URI hash
    uint32_t uri_hash;

    /// OOB information
    uint16_t oob_info;

    /// Public key OOB information available
    uint8_t  pub_key_oob;

    /// Static OOB information available
    uint8_t  static_oob;

    /// Maximum size of Output OOB supported
    uint8_t  out_oob_size;

    /// Maximum size in octets of Input OOB supported
    uint8_t  in_oob_size;

    /// Supported Output OOB Actions (@see enum om_prov_out_oob)
    uint16_t out_oob_action;

    /// Supported Input OOB Actions (@see enum om_prov_in_oob)
    uint16_t in_oob_action;

    /// Number of elements
    uint8_t  nb_elt;

    /// Bit field providing additional information (@see enum om_prov_info)
    uint8_t  info;
} omesh_api_prov_param_cfm_t;

/// Mesh model receive data structure
typedef struct omesh_mm_rec_data {
    /// opcode received of the message
    uint32_t opcode;

    /// Source address of the message
    uint16_t src;

    /// Destination address of the message
    uint16_t dst;

    /// Network key local index of the message
    uint8_t net_lid;

    /// Application key local index of the message
    uint8_t app_lid;

    /// Sequence number of the message
    uint32_t seq;

    /// TTL and CTL of the message
    uint8_t ttl_ctl;

    /// Length of the data part
    uint16_t p_data_len;

    /// The vaild data received pointer
    uint8_t *p_data;
} omesh_mm_rec_data_t;

/// Mesh model send ack data structure
typedef struct omesh_mm_send_ack_data {
    /// opcode received of the message
    uint32_t opcode;

    /// Model local index of the message
    uint8_t model_lid;

    /// Is vendor model of the message
    bool vendor;

    /// Length of the data part
    uint16_t p_data_len;

    /// The vaild data received pointer
    uint8_t *p_data;
} omesh_mm_send_ack_data_t;

/**
 ****************************************************************************************
 * @brief Definition of callback function to call upon reception of a PDU for specific model identifier
 *
 * @param[in] model_lid    Model Local Identifier
 * @param[in] opcode       Operation code
 * @param[in] p_buf        Pointer to the buffer containing the message PDU. - No need to release buffer.
 * @param[in] app_key_lid  Application Key Local identifier (Required for a response)
 * @param[in] src          Source address of the message (Required for a response)
 * @param[in] rssi         Measured RSSI level for the received PDU.
 * @param[in] not_relayed  True if message have been received by an immediate peer; False, it can have been relayed
 ****************************************************************************************
 */
typedef void (*omesh_api_model_rx_cb)(omesh_lid_t model_lid, uint32_t opcode, omesh_apis_buf_t *p_buf, omesh_lid_t app_key_lid, uint16_t src, int8_t rssi, bool not_relayed);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call upon reception of a PDU to check that model can handle it
 *
 * @note omesh_stack_api_model_opcode_status function must be used to provide information about opcode support
 *
 * @param[in] model_lid Model Local Identifier
 * @param[in] opcode    Operation code to check
 ****************************************************************************************
 */
typedef void (*omesh_api_model_opcode_check_cb)(omesh_lid_t model_lid, uint32_t opcode);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call upon reception of a new publish period value.
 *
 * @param[in] model_lid    Model Local Identifier
 * @param[in] period_ms    Publish period in milliseconds
 ****************************************************************************************
 */
typedef void (*omesh_api_model_publish_period_cb)(omesh_lid_t model_lid, uint32_t period_ms);

/**
 ****************************************************************************************
 * @brief Definition of callback function to call once PDU has been sent.
 *
 * @param[in] model_lid Model Local Identifier
 * @param[in] tx_hdl    Handle value configured by model when message has been requested to be sent
 * @param[in] p_buf     Pointer to the buffer containing the transmitted PDU. - Buffer must be released by model.
 * @param[in] status    Transmission status.
 ****************************************************************************************
 */
typedef void (*omesh_api_model_sent_cb)(omesh_lid_t model_lid, uint8_t tx_hdl, omesh_apis_buf_t *p_buf, uint16_t status);

/// Callback Structure for registered models
typedef struct omesh_api_model_cb {
    /// Reception of a buffer for model
    omesh_api_model_rx_cb             cb_rx;

    /// Callback executed when a PDU is properly sent
    omesh_api_model_sent_cb           cb_sent;

    /// Check if model can handle operation code
    omesh_api_model_opcode_check_cb   cb_opcode_check;

    /// Callback function called when a new publish period is received
    omesh_api_model_publish_period_cb cb_publish_period;
} omesh_api_model_cb_t;


/*******************************************************************************
 * MESH GLOBAL APIS FOR APP USE
 */

/**
 ****************************************************************************************
 * @brief Mesh api of inits mesh stack allocate required resources
 ****************************************************************************************
 */
void omesh_stack_init(void);

/**
 ****************************************************************************************
 * @brief Mesh api of uninit mesh stack release resources
 ****************************************************************************************
 */
void omesh_stack_uninit(void);

/**
 ****************************************************************************************
 * @brief Mesh api of enable mesh stack begain to advertising
 ****************************************************************************************
 */
void omesh_stack_enable(void);

/**
 ****************************************************************************************
 * @brief Mesh api of disable mesh stack
 ****************************************************************************************
 */
void omesh_stack_disable(void);

/**
 ****************************************************************************************
 * @brief Mesh api of set mesh stack feature
 *        when you want feature active,set it first,
 *        and then set the corresponding state of the following functions
 ****************************************************************************************
 */
void omesh_set_feature_en(bool relay_en, bool proxy_en, bool frnd_en, bool lpn_en);

/**
 ****************************************************************************************
 * @brief Mesh api of relay state set and get
 ****************************************************************************************
 */
void omesh_set_relay_state(bool en);
bool omesh_get_relay_state(void);

/**
 ****************************************************************************************
 * @brief Mesh api of gatt proxy state set and get
 ****************************************************************************************
 */
void omesh_set_proxy_state(bool en);
bool omesh_get_proxy_state(void);

/**
 ****************************************************************************************
 * @brief Mesh api of friend state set and get
 ****************************************************************************************
 */
void omesh_set_friend_state(bool en);
bool omesh_get_friend_state(void);

/**
 ****************************************************************************************
 * @brief Mesh api of lpn state set and get
 ****************************************************************************************
 */
void omesh_set_lpn_state(bool en);
bool omesh_get_lpn_state(void);

/**
 ****************************************************************************************
 * @brief Mesh api of network beacon enable set and get
 ****************************************************************************************
 */
void omesh_set_network_bcn_en(bool en);
bool omesh_get_network_bcn_en(void);

/**
 ****************************************************************************************
 * @brief Mesh api of log init
 *
 * @param[in] mask      Defines various sources for logging messages
 * @param[in] level     Defines possible criticality levels for logged messages.
 *
 ****************************************************************************************
 */
void omesh_log_init(uint32_t mask, uint32_t level);

/**
 ****************************************************************************************
 * @brief Mesh api of uuid set and get,length must be MESH_DEV_UUID_LEN
 ****************************************************************************************
 */
void omesh_set_uuid(uint8_t *uuid);
uint8_t *omesh_get_uuid(void);

/**
 ****************************************************************************************
 * @brief Mesh api of node company id set and get
 ****************************************************************************************
 */
void omesh_set_cid(uint16_t cid);
uint16_t omesh_get_cid(void);

/**
 ****************************************************************************************
 * @brief Mesh api of node product id set and get
 ****************************************************************************************
 */
void omesh_set_pid(uint16_t pid);
uint16_t omesh_get_pid(void);

/**
 ****************************************************************************************
 * @brief Mesh api of node version id set and get
 ****************************************************************************************
 */
void omesh_set_vid(uint16_t vid);
uint16_t omesh_get_vid(void);

/**
 ****************************************************************************************
 * @brief Mesh api of mesh provision authentication data set and get
 ****************************************************************************************
 */
void omesh_set_prov_auth_data(omesh_prov_auth_data_t auth_data);
omesh_prov_auth_data_t *omesh_get_prov_auth_data(void);

/**
 ****************************************************************************************
 * @brief Mesh api of mesh default TTL set and get
 ****************************************************************************************
 */
void omesh_set_default_ttl(uint8_t default_ttl);
uint8_t omesh_get_default_ttl(void);

/**
 ****************************************************************************************
 * @brief Mesh api of mesh primary address set and get
 *        when you set primary address,it will delete all publish/subscription/binding information
 *        and then auto restart chip to finish set
 ****************************************************************************************
 */
omesh_error_t omesh_set_prim_addr(uint16_t addr);
uint16_t omesh_get_prim_addr(void);

/**
 ****************************************************************************************
 * @brief Mesh api of set network layer params
 ****************************************************************************************
 */
omesh_error_t omesh_set_net_params(omesh_net_params_t *p_net_params);

/**
 ****************************************************************************************
 * @brief Mesh api of get mesh network layer TX params
 ****************************************************************************************
 */
void omesh_get_net_tx_params(uint8_t *p_tx_count, uint16_t *p_intv_slots);

/**
 ****************************************************************************************
 * @brief Mesh api of mesh IV index and SEQ set and get
 ****************************************************************************************
 */
void omesh_set_iv_seq(uint32_t iv, uint32_t seq);
void omesh_get_iv_seq(uint32_t *p_iv, uint32_t *p_seq);

/**
 ****************************************************************************************
 * @brief Mesh api of mesh storage information save and load
 ****************************************************************************************
 */
void omesh_storage_save(void);
void omesh_storage_load(void);

/**
 ****************************************************************************************
 * @brief Mesh api of get provisioned successfully
 ****************************************************************************************
 */
bool omesh_get_if_prov_success(void);

/**
 ****************************************************************************************
 * @brief Mesh api of device key update, key length must be MESH_KEY_LEN
 ****************************************************************************************
 */
omesh_error_t omesh_devkey_update(uint8_t *key);

/**
 ****************************************************************************************
 * @brief Mesh api of get registered models number
 ****************************************************************************************
 */
uint8_t omesh_get_models_num(void);

/**
 ****************************************************************************************
 * @brief Mesh api of get netkey number
 ****************************************************************************************
 */
uint8_t omesh_get_netkey_num(void);

/**
 ****************************************************************************************
 * @brief Mesh api of get appkey number by network key local index
 ****************************************************************************************
 */
uint8_t omesh_get_appkey_num(uint8_t net_key_lid);

/**
 ****************************************************************************************
 * @brief Mesh api of get netkey local index information
 ****************************************************************************************
 */
void omesh_get_netkey_lid_info(uint8_t *p_net_lid_start, uint8_t *p_net_lid_end);

/**
 ****************************************************************************************
 * @brief Mesh api of get appkey local index information
 ****************************************************************************************
 */
void omesh_get_appkey_lid_info(uint8_t *p_app_lid_start, uint8_t *p_app_lid_end);

/**
 ****************************************************************************************
 * @brief Mesh api of get netkey local index and identifier list
 ****************************************************************************************
 */
omesh_error_t omesh_get_netkey_lid_and_id_list(uint8_t *p_net_lid, uint16_t *p_net_id);

/**
 ****************************************************************************************
 * @brief Mesh api of get appkey local index and identifier list
 ****************************************************************************************
 */
omesh_error_t omesh_get_appkey_lid_and_id_list(uint16_t netkey_id, uint8_t *p_app_lid, uint16_t *p_app_id);

/**
 ****************************************************************************************
 * @brief Mesh api of add netkey
 ****************************************************************************************
 */
omesh_error_t omesh_add_netkey(uint16_t netkey_id, const uint8_t *p_key);

/**
 ****************************************************************************************
 * @brief Mesh api of delete netkey
 ****************************************************************************************
 */
omesh_error_t omesh_del_netkey(uint16_t netkey_id);

/**
 ****************************************************************************************
 * @brief Mesh api of add appkey
 ****************************************************************************************
 */
omesh_error_t omesh_add_appkey(uint16_t netkey_id, uint16_t appkey_id, const uint8_t *p_key);

/**
 ****************************************************************************************
 * @brief Mesh api of delete appkey
 ****************************************************************************************
 */
omesh_error_t omesh_del_appkey(uint16_t netkey_id, uint16_t appkey_id);

/**
 ****************************************************************************************
 * @brief Mesh api of reset node to device
 *          when you call it,it will delete all provision data,
 *          and then auto restart chip back to unproved state
 ****************************************************************************************
 */
void omesh_node_reset_to_device(void);

/**
 ****************************************************************************************
 * @brief Mesh api of lpn start
 ****************************************************************************************
 */
void omesh_lpn_start(void);

/**
 ****************************************************************************************
 * @brief Mesh api of lpn stop
 ****************************************************************************************
 */
void omesh_lpn_stop(void);

/**
 ****************************************************************************************
 * @brief Mesh api of lpn select friend by address
 ****************************************************************************************
 */
void omesh_lpn_select_friend(uint16_t friend_addr);

/**
 ****************************************************************************************
 * @brief Mesh api of control proxy connectable advertising
 ****************************************************************************************
 */
void omesh_proxy_con_adv_ctrl(omesh_proxy_adv_ctl_t ctrl);

/**
 ****************************************************************************************
 * @brief Mesh api of publish health status
 ****************************************************************************************
 */
void omesh_publish_health_status(uint8_t length, uint8_t *p_falut_array);

/**
 ****************************************************************************************
 * @brief Mesh api of publish health fault status
 ****************************************************************************************
 */
void omesh_publish_health_fault_status(uint8_t length, uint8_t *p_falut_array);

/**
 ****************************************************************************************
 * @brief Mesh api of model identifier add subscription address
 ****************************************************************************************
 */
omesh_error_t omesh_add_subs_list(uint32_t model_id, uint16_t sub_adr);

/**
 ****************************************************************************************
 * @brief Mesh api of model identifier delete subscription address
 ****************************************************************************************
 */
omesh_error_t omesh_del_subs_list(uint32_t model_id, uint16_t sub_adr);

/**
 ****************************************************************************************
 * @brief Mesh api of model identifier delete all subscription address
 ****************************************************************************************
 */
omesh_error_t omesh_del_all_subs_list(uint32_t model_id);

/**
 ****************************************************************************************
 * @brief Mesh api of model identifier get subscription address number
 ****************************************************************************************
 */
uint8_t omesh_get_nb_subs_list(uint32_t model_id);

/**
 ****************************************************************************************
 * @brief Mesh api of model identifier get subscription address list
 ****************************************************************************************
 */
omesh_error_t omesh_get_adr_subs_list(uint32_t model_id, uint16_t *p_subs_list);

/**
 ****************************************************************************************
 * @brief Mesh api of get model local index by identifier
 ****************************************************************************************
 */
omesh_error_t omesh_get_model_lid_by_id(uint32_t model_id, uint8_t *p_model_lid);

/**
 ****************************************************************************************
 * @brief Mesh api of get model identifier by local index
 ****************************************************************************************
 */
omesh_error_t omesh_get_model_id_by_lid(uint8_t model_lid, uint32_t *p_model_id);

/**
 ****************************************************************************************
 * @brief Mesh api of appkey bind to model
 ****************************************************************************************
 */
omesh_error_t omesh_model_bind_appkey(uint32_t model_id, uint8_t app_key_lid);

/**
 ****************************************************************************************
 * @brief Mesh api of appkey bind to all models
 ****************************************************************************************
 */
omesh_error_t omesh_all_model_bind_appkey(uint8_t app_key_lid);

/**
 ****************************************************************************************
 * @brief Mesh api of appkey unbind to model
 ****************************************************************************************
 */
omesh_error_t omesh_model_unbind_appkey(uint32_t model_id, uint8_t app_key_lid);

/**
 ****************************************************************************************
 * @brief Mesh api of appkey unbind to all modelss
 ****************************************************************************************
 */
omesh_error_t omesh_all_model_unbind_appkey(uint8_t app_key_lid);

/**
 ****************************************************************************************
 * @brief Mesh api of appkey number bind to model
 ****************************************************************************************
 */
uint8_t omesh_get_model_bind_appkey_nb(uint32_t model_id);

/**
 ****************************************************************************************
 * @brief Mesh api of model publish data
 ****************************************************************************************
 */
omesh_error_t omesh_model_publish(uint32_t model_id, uint32_t appkey_id, uint16_t pub_adr,
                                  uint32_t opcode, bool vendor, uint8_t *p_data, uint16_t p_data_len);

/**
 ****************************************************************************************
 * @brief Mesh api of automatic join into mesh network.
 *          when you call it, will auto restart chip to proved state.
 ****************************************************************************************
 */
omesh_error_t omesh_auto_prov_handle(omesh_auto_prov_t *p_ap_env);

/**
 ****************************************************************************************
 * @brief Mesh api of output stack information
 ****************************************************************************************
 */
void omesh_info_log(void);

/**
 ****************************************************************************************
 * @brief Mesh api of provisioning authentication data requeset indicate handle
 ****************************************************************************************
 */
void omesh_prov_auth_data_req_ind_handle(omesh_api_prov_auth_data_cfm_t *cfm);

/**
 ****************************************************************************************
 * @brief Mesh api of provisioning params requeset indicate handle
 ****************************************************************************************
 */
void omesh_prov_param_req_ind_handle(omesh_api_prov_param_cfm_t *cfm);

/**
 ****************************************************************************************
 * @brief Get publication parameters to be used for a given model instance.
 *
 * @param[in] model_lid        Model LID.
 * @param[in] p_params         Pointer to a buffer that will contain the publication parameters.
 *
 * @return MESH_ERR_NO_ERROR if publication parameters have been written.
 *         MESH_ERR_COMMAND_DISALLOWED if publication parameters have not been set.
 ****************************************************************************************
 */
uint16_t om_tb_mio_get_publi_param(omesh_lid_t model_lid, uint16_t *p_addr, uint8_t* p_va_lid,
        uint8_t *p_app_key_lid, uint8_t *p_ttl, uint8_t *p_period,
        uint8_t *p_retx_params, uint8_t *p_friend_cred);

/**
 ****************************************************************************************
 * @brief BLE mesh security related initialization
 ****************************************************************************************
 */
void omesh_sec_init(void);

/**
 ****************************************************************************************
 * @brief Set IV update mode and ignore 96-hour limit
 *
 * @param[in] update    True if transition to IV Update in Progress state is required, False if
 * require to transit to Normal state
 ****************************************************************************************
 */
void omesh_api_iv_upd_test_mode(bool update);

/**
 ****************************************************************************************
 * @brief Get values for the first page of the Composition Data
 *
 * @param[out] p_cid        Pointer to variable that will contain the 16-bit company identifier
 * assigned by the Bluetooth SIG
 * @param[out] p_cid        Pointer to variable that will contain the 16-bit vendor-assigned
 * product identifier
 * @param[out] p_vid        Pointer to variable that will contain the 16-bit vendor-assigned
 * product version identifier
 * @param[out] p_features   Pointer to variable that will contain supported features
 * @param[out] p_loc        Pointer to variable that will contain localization descriptor
 ****************************************************************************************
 */
void om_tb_state_get_compo_info(uint16_t *p_cid, uint16_t *p_pid, uint16_t *p_vid, uint16_t *p_features, uint16_t *p_loc);

/**
 ****************************************************************************************
 * @brief Allocate a buffer and specify initial length of head, data and tail parts.
 * If total length of the buffer is higher than @see MESH_BUF_LONG_SIZE, the buffer will be
 * dynamically allocated.
 * If total length is lower than @see MESH_BUF_SMALL_SIZE, a small buffer will be allocated.
 * Else a long buffer is used.
 *
 * @param[out] p_buf       Pointer to a variable that will contain the address of the allocated
 *                         buffer.
 * @param[in] head_len     Initial Head Length.
 * @param[in] data_len     Initial Data Length.
 * @param[in] tail_len     Initial Tail Length.
 *
 * @return MESH_ERR_NO_ERROR if buffer can be allocated.
 *         MESH_ERR_NO_RESOURCES if no more buffers are available.
 ****************************************************************************************
 */
uint16_t omesh_api_tb_buf_alloc(void **pp_buf, uint16_t head_len, uint16_t data_len, uint16_t tail_len);

/**
 ****************************************************************************************
 * @brief Request to release previously acquired buffer. The acquire counter for this buffer
 * is decremented. If the acquire counter value becomes zero, the buffer is freed as no more
 * entity is using the buffer anymore.
 *
 * @param[in] p_buf        Pointer to acquired buffer.
 *
 * @return MESH_ERR_NO_ERROR if buffer has been released.
 *         MESH_ERR_COMMAND_DISALLOWED if buffer was free.
 ****************************************************************************************
 */
uint16_t omesh_api_tb_buf_release(void *p_buf);


/*******************************************************************************
 * EXPORTED FUNCTION DEFINITIONS OF MESH MODEL
 */
/**
 ****************************************************************************************
 * @brief Mesh Model API for process receive message
 *
 * @param[in] p_process_queue   Queue of messages to be processed.
 * @param[out] p_rec            Pointer to the variable that will contain the received message. see @omesh_mm_rec_data_t for detail.
 *
 * @return None
 ****************************************************************************************
 */
extern void omesh_model_api_process_receive(omesh_api_list_t *p_process_queue,  omesh_mm_rec_data_t *p_rec);

/**
 ****************************************************************************************
 * @brief Mesh Model API for points to the next message preprocessing
 *
 * @param[in] p_process_queue   Queue of messages to be processed.
 * @param[in] p_djob            Insert message to delay job module for process.
 *
 * @return None
 ****************************************************************************************
 */
extern void omesh_model_api_process_next(omesh_api_list_t *p_process_queue, omesh_djob_t *p_djob);

/**
 ****************************************************************************************
 * @brief Mesh Model API for send ack when recevice message
 *
 * @param[in] p_process_queue   Queue of messages to be processed.
 * @param[in] p_ack             Pointer to the variable that will contain the ack message. see @omesh_mm_send_ack_data_t for detail.
 *
 * @return None
 ****************************************************************************************
 */
extern void omesh_model_api_send_ack(omesh_api_list_t *p_process_queue, omesh_mm_send_ack_data_t *p_ack);

/**
 ****************************************************************************************
 * @brief Mesh Model API for push recevice message into queue
 *
 * @param[in] p_process_queue   Queue of messages to be processed.
 * @param[in] p_api_buf         Pointer to the buffer containing the message PDU.
 * @param[in] p_djob            Insert message to delay job module for process.
 *
 * @return None
 ****************************************************************************************
 */
extern void omesh_model_api_cb_rx(omesh_api_list_t *p_process_queue,  omesh_apis_buf_t *p_api_buf, omesh_djob_t *p_djob);

/**
 ****************************************************************************************
 * @brief Mesh Model API for handle message sent
 *
 * @param[in] p_api_buf         Pointer to the buffer containing the message PDU.
 *
 * @return None
 ****************************************************************************************
 */
extern void omesh_model_api_cb_sent(omesh_apis_buf_t *p_api_buf);

/**
 ****************************************************************************************
 * @brief Mesh Model API for receive message opcode field check
 *
 * @param[in] mdl_lid           Model local index.
 * @param[in] opcode            Opcode field of receive message.
 * @param[in] status            Check result for inform stack manager.
 *
 * @return None
 ****************************************************************************************
 */
extern void omesh_model_api_cb_opcode_check(uint8_t mdl_lid, uint32_t opcode, uint16_t status);

/**
 ****************************************************************************************
 * @brief Mesh Model API for inform the model about a new publish period
 *
 * @param[in] mdl_lid           Model local index.
 * @param[in] period_ms         Publish period in milliseconds.
 *
 * @return None
 ****************************************************************************************
 */
extern void omesh_model_api_cb_publish_period(uint8_t mdl_lid, uint32_t period_ms);

/**
 ****************************************************************************************
 * @brief Register a model
 *
 * @param[in] model_id          Model ID.
 * @param[in] addr_offset       Offset from primary element address.
 * @param[in] vendor            Vendor Model ID or SIG Model ID.
 * @param[in] p_cb              Pointer to callback functions defined by the model
 * @param[out] p_model_lid      Pointer to the variable that will contain the allocated Model LID.
 *
 * @return Execution status code
 ****************************************************************************************
 */
extern uint16_t omesh_model_api_register_model(uint32_t model_id, uint8_t addr_offset, bool vendor, const omesh_api_model_cb_t *p_cb, omesh_lid_t *p_model_lid);

#ifdef __cplusplus
}
#endif

#endif /* __OMSH_APIS_H__ */

/** @} */