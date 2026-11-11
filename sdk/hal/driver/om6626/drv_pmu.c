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
 * @file     pmu.c
 * @brief    pmu driver
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
#if (RTE_PMU)
#include <stddef.h>
#include "om.h"
#include "om_driver.h"

/*******************************************************************************
 * MACROS
 */
#define CONFIG_IO_OUTPUT_LATCH_CTRL_BY_SOFTWARE

/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * CONST & VARIABLES
 */
pmu_env_t drv_pmu_env;

/*******************************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief drv_pmu_gpio_lowpower_is_ready()
 *
 * @return
 **/
__RAM_CODES("PM")
static bool drv_pmu_gpio_lowpower_is_ready(void)
{
    uint32_t pin_wakeup  = OM_PMU->GPIO_WAKEUP;
    bool is_pin_ready = true;

    if (pin_wakeup)
    {
        uint32_t pin_wakeup_cur_level = drv_gpio_read(OM_GPIO0, pin_wakeup);

        if (drv_pmu_env.pin_wakeup_double_edge_mask)
        {
            uint32_t pin_wakeup_cur_level_double_edge = pin_wakeup_cur_level & drv_pmu_env.pin_wakeup_double_edge_mask;
            REGW(&OM_PMU->GPIO_POL, drv_pmu_env.pin_wakeup_double_edge_mask, pin_wakeup_cur_level_double_edge);
        }

        if (drv_pmu_env.pin_wakeup_none_edge_mask)
        {
            uint32_t pin_wakeup_cur_level_none_edge = pin_wakeup_cur_level & drv_pmu_env.pin_wakeup_none_edge_mask;
            uint32_t pin_wakeup_sleep_level_none_edge = OM_PMU->GPIO_POL & drv_pmu_env.pin_wakeup_none_edge_mask;
            if (pin_wakeup_cur_level_none_edge != pin_wakeup_sleep_level_none_edge)
                is_pin_ready = false;
        }
    }

    return is_pin_ready;
}

/**
 * @brief  pmu pin input enable
 *
 * @param[in] enable_mask  enable mask
 **/
static void drv_pmu_pin_input_enable(uint32_t enable_mask)
{
    REGW(&OM_PMU->GPIO_IE_CTRL, MASK_1REG(PMU_GPIO_IE_CTRL_GPIO_IE_CTRL, enable_mask));
}

/**
 * @brief  pmu select 32k get from reg
 *
 * @return 32k
 **/
//static pmu_32k_sel_t drv_pmu_select_32k_get_reg(void)
//{
//    return (pmu_32k_sel_t)REGR(&OM_PMU->MISC_CTRL, MASK_POS(PMU_MISC_CTRL_CLK_32K_SEL));
//}

/**
 * @brief  pmu 32k switch to rc
 *
 * @param[in] calib  calib
 * @param[in] pd_others  pd others
 **/
void drv_pmu_32k_switch_to_rc(bool calib, bool pd_others)
{
    // Power on rc32k
//    REGW0(&OM_PMU->MISC_CTRL, PMU_MISC_REG_PD_RC32K_MASK);
//    while(!(OM_PMU->STATUS_READ & PMU_STATUS_READ_RC32K_READY_MASK));

    // calib it
    if(calib)
        drv_calib_rc32k();

    // Switch
    REGW(&OM_PMU->MISC_CTRL, MASK_1REG(PMU_MISC_CTRL_CLK_32K_SEL, PMU_32K_SEL_RC));
    while((OM_PMU->STATUS_READ & (PMU_STATUS_READ_CLK_32K_RC_CRY_DONE_MASK|PMU_STATUS_READ_CLK_32K_DIV_DONE_MASK)) !=
                                 (PMU_STATUS_READ_CLK_32K_RC_CRY_DONE_MASK|PMU_STATUS_READ_CLK_32K_DIV_DONE_MASK));

    if (pd_others)
    {
        REGW1(&OM_PMU->MISC_CTRL, PMU_MISC_CTRL_PD_CRY32K_MASK); // xtal32k
    }
}

/**
 * @brief drv_pmu_wakeup_pin_wait_idle()
 *
 * @return
 **/
__RAM_CODES("PM")
static void drv_pmu_wakeup_pin_wait_idle(void)
{
    while(OM_PMU->MISC_CTRL & PMU_MISC_CTRL_CLR_PMU_INT_MASK);
    while(OM_PMU->STATUS_READ & PMU_STATUS_READ_CLR_PMU_INT_SYNC_APB_MASK);
}

#if 0
/**
 * @brief  pmu xtal32m is keep on
 *
 * @return on ?
 **/
static bool pmu_topclk_xtal32m_is_keep_on(void)
{
    return (OM_PMU->MISC_CTRL_1 & PMU_MISC_CTRL_1_CRY32M_KEEP_ON_MASK) ? true : false;
}
#endif

/**
 * @brief  pmu topclk xtal32m wait ready
 **/
__RAM_CODES("PM")
static void drv_pmu_topclk_xtal32m_wait_ready(void)
{
    // WAIT Ready
    while(!(OM_DAIF->XTAL32M_INTRS & DAIF_XTAL32M_CLK_RDY_MASK));

//    // Check
//    if(!(OM_DAIF->XTAL32M_INTRS & DAIF_XTAL32M_CLK_RDY_MASK))
//        // System error, LOOP forever
//        co_fault_set(CO_FAULT_HCLK_CRASH);
}

/**
 * @brief  pmu topclk double preset
 **/
void drv_pmu_topclk_x2_enable(bool enable)
{
    if (enable)
    {
        // 64MHz DVDD is 1.05v (FT calib to 1V)
        REGSW(&OM_PMU->ANA_REG, MASK_STEP_SSAT(PMU_ANA_REG_PMU_DIG_LDO_TRIM, drv_calib_repair_env.dig_ldo+1), true/*should_update*/, 10/*delay_us*/);
        // enable
        REGW1(&OM_PMU->ANA_PD, PMU_ANA_PD_EN_64M_MASK);
    }
    else
    {
        if(OM_PMU->ANA_PD & PMU_ANA_PD_EN_64M_MASK)
        {
            // disable
            REGW0(&OM_PMU->ANA_PD, PMU_ANA_PD_EN_64M_MASK);
            // 64MHz DVDD is 0.95v (FT calib to 1V)
            REGSW(&OM_PMU->ANA_REG, MASK_STEP_SSAT(PMU_ANA_REG_PMU_DIG_LDO_TRIM, drv_calib_repair_env.dig_ldo-1), true/*should_update*/, 10/*delay_us*/);
        }
    }
}

/**
 * @brief  pmu topclk switch to rc32m
 **/
void drv_pmu_topclk_switch_to_rc32m(void)
{
    // To RC32M
    REGW0(&OM_PMU->MISC_CTRL, PMU_MISC_CTRL_MAIN_CLK_SEL_MASK); // 0:RC32MHz 1:64MHz
    while(!(OM_CPM->STATUS_READ & CPM_STATUS_READ_MAIN_CLK_SYNC_DONE_MASK));
}

#if 0
/**
 * @brief  pmu topclk switch to rc64m
 **/
static void drv_pmu_topclk_switch_to_rc32m_x2(void)
{
    // from RC32M
    REGW1(&OM_PMU->CLK_CTRL_2, PMU_CLK_CTRL_2_SEL_RCOSC_MASK); // 0:XTAL 1:RC
    REGW1(&OM_PMU->XTAL32M_CNS0, PMU_XTAL32M_CNS0_SEL_CPUCLK_MASK); // 0:XTAL32M 1:64M
    DRV_DELAY_US(1);
    // To 64M
    REGW1(&OM_PMU->MISC_CTRL, PMU_MISC_CTRL_MAIN_CLK_SEL_MASK); // 0:RC32MHz 1:64MHz
    while(!(OM_CPM->STATUS_READ & CPM_STATUS_READ_MAIN_CLK_SYNC_DONE_MASK));
}
#endif

/**
 * @brief  pmu xtal32m switch to 32m
 **/
void drv_pmu_topclk_switch_to_xtal32m(void)
{
    // from XTAL32M
    REGW0(&OM_PMU->XTAL32M_CNS0, PMU_XTAL32M_CNS0_SEL_CPUCLK_MASK); // 0:XTAL32M 1:64M
    DRV_DELAY_US(1);
    // To XTAL32M
    REGW1(&OM_PMU->MISC_CTRL, PMU_MISC_CTRL_MAIN_CLK_SEL_MASK); // 0:RC32MHz 1:CPUCLK
    while(!(OM_CPM->STATUS_READ & CPM_STATUS_READ_MAIN_CLK_SYNC_DONE_MASK));
}

/**
 * @brief  pmu xtal32m switch to 64m
 **/
