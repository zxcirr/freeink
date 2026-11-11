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
 * @file     system_device.h
 * @brief
 * @date     26. Sept 2021
 * @author   OnMicro SW Team
 *
 * @defgroup DOC DOC
 * @ingroup  DOCUMENT
 * @brief    system device declation
 * @details  system device
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


#ifndef __SYSTEM_DEVICE_H
#define __SYSTEM_DEVICE_H

/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include "RTE_OM662x.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (RTE_SYSTEM_USING_ROM_SYMBOL)
#define SystemEnterDeepSleep                rom_SystemEnterDeepSleep
#define SystemExitDeepSleep                 rom_SystemExitDeepSleep
#define SystemFromRomExitDeepSleepEnable    rom_SystemFromRomExitDeepSleepEnable
#define SystemRunTo                         rom_SystemRunTo
#endif

/*******************************************************************************
 * TYPEDEFS
 */
/**
  \brief Exception / Interrupt Handler Function Prototype
*/
typedef void (*VECTOR_TABLE_Type)(void);


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Setup the microcontroller system.
 *
 *******************************************************************************
 */
extern void SystemInit(void);

/**
 *******************************************************************************
 * @brief  system init post
 *******************************************************************************
 */
extern void SystemInitPost(void);

/**
 *******************************************************************************
 * @brief  system init post
 *******************************************************************************
 */
extern void SystemInitPostLib(void);

/**
 *******************************************************************************
 * @brief  system config
 *******************************************************************************
 */
void SystemConfig(void);

/**
 *******************************************************************************
 * @brief  system enter deep sleep
 *******************************************************************************
 **/
extern void SystemEnterDeepSleep(void);

/**
 *******************************************************************************
 * @brief  system exit deep sleep
 *******************************************************************************
 **/
extern void SystemExitDeepSleep(void);

/**
 *******************************************************************************
 * @brief  system from rom exit deep sleep enable
 *******************************************************************************
 **/
extern void SystemFromRomExitDeepSleepEnable(void);

/**
 *******************************************************************************
 * @brief  system run to
 *
 * @param[in] vector_base  vector base
 *******************************************************************************
 */
extern void SystemRunTo(uint32_t vector_base);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_DEVICE_H */


/** @} */
