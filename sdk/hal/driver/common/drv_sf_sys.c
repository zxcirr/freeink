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
 * @file     drv_sf_sys.c
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

/*********************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_SF_SYS) && (!RTE_SF_SYS_USING_ROM_SYMBOL)
#include "om_driver.h"
#include "om.h"

/*********************************************************************
 * MACROS
 */
// xflash as flash memory
#define DRV_SFS_XFLASH_MISO_PIN 7
#define DRV_SFS_XFLASH_CSN_PIN  8
#define DRV_SFS_XFLASH_CLK_PIN  9
#define DRV_SFS_XFLASH_MOSI_PIN 10

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * CONSTANTS
 */
static OM_SF_Type * const drv_sfs_regobj_tbl[DRV_SFS_LOCATE_NUM] = {OM_SF, OM_SF};

#if 1
#define DRV_SFS_CS_DECLARE(cs_locate)    uint32_t cs = (uint32_t)(cs_locate)
#else
#define DRV_SFS_CS_DECLARE(cs_locate)    const uint32_t cs = 0;
#endif

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
drv_sfs_env_t drv_sfs_env = {
    /* locate     */    DRV_SFS_IFLASH,
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

#if 0
/**
 * @brief drv_sfs_xflash_quad_pinmux()
 *
 * @return
 **/
static void drv_sfs_xflash_quad_pinmux(void)
{
    drv_pmu_pin_mode_set(BITMASK(DRV_SFS_XFLASH_WP_PIN), PMU_PIN_MODE_PU);
    drv_pmu_pin_mode_set(BITMASK(DRV_SFS_XFLASH_HD_PIN), PMU_PIN_MODE_PU);

    drv_pin_mux_set(DRV_SFS_XFLASH_WP_PIN, PINMUX_SFLASH1_WP_CFG);
    drv_pin_mux_set(DRV_SFS_XFLASH_HD_PIN, PINMUX_SFLASH1_HD_CFG);
}
#endif

/**
 * @brief drv_sfs_xflash_pinmux()
 *
 * @return
 **/
static void drv_sfs_xflash_pinmux(void)
{
    drv_pmu_pin_mode_set(BITMASK(DRV_SFS_XFLASH_MISO_PIN), PMU_PIN_MODE_PD);
    drv_pmu_pin_mode_set(BITMASK(DRV_SFS_XFLASH_CSN_PIN),  PMU_PIN_MODE_PU);

    DRV_PIN_MUX_SET(DRV_SFS_XFLASH_MISO_PIN, PINMUX_PAD7_SFLASH_SO_CFG);
    DRV_PIN_MUX_SET(DRV_SFS_XFLASH_CSN_PIN,  PINMUX_PAD8_SFLASH_CS_CFG);
    DRV_PIN_MUX_SET(DRV_SFS_XFLASH_CLK_PIN,  PINMUX_PAD9_SFLASH_CK_CFG);
    DRV_PIN_MUX_SET(DRV_SFS_XFLASH_MOSI_PIN, PINMUX_PAD10_SFLASH_SI_CFG);
}

#if 0
/**
 * @brief  sfs xflash power on
 **/
static void drv_sfs_xflash_power_on(void)
{
    // mode,driven,pinmux
    drv_pmu_pin_mode_set(BITMASK(DRV_SFS_XFLASH_PWR_PIN), PMU_PIN_MODE_PP);
    drv_pmu_pin_driven_current_set(BITMASK(DRV_SFS_XFLASH_PWR_PIN), PMU_PIN_DRIVEN_CURRENT_MAX);
    drv_pin_mux_set(DRV_SFS_XFLASH_PWR_PIN, PINMUX_GPIO_MODE_CFG);

    // init gpio
    gpio_open_clock();
    drv_gpio_write(OM_GPIO0, BITMASK(DRV_SFS_XFLASH_PWR_PIN), GPIO_HIGH);
    gpio_set_direction(BITMASK(DRV_SFS_XFLASH_PWR_PIN), GPIO_OUTPUT);

    // wait ready
    DRV_DELAY_MS(10);
}
#endif

/**
 * @brief  sfs xflash open
 **/
static void drv_sfs_xflash_open(void)
{
#if 0
    drv_sfs_xflash_power_on();
#endif

    drv_sfs_xflash_pinmux();
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief sf raw opration
 *
 * JUST FOR TEST
 *
 * @param[in] locate  0:inside 1:outside 0xFF:auto
 * @param[in] ctrl  1:read 2:write
 * @param[in] cmd  sf command
 * @param[in] cmd_bits  sf command bits
 * @param[in] data  sf data
 * @param[in] data_bytes  sf data bytes
 *
 * @return errno
 **/
void drv_sfs_raw_op(uint8_t locate, uint8_t ctrl,
                uint32_t cmd[2], uint8_t cmd_bits,
                void *data, uint16_t data_bytes)
{
    drv_sfb_rw_params_t param;
    OM_SF_Type *sf = locate < DRV_SFS_LOCATE_NUM ? drv_sfs_regobj_tbl[locate] : drv_sfs_cur_obj();
    DRV_SFS_CS_DECLARE(locate);

    param.cmd[0] = cmd[0];
    param.cmd[1] = cmd[1];
    param.cmd_bits = cmd_bits;
    param.data = data;
    param.data_bytes = data_bytes;

    if(ctrl == 1) // read
        drv_sfb_read_nodma(sf, cs, &param);
    else // write
        drv_sfb_write_nodma(sf, cs, &param);
}

/**
 * @brief config
 *
 * @param[in] freq_hz  frequency in Hz
 * @param[in] width  width
 * @param[in] delay  delay
 **/
void drv_sfs_config(uint32_t freq_hz, drv_sf_width_t width, uint8_t delay)
{
    drv_sf_config_t sfrconfig;

#if 0
    if (width==DRV_SF_WIDTH_4LINE && drv_sfs_env.locate==DRV_SFS_XFLASH)
        drv_sfs_xflash_quad_pinmux();
#endif

    sfrconfig.freq_hz = freq_hz;
    sfrconfig.width = width;
    sfrconfig.delay = delay;
    drv_sf_config(DRV_SFS_CUR_OBJ, &sfrconfig);
}

/**
 * @brief probe
 *
 * @param[in] locate  locate
 * @param[in] freq_hz  frequency in Hz
 *
 * @return errno
 **/
int drv_sfs_probe(drv_sfs_locate_t locate, uint32_t freq_hz)
{
    drv_sf_config_t sfrconfig;
    OM_SF_Type *sf = drv_sfs_regobj_tbl[locate];
    DRV_SFS_CS_DECLARE(locate);
    drv_sf_status_t status = drv_sf_status(sf, cs);
    bool detected;

    if (status == DRV_SF_STATUS_ABSENT)
    {
        return -ENODEV;
    }
    else if (status == DRV_SF_STATUS_PRESENT)
    {
        drv_sfs_env.locate = locate;
        return 0;
    }

    // pinmux for xflash
    if(locate == DRV_SFS_XFLASH)
        drv_sfs_xflash_open();

    // open
    drv_sf_enable(sf, cs);

    // config
    sfrconfig.freq_hz = freq_hz;
    sfrconfig.delay = DRV_SF_DELAY_DEFAULT;
    sfrconfig.width = DRV_SF_WIDTH_1LINE;
    drv_sf_config(sf, cs, &sfrconfig);

    // detect
    detected = drv_sf_detect(sf, cs);

    if (!detected)
    {
        // try leave lowpower mode
        drv_sf_deep_power_down_leave(sf, cs);

        // re-detect
        detected = drv_sf_detect(sf, cs);
    }

    if (!detected)
    {
        // iflash power ctrl immediately
        drv_sf_iflash_power_ctrl_immediate_enable(true);

        // disable SF
        drv_sf_disable(sf, cs);

        // delay 50ms
        DRV_DELAY_MS(50);

        // open
        drv_sf_enable(sf, cs);

        // delay 10ms
        DRV_DELAY_MS(10);

        // iflash
        drv_sf_iflash_power_ctrl_immediate_enable(false);

        // re-detect
        detected = drv_sf_detect(sf, cs);
    }

    if (detected)
    {
        drv_sfs_env.locate = locate;
        return 0;
    }
    else
    {
        return -ENODEV;
    }
}

/**
 * @brief select
 *
 * @param[in] locate  locate
 *
 * @return errno
 **/
int drv_sfs_select(drv_sfs_locate_t locate)
{
    DRV_SFS_CS_DECLARE(locate);

    if(drv_sf_status(drv_sfs_regobj_tbl[locate], cs) != DRV_SF_STATUS_PRESENT)
        return -ENODEV;

    drv_sfs_env.locate = locate;
    return 0;
}

/**
 * @brief  sfs located
 *
 * @return located
 **/
drv_sfs_locate_t drv_sfs_located(void)
{
    return drv_sfs_env.locate;
}

/**
 * @brief  sfs cur obj
 *
 * @return
 **/
OM_SF_Type *drv_sfs_cur_obj(void)
{
    return drv_sfs_regobj_tbl[drv_sfs_env.locate];
}

#endif  /* (RTE_SF_SYS) */

/** @} */

