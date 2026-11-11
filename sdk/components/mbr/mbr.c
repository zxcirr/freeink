/*********************************************************************
 * @file mbr.c
 * @brief
 * @version 1.0
 * @date 16 July 2016
 * @author luwei
 *
 * @addtogroup mbr
 * @ingroup
 * @details
 *
 * @note
 */

/*********************************************************************
 * INCLUDES
 */

#include "om.h"
#include "mbr.h"
#include "om_driver.h"

/*********************************************************************
 * MACROS
 */
#define TAG "[mbr]"

#define SECTOR_NIL 0xFF // uint8_t

/*********************************************************************
 * LOCAL VARIABLES
 */
typedef struct
{
    uint8_t sector_no_mas;
    uint8_t sector_no_bak;
    uint8_t probed;
} mbr_info_t;

static mbr_info_t m_mbr_info = {SECTOR_NIL, SECTOR_NIL, 0};

/********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief  mbr sf sector verify
 *
 * @param[in] sector_n  sector n
 * @param[in] offset  offset
 * @param[in] data  data
 * @param[in] length  length
 *
 * @return
 **/
static bool mbr_sf_sector_is_ok(uint32_t sector_n, uint32_t offset, const void *data, uint32_t length)
{
    __ALIGNED(8)
    uint8_t vbuffer[64];
    uint32_t voffset, vleft, vlength;

    for (voffset = 0; voffset < length; voffset += vlength)
    {
        vleft = length - voffset;
        vlength = vleft < sizeof(vbuffer) ? vleft : sizeof(vbuffer);

        drv_sfs_read_sector_n(sector_n, offset + voffset, vbuffer, vlength);

        if (memcmp((uint8_t *)data + voffset, vbuffer, vlength) != 0)
            return false;
    }

    return true;
}

/**
 * @brief  mbr sf read sector
 *
 * @param[in] sector_n  sector n
 * @param[in] offset  offset
 * @param[in] data  data
 * @param[in] length  length
 **/
void mbr_sf_read_sector_verify(uint32_t sector_n, uint32_t offset, void *data, uint32_t length)
{
    do
    {
        drv_sfs_read_sector_n(sector_n, offset, data, length);
    } while (!mbr_sf_sector_is_ok(sector_n, offset, data, length));
}

/**
 * @brief  mbr sf erase write sector verify
 *
 * @param[in] sector_n  sector n
 * @param[in] offset  offset
 * @param[in] data  data
 * @param[in] length  length
 **/
static void mbr_sf_erase_write_sector_verify(uint32_t sector_n, uint32_t offset, const void *data, uint32_t length)
{
    do
    {
        drv_sfs_erase_sector_n(sector_n);
        drv_sfs_write_sector_n(sector_n, offset, data, length);
    } while (!mbr_sf_sector_is_ok(sector_n, offset, data, length));
}

/**
 * @brief  mbr sf erase write sector verify
 *
 * @param[in] sector_n  sector n
 * @param[in] offset  offset
 * @param[in] data  data
 * @param[in] length  length
 **/
void mbr_sf_erase_sector_test(uint32_t sector_n, uint32_t offset, uint32_t length)
{
    drv_sf_erase(OM_SF, 0, (sector_n << 12) + offset, length);
    // drv_sf_write(OM_SF, 0, (sector_n << 12) + offset , data, length);
}

void mbr_sf_write_sector_verify(uint32_t sector_n, uint32_t offset, const void *data, uint32_t length)
{
    do
    {
        drv_sf_write(OM_SF, 0, (sector_n << 12) + offset, data, length);
    } while (!mbr_sf_sector_is_ok(sector_n, offset, data, length));
}

/**
 * @brief  mbr sf write sector noverify
 *
 * @param[in] sector_n  sector n
 * @param[in] offset  offset
 * @param[in] data  data
 * @param[in] length  length
 **/
