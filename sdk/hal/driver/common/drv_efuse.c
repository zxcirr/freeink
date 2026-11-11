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
 * @file     drv_gpio.c
 * @brief
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
#if (RTE_EFUSE)
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
// For ROM inside
bool efuse_write_calib_section_disable = false;

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
/**
 * @brief  efuse power enable
 *
 * @param[in] enable  enable
 **/
static void drv_efuse_write_power_enable(bool enable)
{
    // TODO
}

static inline uint32_t _cal_timing(uint32_t efuse_clk, uint16_t timing_param)
{
    return (((efuse_clk / 1000000) * timing_param) / 1000 + 1);
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief  efuse open
 **/
void drv_efuse_init(void)
{
    uint32_t efuse_clk;

    DRV_RCC_CLOCK_ENABLE(RCC_CLK_EFUSE, 1U);

    efuse_clk = drv_rcc_clock_get(RCC_CLK_CPU);

    OM_EFUSE->AVDD_TIMING_CFG = _cal_timing(efuse_clk, EFUSE_AVDD_TIMING_NS);
    OM_EFUSE->PROGRAM_CFG0    = (_cal_timing(efuse_clk, EFUSE_PROGRAM_CFG0_L16_NS)) | (_cal_timing(efuse_clk, EFUSE_PROGRAM_CFG0_H16_NS) << 16);
    OM_EFUSE->PROGRAM_CFG1    = (_cal_timing(efuse_clk, EFUSE_PROGRAM_CFG1_L16_NS)) | (_cal_timing(efuse_clk, EFUSE_PROGRAM_CFG1_H16_NS) << 16);
    OM_EFUSE->PROGRAM_CFG2    = _cal_timing(efuse_clk, EFUSE_PROGRAM_CFG2_L16_NS);
    OM_EFUSE->READ_CFG        = (_cal_timing(efuse_clk, EFUSE_READ_CFG_L8_NS))
                              | ((_cal_timing(efuse_clk, EFUSE_READ_CFG_L16_NS) + _cal_timing(efuse_clk, EFUSE_READ_CFG_L8_NS)) << 8)
                              | (_cal_timing(efuse_clk, EFUSE_READ_CFG_H24_NS) << 16)
                              | (_cal_timing(efuse_clk, EFUSE_READ_CFG_H32_NS) << 24);
}

/**
 * @brief  efuse write
 *
 * @note
 *
 * The default value of efuse is 0, and any bit can be changed from 0 to 1,
 * multiple write operations can be performed.
 *
 * The last 128bit is CP calibration data, don't touch it.
 *
 * @param[in] addr  address
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_efuse_write(uint32_t addr, const void *data, uint32_t length)
{
    const uint8_t *pdata = data;

    drv_efuse_write_power_enable(true);  //lint !e522 !e523

    // enable efuse program
    OM_EFUSE->PROGRAM_ENABLE = 1;
    while(!(OM_EFUSE->STATUS & EFUSE_PROGRAM_START_MASK));

    for (uint32_t i = 0; i < length; i ++)
    {
        if (efuse_write_calib_section_disable && addr >= EFUSE_USER_SECTION_SIZE)
            break;

        OM_EFUSE->PROGRAM_ADDRESS = addr++;
        OM_EFUSE->PROGRAM_DATA = pdata[i];

        OM_EFUSE->PROGRAM_START |= EFUSE_PROGRAM_START_MASK;
        while(OM_EFUSE->PROGRAM_START & EFUSE_PROGRAM_START_MASK);
    }

    // disable efuse
    OM_EFUSE->PROGRAM_ENABLE = 0;

    drv_efuse_write_power_enable(false);  //lint !e522 !e523
}

/**
 * @brief  opt read
 *
 * @param[in] addr  address
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_efuse_read(uint32_t addr, void *data, uint32_t length)
{
    memcpy(data, (void*)(OM_EFUSE->READ_DATA + addr), length);
}

/**
 *******************************************************************************
 * @brief  drv efuse iflash encrypt fetch
 *******************************************************************************
 */
void drv_efuse_iflash_encrypt_uid_fetch(void)
{
    // fetch
    REGW1(&OM_EFUSE->CTRL, EFUSE_CTRL_UID_READ_MASK);
    // wait fetch OK
    while(OM_EFUSE->CTRL & EFUSE_CTRL_UID_READ_MASK);
}

#endif

/** @} */

