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
 * @file     drv_adc.c
 * @brief    adc
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
#if (RTE_GPADC)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */
#ifdef ADC_CALIB_EN
#define GPADC_OUT_IDEAL_DIFF(ideal_2, ideal_1)      ((ideal_2) - (ideal_1))

#define GPADC_OUT_UPPER_BOUND(ideal)                ((ideal) * 13 / 10)
#define GPADC_OUT_LOW_BOUND(ideal)                  ((ideal) * 6 / 10)

#define GPADC_CALIB_COMMON_NUM                      (2)
#endif

#define GPADC_CALIB_VBG_NUM                         (10)

#define GPADC_CH_MAX                                (12)

#ifdef CONFIG_ADC_LOG
#define OM_ADC_LOG_DEBUG(format, ...)               om_log(OM_LOG_INFO, format,  ## __VA_ARGS__)
#else
#define OM_ADC_LOG_DEBUG(format, ...)
#endif

/*******************************************************************************
 * TYPEDEFS
 */
#ifdef ADC_CALIB_EN
typedef struct {
    uint16_t gainErr_set;
    uint16_t vos_set;
    uint16_t vosTemp_set;
    uint16_t gainErr_vbat_set;

    uint16_t vbg_code_trim_1;
    uint16_t vbg_code_trim_3;

    uint16_t gainErr_set_gain_1_3;
    uint16_t vos_set_gain_1_3;
    float    vosTemp_cal_gain_1_3;

    float    gainErr_cal;
    float    vos_cal;
    float    vosTemp_cal;
    float    gainErr_vbat_cal;

    float    adcOut_1;
    float    adcOut_2;
} drv_adc_calib_param_t;
#endif

typedef struct {
    bool      use_efuse;
    bool      use_flash_ex;
    bool      use_flash_ex_2;
    bool      use_flash_ex_3;
    bool      register_store;
#if (RTE_DMA)
    uint8_t   dma_chan;
#endif

    uint16_t  busy;
    uint16_t  rx_cnt;
    uint16_t  rx_num;
    int16_t  *rx_buf;
    float     calib_temper;
    uint32_t  adc_cfg0;
    uint32_t  adc_cfg1;
    uint32_t  adc_cfg2;
    uint32_t  cali_cfg;
    drv_adc_mode_t              mode;
    drv_adc_gain_t              gain;
    drv_isr_callback_t          isr_cb;
    drv_adc_calib_t             cp_calib_value;
    drv_adc_flash_calib_t       ft_calib_value;
    drv_adc_flash_calib_ex_t    ft_calib_ex_value;
    drv_adc_flash_calib_ex_2_t  ft_calib_ex_2_value;
    drv_adc_flash_calib_ex_3_t  ft_calib_ex_3_value;
} drv_adc_env_t;

/*******************************************************************************
 * CONST & VARIABLES
 */
#ifdef ADC_CALIB_EN
static const float GPADC_SINGLE_OUT_1_IDEAL[ADC_GAIN_MAX] = {
    248.24, 218.45, 655.36
};
static const float GPADC_SINGLE_OUT_2_IDEAL[ADC_GAIN_MAX] = {
    3723.64, 3276.8, 3276.8
};
static drv_adc_calib_param_t adc_para;
#endif

static drv_adc_env_t adc_env = {
#if (RTE_DMA)
    .dma_chan     = DMA_NUMBER_OF_CHANNELS,
#endif
    .calib_temper = 25,
    .busy         = 0,
    .rx_cnt       = 0,
    .rx_num       = 0,
    .rx_buf       = NULL,
    .mode         = ADC_MODE_SINGLE,
    .isr_cb       = NULL,
};



/*******************************************************************************
 * LOCAL FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief set ADC calibration parameters to register
 *
 * @param[in] gain  adc gain
 *
 * @return none
 *******************************************************************************
 **/
static void drv_adc_set_calib2reg(drv_adc_gain_t gain)
{
    if (adc_env.use_efuse) {
        REGW(&OM_GPADC->CALI_CFG, MASK_1REG(GPADC_GAIN_ERR, adc_env.cp_calib_value.gain_error & 0x3FFF));
        if (adc_env.cp_calib_value.vos >> 14 & 0x1) {
            REGW(&OM_GPADC->CALI_CFG, MASK_1REG(GPADC_VOS, adc_env.cp_calib_value.vos | (0x1 << 15)));
        } else {
            REGW(&OM_GPADC->CALI_CFG, MASK_1REG(GPADC_VOS, adc_env.cp_calib_value.vos));
        }
        if (adc_env.cp_calib_value.vos_temp >> 9 & 0x1) {
            REGW(&OM_GPADC->TEMP_CFG, MASK_1REG(GPADC_VOS_TEMP, (adc_env.cp_calib_value.vos_temp << 2) | (0x3 << 12)));
        } else {
            REGW(&OM_GPADC->TEMP_CFG, MASK_1REG(GPADC_VOS_TEMP, adc_env.cp_calib_value.vos_temp << 2));
        }
    } else {
        REGW(&OM_GPADC->CALI_CFG, MASK_2REG(GPADC_GAIN_ERR, adc_env.ft_calib_value.data[gain].gain_error & 0x3FFF,
                                            GPADC_VOS, adc_env.ft_calib_value.data[gain].vos));
        REGW(&OM_GPADC->TEMP_CFG, MASK_1REG(GPADC_VOS_TEMP, adc_env.ft_calib_value.data[gain].vos_temp & 0x3FFF));
    }
}

/**
 *******************************************************************************
 * @brief set ADC temperature calibration by software
 *
 * @param[in] ret_raw   adc raw data
 *
 * @return temperature
 *******************************************************************************
 **/
static int16_t drv_adc_temperature_calib_by_sw(uint16_t ret_raw)
{
    float ret, vt, gain_error, vos, vos_temp, gain;

    if (adc_env.use_efuse) {
        gain_error = 8192.0f / adc_env.cp_calib_value.gain_error;
        if (adc_env.cp_calib_value.vos >> 14 & 0x1) {
            vos = (int16_t)(adc_env.cp_calib_value.vos | (0x1 << 15)) / 16.0f;
        } else {
            vos = (int16_t)(adc_env.cp_calib_value.vos) / 16.0f;
        }
        if (adc_env.cp_calib_value.vos_temp >> 9 & 0x1) {
            vos_temp = (int16_t)(((adc_env.cp_calib_value.vos_temp << 2) | (0x3 << 12)) << 2) / 256.0f;
        } else {
            vos_temp = (int16_t)((adc_env.cp_calib_value.vos_temp << 2) << 2) / 256.0f;
        }
        gain = 3;
    } else {
        gain_error = 8192.0f / adc_env.ft_calib_value.data[adc_env.gain].gain_error;
        vos = (int16_t)adc_env.ft_calib_value.data[adc_env.gain].vos / 16.0f;
        vos_temp = (int16_t)(adc_env.ft_calib_value.data[adc_env.gain].vos_temp << 2) / 256.0f;
        gain = (ADC_GAIN_1_3 == adc_env.gain) ? 3 : 1;
    }

    vt = ((ret_raw / 16.0f) - vos) * 1.25f / 4096.0f / gain_error * gain;
    ret = vt * 1000 / 2.959f - vos_temp - 273;

    return (int16_t)(ret * 10);
}

