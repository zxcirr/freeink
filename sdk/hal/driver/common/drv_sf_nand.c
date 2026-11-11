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
 * @file     drv_sf_nand.c
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

/*********************************************************************
 * INCLUDES
 */
#include "om_driver.h"
#include "drv_sf_nand.h"

/*********************************************************************
 * MACROS
 */
#define SPI_CMD_WREN            0x06u /* Write enable */
#define SPI_CMD_WRDI            0x04u /* Write disable */
#define SPI_CMD_GETFEAT         0x0Fu /* Get Features */
#define SPI_CMD_SETFEAT         0x1Fu /* Set Features */
#define SPI_CMD_READ_TO_CACHE   0x13u /* Page Read */
#define SPI_CMD_READ_CACHE      0x03u /* Read data bytes) */
#define SPI_CMD_DUAL_READ_CACHE 0x3Bu /* Dual output fast read */
#define SPI_CMD_QUAD_READ_CACHE 0x6Bu /* Quad output fast read */
#define SPI_CMD_PROG_LOAD       0x02u /* Page program (up to page in 256 bytes) */
#define SPI_CMD_PROG_EXE        0x10u /* Page program (up to page in 256 bytes) */
#define SPI_CMD_RDID            0x9fu /* Read JEDEC ID */
#define SPI_CMD_BLOCK_ERASE     0xD8u /* Erase block */
#define SPI_CMD_RESET           0xFFu /* Reset */

#define FEAT_CMD_LOCK           0xA0
#define FEAT_CMD_FEAT           0xB0
#define FEAT_CMD_STATUS         0xC0

#define FEAT_STATUS_OIP         0x01  /* Write in progress */
#define FEAT_STATUS_WEL         0x02  /* Write enable latch */
#define FEAT_STATUS_E_FAIL      0x04  /* Erase fail */
#define FEAT_STATUS_P_FAIL      0x08  /* Prog fail */
#define FEAT_STATUS_ECCS0       0x04  /* ECC status 0 */
#define FEAT_STATUS_ECCS1       0x08  /* ECC status 1 */
#define FEAT_STATUS_ECCS2       0x10  /* ECC status 2 */
#define FEAT_STATUS_ECCS3       0x20  /* ECC status 3 */
#define FEAT_STATUS_ECCS_POS       2  /* ECC status */
#define FEAT_STATUS_ECCS_MASK   0x3C  /* ECC status */

#define FEAT_QE                 0x01
#define FEAT_ECC_EN             0x10
#define FEAT_OPT_EN             0x40
#define FEAT_OPT_PRT            0x80

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief  sf nand read feat
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *
 * @return feature
 **/
uint8_t drv_sf_nand_read_status(OM_SF_Type *sf, uint32_t cs)
{
    drv_sfb_rw_params_t param;
    uint8_t feature;

    param.cm.d0 = SPI_CMD_GETFEAT;
    param.cm.d1 = FEAT_CMD_STATUS;
    param.cmd_bits = 16;
    param.data = &feature;
    param.data_bytes = 1;

    drv_sfb_read_nodma(sf, cs, &param);

    return feature;
}

/**
 * @brief  sf wait sr no busy
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 **/
void drv_sf_nand_wait_feat_no_busy(OM_SF_Type *sf, uint32_t cs)
{
    // OIP: bit0
    while(drv_sf_nand_read_status(sf, cs) & FEAT_STATUS_OIP)
    {
        drv_dwt_delay_us(10 * 2);
    }
}

/**
 * @brief  sf nand write lock
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] feature  feature
 **/
void drv_sf_nand_write_lock(OM_SF_Type *sf, uint32_t cs, uint8_t lock)
{
    drv_sfb_rw_params_t param;

    drv_sf_nand_write_enable(sf, cs);

    param.cm.d0 = SPI_CMD_SETFEAT;
    param.cm.d1 = FEAT_CMD_LOCK;
    param.cmd_bits = 16;
    param.data = &lock;
    param.data_bytes = 1;

    drv_sfb_write_nodma(sf, cs, &param);

    drv_sf_nand_wait_feat_no_busy(sf, cs);
}

