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
 * @file     drv_calib.c
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
#include <stddef.h>
#include "om_driver.h"
#include "pm.h"

/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * CONST & VARIABLES
 */
drv_calib_repair_t drv_calib_repair_env = {
    .temperature            = 25,
    .rc_rf_repair_delay     = PMU_TIMER_MS2TICK(60*1000),
    .rc_rf_repair_time      = DRV_INVALID_TIME,
    .rc_repair_temperature  = DRV_INVALID_TEMPERATURE,
    .rf_repair_temperature  = DRV_INVALID_TEMPERATURE,
    .rc32k_repair_delay     = PMU_TIMER_MS2TICK(60*1000),
    .rc32k_repair_time      = DRV_INVALID_TIME,
    .xtal32m_cap            = 0,
    .trim_vref              = 12,
    .dig_ldo                = 6,
    .pa_ldo                 = 6,
    .dcdc_vout              = 9,
    .pfd_ldo                = 1,
    .vco_ldo                = 5,
    .buff_ldo               = 4,
    .ana_ldo                = 4,
    .kdco_lut_1m            = -1,
    .kdco_lut_2m            = -1,
    .con_bias_idac_pll      = -1,
    .vco_cur                = 15,
};

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
/**
 * @brief  calib repair voltage temperature
 *
 * @param[in] t  temperature
 **/
static void drv_calib_repair_voltage_temperature(int16_t t)
{
    REGSW(&OM_PMU->TRIM_SET, MASK_STEP(PMU_TRIM_SET_PMU_TRIM_VREF, drv_calib_repair_env.trim_vref), true/*should_update*/, 10/*delay_us*/);
}

/**
 * @brief  calib repair xtal32m temperature
 *
 * @param[in] t  temperature
 **/
static void drv_calib_repair_xtal32m_temperature(int16_t t)
{
}

/**
 * @brief rf pll temperature repair
 **/
