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
 * @file     trc_io.c
 * @brief    event
 * @date     01. April 2020
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
#include "om_device.h"
#include "trc_io.h"
#include "om_driver.h"

#ifdef CONFIG_TRACE_IO

/*********************************************************************
 * MACROS
 */
#define TRC_IO_PIN(pin)             ((pin) & 0x3F)
#define TRC_IO_PIN_REVERSE(pin)     ((pin) | 0x80)
#define TRC_IO_PIN_NOREVERSE(pin)   ((pin) & 0x3F)
#define TRC_IO_PIN_IS_REVERSED(pin) ((pin) & 0x80)

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * VARIABLES
 */
static uint8_t trc_io_evt2io_tbl[TRC_IO_EVENT_NUM] = {0};
trc_io_event_callback_t trc_io_event_callback = NULL;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 *******************************************************************************
 * @brief  trc io event handler
 *
 * @param[in] event  event
 * @param[in] is_on  is on
 *******************************************************************************
 */
static void trc_io_event_handler(trc_io_event_t event, int is_on)
{
    if(trc_io_evt2io_tbl[event] != 0)
    {
        uint32_t pin_mask = 1u << TRC_IO_PIN(trc_io_evt2io_tbl[event]);
        uint32_t idle_level = TRC_IO_PIN_IS_REVERSED(trc_io_evt2io_tbl[event]) ? (~0u) : (0u);
        drv_gpio_write(OM_GPIO0, pin_mask, (gpio_level_t)(is_on?(~idle_level):(idle_level)));
    }
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 *******************************************************************************
 * @brief  trc io set
 *
 * @param[in] event  event
 * @param[in] pin  pin
 *******************************************************************************
 */
void trc_io_set(trc_io_event_t event, uint8_t pin)
{
    OM_ASSERT(event < TRC_IO_EVENT_NUM);

    // callabck
    trc_io_event_callback = trc_io_event_handler;

    // Write new configuration
    trc_io_evt2io_tbl[event] = pin;

    // default is off
    trc_io_event_handler(event, 0);
}

/*********************************************************************
 * GLOBAL VARIABLES
 */

#endif

/** @} */