static void mbr_sf_write_sector_noverify(uint32_t sector_n, uint32_t offset, const void *data, uint32_t length)
{
    drv_sfs_write_sector_n(sector_n, offset, data, length);
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

int mbr_validate_app(uint32_t addr, uint32_t len)
{
    return 0; /* always ok, do validation after decrypted */
}

/**
 * @brief mbr_get_security(), read option bytes and encryption key in iflash or xflash
 *
 * @return errno
 **/
int mbr_get_security(uint32_t *p_option_bytes, uint8_t *p_key)
{
    if (!drv_sfs_is_present())
    {
        // For OTP Burn
        *p_option_bytes = 0xFFFFFFFF;
        return 0;
    }

    if (p_option_bytes != NULL)
    {
        DRV_SF_IFLASH_ENCRYPT_DISABLE();
        mbr_sf_read_sector_verify(MBR_SECTOR_START, 0, p_option_bytes, sizeof(uint32_t));
        DRV_SF_IFLASH_ENCRYPT_RESTORE();
    }

    return 0;
}

/**
 * @brief mbr_set_security(), program option bytes and encryption key in iflash or xflash of CFG(204k - 1024K)
 *
 * @return errno
 **/

int mbr_set_data(uint8_t *option_bytes, uint16_t len, uint32_t sector, uint32_t offset)
{
    static uint16_t _stored_data_num;

    if (_stored_data_num >= 512 || offset == 0)
        _stored_data_num = 0;

    /* swap write */
    __ALIGNED(8)
    uint8_t write_buf[8];

    if (!drv_sfs_is_present())
        return -ENODEV;

    // disable encrypt
    DRV_SF_IFLASH_ENCRYPT_DISABLE();
    if (_stored_data_num++ == 0)
        mbr_sf_erase_sector_test(sector, offset, sizeof(write_buf));
    for (uint8_t i = 0; i < len; i++)
    {
        write_buf[i] = option_bytes[i];
    }

    mbr_sf_write_sector_verify(sector, offset, write_buf, len);
    // restore encrypt
    DRV_SF_IFLASH_ENCRYPT_RESTORE();

    return 0;
}

int write_cgms_data(uint8_t *data)
{
    static uint32_t offset_cgms_data;

    if (offset_cgms_data >= FLASH_DATA_MAX_OFFSET)
        offset_cgms_data = 0;

    mbr_set_data(data, 8, PART_TYPE_USER, offset_cgms_data);

    offset_cgms_data += 8;

    return 0;
}
/**
idx(0-xxx)
**/
void get_cgms_data(uint8_t *option_bytes, uint32_t idx_start, uint32_t idx_end)
{
    if (!drv_sfs_is_present())
        return;

    drv_sf_read(OM_SF, 0, ((PART_TYPE_USER << 12) + 8 * idx_start), option_bytes, (idx_end - idx_start + 1) * 8);
}

/**
 * @brief  mbr set security option with mask
 *
 * @param[in] option_mask  option bytes
 *
 * @return errno
 **/
/**
int mbr_set_security_option_with_mask(uint32_t option_mask, uint32_t option_value, uint32_t *new_option)
{
    uint32_t option_bytes;
    int error = mbr_get_security(&option_bytes, NULL);
    if (error >= 0)
    {
        option_bytes = (option_bytes & ~option_mask) | option_value;
        error = mbr_set_security(option_bytes, PART_TYPE_CFG, OPT_BYTES_OFFSET);
        if (error >= 0)
            error = mbr_get_security(new_option, NULL);
    }

    return error;
}
**/
/**
 * @brief mbr_probe(), probe mbr data and its swap during boot or isp
 *
 * @return errno or 0 if mbr is valid
 **/
int mbr_probe(bool create_if_fail)
{
    uint8_t i, sector_no;
    uint16_t sigs[2];

    sector_no = MBR_SECTOR_START;
    for (i = 0; i < 2; i++)
    {
        mbr_sf_read_sector_verify(sector_no + i, DOS_PART_MAGIC_OFFSET, &sigs[i], sizeof(uint16_t));
    }

    if ((sigs[0] != MBR_SIGNATURE) && (sigs[1] != MBR_SIGNATURE))
    {
        if (create_if_fail)
        {
            sigs[0] = MBR_SIGNATURE;
            mbr_sf_erase_write_sector_verify(sector_no, DOS_PART_MAGIC_OFFSET, &sigs[0], sizeof(uint16_t));
            m_mbr_info.sector_no_mas = sector_no;
            m_mbr_info.sector_no_bak = sector_no + 1;
            m_mbr_info.probed = 1;
        }
        else
        {
            m_mbr_info.probed = 0;
        }
        return -ENXIO;
    }

    if (sigs[0] == MBR_SIGNATURE)
    {
        m_mbr_info.sector_no_mas = sector_no;
        m_mbr_info.sector_no_bak = sector_no + 1;
        m_mbr_info.probed = 1;
        return 0;
    }

    if (sigs[1] == MBR_SIGNATURE)
    {
        m_mbr_info.sector_no_mas = sector_no + 1;
        m_mbr_info.sector_no_bak = sector_no;
        m_mbr_info.probed = 1;
        return 0;
    }

    return -ENODEV;
}

/**
 * @brief mbr_read_part(), get the specific partition's location, len and crc
 * @note  don't switch chip select, but use the current flash
 *
 * @return errno or 0 if found
 **/
int mbr_read_part(int img_type, uint32_t *p_addr, uint32_t *p_len, uint16_t *p_crc)
{
    mbr_part_t part;
    int data_off;
    int idx;

    if (!m_mbr_info.probed)
    {
        mbr_probe(false /*create_if_fail*/);
        if (SECTOR_NIL == m_mbr_info.sector_no_mas)
            return -ENODEV;
    }
    if ((uint8_t)img_type & PART_TYPE_MASK_HS)
        idx = img_type - PART_TYPE_APP;
    else
        idx = img_type;
    if (idx >= MBR_PART_NUM)
        return -EINVAL;

    data_off = DOS_PART_TBL_OFFSET + idx * sizeof(mbr_part_t);
    OM_LOG_DEBUG("data_off=%x,sizeof_mbr_part=%d\n", data_off, sizeof(mbr_part_t));
    mbr_sf_read_sector_verify(m_mbr_info.sector_no_mas, data_off, &part, sizeof(mbr_part_t));

    if (part.partitionType == img_type)
    {
        if (p_addr)
            *p_addr = part.firstLBA << DRV_SF_SECTOR_SHIFT;
        if (p_len)
            *p_len = part.lengthLBA;
        if (p_crc)
            *p_crc = part.crc16;
        return 0;
    }

    return -ENOENT;
}

/**
 * @brief mbr_alloc_part(), allocate a partition during ISP
 * @note  don't switch chip select, but use the current flash
 *
 * @return errno or 0 if success
 **/
int mbr_alloc_part(int img_type, uint32_t *p_addr, uint32_t len)
{
    uint32_t cfg_addr, addr;

    if (!m_mbr_info.probed)
    {
        mbr_probe(true /*create_if_fail*/);
        if (SECTOR_NIL == m_mbr_info.sector_no_mas)
            return -ENODEV;
    }

    /* cfg partition's address in default: don't occupy the last sector */
    cfg_addr = drv_sfs_capacity() - DRV_SF_SECTOR_SIZE * (2 + 1 /*cfg's swap*/ + 1 /*trace id*/);

    if (PART_TYPE_APP == img_type)
    {
        /* app partition's address in default */
        addr = APP_SECTOR_START * DRV_SF_SECTOR_SIZE;
        if ((addr + len) > cfg_addr)
            return -EINVAL;
    }
#ifdef CONFIG_PATCH_PACKAGE_ON_FLASH
    else if (PART_TYPE_PATCH == img_type)
    {
        /* put near to cfg partition and align to sector */
        addr = cfg_addr - len;
        addr = addr & ~(DRV_SF_SECTOR_SIZE - 1);

        /* check collision with the existing app partition */
        {
            uint32_t app_addr, app_len;
            int err;
            err = mbr_read_part(PART_TYPE_APP, &app_addr, &app_len, NULL);
            if ((err >= 0) && (addr < (app_addr + app_len)))
                return -EINVAL;
        }
    }
#endif
    else if (PART_TYPE_CFG == img_type)
    {
        addr = cfg_addr;
    }
    else
    {
        return -EINVAL;
    }

    if (p_addr)
        *p_addr = addr;

    return 0;
}

/**
 * @brief mbr_write_part(), write a partition during ISP
 * @note  don't switch chip select, but use the current flash
 *
 * @return errno or 0 if success
 **/
int mbr_write_part(int img_type, uint32_t addr, uint32_t len, uint16_t crc)
{
    mbr_part_t part;
    int idx, data_off;
    uint32_t invalid_mbr_part_magic = 0;

    if (!m_mbr_info.probed)
    {
        mbr_probe(true /*create_if_fail*/);
        if (SECTOR_NIL == m_mbr_info.sector_no_mas)
            return -ENODEV;
    }
    if ((uint8_t)img_type & PART_TYPE_MASK_HS)
        idx = img_type - PART_TYPE_APP;
    else
        idx = img_type;
    if (idx >= MBR_PART_NUM)
        return -EINVAL;

    data_off = DOS_PART_TBL_OFFSET + idx * sizeof(mbr_part_t);

    // make sure write is ok
    while (1)
    {
        mbr_sf_read_sector_verify(m_mbr_info.sector_no_mas, data_off, &part, sizeof(mbr_part_t));

        if ((part.partitionType == img_type) &&
            (part.crc16 == crc) &&
            (part.firstLBA == (addr >> DRV_SF_SECTOR_SHIFT)) &&
            (part.lengthLBA == len))
        {
            /* same with the existing */
            return 0;
        }
        else
        {
            /* swap write */
            __ALIGNED(8)
            uint8_t buf[512];
            /* Backup */
            mbr_sf_read_sector_verify(m_mbr_info.sector_no_mas, 0, buf, sizeof(buf));
            mbr_sf_erase_write_sector_verify(m_mbr_info.sector_no_bak, 0, buf, sizeof(buf));
            /* Destroy used section magic code */
            mbr_sf_write_sector_noverify(m_mbr_info.sector_no_mas, DOS_PART_MAGIC_OFFSET, &invalid_mbr_part_magic, sizeof(uint16_t));
            /* Fresh new */
            part.partitionType = img_type;
            part.crc16 = crc;
            part.firstLBA = addr >> DRV_SF_SECTOR_SHIFT;
            part.lengthLBA = len;
            memcpy(buf + data_off, &part, sizeof(mbr_part_t));
            mbr_sf_erase_write_sector_verify(m_mbr_info.sector_no_mas, 0, buf, sizeof(buf));
            /* Destroy backup section magic code */
            mbr_sf_write_sector_noverify(m_mbr_info.sector_no_bak, DOS_PART_MAGIC_OFFSET, &invalid_mbr_part_magic, sizeof(uint16_t));
            /* ok */
            return 0;
        }
    }
}

int mbr_write_clock(uint8_t clock, uint8_t width, uint8_t delay)
{
    int data_off;
    uint32_t old;
    uint32_t new = (delay << 16u) | (width << 8u) | (clock << 0u); // FIXME: endian
    uint32_t invalid_mbr_part_magic = 0;

    data_off = FLASH_CLOCK_OFFSET;

    // make sure write is ok
    while (1)
    {
        mbr_sf_read_sector_verify(m_mbr_info.sector_no_mas, data_off, &old, FLASH_SETTINGS_SIZE);
        old &= 0xFFFFFF;

        if (old == new)
        {
            /* same with the existing */
            return 0;
        }
        else
        {
            /* swap write */
            __ALIGNED(8)
            uint8_t buf[512];
            /* Backup */
            mbr_sf_read_sector_verify(m_mbr_info.sector_no_mas, 0, buf, sizeof(buf));
            mbr_sf_erase_write_sector_verify(m_mbr_info.sector_no_bak, 0, buf, sizeof(buf));
            /* Destroy used section magic code */
            mbr_sf_write_sector_noverify(m_mbr_info.sector_no_mas, DOS_PART_MAGIC_OFFSET, &invalid_mbr_part_magic, sizeof(uint16_t));
            /* Fresh new */
            memcpy(buf + data_off, &new, FLASH_SETTINGS_SIZE);
            mbr_sf_erase_write_sector_verify(m_mbr_info.sector_no_mas, 0, buf, sizeof(buf));
            /* Destroy backup section magic code */
            mbr_sf_write_sector_noverify(m_mbr_info.sector_no_bak, DOS_PART_MAGIC_OFFSET, &invalid_mbr_part_magic, sizeof(uint16_t));
            /* ok */
            return 0;
        }
    }
}

void mbr_boost_clock(void)
{
    uint8_t settings[FLASH_SETTINGS_SIZE];
    uint32_t freq_hz;
    drv_sf_width_t width;
    uint8_t delay;

    mbr_sf_read_sector_verify(m_mbr_info.sector_no_mas, FLASH_CLOCK_OFFSET, &settings, FLASH_SETTINGS_SIZE);

    if (settings[0] != 0xFF && settings[1] != 0xFF && settings[2] != 0xFF)
    {
        freq_hz = settings[0] * 1000000;
        width = settings[1] ? (drv_sf_width_t)settings[1] : DRV_SF_WIDTH_1LINE;
        delay = settings[2];
        drv_sfs_config(freq_hz, width, delay);
    }
}

void mbr_restore_clock(void)
{
    drv_sfs_config(DRV_SFS_CLK_FREQ_HZ_DEF, DRV_SF_WIDTH_1LINE, DRV_SF_DELAY_DEFAULT);
}

/**
 * @brief mbr_get_app(), get the address and length of app partition during boot
 * @note  don't toggle the pins of xflash, unless there is no app in iflash
 * @note  if partition is valid, it will boost flash at new clock and data width.
 *
 * @param[in] p_addr  the pointer to the address of app partition in byte
 * @param[in] p_len   the pointer to the length  of app partition in byte
 *
 * @return errno or chip select
 **/
int mbr_get_app(uint32_t *p_addr, uint32_t *p_len, uint16_t *p_crc)
{
    int err;
    uint32_t addr, len;
    uint8_t mbr_in_iflash = 0;

    err = drv_sfs_probe(DRV_SFS_IFLASH, DRV_SFS_CLK_FREQ_HZ_DEF);
    if (err >= 0)
    {
        /* iflash is present */
        err = mbr_probe(false /*create_if_fail*/);
        if (err >= 0)
        {
            mbr_in_iflash = 1;
            /* try app with iflash's MBR */
            err = mbr_read_part(PART_TYPE_APP, &addr, &len, p_crc);
            if (err >= 0)
            {
                /* try improve clock and data width */
                mbr_boost_clock();
                err = mbr_validate_app(addr, len);
                if (err >= 0)
                {
                    err = DRV_SFS_IFLASH;
                    goto ret_ok;
                }
            }
        }
    }
    OM_LOG_DEBUG("probe XFLASH\n");
    err = drv_sfs_probe(DRV_SFS_XFLASH, DRV_SFS_CLK_FREQ_HZ_DEF);
    if (err >= 0)
    {
        /* xflash is present */
        err = mbr_probe(false /*create_if_fail*/);
        if (err >= 0)
        {
            /* try app with xflash's MBR */
            err = mbr_read_part(PART_TYPE_APP, &addr, &len, p_crc);
            if (err >= 0)
            {
                /* try improve clock and data width */
                mbr_boost_clock();
                err = mbr_validate_app(addr, len);
                if (err >= 0)
                {
                    err = DRV_SFS_XFLASH;
                    goto ret_ok;
                }
            }
        }
    }

    /* restore to the flash in case of patch in iflash */
    if (mbr_in_iflash)
        drv_sfs_select(DRV_SFS_IFLASH);
    return -ENOEXEC;

ret_ok:
    if (p_addr)
        *p_addr = addr;
    if (p_len)
        *p_len = len;
    return err;
}

#ifdef CONFIG_PATCH_PACKAGE_ON_FLASH
/**
 * @brief mbr_get_patch(), get the address and length of patch partition during boot
 * @note  don't toggle the pins of xflash, unless there is no patch in iflash
 * @note  don't boost flash at new clock and data width.
 *
 * @param[in] p_addr  the pointer to the address of patch partition in byte
 * @param[in] p_len   the pointer to the length  of patch partition in byte
 *
 * @return errno or chip select
 **/
int mbr_get_patch(uint32_t *p_addr, uint32_t *p_len, uint16_t *p_crc)
{
    int err;
    uint32_t addr, len;
    uint8_t mbr_in_iflash = 0;

    err = drv_sfs_probe(DRV_SFS_IFLASH, DRV_SFS_CLK_FREQ_HZ_DEF);
    if (err >= 0)
    {
        /* iflash is present */
        err = mbr_probe(false /*create_if_fail*/);
        if (err >= 0)
        {
            mbr_in_iflash = 1;
            /* try patch with iflash's MBR */
            err = mbr_read_part(PART_TYPE_PATCH, &addr, &len, p_crc);
            if (err >= 0)
            {
                err = DRV_SFS_IFLASH;
                goto ret_ok;
            }
        }
    }
    /* don't toggle xflash's pins if app in iflash */
    if (mbr_in_iflash)
        return -ENOEXEC;

    err = drv_sfs_probe(DRV_SFS_XFLASH, DRV_SFS_CLK_FREQ_HZ_DEF);
    if (err >= 0)
    {
        /* xflash is present */
        err = mbr_probe(false /*create_if_fail*/);
        if (err >= 0)
        {
            /* try patch with xflash's MBR */
            err = mbr_read_part(PART_TYPE_PATCH, &addr, &len, p_crc);
            if (err >= 0)
            {
                err = DRV_SFS_XFLASH;
                goto ret_ok;
            }
        }
    }

    /* restore to the flash in case of cfg in iflash */
    if (mbr_in_iflash)
        drv_sfs_select(DRV_SFS_IFLASH);
    return -ENOEXEC;

ret_ok:
    if (p_addr)
        *p_addr = addr;
    if (p_len)
        *p_len = len;
    return err;
}
#endif

#if 0
/**
 * @brief get cpft address and length
 *
 * @param[out] p_addr  cpft base address
 * @param[out] p_len  cpft length
 *
 * @return errno or 0 success
 **/
int mbr_get_cpft(uint32_t *p_addr, uint32_t *p_len)
{
    uint32_t capacity = drv_sfs_capacity();

    if(capacity == 0)
      return -ENOEXEC;

    if(p_len)
      *p_len = DRV_SF_SECTOR_SIZE;
    if(p_addr)
      *p_addr = capacity - DRV_SF_SECTOR_SIZE;

    return 0;
}

/**
 * @brief get free space address and length
 *
 * @param[out] p_addr  free space address
 * @param[out] p_len  freee space length
 *
 * @return errno or 0 success
 **/
int mbr_get_free_area(uint32_t *p_addr, uint32_t *p_len)
{
    int err;
    uint32_t app_base, app_len;
    uint32_t patch_base, patch_len;
    uint32_t free_base, free_limit, free_len;
    uint32_t sector_mask;
    uint16_t crc;
    int capacity = drv_sfs_capacity();

    if(capacity == 0)
        return -ENOEXEC;

    err = mbr_read_part(PART_TYPE_APP, &app_base, &app_len, &crc);
    if(err)
        free_base = APP_SECTOR_START<<DRV_SF_SECTOR_SHIFT;
    else
        free_base = app_base + app_len;

    err = mbr_read_part(PART_TYPE_PATCH, &patch_base, &patch_len, &crc);
    if(err)
    {
        uint32_t cfg_base, cfg_len;
        err = mbr_read_part(PART_TYPE_CFG, &cfg_base, &cfg_len, &crc);
        if(err)
        {
            uint32_t cpft_base, cpft_len;
            err = mbr_get_cpft(&cpft_base, &cpft_len);
            if(err)
                free_limit = capacity;
            else
                free_limit = cpft_base;
        }
        else
        {
            free_limit = cfg_base;
        }
    }
    else
    {
        free_limit = patch_base;
    }
    free_len = free_limit - free_base;

    sector_mask = DRV_SF_SECTOR_SIZE - 1;

    *p_addr = (free_base + sector_mask) & ~sector_mask;
    *p_len = free_len & ~sector_mask;

    return 0;
}
#endif