void drv_calib_repair_rf_pll_temperature_repair(bool is_calib_start)
{
    static int16_t calib_repair_env_calib_start_temperature;
    int8_t con_bias_idac_pll_delta = 0;
    uint8_t ftun = 0;
    uint8_t vtrack_mo = 4;
    int8_t  pll_vco_ibias_cur_delta = 0;

    if (is_calib_start)
    {
        calib_repair_env_calib_start_temperature = drv_calib_repair_env.rf_repair_temperature;
        if(calib_repair_env_calib_start_temperature > 40)
        {
            ftun = 3;
        }
        else if(calib_repair_env_calib_start_temperature <= 10)
        {
            ftun = 0;
        }
        else
        {
            ftun = 1;
        }
    }
    else
    {
        if((calib_repair_env_calib_start_temperature>10)&&(calib_repair_env_calib_start_temperature<=40))
        {
            ftun = 1;       
            vtrack_mo = 4;
            if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) > 80)
            {
                ftun = 3;
                vtrack_mo = 6;
            }
            else if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) > 20)
            {
                ftun = 3;
                vtrack_mo = 5;
            }
            else if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) < -20)
            {
                ftun = 0;
                vtrack_mo = 4;
            }
        }
        else if((calib_repair_env_calib_start_temperature>40)&&(calib_repair_env_calib_start_temperature<=60))///50
        {
            ftun = 3;
            vtrack_mo = 4;
            if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) > 60) 
            {
                ftun = 3;
                vtrack_mo = 6;
            }
            else if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) > 20) 
            {
                ftun = 3;
                vtrack_mo = 5;
            }

            if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) < -40)
            {
                ftun = 0;
                vtrack_mo = 3;
            }
            else if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) < -20)
            {
                ftun = 0;
                vtrack_mo = 4;
            }
        }
        else if((calib_repair_env_calib_start_temperature>60)&&(calib_repair_env_calib_start_temperature<=80))///70
        {
            ftun = 3;
            vtrack_mo = 4;
            if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) < -60) //-0
            {
                ftun = 0;
                vtrack_mo = 2;
            }
            else if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) < -40) //25
            {
                ftun = 0;
                vtrack_mo = 4;
            }
        }
        else if((calib_repair_env_calib_start_temperature>80)&&(calib_repair_env_calib_start_temperature<=100))///90
        {
            ftun = 3;
            vtrack_mo = 4;
            if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) < -80) //0
            {
                ftun = 0;
                vtrack_mo = 2;
            }
            else if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) < -60) //25
            {
                ftun = 0;
                vtrack_mo = 3;
            }
            else if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) < -40) //50
            {
                ftun = 1;
                vtrack_mo = 4;
            }
        }
        else if(calib_repair_env_calib_start_temperature>100)///115
        {
            ftun = 3;
            vtrack_mo = 4;
        }
        else if((calib_repair_env_calib_start_temperature>-10)&&(calib_repair_env_calib_start_temperature<=10))///0
        {
            ftun = 0;
            vtrack_mo = 4;
            if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) > 106)
            {
                ftun = 3;
                vtrack_mo = 6;
            }
            else if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) > 50)
            {
                ftun = 3;
                vtrack_mo = 5;
            }
            else if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) > 30)
            {
                ftun = 3;
                vtrack_mo = 4;
            }

            if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) < -30)
            {
                ftun = 0;
                vtrack_mo = 3;
            }
        }
        else if((calib_repair_env_calib_start_temperature>-30)&&(calib_repair_env_calib_start_temperature<=-10))///-25
        {
            ftun = 0;
            vtrack_mo = 5;
            if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) > 110)
            {
                ftun = 3;
                vtrack_mo = 7;
            }
            else if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) > 50)
            {
                ftun = 3;
                vtrack_mo = 6;
            }
            else if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) > 30)
            {
                ftun = 0;
                vtrack_mo = 6;
            }

            if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) < -15)
            {
                ftun = 0;
                vtrack_mo = 4;
            }
        }
        else if(calib_repair_env_calib_start_temperature<=-30)///-40
        {
            ftun = 0;
            vtrack_mo = 6;
            if((drv_calib_repair_env.temperature - calib_repair_env_calib_start_temperature) > 50)
            {
                ftun = 3;
                vtrack_mo = 6;
            }
        }

        if((drv_calib_repair_env.temperature>=-10)&&(drv_calib_repair_env.temperature<=70))
        {
            con_bias_idac_pll_delta = 0;
        }
        else if((drv_calib_repair_env.temperature) > 100)  
        {
            con_bias_idac_pll_delta = 8;
        }
        else if((drv_calib_repair_env.temperature) > 90)  
        {
            con_bias_idac_pll_delta = 7;
        }
        else if((drv_calib_repair_env.temperature) > 80)  
        {
            con_bias_idac_pll_delta = 5;
        }
        else if((drv_calib_repair_env.temperature) > 70)  
        {
            con_bias_idac_pll_delta = 2;
        }
        else if((drv_calib_repair_env.temperature) < -28)  
        {
            con_bias_idac_pll_delta = 7;
        }
        else if((drv_calib_repair_env.temperature) < -20)  
        {
            con_bias_idac_pll_delta = 5;
        }
        else if((drv_calib_repair_env.temperature) < -10)  
        {
            con_bias_idac_pll_delta = 2;
        }

        if((drv_calib_repair_env.temperature) < 0)
        {
            pll_vco_ibias_cur_delta = 1;
        }
    }

    int8_t use_bias_idac_pll     = drv_calib_repair_env.con_bias_idac_pll+con_bias_idac_pll_delta;
    int8_t use_pll_vco_ibias_cur = drv_calib_repair_env.vco_cur+pll_vco_ibias_cur_delta;
    if (use_bias_idac_pll < 0) {
        use_bias_idac_pll = 0;
    } else if (use_bias_idac_pll > 63) {
        use_bias_idac_pll = 63;
    }
    if (use_pll_vco_ibias_cur < 0) {
        use_pll_vco_ibias_cur = 0;
    } else if (use_pll_vco_ibias_cur > 15) {
        use_pll_vco_ibias_cur = 15;
    }

    DRV_RCC_ANA_CLK_ENABLE();
    REGW(&OM_DAIF->PLL_CTRL1,  MASK_2REG(DAIF_CON_BIAS_IDAC_PLL,     use_bias_idac_pll,
                                         DAIF_RDPLL_SEL_VCO_IBIAS,   use_pll_vco_ibias_cur));
    REGW(&OM_DAIF->PLL_CTRL0,  MASK_1REG(DAIF_REG_FTUN,  ftun));
    REGW(&OM_DAIF->VCO_CTRL0,  MASK_2REG(DAIF_VTRACK_EN, 0, DAIF_VTRACK_MO, vtrack_mo));
    DRV_RCC_ANA_CLK_RESTORE();
}

/**
 * @brief rf temperature repair check
 **/
static void drv_calib_repair_rc_rf_temperature(uint32_t cur_time)
{
#if (RTE_GPADC)
    if(!drv_adc_control(ADC_CONTROL_IS_BUSY, NULL))
    {
        int16_t t = (int)drv_adc_control(ADC_CONTROL_READ_TEMPERATURE, NULL);
        int16_t delta_t_rc = drv_calib_repair_env.rc_repair_temperature - t;
        int16_t pre_all_repair_t = drv_calib_repair_env.rf_repair_temperature;
        bool topclk_recalibed = false;

        drv_calib_repair_env.temperature = t;
        drv_calib_repair_env.rc_rf_repair_time = cur_time;

        // if delta-temperature>15C, re-calib rc
        if (delta_t_rc>15 || delta_t_rc<-15)
        {
            drv_calib_repair_env.rc_repair_temperature = t;

            drv_pmu_topclk_recalib();
            topclk_recalibed = true;
        }

        // re-calib all
        if ((pre_all_repair_t<85 && t>90) || (pre_all_repair_t>90 && t<85))
        {
            drv_calib_repair_env.rc_repair_temperature = t;
            drv_calib_repair_env.rf_repair_temperature = t;

            if(!topclk_recalibed)
                drv_pmu_topclk_recalib();

            drv_calib_repair_voltage_temperature(t);

            drv_calib_rf();
        }

        drv_calib_repair_rf_pll_temperature_repair(false);
        // xtal32m
        drv_calib_repair_xtal32m_temperature(t);
    }
#endif
}

