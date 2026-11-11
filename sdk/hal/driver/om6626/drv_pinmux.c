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
 * @file     pin_om662x.c
 * @brief    pin driver
 * @date     14. December 2021
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_PIN)
#include <stdint.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * CONST & VARIABLES
 */


/*******************************************************************************
 * LOCAL FUNCTIONS
 */



/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Initialize pin function
 *
 * @param[in] pin      pin
 * @param[in] func     function
 *
 *******************************************************************************
 */
void drv_pin_mux_set(uint8_t pin, pin_func_t func)
{
    uint32_t i, p;

    OM_ASSERT(pin < DRV_PIN_NUM);

    if (func.pin_func < PINMUX_COMMON_PERIPHERAL_CFG) {     // common peripheral
        i = pin / 4;
        p = (pin % 4) * 8;

        register_set(&OM_SYS->PINMUX[i], SYS_PINMUX_MASK(func.pin_func, p));
    } else {
        if (func.pin_func == PINMUX_LP_TIMER_OUT0_CFG || func.pin_func == PINMUX_LP_TIMER_OUT1_CFG) {
            OM_PMU->LP_TIMER_OUT_CTRL |= BITMASK(pin);
        }
    }
}

/**
 *******************************************************************************
 * @brief Initialize pin function and electrical characteristics
 *
 * @param[in] pin_cfg      Configuration for pinmux
 * @param[in] pin_cfg_num      Configuration number for pinmux
 *
 *******************************************************************************
 */
void drv_pin_init(const pin_config_t *pin_cfg, uint32_t pin_cfg_num)
{
    uint32_t i;

    OM_ASSERT(pin_cfg);
    for (i = 0; i < pin_cfg_num; ++i) {
        /*lint -save -e613 */
        drv_pin_mux_set(pin_cfg[i].dig_pad, pin_cfg[i].func);
        drv_pmu_pin_mode_set(BITMASK(pin_cfg[i].dig_pad), pin_cfg[i].mode);
        drv_pmu_pin_driven_current_set(BITMASK(pin_cfg[i].dig_pad), pin_cfg[i].drv);
        /*lint -restore */
    }
}


#endif  /* RTE_PIN */


/** @} */
