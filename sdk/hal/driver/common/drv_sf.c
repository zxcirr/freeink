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
 * @file     drv_sf.c
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
#include "RTE_OM662x.h"
#if (RTE_SF) && (!RTE_SF_USING_ROM_SYMBOL)
#include "om_driver.h"
#include "om_utils.h"

/*********************************************************************
 * MACROS
 */
#define SPI_CMD_WREN        0x06u /* Write enable */
#define SPI_CMD_WRDI        0x04u /* Write disable */
#define SPI_CMD_RDSR        0x05u /* Read status register */
#define SPI_CMD_RDSR2       0x35u /* Read status register (high byte) */
#define SPI_CMD_WRSR        0x01u /* Write status register */
#define SPI_CMD_READ        0x03u /* Read data bytes (low frequency) */
#define SPI_CMD_FAST_READ   0x0Bu /* Read data bytes (high frequency) */
#define SPI_CMD_DUAL_READ   0x3Bu /* Dual output fast read: HS6620 doesn't support 0xBB */
#define SPI_CMD_QUAD_READ   0x6Bu /* Quad output fast read: PN25F04C doesn't support 0x6B, but HS6620 doesn't support 0xEB */
#define SPI_CMD_PP          0x02u /* Page program (up to page in 256 bytes) */
#define SPI_CMD_RDID        0x9fu /* Read JEDEC ID */
#define SPI_CMD_RDUID       0x4Bu /* Read UID */
#define SPI_CMD_SE          0x20u /* Sector erase (usually 4KiB) */
#define SPI_CMD_BE_32K      0x52u /* Erase 32KiB block */
#define SPI_CMD_BE_64K      0xD8u /* Erase 64KiB block */
#define SPI_CMD_CE          0x60u /* Erase whole flash chip, or 0xC7 */
#define SPI_CMD_ENDP        0xB9u /* Enter Deep Power-Down */
#define SPI_CMD_EXDP        0xABu /* Exit Deep Power-Down */
#define SPI_CMD_RDSEC       0x48u /* Read Security Registers */
#define SPI_CMD_ERSEC       0x44u /* Erase Security Registers */
#define SPI_CMD_PPSEC       0x42u /* Program Security Registers */
#define SPI_CMD_SUSPEND     0x75u /* Program/Erase Suspend */
#define SPI_CMD_RESUME      0x7Au /* Program/Erase Resume */
#define SPI_CMD_EQPI        0x38u /* Enable Quad Peripheral Interface: HS6620 doesn't support */

#define SR_WIP              0x0001 /* Write in progress */
#define SR_WEL              0x0002 /* Write enable latch */
#define SR_BP0              0x0004 /* Block protect 0 */
#define SR_BP1              0x0008 /* Block protect 1 */
#define SR_BP2              0x0010 /* Block protect 2 */
#define SR_BP3              0x0020 /* Block protect 3 */
#define SR_BP4              0x0040 /* Block protect 4 */
#define SR_SRP0             0x0080 /* SR write protect 0 */
#define SR_SRP1             0x0100 /* SR write protect 1 */
#define SR_QE               0x0200 /* Quad enable */
#define SR_SUS2             0x0400 /* Suspend status 2, bad compatibility */
#define SR_LB1              0x0800 /* Lock OTP 1, bad compatibility */
#define SR_LB2              0x1000 /* Lock OTP 2, bad compatibility */
#define SR_LB3              0x2000 /* Lock OTP 3, bad compatibility */
#define SR_CMP              0x4000 /* Block protect: conjunction the BP4-BP0 */
#define SR_SUS1             0x8000 /* Suspend status 1 */

#define DRV_SF_READ_FAST_DMA_CLOCK_FREQ_HZ_THRESHOLD    20000000 // 20MHz

//#define DRV_SF_WORKAROUND_PUYA_FLASH_OVERWRITE_ISSUE
//#define DRV_SF_NOT_MODIFY_IFLASH_LAST_SECTOR
#define DRV_SF_NOT_MODIFY_IFLASH_4TH_SECTOR

#define DRV_SF_RW_PARAM_CMD_DECLARE(name, cmd, data, len) \
    drv_sfb_rw_params_t name = {{{((cmd)<<24), 0}}, 8, (data), (len)}
#define DRV_SF_RW_PARAM_CMD_ADDR_DECLARE(name, cmd, addr, data, len) \
    drv_sfb_rw_params_t name = {{{((cmd)<<24)|(addr), 0}}, 32, (data), (len)}
#define DRV_SF_RW_PARAM_CMD_ADDR_40BITS_DECLARE(name, cmd, addr, data, len) \
    drv_sfb_rw_params_t name = {{{((cmd)<<24)|(addr), 0}}, 40, (data), (len)}

#define IFLASH_INFO (&drv_sf_env.info[0][0])

#define DRV_SF_WRITE_BEGIN(sf, cs)                      do{ uint32_t irq_save_is_disabled = drv_sf_write_begin(sf, cs)
#define DRV_SF_WRITE_NODMA_END(sf, cs, param)           drv_sf_write_end(sf, cs, &param, irq_save_is_disabled, false/*dma*/, true/*suspend*/); } while(0)
#define DRV_SF_WRITE_DMA_END(sf, cs, param)             drv_sf_write_end(sf, cs, &param, irq_save_is_disabled, true/*dma*/, true/*suspend*/); } while(0)
#define DRV_SF_WRITE_NODMA_NOSUSPEND_END(sf, cs, param) drv_sf_write_end(sf, cs, &param, irq_save_is_disabled, false/*dma*/, false/*suspend*/); } while(0)

/*********************************************************************
 * TYPEDEFS
 */
