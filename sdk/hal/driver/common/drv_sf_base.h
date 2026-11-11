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
 * @file     drv_sf_base.h
 * @brief    The base driver of sflash
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup SF_BASE SFLASH BASE
 * @ingroup  DRIVER
 * @brief    The base driver of SFLASH
 * @details  The base driver of SFLASH
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __SF_BASE_H__
#define __SF_BASE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "om_driver.h"

/*********************************************************************
 * MACROS
 */
/// module number (OM_SF)
#define DRV_SFB_MODULE_NUM         1
/// DMA data max length
#define DRV_SFB_DMA_DATA_LEN_MAX   0xFFFF0 /* IP limitation & aligned to 20-byte */

/// CS number
#define DRV_SFB_CS_NUM             2

/// @cond
#if (RTE_SF_BASE_USING_ROM_SYMBOL)
#define drv_sfb_isr                         rom_drv_sfb_isr
#define drv_sfb_read_dma                    rom_drv_sfb_read_dma
#define drv_sfb_read_dma_ex                 rom_drv_sfb_read_dma_ex
#define drv_sfb_write_dma                   rom_drv_sfb_write_dma
#define drv_sfb_write_dma_ex                rom_drv_sfb_write_dma_ex
#define drv_sfb_read_nodma                  rom_drv_sfb_read_nodma
#define drv_sfb_write_nodma                 rom_drv_sfb_write_nodma
#define drv_sfb_open                        rom_drv_sfb_open
#define drv_sfb_close                       rom_drv_sfb_close
#define drv_sfb_dma_done_event_register     rom_drv_sfb_dma_done_event_register
#define drv_sfb_dma_done_event_get          rom_drv_sfb_dma_done_event_get
#define drv_sfb_config                      rom_drv_sfb_config
#define drv_sfb_lcd2lane_enable             rom_drv_sfb_lcd2lane_enable
#define drv_sfb_regs_get                    rom_drv_sfb_regs_get
#define drv_sfb_regs_set                    rom_drv_sfb_regs_set
#define drv_sfb_critical_object_set         rom_drv_sfb_critical_object_set
#define drv_sfb_critical_object_get         rom_drv_sfb_critical_object_get
#define drv_sfb_critical_cs_set             rom_drv_sfb_critical_cs_set
#define drv_sfb_critical_cs_get             rom_drv_sfb_critical_cs_get
#define drv_sfb_enable                      rom_drv_sfb_enable
#endif
/// @endcond

/*********************************************************************
 * TYPEDEFS
 */

/**
 * @brief  event callback
 *
 * @param[in] sf  sf object
 **/
typedef void (*drv_sfb_callback_t)(OM_SF_Type *sf);

/// SPI transmite mode
typedef enum
{
    /// Mode 0: CPOL=0 CPHA=0
    DRV_SFB_SPI_MODE_0 = 0,
    /// Mode 1: CPOL=0 CPHA=1
    DRV_SFB_SPI_MODE_1 = 1,
    /// Mode 2: CPOL=1 CPHA=0
    DRV_SFB_SPI_MODE_2 = 2,
    /// Mode 3: CPOL=1 CPHA=1
    DRV_SFB_SPI_MODE_3 = 3,
}drv_sfb_spi_transmode_t;

/// SPI CS acive pol
typedef enum
{
    /// Chip select is active low
    DRV_SFB_SPI_CS_LOW_ACTIVE = 0,
    /// Chip select is active high
    DRV_SFB_SPI_CS_HIGH_ACTIVE = 1,
}drv_sfb_spi_cs_pol_t;

/// SPI keep CS type
typedef enum
{
    /// Not keep (normal mode)
    DRV_SFB_CS_NOKEEP,
    /// Keep CS begin
    DRV_SFB_CS_BEGIN,
    /// Keep CS continue
    DRV_SFB_CS_KEEP,
    /// Keep CS end
    DRV_SFB_CS_END,
}drv_sfb_keep_cs_t;

/// cmd bytes
typedef struct
{
    /// byte3
    uint8_t d3;
    /// byte2
    uint8_t d2;
    /// byte1
    uint8_t d1;
    /// byte0
    uint8_t d0;
    /// byte7
    uint8_t d7;
    /// byte6
    uint8_t d6;
    /// byte5
    uint8_t d5;
    /// byte4
    uint8_t d4;
}drv_sfb_cmd_bytes_t;

/// SFB config
typedef struct
{
    /// frequence in Hz. if less than 256, its division
    uint32_t freq_hz;
    /// Delayed Sampling
    uint8_t delay;
    /// spi transmode
    drv_sfb_spi_transmode_t transmode;
    /// spi cs active pol
    drv_sfb_spi_cs_pol_t cs_pol;
}drv_sfb_config_t;

