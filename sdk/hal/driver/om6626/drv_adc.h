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
 * @file     drv_adc.h
 * @brief    header file for ADC
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __DRV_ADC_H
#define __DRV_ADC_H

/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_GPADC)
#include <stdint.h>
#include "om_driver.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */
/// ADC channels
typedef enum {
    /// temperature channel
    ADC_CH_TEMPERATURE = (1<<0U),
    /// VBAT channel
    ADC_CH_VBAT        = (1<<1U),
    /// GPIO2 channel
    ADC_CH_GPIO2       = (1<<4U),
    /// GPIO3 channel
    ADC_CH_GPIO3       = (1<<5U),
    /// GPIO7 channel
    ADC_CH_GPIO7       = (1<<6U),
    /// GPIO8 channel
    ADC_CH_GPIO8       = (1<<7U),
    /// GPIO11 channel
    ADC_CH_GPIO11      = (1<<8U),
    /// GPIO12 channel
    ADC_CH_GPIO12      = (1<<9U),
    /// GPIO14 channel
    ADC_CH_GPIO14      = (1<<10U),
    /// GPIO15 channel
    ADC_CH_GPIO15      = (1<<11U),
} drv_adc_channel_t;

/// ADC mode
typedef enum {
    /// single ended mode
    ADC_MODE_SINGLE  = 0U,
    /// differential mode
    ADC_MODE_DIFF    = 1U,
} drv_adc_mode_t;

/// ADC gain
typedef enum {
    /// gain: 1, select VBAT as reference voltage
    ADC_GAIN_1            = 0U,
    /// gain: 1/3, select internal reference
    ADC_GAIN_1_3          = 1U,
    /// gain: 1, select internal reference
    ADC_GAIN_1_SEL_VREF_0 = 2U,

    ADC_GAIN_MAX,
} drv_adc_gain_t;

/// ADC summation number
typedef enum {
    /// summation number: 2
    ADC_SUM_NUM_2   = 0U,
    /// summation number: 4
    ADC_SUM_NUM_4   = 1U,
    /// summation number: 8
    ADC_SUM_NUM_8   = 2U,
    /// summation number: 16
    ADC_SUM_NUM_16  = 3U,
    /// summation number: 32
    ADC_SUM_NUM_32  = 4U,
    /// summation number: 64
    ADC_SUM_NUM_64  = 5U,
    /// summation number: 128
    ADC_SUM_NUM_128 = 6U,
    /// summation number: 256
    ADC_SUM_NUM_256 = 7U,

    ADC_SUM_NUM_MAX,
} drv_adc_sum_num_t;

/// ADC sampling cycles
typedef enum {
    /// sampling cycles: 0
    ADC_SAMPLING_CYCLES_0   = 0U,
    /// sampling cycles: 2
    ADC_SAMPLING_CYCLES_2   = 1U,
    /// sampling cycles: 8
    ADC_SAMPLING_CYCLES_8   = 2U,
    /// sampling cycles: 16
    ADC_SAMPLING_CYCLES_16  = 3U,
    /// sampling cycles: 32
    ADC_SAMPLING_CYCLES_32  = 4U,
    /// sampling cycles: 64
    ADC_SAMPLING_CYCLES_64  = 5U,
    /// sampling cycles: 80
    ADC_SAMPLING_CYCLES_80  = 6U,
    /// sampling cycles: 128
    ADC_SAMPLING_CYCLES_128 = 7U,

    ADC_SAMPLING_CYCLES_MAX,
} drv_adc_sampling_cycles_t;

/// ADC calib sel
typedef enum {
    ADC_CALIB_FT_SINGLE = 0,
    ADC_CALIB_CP_1      = 1,
    ADC_CALIB_CP_2      = 2,
} drv_adc_calib_sel_t;

/// ADC control
typedef enum {
    ADC_CONTROL_ENABLE_CLOCK        = 0U,    /**< enable adc clock */
    ADC_CONTROL_DISABLE_CLOCK       = 1U,    /**< enable adc clock */
    ADC_CONTROL_SET_PARAM           = 2U,    /**< set value to register */
    ADC_CONTROL_IS_BUSY             = 3U,    /**< check adc is busy */
    ADC_CONTROL_READ_TEMPERATURE    = 4U,    /**< read temperature */
    ADC_CONTROL_SET_CALIB_TEMPER    = 5U,    /**< set temperature during calibration */
    ADC_CONTROL_TEMPERATURE_COMPEN  = 6U,    /**< check adc is busy */
} drv_adc_control_t;

/// ADC temperature compensation
typedef struct {
    /// ADC temperature data
    int16_t  temp_data;
    /// ADC vbat date
    int16_t  vbat_data;
} drv_adc_temperature_compen_t;

/// ADC config
typedef struct {
    /// ADC channels
    uint16_t                   channel;
    /// ADC mode
    drv_adc_mode_t             mode;
    /// ADC gain
    drv_adc_gain_t             gain;
    /// ADC summation number
    drv_adc_sum_num_t          sum_num;
    /// ADC sampling cycles
    drv_adc_sampling_cycles_t  sampling_cycles;
} drv_adc_config_t;

/// ADC calibration parameters in efuse
typedef struct {
    uint16_t gain_error;
    uint16_t vos;
    uint16_t vos_temp;
} __PACKED drv_adc_calib_t;

/// ADC calibration parameters in flash
typedef struct {
    drv_adc_calib_t data[ADC_GAIN_MAX];
} __PACKED drv_adc_flash_calib_t;

/// ADC extral calibration parameters in flash
typedef struct {
    uint16_t gain_error_vbat;
} __PACKED drv_adc_flash_calib_ex_t;