/**
 * @brief  sf nand write feat
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] feature  feature
 **/
void drv_sf_nand_write_feat(OM_SF_Type *sf, uint32_t cs, uint8_t feature)
{
    drv_sfb_rw_params_t param;

    drv_sf_nand_write_enable(sf, cs);

    param.cm.d0 = SPI_CMD_SETFEAT;
    param.cm.d1 = FEAT_CMD_FEAT;
    param.cmd_bits = 16;
    param.data = &feature;
    param.data_bytes = 1;

    drv_sfb_write_nodma(sf, cs, &param);

    drv_sf_nand_wait_feat_no_busy(sf, cs);
}

/**
 * @brief  sf nand read feat
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *
 * @return feature
 **/
uint8_t drv_sf_nand_read_feat(OM_SF_Type *sf, uint32_t cs)
{
    drv_sfb_rw_params_t param;
    uint8_t feature;

    param.cm.d0 = SPI_CMD_GETFEAT;
    param.cm.d1 = FEAT_CMD_FEAT;
    param.cmd_bits = 16;
    param.data = &feature;
    param.data_bytes = 1;

    drv_sfb_read_nodma(sf, cs, &param);

    return feature;
}

/**
 * @brief  sf nand read id
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *
 * @return id: MID + DID
 **/
uint32_t drv_sf_nand_read_id(OM_SF_Type *sf, uint32_t cs)
{
    uint32_t id = 0;
    drv_sfb_rw_params_t param;

    param.cm.d0 = SPI_CMD_RDID;
    param.cm.d1 = 0;
    param.cmd_bits = 16;
    param.data = &id;
    param.data_bytes = 2;

    drv_sfb_read_nodma(sf, cs, &param);

    id = ((id&0xFF00)>>8) | ((id&0x00FF)<<8);

    return id;
}

/**
 * @brief  sf nand read to cache
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] raw_addr  raw addr
 **/
void drv_sf_nand_read_to_cache(OM_SF_Type *sf, uint32_t cs, uint32_t raw_addr)
{
    drv_sfb_rw_params_t param;

    param.cmd[0] = (SPI_CMD_READ_TO_CACHE<<24) | raw_addr;
    param.cmd_bits = 32;
    param.data = NULL;
    param.data_bytes = 0;
    drv_sfb_write_nodma(sf, cs, &param);

    drv_sf_nand_wait_feat_no_busy(sf, cs);
}

