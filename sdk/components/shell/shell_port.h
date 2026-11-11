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
 * @file     shell_cmd.h
 * @brief    shell system command
 * @date     18 Feb. 2022
 * @author   OnMicro SW Team
 *
 * @defgroup SHELL_PORT SHELL_PORT
 * @ingroup  Shell
 * @brief    templete
 * @details  shell system command
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __SHELL_PORT_H
#define __SHELL_PORT_H


/*******************************************************************************
 * INCLUDES
 */
#include "om_device.h"
#include "shell_cmd.h"
#include "om_printf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * MACRO
 */
#ifdef CONFIG_SHELL
#define shell_printf(format, ...)       om_printf(format,  ## __VA_ARGS__)
#else
#define shell_printf(format, ...)
#endif

/*******************************************************************************
 * EXTERN FUNCTIONS
 */

/**
 *******************************************************************************
 * @brief      Initializer shell
 *
 * @param[in]   shell_cmd    Pointer to shell command
 *******************************************************************************
 */
#ifdef CONFIG_SHELL
void shell_init(const shell_cmd_t *shell_cmd);
#else
__STATIC_INLINE void shell_init(const shell_cmd_t *shell_cmd) {}
#endif

/**
 *******************************************************************************
 * @brief Output a character to shell
 *
 * @param[in]  Character to shell
 *******************************************************************************
 */
void shell_out(char c);

#ifdef __cplusplus
}
#endif

#endif  /* __SHELL_PORT_H */


/** @} */