/**
 *******************************************************************************
 * @brief set ADC temperature compensation
 *
 * @param[in] argu    Operation argument
 *
 * @return temperature
 *******************************************************************************
 **/
static int16_t drv_adc_temperature_compen(drv_adc_temperature_compen_t *argu)
{
    float ret;

    int16_t ret_vbat = argu->vbat_data;
    int16_t ret_temp = argu->temp_data;

    if (adc_env.use_flash_ex_3) {
        float step_temp = adc_env.ft_calib_ex_3_value.temperature_vbat_3p3v - adc_env.ft_calib_ex_3_value.temperature_vbat_2p3v;
        ret = (3300 - ret_vbat) * (step_temp / 10.0f) / 1000.0f + ret_temp / 10.0f;
    } else {
        ret = (3300 - ret_vbat) * 0.3f / 0.2f / 1000.0f + ret_temp / 10.0f;
    }

    return (int16_t)(ret * 10);
}

/**
 *******************************************************************************
 * @brief get channel data
 *
 * @param[in] channel        adc channels
 * @param[in] data           Pointer where data to receive from adc.when reading the temperature,
 *                           for example,read 265 for 26.5 degrees Celsius.when the voltage is read,
 *                           give an example,2900 indicates 2900 millivolts.
 * @param[in] num            Number of data from adc channel
 *
 * @return raw data
 *******************************************************************************
 **/
static void drv_adc_get_channel_data(uint16_t channel, int16_t *data, uint16_t num)
{
    /* clear interrupt */
    OM_GPADC->INTR = GPADC_INTR_ALL_MASK;

    /* power-on ADC */
    REGW(&OM_GPADC->ADC_CFG0, MASK_1REG(GPADC_PD, 0));

    /* digital bug : abandon first date */
    for (uint8_t j = 0; j < GPADC_CH_MAX; j++) {
        if ((channel >> j) & 0x1) {
            /* check interrupt from ch0 to ch9 and clear*/
            while ((OM_GPADC->INTR & (1 << j)) != (1 << j));
            OM_GPADC->INTR |= 1 << j;
        }
    }

    /* get channel data */
    int i = 0;
    while (i < num) {
        for (uint8_t j = 0; j < GPADC_CH_MAX; j++) {
            if ((channel >> j) & 0x1) {
                /* check interrupt from ch0 to ch9 and clear*/
                while ((OM_GPADC->INTR & (1 << j)) != (1 << j));
                OM_GPADC->INTR |= 1 << j;

                data[i%num] = REGR(((uint32_t *)&OM_GPADC->CH_0_DATA) + j, MASK_POS(GPADC_DATA_LOW));
                i++;
            }
        }
    }

    /* power-down ADC */
    REGW(&OM_GPADC->ADC_CFG0, MASK_1REG(GPADC_PD, 1));
}

/**
 *******************************************************************************
 * @brief convert channel data
 *
 * @param[in] channel   adc channel
 * @param[in] ret_raw   adc raw data
 *
 * @return voltage/temperature
 *******************************************************************************
 **/
static int16_t drv_adc_convert_channel_data(drv_adc_channel_t channel, uint16_t ret_raw)
{
    int16_t ret = 0;
    float k = 0;

    switch(channel) {
        case ADC_CH_TEMPERATURE:
            ret = drv_adc_temperature_calib_by_sw(ret_raw);
            break;
        case ADC_CH_VBAT:
            if (adc_env.use_flash_ex) {
                k = adc_env.ft_calib_ex_value.gain_error_vbat / 32768.0f;
                ret = (int16_t)(ret_raw * 3 * 1000 * k / 2048.0f + 0.5);
            } else {
                ret = (int16_t)(ret_raw * 3 * 1000 * 1.04286f / 2048.0f + 0.5);
            }
            break;
        default:
            if (ADC_MODE_SINGLE == adc_env.mode) {
                if (ADC_GAIN_1 == adc_env.gain) {
                    /* fix digital bug : ret = ret / 1.25 * 3.3 */
                    ret = (int16_t)(ret_raw * 132000 / 102400.0f + 0.5);
                } else {
                    ret = (int16_t)(ret_raw * 1000 / 2048.0f + 0.5);
                }
            } else {
                ret = (int16_t)(((ret_raw / 16) * 2 - 4096) * 1000 * 5 / 4 / 4096);
            }
            break;
    }

    return ret;
}

/**
 *******************************************************************************
 * @brief adc register config
 *
 * @param[in] channel   adc channel
 * @param[in] ret_raw   adc raw data
 *
 * @return none
 *******************************************************************************
 **/
