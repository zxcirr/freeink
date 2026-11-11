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
 * @file     obc_hci_h4.h
 * @brief    obc_hci_h4
 * @date     15 December 2021
 * @author   OnMicro SW Team
 *
 * @defgroup obc_hci_h4 obc_hci_h4
 * @ingroup  OBC
 * @brief    obc_hci_h4 Driver
 * @details  obc_hci_h4 Driver

 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef H4TL_H_
#define H4TL_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>       // standard integer definition
#include <stdbool.h>      // standard boolean definition

/*
 * DEFINES
 ****************************************************************************************
 */

/**
 *******************************************************************************
 * @brief  obc hci h4 transmit callback  type
 *
 * @param[in] pdata  pdata
 * @param[in] length  length
 *******************************************************************************
 */
typedef void (*obc_hci_h4_transmit_callback_t)(const uint8_t *pdata, uint32_t length);

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 *******************************************************************************
 * @brief  obc hci h4 receive handler
 *
 * @param[in] pdata  pdata
 * @param[in] length  length
 *******************************************************************************
 */
void obc_hci_h4_receive_handler(const uint8_t *pdata, uint32_t length);

/**
 *******************************************************************************
 * @brief  obc hci h4 transmit callback register
 *
 * @param[in] cb  cb
 *******************************************************************************
 */
void obc_hci_h4_transmit_callback_set(obc_hci_h4_transmit_callback_t cb);

#endif
/// @} H4TL
