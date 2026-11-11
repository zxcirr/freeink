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
 * @file     drv_pmu.h
 * @brief    Power Manager Unit for om662x
 * @date     30. March 2020
 * @author   OnMicro SW Team
 *
 * @defgroup PMU PMU
 * @ingroup  DRIVER
 * @brief    Power Manager Unit for om662x
 * @details  Power Manager Unit for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


#ifndef __PMU_OM662X_H
#define __PMU_OM662X_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_PMU)
#include <stdint.h>
#include <stdbool.h>
#include "om_device.h"
#include "om_driver.h"
#if (RTE_PM)
#include "pm.h"
#endif  /* (RTE_PM) */


#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
#define PMU_PIN_ALL_MASK       0x3FFFFFFU

/*********************************************************************
 * TYPEDEFS
 */

/// PMU analog power type
typedef enum
{
    /// PMU analog power type: RF for calib/test/carrier...
    PMU_ANA_RF          = 1<<0,
    /// PMU analog power type: RF only for 24G MAC
    PMU_ANA_RF_24G      = 1<<1,
    /// PMU analog power type: RF only for BLE MAC
    PMU_ANA_RF_BLE      = 1<<2,
    /// PMU analog power type: ADC
    PMU_ANA_ADC         = 1<<3,
    /// PMU analog power type: Calibration RC32K
    PMU_ANA_CALIB_RC32K = 1<<4,
}pmu_ana_type_t;

/// 32k select, work with RTC, baseband in sleep
typedef enum
{
    /// Select RC32K
    PMU_32K_SEL_RC           = 0,
    /// Select XTAL32K
    PMU_32K_SEL_32768HZ_XTAL = 1,
    /// Select DIV32K (Div by Xtal32M)
    PMU_32K_SEL_DIV          = 2,
}pmu_32k_sel_t;

/// Mode
typedef enum
{
    /// Float (Input)
    PMU_PIN_MODE_FLOAT,
    /// Push pull (Output)
    PMU_PIN_MODE_PP,
    /// Pull up, @ref drv_pmu_pull_up_resistance_set()
    PMU_PIN_MODE_PU,
    /// Pull down (141kOHM~303kOHM)
    PMU_PIN_MODE_PD,
    /// Open drain
    PMU_PIN_MODE_OD,
    /// Open drain, pull up
    PMU_PIN_MODE_OD_PU,
}pmu_pin_mode_t;

/// Interrupt trigger type
typedef enum
{
    /// Pin wakeup trigger by low level
    PMU_PIN_WAKEUP_LOW_LEVEL,
    /// Pin wakeup trigger by high level
    PMU_PIN_WAKEUP_HIGH_LEVEL,
    /// Pin wakeup trigger type is falling edge
    /// NOTE: It cannot be used in deep sleep mode with 32k off (@ref drv_pmu_32k_enable_in_deep_sleep)
    PMU_PIN_WAKEUP_FALLING_EDGE,
    /// Pin wakeup trigger type is rising edge
    /// NOTE: It cannot be used in deep sleep mode with 32k off (@ref drv_pmu_32k_enable_in_deep_sleep)
    PMU_PIN_WAKEUP_RISING_EDGE,
    /// Pin wakeup trigger type  is both edge
    PMU_PIN_WAKEUP_RISING_FAILING_EDGE,
    /// Pin wakeup trigger disable
    PMU_PIN_WAKEUP_DISABLE,
}pmu_pin_wakeup_type_t;

/// Charge status
typedef enum
{
    /// Charger extract
    PMU_CHARGE_EXTRACT,
    /// Charger insert and charging
    PMU_CHARGE_CHARGING,
    /// Charger insert and charge complete
    PMU_CHARGE_COMPLETE,
}pmu_charge_status_t;

/// Reboot reason
typedef enum
{
    /// Reboot from power on
    PMU_REBOOT_FROM_POWER_ON,
    /// Reboot from ultra deep sleep
    PMU_REBOOT_FROM_ULTRA_DEEP_SLEEP,
    /// Reboot from ISP
    PMU_REBOOT_FROM_ISP,
    /// Users take the initiative to reboot
    PMU_REBOOT_FROM_USER,
    /// Reboot from watch dog
    PMU_REBOOT_FROM_WDT,
    /// Reboot from inside flash low voltage reset
    PMU_REBOOT_FROM_IFLASH_LOW_V,
}pmu_reboot_reason_t;