static void drv_adc_config(const drv_adc_config_t* config)
{
    /* ADC_CFG0 config */
    REGW(&OM_GPADC->ADC_CFG0, MASK_7REG(GPADC_MODE_SEL, config->mode,
                                        GPADC_EN_SCALE, (ADC_GAIN_1_3 == config->gain)? 1 : 0,
                                        GPADC_SEL_VREF, (ADC_GAIN_1 == config->gain)? 1 : 0,
                                        GPADC_PD_VBAT_DET, ((ADC_CH_VBAT & config->channel) == ADC_CH_VBAT)? 0 : 1,
                                        GPADC_PMU_TS_ICTRL, (ADC_CH_TEMPERATURE == config->channel)? 3 : 0,
                                        GPADC_VCTRL_LDO, 0,
                                        GPADC_CTRL_VREF, 1));

    /* ADC_CFG1 config */
    REGW(&OM_GPADC->ADC_CFG1, MASK_7REG(GPADC_SMP_TIME, config->sampling_cycles,
                                        GPADC_AVG_BYPASS, 1,
                                        GPADC_SUM_NUM, config->sum_num,
                                        GPADC_AUTO_PD1, 0,
                                        GPADC_AUTO_PD2, 0,
                                        GPADC_TRIG_RES, 0,
                                        GPADC_DIGI_CLK_FREQ, 0x20));

    /* ADC_CFG2 config */
    REGW(&OM_GPADC->ADC_CFG2, MASK_4REG(GPADC_SEQ_VECT, config->channel,
                                        GPADC_SCANDIR, 1,
                                        GPADC_SEQ_LIFE, 0,
                                        GPADC_SW_TRIGGER_TRUE, 1));

    /* set AUTO_COMPEN, default 0 */
    REGW(&OM_GPADC->CALI_CFG, MASK_1REG(GPADC_AUTO_COMPEN, (ADC_MODE_DIFF == config->mode || ADC_CH_TEMPERATURE == config->channel)? 0 : 1));

    /* PD_CFG0 config */
    REGW(&OM_DAIF->PD_CFG0, MASK_1REG(DAIF_GPADC_INPUT_SELN_VBAT, (ADC_MODE_DIFF == config->mode)? 1 : 0));

    /* set the interval of interruption */
    REGW(&OM_GPADC->DLY_CFG, MASK_3REG(GPADC_CFG_CHG_DLY, 0x50,
                                       GPADC_PD_DLY_MAX, 0x50,
                                       GPADC_ORB_DLY, 0xE));
}

#if (RTE_DMA)
/**
 *******************************************************************************
 * @brief dma callback
 *
 * @return none
 *******************************************************************************
 **/
static void adc_dma_event_cb(void *cb_param, drv_event_t event, void *next_chain)
{
    drv_event_t drv_event = DRV_EVENT_COMMON_NONE;

    switch (event) {
        case DRV_DMA_EVENT_TERMINAL_COUNT_REQUEST:
            drv_event = DRV_EVENT_COMMON_READ_COMPLETED;

            /* disable ADC DMA */
            REGW0(&OM_GPADC->ADC_CFG1, GPADC_DMA_EN_MASK);

            /* power-down ADC */
            REGW(&OM_GPADC->ADC_CFG0, MASK_1REG(GPADC_PD, 1));

            for (uint32_t i = 0; i < adc_env.rx_num; i++) {
                adc_env.rx_buf[i] = drv_adc_convert_channel_data((drv_adc_channel_t)adc_env.busy, (uint16_t)(adc_env.rx_buf[i]));
            }
            break;
        case DRV_DMA_EVENT_ABORT:
            drv_event = DRV_EVENT_COMMON_ABORT;
            break;
        default:
            drv_event = DRV_EVENT_COMMON_ERROR;
            break;
    }

    adc_env.busy = 0;

    if (adc_env.isr_cb) {
        adc_env.isr_cb(OM_GPADC, drv_event, (void *)(adc_env.rx_buf), (void *)((uint32_t)adc_env.rx_num));
    }
}
#endif

/**
 *******************************************************************************
 * @brief set new ADC calibration parameters
 *
 * @return none
 *******************************************************************************
 **/
static void drv_adc_set_new_calibrate_param(void)
{
    if (adc_env.use_flash_ex_2) {
        int16_t vbg_code_trim_1[GPADC_CALIB_VBG_NUM];
        int16_t vbg_code_trim_3[GPADC_CALIB_VBG_NUM];
        int16_t vbat_out[GPADC_CALIB_VBG_NUM];
        int32_t vbg_code_trim_1_new = 0;
        int32_t vbg_code_trim_3_new = 0;
        int32_t vbat_out_sum = 0;
        uint16_t gain_error = 0;
        uint16_t gain_error_new = 0;
        uint16_t out_of_range = 0;
        float k = 0;
        float k_sum = 0;
        float delta = 0;

        drv_adc_config_t config_vbat;
        config_vbat.channel = ADC_CH_VBAT;
        config_vbat.mode = ADC_MODE_SINGLE;
        config_vbat.gain = ADC_GAIN_1_3;
        config_vbat.sum_num = ADC_SUM_NUM_2;
        config_vbat.sampling_cycles = ADC_SAMPLING_CYCLES_128;

        drv_adc_init(&config_vbat);
        drv_adc_read(ADC_CH_VBAT, &vbat_out[0], GPADC_CALIB_VBG_NUM);
        drv_adc_control(ADC_CONTROL_DISABLE_CLOCK, NULL);

        for (int i = 0; i < GPADC_CALIB_VBG_NUM; i++) {
            vbat_out_sum += vbat_out[i];
        }
        vbat_out_sum = vbat_out_sum/GPADC_CALIB_VBG_NUM;

        drv_adc_config_t config;
        config.channel = 1<<2U;
        config.mode = ADC_MODE_SINGLE;
        config.gain = ADC_GAIN_1_3;
        config.sum_num = ADC_SUM_NUM_256;
        config.sampling_cycles = ADC_SAMPLING_CYCLES_0;

        drv_adc_control(ADC_CONTROL_ENABLE_CLOCK, NULL);
        drv_adc_config(&config);
        REGW(&OM_GPADC->CALI_CFG, MASK_1REG(GPADC_AUTO_COMPEN, 0));

        uint8_t pmu_trim_store = REGR(&OM_GPADC->ADC_CFG0, MASK_POS(GPADC_PMU_TRIM_CTRL));

        gain_error = adc_env.ft_calib_value.data[ADC_GAIN_1_3].gain_error;

        for (int j = 0; j < 5; j++) {
            REGW(&OM_GPADC->ADC_CFG0, MASK_1REG(GPADC_PMU_TRIM_CTRL, 1));
            drv_adc_get_channel_data(1<<2U, &vbg_code_trim_1[0], GPADC_CALIB_VBG_NUM);

            REGW(&OM_GPADC->ADC_CFG0, MASK_1REG(GPADC_PMU_TRIM_CTRL, 3));
            drv_adc_get_channel_data(1<<2U, &vbg_code_trim_3[0], GPADC_CALIB_VBG_NUM);

            for (int i = 0; i < GPADC_CALIB_VBG_NUM; i++) {
                vbg_code_trim_1_new += vbg_code_trim_1[i];
                vbg_code_trim_3_new += vbg_code_trim_3[i];
            }

            float param = (float)(3300 - vbat_out_sum)/100.0f;
            if (adc_env.use_flash_ex_3) {
                float step_trim_1 = (float)(adc_env.ft_calib_ex_2_value.vbg_code_trim_1_vbat_3p3v - adc_env.ft_calib_ex_3_value.vbg_code_trim_1_vbat_2p3v) / 10.0f;
                float step_trim_3 = (float)(adc_env.ft_calib_ex_2_value.vbg_code_trim_3_vbat_3p3v - adc_env.ft_calib_ex_3_value.vbg_code_trim_3_vbat_2p3v) / 10.0f;
                vbg_code_trim_1_new = vbg_code_trim_1_new / GPADC_CALIB_VBG_NUM + step_trim_1 * param;
                vbg_code_trim_3_new = vbg_code_trim_3_new / GPADC_CALIB_VBG_NUM + step_trim_3 * param;
            } else {
                vbg_code_trim_1_new = vbg_code_trim_1_new / GPADC_CALIB_VBG_NUM + (-2.3f) * param;
                vbg_code_trim_3_new = vbg_code_trim_3_new / GPADC_CALIB_VBG_NUM + 10.8f * param;
            }

            k = (float)(vbg_code_trim_3_new - vbg_code_trim_1_new) /
                (adc_env.ft_calib_ex_2_value.vbg_code_trim_3_vbat_3p3v - adc_env.ft_calib_ex_2_value.vbg_code_trim_1_vbat_3p3v);

            delta = 8192.0f / gain_error * (k - 1);

            if (delta > 0.002f || delta < -0.002f) {
                out_of_range++;
            }

            k_sum += k;
            vbg_code_trim_1_new = 0;
            vbg_code_trim_3_new = 0;
        }

        if (out_of_range > 3) {
            k_sum = k_sum / 5;
            gain_error_new = gain_error / k_sum;
        } else {
            gain_error_new = gain_error;
        }

        REGW(&OM_GPADC->ADC_CFG0, MASK_1REG(GPADC_PMU_TRIM_CTRL, pmu_trim_store));

        drv_adc_control(ADC_CONTROL_DISABLE_CLOCK, NULL);

        adc_env.ft_calib_value.data[ADC_GAIN_1_3].gain_error = gain_error_new;
    }
}

