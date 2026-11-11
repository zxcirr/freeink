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
 * @file     driver_om6626.h
 * @brief    om662x Driver
 * @date     22. Oct 2021
 * @author   OnMicro SW Team
 *
 * @defgroup DRIVER Driver
 * @ingroup  HAL
 * @brief    Driver module
 * @details  Driver module
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __DRIVER_OM6626_H
#define __DRIVER_OM6626_H


/*******************************************************************************
 * MACRO
 */


/*******************************************************************************
 * INCLUDES
 */
#include "om_error.h"
#include "om_utils.h"
#include "om_compiler.h"

#include "pm.h"

#include "../common/drv_utils.h"
#include "../common/drv_common.h"

#include "../om6626/drv_rcc.h"
#include "../om6626/drv_pmu.h"
#include "../om6626/drv_pinmux.h"
#include "../om6626/drv_calib.h"
#include "../om6626/drv_calib_repair.h"
#include "../om6626/drv_adc.h"

#include "../common/drv_cortex.h"
#include "../common/drv_cache.h"

#include "../common/drv_efuse.h"
#include "../common/drv_usart.h"
#include "../common/drv_usart_ex.h"
#include "../common/drv_spi.h"
#include "../common/drv_i2c.h"
#include "../common/drv_wdt.h"
#include "../common/drv_pmu_timer.h"
#include "../common/drv_24g.h"
#include "../common/drv_aes_hw.h"
#include "../common/drv_gpio.h"
#include "../common/drv_sf_base.h"
#include "../common/drv_sf.h"
#include "../common/drv_sf_sys.h"
#include "../common/drv_dma.h"
#include "../common/drv_rtc.h"
#include "../common/drv_radio.h"
#include "../common/drv_tim.h"
#include "../common/drv_lp_tim.h"
#include "../common/drv_rng.h"

#include "eeprom.h"

#ifdef CONFIG_CPFT_DATA_FETCH
#include "../om6626/drv_cpft.h"
#endif

#include "om_time.h"

#if (RTE_CORTEX_DWT_DELAY)
/// delay with CPU cycles
#define DRV_DELAY_CYCLES(cycles)                        drv_dwt_delay_cycles(cycles)
/// delay microsecond
#define DRV_DELAY_US(us)                                drv_dwt_delay_us(us)
/// delay millisecond
#define DRV_DELAY_MS(ms)                                drv_dwt_delay_ms(ms)
#else
#error "Delay functions are undefined"
/// delay with CPU cycles
#define DRV_DELAY_CYCLES(cycles)
/// delay microsecond
#define DRV_DELAY_US(us)
/// delay millisecond
#define DRV_DELAY_MS(ms)
#endif

#if (RTE_CORTEX_DWT_TIMEOUT)
#define DRV_WAIT_MS_UNTIL_TO(wait_val, to_ms, ret)      DRV_DWT_WAIT_MS_UNTIL_TO(wait_val, to_ms, ret)
#define DRV_WAIT_US_UNTIL_TO(wait_val, to_us, ret)      DRV_DWT_WAIT_US_UNTIL_TO(wait_val, to_us, ret)
#else
#warning "Timeout functions are undefined"
#define DRV_WAIT_MS_UNTIL_TO(wait_val, to_ms, ret)  {while (wait_val); ret = OM_ERROR_OK;}
#define DRV_WAIT_US_UNTIL_TO(wait_val, to_us, ret)  {while (wait_val); ret = OM_ERROR_OK;}
#endif

#endif  /* __DRIVER_OM6626_H */


/** @} */

