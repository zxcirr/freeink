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
 * @file     drv_calib_repair.h
 * @brief    Header file of Calib Repair
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup CALIB_REPAIR CALIB_REPAIR
 * @ingroup  DRIVER
 * @brief    Calib Repair for om662x
 * @details  Calib Repair for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __DRV_CALIB_REPAIR_H
#define __DRV_CALIB_REPAIR_H


/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
#define DRV_CALIB_REPAIR_DCDC_MAXDBM_DELTA                (SYS_IS_A3() ? 6 : 8)
#define DRV_CALIB_REPAIR_LDO_MAXDBM_DELTA                 (SYS_IS_A3() ? 4 : 5)

#define DRV_CALIB_REPAIR_DCDC_8DBM_DELTA                  4
#define DRV_CALIB_REPAIR_LDO_8DBM_DELTA                   2

#define DRV_CALIB_REPAIR_DCDC_7DBM_DELTA                  1
#define DRV_CALIB_REPAIR_LDO_7DBM_DELTA                   0

#define DRV_CALIB_REPAIR_DCDC_HIGHT_T_DELTA               3
#define DRV_CALIB_REPAIR_LDO_HIGHT_T_DELTA                1

/*******************************************************************************
 * TYPEDEFS
 */
typedef struct
{
    // Fix RF bug: temperature
    int16_t temperature;

    // rf and rc temperature repiar
    uint32_t rc_rf_repair_delay;
    uint32_t rc_rf_repair_time;
    int16_t  rc_repair_temperature;
    int16_t  rf_repair_temperature;

    // rc32k temperature repair
    uint32_t rc32k_repair_delay;
    uint32_t rc32k_repair_time;

    // xtal32m
    uint32_t xtal32m_cap;

    // sys voltage
    int8_t trim_vref;           // in CP
    int8_t dig_ldo;             // in CP
    int8_t pa_ldo;              // in CP
    int8_t dcdc_vout;           // in CP
    int8_t pfd_ldo;             // in CP
    int8_t vco_ldo;             // in CP
    int8_t buff_ldo;            // in CP
    int8_t ana_ldo;             // in CP/FT
    int8_t kdco_lut_1m;         // in FT
    int8_t kdco_lut_2m;         // in FT
    int8_t con_bias_idac_pll;   // in FT
    int8_t vco_cur;             // in CP/FT
}drv_calib_repair_t;

/*******************************************************************************
 * EXTERN VARIABLES
 */
extern drv_calib_repair_t drv_calib_repair_env;


/*******************************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief  calib repair rc rf init
 **/
void drv_calib_repair_init(void);

/**
 * @brief rf pll temperature repair
 **/
void drv_calib_repair_rf_pll_temperature_repair(bool is_calib_start);


#ifdef __cplusplus
}
#endif

#endif  /* __DRV_CALIB_H */


/** @} */