/// PIN driver current
typedef enum {
    /// Pin driver current: MIN
    PMU_PIN_DRIVER_CURRENT_MIN    = 0,
    /// Pin driver current: Normal
    PMU_PIN_DRIVER_CURRENT_NORMAL = 0,
    /// Pin driver current: MAX
    PMU_PIN_DRIVER_CURRENT_MAX    = 1,
} pmu_pin_driver_current_t;

/// Sleep leave step
typedef enum {
    /// PMU sleep leave step1: on rc32m
    PMU_SLEEP_LEAVE_STEP1_ON_RC32M      = 0x01,
    /// PMU sleep leave step2: wait xtal32m
    PMU_SLEEP_LEAVE_STEP2_WAIT_XTAL32M  = 0x02,
    /// PMU sleep leave step3: finish
    PMU_SLEEP_LEAVE_STEP3_FINISH        = 0x04,
    /// PMU sleep leave step all
    PMU_SLEEP_LEAVE_STEP_ALL            = 0xFF,
} pmu_sleep_leave_step_t;

/// Pull up resistance
/// @note These resistance values are approximate
typedef enum
{
    PMU_PIN_PU_RESISTANCE_4K    = 0,
    PMU_PIN_PU_RESISTANCE_10K   = 1,
    PMU_PIN_PU_RESISTANCE_300K  = 2,
    PMU_PIN_PU_RESISTANCE_2M    = 3,
}pmu_pin_pu_resistance_t;

/// @cond
typedef struct
{
    volatile uint32_t ana_mask;
    pmu_32k_sel_t clock_32k;

    drv_isr_callback_t pin_wakeup_sleep_callback;
    volatile bool pin_wakeup_sleep_recently;
    bool enable_32k_with_deep_sleep;

    // just for debug
    bool enable_swd_in_sleep;
    // force ie
    uint32_t pin_sleep_force_enable_ie_mask;

    // wakeup pin edge
    volatile uint32_t pin_wakeup_none_edge_mask;
    volatile uint32_t pin_wakeup_double_edge_mask;
}pmu_env_t;
/// @endcond

/*********************************************************************
 * EXTERN VARIABLES
 */
/// @cond
extern pmu_env_t drv_pmu_env;
/// @endcond

/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 *******************************************************************************
 * @brief pmu initialize
 *******************************************************************************
 **/
void drv_pmu_init(void);

/**
 *******************************************************************************
 * @brief pmu select xtal32m as top clock, call by system
 *
 * @note  This function only be called in system init.
 *******************************************************************************
 **/
void drv_pmu_xtal32m_startup(void);

/**
 *******************************************************************************
 * @brief  pmu xtal32m x2 startup that lead system top clock is 64MHz
 *
 * @note  drv_pmu_xtal32m_startup() must be called before calling this
 *******************************************************************************
 **/
void drv_pmu_xtal32m_x2_startup(void);

/**
 *******************************************************************************
 * @brief  drv pmu xtal32m x2 close
 *******************************************************************************
 */
void drv_pmu_xtal32m_x2_close(void);

/**
 *******************************************************************************
 * @brief  pmu xtal32m fast startup
 *
 * @param[in] force  force
 *******************************************************************************
 **/
void drv_pmu_xtal32m_fast_startup(bool force);

/**
 *******************************************************************************
 * @brief pmu select 32k
 *
 * @param[in] clk32k  32k clock select
 *
 * @note
 *   If selecting PMU_32K_SEL_32768HZ_XTAL clock and using BLE,
 *   the `obcc.lpclk_drift` in obc_cc.h should be changed to correct ppm (may be 50 ppm)
 *
 *******************************************************************************
 **/
void drv_pmu_select_32k(pmu_32k_sel_t clk32k);

/**
 *******************************************************************************
 * @brief pmu get 32k select
 *
 * @return 32k select
 *******************************************************************************
 **/
pmu_32k_sel_t drv_pmu_select_32k_get(void);

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
void drv_pmu_ram_power_on(uint32_t blocks);

