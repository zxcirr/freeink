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
 * @file     shell.h
 * @brief    shell
 * @date     03 Apr. 2020
 * @author   OnMicro SW Team
 *
 * @defgroup SHELL
 * @ingroup  Shell
 * @brief    shell
 * @details  shell header file
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __SHELL_H
#define __SHELL_H


/*******************************************************************************
 * INCLUDES
 */
#include <stdbool.h>
#include <stdarg.h>
#include "shell_cmd.h"
#include "shell_port.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
// shell maximum arguments per command
#ifndef SHELL_MAX_ARGUMENTS
#define SHELL_MAX_ARGUMENTS                    (80)
#endif

#define SHELL_CMD_SPLIT             "split"
#define SHELL_SPLIT_LINE(mod)       {SHELL_CMD_SPLIT, NULL, mod}


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief   Reads a whole line from the input channel.
 *
 * @param[in] line        pointer to the line buffer
 * @param[in] line_size   buffer maximum length
 *
 * @return                The operation status.
 * @retval true           the channel was reset or CTRL-D pressed.
 * @retval false          operation successful.
 *******************************************************************************
 */
extern bool shell_get_line(char c, char *line, unsigned line_size, unsigned *line_index);

/**
 *******************************************************************************
 * @brief   Shell thread function.
 *
 * @param[in] line           pointer to the line buffer
 * @param[in] scp            pointer to the shell command
 *
 *******************************************************************************
 */
extern void shell_main(char *line, const shell_cmd_t *scp);


#ifdef __cplusplus
}
#endif


#endif  /*__SHELL */


/** @} */
