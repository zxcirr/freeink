/*********************************************************************
 * @file eeprom.h
 * @brief
 * @version 1.0
 * @date
 * @author
 *
 * @addtogroup eeprom
 * @ingroup
 * @details
 *
 * @note
 */
#ifndef __EEPROM
#define __EEPROM

#include <stdint.h>
#include <stdbool.h>
#include <mbr.h>
#include "om_driver.h"
#include "om.h"

#define EE_PAGE_COUNT ((((PART_TYPE_CFG + 2) << 12) - (PART_TYPE_CFG << 12)) / DRV_SF_SECTOR_SIZE)
#define EE_ITEM_COUNT (DRV_SF_SECTOR_SIZE / FLASH_DATA_SIZE)

#define EE_ADDR_Speed	0
#define EE_ADDR_SN	2
#define EE_ADDR_Dummy	5
#define EE_ADDR_PINCODE 1

typedef enum
{
	eePageStatusErased = 0xFF,
	eePageStatusReceiving = 0xAA,
	eePageStatusActive = 0x00,
} EE_PageStatus_t;

bool EE_Init(void);
void EE_Format(void);
bool EE_Read(uint32_t ee_addr, uint32_t *value);
bool EE_Write(uint32_t ee_addr, uint32_t value);

EE_PageStatus_t EE_getPageStatus(uint32_t page);
void EE_FlashErase(uint32_t page);
void EE_FlashWrite(uint32_t page, uint32_t index, uint32_t ee_addr, uint32_t value);
void EE_FlashRead(uint32_t page, uint32_t index, uint32_t *ee_addr, uint32_t *value);

#endif