/**
 *******************************************************************************
 * @brief 32k enable in deep sleep
 *
 * @note
 *  The xtal 32768Hz crystal startup is very very slow (0.5s~2s),
 *  Its current consumption is about 0.3uA,
 *  Enable it in deepsleep, can make the wakeup faster
 *
 * @param[in] enable  enable or disable
 *******************************************************************************
 **/
void drv_pmu_32k_enable_in_deep_sleep(bool enable);

/**
 * @brief Change xtal 32k params
 *
 * @param[in] load_capacitance  load capacitance, range:0~3, default:0(0pF), step:1.5pF, max:4.5pF
 * @param[in] drive_current  drive current, range:0-3, default:0
 *
 * @note load_capacitance will effect xtal 32k startup time and precision,
 *       appropriate value can speed up startup time.
 *
 * @return None
 **/
void drv_pmu_xtal32k_change_param(int load_capacitance, int drive_current);

/**
 *******************************************************************************
 * @brief Change xtal 24m params
 *
 * @param[in] load_capacitance  load capacitance, range:0~63, default:32, step:0.379pF(<20ppm), max:23.877pF
 *
 * @note load_capacitance will effect xtal 24m precision and frequency offset.
 *******************************************************************************
 **/
void drv_pmu_xtal32m_change_param(int load_capacitance);

/**
 *******************************************************************************
 * @brief  pmu xtal32m get param
 *
 * @param[in] load_capacitance  load capacitance
 *******************************************************************************
 **/
void drv_pmu_xtal32m_get_param(int *load_capacitance);

/**
 *******************************************************************************
 * @brief pmu gpio wakeup pin setup
 *
 * @param[in] pin_mask  pin mask
 * @param[in] trigger_type  wakeup trigger type
 *******************************************************************************
 **/
void drv_pmu_wakeup_pin_set(uint32_t pin_mask, pmu_pin_wakeup_type_t trigger_type);

/**
 *******************************************************************************
 * @brief wakeup pin get
 *
 * @return wakeup pin mask
 *******************************************************************************
 **/
uint32_t drv_pmu_wakeup_pin_get(void);

/**
 *******************************************************************************
 * @brief pmu wakeup pin register callback
 *
 * @param[in] sleep_callback  callback
 *
 * @note
 *  When deepsleep, xtal32k startup is very very slow (0.5s-2s),
 *  So the deepsleep pin wakeup irq may don't debounce
 *******************************************************************************
 **/
void drv_pmu_wakeup_pin_register_callback(drv_isr_callback_t sleep_callback);

/**
 *******************************************************************************
 * @brief what power status should be entried
 *
 * @return power status
 *******************************************************************************
 **/
pm_status_t drv_pmu_sleep_status(void);

/**
 *******************************************************************************
 * @brief  pmu sleep enter
 *
 * @param[in] sleep_status  sleep status
 * @param[in] reboot_req  reboot req
 *******************************************************************************
 */
void drv_pmu_sleep_enter(pm_status_t sleep_status, bool reboot_req);

/**
 *******************************************************************************
 * @brief pmu leave lowpower status, call by system
 *
 * @param step  step
 * @param sleep_status  lowpower state
 *******************************************************************************
 **/
void drv_pmu_sleep_leave(pmu_sleep_leave_step_t step, pm_status_t sleep_status);

/**
 *******************************************************************************
 * @brief pmu pin state store, call by system
 *******************************************************************************
 **/
void drv_pmu_pin_state_store(void);

/**
 *******************************************************************************
 * @brief pmu pin state restore, call by system
 *******************************************************************************
 **/
void drv_pmu_pin_state_restore(void);

/**
 *******************************************************************************
 * @brief Set pin mode
 *
 * @param[in] pin_mask  pin mask
 * @param[in] mode  pin mode
 *******************************************************************************
 **/
void drv_pmu_pin_mode_set(uint32_t pin_mask, pmu_pin_mode_t mode);

/**
 *******************************************************************************
 * @brief Set gpio driven current
 *
 * @param[in] pin_mask  pin mask
 * @param[in] driven  current driven (Large driven current should be push-pull output)
 *******************************************************************************
 **/
void drv_pmu_pin_driven_current_set(uint32_t pin_mask, pmu_pin_driver_current_t driven);