/**
 *******************************************************************************
 * @brief set ADC calibration parameters
 *
 * @param[in] config  calibration parameters
 *
 * @return none
 *******************************************************************************
 **/
static void drv_adc_set_calibrate_param(drv_adc_cpft_calib_t *config)
{
    if (config->flash != NULL) {
        adc_env.use_efuse = false;
        memcpy(&adc_env.ft_calib_value, config->flash, sizeof(drv_adc_flash_calib_t));
        if (config->flash_ex != NULL) {
            adc_env.use_flash_ex = true;
            memcpy(&adc_env.ft_calib_ex_value, config->flash_ex, sizeof(drv_adc_flash_calib_ex_t));
        } else {
            adc_env.use_flash_ex = false;
        }
        if (config->flash_ex_2 != NULL) {
            adc_env.use_flash_ex_2 = true;
            memcpy(&adc_env.ft_calib_ex_2_value, config->flash_ex_2, sizeof(drv_adc_flash_calib_ex_2_t));
        } else {
            adc_env.use_flash_ex_2 = false;
        }
        if (config->flash_ex_3 != NULL) {
            adc_env.use_flash_ex_3 = true;
            memcpy(&adc_env.ft_calib_ex_3_value, config->flash_ex_3, sizeof(drv_adc_flash_calib_ex_3_t));
        } else {
            adc_env.use_flash_ex_3 = false;
        }
        drv_adc_set_new_calibrate_param();
    } else if (config->efuse != NULL) {
        adc_env.use_efuse = true;
        memcpy(&adc_env.cp_calib_value, config->efuse, sizeof(drv_adc_calib_t));
    }
}

#ifdef ADC_CALIB_EN
/**
 *******************************************************************************
 * @brief calibrate common function
 *
 * @param[in] channel           adc channel
 * @param[in] gain              adc gain
 * @param[in] compen            If set to 0, the adc will do nothing but transparent transmission
 * @param[in] mode              adc mode
 * 
 * @return adc_out
 *******************************************************************************
 **/
static uint16_t drv_adc_calib_common(drv_adc_channel_t channel, drv_adc_gain_t gain, bool compen, drv_adc_mode_t mode)
{
    uint16_t ret_raw[GPADC_CALIB_VBG_NUM];
    uint32_t sum = 0;

    drv_adc_config_t config;
    config.channel = channel;
    config.mode = mode;
    config.gain = gain;
    config.sum_num = ADC_SUM_NUM_256;
    config.sampling_cycles = ADC_SAMPLING_CYCLES_128;

    drv_adc_config(&config);

    REGW(&OM_GPADC->CALI_CFG, MASK_1REG(GPADC_AUTO_COMPEN, compen));

    if (1<<2 == channel) {
        REGW(&OM_GPADC->ADC_CFG1, MASK_1REG(GPADC_SMP_TIME, ADC_SAMPLING_CYCLES_0));
        drv_adc_get_channel_data(channel, (int16_t *)(&ret_raw[0]), GPADC_CALIB_VBG_NUM);
        for (int i = 0; i < GPADC_CALIB_VBG_NUM; i++) {
            sum += ret_raw[i];
        }

        return (uint16_t)(sum / GPADC_CALIB_VBG_NUM);
    } else {
        drv_adc_get_channel_data(channel, (int16_t *)(&ret_raw[0]), GPADC_CALIB_COMMON_NUM);
        for (int i = 0; i < GPADC_CALIB_COMMON_NUM; i++) {
            sum += ret_raw[i];
        }

        return (uint16_t)(sum / GPADC_CALIB_COMMON_NUM);
    }
}

/**
 *******************************************************************************
 * @brief calibrate vosTemp
 *
 * @param[in] gain              adc gain
 * @param[in] gainErr_set       gainErr
 * @param[in] vos_set           vos
 *
 * @return vosTemp
 *******************************************************************************
 **/
static float drv_adc_calib_vos_temp(drv_adc_gain_t gain, uint16_t gainErr_set, uint16_t vos_set)
{
    uint16_t vt = 0;
    float vosTemp = 0;

    /* config GPADC_GAIN_ERR and GPADC_VOS */
    REGW(&OM_GPADC->CALI_CFG, MASK_2REG(GPADC_GAIN_ERR, (gainErr_set & 0x3FFF), GPADC_VOS, vos_set));

    /* start adc */
    drv_adc_calib_common(ADC_CH_TEMPERATURE, gain, 1, ADC_MODE_SINGLE);

    /* We can get the calibrate value from responding vector */
    vt = REGR(&OM_GPADC->ADC_DBG1, MASK_POS(GPADC_SMP_VIN));

    OM_ADC_LOG_DEBUG("vt[%d]=%f\r\n", gain, (int16_t)vt / 2048.0f);

    vosTemp = ((int16_t)vt * 1000) / 2.959f / 2048 - (adc_env.calib_temper + 273);

    return vosTemp;
}