typedef enum
{
    DRV_SF_MID_NONE        = 0,

    DRV_SF_MID_PUYA        = 0x85,    /* Puya */
    DRV_SF_MID_BOYA_0      = 0xE0,    /* Boya */
    DRV_SF_MID_BOYA_1      = 0x68,    /* Boya */
    DRV_SF_MID_TONGXIN     = 0xEB,    /* Tongxin */
    DRV_SF_MID_XTX         = 0x0B,    /* XTX */
    DRV_SF_MID_ALLIANCE    = 0x52,    /* Alliance Semiconductor */
    DRV_SF_MID_AMD         = 0x01,    /* AMD */
    DRV_SF_MID_AMIC        = 0x37,    /* AMIC */
    DRV_SF_MID_ATMEL       = 0x1F,    /* Atmel (now used by Adesto) */
    DRV_SF_MID_BRIGHT      = 0xAD,    /* Bright Microelectronics */
    DRV_SF_MID_CATALYST    = 0x31,    /* Catalyst */
    DRV_SF_MID_ESMT        = 0x8C,    /* Elite Semiconductor Memory Technology (ESMT) / EFST Elite Flash Storage */
    DRV_SF_MID_EON         = 0x1C,    /* EON, missing 0x7F prefix */
    DRV_SF_MID_EXCEL       = 0x4A,    /* ESI, missing 0x7F prefix */
    DRV_SF_MID_FIDELIX     = 0xF8,    /* Fidelix */
    DRV_SF_MID_FUJITSU     = 0x04,    /* Fujitsu */
    DRV_SF_MID_GIGADEVICE  = 0xC8,    /* GigaDevice */
    DRV_SF_MID_GIGADEVICE_XD = 0x50,  /* GigaDevice */
    DRV_SF_MID_GIGADEVICE_MD = 0x51,  /* GigaDevice */
    DRV_SF_MID_HYUNDAI     = 0xAD,    /* Hyundai */
    DRV_SF_MID_IMT         = 0x1F,    /* Integrated Memory Technologies */
    DRV_SF_MID_INTEL       = 0x89,    /* Intel */
    DRV_SF_MID_ISSI        = 0xD5,    /* ISSI Integrated Silicon Solutions, see also PMC. */
    DRV_SF_MID_MACRONIX    = 0xC2,    /* Macronix (MX) */
    DRV_SF_MID_NANTRONICS  = 0xD5,    /* Nantronics, missing prefix */
    DRV_SF_MID_PMC         = 0x9D,    /* PMC, missing 0x7F prefix */
    DRV_SF_MID_SANYO       = 0x62,    /* Sanyo */
    DRV_SF_MID_SHARP       = 0xB0,    /* Sharp */
    DRV_SF_MID_SPANSION    = 0x01,    /* Spansion, same ID as AMD */
    DRV_SF_MID_SST         = 0xBF,    /* SST */
    DRV_SF_MID_ST          = 0x20,    /* ST / SGS/Thomson / Numonyx (later acquired by Micron) */
    DRV_SF_MID_SYNCMOS_MVC = 0x40,    /* SyncMOS (SM) and Mosel Vitelic Corporation (MVC) */
    DRV_SF_MID_TI          = 0x97,    /* Texas Instruments */
    DRV_SF_MID_WINBOND_NEX = 0xEF,    /* Winbond (ex Nexcom) serial flashes */
    DRV_SF_MID_WINBOND     = 0xDA,    /* Winbond */
}drv_sf_mid_type_t;

typedef struct
{
    uint32_t freq_hz;
    drv_sf_width_t width;
    drv_sf_mid_type_t mid;  // Manufacturer ID
    uint8_t mtid;           // Memory Type ID
    uint8_t cid;            // Capacity ID (shift value)
    drv_sf_status_t status;
}drv_sf_info_t;

typedef struct
{
    // Power on delay
    uint32_t iflash_extra_open_delay_10us;
    // is critical sf suspend enabled
    bool is_critical_sf_suspend_enabled;
    // info
    drv_sf_info_t info[DRV_SFB_MODULE_NUM][DRV_SFB_CS_NUM];
}drv_sf_env_t;

typedef void (* drv_sf_rw_func_t)(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length);

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static drv_sf_env_t drv_sf_env = {0};

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief  sf is iflash
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *
 * @return is iflash ?
 **/
static bool drv_sf_is_iflash(OM_SF_Type *sf, uint32_t cs)
{
    return sf==OM_SF && cs==0;
}

/**
 * @brief  sf write begin
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *
 * @return
 **/
static uint32_t drv_sf_write_begin(OM_SF_Type *sf, uint32_t cs)
{
    uint32_t irq_save_is_disabled = 0;
    bool is_critical_sf = (drv_sfb_critical_object_get()==sf) && (drv_sfb_critical_cs_get() == cs);

    if(is_critical_sf)
        OM_CRITICAL_BEGIN_EX(irq_save_is_disabled);
        // OM_ENTER_CRITICAL_EXCEPT_EX(RTE_SF_BUSY_ALLOW_IRQ_PRIORIT, param);

    drv_sf_write_enable(sf, cs);

    return irq_save_is_disabled;
}


/**
 *******************************************************************************
 * @brief  drv sf write end
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] param  param
 * @param[in] irq_save_is_disabled  irq save is disabled
 * @param[in] is_dma  is dma
 * @param[in] is_allow_suspend  is allow suspend
 *******************************************************************************
 */
__RAM_CODE
static void drv_sf_write_end(OM_SF_Type *sf, uint32_t cs, drv_sfb_rw_params_t *param, uint32_t irq_save_is_disabled, bool is_dma, bool is_allow_suspend)
{
    bool is_critical_sf = (drv_sfb_critical_object_get()==sf) && (drv_sfb_critical_cs_get() == cs);

    if (is_dma)
        drv_sfb_write_dma(sf, cs, param);
    else
        drv_sfb_write_nodma(sf, cs, param);

    if (drv_sf_env.is_critical_sf_suspend_enabled && is_critical_sf && is_allow_suspend)
        drv_sf_wait_sr_no_busy_with_suspend(sf, cs, irq_save_is_disabled);
    else
        drv_sf_wait_sr_no_busy(sf, cs);

    if(is_critical_sf)
        OM_CRITICAL_END_EX(irq_save_is_disabled);
        //OM_EXIT_CRITICAL_EXCEPT_EX(irq_save_is_disabled);
}

/**
 * @brief  sf low voltage detection enable
 *
 * @param[in] enable  enable
 **/
static void drv_sf_iflash_low_voltage_detection_enable(bool enable)
{
    // Enable low voltage reset
    if (enable)
        OM_PMU->FLASH_LOW_VOL_CTRL_0 = 0x6666;

    // !!! REMVOE follow code to prevent: turn off abnormally !!!
    //else
    //    OM_PMU->FLASH_LOW_VOL_CTRL_1 = 0x9999;
}