void drv_pmu_topclk_switch_to_xtal32m_x2(void)
{
    // from XTAL32M
    REGW0(&OM_PMU->CLK_CTRL_2, PMU_CLK_CTRL_2_SEL_RCOSC_MASK); // 0:XTAL 1:RC
    REGW1(&OM_PMU->XTAL32M_CNS0, PMU_XTAL32M_CNS0_SEL_CPUCLK_MASK); // 0:XTAL32M 1:64M
    DRV_DELAY_US(1);
    // To 64M
    REGW1(&OM_PMU->MISC_CTRL, PMU_MISC_CTRL_MAIN_CLK_SEL_MASK); // 0:RC32MHz 1:64MHz
    while(!(OM_CPM->STATUS_READ & CPM_STATUS_READ_MAIN_CLK_SYNC_DONE_MASK));
}

/**
 * @brief  pmu rc32m power enable
 *
 * @return is_last_enabled ?
 **/
__RAM_CODES("PM")
bool drv_pmu_topclk_rc32m_power_enable(bool enable)
{
    bool is_last_enabled = (OM_PMU->MISC_CTRL_1 & PMU_MISC_CTRL_1_REG_PD_RC32M_MASK) ? false : true;

    if(enable)
    {
        if (!is_last_enabled)
        {
            REGW0(&OM_PMU->MISC_CTRL_1, PMU_MISC_CTRL_1_REG_PD_RC32M_MASK);
            // must delay more than 15us
            DRV_DELAY_US(10 *3);
        }
    }
    else
    {
        if (is_last_enabled)
            REGW1(&OM_PMU->MISC_CTRL_1, PMU_MISC_CTRL_1_REG_PD_RC32M_MASK);
    }

    return is_last_enabled;
}

/**
 * @brief  pmu topclk xtal32m power enable
 *
 * @param[in] enable
 *
 * XTAL32M
 *
 * if CRY32M_EN==1, xtal24m will be fast-startuped automatically after wakeup (ignore PD_CRY32M)
 *
 * CRY32M_EN does not control xtal24m directly, fucntion:
 *   - xtal24m fast-startup FLAG after wakeup (ignore PD_CRY32M)
 *   - 0 to reset xtal24m-startup-ready signal
 *
 * PD_CRY32M edge can control xtal24m directly, function:
 *   - rising edge: power down
 *   - falling edge: power on
 *
 **/
void drv_pmu_topclk_xtal32m_power_enable(bool enable)
{
    if(enable)
    {
        // open xtal32m ctrl clock
        REGW1(&OM_DAIF->CLK_ENS, DAIF_XTAL32M_CTRL_CLK_EN_MASK);

        // Power on
        REGW1(&OM_PMU->MISC_CTRL_1, PMU_MISC_CTRL_1_CRY32M_EN_MASK);

        // wait
        drv_pmu_topclk_xtal32m_wait_ready();

        // close xtal32m ctrl clock
        REGW0(&OM_DAIF->CLK_ENS, DAIF_XTAL32M_CTRL_CLK_EN_MASK);
    }
    else
    {
        REGW0(&OM_PMU->MISC_CTRL_1, PMU_MISC_CTRL_1_CRY32M_EN_MASK);
    }
}

/**
 * @brief  pmu topclk xtal32m is enabled
 **/
bool drv_pmu_topclk_xtal32m_is_enabled(void)
{
    return (OM_PMU->MISC_CTRL & PMU_MISC_CTRL_MAIN_CLK_SEL_MASK) ? true : false;
}

/**
 * @brief  pmu topclk xtal32m is enabled
 **/
bool drv_pmu_topclk_xtal32m_x2_is_enabled(void)
{
    return (OM_PMU->ANA_PD & PMU_ANA_PD_EN_64M_MASK) ? true : false;
}

/**
 *******************************************************************************
 * @brief  drv pmu topclk xtal32m switch to fast startup mode
 *******************************************************************************
 */
static void drv_pmu_topclk_xtal32m_switch_to_fast_startup_mode(void)
{
    // open xtal32m ctrl clock
    REGW1(&OM_DAIF->CLK_ENS, DAIF_XTAL32M_CTRL_CLK_EN_MASK);

    // Next XTAL32M startup use fast-startup mode (this will lead XTAL32M non-ready)
    REGW0(&OM_PMU->MISC_CTRL, PMU_MISC_CTRL_FIRST_RUN_REG_MASK);
    // No real fast startup flow
    REGW(&OM_PMU->XTAL32M_CNS0, MASK_4REG(PMU_XTAL32M_CNS0_EN_OSC32M_CHIRPRAMP_ME,1, PMU_XTAL32M_CNS0_EN_OSC32M_CHIRPRAMP_MO,0,
                                          PMU_XTAL32M_CNS0_EN_XTAL32M_NRB_ME,1,      PMU_XTAL32M_CNS0_EN_XTAL32M_NRB_MO,0));
    // Restart
    REGW1(&OM_PMU->XTAL32M_CNS1, PMU_XTAL32M_CNS1_XTAL32M_RESTART_MASK);
    // Wait ready
    drv_pmu_topclk_xtal32m_wait_ready();
    // FSM ctrl
    REGW(&OM_PMU->XTAL32M_CNS0, MASK_2REG(PMU_XTAL32M_CNS0_EN_OSC32M_CHIRPRAMP_ME,0, PMU_XTAL32M_CNS0_EN_XTAL32M_NRB_ME,0));

    // close xtal32m ctrl clock
    REGW0(&OM_DAIF->CLK_ENS, DAIF_XTAL32M_CTRL_CLK_EN_MASK);
}

/**
 * @brief  pmu xtal32m startup param setup
 **/
static void drv_pmu_xtal32m_startup_param_setup(void)
{
//    REGW(&OM_PMU->CLK_CTRL_2, MASK_1REG(PMU_CLK_CTRL_2_CT_XTAL32M, 8));
//    REGW(&OM_PMU->XTAL32M_CNS1, MASK_1REG(PMU_XTAL32M_CNS1_XTAL32M_NRB_POR, 0));
//    REGW(&OM_PMU->XTAL32M_CNS0, MASK_2REG(PMU_XTAL32M_CNS0_EN_XTAL32M_NRB_ME, 1, PMU_XTAL32M_CNS0_EN_XTAL32M_NRB_MO, 0x3));
}

/**
 * @brief  pmu xtal32m fast startup param setup
 **/
static void drv_pmu_xtal32m_fast_startup_param_setup(void)
{
    REGW(&OM_PMU->XTAL32M_CNS1, MASK_1REG(PMU_XTAL32M_CNS1_XTAL32M_NRB_POR,3));
}

/**
 * @brief Force into reboot sleep mode
 *
 * Power consumption is lower than the deep sleep mode. All SRAM will be powered down.
 * But chip will be reboot from ROM when wakeup.
 *
 * @return None
 **/