/**
 *******************************************************************************
 * @brief calibrate gainErr_vbat
 *
 * @param[in] gain              adc gain
 * @param[in] gainErr_set       gainErr
 * @param[in] vos_set           vos
 *
 * @return gainErr_vbat
 *******************************************************************************
 **/
static float drv_adc_calib_vbat(drv_adc_gain_t gain, uint16_t gainErr_set, uint16_t vos_set)
{
    uint16_t vbat_raw;
    float gainError_vbat;

    /* config GPADC_GAIN_ERR and GPADC_VOS */
    REGW(&OM_GPADC->CALI_CFG, MASK_2REG(GPADC_GAIN_ERR, (gainErr_set & 0x3FFF), GPADC_VOS, vos_set));

    /* start adc */
    vbat_raw = drv_adc_calib_common(ADC_CH_VBAT, gain, 1, ADC_MODE_SINGLE);

    /* calculate gain error vbat*/
    gainError_vbat = 1.1f / (vbat_raw / 2048.0f);

    return gainError_vbat;
}

/**
 *******************************************************************************
 * @brief calibrate temperature
 *
 * @param[in] gain              adc gain
 * @param[in] pmu_trim          adc pmu trim
 *
 * @return vbg_code
 *******************************************************************************
 **/
static uint16_t drv_adc_calib_temprature(drv_adc_gain_t gain, uint8_t pmu_trim)
{
    uint16_t vbg_raw;

    /* store PMU_TRIM_CTRL */
    uint8_t pmu_trim_store = REGR(&OM_GPADC->ADC_CFG0, MASK_POS(GPADC_PMU_TRIM_CTRL));

    /* config PMU_TRIM_CTRL */
    REGW(&OM_GPADC->ADC_CFG0, MASK_1REG(GPADC_PMU_TRIM_CTRL, pmu_trim));

    /* start adc */
    vbg_raw = drv_adc_calib_common(1<<2U, gain, 0, ADC_MODE_SINGLE);

    /* restore PMU_TRIM_CTRL */
    REGW(&OM_GPADC->ADC_CFG0, MASK_1REG(GPADC_PMU_TRIM_CTRL, pmu_trim_store));

    return vbg_raw;
}
#endif

/**
 *******************************************************************************
 * @brief store adc register
 *
 * @return none
 *******************************************************************************
 **/
static void drv_adc_register_store(void)
{
    /* store ADC_CFG0 config */
    adc_env.adc_cfg0 = OM_GPADC->ADC_CFG0;

    /* store ADC_CFG1 config */
    adc_env.adc_cfg1 = OM_GPADC->ADC_CFG1;

    /* store ADC_CFG2 config */
    adc_env.adc_cfg2 = OM_GPADC->ADC_CFG2;

    /* store AUTO_COMPEN config */
    adc_env.cali_cfg = OM_GPADC->CALI_CFG;
}

/**
 *******************************************************************************
 * @brief restore adc register
 *
 * @return none
 *******************************************************************************
 **/
static void drv_adc_register_restore(void)
{
    /* restore ADC_CFG0 config */
    OM_GPADC->ADC_CFG0 = adc_env.adc_cfg0;

    /* restore ADC_CFG1 config */
    OM_GPADC->ADC_CFG1 = adc_env.adc_cfg1;

    /* restore ADC_CFG2 config */
    OM_GPADC->ADC_CFG2 = adc_env.adc_cfg2;

    /* restore AUTO_COMPEN config */
    OM_GPADC->CALI_CFG = adc_env.cali_cfg;
}

/**
 *******************************************************************************
 * @brief adc temperature read
 *
 * @return temperature
 *******************************************************************************
 **/
static int drv_adc_temperature_read(void)
{
    int16_t ret = 0;

    if (0 == REGR(&OM_GPADC->ADC_CFG0, MASK_POS(GPADC_RST))) {
        adc_env.register_store = 1;
        drv_adc_register_store();
    }

    drv_adc_config_t config;
    config.channel = ADC_CH_TEMPERATURE;
    config.mode = ADC_MODE_SINGLE;
    config.gain = ADC_GAIN_1_3;
    config.sum_num = ADC_SUM_NUM_2;
    config.sampling_cycles = ADC_SAMPLING_CYCLES_128;

    drv_adc_init(&config);

    drv_adc_read(ADC_CH_TEMPERATURE, &ret, 1);

    if (1 == adc_env.register_store) {
        adc_env.register_store = 0;
        drv_adc_register_restore();
    } else {
        drv_adc_control(ADC_CONTROL_DISABLE_CLOCK, NULL);
    }

    return (ret / 10);
}

/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief ADC interrupt service routine
 *
 * @return None
 *******************************************************************************
 **/
void drv_adc_isr(void)
{
    uint32_t intr_mask = OM_GPADC->INTR_MSK;
    OM_GPADC->INTR_MSK = 0;

    uint32_t src = OM_GPADC->INTR & GPADC_INTR_ALL_MASK;
    OM_GPADC->INTR = src;

    if (adc_env.rx_cnt < adc_env.rx_num) {
        for (uint8_t i = 0; i < GPADC_CH_MAX; i++) {
            if ((src & (1 << i)) && (adc_env.busy & (1 << i))) {
                adc_env.rx_buf[adc_env.rx_cnt] = REGR(((uint32_t *)&OM_GPADC->CH_0_DATA) + i, MASK_POS(GPADC_DATA_LOW));
                adc_env.rx_cnt++;
            }
        }
    }

    if (adc_env.rx_cnt == adc_env.rx_num) {
        OM_GPADC->INTR_MSK &= ~adc_env.busy;
        NVIC_DisableIRQ(ADC_IRQn);

        /* power-down ADC */
        REGW(&OM_GPADC->ADC_CFG0, MASK_1REG(GPADC_PD, 1));

        for (int i = 0; i < adc_env.rx_num; i++) {
            adc_env.rx_buf[i] = drv_adc_convert_channel_data((drv_adc_channel_t)adc_env.busy, (uint16_t)(adc_env.rx_buf[i]));
        }

        adc_env.busy = 0;
        adc_env.isr_cb(OM_GPADC, DRV_EVENT_COMMON_READ_COMPLETED, (void *)adc_env.rx_buf, (void *)((uint32_t)adc_env.rx_num));
    } else {
        OM_GPADC->INTR_MSK = intr_mask;
    }
}

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
om_error_t drv_adc_init(const drv_adc_config_t *config)
{
    if (((ADC_CH_TEMPERATURE & config->channel) && (ADC_CH_VBAT & config->channel)) ||
        ((ADC_CH_TEMPERATURE & config->channel) && (config->channel > ADC_CH_GPIO2))) {
        return OM_ERROR_PARAMETER;
    }

    if (ADC_MODE_DIFF == config->mode && (ADC_CH_TEMPERATURE == config->channel || ADC_CH_VBAT == config->channel)) {
        return OM_ERROR_PARAMETER;
    }

    drv_adc_control(ADC_CONTROL_ENABLE_CLOCK, NULL);

    drv_adc_config(config);

    drv_adc_set_calib2reg(config->gain);

    adc_env.mode = config->mode;
    adc_env.gain = config->gain;
    adc_env.busy = 0;

    return OM_ERROR_OK;
}