/**
 *******************************************************************************
 * @brief  drv sf iflash auto powerdown in sleep enable
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
static void drv_sf_iflash_auto_send_deep_powerdown_in_sleep_enable(bool enable)
{
    // Digital BUG: must be enabled, otherwise wakeup pinmux can not be restored
    enable = true;

    // Default enable iflash auto send deep power down ctrl in sleep
    //   RDI: 0=10us 1=20us 2=30us 3=60us
    //   DP:  0=5us  1=10us 2=20us 3=30us
    REGW(&OM_PMU->SLEEP_WAKE_CTRL, MASK_4REG(PMU_SLEEP_WAKE_CTRL_SF_RDI_EN,enable?1:0, PMU_SLEEP_WAKE_CTRL_SF_DP_EN,enable?1:0,
                PMU_SLEEP_WAKE_CTRL_SF_RDI_WAIT_CTRL,2, PMU_SLEEP_WAKE_CTRL_SF_DP_WAIT_CTRL,0));
}

/**
 *******************************************************************************
 * @brief  drv sf iflash power enable
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
static void drv_sf_iflash_power_enable(bool enable)
{
    if(enable)
    {
        if (!(OM_PMU->SW_STATUS & PMU_SW_STATUS_FLASH_OPEND_MASK))
        {
            bool delayed = false;

            // sf controller will not power off flash in sleep (immediate depend on PMU_ANA_PD_PD_FLASH_ME)
            REGW0(&OM_PMU->ANA_PD, PMU_ANA_PD_PD_FLASH_REG_MASK);
            // flag
            REGW1(&OM_PMU->SW_STATUS, PMU_SW_STATUS_FLASH_OPEND_MASK);
            // check
            while(!(OM_PMU->FLASH_LOW_VOL_CTRL_0 & PMU_FLASH_LOW_VOL_CTRL_0_FLASH_POWER_READY_SYNC_MASK));

            if (IFLASH_INFO->status == DRV_SF_STATUS_PRESENT)
            {
                switch(IFLASH_INFO->mid)
                {
                    case DRV_SF_MID_PUYA:
                    case DRV_SF_MID_TONGXIN:
                        if (IFLASH_INFO->mtid == 0x60)
                        {
                            // tVSL = 70us
                            DRV_DELAY_US(10 * 20);
                            delayed = true;
                        }
                        break;

                    default:
                        break;
                }
            }

            // worst case (tVSL, tPUW)
            if (delayed)
                DRV_DELAY_US(10 * drv_sf_env.iflash_extra_open_delay_10us);
            else
                DRV_DELAY_MS(10);
        }
    }
    else
    {
        // flag
        REGW0(&OM_PMU->SW_STATUS, PMU_SW_STATUS_FLASH_OPEND_MASK);
    }
}

/**
 *******************************************************************************
 * @brief  drv sf setup default setting
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *******************************************************************************
 */
static void drv_sf_iflash_setup_default_setting(OM_SF_Type *sf, uint32_t cs)
{
    bool is_auto_pd = true;

    if (IFLASH_INFO->status == DRV_SF_STATUS_PRESENT)
    {
        // In OM6626, HW wakeup flow: from PD=0 to SW-run is about 167us
        switch(IFLASH_INFO->mid)
        {
            // tVSL=1ms (GD25WQ80E)
            case DRV_SF_MID_GIGADEVICE:
                is_auto_pd = false;
                break;

            // tVSL=150us (P25Q40SU)
            case DRV_SF_MID_PUYA:
            default:
                break;
        }
    }

    // enable auto powerdown or auto deeppowerdown
    drv_sf_iflash_auto_powerdown_in_sleep_enable(is_auto_pd);
    // enable low voltage detection
    drv_sf_iflash_low_voltage_detection_enable(true);
    // auto suspend enable
    drv_sf_auto_suspend_enable(true);
}

/**
 *******************************************************************************
 * @brief  drv sf quad bus read enable
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
__RAM_CODE
static void drv_sf_quad_bus_read_enable(OM_SF_Type *sf, uint32_t cs, bool enable)
{
    if (enable)
        sf->READ_OPCODE_REG = (SPI_CMD_QUAD_READ<<8) | SPI_CMD_QUAD_READ;
    else
        sf->READ_OPCODE_REG = (SPI_CMD_DUAL_READ<<8) | SPI_CMD_DUAL_READ;
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief read sr reg
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return sr value
 **/
__RAM_CODE
uint8_t drv_sf_read_sr(OM_SF_Type *sf, uint32_t cs)
{
    uint8_t sr = 0;
    //DRV_SF_RW_PARAM_CMD_DECLARE(param, SPI_CMD_RDSR, &sr, 1);
    drv_sfb_rw_params_t param;
    param.cmd[0] = SPI_CMD_RDSR << 24;
    param.cmd[1] = 0;
    param.cmd_bits = 8;
    param.data = &sr;
    param.data_bytes = 1;
    drv_sfb_read_nodma(sf, cs, &param);
    return sr;
}

/**
 * @brief read sr2
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return sr2 value
 **/
__RAM_CODE
uint8_t drv_sf_read_sr2(OM_SF_Type *sf, uint32_t cs)
{
    uint8_t sr2 = 0;
    //DRV_SF_RW_PARAM_CMD_DECLARE(param, SPI_CMD_RDSR2, &sr2, 1);
    drv_sfb_rw_params_t param;
    param.cmd[0] = SPI_CMD_RDSR2 << 24;
    param.cmd[1] = 0;
    param.cmd_bits = 8;
    param.data = &sr2;
    param.data_bytes = 1;
    drv_sfb_read_nodma(sf, cs, &param);
    if (sr2 == 0xFF) sr2 = 0;
    return sr2;
}

/**
 * @brief read sr 16bits
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return sr | (sr2<<8)
 **/
uint16_t drv_sf_read_sr_16bits(OM_SF_Type *sf, uint32_t cs)
{
    return drv_sf_read_sr(sf, cs) | (drv_sf_read_sr2(sf, cs) << 8);
}

/**
 *******************************************************************************
 * @brief  drv sf is suspended
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *
 * @return  suspended?
 *******************************************************************************
 */
__RAM_CODE
bool drv_sf_is_suspended(OM_SF_Type *sf, uint32_t cs)
{
    uint16_t sr = drv_sf_read_sr2(sf, cs) << 8;

    return (sr & SR_SUS1) ? true : false;
}

/**
 *******************************************************************************
 * @brief  drv sf is busy
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *
 * @return  is busy ?
 *******************************************************************************
 */
__RAM_CODE
bool drv_sf_is_busy(OM_SF_Type *sf, uint32_t cs)
{
    return (drv_sf_read_sr(sf, cs) & SR_WIP) ? true : false;
}

/**
 * @brief wait sr no busy
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
__RAM_CODE
void drv_sf_wait_sr_no_busy(OM_SF_Type *sf, uint32_t cs)
{
    while (drv_sf_is_busy(sf, cs))
        DRV_DELAY_US(100);
}

/**
 *******************************************************************************
 * @brief  drv sf wait sr no busy with suspend
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] irq_save_is_disabled  irq save is disabled
 *******************************************************************************
 */
