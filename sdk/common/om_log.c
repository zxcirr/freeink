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
 * @file     om_log.c
 * @brief
 * @date     06. Aug 2020
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

/*********************************************************************
 * INCLUDES
 */
#include "features.h"
#include "om_log.h"
#include <stdint.h>
#include <stdarg.h>

#if (CONFIG_LOG)

#if (CONFIG_SEGGER)
#include "SEGGER_SYSVIEW.h"
#endif

#if (CONFIG_LOG_TO_SHELL)
#include "shell.h"
#endif

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static om_log_level_t om_log_level = (om_log_level_t)CONFIG_LOG_LEVEL;

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief  om log
 *
 * @param[in] level  level
 * @param[in] fmt  fmt
 **/
void om_log(om_log_level_t level, const char *fmt, ...)
{
    /*lint -save -e530*/
    va_list ap;

    va_start(ap, fmt);
    if (om_log_level >= level) {
#if (CONFIG_LOG_TO_SEGGER_SYSVIEW)
        uint32_t om2sysview_level_tbl[] = [0, SEGGER_SYSVIEW_ERROR, SEGGER_SYSVIEW_WARNING, SEGGER_SYSVIEW_LOG];
        SEGGER_SYSVIEW_VPrintfTarget(fmt, om2sysview_level_tbl[level], &ap);
#elif (CONFIG_LOG_TO_SHELL)
        om_vprintf(fmt, ap);
#endif
    }
    va_end(ap);
    /*lint -restore */
}

/**
 * @brief  om log level set
 *
 * @param[in] level  level
 **/
void om_log_level_set(om_log_level_t level)
{
    om_log_level = level;
}

#else
void om_log(om_log_level_t level, const char *fmt, ...) {}
void om_log_level_set(om_log_level_t level) {}
#endif

/** @} */