/**
 *******************************************************************************
 * @brief Register isr callback for transmit/receive by interrupt & gpdma mode
 *
 * @param[in] cb       Pointer to callback
 *
 * @return None
 *******************************************************************************
 */
void drv_adc_register_isr_callback(drv_isr_callback_t cb)
{
    adc_env.isr_cb = cb;
}

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
om_error_t drv_adc_read(uint16_t channel, int16_t *data, uint16_t num)
{
    if (adc_env.busy) {
        return OM_ERROR_BUSY;
    }

    adc_env.busy = channel;

    drv_adc_get_channel_data(channel, data, num);

    int i = 0;
    while (i < num) {
        for (uint8_t j = 0; j < GPADC_CH_MAX; j++) {
            if ((channel >> j) & 0x1) {
                data[i%num] = drv_adc_convert_channel_data((drv_adc_channel_t)(0x1 << j), (uint16_t)(data[i%num]));
                i++;
            }
        }
    }

    adc_env.busy = 0;

    return OM_ERROR_OK;
}

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
om_error_t drv_adc_read_int(uint16_t channel, int16_t *data, uint16_t num)
{
    if (adc_env.busy) {
        return OM_ERROR_BUSY;
    }

    /* clear interrupt */
    OM_GPADC->INTR = GPADC_INTR_ALL_MASK;

    /* set INTR_MSK */
    OM_GPADC->INTR_MSK = channel;

    adc_env.busy   = channel;
    adc_env.rx_cnt = 0;
    adc_env.rx_buf = data;
    adc_env.rx_num = num;

    NVIC_ClearPendingIRQ(ADC_IRQn);
    NVIC_SetPriority(ADC_IRQn, RTE_GPADC_IRQ_PRIORITY);
    NVIC_EnableIRQ(ADC_IRQn);

    /* power-on ADC */
    REGW(&OM_GPADC->ADC_CFG0, MASK_1REG(GPADC_PD, 0));

    return OM_ERROR_OK;
}

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
om_error_t drv_adc_dma_channel_allocate(void)
{
    if (adc_env.dma_chan >= DMA_NUMBER_OF_CHANNELS) {
        adc_env.dma_chan = drv_dma_channel_allocate();
        if (adc_env.dma_chan >= DMA_NUMBER_OF_CHANNELS) {
            return OM_ERROR_RESOURCES;
        }
    }

    return OM_ERROR_OK;
}

/**
 *******************************************************************************
 * @brief Release dma channel for adc
 *
 * @return status:
 *    - OM_ERROR_OK:         Release ok
 *    - others:              No
 *******************************************************************************
 */
