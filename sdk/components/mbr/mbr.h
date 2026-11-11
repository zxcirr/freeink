 /*********************************************************************
 * @file mbr.h
 * @brief 
 * @version 1.0
 * @date 20 April 2015
 * @author luwei
 *
 * @addtogroup mbr
 * @ingroup 
 * @details 
 *
 * @note 
 */

#ifndef _MBR_H
#define _MBR_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>

/*********************************************************************
 * TYPEDEFS
 */

/* Sector 0 */
#define OPT_JTAG_ON_MASK (0x7<<0) /* all 1 is jtag on */
#define OPT_ENC_OFF      (0x1<<3) /* 1-Enc is off;  0-Enc is on   */
#define OPT_UNLOCK_MASK  (OPT_JTAG_ON_MASK | OPT_ENC_OFF)

#define OPT_NODELAY_MASK (0xF<<4) /* Not delay */
#define OPT_NODELAY_FLAG (0xA<<4) /* Not delay */

#define OPT_BYTES_OFFSET 0
#define ENC_KEY_OFFSET   4
#define SEC_KEY_LEN      16 /* encryption key, private key in 128-bit */

/* OTP */
#define OTP_CHIP_ID      1 /* LB1 */
#define OTP_CHIP_FUNC    2 /* LB2 */
#define OTP_AUTH         3 /* LB3 */ 

/* Vectors */
//#define VEC_APP_SIZE     8
//#define VEC_APP_CRC      9
#define VEC_RUN2SF_OFFSET 9
#define VEC_RUN2SF_MAGIC  10
#define VEC_RUN2OTP_MAGIC 10
#define VEC_APP_MAGIC     13

/* flag */
#define RUN2SF_MAGIC_FLAG   0x46533252  // R2SF
#define RUN2OTP_MAGIC_FLAG  0x50544f5A  // \x5AOTP
#define RUN2OTP_CP2RAM_MAGIC_FLAG  0x50544fA5  // \xA5OTP

/* Security Registers */
#define SEC_CHIP_ID_OFFSET   (0*256)
#define SEC_CHIP_FUNC_OFFSET (1*256)
#define SEC_AUTH_PRIV_OFFSET (2*256)

/* Reserved flash sector */
#define RSV_TRACE_ID_OFFSET    12
#define RSV_TRACE_ID_LENGTH    8
#define RSV_GPADC_CALI_OFFSET  8
#define RSV_GPADC_CALI_LENGTH  8

#define OPT_SECTOR_START 0
#define MBR_SECTOR_START 128
#define CPFT_SECTOR_START 3
#define APP_SECTOR_START 4

/* Sector 1 */
#define MBR_SIGNATURE UINT16_C(0xAA55)

#define FLASH_SETTINGS_SIZE     3
#define FLASH_CLOCK_OFFSET	    0x17b
#define FLASH_WIDTH_OFFSET	    0x17c
#define FLASH_DELAY_OFFSET	    0x17d
#define DOS_PART_TBL_OFFSET	    0x17e
#define DOS_PART_MAGIC_OFFSET	0x1fe

#define PART_TYPE_MASK_HS 0x60
#define PART_TYPE_APP     0x60
#define PART_TYPE_PATCH   0x61
#define PART_TYPE_CFG     0x32
#define PART_TYPE_USER    0x34
#define PART_TYPE_OTP     0x63
#define PART_TYPE_USR1    0x04
#define PART_TYPE_USR2    0x05
#define PART_TYPE_USR3    0x06
#define PART_TYPE_USR4    0x07
#define MBR_PART_NUM      8

/* Sector CFG */
#define FLASH_DATA_NUM    0x4EC0
#define FLASH_DATA_SIZE   0x8
#define FLASH_DATA_MAX_OFFSET (FLASH_DATA_NUM * FLASH_DATA_SIZE)


// Data for a single MBR partition record
// Note that firstSector and lastSector are in CHS addressing, which
// splits the bits up in a weird way.
// On read or write of MBR entries, firstLBA is an absolute disk sector.
// On read of logical entries, it's relative to the EBR record for that
// partition. When writing EBR records, it's relative to the extended
// partition's start.

typedef struct MBRRecord {
  uint8_t status;
  uint8_t firstSector[3];
  uint8_t partitionType;
  uint8_t lastSector[1];
  uint16_t crc16;
  uint32_t firstLBA;  /* starting sector counting from 0	*/
  uint32_t lengthLBA; /* nr of sectors in partition		*/
} mbr_part_t;

int mbr_get_security(uint32_t *p_option_bytes, uint8_t *p_key);
int mbr_set_data(uint8_t *option_bytes, uint16_t len,uint32_t sector, uint32_t offset);
int write_cgms_data(uint8_t *data);
//int mbr_set_security_option_with_mask(uint32_t option_mask, uint32_t option_value, uint32_t *new_option);
int sec_get_chip_id(uint8_t *p_id);
int sec_get_auth_pri(uint8_t *p_pri);

int mbr_probe(bool create_if_fail);
int mbr_read_part(int img_type, uint32_t *p_addr, uint32_t *p_len, uint16_t *p_crc);
int mbr_alloc_part(int img_type, uint32_t *p_addr, uint32_t len);
int mbr_write_part(int img_type, uint32_t addr, uint32_t len, uint16_t crc);
int mbr_write_clock(uint8_t clock, uint8_t width, uint8_t delay);
void mbr_boost_clock(void);
void mbr_restore_clock(void);
int mbr_validate_app(uint32_t addr, uint32_t len);

int mbr_get_app(uint32_t *p_addr, uint32_t *p_len, uint16_t *p_crc);
int mbr_get_patch(uint32_t *p_addr, uint32_t *p_len, uint16_t *p_crc);
void get_cgms_data(uint8_t *option_bytes ,uint32_t idx_start, uint32_t idx_end);
/**
 * @brief get cpft address and length
 *
 * @param[out] p_addr  cpft base address
 * @param[out] p_len  cpft length
 *
 * @return errno or 0 success
 **/
int mbr_get_cpft(uint32_t *p_addr, uint32_t *p_len);

/**
 * @brief get free space address and length
 *
 * @param[out] p_addr  free space address
 * @param[out] p_len  freee space length
 *
 * @return errno or 0 success
 **/
int mbr_get_free_area(uint32_t *p_addr, uint32_t *p_len);

//basic sf api
void mbr_sf_erase_sector_test(uint32_t sector_n, uint32_t offset, uint32_t length);
void mbr_sf_write_sector_verify(uint32_t sector_n, uint32_t offset, const void *data, uint32_t length);
void mbr_sf_read_sector_verify(uint32_t sector_n, uint32_t offset, void *data, uint32_t length);



#ifdef __cplusplus
}
#endif
#endif