__RAM_CODE
void drv_sf_wait_sr_no_busy_with_suspend(OM_SF_Type *sf, uint32_t cs, uint32_t irq_save_is_disabled)
{
    om_error_t ret;

    while (drv_sf_is_busy(sf, cs))
    {
        // Delay 100us
        DRV_WAIT_US_UNTIL_TO(!(drv_irq_is_any_ext_pending() && !irq_save_is_disabled), 100, ret);

        // is pending
        if (ret == OM_ERROR_OK)
        {
            OM_ASSERT(drv_irq_is_any_ext_pending());

            // suspend
            drv_sf_suspend(sf, cs);
            // like call __enable_irq()
            OM_CRITICAL_END_EX(irq_save_is_disabled);

            // ENTER irq

            // like call __disable_irq()
            OM_CRITICAL_BEGIN_EX(irq_save_is_disabled);
            // resume
            drv_sf_resume(sf, cs);
        }
    }
}

/**
 * @brief write enable
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
void drv_sf_write_enable(OM_SF_Type *sf, uint32_t cs)
{
    DRV_SF_RW_PARAM_CMD_DECLARE(param, SPI_CMD_WREN, NULL, 0);
    drv_sfb_write_nodma(sf, cs, &param);
}

/**
 * @brief write sr
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] sr  sr
 **/
void drv_sf_write_sr(OM_SF_Type *sf, uint32_t cs, uint8_t sr)
{
    if (OM_PMU->SW_STATUS & PMU_SW_STATUS_SCBOOT_IN_FLASH_MASK)
    {
        // MUST: BP0=1 CMP=0, to protect 1st 64kB (less than 2MB flash)
        sr |= SR_BP0;
    }

    DRV_SF_WRITE_BEGIN(sf, cs);

    DRV_SF_RW_PARAM_CMD_DECLARE(param, SPI_CMD_WRSR, &sr, 1);

    DRV_SF_WRITE_NODMA_NOSUSPEND_END(sf, cs, param);
}

/**
 * @brief write sr 16bits
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] sr  sr
 **/
void drv_sf_write_sr_16bits(OM_SF_Type *sf, uint32_t cs, uint16_t sr)
{
    if (OM_PMU->SW_STATUS & PMU_SW_STATUS_SCBOOT_IN_FLASH_MASK)
    {
        // MUST: BP0=1 CMP=0, to protect 1st 64kB (less than 2MB flash)
        sr |=  SR_BP0;
        sr &= ~SR_CMP;
    }

    DRV_SF_WRITE_BEGIN(sf, cs);

    DRV_SF_RW_PARAM_CMD_DECLARE(param, SPI_CMD_WRSR, &sr, 2);

    DRV_SF_WRITE_NODMA_NOSUSPEND_END(sf, cs, param);
}

/**
 * @brief write sr with mask
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] mask  sr mask
 * @param[in] value  sr value
 **/
void drv_sf_write_sr_mask(OM_SF_Type *sf, uint32_t cs, uint8_t mask, uint8_t value)
{
    uint8_t sr, sr2;

    sr = drv_sf_read_sr(sf, cs);
    sr2 = drv_sf_read_sr(sf, cs);

    // Two consecutive readings are the same
    while(sr != sr2)    //lint !e681
    {
        DRV_DELAY_US(10 * 10);
        sr = drv_sf_read_sr(sf, cs);
        sr2 = drv_sf_read_sr(sf, cs);
    }

    // write is dangerous
    value &= mask;
    if((sr & mask) != value)
        drv_sf_write_sr(sf, cs, (sr & ~mask) | value);
}

/**
 * @brief write 16bits sr with mask
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] mask  sr mask
 * @param[in] value  sr value
 **/
void drv_sf_write_sr_mask_16bits(OM_SF_Type *sf, uint32_t cs, uint16_t mask, uint16_t value)
{
    uint16_t sr, sr2;

    sr = drv_sf_read_sr_16bits(sf, cs);
    sr2 = drv_sf_read_sr_16bits(sf, cs);

    // Two consecutive readings are the same
    while(sr != sr2)
    {
        DRV_DELAY_US(10 * 10);
        sr = drv_sf_read_sr_16bits(sf, cs);
        sr2 = drv_sf_read_sr_16bits(sf, cs);
    }

    // write is dangerous
    value &= mask;
    if((sr & mask) != value)
        drv_sf_write_sr_16bits(sf, cs, (sr & ~mask) | value);
}

/**
 * @brief quad enable
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] enable  true or false
 **/
void drv_sf_quad_enable(OM_SF_Type *sf, uint32_t cs, bool enable)
{
    drv_sf_write_sr_mask_16bits(sf, cs, SR_QE, enable ? SR_QE : 0);
}

/**
 * @brief otp set
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] lb_mask  lb mask
 **/
void drv_sf_otp_set(OM_SF_Type *sf, uint32_t cs, uint8_t lb_mask)
{
    uint32_t sr;
    int drv_sf_index = drv_sfb_index(sf);
    drv_sf_mid_type_t mid = drv_sf_env.info[drv_sf_index][cs].mid;
    uint8_t mtid = drv_sf_env.info[drv_sf_index][cs].mtid;

    sr = drv_sf_read_sr_16bits(sf, cs);

    if (mid == DRV_SF_MID_GIGADEVICE && mtid == 0x40)
        sr |= (lb_mask & 0x1) << (2+8);
    else
        sr |= (lb_mask & 0x7) << (3+8);

    drv_sf_write_sr_16bits(sf, cs, sr);
}

/**
 * @brief otp get
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return lb_mask
 **/
uint8_t drv_sf_otp_get(OM_SF_Type *sf, uint32_t cs)
{
    uint32_t sr2 = drv_sf_read_sr2(sf, cs);
    int drv_sf_index = drv_sfb_index(sf);
    drv_sf_mid_type_t mid = drv_sf_env.info[drv_sf_index][cs].mid;
    uint8_t mtid = drv_sf_env.info[drv_sf_index][cs].mtid;

    if (mid == DRV_SF_MID_GIGADEVICE && mtid == 0x40)
        return (sr2 >> 2) & 0x1;
    else
        return (sr2 >> 3) & 0x7;
}

/**
 * @brief  sf lowpower enter
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 **/
void drv_sf_deep_power_down_enter(OM_SF_Type *sf, uint32_t cs)
{
    int drv_sf_index = drv_sfb_index(sf);
    bool delayed = false;

    // enter
    DRV_SF_RW_PARAM_CMD_DECLARE(param, SPI_CMD_ENDP, NULL, 0);
    drv_sfb_write_nodma(sf, cs, &param);

    // tDP
    if (drv_sf_env.info[drv_sf_index][cs].status == DRV_SF_STATUS_PRESENT)
    {
        switch(drv_sf_env.info[drv_sf_index][cs].mid)
        {
            case DRV_SF_MID_PUYA:
            case DRV_SF_MID_TONGXIN:
                // tDP = 3us
                DRV_DELAY_US(3*2);
                delayed = true;
                break;

            default:
                break;
        }
    }

    if(!delayed)
        DRV_DELAY_US(20);
}