/**
 *******************************************************************************
 * @brief pmu analog power enable, call by system
 *
 * @param[in] enable  enable/disable
 * @param[in] ana  analog type
 *******************************************************************************
 **/
void drv_pmu_ana_enable(bool enable, pmu_ana_type_t ana);

/**
 *******************************************************************************
 * @brief analog is enabled
 *
 * @param[in] ana  analog module
 *
 * @return enabled?
 *******************************************************************************
 **/
bool drv_pmu_ana_is_enabled(pmu_ana_type_t ana);

/**
 *******************************************************************************
 * @brief Force system to reboot
 *******************************************************************************
 **/
void drv_pmu_force_reboot(void);


/**
 *******************************************************************************
 * @brief Get charge status
 *
 * @return status
 *******************************************************************************
 **/
pmu_charge_status_t drv_pmu_charge_status(void);

/**
 *******************************************************************************
 * @brief pmu reboot reason
 *
 * @return reason
 *******************************************************************************
 **/
pmu_reboot_reason_t drv_pmu_reboot_reason(void);

/**
 *******************************************************************************
 * @brief  drv pmu reboot reason check
 *
 * @note  called by ROM boot
 *******************************************************************************
 */
void drv_pmu_reboot_reason_check(void);

/**
 *******************************************************************************
 * @brief get retention reg value
 *
 * @note This reg value will lost only after power down.
 *
 * @return retention reg value
 *******************************************************************************
 **/
uint16_t drv_pmu_retention_reg_get(void);

/**
 *******************************************************************************
 * @brief set retention reg
 *
 * @note This reg value will lost only after power down.
 *
 * @param[in] value  reg value
 *******************************************************************************
 **/
void drv_pmu_retention_reg_set(uint16_t value);

/**
 * @brief  pmu 32k switch to rc
 *
 * @param[in] calib  calib
 * @param[in] pd_others  pd others
 **/
void drv_pmu_32k_switch_to_rc(bool calib, bool pd_others);

/// @cond
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
void drv_pmu_pin_wakeup_out_of_date(void);

/**
 *******************************************************************************
 * @brief  pmu recalib sysclk
 *******************************************************************************
 **/
void drv_pmu_topclk_recalib(void);
/// @endcond

/**
 *******************************************************************************
 * @brief  enable INSIDE DCDC
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
void drv_pmu_dcdc_enable(bool enable);

/**
 *******************************************************************************
 * @brief  pmu dcdc is enabled
 *
 * @return  is enabled
 *******************************************************************************
 **/
bool drv_pmu_dcdc_is_enabled(void);

/**
 *******************************************************************************
 * @brief  register step set, use: REGSW(...)
 *
 * @param[in] reg  reg
 * @param[in] mask  mask
 * @param[in] pos  pos
 * @param[in] value  value
 * @param[in] should_update  should update
 * @param[in] delay_us  delay us
 *******************************************************************************
 */
void drv_pmu_register_step_set(volatile uint32_t *reg, uint32_t mask, uint32_t pos, uint32_t value, bool should_update, uint32_t delay_us);

/**
 *******************************************************************************
 * @brief pmu dump
 *
 * @param[in] printf_dump_func  like printf
 *
 * @note
 *
 * @verbatim
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
 * @endverbatim
 *******************************************************************************
 **/
void drv_pmu_dump(void *printf_dump_func);

/**
 *******************************************************************************
 * @brief  drv pmu pin wakeup isr
 *******************************************************************************
 */
void drv_pmu_pin_wakeup_isr(void);

/**
 *******************************************************************************
 * @brief  pmu topclk double preset
 *******************************************************************************
 **/
void drv_pmu_topclk_x2_enable(bool enable);

/**
 *******************************************************************************
 * @brief  pmu topclk switch to rc32m
 *******************************************************************************
 **/
void drv_pmu_topclk_switch_to_rc32m(void);

/**
 *******************************************************************************
 * @brief  pmu xtal32m switch to 32m
 *******************************************************************************
 **/
void drv_pmu_topclk_switch_to_xtal32m(void);

/**
 *******************************************************************************
 * @brief  pmu xtal32m switch to 64m
 *******************************************************************************
 **/
void drv_pmu_topclk_switch_to_xtal32m_x2(void);

