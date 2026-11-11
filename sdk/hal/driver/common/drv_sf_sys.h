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
 * @file     drv_sf_sys.h
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

#ifndef __SF_SYS_H__
#define __SF_SYS_H__

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
/// Default system SFlash clock frequency in Hz
#ifdef CONFIG_XIP_FLASH_ALL
#define DRV_SFS_CLK_FREQ_HZ_DEF     16000000
#else
#define DRV_SFS_CLK_FREQ_HZ_DEF     1000000
#endif


/// @cond

// current obj
#define DRV_SFS_CUR_OBJ             drv_sfs_cur_obj(), (uint32_t)drv_sfs_located() // Fix digital bug

// only compatible for old bootloader
#define DRV_SFS_IS_IN_SFLASH(addr)  ((addr) < OM_BUS_SRAM_BASE)

#if (RTE_SF_SYS_USING_ROM_SYMBOL)
#define drv_sfs_config              rom_drv_sfs_config
#define drv_sfs_probe               rom_drv_sfs_probe
#define drv_sfs_select              rom_drv_sfs_select
#define drv_sfs_located             rom_drv_sfs_located
#define drv_sfs_raw_op              rom_drv_sfs_raw_op
#define drv_sfs_lowpower_enter      rom_drv_sfs_lowpower_enter
//#define drv_sfs_lowpower_leave      rom_drv_sfs_lowpower_leave
#define drv_sfs_cur_obj             rom_drv_sfs_cur_obj
#endif  /* (RTE_SF_SYS_USING_ROM_SYMBOL) */

/// @endcond

/*********************************************************************
 * TYPEDEFS
 */
/// system sflash location
typedef enum
{
    DRV_SFS_IFLASH,
    DRV_SFS_XFLASH,
    DRV_SFS_LOCATE_NUM,
}drv_sfs_locate_t;

/// @cond
typedef struct
{
    drv_sfs_locate_t locate;
}drv_sfs_env_t;
/// @endcond

/*********************************************************************
 * EXTERN VARIABLES
 */
/// @cond
extern drv_sfs_env_t drv_sfs_env;
/// @endcond

/*********************************************************************
 * EXTERN FUNCTIONS
 */

/// read sr
#define drv_sfs_read_sr()                                       drv_sf_read_sr(DRV_SFS_CUR_OBJ)
/// read sr2
#define drv_sfs_read_sr2()                                      drv_sf_read_sr2(DRV_SFS_CUR_OBJ)
/// read sr with 16bits
#define drv_sfs_read_sr_16bits()                                drv_sf_read_sr_16bits(DRV_SFS_CUR_OBJ)
/// wait sr no busy
#define drv_sfs_wait_sr_no_busy()                               drv_sf_wait_sr_no_busy(DRV_SFS_CUR_OBJ)
/// write enable
#define drv_sfs_write_enable()                                  drv_sf_write_enable(DRV_SFS_CUR_OBJ)
/// write sr
#define drv_sfs_write_sr(sr)                                    drv_sf_write_sr(DRV_SFS_CUR_OBJ, sr)
/// write sr with 16bits
#define drv_sfs_write_sr_16bits(sr)                             drv_sf_write_sr_16bits(DRV_SFS_CUR_OBJ, sr)
/// write sr with mask
#define drv_sfs_write_sr_mask(mask, value)                      drv_sf_write_sr_mask(DRV_SFS_CUR_OBJ, mask, value)
/// write 16bits sr with mask
#define drv_sfs_write_sr_mask_16bits(mask, value)               drv_sf_write_sr_mask_16bits(DRV_SFS_CUR_OBJ, mask, value)
/// quad enable
#define drv_sfs_quad_enable(enable)                             drv_sf_quad_enable(DRV_SFS_CUR_OBJ, enable)
/// XIP quad enable (if used, re-setup after wakeup)
#define drv_sfs_xip_quad_enable(enable)                         do { OM_SF->READ_OPCODE_REG = enable ? 0x6B6B : 0x3B3B; }while(0)
/// otp set
#define drv_sfs_otp_set(lb_mask)                                drv_sf_otp_set(DRV_SFS_CUR_OBJ, lb_mask)
/// otp get
#define drv_sfs_otp_get()                                       drv_sf_otp_get(DRV_SFS_CUR_OBJ)
/// low power enter
#define drv_sfs_deep_power_down_enter()                         drv_sf_deep_power_down_enter(DRV_SFS_CUR_OBJ)
/// low power leave
#define drv_sfs_deep_power_down_leave()                         drv_sf_deep_power_down_leave(DRV_SFS_CUR_OBJ)
/// unlock all
#define drv_sfs_unlock_all()                                    drv_sf_unlock_all(DRV_SFS_CUR_OBJ)
/// read id
#define drv_sfs_read_id()                                       drv_sf_read_id(DRV_SFS_CUR_OBJ)
/// read uid
#define drv_sfs_read_uid_ex(data, length)                       drv_sf_read_uid_ex(DRV_SFS_CUR_OBJ, data, length)
/// read uid
#define drv_sfs_read_uid()                                      drv_sf_read_uid(DRV_SFS_CUR_OBJ)
/// erase chip
//#define drv_sfs_erase_chip()                                    drv_sf_erase_chip(DRV_SFS_CUR_OBJ)
/// erase sector
#define drv_sfs_erase_sector(addr)                              drv_sf_erase_sector(DRV_SFS_CUR_OBJ, addr)
/// erase block
#define drv_sfs_erase_block(addr)                               drv_sf_erase_block(DRV_SFS_CUR_OBJ, addr)
/// erase sec
#define drv_sfs_erase_sec(addr)                                 drv_sf_erase_sec(DRV_SFS_CUR_OBJ, addr)
/// erase
#define drv_sfs_erase(addr, length)                             drv_sf_erase(DRV_SFS_CUR_OBJ, addr, length)
/// write sec
#define drv_sfs_write_sec(addr, data, length)                   drv_sf_write_sec(DRV_SFS_CUR_OBJ, addr, data, length)
/// write
#define drv_sfs_write(addr, data, length)                       drv_sf_write(DRV_SFS_CUR_OBJ, addr, data, length)
/// read sec
#define drv_sfs_read_sec(addr, data, length)                    drv_sf_read_sec(DRV_SFS_CUR_OBJ, addr, data, length)
/// read
#define drv_sfs_read(addr, data, length)                        drv_sf_read(DRV_SFS_CUR_OBJ, addr, data, length)
/// status
#define drv_sfs_status()                                        drv_sf_status(DRV_SFS_CUR_OBJ)
/// get capacity
#define drv_sfs_capacity()                                      drv_sf_capacity(DRV_SFS_CUR_OBJ)
/// get sf id
#define drv_sfs_id()                                            drv_sf_id(DRV_SFS_CUR_OBJ)
/// enable
#define drv_sfs_enable()                                        drv_sf_enable(DRV_SFS_CUR_OBJ)
/// disable
#define drv_sfs_disable()                                       drv_sf_disable(DRV_SFS_CUR_OBJ)