/**
 * @brief  sf lowpower leave
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 **/
void drv_sf_deep_power_down_leave(OM_SF_Type *sf, uint32_t cs)
{
    uint8_t res;
    int drv_sf_index = drv_sfb_index(sf);
    bool delayed = false;

    // leave
    DRV_SF_RW_PARAM_CMD_ADDR_DECLARE(param, SPI_CMD_EXDP, 0, &res, 1);
    drv_sfb_write_nodma(sf, cs, &param);

    // tRES2
    if (drv_sf_env.info[drv_sf_index][cs].status == DRV_SF_STATUS_PRESENT)
    {
        switch(drv_sf_env.info[drv_sf_index][cs].mid)
        {
            case DRV_SF_MID_PUYA:
            case DRV_SF_MID_TONGXIN:
                // tDP = 8us
                DRV_DELAY_US(8*2);
                delayed = true;
                break;

            default:
                break;
        }
    }

    if(!delayed)
        DRV_DELAY_US(30);
}

/**
 *******************************************************************************
 * @brief  drv sf suspend
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *******************************************************************************
 */
__RAM_CODE
void drv_sf_suspend(OM_SF_Type *sf, uint32_t cs)
{
    // enter
    //DRV_SF_RW_PARAM_CMD_DECLARE(param, SPI_CMD_SUSPEND, NULL, 0);
    drv_sfb_rw_params_t param;
    param.cmd[0] = SPI_CMD_SUSPEND << 24;
    param.cmd[1] = 0;
    param.cmd_bits = 8;
    param.data = NULL;
    param.data_bytes = 0;
    drv_sfb_write_nodma(sf, cs, &param);

    // CS# High To Next Command After Suspend
    // - PUYA(0x856014):  tPSL = 30us
    // - GD(0xc86514):    tSUS = 40us
    DRV_DELAY_US(50);
}

/**
 *******************************************************************************
 * @brief  drv sf resume
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *******************************************************************************
 */
__RAM_CODE
void drv_sf_resume(OM_SF_Type *sf, uint32_t cs)
{
    // leave
    //DRV_SF_RW_PARAM_CMD_DECLARE(param, SPI_CMD_RESUME, NULL, 0);
    drv_sfb_rw_params_t param;
    param.cmd[0] = SPI_CMD_RESUME << 24;
    param.cmd[1] = 0;
    param.cmd_bits = 8;
    param.data = NULL;
    param.data_bytes = 0;
    drv_sfb_write_nodma(sf, cs, &param);

    // Latency between Program/Erase Resume and next Suspend
    // - PUYA(0x856014): tPRS = 20us
    // - GD(0xc86514):   tRS = 100us
    DRV_DELAY_US(110);
}

/**
 * @brief unlock all
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
void drv_sf_unlock_all(OM_SF_Type *sf, uint32_t cs)
{
    int drv_sf_index = drv_sfb_index(sf);
    drv_sf_mid_type_t mid = drv_sf_env.info[drv_sf_index][cs].mid;
    uint16_t bpx, cmp=0;

    /* cancel Complement Protect if set */
    switch(mid)
    {
        case DRV_SF_MID_GIGADEVICE:
        case DRV_SF_MID_BOYA_0:
        case DRV_SF_MID_WINBOND_NEX:
        case DRV_SF_MID_PUYA:
            cmp = 1 << 14;
            break;
        default:
            break;
    }

    /* cancel Block Protect if set */
    switch(mid)
    {
        case DRV_SF_MID_PUYA:
        case DRV_SF_MID_GIGADEVICE: bpx = 0x1F << 2; break;
        case DRV_SF_MID_EON:        bpx = 0x0F << 2; break;
        default:                bpx = 0x7  << 2; break;
    }

    if(cmp)
        drv_sf_write_sr_mask_16bits(sf, cs, cmp|bpx, 0);
    else
        drv_sf_write_sr_mask(sf, cs, bpx, 0);
}

/**
 * @brief read id
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return sflash id (24bits)
 **/
uint32_t drv_sf_read_id(OM_SF_Type *sf, uint32_t cs)
{
    uint32_t id = 0;
    DRV_SF_RW_PARAM_CMD_DECLARE(param, SPI_CMD_RDID, &id, 3);
    drv_sfb_read_nodma(sf, cs, &param);
    id = ((id & 0xFF0000) >> 16) | (id & 0x00FF00) | ((id & 0x0000FF) << 16);
    return id;
}

/**
 *******************************************************************************
 * @brief  drv sf read id safe
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 *
 * @return ID
 *******************************************************************************
 */
uint32_t drv_sf_read_id_safe(OM_SF_Type *sf, uint32_t cs)
{
    uint32_t id, id2;

    id = drv_sf_read_id(sf, cs);
    id2 = drv_sf_read_id(sf, cs);

    // Two consecutive readings are the same
    while(id != id2)
    {
        DRV_DELAY_US(10 * 10);
        id = drv_sf_read_id(sf, cs);
        id2 = drv_sf_read_id(sf, cs);
    }

    return id;
}

/**
 * @brief read uid
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] data  read uid buffer
 * @param[in] length  length
 **/
void drv_sf_read_uid_ex(OM_SF_Type *sf, uint32_t cs, void *data, uint32_t length)
{
    DRV_SF_RW_PARAM_CMD_ADDR_40BITS_DECLARE(param, SPI_CMD_RDUID, 0, data, length);
    drv_sfb_read_nodma(sf, cs, &param);
}

/**
 * @brief read uid
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return sflash UID
 **/
uint32_t drv_sf_read_uid(OM_SF_Type *sf, uint32_t cs)
{
    uint32_t uid, uidbuf[4];
    int drv_sf_index = drv_sfb_index(sf);

    switch (drv_sf_env.info[drv_sf_index][cs].mid)
    {
        // Puya: 128bit UID
        case DRV_SF_MID_PUYA:
        case DRV_SF_MID_TONGXIN:
        case DRV_SF_MID_GIGADEVICE:
            drv_sf_read_uid_ex(sf, cs, uidbuf, 16);
            uid = uidbuf[0] ^ uidbuf[1] ^ uidbuf[2] ^ uidbuf[3];
            break;

        // Boya: 64bit UID
        case DRV_SF_MID_BOYA_0:
        case DRV_SF_MID_BOYA_1:
            drv_sf_read_uid_ex(sf, cs, uidbuf, 8);
            uid = uidbuf[0] ^ uidbuf[1];
            break;

        default:
            uid = drv_sf_read_id(sf, cs);
            break;
    }

    return uid;
}