/**
 *******************************************************************************
 * @brief  pmu rc32m power enable
 *
 * @return is_last_enabled ?
 *******************************************************************************
 **/
bool drv_pmu_topclk_rc32m_power_enable(bool enable);

/**
 *******************************************************************************
 * @brief  pmu topclk xtal32m power enable
 *
 * @param[in] enable
 *******************************************************************************
 **/
void drv_pmu_topclk_xtal32m_power_enable(bool enable);

/**
 *******************************************************************************
 * @brief  pmu topclk xtal32m is enabled
 *******************************************************************************
 **/
bool drv_pmu_topclk_xtal32m_is_enabled(void);

/**
 *******************************************************************************
 * @brief  pmu topclk xtal32m is enabled
 *******************************************************************************
 **/
bool drv_pmu_topclk_xtal32m_x2_is_enabled(void);

/**
 *******************************************************************************
 * @brief pmu reset cpu
 *
 * @note: This function must be INLINE function, avoid stack used
 *******************************************************************************
 **/
__STATIC_FORCEINLINE void drv_pmu_cpu_reset(void)
{
    // flag
    register_set1(&OM_PMU->SW_STATUS, PMU_SW_STATUS_REBOOT_FROM_SOFT_RESET_MASK);

    NVIC_SystemReset();
}

/**
 *******************************************************************************
 * @brief get ultra deep sleep wakeup pin value
 *
 * @note Only valid when drv_pmu_reboot_reason() return PMU_REBOOT_FROM_ULTRA_DEEP_SLEEP.
 * @note Must be called before any drv_pmu_wakeup_pin_set() calling.
 *
 * @return ultra deep sleep wakeup pin value
 *******************************************************************************
 **/
__STATIC_INLINE uint32_t drv_pmu_ultra_deep_sleep_wakeup_pin(void)
{
    return OM_PMU->GPIO_LATCH;
}

/**
 *******************************************************************************
 * @brief  pmu jtag enable
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_pmu_jtag_enable(bool enable)
{
    REGW(&OM_PMU->MISC_CTRL, MASK_1REG(PMU_MISC_CTRL_O_JTAG_ENABLE, enable ? 1 : 0));
}

/**
 *******************************************************************************
 * @brief  pmu pin io13 reset mode enable
 *
 * @note  IO13 is GPIO mode by default.
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_pmu_pin_io13_reset_mode_enable(bool enable)
{
    //   1:  IO10 is RESET
    //   0:  IO10 is GPIO
    REGW(&OM_PMU->MISC_CTRL_1, MASK_1REG(PMU_MISC_CTRL_1_EN_RSTB, enable ? 1 : 0));
}

/**
 *******************************************************************************
 * @brief  pmu pull up resistance set
 *
 * @param[in] pin_mask    pin mask
 * @param[in] resistance  resistance
 *******************************************************************************
 */
__STATIC_INLINE void drv_pmu_pull_up_resistance_set(uint32_t pin_mask, pmu_pin_pu_resistance_t resistance)
{
    if(resistance & 0x01)
        OM_PMU->GPIO_DRV_CTRL_2 |= pin_mask;
    else
        OM_PMU->GPIO_DRV_CTRL_2 &= ~pin_mask;

    if(resistance & 0x02)
        OM_PMU->GPIO_PU_CTRL_1 |= pin_mask;
    else
        OM_PMU->GPIO_PU_CTRL_1 &= ~pin_mask;
}

/**
 *******************************************************************************
 * @brief  Enable the chip pof voltage, set trigger threshold. Accuracy is positive or negative 5% of the threshold.
 *
 * @param[in] enable  true or false
 * @param[in] voltage Set trigger threshold
 * 
 *******************************************************************************
 */
extern void drv_pmu_pof_enable(bool enable, uint8_t voltage);

/**
 *******************************************************************************
 * @brief  Obtain whether the chip voltage is below the threshold.
 * 
 * @return status:
 *    - true: the chip voltage is below threshold.
 *    - false: the chip voltage above threshold.
 *******************************************************************************
 */
extern bool drv_pmu_pof_is_trigger(void);

#ifdef __cplusplus
}
#endif

#endif  /* (RTE_PMU) */

#endif  /* __PMU_OM662X_H */


/** @} */

