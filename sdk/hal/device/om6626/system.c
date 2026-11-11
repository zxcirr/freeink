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
 * @file     system_om6681a_core1.c
 * @brief    CMSIS Device System Source File for OM6681A core1
 * @date     7. Jan. 2022
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
#include "om_device.h"
#include "om_driver.h"


/**
 *******************************************************************************
 * @brief Exception / Interrupt Vector table
 *******************************************************************************
 */
extern const VECTOR_TABLE_Type __VECTOR_TABLE[];    /*lint !e526*/


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief System initialization function, called before init ".data", ".bss" and "other section"
 *******************************************************************************
 */
void SystemInit(void)
{
    #if defined(CONFIG_XIP_FLASH_ALL)
    drv_icache_enable();
    #endif

    #if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    SCB->VTOR = (uint32_t) & (__VECTOR_TABLE[0]);
    #endif

    #if defined (__FPU_USED) && (__FPU_USED == 1U)
    SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                   (3U << 11U*2U)  );         /* enable CP11 Full Access */
    #endif

    #ifdef UNALIGNED_SUPPORT_DISABLE
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
    #endif

    #if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    TZ_SAU_Setup();
    #endif

    drv_pmu_init();
}

/**
 *******************************************************************************
 * @brief  system init post, called after init ".data", ".bss" and "other section"
 *
 * @note  This may be implemented in the system library to import data such as CP/FT calibration.
 *******************************************************************************
 */
void SystemInitPost(void)
{
#if defined(CONFIG_LIB_USE)
    // Use lib SystemInitPost
    //SystemInitPostLib();
		SystemConfig();
#elif defined(CONFIG_CPFT_DATA_FETCH)
    // CPFT data
    //drv_cpft_init();
    // Config system
    //SystemConfig();
    // CPFT data
    //drv_cpft_uninit();
#else
    // Config system
    //SystemConfig();
#endif
}

/**
 *******************************************************************************
 * @brief  system config, called in SystemInitPost
 *******************************************************************************
 */
void SystemConfig(void)
{
    // Init PM
    pm_init();

    // Init Pmu timer
    drv_pmu_timer_init();

    // Initialize random process
    #if (RTE_RNG)
    srand(drv_rng_get());
    #endif

    // switch to xtal
    drv_pmu_xtal32m_change_param(36); // Change XTAL32M load capacitance
    drv_pmu_xtal32m_startup();        // TOP clock is 32MHz. If xtal32m is used, this function must be called before other functions that depend on xtal32m.
    //drv_pmu_xtal32m_x2_startup();   // TOP clock is 64MHz. This function depend on xtal32m.

    // Init flash
    drv_sfs_probe(DRV_SFS_IFLASH, 2/*freq*/);
    drv_sfs_config(1/*freq*/, DRV_SF_WIDTH_4LINE, DRV_SF_DELAY_AUTO);
}

/** @} */
