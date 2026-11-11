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
 * @file     drv_sf.h
 * @brief    sflash driver
 * @date     30. March 2020
 * @author   OnMicro SW Team
 *
 * @defgroup SF SFLASH
 * @ingroup  DRIVER
 * @brief    SFLASH driver
 * @details  sflash driver
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 *
 * @example example_sf.c
 * This is an example of how to use sf driver
 */


#ifndef __SF_RAW_H__
#define __SF_RAW_H__

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
/// SF page shift
#define DRV_SF_PAGE_SHIFT       8
/// SF sector shift
#define DRV_SF_SECTOR_SHIFT     12
/// SF page size
#define DRV_SF_PAGE_SIZE        256
/// SF sector size
#define DRV_SF_SECTOR_SIZE      (4*1024)
/// SF half block size
#define DRV_SF_HALF_BLOCK_SIZE  (32*1024)
/// SF block size
#define DRV_SF_BLOCK_SIZE       (64*1024)
/// SF default delay
#define DRV_SF_DELAY_DEFAULT    0x02
/// SF max delay
#define DRV_SF_DELAY_MAX        0x0F
/// SF auto detect delay
#define DRV_SF_DELAY_AUTO       0xFF

/// encrypt DIS
#define DRV_SF_IFLASH_ENCRYPT_DISABLE()                     \
    OM_CRITICAL_BEGIN();                                    \
    do {                                                    \
        uint32_t iflash_encrpyt_save = OM_SF->CIPHER_CTRL;  \
        if (iflash_encrpyt_save)                            \
            OM_SF->CIPHER_CTRL = 0;
/// encrypt RESTORE
#define DRV_SF_IFLASH_ENCRYPT_RESTORE()                     \
        if (iflash_encrpyt_save)                            \
            OM_SF->CIPHER_CTRL = iflash_encrpyt_save;       \
    } while (0);                                            \
    OM_CRITICAL_END();

/// @cond
#if (RTE_SF_USING_ROM_SYMBOL)
#define drv_sf_read_sr                              rom_drv_sf_read_sr
#define drv_sf_read_sr2                             rom_drv_sf_read_sr2
#define drv_sf_read_sr_16bits                       rom_drv_sf_read_sr_16bits
#define drv_sf_wait_sr_no_busy                      rom_drv_sf_wait_sr_no_busy
#define drv_sf_write_enable                         rom_drv_sf_write_enable
#define drv_sf_write_sr                             rom_drv_sf_write_sr
#define drv_sf_write_sr_16bits                      rom_drv_sf_write_sr_16bits
#define drv_sf_write_sr_mask                        rom_drv_sf_write_sr_mask
#define drv_sf_write_sr_mask_16bits                 rom_drv_sf_write_sr_mask_16bits
#define drv_sf_quad_enable                          rom_drv_sf_quad_enable
#define drv_sf_otp_set                              rom_drv_sf_otp_set
#define drv_sf_otp_get                              rom_drv_sf_otp_get
#define drv_sf_deep_power_down_enter                rom_drv_sf_deep_power_down_enter
#define drv_sf_deep_power_down_leave                rom_drv_sf_deep_power_down_leave
#define drv_sf_unlock_all                           rom_drv_sf_unlock_all
#define drv_sf_read_id                              rom_drv_sf_read_id
#define drv_sf_read_uid_ex                          rom_drv_sf_read_uid_ex
#define drv_sf_read_uid                             rom_drv_sf_read_uid
#define drv_sf_erase_chip                           rom_drv_sf_erase_chip
#define drv_sf_erase_sector                         rom_drv_sf_erase_sector
#define drv_sf_erase_half_block                     rom_drv_sf_erase_half_block
#define drv_sf_erase_block                          rom_drv_sf_erase_block
#define drv_sf_erase_sec                            rom_drv_sf_erase_sec
#define drv_sf_erase                                rom_drv_sf_erase
#define drv_sf_write_page_nodma                     rom_drv_sf_write_page_nodma
#define drv_sf_write_page_dma                       rom_drv_sf_write_page_dma
#define drv_sf_write_page                           rom_drv_sf_write_page
#define drv_sf_write_sec                            rom_drv_sf_write_sec
#define drv_sf_write                                rom_drv_sf_write
#define drv_sf_read_normal_nodma                    rom_drv_sf_read_normal_nodma
#define drv_sf_read_normal_dma                      rom_drv_sf_read_normal_dma
#define drv_sf_read_fast_nodma                      rom_drv_sf_read_fast_nodma
#define drv_sf_read_fast_dma                        rom_drv_sf_read_fast_dma
#define drv_sf_read_fast_dual_dma                   rom_drv_sf_read_fast_dual_dma
#define drv_sf_read_fast_quad_dma                   rom_drv_sf_read_fast_quad_dma
#define drv_sf_read_sec                             rom_drv_sf_read_sec
#define drv_sf_read                                 rom_drv_sf_read
#define drv_sf_config                               rom_drv_sf_config
#define drv_sf_enable                               rom_drv_sf_enable
#define drv_sf_disable                              rom_drv_sf_disable
#define drv_sf_iflash_power_ctrl_immediate_enable   rom_drv_sf_iflash_power_ctrl_immediate_enable
#define drv_sf_iflash_auto_powerdown_in_sleep_enable rom_drv_sf_iflash_auto_powerdown_in_sleep_enable
#define drv_sf_iflash_auto_close                    rom_drv_sf_iflash_auto_close
#define drv_sf_iflash_extra_open_delay_set          rom_drv_sf_iflash_extra_open_delay_set
#define drv_sf_iflash_encrypt_enable                rom_drv_sf_iflash_encrypt_enable
#define drv_sf_detect                               rom_drv_sf_detect
#define drv_sf_status                               rom_drv_sf_status
#define drv_sf_capacity                             rom_drv_sf_capacity
#define drv_sf_id                                   rom_drv_sf_id
#endif
/// @endcond