static void drv_pmu_force_into_reboot_sleep_mode(uint32_t ram_retention)
{
    // Disable ALL IRQ, MUST use __set_PRIMASK(1)
     __set_PRIMASK(1);

    // Set flag
    OM_PMU->SW_STATUS &= ~PMU_SW_STATUS_REBOOT_SW_MASK;

    // Set flag
    OM_PMU->SW_STATUS |= PMU_SW_STATUS_REBOOT_FROM_SLEEP_MASK | PMU_SW_STATUS_REBOOT_FROM_SOFT_RESET_MASK;

    // Make sure the isp bit is cleared
    OM_PMU->BOOT_SEL &= ~1u; // @ref BOOT_SEL_ISP_BIT

    // DEEPSLEEP flag
    SCB->SCR |= 0x04;

    // All SRAM into poweroff mode in sleep
    REGW(&OM_PMU->PSO_PM, MASK_2REG(PMU_PSO_PM_RAM_POWER_ON, 0,                   // SRAM: middle 64kB
                                    PMU_PSO_PM_ICACHE_POWER_ON, 0));              // SRAM: ICACHE
    // All SRAM into shutdown mode in sleep
    REGW(&OM_PMU->RAM_CTRL_2, MASK_6REG(PMU_RAM_CTRL_2_RAM_BLE_SD_HW_CTRL_EN, 1,  // SRAM: BLE
                                        PMU_RAM_CTRL_2_RAM_IC_SD_HW_CTRL_EN, 1,   // SRAM: ICACHE, PowerSwitch ref: PMU_PSO_PM_ICACHE_POWER_ON
                                        PMU_RAM_CTRL_2_RAM_PSO_SD_HW_CTRL_EN, 0,  // SRAM: DAIF (Can't shutdown)
                                        PMU_RAM_CTRL_2_RAM0_SD_HW_CTRL_EN, 1,     // SRAM: 1st 8kB and last 8kB
                                        PMU_RAM_CTRL_2_RAM1_SD_HW_CTRL_EN, 1,     // SRAM: middle 32kB, PowerSwitch ref: PMU_PSO_PM_RAM_POWER_ON
                                        PMU_RAM_CTRL_2_RAM2_SD_HW_CTRL_EN, 1));   // SRAM: middle next 32kB, PowerSwitch ref: PMU_PSO_PM_RAM_POWER_ON

    // Good Night!
    __WFI();

    // Must be some IRQ pending, Force reboot
    drv_pmu_cpu_reset();

    // Never come here
    while(1);
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief pmu initialize
 *
 * @return None
 **/
void drv_pmu_init(void)
{
    // Disable test_en
    REGW0(&OM_PMU->MISC_CTRL_1, PMU_MISC_CTRL_1_CPU_JTAG_TEST_EN_MASK|PMU_MISC_CTRL_1_CPU_JTAG_ICG_EN_MASK);

    // Default: GPIO wakeup mask disable
    OM_PMU->GPIO_WAKEUP = 0;
    OM_PMU->GPIO_POL = 0;

    // Default: enable all IE control
    drv_pmu_pin_input_enable(0xFFFFFFFF);

    // Do not power off SRAM in sleep: middle 64kB
    // Do not power off icache in sleep
    REGW1(&OM_PMU->PSO_PM, PMU_PSO_PM_RAM_POWER_ON_MASK | PMU_PSO_PM_ICACHE_POWER_ON_MASK);
    // Most of RAM do not into SD mode in sleep
    REGW(&OM_PMU->RAM_CTRL_2, MASK_6REG(PMU_RAM_CTRL_2_RAM_BLE_SD_HW_CTRL_EN, 1,  // SRAM: BLE (shutdown)
                                        PMU_RAM_CTRL_2_RAM_IC_SD_HW_CTRL_EN, 0,   // SRAM: ICACHE, PowerSwitch ref: PMU_PSO_PM_ICACHE_POWER_ON
                                        PMU_RAM_CTRL_2_RAM_PSO_SD_HW_CTRL_EN, 0,  // SRAM: DAIF (Can't shutdown)
                                        PMU_RAM_CTRL_2_RAM0_SD_HW_CTRL_EN, 0,     // SRAM: 1st 8kB and last 8kB
                                        PMU_RAM_CTRL_2_RAM1_SD_HW_CTRL_EN, 0,     // SRAM: middle 32kB, PowerSwitch ref: PMU_PSO_PM_RAM_POWER_ON
                                        PMU_RAM_CTRL_2_RAM2_SD_HW_CTRL_EN, 0));   // SRAM: middle next 32kB, PowerSwitch ref: PMU_PSO_PM_RAM_POWER_ON
    // All RAM come into DS mode in sleep
    REGW1(&OM_PMU->RAM_CTRL_2, PMU_RAM_CTRL_2_RAM_BLE_DS_HW_CTRL_EN_MASK |
                               PMU_RAM_CTRL_2_RAM_IC_DS_HW_CTRL_EN_MASK |
                               PMU_RAM_CTRL_2_RAM_PSO_DS_HW_CTRL_EN_MASK |
                               PMU_RAM_CTRL_2_RAM2_DS_HW_CTRL_EN_MASK |
                               PMU_RAM_CTRL_2_RAM1_DS_HW_CTRL_EN_MASK |
                               PMU_RAM_CTRL_2_RAM0_DS_HW_CTRL_EN_MASK);

#ifdef CONFIG_IO_OUTPUT_LATCH_CTRL_BY_SOFTWARE
    // Disable auto latch
    REGW(&OM_PMU->MISC_CTRL, MASK_2REG(PMU_MISC_CTRL_GPIO_AUTO_LATCH_FSM_DIS,1, PMU_MISC_CTRL_GPIO_AUTO_LATCH_CTRL, 0));
#endif

    // Retention LDO: 0=0.6v 2=0.7v 3=0.75v 4=0.8v 5=0.85v 6=0.9v 7=0.95v(default) 8=1.0v
    REGW(&OM_PMU->ANA_PD_1, MASK_1REG(PMU_ANA_PD_1_DIG_RETLDO_TRIM, 5));
    // DCDC on delay is 3*32k
    REGW(&OM_PMU->BASIC, MASK_1REG(PMU_BASIC_DCDC_ON_DELAY, 3));
    // DCDC频率振荡调节寄存器,CTUNE值越大振荡频率越慢
    REGW(&OM_PMU->ANA_PD, MASK_1REG(PMU_ANA_PD_DCDC_CTUNE, 3));
    // DCDC PSM/PWM 电流切换调节寄存器,电流大的时候DCDC会从PSM模式切到PWM模式
    REGW(&OM_PMU->ANA_PD_1, MASK_1REG(PMU_ANA_PD_1_DCDC_PSM_VREF, 0));
}

/**
 * @brief pmu select xtal24m as top clock, call by system
 **/
void drv_pmu_xtal32m_startup(void)
{
    DRV_RCC_ANA_CLK_ENABLE();

    // check: whether is xtal24m opened
    if(!drv_pmu_topclk_xtal32m_is_enabled())
    {
        // Next XTAL32M startup use normal-startup mode.
        REGW1(&OM_PMU->MISC_CTRL, PMU_MISC_CTRL_FIRST_RUN_REG_MASK);

        // power on rc32m and switch to it
        // Make sure CPU running on RC32M
        drv_pmu_topclk_rc32m_power_enable(true);
        drv_pmu_topclk_switch_to_rc32m();

        // Try open xtal32m
        drv_pmu_xtal32m_startup_param_setup();
        drv_pmu_topclk_xtal32m_power_enable(true);

        // calib RC
        drv_calib_sys_rc();

        // switch to fast startup mode (must before switch_to_xtal32m)
        drv_pmu_topclk_xtal32m_switch_to_fast_startup_mode();

        // to xtal32m
        drv_pmu_topclk_switch_to_xtal32m();

        // calib RC32M
        drv_calib_sys_rc32m();

        // power off rc32m
        drv_pmu_topclk_rc32m_power_enable(false);
    }

    // disable all daif clock
    OM_DAIF->CLK_ENS = 0;

    SystemCoreClock = drv_rcc_clock_get(RCC_CLK_CPU);

    DRV_RCC_ANA_CLK_RESTORE();
}

/**
 * @brief  pmu xtal32m fast startup
 *
 * @param[in] force  force
 **/
void drv_pmu_xtal32m_fast_startup(bool force)
{
    DRV_RCC_ANA_CLK_ENABLE();

    // check
    if(force || !drv_pmu_topclk_xtal32m_is_enabled())
    {
        // Next XTAL32M startup use fast-startup mode.
        REGW0(&OM_PMU->MISC_CTRL, PMU_MISC_CTRL_FIRST_RUN_REG_MASK);

        // Make sure CPU running on RC32M
        drv_pmu_topclk_rc32m_power_enable(true);
        drv_pmu_topclk_switch_to_rc32m();

        // Try open xtal24m
        drv_pmu_xtal32m_fast_startup_param_setup();
        drv_pmu_topclk_xtal32m_power_enable(true);

        // to xtal32m
        drv_pmu_topclk_switch_to_xtal32m();

        // power off rc32m
        drv_pmu_topclk_rc32m_power_enable(false);
    }

    SystemCoreClock = drv_rcc_clock_get(RCC_CLK_CPU);

    DRV_RCC_ANA_CLK_RESTORE();
}

/**
 * @brief  pmu xtal32m x2 startup
 **/
void drv_pmu_xtal32m_x2_startup(void)
{
    DRV_RCC_ANA_CLK_ENABLE();

    // startup xtal32m
    drv_pmu_xtal32m_fast_startup(false/*force*/);

    // to rc32m
    drv_pmu_topclk_rc32m_power_enable(true);
    drv_pmu_topclk_switch_to_rc32m();

    // delay 2us
    DRV_DELAY_US(2);

    // to xtal64m
    drv_pmu_topclk_x2_enable(true);
    drv_pmu_topclk_switch_to_xtal32m_x2();

    // power off rc32m
    drv_pmu_topclk_rc32m_power_enable(false);

    SystemCoreClock = drv_rcc_clock_get(RCC_CLK_CPU);

    DRV_RCC_ANA_CLK_RESTORE();
}

/**
 *******************************************************************************
 * @brief  drv pmu xtal32m x2 close
 *******************************************************************************
 */
void drv_pmu_xtal32m_x2_close(void)
{
    DRV_RCC_ANA_CLK_ENABLE();

    // startup xtal32m x2
    if (drv_pmu_topclk_xtal32m_is_enabled() && drv_pmu_topclk_xtal32m_x2_is_enabled())
    {
        // to rc32m
        drv_pmu_topclk_rc32m_power_enable(true);
        drv_pmu_topclk_switch_to_rc32m();

        // delay 2us
        DRV_DELAY_US(2);

        // switch to xtal32m
        drv_pmu_topclk_switch_to_xtal32m();

        // disable x2
        drv_pmu_topclk_x2_enable(false);

        // power off rc32m
        drv_pmu_topclk_rc32m_power_enable(false);
    }

    SystemCoreClock = drv_rcc_clock_get(RCC_CLK_CPU);

    DRV_RCC_ANA_CLK_RESTORE();
}

/**
 * @brief  pmu select 32k
 *
 * @param[in] clk32k  clk32k
 *
 * @note
 *   If selecting PMU_32K_SEL_32768HZ_XTAL clock and using BLE,
 *   the `obcc.lpclk_drift` in obc_cc.h should be changed to correct ppm (may be 50 ppm)
 *
 **/
void drv_pmu_select_32k(pmu_32k_sel_t clk32k)
{
    // Default: rc32k powered on, xtal32k powered down
    switch(clk32k)
    {
        case PMU_32K_SEL_RC:
            drv_pmu_select_32k(PMU_32K_SEL_DIV);
            drv_pmu_32k_switch_to_rc(true /*calib*/, true/*pd_others*/);
            break;

        case PMU_32K_SEL_32768HZ_XTAL:
            // Power on rc32k
            //REGW0(&OM_PMU->MISC_CTRL, PMU_MISC_REG_PD_RC32K_MASK);
            //while(!(OM_PMU->STATUS_READ & PMU_RC32K_READY_MASK));

            DRV_RCC_ANA_CLK_ENABLE_NOIRQ();
            // GPIO reuse (IO24/IO25 reuse as xtal32k input)
            REGW1(&OM_PMU->ANA_REG, PMU_ANA_REG_GPIO_REUSE_MASK);
            // Open xtal32m calibration clock for xtal32k ready check
            REGW1(&OM_DAIF->CLK_ENS, DAIF_XTAL32M_CTRL_CLK_EN_MASK|DAIF_C32K_CLK_EN_MASK);
            // Power on xtal32k
            REGW0(&OM_PMU->MISC_CTRL, PMU_MISC_CTRL_PD_CRY32K_MASK);
            while(!(OM_PMU->BASIC & PMU_BASIC_CRY32K_READY_MASK));
            // Close xtal32m calibration clock
            REGW0(&OM_DAIF->CLK_ENS, DAIF_XTAL32M_CTRL_CLK_EN_MASK|DAIF_C32K_CLK_EN_MASK);
            DRV_RCC_ANA_CLK_RESTORE_NOIRQ();

            // Switch
            REGW(&OM_PMU->MISC_CTRL, MASK_1REG(PMU_MISC_CTRL_CLK_32K_SEL, PMU_32K_SEL_32768HZ_XTAL));
            while((OM_PMU->STATUS_READ & (PMU_STATUS_READ_CLK_32K_RC_CRY_DONE_MASK|PMU_STATUS_READ_CLK_32K_DIV_DONE_MASK)) !=
                                         (PMU_STATUS_READ_CLK_32K_RC_CRY_DONE_MASK|PMU_STATUS_READ_CLK_32K_DIV_DONE_MASK));

            // power down others
            //REGW1(&OM_PMU->MISC_CTRL, PMU_MISC_REG_PD_RC32K_MASK);

            // Keep on
            REGW0(&OM_PMU->MISC_CTRL_1, PMU_MISC_CTRL_1_CRY32M_KEEP_ON_MASK);
            break;

        case PMU_32K_SEL_DIV:
            // Power on rc32k
            //REGW0(&OM_PMU->MISC_CTRL, PMU_MISC_REG_PD_RC32K_MASK);
            //while(!(OM_PMU->STATUS_READ & PMU_RC32K_READY_MASK));

            // Open clock
            REGW1(&OM_SYS->RST_32K_OSC_CTRL, SYS_CRY32M_DIV_EN_MASK);

            // Switch
            REGW(&OM_PMU->MISC_CTRL, MASK_1REG(PMU_MISC_CTRL_CLK_32K_SEL, PMU_32K_SEL_DIV));
            while((OM_PMU->STATUS_READ & (PMU_STATUS_READ_CLK_32K_RC_CRY_DONE_MASK|PMU_STATUS_READ_CLK_32K_DIV_DONE_MASK)) !=
                                         (PMU_STATUS_READ_CLK_32K_RC_CRY_DONE_MASK|PMU_STATUS_READ_CLK_32K_DIV_DONE_MASK));

            // power down others
            //REGW1(&OM_PMU->MISC_CTRL, PMU_MISC_REG_PD_CRY32K_MASK | PMU_MISC_REG_PD_RC32K_MASK);
            break;
    }

    drv_pmu_env.clock_32k = clk32k;
}

/**
 * @brief pmu get 32k select
 *
 * @return 32k select
 **/
__RAM_CODES("PM")
pmu_32k_sel_t drv_pmu_select_32k_get(void)
{
    return drv_pmu_env.clock_32k;
}

/**
 * @brief drv_pmu_wakeup_pin_get()
 *
 * @return
 **/
uint32_t drv_pmu_wakeup_pin_get(void)
{
    return OM_PMU->GPIO_WAKEUP;
}

/**
 * @brief what power status should be entried
 *
 * @return power status
 **/
__RAM_CODES("PM")
pm_status_t drv_pmu_sleep_status(void)
{
    pm_status_t max_power_status = PM_STATUS_DEEP_SLEEP;

    return drv_pmu_gpio_lowpower_is_ready() ? max_power_status : PM_STATUS_IDLE;
}

/**
 *******************************************************************************
 * @brief  pmu sleep enter
 *
 * @param[in] sleep_status  sleep status
 * @param[in] reboot_req  reboot req
 *******************************************************************************
 */
__RAM_CODES("PM")
void drv_pmu_sleep_enter(pm_status_t sleep_status, bool reboot_req)
{
    switch(sleep_status)
    {
        case PM_STATUS_DEEP_SLEEP:
            if(!drv_pmu_env.enable_32k_with_deep_sleep)
            {
                if (drv_pmu_env.clock_32k == PMU_32K_SEL_32768HZ_XTAL)
                {
                    // Power on 32k rc and switch to it
                    drv_pmu_32k_switch_to_rc(false /*calib*/, true /*pd_others*/);
                }
                // Set a flag to power down 32K (PMU_BASIC_WAKEUPB_DIS=0: when close 32k, PIN can wakeup)
                REGW(&OM_PMU->BASIC, MASK_2REG(PMU_BASIC_SLEEP_WO_32K,1, PMU_BASIC_WAKEUPB_DIS,0));
                break;
            }
            // fall through

        case PM_STATUS_SLEEP:
            // Set a flag to keep on 32K
            REGW0(&OM_PMU->BASIC, PMU_BASIC_SLEEP_WO_32K_MASK);
            break;

        default:
            while(1);
            break;
    }

    // Wait wake IO clear ok
    drv_pmu_wakeup_pin_wait_idle();

#ifdef CONFIG_IO_OUTPUT_LATCH_CTRL_BY_SOFTWARE
    // Latch IO
    REGW1(&OM_PMU->MISC_CTRL, PMU_MISC_CTRL_GPIO_AUTO_LATCH_CTRL_MASK);
#endif

    // Into reboot sleep mode
    if (reboot_req)
        drv_pmu_force_into_reboot_sleep_mode(0/*ram-retention*/);

    // BUG: Fix abnormal GPIO interrupts STEP(1)
    OM_SYS->MON = 0x1F00;
}

/**
 * @brief pmu leave lowpower status, call by system
 *
 * @param[in] step  step
 * @param[in] state  lowpower state
 *
 * @return None
 **/
__RAM_CODES("PM")
void drv_pmu_sleep_leave(pmu_sleep_leave_step_t step, pm_status_t sleep_status)
{
    if (step & PMU_SLEEP_LEAVE_STEP1_ON_RC32M)
    {
        // BUG: Fix abnormal GPIO interrupts STEP(2)
        if (OM_SYS->MON == 0)
        {
            OM_GPIO0->INTSTATUS = ~0;
            NVIC_ClearPendingIRQ(GPIO_IRQn);
        }

        // Open daif clock
        DRV_RCC_CLOCK_ENABLE(RCC_CLK_DAIF, 1U);

        drv_pmu_env.ana_mask = 0;
        drv_pmu_env.pin_wakeup_sleep_recently = true;
    }

    if (step & PMU_SLEEP_LEAVE_STEP2_WAIT_XTAL32M)
    {
        // Wait xtal32m ready
        drv_pmu_topclk_xtal32m_wait_ready();
        // Wait switch to xtal32m OK
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();

        // default rc32m is opened, close it
        drv_pmu_topclk_rc32m_power_enable(false);
    }

    if (step & PMU_SLEEP_LEAVE_STEP3_FINISH)
    {
#ifdef CONFIG_IO_OUTPUT_LATCH_CTRL_BY_SOFTWARE
        // Release IO
        REGW0(&OM_PMU->MISC_CTRL, PMU_MISC_CTRL_GPIO_AUTO_LATCH_CTRL_MASK);
#endif

        // re-enable xtal32k
        if(!drv_pmu_env.enable_32k_with_deep_sleep)
        {
            if (drv_pmu_env.clock_32k == PMU_32K_SEL_32768HZ_XTAL)
            {
                drv_pmu_select_32k(drv_pmu_env.clock_32k);
            }
        }

        // disable all daif clock
        OM_DAIF->CLK_ENS = 0;

        // close daif clock
        DRV_RCC_CLOCK_ENABLE(RCC_CLK_DAIF, 0U);

    }
}

/**
 * @brief pmu pin state store, call by system
 *
 * @return None
 **/
void drv_pmu_pin_state_store(void)
{
}

/**
 * @brief pmu pin state restore, call by system
 *
 * @return None
 **/
void drv_pmu_pin_state_restore(void)
{
}

/**
 * @brief Set pin mode
 *
 * @param[in] pin_mask  pin mask
 * @param[in] mode  pin mode
 *
 * @return None
 **/
void drv_pmu_pin_mode_set(uint32_t pin_mask, pmu_pin_mode_t mode)
{
    switch(mode)
    {
        case PMU_PIN_MODE_FLOAT:
        case PMU_PIN_MODE_PP:
            OM_PMU->GPIO_ODE_CTRL &= ~pin_mask;
            OM_PMU->GPIO_PU_CTRL  &= ~pin_mask;
            OM_PMU->GPIO_PD_CTRL  &= ~pin_mask;
            break;

        case PMU_PIN_MODE_PD:
            OM_PMU->GPIO_ODE_CTRL &= ~pin_mask;
            OM_PMU->GPIO_PU_CTRL  &= ~pin_mask;
            OM_PMU->GPIO_PD_CTRL  |=  pin_mask;
            break;

        case PMU_PIN_MODE_PU:
            OM_PMU->GPIO_ODE_CTRL &= ~pin_mask;
            OM_PMU->GPIO_PU_CTRL  |=  pin_mask;
            OM_PMU->GPIO_PD_CTRL  &= ~pin_mask;
            break;

        case PMU_PIN_MODE_OD:
            OM_PMU->GPIO_ODE_CTRL |=  pin_mask;
            OM_PMU->GPIO_PU_CTRL  &= ~pin_mask;
            OM_PMU->GPIO_PD_CTRL  &= ~pin_mask;
            break;

        case PMU_PIN_MODE_OD_PU:
            OM_PMU->GPIO_ODE_CTRL |=  pin_mask;
            OM_PMU->GPIO_PU_CTRL  |=  pin_mask;
            OM_PMU->GPIO_PD_CTRL  &= ~pin_mask;
            break;

        default:
            break;
    }
}

/**
 * @brief Set gpio driven current
 *
 * @param[in] pin_mask  pin mask
 * @param[in] driven  current driven (Large driven current should be push-pull output)
 *
 * @return None
 **/
void drv_pmu_pin_driven_current_set(uint32_t pin_mask, pmu_pin_driver_current_t driven)
{
    if(driven & 0x01)
        OM_PMU->GPIO_DRV_CTRL_0 |= (pin_mask & PMU_PIN_ALL_MASK);
    else
        OM_PMU->GPIO_DRV_CTRL_0 &= ~(pin_mask & PMU_PIN_ALL_MASK);
}

/**
 * @brief pmu analog power enable, call by system
 *
 * @param[in] enable  enable/disable
 * @param[in] ana  analog type
 *
 * @return None
 **/
__RAM_CODES("PM")
void drv_pmu_ana_enable(bool enable, pmu_ana_type_t ana)
{
    OM_CRITICAL_BEGIN();

    if(enable)
    {
        if ((drv_pmu_env.ana_mask & ana) == 0)
        {
            if(drv_pmu_env.ana_mask == 0)
                DRV_RCC_CLOCK_ENABLE(RCC_CLK_DAIF, 1U);

            switch(ana)
            {
                case PMU_ANA_RF:
                case PMU_ANA_RF_24G:
                case PMU_ANA_RF_BLE:
                    if ((drv_pmu_env.ana_mask & (PMU_ANA_RF|PMU_ANA_RF_24G|PMU_ANA_RF_BLE)) == 0)
                    {
                        /*
                         * PHY
                         */
                        DRV_RCC_CLOCK_ENABLE(RCC_CLK_PHY, 1U);
                        // Enable RSSI function
                        OM_PHY->RSSI_EST_EN = 1;
                        // FIX issue: 2M PHY preamble leading should be carrier wave
                        REGW(&OM_PHY->TX_CTRL0, MASK_2REG(PHY_TX_CTRL0_BDR_PPM_TX,0, PHY_TX_CTRL0_EN_INTERP,1));
                        // 优化阻塞信道灵敏度策略
                        REGWA(&OM_PHY->TONE_SUPPRESSION_CTRL, MASK_7REG(PHY_TONE_SUPPRESSION_CTRL_EN_TS,2,
                                                                        PHY_TONE_SUPPRESSION_CTRL_TS_EN_GAIN_COND,0,
                                                                        PHY_TONE_SUPPRESSION_CTRL_TS_MAG_LIM,3,
                                                                        PHY_TONE_SUPPRESSION_CTRL_TS_MODE,1,
                                                                        PHY_TONE_SUPPRESSION_CTRL_TS_MAG_EN,1,
                                                                        PHY_TONE_SUPPRESSION_CTRL_TS_K,1,
                                                                        PHY_TONE_SUPPRESSION_CTRL_TS_EST_DUR,1));

                        /*
                         * ANA power/clock
                         */
                        REGW1(&OM_DAIF->CLK_ENS, DAIF_PLL_VTRACK_CLK_EN_MASK | DAIF_PLL_LUT_CLK_EN_MASK |
                                DAIF_MAIN_FSM_CLK_EN_MASK | DAIF_RX_AGC_CLK_EN_MASK | DAIF_DCOC_LUT_CLK_EN_MASK |
                                DAIF_SDM_CLK_EN_MASK | DAIF_PLL_CLK_REF_EN_MASK | DAIF_PLL_AFC_CLK_EN_MASK);
                        REGW1(&OM_DAIF->CLK_CFG, DAIF_XTAL32M_EN_CKO16M_DIG_MASK|DAIF_XTAL32M_EN_CKO16M_ANA_MASK|DAIF_XTAL32M_EN_CKO16M_PLL_MASK);
                    }
                    break;

                case PMU_ANA_ADC:
                    REGW1(&OM_DAIF->CLK_ENS, DAIF_ADC_CLK_EN_MASK);
                    REGW1(&OM_DAIF->CLK_CFG, DAIF_XTAL32M_SEL_CKO16M_GPADC_MASK);
                    break;

                case PMU_ANA_CALIB_RC32K:
                    REGW1(&OM_DAIF->CLK_ENS, DAIF_RC_32K_TUNE_CLK_EN_MASK);
                    break;

                default:
                    break;
            }

            drv_pmu_env.ana_mask |= ana;
        }
    }
    else
    {
        if (drv_pmu_env.ana_mask & ana)
        {
            drv_pmu_env.ana_mask &= ~ana;

            switch(ana)
            {
                case PMU_ANA_RF:
                case PMU_ANA_RF_24G:
                case PMU_ANA_RF_BLE:
                    if ((drv_pmu_env.ana_mask & (PMU_ANA_RF|PMU_ANA_RF_24G|PMU_ANA_RF_BLE)) == 0)
                    {

                        /*
                         * ANA power/clock
                         */
                        // Wait for the digital state machine to finish running before turning off the clock.
                        REGW(&OM_DAIF->DBG_REG, MASK_2REG(DAIF_DBG_EN, 1, DAIF_DBG_IDX, 3));
                        om_error_t ret; // Max: 25us
                        DRV_WAIT_US_UNTIL_TO((REGR(&OM_DAIF->DBG_REG, MASK_POS(DAIF_DBG_DATA)) & 0xF), 30, ret); (void) ret;
                        REGW(&OM_DAIF->DBG_REG, MASK_1REG(DAIF_DBG_EN, 0));
                        /*
                         * disable PHY clk,  If the PHY clock is turned off before waiting for the IDLE state, causing the PHY to not continue outputting data to the MAC, 
                         * MAC state machine stuck in RX_cRC state; However, MAC cannot return to the STANDBY state after CE is pulled low in RX_cRC state, resulting in RX_1N remaining high and the digital analog interface cannot return to IDLE;
                         */
                        DRV_DELAY_US(1);
                        DRV_RCC_CLOCK_ENABLE(RCC_CLK_PHY, 0U);
                        REGW0(&OM_DAIF->CLK_ENS, DAIF_PLL_VTRACK_CLK_EN_MASK | DAIF_PLL_LUT_CLK_EN_MASK |
                                DAIF_MAIN_FSM_CLK_EN_MASK | DAIF_RX_AGC_CLK_EN_MASK | DAIF_DCOC_LUT_CLK_EN_MASK |
                                DAIF_SDM_CLK_EN_MASK | DAIF_PLL_CLK_REF_EN_MASK | DAIF_PLL_AFC_CLK_EN_MASK);
                        REGW0(&OM_DAIF->CLK_CFG, /*DAIF_XTAL32M_EN_CKO16M_DIG_MASK|*/DAIF_XTAL32M_EN_CKO16M_ANA_MASK|DAIF_XTAL32M_EN_CKO16M_PLL_MASK);
                    }
                    break;

                case PMU_ANA_ADC:
                    REGW0(&OM_DAIF->CLK_ENS, DAIF_ADC_CLK_EN_MASK);
                    REGW0(&OM_DAIF->CLK_CFG, DAIF_XTAL32M_SEL_CKO16M_GPADC_MASK);
                    break;

                case PMU_ANA_CALIB_RC32K:
                    REGW0(&OM_DAIF->CLK_ENS, DAIF_RC_32K_TUNE_CLK_EN_MASK);
                    break;

                default:
                    break;
            }

            if(drv_pmu_env.ana_mask == 0)
                DRV_RCC_CLOCK_ENABLE(RCC_CLK_DAIF, 0U);
        }
    }

    OM_CRITICAL_END();
}

/**
 * @brief analog is enabled
 *
 * @param[in] ana  analog module
 *
 * @return enabled?
 **/
bool drv_pmu_ana_is_enabled(pmu_ana_type_t ana)
{
    return (drv_pmu_env.ana_mask & ana) ? true : false;
}

/**
 *******************************************************************************
 * @brief  drv pmu pin wakeup out of date
 *
 * @note:
 * PIN_WAKEUP_IRQ will be generated not only from pin-wakeup but also each GPIO irq(not sleep)
 * So do this to let PIN_WAKEUP_IRQ only generate from pin-wakeup
 *
 * @note: PIN_WAKEUP_IRQn must less then GPIO0_IRQn/GPIO1_IRQn
 *******************************************************************************
 */
void drv_pmu_pin_wakeup_out_of_date(void)
{
    drv_pmu_env.pin_wakeup_sleep_recently = false;
}

#ifndef CONFIG_DCDC_ABSENT
/**
 * @brief  pmu dcdc is enabled
 *
 * @return  is enabled
 **/
bool drv_pmu_dcdc_is_enabled(void)
{
    return (OM_PMU->SW_STATUS & PMU_SW_STATUS_DCDC_ENABLED_MASK) ? true : false;
}

/**
 * @brief pmu enable INSIDE dcdc
 *
 * @param[in] enable  enable/disable
 *
 * @return None
 **/
void drv_pmu_dcdc_enable(bool enable)
{
    if(enable)
    {
        REGW(&OM_PMU->ANA_PD, MASK_1REG(PMU_ANA_PD_DCDC_DIS, 0));
        DRV_DELAY_US(10 *20);
        REGW(&OM_PMU->ANA_PD, MASK_1REG(PMU_ANA_PD_ANA_1P2LDO_DIS, 1));
        REGW1(&OM_PMU->SW_STATUS, PMU_SW_STATUS_DCDC_ENABLED_MASK);
    }
    else
    {
        REGW(&OM_PMU->ANA_PD, MASK_1REG(PMU_ANA_PD_ANA_1P2LDO_DIS, 0));
        DRV_DELAY_US(10 *20);
        REGW(&OM_PMU->ANA_PD, MASK_1REG(PMU_ANA_PD_DCDC_DIS, 1));
        REGW0(&OM_PMU->SW_STATUS, PMU_SW_STATUS_DCDC_ENABLED_MASK);
    }
}
#endif

/**
 * @brief pmu ram power on
 *
 * @param[in] blocks  RAM block mask.
 *
 * @note
 *
 * Param 'blocks':
*   - [bit0]   (1st 8K and last 8K RAM) [0x20000000, 0x20002000) and [0x20012000, 0x20014000) consumption: 0.15uA
 *  - [bit1]   (middle 32K RAM)         [0x20002000, 0x2000A000)                              consumption: 0.3uA
 *  - [bit2]   (middle next 32K RAM)    [0x2000A000, 0x20012000)                              consumption: 0.3uA

 **/
void drv_pmu_ram_power_on(uint32_t blocks)
{
    // 80K SRAM into shutdown mode in sleep
    REGW(&OM_PMU->RAM_CTRL_2, MASK_3REG(PMU_RAM_CTRL_2_RAM0_SD_HW_CTRL_EN, (blocks & BITMASK(0)) ? 0 : 1,     // SRAM: 1st 8kB and last 8kB
                                        PMU_RAM_CTRL_2_RAM1_SD_HW_CTRL_EN, (blocks & BITMASK(1)) ? 0 : 1,     // SRAM: middle 32kB, PowerSwitch ref: PMU_PSO_PM_RAM_POWER_ON
                                        PMU_RAM_CTRL_2_RAM2_SD_HW_CTRL_EN, (blocks & BITMASK(2)) ? 0 : 1));   // SRAM: middle next 32kB, PowerSwitch ref: PMU_PSO_PM_RAM_POWER_ON

    // middle 64kB(middle 32kB and middle next 32kB) SRAM into poweroff mode in sleep
    REGW(&OM_PMU->PSO_PM, MASK_1REG(PMU_PSO_PM_RAM_POWER_ON, (blocks & (BITMASK(1) | BITMASK(2))) ? 1 : 0));
}

/**
 * @brief 32k enable in deep sleep
 *
 * @note
 *  The xtal 32768Hz crystal startup is very very slow (0.5s~2s),
 *  Enable it in deepsleep, can make the wakeup faster
 *
 * @param[in] enable  enable or disable
 *
 * @return None
 **/
void drv_pmu_32k_enable_in_deep_sleep(bool enable)
{
    drv_pmu_env.enable_32k_with_deep_sleep = enable;
}

/**
 * @brief Change xtal 32k params
 *
 * @param[in] load_capacitance  load capacitance, range:0~3 default:0 step:1.5pF, max:4.5pF
 * @param[in] drive_current  drive current, range:0-3, default:0
 *
 * @note load_capacitance will effect xtal 32k startup time and precision,
 *       appropriate value can speed up startup time.
 *
 * @return None
 **/
void drv_pmu_xtal32k_change_param(int load_capacitance, int drive_current)
{
    if(load_capacitance >= 0)
        REGW(&OM_PMU->XTAL32K_CTRL, MASK_1REG(PMU_XTAL32K_CTRL_XTAL32K_CTUNE, load_capacitance));

    if(drive_current >= 0)
        REGW(&OM_PMU->CLK_CTRL_2, MASK_1REG(PMU_CLK_CTRL_2_XTAL32K_ISEL, drive_current));
}

/**
 * @brief Change xtal 32m params
 *
 * @param[in] load_capacitance  load capacitance, range:0~63, default:32, step:0.379pF(<20ppm), max:23.877pF
 *
 * @note load_capacitance will effect xtal 32m precision and frequency offset.
 *
 * @return None
 **/
void drv_pmu_xtal32m_change_param(int load_capacitance)
{
    if(load_capacitance >= 0)
    {
        REGSW(&OM_PMU->CLK_CTRL_2, MASK_STEP(PMU_CLK_CTRL_2_CT_XTAL32M, load_capacitance), false/*should_update*/, 10/*delay_us*/);
    }
}

/**
 * @brief  pmu xtal32m get param
 *
 * @param[in] load_capacitance  load capacitance
 **/
void drv_pmu_xtal32m_get_param(int *load_capacitance)
{
    *load_capacitance = REGR(&OM_PMU->CLK_CTRL_2, MASK_POS(PMU_CLK_CTRL_2_CT_XTAL32M));
}

/**
 * @brief pmu gpio wakeup pin setup
 *
 * @param[in] pin_mask  pin mask
 * @param[in] trigger_type  wakeup trigger type
 *
 * @return None
 **/
void drv_pmu_wakeup_pin_set(uint32_t pin_mask, pmu_pin_wakeup_type_t trigger_type)
{
    OM_CRITICAL_BEGIN();

    switch(trigger_type)
    {
        case PMU_PIN_WAKEUP_DISABLE:
            REGW0(&OM_PMU->GPIO_WAKEUP, pin_mask);
            REGW0(&OM_PMU->GPIO_POL, pin_mask);
            break;

        case PMU_PIN_WAKEUP_FALLING_EDGE:
        case PMU_PIN_WAKEUP_LOW_LEVEL: // FALLING_EDGE
            REGW1(&OM_PMU->GPIO_WAKEUP, pin_mask);
            REGW1(&OM_PMU->GPIO_POL, pin_mask);
            break;

        case PMU_PIN_WAKEUP_RISING_FAILING_EDGE:
        case PMU_PIN_WAKEUP_RISING_EDGE:
        case PMU_PIN_WAKEUP_HIGH_LEVEL: // RISING_EDGE
            REGW1(&OM_PMU->GPIO_WAKEUP, pin_mask);
            REGW0(&OM_PMU->GPIO_POL, pin_mask);
            break;
    }

    switch (trigger_type)
    {
        case PMU_PIN_WAKEUP_LOW_LEVEL:
        case PMU_PIN_WAKEUP_HIGH_LEVEL:
            drv_pmu_env.pin_wakeup_none_edge_mask |= pin_mask;
            drv_pmu_env.pin_wakeup_double_edge_mask &= ~pin_mask;
            break;

        case PMU_PIN_WAKEUP_RISING_FAILING_EDGE:
            drv_pmu_env.pin_wakeup_none_edge_mask &= ~pin_mask;
            drv_pmu_env.pin_wakeup_double_edge_mask |= pin_mask;
            break;

        default:
            break;
    }

    REGW(&OM_PMU->WAKE_DEB, PMU_WAKE_DEB_PIN_WAKE_LEVEL_EDGE_SEL_MASK | PMU_WAKE_DEB_PIN_DEBOUNCE_CYCLE_WAKE_MASK |
                                    PMU_WAKE_DEB_PIN_DEBOUNCE_COEFF_WAKE_MASK | PMU_WAKE_DEB_PIN_DEB_RST_MASK,
                                    (1<<PMU_WAKE_DEB_PIN_WAKE_LEVEL_EDGE_SEL_POS) | PMU_WAKE_DEB_PIN_DEB_RST_MASK);

    // clear interrupt
    REGW1(&OM_PMU->MISC_CTRL, PMU_MISC_CTRL_CLR_PMU_INT_MASK);
    drv_pmu_wakeup_pin_wait_idle();

    OM_CRITICAL_END();

    if(trigger_type != PMU_PIN_WAKEUP_DISABLE)
    {
        // Enable IRQ
        NVIC_ClearPendingIRQ(PIN_WAKEUP_IRQn);
        NVIC_SetPriority(PIN_WAKEUP_IRQn, RTE_PMU_WAKEUP_PIN_IRQ_PRIORITY);
        NVIC_EnableIRQ(PIN_WAKEUP_IRQn);
    }
}

/**
 * @brief pmu wakeup pin register callback
 *
 * @param[in] sleep_callback  callback
 * @param[in] deep_sleep_callback callback
 *
 * @note
 *  When deepsleep, xtal32k startup is very very slow (0.5s-2s),
 *  So the deepsleep pin wakeup irq may don't debounce
 *  @ref drv_pmu_32k_enable_in_deep_sleep()
 *
 * @return None
 **/
void drv_pmu_wakeup_pin_register_callback(drv_isr_callback_t sleep_callback)
{
    drv_pmu_env.pin_wakeup_sleep_callback = sleep_callback;
}

/**
 * @brief Force system to reboot
 *
 * @return None
 **/
void drv_pmu_force_reboot(void)
{
    // Disable ALL IRQ, MUST use __set_PRIMASK(1)
     __set_PRIMASK(1);

    // Set flag
    OM_PMU->SW_STATUS &= ~PMU_SW_STATUS_REBOOT_SW_MASK;

    // Set flag
    OM_PMU->SW_STATUS |= PMU_SW_STATUS_REBOOT_FORCE_MASK;

    // Make sure the isp bit is cleared
    OM_PMU->BOOT_SEL &= ~1u; // @ref BOOT_SEL_ISP_BIT

    // Remap and Reset
    drv_pmu_cpu_reset();

    // Never come here
    while(1);
}

/**
 * @brief pmu validate the configuration
 *
 * @return None
 **/
void drv_pmu_configuration_validate(void)
{
}

/**
 * @brief Get charge status
 *
 * @return status
 **/
pmu_charge_status_t drv_pmu_charge_status(void)
{
    volatile uint32_t charge_status = OM_SYS->CHRGR_STAT;

    if(charge_status & SYS_CHRGR_INSERT_DETECT_MASK)
    {
        if(charge_status & SYS_CHRGR_FINISH_MASK)
            return PMU_CHARGE_COMPLETE;
        else
            return PMU_CHARGE_CHARGING;
    }
    else
    {
        return PMU_CHARGE_EXTRACT;
    }
}

/**
 *******************************************************************************
 * @brief  drv pmu reboot reason
 *
 * @return reason
 *******************************************************************************
 */
pmu_reboot_reason_t drv_pmu_reboot_reason(void)
{
    if(OM_PMU->SW_STATUS & PMU_SW_STATUS_REBOOT_FROM_WDT_MASK)
        return PMU_REBOOT_FROM_WDT;

    if(OM_PMU->SW_STATUS & PMU_SW_STATUS_REBOOT_FROM_IFLASH_LOW_V_MASK)
        return PMU_REBOOT_FROM_IFLASH_LOW_V;

    if(OM_PMU->SW_STATUS & PMU_SW_STATUS_REBOOT_FROM_SLEEP_MASK)
        return PMU_REBOOT_FROM_ULTRA_DEEP_SLEEP;

    if(OM_PMU->SW_STATUS & PMU_SW_STATUS_REBOOT_FROM_ISP_MASK)
        return PMU_REBOOT_FROM_ISP;

    if(OM_PMU->SW_STATUS & PMU_SW_STATUS_REBOOT_FORCE_MASK)
        return PMU_REBOOT_FROM_USER;

    return PMU_REBOOT_FROM_POWER_ON;
}

/**
 *******************************************************************************
 * @brief  drv pmu reboot reason check
 *
 * @note  called by ROM boot
 *******************************************************************************
 */
void drv_pmu_reboot_reason_check(void)
{
    // Store wdt-reset and clear it
    if(OM_PMU->WDT_STATUS & PMU_WDT_STATUS_WDT_FLAG_MASK)
    {
        // Clear
        REGW1(&OM_PMU->WDT_STATUS, PMU_WDT_STATUS_WDT_FLAG_CLR_MASK);

        // Soft flag
        REGW1(&OM_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_WDT_MASK);
        REGW0(&OM_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_SOFT_RESET_MASK);
    }
    else
    {
        REGW0(&OM_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_WDT_MASK);
    }

    if (OM_PMU->FLASH_LOW_VOL_CTRL_0 & PMU_FLASH_LOW_VOL_CTRL_0_FLASH_LOW_VOL_FLAG_MASK)
    {
        // clear
        OM_PMU->FLASH_LOW_VOL_CTRL_1 = 0;

        // Soft flag
        REGW1(&OM_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_IFLASH_LOW_V_MASK);
        REGW0(&OM_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_SOFT_RESET_MASK);
    }
    else
    {
        REGW0(&OM_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_IFLASH_LOW_V_MASK);
    }
}

/**
 * @brief get retention reg value
 *
 * @note This reg value will lost only after power down. default is 0x0000
 *
 * @return retention reg value
 **/
uint16_t drv_pmu_retention_reg_get(void)
{
    return (uint16_t)register_get(&OM_PMU->SW_STATUS, MASK_POS(PMU_SW_STATUS_USER_RETENTION));
}

/**
 * @brief set retention reg
 *
 * @note This reg value will lost only after power down. default is 0x0000
 *
 * @param[in] data  reg value
 *
 * @return None
 **/
void drv_pmu_retention_reg_set(uint16_t value)
{
    REGW(&OM_PMU->SW_STATUS, MASK_1REG(PMU_SW_STATUS_USER_RETENTION, value));
}

/**
 * @brief  pmu recalib sysclk
 **/
void drv_pmu_topclk_recalib(void)
{
    DRV_RCC_ANA_CLK_ENABLE();

    drv_pmu_xtal32m_fast_startup(false);

    // power on rc32m and switch to it
    // Make sure CPU running on RC32M
    drv_pmu_topclk_rc32m_power_enable(true);
    drv_pmu_topclk_switch_to_rc32m();

    // calib RC
    drv_calib_sys_rc();

    // to xtal32m
    drv_pmu_topclk_switch_to_xtal32m();

    // calib RC32M
    drv_calib_sys_rc32m();

    if (drv_pmu_topclk_xtal32m_x2_is_enabled())
    {
        // to rc32m
        drv_pmu_topclk_switch_to_rc32m();

        // delay 2us
        DRV_DELAY_US(2);

        // to xtal64m
        drv_pmu_topclk_x2_enable(true);
        drv_pmu_topclk_switch_to_xtal32m_x2();
    }

    // power off rc32m
    drv_pmu_topclk_rc32m_power_enable(false);

    DRV_RCC_ANA_CLK_RESTORE();
}

/**
 *******************************************************************************
 * @brief  register step set, @ref REGSW()
 *
 * @param[in] reg  reg
 * @param[in] mask  mask
 * @param[in] pos  pos
 * @param[in] value  value
 * @param[in] should_update  should update
 * @param[in] delay_us  delay us
 *******************************************************************************
 */
void drv_pmu_register_step_set(volatile uint32_t *reg, uint32_t mask, uint32_t pos, uint32_t value, bool should_update, uint32_t delay_us)
{
    uint32_t cur = REGR(reg, mask, pos);

    if (cur > value) {
        while(cur-- > value) {
            REGW(reg, mask, cur<<pos);
            if(should_update) {
                REGW1(&OM_PMU->ANA_REG, PMU_ANA_REG_DIG_LDO_UPDATE_MASK);
                while(OM_PMU->ANA_REG & PMU_ANA_REG_DIG_LDO_UPDATE_MASK);
            }
            DRV_DELAY_US(delay_us);
        }
    } else if (cur < value) {
        while(cur++ < value) {
            REGW(reg, mask, cur<<pos);
            if(should_update) {
                REGW1(&OM_PMU->ANA_REG, PMU_ANA_REG_DIG_LDO_UPDATE_MASK);
                while(OM_PMU->ANA_REG & PMU_ANA_REG_DIG_LDO_UPDATE_MASK);
            }
            DRV_DELAY_US(delay_us);
        }
    }

    OM_ASSERT(REGR(reg, mask, pos) == value);
}

/**
 * @brief pmu dump
 *
 * @param[in] printf_dump_func  like printf
 *
 * @note
 *
 * The dump infomation looks like this:
 *   [PMU] prevent_status=00000000
 *   [PMU] wakeup_pin=0001000004(cur_level=0001000004 sleep_level=0001000004)
 *   [PMU] pull_up=FFFD7F9CDF(cur_level=FFFD7F9CDC) pull_down=0000000000(cur_level=0000000000) all_cur_level=FFFFFFFFFC
 *   [PMU] clocking: CPU(128MHz) SRAM(000087FF,ULP:32MHz) SF0 SF1 UART0 GPIO ANA
 *
 * Explain:
 * 1st line:
 *   Something (peripheral, user...) prevent system sleep.
 *   bitmask reference @ref pmu_lowpower_peripheral_t
 * 2nd line:
 *   Bitmask of wakeup pin.
 *   If cur_level != sleep_level, system can't sleep.
 * 3rd line:
 *   Inside pull-up and pull-down status.
 *   if pull_up is not equal to it's cur_level, symtem has current leakage in sleep.
 *   if pull_down's cur_level is not equal to 0, system has current leakage in sleep.
 * 4th line:
 *   Working modules.
 *   SRAM: powered block, the more block are powered on, the greater the current consumption during sleep.
 *         reference: @ref pmu_ram_power_on and @ref pmu_ram_power_off_invalid_block
 **/
void drv_pmu_dump(void *printf_dump_func)
{
    uint32_t wake_en_mask       = OM_PMU->GPIO_WAKEUP;
    uint32_t wakeup_level_mask  = OM_PMU->GPIO_POL;
    uint32_t pull_up_mask       = OM_PMU->GPIO_PU_CTRL;
    uint32_t pull_down_mask     = OM_PMU->GPIO_PD_CTRL;
    uint32_t current_level_mask = OM_GPIO0->DATA;

    void (*__printf)(const char *format, ...) = /*lint -e{611}*/ (void (*)(const char *format, ...))printf_dump_func;
    if (__printf == NULL) {
        return;
    }

    // wakeup pin
    __printf("[PMU] wakeup pin: 0x%08X(cur_level: 0x%08X sleep_level: 0x%08X)\n",
            wake_en_mask, wake_en_mask & current_level_mask, wakeup_level_mask);
    // pull status
    __printf("[PMU] pull up: 0x%08X(cur_level:0x%08X) pull down: 0x%08X(cur_level:0x%08X) all cur_level: 0x%08X\n",
            pull_up_mask, pull_up_mask & current_level_mask, pull_down_mask, pull_down_mask & current_level_mask, current_level_mask);
    // cpu clock
    __printf("[PMU] clocking CPU[%dMHZ]\n", drv_rcc_clock_get(RCC_CLK_CPU) / 1000000);
    // peripherals clock
    if (!(OM_CPM->SF_CFG & CPM_SF_CFG_SF_GATE_EN_MASK)) {
        __printf(" SF");
    }
    if (!(OM_CPM->TIMER0_CFG & CPM_TIMERX_GATE_EN_MASK)) {
        __printf(" TIM0");
    }
    if (!(OM_CPM->TIMER1_CFG & CPM_TIMERX_GATE_EN_MASK)) {
        __printf(" TIM1");
    }
    if (!(OM_CPM-> TIMER2_CFG & CPM_TIMERX_GATE_EN_MASK)) {
        __printf(" TIM2");
    }
    if (!(OM_CPM->UART0_CFG & CPM_UARTx_CFG_UARTx_GATE_EN_MASK)) {
        __printf(" UART0");
    }
    if (!(OM_CPM->UART1_CFG & CPM_UARTx_CFG_UARTx_GATE_EN_MASK)) {
        __printf(" UART1");
    }
    if (!(OM_CPM->I2C_CFG & CPM_I2C_CFG_I2C_GATE_EN_MASK)) {
        __printf(" I2C0");
    }
    if (!(OM_PMU->BASIC & PMU_BASIC_LPTIM_32K_CLK_GATE_MASK)) {
        __printf(" LPTIM");
    }
    if (!(OM_CPM->BLE_CFG & CPM_BLE_CFG_BLE_AHB_GATE_EN_MASK)) {
        __printf(" BLE");
    }
    if (!(OM_CPM->DMA_CFG & CPM_DMA_CFG_DMA_GATE_EN_MASK)) {
        __printf(" DMA");
    }
    if (OM_CPM->AES_CFG & CPM_AES_CLK_EN_MASK) {
        __printf(" AES");
    }
    if (!(OM_CPM->GPIO_CFG & CPM_GPIO_CFG_GPIO_GATE_EN_MASK)) {
        __printf(" GPIO0");
    }
    if (!(OM_CPM->PHY_CFG & CPM_PHY_CFG_PHY_16M_GATE_EN_MASK)) {
        __printf(" PHY");
    }
    if (!(OM_CPM->RNG_CFG & CPM_RNG_CFG_RNG_GATE_EN_MASK)) {
        __printf(" RNG");
    }
    if (!(OM_CPM->MAC_2P4_CFG & CPM_2P4_CFG_MAC_2G4_GATE_EN_MASK)) {
        __printf(" 2.4G");
    }
    if (!(OM_CPM->ANA_IF_CFG)) {
        __printf(" DAIF");
    }
    if (!(OM_CPM->EFUSE_CFG & CPM_EFUSE_GATE_EN_MASK)) {
        __printf(" EFUSE");
    }
    if (!(OM_CPM->SPI0_CFG & CPM_SPI0_CFG_SPI0_GATE_EN_MASK)) {
        __printf(" SPI0");
    }
    if (!(OM_CPM->SPI1_CFG & CPM_SPI1_CFG_SPI1_GATE_EN_MASK)) {
        __printf(" SPI1");
    }
    if (!(OM_CPM->APB_CFG & CPM_APB_CFG_RTC_APB_GATE_EN_MASK)) {
        __printf(" RTC");
    }
    __printf("\n");
}

/**
 *******************************************************************************
 * @brief  drv pmu pin wakeup isr
 *******************************************************************************
 */
void drv_pmu_pin_wakeup_isr(void)
{
    uint32_t int_status = OM_PMU->GPIO_LATCH;

    // clear interrupt
    REGW1(&OM_PMU->MISC_CTRL, PMU_MISC_CTRL_CLR_PMU_INT_MASK);

    // Sleep wakeup: may 2ms delay
    if(drv_pmu_env.pin_wakeup_sleep_recently)
    {
        // clear gpio irq pending
        drv_gpio_control(OM_GPIO0, GPIO_CONTROL_CLEAR_INT, (void *)int_status);

        // Sleep wakeup: may 2ms delay
        if(drv_pmu_env.pin_wakeup_sleep_callback && int_status)
            drv_pmu_env.pin_wakeup_sleep_callback(OM_PMU, DRV_EVENT_COMMON_GENERAL, (void *)int_status, (void *)(OM_GPIO0->DATA));

        // wakeup out of date
        drv_pmu_pin_wakeup_out_of_date();
    }
}

void drv_pmu_pof_enable(bool enable, uint8_t voltage)
{
    if(enable) {
        REGW(&OM_PMU->POF_INT_CTRL, MASK_5REG(PMU_POF_INT_CTRL_PMU_POF_INT_EN, 1, PMU_POF_INT_CTRL_PMU_PD_POF_EN, 0, PMU_POF_INT_CTRL_PMU_PD_POF_ME, 1, PMU_POF_INT_CTRL_PMU_PD_POF_REG, 0, PMU_POF_INT_CTRL_PMU_POF_TH_REG, voltage));
    } else {    
        REGW(&OM_PMU->POF_INT_CTRL, MASK_4REG(PMU_POF_INT_CTRL_PMU_PD_POF_ME, 0, PMU_POF_INT_CTRL_PMU_PD_POF_REG, 1, PMU_POF_INT_CTRL_PMU_POF_INT_EN, 0, PMU_POF_INT_CTRL_PMU_PD_POF_EN, 1));
    }
}

bool drv_pmu_pof_is_trigger(void)
{
    return (OM_PMU->POF_INT_CTRL & 0x100) ? true : false;
}
#endif /* RTE_PMU */

/** @} */