/// erase sector
#define drv_sfs_erase_sector_n(sector_n)                        drv_sf_erase(DRV_SFS_CUR_OBJ, ((sector_n)<<DRV_SF_SECTOR_SHIFT), DRV_SF_SECTOR_SIZE)
/// write sector
#define drv_sfs_write_sector_n(sector_n, offset, data, length)  drv_sf_write(DRV_SFS_CUR_OBJ, ((sector_n)<<DRV_SF_SECTOR_SHIFT) + (offset), data, length)
/// read sector
#define drv_sfs_read_sector_n(sector_n, offset, data, length)   drv_sf_read(DRV_SFS_CUR_OBJ, ((sector_n)<<DRV_SF_SECTOR_SHIFT) + (offset), data, length)
/// is present ?
#define drv_sfs_is_present()                                    (drv_sfs_status() == DRV_SF_STATUS_PRESENT)

/**
 * @brief config
 *
 * @param[in] freq_hz  frequency in Hz
 * @param[in] width  width
 * @param[in] delay  delay
 **/
void drv_sfs_config(uint32_t freq_hz, drv_sf_width_t width, uint8_t delay);

/**
 * @brief sflash probe
 *
 * @param[in] locate  locate
 * @param[in] freq_hz  frequency in Hz
 *
 * @return errno
 **/
int drv_sfs_probe(drv_sfs_locate_t locate, uint32_t freq_hz);

/**
 * @brief select inside or outside flash
 *
 * @param[in] locate  locate
 *
 * @return errno
 **/
int drv_sfs_select(drv_sfs_locate_t locate);

/**
 * @brief  sfs located
 *
 * @return located
 **/
drv_sfs_locate_t drv_sfs_located(void);

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
                void *data, uint16_t data_bytes);

/**
 * @brief  sfs lowpower enter
 *
 * @param[in] power_status  power status
 **/
void drv_sfs_lowpower_enter(pm_status_t power_status);

/**
 * @brief  sfs lowpower leave
 *
 * @param[in] power_status  power status
 **/
void drv_sfs_lowpower_leave(pm_status_t power_status);

/**
 * @brief  sfs cur obj
 *
 * @return
 **/
OM_SF_Type *drv_sfs_cur_obj(void);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