#if 0
/**
 * @brief erase chip
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
void drv_sf_erase_chip(OM_SF_Type *sf, uint32_t cs)
{
    return;

    DRV_SF_WRITE_BEGIN(sf, cs);

    DRV_SF_RW_PARAM_CMD_DECLARE(param, SPI_CMD_CE, NULL, 0);

    DRV_SF_WRITE_NODMA_END(sf, cs, param);
}
#endif

/**
 * @brief erase sector
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 **/
void drv_sf_erase_sector(OM_SF_Type *sf, uint32_t cs, uint32_t addr)
{
#ifdef DRV_SF_NOT_MODIFY_IFLASH_4TH_SECTOR
    if (drv_sf_is_iflash(sf, cs) && ((addr/DRV_SF_SECTOR_SIZE) == 3))
        return;
#endif

    DRV_SF_WRITE_BEGIN(sf, cs);

    DRV_SF_RW_PARAM_CMD_ADDR_DECLARE(param, SPI_CMD_SE, addr, NULL, 0);

    DRV_SF_WRITE_NODMA_END(sf, cs, param);
}

/**
 * @brief erase half block (32kB)
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 **/
void drv_sf_erase_half_block(OM_SF_Type *sf, uint32_t cs, uint32_t addr)
{
#ifdef DRV_SF_NOT_MODIFY_IFLASH_4TH_SECTOR
    if (drv_sf_is_iflash(sf, cs) && ((addr/DRV_SF_HALF_BLOCK_SIZE) == 0))
        return;
#endif

    DRV_SF_WRITE_BEGIN(sf, cs);

    DRV_SF_RW_PARAM_CMD_ADDR_DECLARE(param, SPI_CMD_BE_32K, addr, NULL, 0);

    DRV_SF_WRITE_NODMA_END(sf, cs, param);
}

/**
 * @brief erase block (64kB)
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 **/
void drv_sf_erase_block(OM_SF_Type *sf, uint32_t cs, uint32_t addr)
{
#ifdef DRV_SF_NOT_MODIFY_IFLASH_4TH_SECTOR
    if (drv_sf_is_iflash(sf, cs) && ((addr/DRV_SF_BLOCK_SIZE) == 0))
        return;
#endif

    DRV_SF_WRITE_BEGIN(sf, cs);

    DRV_SF_RW_PARAM_CMD_ADDR_DECLARE(param, SPI_CMD_BE_64K, addr, NULL, 0);

    DRV_SF_WRITE_NODMA_END(sf, cs, param);
}

/**
 * @brief erase sec
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 **/
void drv_sf_erase_sec(OM_SF_Type *sf, uint32_t cs, uint32_t addr)
{
    DRV_SF_WRITE_BEGIN(sf, cs);

    DRV_SF_RW_PARAM_CMD_ADDR_DECLARE(param, SPI_CMD_ERSEC, addr, NULL, 0);

    DRV_SF_WRITE_NODMA_END(sf, cs, param);
}

/**
 * @brief sf erase
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[in] length  length
 **/
void drv_sf_erase(OM_SF_Type *sf, uint32_t cs, uint32_t addr, uint32_t length)
{
    uint32_t capacity = drv_sf_capacity(sf, cs);
    uint32_t sector, sector_end;

    if(length == 0)
    {
        // Erase ALl Flash

        sector_end = capacity;

#ifdef DRV_SF_NOT_MODIFY_IFLASH_LAST_SECTOR
        if(drv_sf_is_iflash(sf, cs))
            sector_end = capacity - DRV_SF_SECTOR_SIZE;
#endif

        for (sector=DRV_SF_SECTOR_SIZE; sector<sector_end; sector+=DRV_SF_SECTOR_SIZE)
            drv_sf_erase_sector(sf, cs, sector);

        drv_sf_erase_sector(sf, cs, 0);
    }
    else
    {
        sector_end = addr + length;

        if (sector_end > capacity)
            sector_end = capacity;

#ifdef DRV_SF_NOT_MODIFY_IFLASH_LAST_SECTOR
        if(drv_sf_is_iflash(sf, cs))
            if (sector_end > capacity-DRV_SF_SECTOR_SIZE)
                sector_end = capacity-DRV_SF_SECTOR_SIZE;
#endif

        for (sector = addr & ~(DRV_SF_SECTOR_SIZE-1); sector<sector_end; sector+=DRV_SF_SECTOR_SIZE)
            drv_sf_erase_sector(sf, cs, sector);
    }
}

#if 0
/**
 * @brief write page without dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[in] data  write data
 * @param[in] length  length
 **/
void drv_sf_write_page_nodma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, const void *data, uint32_t length)
{
    OM_ASSERT(((addr & (DRV_SF_PAGE_SIZE-1)) + length) <= DRV_SF_PAGE_SIZE);

    DRV_SF_WRITE_BEGIN(sf, cs);

    DRV_SF_RW_PARAM_CMD_ADDR_DECLARE(param, SPI_CMD_PP, addr, data, length);

    DRV_SF_WRITE_NODMA_END(sf, cs, param);
}
#endif

/**
 * @brief write page with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[in] data  write data
 * @param[in] length  length
 **/
void drv_sf_write_page_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, const void *data, uint32_t length)
{
//    OM_ASSERT((length & 3) == 0);
//    OM_ASSERT(((uint32_t)data & 3) == 0);

    OM_ASSERT(((addr & (DRV_SF_PAGE_SIZE-1)) + length) <= DRV_SF_PAGE_SIZE);

    DRV_SF_WRITE_BEGIN(sf, cs);

    DRV_SF_RW_PARAM_CMD_ADDR_DECLARE(param, SPI_CMD_PP, addr, data, length);

    DRV_SF_WRITE_DMA_END(sf, cs, param);
}

/**
 * @brief write page
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[in] data  write data
 * @param[in] length  length
 **/
void drv_sf_write_page(OM_SF_Type *sf, uint32_t cs, uint32_t addr, const void *data, uint32_t length)
{
#ifdef DRV_SF_NOT_MODIFY_IFLASH_4TH_SECTOR
    if (drv_sf_is_iflash(sf, cs) && ((addr/DRV_SF_SECTOR_SIZE) == 3))
        return;
#endif

    drv_sf_write_page_dma(sf, cs, addr, data, length);
}

/**
 * @brief write sec
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[in] data  write data
 * @param[in] length  length
 **/
void drv_sf_write_sec(OM_SF_Type *sf, uint32_t cs, uint32_t addr, const void *data, uint32_t length)
{
    DRV_SF_WRITE_BEGIN(sf, cs);

    DRV_SF_RW_PARAM_CMD_ADDR_DECLARE(param, SPI_CMD_PPSEC, addr, data, length);

    DRV_SF_WRITE_NODMA_END(sf, cs, param);
}