/// ADC extral calibration parameters in flash
typedef struct {
    uint16_t vbg_code_trim_1_vbat_3p3v;
    uint16_t vbg_code_trim_3_vbat_3p3v;
} __PACKED drv_adc_flash_calib_ex_2_t;

/// ADC extral calibration parameters in flash
typedef struct {
    uint16_t vbg_code_trim_1_vbat_2p3v;
    uint16_t vbg_code_trim_3_vbat_2p3v;
    int16_t temperature_vbat_3p3v;
    int16_t temperature_vbat_2p3v;
} __PACKED drv_adc_flash_calib_ex_3_t;

/// ADC calibration parameters
typedef struct {
    const drv_adc_calib_t               *efuse;
    const drv_adc_flash_calib_t         *flash;
    const drv_adc_flash_calib_ex_t      *flash_ex;
    const drv_adc_flash_calib_ex_2_t    *flash_ex_2;
    const drv_adc_flash_calib_ex_3_t    *flash_ex_3;
    const void                          *reserved[5];
} drv_adc_cpft_calib_t;

/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief ADC interrupt service routine
 *
 * @return None
 *******************************************************************************
 **/
extern void drv_adc_isr(void);

/**
 *******************************************************************************
 * @brief GPADC initialization
 *
 * @param[in] config         Configuration for GPADC
 *
 * @return status:
 *    - OM_ERROR_OK:         init done
 *    - others:              No
 *******************************************************************************
 */
extern om_error_t drv_adc_init(const drv_adc_config_t *config);

/**
 *******************************************************************************
 * @brief Register isr callback for transmit/receive by interrupt & gpdma mode
 *
 * @param[in] cb       Pointer to callback
 *
 * @return None
 *******************************************************************************
 */
extern void drv_adc_register_isr_callback(drv_isr_callback_t cb);

/**
 *******************************************************************************
 * @brief Prepare receive number of data by block mode
 *
 * @param[in] channel        adc channels
 * @param[in] data           Pointer where data to receive from adc.when reading the temperature,
 *                           for example,read 265 for 26.5 degrees Celsius.when the voltage is read,
 *                           give an example,2900 indicates 2900 millivolts.
 * @param[in] num            Number of data from adc channel
 *
 * @return status:
 *    - OM_ERROR_OK:         receive done
 *    - others:              No
 *******************************************************************************
 */
extern om_error_t drv_adc_read(uint16_t channel, int16_t *data, uint16_t num);

/**
 *******************************************************************************
 * @brief Prepare receive number of data by interrupt mode,
 *
 * @param[in] channel        adc channels
 * @param[in] data           Pointer where data to receive from adc
 * @param[in] num            Number of data from adc receiver
 *
 * @return status:
 *    - OM_ERROR_OK:         Begin to receive
 *    - others:              No
 *******************************************************************************
 */
extern om_error_t drv_adc_read_int(uint16_t channel, int16_t *data, uint16_t num);

#if (RTE_DMA)
/**
 *******************************************************************************
 * @brief Allocate dma channel for adc
 *
 * @return status:
 *    - OM_ERROR_OK:         Allocate ok
 *    - others:              No
 *******************************************************************************
 */
extern om_error_t drv_adc_dma_channel_allocate(void);

/**
 *******************************************************************************
 * @brief Release dma channel for adc
 *
 * @return status:
 *    - OM_ERROR_OK:         Release ok
 *    - others:              No
 *******************************************************************************
 */
extern om_error_t drv_adc_dma_channel_release(void);

/**
 *******************************************************************************
 * @brief Prepare receive number of data by GPDMA mode
 *
 * @param[in] channel        adc channels
 * @param[in] data           Pointer where data to receive from adc
 * @param[in] num            Number of data from adc receiver
 *
 * @return status:
 *    - OM_ERROR_OK:         Begin to receive
 *    - others:              No
 *******************************************************************************
 */
extern om_error_t drv_adc_read_dma(uint16_t channel, int16_t *data, uint16_t num);
#endif  /* (RTE_DMA) */

/**
 *******************************************************************************
 * @brief Control ADC interface.
 *
 * @param[in] control        Operation
 * @param[in] argu           Operation argument
 *
 * @return status:           Control status
 *******************************************************************************
 */
extern void *drv_adc_control(drv_adc_control_t control, void *argu);

#ifdef ADC_CALIB_EN
/**
 *******************************************************************************
 * @brief Calibrate ADC and initialize ADC compensation parameters.
 *
 * @param[in] gain           adc gain
 * @param[in] calib_sel      adc select calibration phase
 * @param[out] pvalue        Pointer to @p drv_adc_calib_t object where the
 *                           calibration parameters are to be returned.
 * @param[out] pvalue_vbat   Pointer to @p drv_adc_flash_calib_ex_t object where the
 *                           calibration extra parameters are to be returned.
 * @param[out] pvalue_temp   Pointer to @p drv_adc_flash_calib_ex_2_t object where the
 *                           calibration extra parameters are to be returned.
 * @param[out] pvalue_temp_2 Pointer to @p drv_adc_flash_calib_ex_3_t object where the
 *                           calibration extra parameters are to be returned.
 *
 * @return status:
 *    - OM_ERROR_OK:         Calibrate done
 *    - others:              No
 *******************************************************************************
 */
extern om_error_t drv_adc_calibrate(drv_adc_calib_t *pvalue, drv_adc_flash_calib_ex_t *pvalue_vbat, drv_adc_flash_calib_ex_2_t *pvalue_temp,
                                    drv_adc_flash_calib_ex_3_t *pvalue_temp_2, drv_adc_gain_t gain, drv_adc_calib_sel_t calib_sel);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* RTE_GPADC */
#endif  /* __DRV_ADC_H */


/** @} */

