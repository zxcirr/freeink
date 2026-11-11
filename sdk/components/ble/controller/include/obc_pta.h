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
 * @file     obc_pta.h
 * @brief    PTA
 * @date     01. April 2020
 * @author   OnMicro SW Team
 *
 * @defgroup OBC_PTA PTA
 * @ingroup  OBC
 * @brief    PTA
 * @details  PTA
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __BB_PTA_H
#define __BB_PTA_H


/*******************************************************************************
 * INCLUDES
 */
#include "features.h"

#ifdef  __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */

typedef struct {
    uint8_t grant_active_level;
    uint8_t priority_txrx_include;
    uint8_t priority_txrx_tx_active_level;
    uint8_t priority_prio_active_keep_us;
    uint8_t priority_txrx_active_keep_us;
    uint8_t priority_threshold;

    // priority of eche ble state
    uint8_t priority_state_initiating_connection_ind_rsp;
    uint8_t priority_state_connection_llcp;
    uint8_t priority_state_connection_data_channel;
    uint8_t priority_state_initiating_scanning;
    uint8_t priority_state_active_scanning;
    uint8_t priority_state_connectable_advertising;
    uint8_t priority_state_non_connectable_advertising;
    uint8_t priority_state_passive_scanning;
} obc_pta_ctrl_t;

/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief  bb pta enable
 *
 * @detail
 *
 * SiliconLab PTA:
 *
 * 4line:
 *   grand      蓝牙端输入，WLAN TX工作时请求蓝牙关闭
 *   request    蓝牙端输出，同蓝牙的TXEN/RXEN
 *   priority   蓝牙端输出，蓝牙priority_state_xxx大于priority_threshold时产生高电平。request后延时priority_prio_active_keep_us之后输出是TX还是RX
 *   freq       蓝牙端输出，当蓝牙频率出现2412/2417/2422/2427/2432/2437/2442/2447/2452/2457/2462/2467/2472/2484MHz时输出高
 *
 *
 * @param[in] enable  enable
 * @param[in] ctrl  ctrl
 *******************************************************************************
 */
void obc_pta_enable(bool enable, const obc_pta_ctrl_t *ctrl);


#ifdef  __cplusplus
}
#endif

#endif  /* __EVT_H */

/** @} */