/*********************************************************************
 * TYPEDEFS
 */
/// SF wire width
typedef enum
{
    /// 1line mode
    DRV_SF_WIDTH_1LINE = 1,
    /// 2line mode
    DRV_SF_WIDTH_2LINE = 2,
    /// 4line mode
    DRV_SF_WIDTH_4LINE = 4,
}drv_sf_width_t;

/// SF status
typedef enum
{
    /// Not detected
    DRV_SF_STATUS_NONE,
    /// Detected and sflash absent
    DRV_SF_STATUS_ABSENT,
    /// Detected and sflash present
    DRV_SF_STATUS_PRESENT,
}drv_sf_status_t;

/// SF config
typedef struct
{
    /// frequency in Hz
    uint32_t freq_hz;
    /// width @ref drv_sf_width_t
    drv_sf_width_t width;
    /// Delayed Sampling, default is 0
    uint8_t delay;
}drv_sf_config_t;

/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief read sr reg
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return sr value
 **/
uint8_t drv_sf_read_sr(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief read sr2
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return sr2 value
 **/
uint8_t drv_sf_read_sr2(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief read sr 16bits
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return sr | (sr2<<8)
 **/
uint16_t drv_sf_read_sr_16bits(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief wait sr no busy
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
void drv_sf_wait_sr_no_busy(OM_SF_Type *sf, uint32_t cs);

/**
 *******************************************************************************
 * @brief  drv sf wait sr no busy with suspend
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] irq_save_is_disabled  irq save is disabled
 *******************************************************************************
 */
void drv_sf_wait_sr_no_busy_with_suspend(OM_SF_Type *sf, uint32_t cs, uint32_t irq_save_is_disabled);

/**
 * @brief write enable
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
void drv_sf_write_enable(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief write sr
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] sr  sr
 **/
void drv_sf_write_sr(OM_SF_Type *sf, uint32_t cs, uint8_t sr);

/**
 * @brief write sr 16bits
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] sr  sr
 **/
void drv_sf_write_sr_16bits(OM_SF_Type *sf, uint32_t cs, uint16_t sr);

/**
 * @brief write sr with mask
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] mask  sr mask
 * @param[in] value  sr value
 **/
void drv_sf_write_sr_mask(OM_SF_Type *sf, uint32_t cs, uint8_t mask, uint8_t value);

/**
 * @brief write 16bits sr with mask
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] mask  sr mask
 * @param[in] value  sr value
 **/
void drv_sf_write_sr_mask_16bits(OM_SF_Type *sf, uint32_t cs, uint16_t mask, uint16_t value);

/**
 * @brief quad enable
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] enable  true or false
 **/
void drv_sf_quad_enable(OM_SF_Type *sf, uint32_t cs, bool enable);

/**
 * @brief otp set
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] lb_mask  lb mask
 **/
void drv_sf_otp_set(OM_SF_Type *sf, uint32_t cs, uint8_t lb_mask);

/**
 * @brief otp get
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return lb_mask
 **/
uint8_t drv_sf_otp_get(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief  sf lowpower enter
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 **/
void drv_sf_deep_power_down_enter(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief  sf lowpower leave
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 **/
void drv_sf_deep_power_down_leave(OM_SF_Type *sf, uint32_t cs);

/**
 *******************************************************************************
 * @brief  drv sf suspend
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *******************************************************************************
 */
void drv_sf_suspend(OM_SF_Type *sf, uint32_t cs);

/**
 *******************************************************************************
 * @brief  drv sf resume
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *******************************************************************************
 */
void drv_sf_resume(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief unlock all
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
void drv_sf_unlock_all(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief read id
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return sflash id (24bits)
 **/
uint32_t drv_sf_read_id(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief read uid
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] data  read uid buffer
 * @param[in] length  length
 **/
void drv_sf_read_uid_ex(OM_SF_Type *sf, uint32_t cs, void *data, uint32_t length);

/**
 * @brief read uid
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return sflash UID
 **/
uint32_t drv_sf_read_uid(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief erase chip
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
void drv_sf_erase_chip(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief erase sector
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 **/
void drv_sf_erase_sector(OM_SF_Type *sf, uint32_t cs, uint32_t addr);

/**
 * @brief erase block
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 **/
void drv_sf_erase_half_block(OM_SF_Type *sf, uint32_t cs, uint32_t addr);

/**
 * @brief erase block
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 **/
void drv_sf_erase_block(OM_SF_Type *sf, uint32_t cs, uint32_t addr);

/**
 * @brief erase sec
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 **/
void drv_sf_erase_sec(OM_SF_Type *sf, uint32_t cs, uint32_t addr);

/**
 * @brief sf erase
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[in] length  length
 **/
void drv_sf_erase(OM_SF_Type *sf, uint32_t cs, uint32_t addr, uint32_t length);

/**
 * @brief write page without dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[in] data  write data
 * @param[in] length  length
 **/
void drv_sf_write_page_nodma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, const void *data, uint32_t length);

/**
 * @brief write page with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[in] data  write data
 * @param[in] length  length
 **/
void drv_sf_write_page_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, const void *data, uint32_t length);

/**
 * @brief write page
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[in] data  write data
 * @param[in] length  length
 **/
void drv_sf_write_page(OM_SF_Type *sf, uint32_t cs, uint32_t addr, const void *data, uint32_t length);

/**
 * @brief write sec
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[in] data  write data
 * @param[in] length  length
 **/
void drv_sf_write_sec(OM_SF_Type *sf, uint32_t cs, uint32_t addr, const void *data, uint32_t length);

/**
 * @brief sf write
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[in] data  write data
 * @param[in] length  length
 **/
void drv_sf_write(OM_SF_Type *sf, uint32_t cs, uint32_t addr, const void *data, uint32_t length);

/**
 * @brief read normal without dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_normal_nodma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length);

/**
 * @brief read normal with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_normal_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length);

/**
 * @brief read fast with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_fast_nodma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length);

/**
 * @brief read fast with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_fast_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length);

/**
 * @brief read fast 2line with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_fast_dual_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length);

/**
 * @brief read fast 4line with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_fast_quad_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length);

/**
 * @brief read sec
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_sec(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length);

/**
 * @brief sf read
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length);

/**
 * @brief sf config
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] config  sf config
 **/
void drv_sf_config(OM_SF_Type *sf, uint32_t cs, const drv_sf_config_t *config);

/**
 * @brief sf enable
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
void drv_sf_enable(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief  sf disable
 *
 * @param[in] sf  sf
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
void drv_sf_disable(OM_SF_Type *sf, uint32_t cs);

/**
 *******************************************************************************
 * @brief  drv sf iflash disable power immediate
 *
 * @param[in] enable  immediate
 *******************************************************************************
 */
void drv_sf_iflash_power_ctrl_immediate_enable(bool enable);

/**
 *******************************************************************************
 * @brief  drv sf iflash auto powerdown in sleep enable
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
void drv_sf_iflash_auto_powerdown_in_sleep_enable(bool enable);

/**
 * @brief sf iflash auto close
 *
 * @param[in] delay_ms  
 **/
void drv_sf_iflash_auto_close(uint32_t delay_ms);

/**
 * @brief  sf iflash open extra delay set
 *
 * @param[in] delay_10us  delay 10us
 **/
void drv_sf_iflash_extra_open_delay_set(uint32_t delay_10us);

/**
 *******************************************************************************
 * @brief  drv sf iflash encrypt enable
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
void drv_sf_iflash_encrypt_enable(bool enable);

/**
 *******************************************************************************
 * @brief  drv sf suspend enable
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
void drv_sf_auto_suspend_enable(bool enable);

/**
 * @brief sf detect
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return is present ?
 **/
bool drv_sf_detect(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief sf status
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return status
 **/
drv_sf_status_t drv_sf_status(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief sf capacity
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return capacity
 **/
uint32_t drv_sf_capacity(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief sf get id
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return sflash id (saved by @ref drv_sf_detect)
 **/
uint32_t drv_sf_id(OM_SF_Type *sf, uint32_t cs);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