/**
 * @brief sf write
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[in] data  write data
 * @param[in] length  length
 **/
void drv_sf_write(OM_SF_Type *sf, uint32_t cs, uint32_t addr, const void *data, uint32_t length)
{
    uint32_t page_offset;
    uint32_t tlen;
    uint32_t i;

    OM_ASSERT(data != NULL);
    OM_ASSERT(length != 0);

#ifdef DRV_SF_WORKAROUND_PUYA_FLASH_OVERWRITE_ISSUE
    // Workaround PUYA flash(0x856013,0x856014) BUG:
    // Same place write more than 1 times, some bit many from 0 to 1.
    // Only cover 1,2 bytes over-write
    uint8_t write_data[4];
    if(length < 3)
    {
        uint8_t flash_data[4];

        drv_sf_read_fast_dma(sf, cs, addr, flash_data, length);

        for(i=0; i<length; ++i)
            write_data[i] = ((uint8_t *)data)[i] & flash_data[i];

        data = write_data;
    }
#endif

#ifdef DRV_SF_NOT_MODIFY_IFLASH_LAST_SECTOR
    if(drv_sf_is_iflash(sf, cs))
    {
        uint32_t capacity = drv_sf_capacity(sf, cs);
        uint32_t last_sector = capacity - DRV_SF_SECTOR_SIZE;
        if(addr+length > last_sector)
        {
            if (addr >= last_sector)
                return;
            length = last_sector - addr;
        }
    }
#endif

    page_offset = addr & (DRV_SF_PAGE_SIZE - 1);
    tlen = DRV_SF_PAGE_SIZE - page_offset;

    if (tlen > length)
        tlen = length;

    drv_sf_write_page(sf, cs, addr, data, tlen);

    for (i=tlen; i<length; i+=DRV_SF_PAGE_SIZE)
    {
        tlen = length - i;
        if(tlen > DRV_SF_PAGE_SIZE)
            tlen = DRV_SF_PAGE_SIZE;
        drv_sf_write_page(sf, cs, addr+i, (uint8_t *)data+i, tlen);
    }
}

#if 0
/**
 * @brief read normal without dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_normal_nodma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length)
{
    DRV_SF_RW_PARAM_CMD_ADDR_DECLARE(param, SPI_CMD_READ, addr, data, length);
    drv_sfb_read_nodma(sf, cs, &param);
}

/**
 * @brief read normal with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_normal_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length)
{
    OM_ASSERT((length & 3) == 0);
    OM_ASSERT(((uint32_t)data & 3) == 0);

    DRV_SF_RW_PARAM_CMD_ADDR_DECLARE(param, SPI_CMD_READ, addr, data, length);
    drv_sfb_read_dma(sf, cs, &param);
}

/**
 * @brief read fast with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_fast_nodma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length)
{
    DRV_SF_RW_PARAM_CMD_ADDR_40BITS_DECLARE(param, SPI_CMD_FAST_READ, addr, data, length);
    drv_sfb_read_nodma(sf, cs, &param);
}
#endif

/**
 * @brief read fast with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_fast_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length)
{
//    OM_ASSERT((length & 3) == 0);
//    OM_ASSERT(((uint32_t)data & 3) == 0);

    DRV_SF_RW_PARAM_CMD_ADDR_40BITS_DECLARE(param, SPI_CMD_FAST_READ, addr, data, length);
    drv_sfb_read_dma(sf, cs, &param);
}

/**
 * @brief read fast 2line with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_fast_dual_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length)
{
//    OM_ASSERT((length & 3) == 0);
//    OM_ASSERT(((uint32_t)data & 3) == 0);

    DRV_SF_RW_PARAM_CMD_ADDR_40BITS_DECLARE(param, SPI_CMD_DUAL_READ, addr, data, length);
    drv_sfb_read_dma(sf, cs, &param);
}

/**
 * @brief read fast 4line with dma (Not auto enable quad mode, please call drv_sf_quad_enable outside)
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_fast_quad_dma(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length)
{
//    OM_ASSERT((length & 3) == 0);
//    OM_ASSERT(((uint32_t)data & 3) == 0);

    DRV_SF_RW_PARAM_CMD_ADDR_40BITS_DECLARE(param, SPI_CMD_QUAD_READ, addr, data, length);
    drv_sfb_read_dma(sf, cs, &param);
}

/**
 * @brief read sec
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read_sec(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length)
{
    DRV_SF_RW_PARAM_CMD_ADDR_40BITS_DECLARE(param, SPI_CMD_RDSEC, addr, data, length);
    drv_sfb_read_nodma(sf, cs, &param);
}

/**
 * @brief sf read
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] addr  sflash address
 * @param[out] data  read data buffer
 * @param[in] length  length
 **/
void drv_sf_read(OM_SF_Type *sf, uint32_t cs, uint32_t addr, void *data, uint32_t length)
{
    uint32_t once_dma_length;
    drv_sf_rw_func_t rw_dma_func;
    uint32_t src = addr;
    uint8_t * dst = data;
    int drv_sf_index = drv_sfb_index(sf);
    drv_sf_width_t width = drv_sf_env.info[drv_sf_index][cs].width;

    OM_ASSERT(data != NULL);
    OM_ASSERT(length != 0);

    switch(width)
    {
        case DRV_SF_WIDTH_2LINE: rw_dma_func = drv_sf_read_fast_dual_dma; break;
        case DRV_SF_WIDTH_4LINE: rw_dma_func = drv_sf_read_fast_quad_dma; break;
        default:                 rw_dma_func = drv_sf_read_fast_dma;      break;
    }

    while (length)
    {
        once_dma_length = length>DRV_SFB_DMA_DATA_LEN_MAX ? DRV_SFB_DMA_DATA_LEN_MAX : length;

        rw_dma_func(sf, cs, src, dst, once_dma_length);
        src += once_dma_length;
        dst += once_dma_length;

        length -= once_dma_length;
    }
}

/**
 * @brief sf config
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] config  sf config
 **/