/**
 * @brief  sf nand read from cache normal dma
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] col_addr  col addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_sf_nand_read_from_cache_normal_dma(OM_SF_Type *sf, uint32_t cs, uint32_t col_addr, void *data, uint32_t length)
{
    drv_sfb_rw_params_t param;

    param.cmd[0] = (SPI_CMD_READ_CACHE<<24) | (col_addr<<8);
    param.cmd_bits = 32;
    param.data = data;
    param.data_bytes = length;

    drv_sfb_read_dma(sf, cs, &param);
}

/**
 * @brief  sf nand read from cache dual dma
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] col_addr  col addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_sf_nand_read_from_cache_dual_dma(OM_SF_Type *sf, uint32_t cs, uint32_t col_addr, void *data, uint32_t length)
{
    drv_sfb_rw_params_t param;

    param.cmd[0] = (SPI_CMD_DUAL_READ_CACHE<<24) | (col_addr<<8);
    param.cmd_bits = 32;
    param.data = data;
    param.data_bytes = length;

    drv_sfb_read_dma(sf, cs, &param);
}

/**
 * @brief  sf nand read from cache quad dma
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] col_addr  col addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_sf_nand_read_from_cache_quad_dma(OM_SF_Type *sf, uint32_t cs, uint32_t col_addr, void *data, uint32_t length)
{
    drv_sfb_rw_params_t param;

    param.cmd[0] = (SPI_CMD_QUAD_READ_CACHE<<24) | (col_addr<<8);
    param.cmd_bits = 32;
    param.data = data;
    param.data_bytes = length;

    drv_sfb_read_dma(sf, cs, &param);
}

/**
 * @brief  sf read dual dma nand
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] addr  addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_sf_nand_read_page_normal_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length)
{
    OM_ASSERT((length & 3) == 0);
    OM_ASSERT(((uint32_t)data & 3) == 0);

    drv_sf_nand_read_to_cache(sf, cs, DRV_SF_NAND_ADDR2ROW(addr));

    drv_sf_nand_read_from_cache_normal_dma(sf, cs, DRV_SF_NAND_ADDR2COL(addr), data, length);
}

/**
 * @brief  sf read dual dma nand
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] addr  addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_sf_nand_read_page_dual_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length)
{
    OM_ASSERT((length & 7) == 0);
    OM_ASSERT(((uint32_t)data & 7) == 0);

    drv_sf_nand_read_to_cache(sf, cs, DRV_SF_NAND_ADDR2ROW(addr));

    drv_sf_nand_read_from_cache_dual_dma(sf, cs, DRV_SF_NAND_ADDR2COL(addr), data, length);
}

/**
 * @brief  sf nand read page quad dma
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] addr  addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_sf_nand_read_page_quad_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length)
{
    OM_ASSERT((length & 15) == 0);
    OM_ASSERT(((uint32_t)data & 15) == 0);

    drv_sf_nand_read_to_cache(sf, cs, DRV_SF_NAND_ADDR2ROW(addr));

    drv_sf_nand_read_from_cache_quad_dma(sf, cs, DRV_SF_NAND_ADDR2COL(addr), data, length);
}

/**
 * @brief  sf nand write to cache dma
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] col_addr  col addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_sf_nand_prog_load_dma(OM_SF_Type *sf, uint32_t cs, uint32_t col_addr, const void *data, uint32_t length)
{
    drv_sfb_rw_params_t param;

    param.cmd[0] = (SPI_CMD_PROG_LOAD<<24) | col_addr;
    param.cmd_bits = 24;
    param.data = data;
    param.data_bytes = length;
    drv_sfb_write_dma(sf, cs, &param);
}

/**
 * @brief  sf nand prog exe
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] addr  addr
 **/
void drv_sf_nand_prog_exe(OM_SF_Type *sf, uint32_t cs, uint32_t addr)
{
    drv_sfb_rw_params_t param;

    drv_sf_nand_write_enable(sf, cs);

    param.cmd[0] = (SPI_CMD_PROG_EXE<<24) | (addr);
    param.cmd_bits = 32;
    param.data = NULL;
    param.data_bytes = 0;
    drv_sfb_write_nodma(sf, cs, &param);

    drv_sf_nand_wait_feat_no_busy(sf, cs);
}

/**
 * @brief  sf nand write page dma
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] addr  addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_sf_nand_write_page_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, const void *data, uint32_t length)
{
    OM_ASSERT((length & 3) == 0);
    OM_ASSERT(((uint32_t)data & 3) == 0);

    drv_sf_nand_prog_load_dma(sf, cs, DRV_SF_NAND_ADDR2COL(addr), data, length);

    drv_sf_nand_prog_exe(sf, cs, DRV_SF_NAND_ADDR2ROW(addr));
}

/**
 * @brief  sf nand erase block
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] addr  addr
 **/
void drv_sf_nand_erase_block(OM_SF_Type *sf, uint32_t cs, uint32_t addr)
{
    drv_sfb_rw_params_t param;

    drv_sf_nand_write_enable(sf, cs);

    param.cmd[0] = (SPI_CMD_BLOCK_ERASE<<24) | (addr);
    param.cmd_bits = 32;
    param.data = NULL;
    param.data_bytes = 0;
    drv_sfb_write_nodma(sf, cs, &param);

    drv_sf_nand_wait_feat_no_busy(sf, cs);
}

/** @} */