om_error_t drv_adc_dma_channel_release(void)
{
    drv_dma_channel_release(adc_env.dma_chan);
    adc_env.dma_chan = DMA_NUMBER_OF_CHANNELS;

    return OM_ERROR_OK;
}

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
om_error_t drv_adc_read_dma(uint16_t channel, int16_t *data, uint16_t num)
{
    if (adc_env.busy) {
        return OM_ERROR_BUSY;
    }

    dma_config_t    dma_config;
    om_error_t      error;

    /* ADC_CFG1 */
    REGW(&OM_GPADC->ADC_CFG1, MASK_4REG(GPADC_AUTO_PD1, 0x1,
                                        GPADC_AUTO_PD2, 0x1,
                                        GPADC_TRIG_RES, 0x1,
                                        GPADC_DIGI_CLK_FREQ, 0x1F));

    adc_env.busy = channel;
    adc_env.rx_cnt = 0;
    adc_env.rx_buf = data;
    adc_env.rx_num = num;

    /* ADC DMA config */
    dma_config.channel_ctrl  = DMA_SET_CTRL(DMA_ADDR_CTRL_FIXED, DMA_ADDR_CTRL_INC,
                                            DMA_TRANS_WIDTH_2B,  DMA_TRANS_WIDTH_2B,
                                            DMA_BURST_SIZE_1T,   DMA_PRIORITY_LOW);
    dma_config.src_id        = DMA_ID_GPADC;
    dma_config.dst_id        = DMA_ID_MEM;
    dma_config.event_cb      = adc_dma_event_cb;
    dma_config.cb_param      = NULL;
    dma_config.chain_trans   = NULL;
    dma_config.chain_trans_num = 0;

    error = drv_dma_channel_config(adc_env.dma_chan, &dma_config);
    if (error) {
        return error;
    }

    /* Enable ADC DMA */
    REGW1(&OM_GPADC->ADC_CFG1, GPADC_DMA_EN_MASK);

    /* power-on ADC */
    REGW(&OM_GPADC->ADC_CFG0, MASK_1REG(GPADC_PD, 0));

    error = drv_dma_channel_enable(adc_env.dma_chan, (uint32_t)data, (uint32_t)&OM_GPADC->CH_DMA_DATA, 2 * num);
    if (error) {
        return error;
    }

    return OM_ERROR_OK;
}
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
void *drv_adc_control(drv_adc_control_t control, void *argu)
{
    uint32_t ret = (uint32_t)OM_ERROR_OK;
    OM_CRITICAL_BEGIN();
    switch (control) {
        case ADC_CONTROL_ENABLE_CLOCK:
            drv_pmu_ana_enable(true, PMU_ANA_ADC);
            REGW(&OM_GPADC->ADC_CFG0, MASK_1REG(GPADC_RST, 0));
            break;
        case ADC_CONTROL_DISABLE_CLOCK:
            REGW(&OM_GPADC->ADC_CFG0, MASK_1REG(GPADC_RST, 1));
            drv_pmu_ana_enable(false, PMU_ANA_ADC);
            break;
        case ADC_CONTROL_SET_PARAM:
            if (argu == NULL) {
                ret = (uint32_t)OM_ERROR_PARAMETER;
                break;
            }
            drv_adc_set_calibrate_param((drv_adc_cpft_calib_t *)argu);
            break;
        case ADC_CONTROL_IS_BUSY:
            if (adc_env.busy)
                ret = true;
            else
                ret = false;
            break;
        case ADC_CONTROL_READ_TEMPERATURE:
            ret = drv_adc_temperature_read();
            break;
        case ADC_CONTROL_SET_CALIB_TEMPER:
            adc_env.calib_temper = *(float *)argu;
            break;
        case ADC_CONTROL_TEMPERATURE_COMPEN:
            ret = drv_adc_temperature_compen((drv_adc_temperature_compen_t *)argu);
            break;
        default:
            break;
    }
    OM_CRITICAL_END();
    return (void *)ret;
}

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
om_error_t drv_adc_calibrate(drv_adc_calib_t *pvalue, drv_adc_flash_calib_ex_t *pvalue_vbat, drv_adc_flash_calib_ex_2_t *pvalue_temp,
                             drv_adc_flash_calib_ex_3_t *pvalue_temp_2, drv_adc_gain_t gain, drv_adc_calib_sel_t calib_sel)
{
    if (adc_env.busy) {
        return OM_ERROR_BUSY;
    }

    om_error_t error = OM_ERROR_OK;
    uint16_t ret_raw = 0;

    drv_adc_control(ADC_CONTROL_ENABLE_CLOCK, NULL);
    adc_env.busy = ADC_CH_GPIO2 | ADC_CH_GPIO3;

    if (ADC_CALIB_FT_SINGLE == calib_sel) {
        do {
            if (pvalue != NULL) {
                /* get adc_out_real_1[15:0] */
                ret_raw = drv_adc_calib_common(ADC_CH_GPIO2, gain, 0, ADC_MODE_SINGLE);
                adc_para.adcOut_1 = ret_raw / 16.0f;
                OM_ADC_LOG_DEBUG("adc_out_real_1[%d]=%f\r\n", gain, adc_para.adcOut_1);
                if ((adc_para.adcOut_1 > GPADC_OUT_UPPER_BOUND(GPADC_SINGLE_OUT_1_IDEAL[gain])) ||
                    (adc_para.adcOut_1 < GPADC_OUT_LOW_BOUND(GPADC_SINGLE_OUT_1_IDEAL[gain]))) {
                    error = OM_ERROR_OUT_OF_RANGE;
                    break;
                }

                /* get adc_out_real_2[15:0] */
                ret_raw = drv_adc_calib_common(ADC_CH_GPIO3, gain, 0, ADC_MODE_SINGLE);
                adc_para.adcOut_2 = ret_raw / 16.0f;
                OM_ADC_LOG_DEBUG("adc_out_real_2[%d]=%f\r\n", gain, adc_para.adcOut_2);
                if ((adc_para.adcOut_2 > GPADC_OUT_UPPER_BOUND(GPADC_SINGLE_OUT_2_IDEAL[gain])) ||
                    (adc_para.adcOut_2 < GPADC_OUT_LOW_BOUND(GPADC_SINGLE_OUT_2_IDEAL[gain]))) {
                    error = OM_ERROR_OUT_OF_RANGE;
                    break;
                }

                /* calib gain err[13:0] */
                adc_para.gainErr_cal = (adc_para.adcOut_2 - adc_para.adcOut_1) /
                                            GPADC_OUT_IDEAL_DIFF(GPADC_SINGLE_OUT_2_IDEAL[gain], GPADC_SINGLE_OUT_1_IDEAL[gain]);
                adc_para.gainErr_set = (uint16_t)(8192.0f / adc_para.gainErr_cal);
                OM_ADC_LOG_DEBUG("gainErr_cal[%d] = %f, gainErr_set[%d] = %x\r\n", gain, adc_para.gainErr_cal, gain, adc_para.gainErr_set);

                /* calib vos[15:0] */
                adc_para.vos_cal = adc_para.adcOut_2 - adc_para.gainErr_cal * GPADC_SINGLE_OUT_2_IDEAL[gain];
                if (adc_para.vos_cal < 0) {
                    adc_para.vos_set = ~(uint16_t)(0 - (adc_para.vos_cal * 16)) + 1;
                } else {
                    adc_para.vos_set = (uint16_t)(adc_para.vos_cal * 16);
                }
                OM_ADC_LOG_DEBUG("vos_cal[%d] = %f, vos_set[%d]: %x\r\n", gain, adc_para.vos_cal, gain, adc_para.vos_set);

                /* calib vos_temp[13:0] */
                adc_para.vosTemp_cal = drv_adc_calib_vos_temp(gain, adc_para.gainErr_set, adc_para.vos_set);
                if (adc_para.vosTemp_cal < 0) {
                    adc_para.vosTemp_set = ~(uint16_t)(0 - adc_para.vosTemp_cal * 64) + 1;
                } else {
                    adc_para.vosTemp_set = (uint16_t)(adc_para.vosTemp_cal * 64);
                }
                OM_ADC_LOG_DEBUG("vosTemp_cal[%d] = %f, vosTemp_set[%d] = %x\r\n", gain, adc_para.vosTemp_cal, gain, adc_para.vosTemp_set);

                pvalue->gain_error = adc_para.gainErr_set;
                pvalue->vos = adc_para.vos_set;
                pvalue->vos_temp = adc_para.vosTemp_set;

                if (gain == ADC_GAIN_1_3) {
                    adc_para.vosTemp_cal_gain_1_3 = adc_para.vosTemp_cal;
                    adc_para.gainErr_set_gain_1_3 = adc_para.gainErr_set;
                    adc_para.vos_set_gain_1_3 = adc_para.vos_set;
                }
            }

            /* calib vbat */
            if (pvalue_vbat != NULL) {
                adc_para.gainErr_vbat_cal = drv_adc_calib_vbat(gain, adc_para.gainErr_set, adc_para.vos_set);
                if (adc_para.gainErr_vbat_cal < 0) {
                    adc_para.gainErr_vbat_set = ~(uint16_t)(0 - adc_para.gainErr_vbat_cal * 32768) + 1;
                } else {
                    adc_para.gainErr_vbat_set = (uint16_t)(adc_para.gainErr_vbat_cal * 32768);
                }
                OM_ADC_LOG_DEBUG("gainErr_vbat_cal[%d] = %f, gainErr_vbat_set[%d] = %x\r\n", gain, adc_para.gainErr_vbat_cal, gain, adc_para.gainErr_vbat_set);

                pvalue_vbat->gain_error_vbat = adc_para.gainErr_vbat_set;
            }

            /* calib tempereture */
            if (pvalue_temp != NULL) {
                adc_para.vbg_code_trim_1 = drv_adc_calib_temprature(gain, 1);
                adc_para.vbg_code_trim_3 = drv_adc_calib_temprature(gain, 3);
                OM_ADC_LOG_DEBUG("vbg_code_trim_1_vbat_3p3v = %d, vbg_code_trim_3_vbat_3p3v = %d\r\n", adc_para.vbg_code_trim_1, adc_para.vbg_code_trim_3);

                pvalue_temp->vbg_code_trim_1_vbat_3p3v = adc_para.vbg_code_trim_1;
                pvalue_temp->vbg_code_trim_3_vbat_3p3v = adc_para.vbg_code_trim_3;
            }
            
            if (pvalue_temp_2 != NULL) {
                adc_para.vbg_code_trim_1 = drv_adc_calib_temprature(gain, 1);
                adc_para.vbg_code_trim_3 = drv_adc_calib_temprature(gain, 3);
                OM_ADC_LOG_DEBUG("vbg_code_trim_1_vbat_2p3v = %d, vbg_code_trim_3_vbat_2p3v = %d\r\n", adc_para.vbg_code_trim_1, adc_para.vbg_code_trim_3);

                pvalue_temp_2->vbg_code_trim_1_vbat_2p3v = adc_para.vbg_code_trim_1;
                pvalue_temp_2->vbg_code_trim_3_vbat_2p3v = adc_para.vbg_code_trim_3;

                pvalue_temp_2->temperature_vbat_3p3v = (int16_t)((adc_para.vosTemp_cal_gain_1_3 + adc_env.calib_temper) * 10);
                pvalue_temp_2->temperature_vbat_2p3v = (int16_t)((drv_adc_calib_vos_temp(gain, adc_para.gainErr_set_gain_1_3, adc_para.vos_set_gain_1_3) + adc_env.calib_temper) * 10);
            }
        } while(0);
    } else if (ADC_CALIB_CP_1 == calib_sel) {
        do {
            /* get adc_out_real_1[15:0] */
            ret_raw = drv_adc_calib_common(ADC_CH_GPIO3, gain, 0, ADC_MODE_SINGLE);
            adc_para.adcOut_1 = ret_raw / 16.0f;
            OM_ADC_LOG_DEBUG("adc_out_real_1[%d]=%f\r\n", gain, adc_para.adcOut_1);
            if ((adc_para.adcOut_1 > GPADC_OUT_UPPER_BOUND(GPADC_SINGLE_OUT_1_IDEAL[gain])) ||
                (adc_para.adcOut_1 < GPADC_OUT_LOW_BOUND(GPADC_SINGLE_OUT_1_IDEAL[gain]))) {
                error = OM_ERROR_OUT_OF_RANGE;
                break;
            }
        } while(0);
    } else if (ADC_CALIB_CP_2 == calib_sel) {
        do {
            /* get adc_out_real_2[15:0] */
            ret_raw = drv_adc_calib_common(ADC_CH_GPIO3, gain, 0, ADC_MODE_SINGLE);
            adc_para.adcOut_2 = ret_raw / 16.0f;
            OM_ADC_LOG_DEBUG("adc_out_real_2[%d]=%f\r\n", gain, adc_para.adcOut_2);
            if ((adc_para.adcOut_2 > GPADC_OUT_UPPER_BOUND(GPADC_SINGLE_OUT_2_IDEAL[gain])) ||
                (adc_para.adcOut_2 < GPADC_OUT_LOW_BOUND(GPADC_SINGLE_OUT_2_IDEAL[gain]))) {
                error = OM_ERROR_OUT_OF_RANGE;
                break;
            }

            /* calib gain err[13:0] */
            adc_para.gainErr_cal = (adc_para.adcOut_2 - adc_para.adcOut_1) /
                                        GPADC_OUT_IDEAL_DIFF(GPADC_SINGLE_OUT_2_IDEAL[gain], GPADC_SINGLE_OUT_1_IDEAL[gain]);
            adc_para.gainErr_set = (uint16_t)(8192.0f / adc_para.gainErr_cal);
            OM_ADC_LOG_DEBUG("gainErr_cal[%d] = %f, gainErr_set[%d] = %x\r\n", gain, adc_para.gainErr_cal, gain, adc_para.gainErr_set);

            /* calib vos[15:0] */
            adc_para.vos_cal = adc_para.adcOut_2 - adc_para.gainErr_cal * GPADC_SINGLE_OUT_2_IDEAL[gain];
            if (adc_para.vos_cal < 0) {
                adc_para.vos_set = ~(uint16_t)(0 - (adc_para.vos_cal * 16)) + 1;
            } else {
                adc_para.vos_set = (uint16_t)(adc_para.vos_cal * 16);
            }
            OM_ADC_LOG_DEBUG("vos_cal[%d] = %f, vos_set[%d]: %x\r\n", gain, adc_para.vos_cal, gain, adc_para.vos_set);

            /* calib vos_temp[13:0] */
            adc_para.vosTemp_cal = drv_adc_calib_vos_temp(gain, adc_para.gainErr_set, adc_para.vos_set);
            if (adc_para.vosTemp_cal < 0) {
                adc_para.vosTemp_set = ~(uint16_t)(0 - adc_para.vosTemp_cal * 64) + 1;
            } else {
                adc_para.vosTemp_set = (uint16_t)(adc_para.vosTemp_cal * 64);
            }
            OM_ADC_LOG_DEBUG("vosTemp_cal[%d] = %f, vosTemp_set[%d] = %x\r\n", gain, adc_para.vosTemp_cal, gain, adc_para.vosTemp_set);

            if (pvalue != NULL) {
                pvalue->gain_error = adc_para.gainErr_set;
                pvalue->vos = adc_para.vos_set;
                pvalue->vos_temp = adc_para.vosTemp_set;
            }

        } while(0);
    } else {
        error = OM_ERROR_PARAMETER;
    }

    drv_adc_control(ADC_CONTROL_DISABLE_CLOCK, NULL);
    adc_env.busy = 0;
    return error;
}
#endif

#else

// default drv_adc_control() function for CPFT load when RTE_GPADC=0
void *drv_adc_control(int control, void *argu)
{
    return 0;
}

#endif /* RTE_GPADC */

/** @} */