void drv_sf_config(OM_SF_Type *sf, uint32_t cs, const drv_sf_config_t *config)
{
    drv_sfb_config_t sfbconfig;
    int drv_sf_index = drv_sfb_index(sf);
    uint8_t delay = config->delay;

    // config
    sfbconfig.transmode = DRV_SFB_SPI_MODE_0;
    sfbconfig.cs_pol = DRV_SFB_SPI_CS_LOW_ACTIVE;

    // auto check
    if (config->delay == DRV_SF_DELAY_AUTO)
    {
        uint32_t id_real, id;
        int delayi, delay1=-1, delay2=DRV_SF_DELAY_MAX;

        sfbconfig.freq_hz = 16; // 16div
        sfbconfig.delay = DRV_SF_DELAY_DEFAULT;
        drv_sfb_config(sf, cs, &sfbconfig);

        id_real = drv_sf_read_id_safe(sf, cs);

        for (delayi=0; delayi<=DRV_SF_DELAY_MAX; ++delayi)
        {
            sfbconfig.freq_hz = config->freq_hz;
            sfbconfig.delay = delayi;
            drv_sfb_config(sf, cs, &sfbconfig);

            id = drv_sf_read_id(sf, cs);

            if (id == id_real)
            {
                if (delay1 == -1)
                    delay1 = delayi;
                delay2 = delayi;
            }
            else
            {
                if (delay1 != -1)
                    break;
            }
        }

        delay = (delay1 + delay2) / 2;
    }

    // config
    sfbconfig.freq_hz = config->freq_hz;
    sfbconfig.delay = delay;
    drv_sfb_config(sf, cs, &sfbconfig);

    drv_sf_env.info[drv_sf_index][cs].width = config->width;
    drv_sf_env.info[drv_sf_index][cs].freq_hz = config->freq_hz;

    // Init quad read
    if (config->width == DRV_SF_WIDTH_4LINE)
    {
        drv_sf_quad_enable(sf, cs, true);
        drv_sf_quad_bus_read_enable(sf, cs, true);
    }
    else
    {
        drv_sf_quad_bus_read_enable(sf, cs, false);
    }
}

/**
 * @brief sf enable
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
void drv_sf_enable(OM_SF_Type *sf, uint32_t cs)
{
    // enable
    drv_sfb_enable(sf, cs);
    // power on
    if (drv_sf_is_iflash(sf, cs))
    {
        drv_sf_iflash_power_enable(true);
    }
}

/**
 * @brief  sf disable
 *
 * @param[in] sf  sf
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
void drv_sf_disable(OM_SF_Type *sf, uint32_t cs)
{
    // TODO: Better way to call drv_sfb_close()

    // power on
    if (drv_sf_is_iflash(sf, cs))
    {
        drv_sf_iflash_power_enable(false);
    }
}

/**
 *******************************************************************************
 * @brief  drv sf iflash disable power immediate
 *
 * @param[in] immediate  immediate
 *******************************************************************************
 */
void drv_sf_iflash_power_ctrl_immediate_enable(bool enable)
{
    REGW(&OM_PMU->ANA_PD, MASK_1REG(PMU_ANA_PD_PD_FLASH_ME, enable));
}

/**
 *******************************************************************************
 * @brief  drv sf iflash auto powerdown in sleep enable
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
void drv_sf_iflash_auto_powerdown_in_sleep_enable(bool enable)
{
    // sf controller will not power off flash in sleep (immediate depend on PMU_ANA_PD_PD_FLASH_ME)
    REGW(&OM_PMU->ANA_PD, MASK_1REG(PMU_ANA_PD_PD_FLASH_REG, enable?1:0));
    // sf controller will auto send deep powerdown command in sleep
    drv_sf_iflash_auto_send_deep_powerdown_in_sleep_enable(!enable);
}

/**
 * @brief  sf iflash open extra delay set
 *
 * @param[in] delay_10us  delay 10us
 **/
void drv_sf_iflash_extra_open_delay_set(uint32_t delay_10us)
{
    drv_sf_env.iflash_extra_open_delay_10us = delay_10us;
}

/**
 *******************************************************************************
 * @brief  drv sf iflash encrypt enable
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
void drv_sf_iflash_encrypt_enable(bool enable)
{
#if (RTE_EFUSE)
    // fetch key from efuse
    if (enable)
        drv_efuse_iflash_encrypt_uid_fetch();
#endif

    // enable
    OM_SF->CIPHER_CTRL = enable ? 3 : 0;
}

/**
 *******************************************************************************
 * @brief  drv sf suspend enable
 *
 * @param[in] enable  enable
 *******************************************************************************
 */
void drv_sf_auto_suspend_enable(bool enable)
{
    drv_sf_env.is_critical_sf_suspend_enabled = enable;
}

/**
 * @brief sf detect
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return is present ?
 **/
bool drv_sf_detect(OM_SF_Type *sf, uint32_t cs)
{
    int drv_sf_index = drv_sfb_index(sf);
    uint32_t id = drv_sf_read_id_safe(sf, cs);

    if(id == 0x00FFFFFF || id == 0)
    {
        drv_sf_env.info[drv_sf_index][cs].mid  = DRV_SF_MID_NONE;
        drv_sf_env.info[drv_sf_index][cs].mtid = 0;
        drv_sf_env.info[drv_sf_index][cs].cid  = 0;
        drv_sf_env.info[drv_sf_index][cs].status = DRV_SF_STATUS_ABSENT;
        return false;
    }
    else
    {
        drv_sf_env.info[drv_sf_index][cs].mid  = (drv_sf_mid_type_t)((id >> 16) & 0xFF);
        drv_sf_env.info[drv_sf_index][cs].mtid = (id >> 8)  & 0xFF;
        drv_sf_env.info[drv_sf_index][cs].cid  = (id >> 0)  & 0xFF;
        drv_sf_env.info[drv_sf_index][cs].status = DRV_SF_STATUS_PRESENT;

        // setup iflash default setting
        if (drv_sf_is_iflash(sf, cs))
            drv_sf_iflash_setup_default_setting(sf, cs);

        return true;
    }
}

/**
 * @brief sf status
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return status
 **/
drv_sf_status_t drv_sf_status(OM_SF_Type *sf, uint32_t cs)
{
    return drv_sf_env.info[drv_sfb_index(sf)][cs].status;
}

/**
 * @brief sf capacity
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return capacity
 **/
uint32_t drv_sf_capacity(OM_SF_Type *sf, uint32_t cs)
{
    int drv_sf_index = drv_sfb_index(sf);

    if (drv_sf_env.info[drv_sf_index][cs].cid)
        return 1u << drv_sf_env.info[drv_sf_index][cs].cid;

    // Default is 1MB
    return 1 * 1024 * 1024;
}

/**
 * @brief sf get id
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return sflash id (saved by @ref drv_sf_detect)
 **/
uint32_t drv_sf_id(OM_SF_Type *sf, uint32_t cs)
{
    int drv_sf_index = drv_sfb_index(sf);
    return (drv_sf_env.info[drv_sf_index][cs].mid  << 16) |
           (drv_sf_env.info[drv_sf_index][cs].mtid <<  8) |
           (drv_sf_env.info[drv_sf_index][cs].cid  <<  0);
}

#endif  /* (RTE_SF) */
/** @} */

