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
 * @file     drv_sf_nand.h
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

#ifndef __SF_NAND_H__
#define __SF_NAND_H__

#ifdef __cplusplus
extern "C"
{ /*}*/
#endif

/*********************************************************************
 * INCLUDES
 */
#include "om_driver.h"

/*********************************************************************
 * MACROS
 */
/// Page size
#define DRV_SF_NAND_PAGE_SIZE       (2048+64)
/// Cache size
#define DRV_SF_NAND_CACHE_SIZE      2048
/// Page num of one block
#define DRV_SF_NAND_BLOCK_PAGENUM   64

/// address convert
#define DRV_SF_NAND_ADDR2BLOCK(addr) ( (addr)/(DRV_SF_NAND_BLOCK_PAGENUM*DRV_SF_NAND_PAGE_SIZE))
#define DRV_SF_NAND_ADDR2PAGE(addr)  (((addr)%(DRV_SF_NAND_BLOCK_PAGENUM*DRV_SF_NAND_PAGE_SIZE))/DRV_SF_NAND_PAGE_SIZE)
#define DRV_SF_NAND_ADDR2ROW(addr)   ((DRV_SF_NAND_ADDR2BLOCK(addr)<<6) | DRV_SF_NAND_ADDR2PAGE(addr))
#define DRV_SF_NAND_ADDR2COL(addr)   ((addr)%DRV_SF_NAND_PAGE_SIZE)

/// function
#define drv_sf_nand_enable          drv_sf_enable
#define drv_sf_nand_disable         drv_sf_disable
#define drv_sf_nand_config          drv_sf_config
#define drv_sf_nand_write_enable    drv_sf_write_enable

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 * @brief  sf nand write lock
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] feature  feature
 **/
void drv_sf_nand_write_lock(OM_SF_Type *sf, uint32_t cs, uint8_t lock);

/**
 * @brief  sf nand read id
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *
 * @return id: MID + DID
 **/
uint32_t drv_sf_nand_read_id(OM_SF_Type *sf, uint32_t cs);

/**
 * @brief  sf read normal dma nand
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] addr  addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_sf_nand_read_page_normal_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length);

/**
 * @brief  sf read dual dma nand
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] addr  addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_sf_nand_read_page_dual_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length);

/**
 * @brief  sf nand read page quad dma
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] addr  addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_sf_nand_read_page_quad_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length);

/**
 * @brief  sf nand write page dma
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] addr  addr
 * @param[in] data  data
 * @param[in] length  length
 **/
void drv_sf_nand_write_page_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, const void *data, uint32_t length);

/**
 * @brief  sf nand erase block
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] addr  addr
 **/
void drv_sf_nand_erase_block(OM_SF_Type *sf, uint32_t cs, uint32_t addr);


#ifdef __cplusplus
/*{*/ }
#endif

#endif

/** @} */