/**
 * @brief  calib repair rc32k lpclk drift temperature
 **/
static void drv_calib_repair_rc32k_drift_temperature(void)
{
}

/**
 * @brief rc32k temperature repair check
 **/
static void drv_calib_repair_rc32k_temperature(uint32_t cur_time)
{
    // check and re-select (will trigger calib_rc32k)
    drv_pmu_select_32k(PMU_32K_SEL_RC);

    // save
    drv_calib_repair_env.rc32k_repair_time = cur_time;

    // temperature
    drv_calib_repair_rc32k_drift_temperature();
}

/**
 * @brief  calib repair rc rf temperature check
 *
 * @param[in] allow_repair_delay_ms  allow repair delay ms
 *
 * @return repaired
 **/
__RAM_CODES("PM")
static bool drv_calib_repair_rc_rf_temperature_check(uint32_t allow_repair_delay_ms)
{
    uint32_t pre_time_tmp;
    bool repaired = false;

    do
    {
        if(0 == drv_calib_repair_env.rc_rf_repair_delay)
            break;

        pre_time_tmp = drv_calib_repair_env.rc_rf_repair_time;
        if(om_time_delay_past(drv_calib_repair_env.rc_rf_repair_delay, &pre_time_tmp))
        {
            drv_calib_repair_rc_rf_temperature(pre_time_tmp);
            repaired = true;
        }
    } while(0);

    return repaired;
}

/**
 * @brief  calib repair rc32k temperature check
 *
 * @param[in] allow_repair_delay_ms  allow repair delay ms
 *
 * @return repaired
 **/
__RAM_CODES("PM")
static bool drv_calib_repair_rc32k_temperature_check(uint32_t allow_repair_delay_ms)
{
    uint32_t pre_time_tmp;
    bool repaired = false;

    do
    {
        // calib rc32k need 27ms(6621)/30ms(6621c)
        //if (allow_repair_delay_ms < 31)
        //    break;

        if (0 == drv_calib_repair_env.rc32k_repair_delay)
            break;

        if(drv_pmu_select_32k_get() != PMU_32K_SEL_RC)
            break;

        pre_time_tmp = drv_calib_repair_env.rc32k_repair_time;
        if(om_time_delay_past(drv_calib_repair_env.rc32k_repair_delay, &pre_time_tmp)) // delay is pasted
        {
            drv_calib_repair_rc32k_temperature(pre_time_tmp);
            repaired = true;
        }
    }while(0);

    return repaired;
}

/**
 *******************************************************************************
 * @brief  drv calib repair pm checker handler
 *
 * @return status
 *******************************************************************************
 */
__RAM_CODES("PM")
static pm_status_t drv_calib_repair_pm_checker_handler (void)
{
    // TODO
    drv_calib_repair_rc_rf_temperature_check(100);
    drv_calib_repair_rc32k_temperature_check(100);

    return PM_STATUS_DEEP_SLEEP;
}

/**
 *******************************************************************************
 * @brief  obc pm sleep store restore handler
 *
 * @param[in] sleep_state  sleep state
 * @param[in] power_status  power status
 *******************************************************************************
 */
__RAM_CODES("PM")
static void drv_calib_repair_pm_sleep_store_restore_handler (pm_sleep_state_t sleep_state, pm_status_t power_status)
{
    if(sleep_state == PM_SLEEP_STORE) {

    } else if (sleep_state == PM_SLEEP_RESTORE_HSI) {
        drv_calib_sys_restore();
        drv_calib_rf_restore();
    } else if (sleep_state == PM_SLEEP_RESTORE_HSE) {

    }
}

/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 * @brief  calib repair rc rf init
 **/
void drv_calib_repair_init(void)
{
    int16_t t = 20;

    if(0 == drv_calib_repair_env.rc_rf_repair_delay)
        return;

#if (RTE_GPADC)
    t = (int)drv_adc_control(ADC_CONTROL_READ_TEMPERATURE, NULL);
#endif

    drv_calib_repair_env.temperature = t;
    drv_calib_repair_env.rf_repair_temperature = t;
    drv_calib_repair_env.rc_repair_temperature = t;
    drv_calib_repair_env.rc32k_repair_time = drv_calib_repair_env.rc_rf_repair_time = om_time();

    // xtal32m repair
    drv_calib_repair_xtal32m_temperature(t);

    // voltage repair
    drv_calib_repair_voltage_temperature(t);

    // rc32k drift repair
    if(drv_pmu_select_32k_get() == PMU_32K_SEL_RC)
        drv_calib_repair_rc32k_drift_temperature();

    // pm
    #if (RTE_PM)
    pm_sleep_checker_callback_register(PM_CHECKER_PRIORITY_LOW, drv_calib_repair_pm_checker_handler);
    pm_sleep_store_restore_callback_register(drv_calib_repair_pm_sleep_store_restore_handler);
    #endif
}

/** @} */

