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
 * @file     autoconf_preset.h
 * @brief    autoconf_preset for om662x
 * @date     26. Aug. 2021
 * @author   OnMicro SW Team
 *
 * @defgroup DOC DOC
 * @ingroup  DOCUMENT
 * @brief    templete
 * @details  RTE device for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __AUTOCONF_PRESET_H
#define __AUTOCONF_PRESET_H

/*******************************************************************************
 * MACROS
 */
#if defined(CONFIG_OM6626A)

#if defined(CONFIG_LIB_PRESET_NO_BLE)
#include "../om6626/rom_lib/current/config/autoconf_preset_no_ble.h"

#elif defined(CONFIG_LIB_PRESET_BLE_FULL)
#include "../om6626/rom_lib/current/config/autoconf_preset_ble_full.h"

#elif defined(CONFIG_LIB_PRESET_BLE_1PERIPHERAL)
#include "../om6626/rom_lib/current/config/autoconf_preset_ble_1peripheral.h"

#elif defined(CONFIG_LIB_PRESET_BLE_VELA_MESH)
#include "../om6626/rom_lib/current/config/autoconf_preset_ble_vela_mesh.h"

#elif defined(CONFIG_LIB_PRESET_BLE_MESH)
#include "../om6626/rom_lib/current/config/autoconf_preset_ble_mesh.h"

#elif defined(CONFIG_PROJECT_ROM_BOOT)
#include "autoconf_preset_romboot.h"

#else
#error "Lost autoconf preset macro defined"

#endif

#endif

#endif  /* __AUTOCONF_PRESET_H */


/** @} */