/// SFB read and write param
typedef struct
{
    /// command
    union {
        uint32_t cmd[2];
        drv_sfb_cmd_bytes_t cm;
    };
    /// command bits
    uint8_t cmd_bits;
    /// In DMA mode:
    ///   1-line: 4bytes align
    ///   2-line: 8bytes align
    ///   4-line: 16bytes align
    /// ROM address (0x08000000) can't use DMA write
    /// NOTE: data const type will be casted to non-cast for read operation
    const void *data;
    /// data length
    uint32_t data_bytes;
}drv_sfb_rw_params_t;

/*********************************************************************
 * EXTERN VARIABLES
 */

/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief  drv sfb isr
 *
 * @param[in] sf  sf
 * @param[in] drv_sf_index  sf index
 */
void drv_sfb_isr(OM_SF_Type *sf, uint32_t drv_sf_index);

/**
 * @brief read with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] param  rw param
 **/
void drv_sfb_read_dma(OM_SF_Type *sf, uint32_t cs, drv_sfb_rw_params_t *param);

/**
 * @brief read with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] keep_cs  keep cs
 * @param[in] param  rw param
 **/
void drv_sfb_read_dma_ex(OM_SF_Type *sf, uint32_t cs, drv_sfb_keep_cs_t keep_cs, drv_sfb_rw_params_t *param);

/**
 * @brief write with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] param  rw param
 **/
__RAM_CODE
void drv_sfb_write_dma(OM_SF_Type *sf, uint32_t cs, drv_sfb_rw_params_t *param);

/**
 * @brief write with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] keep_cs  keep cs
 * @param[in] param  rw param
 **/
void drv_sfb_write_dma_ex(OM_SF_Type *sf, uint32_t cs, drv_sfb_keep_cs_t keep_cs, drv_sfb_rw_params_t *param);

/**
 * @brief read without dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] param  rw param
 **/
__RAM_CODE
void drv_sfb_read_nodma(OM_SF_Type *sf, uint32_t cs, drv_sfb_rw_params_t *param);

/**
 * @brief write without dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] param  rw param
 **/
__RAM_CODE
void drv_sfb_write_nodma(OM_SF_Type *sf, uint32_t cs, drv_sfb_rw_params_t *param);

/**
 * @brief open
 *
 * @param[in] sf  sf object
 **/
void drv_sfb_open(OM_SF_Type *sf);

/**
 * @brief close
 *
 * @param[in] sf  sf object
 **/
void drv_sfb_close(OM_SF_Type *sf);

/**
 * @brief set dma done event
 *
 * @param[in] sf  sf object
 * @param[in] callback  event callback
 **/
void drv_sfb_dma_done_event_register(OM_SF_Type *sf, drv_sfb_callback_t callback);

/**
 * @brief  sfb dma done event get
 *
 * @param[in] sf  sf
 *
 * @return callback
 **/
drv_sfb_callback_t drv_sfb_dma_done_event_get(OM_SF_Type *sf);

/**
 * @brief config
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] config  config
 **/
void drv_sfb_config(OM_SF_Type *sf, uint32_t cs, const drv_sfb_config_t *config);

/**
 * @brief  sfb lcd2lane enable
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] enable  enable
 **/
void drv_sfb_lcd2lane_enable(OM_SF_Type *sf, uint32_t cs, bool enable);

/**
 * @brief regs get
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return reg
 **/
uint32_t drv_sfb_regs_get(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief regs get
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] regs  reg
 **/
void drv_sfb_regs_set(OM_SF_Type *sf, uint32_t cs, uint32_t regs);

/**
 * @brief  sfb critical object set
 *
 * @param[in] sf  sf
 **/
void drv_sfb_critical_object_set(OM_SF_Type *sf);

/**
 * @brief  sfb critical object get
 *
 * @return obj
 **/
OM_SF_Type *drv_sfb_critical_object_get(void);

/**
 * @brief  sfb critical object set
 *
 * @param[in] cs  cs
 **/
void drv_sfb_critical_cs_set(uint32_t cs);

/**
 * @brief  sfb critical object get
 *
 * @return obj
 **/
uint32_t drv_sfb_critical_cs_get(void);

/**
 * @brief sfb enable
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
void drv_sfb_enable(OM_SF_Type *sf, uint32_t cs);

/**
 *******************************************************************************
 * @brief  sfb index
 *
 * @param[in] sf  sf object
 *
 * @return index
 *******************************************************************************
 */
__STATIC_FORCEINLINE int drv_sfb_index(OM_SF_Type *sf)
{
    if (sf==OM_SF) return 0;
    else return 1;
}

#ifdef __cplusplus
}
#endif

#endif

/** @} */

