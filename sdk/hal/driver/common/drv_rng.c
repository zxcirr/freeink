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
 * @file     drv_rng.c
 * @brief    source file for rng
 * @date     3 Feb 2023
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
#if (RTE_RNG)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"

/*******************************************************************************
 * MACROS
 */

#define CONFIG_RNG_USE_RC32M
#define CONFIG_RNG_CHANGE_RC32K_TUNE

#define RC32K_RCTUNE_STORE()                                \
    do {                                                    \
        uint8_t _rtune  = REGR(&OM_PMU->CLK_CTRL_2, MASK_POS(PMU_CLK_CTRL_2_RTUNE_RC32K_REG));  \
        uint8_t _ctune  = REGR(&OM_PMU->CLK_CTRL_2, MASK_POS(PMU_CLK_CTRL_2_CTUNE_RC32K_REG));


#define RC32K_RCTUNE_RESTORE()                              \
        rng_modify_rc32k_rctune(_rtune, _ctune);            \
    } while (0)

#define RC32K_RCTUNE_SET0()                                 \
        rng_modify_rc32k_rctune(0, 0);

/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * CONST & VARIABLES
 */


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#ifdef CONFIG_RNG_CHANGE_RC32K_TUNE
static void rng_modify_rc32k_rctune(uint8_t rtune, uint8_t ctune)
{
    REGW(&OM_PMU->CLK_CTRL_2, MASK_1REG(PMU_CLK_CTRL_2_RTUNE_RC32K_REG, rtune));
    REGW(&OM_PMU->CLK_CTRL_2, MASK_1REG(PMU_CLK_CTRL_2_CTUNE_RC32K_REG, ctune));
    REGW(&OM_PMU->CLK_CTRL_1, MASK_1REG(PMU_CLK_CTRL_1_RCTUNE_RC32K_UPDATE_REG, 1));
    while(OM_PMU->STATUS_READ & PMU_STATUS_READ_RCTUNE_RC32K_UPDATE_STATUS_MASK);
}
#endif

/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Get random numbers
 *
 * @return Random numbers
 *******************************************************************************
 */
uint32_t drv_rng_get(void)
{
    uint32_t random_val;

    bool is_using_rc32k = drv_pmu_select_32k_get() == PMU_32K_SEL_RC;
    bool is_using_xtal32m = drv_pmu_topclk_xtal32m_is_enabled();

#ifdef CONFIG_RNG_USE_RC32M
    if (is_using_xtal32m) {
        drv_pmu_topclk_rc32m_power_enable(true);
        drv_pmu_topclk_switch_to_rc32m();
    }
#endif

#ifdef CONFIG_RNG_CHANGE_RC32K_TUNE
    if (is_using_rc32k && is_using_xtal32m) {
        drv_pmu_select_32k(PMU_32K_SEL_DIV);
    }
    RC32K_RCTUNE_STORE();
    RC32K_RCTUNE_SET0();
#endif

    DRV_RCC_CLOCK_ENABLE(RCC_CLK_RNG, 1U);

    random_val = 0;
    for (uint8_t i = 0; i < 32; i++) {
        random_val |= ((OM_RNG->RANDOM & 0x1) << i);
        drv_dwt_delay_us(300);
    }

    DRV_RCC_CLOCK_ENABLE(RCC_CLK_RNG, 0U);

#ifdef CONFIG_RNG_CHANGE_RC32K_TUNE
    RC32K_RCTUNE_RESTORE();
    if (is_using_rc32k && is_using_xtal32m) {
        drv_pmu_32k_switch_to_rc(false, false);
    }
#endif

#ifdef CONFIG_RNG_USE_RC32M
    if (is_using_xtal32m) {
        drv_pmu_topclk_switch_to_xtal32m();
        drv_pmu_topclk_rc32m_power_enable(false);
    }
#endif

    return random_val;
}


#endif  /* RTE_RNG */

/** @} */
