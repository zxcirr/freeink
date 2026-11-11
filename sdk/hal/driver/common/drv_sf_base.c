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
 * @file     drv_sf_base.c
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
#if (RTE_SF_BASE) && (!RTE_SF_BASE_USING_ROM_SYMBOL)
#include "om_driver.h"

/*********************************************************************
 * MACROS
 */
#define DRV_SFB_DONE                        0x01
#define DRV_SFB_CMD_NONE                    0
#define DRV_SFB_CMD_READ                    1
#define DRV_SFB_CMD_WRITE                   2
#define DRV_SFB_NODMA_WRITE_DATA_LEN_MAX    8
#define DRV_SFB_NODMA_READ_DATA_LEN_MAX     4

#define DIV_ROUND_UP(n,d)     (((n) + (d) - 1) / (d))

/*********************************************************************
 * TYPEDEFS
 */
typedef struct __spi_cmd_t
{
    uint32_t cmd:2;
    uint32_t cs:1;
    uint32_t lcd2lane:1;
    uint32_t keepCs:1;
    uint32_t cmdBits:7;
    uint32_t dataBytes:20;
} spi_cmd_t;

typedef struct __sfb_env_t
{
    // critical object
    OM_SF_Type *critical_obj;
    uint32_t    critical_cs;

    // callback
    drv_sfb_callback_t callback[DRV_SFB_MODULE_NUM];

    // 2lane mode for LCD
    uint8_t lcd2lane[DRV_SFB_MODULE_NUM][DRV_SFB_CS_NUM];
}drv_sfb_env_t;

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */
static drv_sfb_env_t drv_sfb_env = {
#ifdef CONFIG_XIP_FLASH_ALL
    .critical_obj = OM_SF,
    .critical_cs = 0,
#else
    .critical_obj = NULL,
    .critical_cs = 0,
#endif
    /* ... */
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/**
 * @brief  sfb irqn
 *
 * @param[in] sf  sf
 *
 * @return IRQn
 **/
__STATIC_FORCEINLINE IRQn_Type drv_sfb_irqn(OM_SF_Type *sf)
{
    return SF_IRQn;
}

/**
 * @brief  sfb memcopy reverse
 *
 * @param[in] dst  dst
 * @param[in] src  src
 * @param[in] n  n
 **/
__STATIC_FORCEINLINE void drv_sfb_memcopy_reverse(uint8_t *dst, const uint8_t *src, uint32_t n)
{
    uint32_t i;
    for (i=0; i<n; ++i)
        dst[n-1-i] = src[i];
}

/**
 * @brief  sfb cpm
 *
 * @param[in] sf  sf
 *
 * @return cpm
 **/
__STATIC_FORCEINLINE __IO uint32_t *drv_sfb_cpm(OM_SF_Type *sf)
{
    return &OM_CPM->SF_CFG;
}

/**
 * @brief  sfb clk
 *
 * @param[in] sf  sf
 *
 * @return clk
 **/
__STATIC_FORCEINLINE uint32_t drv_sfb_clk(OM_SF_Type *sf)
{
    return drv_rcc_clock_get(RCC_CLK_SF);
}


/**
 * @brief drv_sfb_is_opened()
 *
 * @param[in] sf  sf object
 *
 * @return
 **/
__STATIC_FORCEINLINE bool drv_sfb_is_opened(OM_SF_Type *sf)
{
    return (*drv_sfb_cpm(sf) & CPM_SF_GATE_EN_MASK) ? false : true;
}

/**
 * @brief drv_sfb_process_nonblock()
 *
 * @param[in] sf  sf object
 * @param[in] spi_cmd
 * @param[in] cmd
 * @param[in] data
 *
 * @return
 **/
__RAM_CODE
static void drv_sfb_process_nonblock(OM_SF_Type *sf, spi_cmd_t *spi_cmd, uint32_t cmd[2], void *data)
{
    int drv_sf_index = drv_sfb_index(sf);

    // check
    OM_ASSERT((sf->INTR_MASK & DRV_SFB_DONE) == 0);

    // Prevent sleep
    pm_sleep_prevent((pm_id_t)(PM_ID_SF0 + drv_sf_index));

    // Clear and Enable done IQR
    sf->RAW_INTR_STATUS = DRV_SFB_DONE;
    sf->INTR_MASK = DRV_SFB_DONE;

    // ctrl
    sf->COMMAND_DATA0_REG = cmd[0];
    sf->COMMAND_DATA1_REG = cmd[1];
    sf->ADDRESS_REG = (uint32_t)data;
    sf->COMMAND = *(uint32_t *)spi_cmd;
}

/**
 * @brief  sfb process block
 *
 * @param[in] sf  sf
 * @param[in] spi_cmd  spi cmd
 * @param[in] cmd cmd
 * @param[in] data  data
 **/
__RAM_CODE
static void drv_sfb_process_block(OM_SF_Type *sf, spi_cmd_t *spi_cmd, uint32_t cmd[2], void *data)
{
    uint32_t irq_save = 0;
    bool is_critical_obj = (drv_sfb_env.critical_obj == sf);
    bool is_critical_cs = (drv_sfb_env.critical_cs == spi_cmd->cs);

    // check
    OM_ASSERT((sf->INTR_MASK & DRV_SFB_DONE) == 0);

    // critical entry
    if(is_critical_obj)
        OM_CRITICAL_BEGIN_EX(irq_save);

    // ctrl
    sf->COMMAND_DATA0_REG = cmd[0];
    sf->COMMAND_DATA1_REG = cmd[1];
    sf->ADDRESS_REG = (uint32_t)data;
    sf->COMMAND = *(uint32_t *)spi_cmd;

    // wait done
    while(!(sf->RAW_INTR_STATUS & DRV_SFB_DONE));
    sf->RAW_INTR_STATUS = DRV_SFB_DONE;

    // restore cs
    if (is_critical_obj && !is_critical_cs)
    {
        spi_cmd_t critical_spi_cmd;
        *((uint32_t *)(&critical_spi_cmd)) = 0;
        critical_spi_cmd.cs = drv_sfb_env.critical_cs;
        sf->COMMAND = *(uint32_t *)&critical_spi_cmd;
    }

    // critical exit
    if(is_critical_obj)
        OM_CRITICAL_END_EX(irq_save);
}

/**
 * @brief drv_sfb_raw_rdata()
 *
 * @param[in] sf  sf object
 * @param[in] data
 *
 * @return
 **/
__STATIC_FORCEINLINE void drv_sfb_raw_rdata(OM_SF_Type *sf, uint32_t data[2])
{
    data[0] = sf->READ0_REG;
//    data[1] = sf->READ1_REG;
}

/**
 * @brief drv_sfb_raw_rdata0()
 *
 * @param[in] sf  sf object
 * @param[in] data
 *
 * @return
 **/
__STATIC_FORCEINLINE void drv_sfb_raw_rdata0(OM_SF_Type *sf, uint32_t data[1])
{
    data[0] = sf->READ0_REG;
}

/**
 * @brief drv sfb read_nodma_1cmd_le3data
 *
 * more efficiency than drv_sfb_read_nodma_common
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] param  rw param
 **/
__RAM_CODE
static void drv_sfb_read_nodma_1cmd_le3data(OM_SF_Type *sf, uint32_t cs, drv_sfb_rw_params_t *param)
{
    spi_cmd_t spi_cmd;
    uint32_t rdata[1];

    OM_ASSERT(cs < DRV_SFB_CS_NUM);

    OM_ASSERT(param->cmd_bits == 8);
    OM_ASSERT(param->data_bytes <= 3);

    // cmd
    *((uint32_t *)(&spi_cmd)) = 0;
    spi_cmd.cmdBits = param->cmd_bits + (param->data_bytes << 3);
    spi_cmd.dataBytes = 0;
    spi_cmd.cmd = DRV_SFB_CMD_READ;
    spi_cmd.cs = cs;

    // send cmd
    drv_sfb_process_block(sf, &spi_cmd, param->cmd, NULL);

    // copy result
    drv_sfb_raw_rdata0(sf, rdata);

    // convert
    drv_sfb_memcopy_reverse((void *)param->data, (void *)rdata, param->data_bytes);
}

/**
 * @brief drv_sfb_read_nodma()
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] param  rw param
 *
 * @return
 **/
__RAM_CODE
static void drv_sfb_read_nodma_common(OM_SF_Type *sf, uint32_t cs, drv_sfb_rw_params_t *param)
{
    spi_cmd_t spi_cmd;
    uint32_t rdata[2], wdata[2] = {0}, i, i4x;
    uint32_t cur_data_bytes, cur_data_bytes_4align;
    uint32_t left_data_bytes = param->data_bytes;
    uint32_t index_data_bytes = 0;

    OM_ASSERT(cs < DRV_SFB_CS_NUM);

    uint32_t irq_save = 0;
    if(drv_sfb_env.critical_obj == sf)
        OM_CRITICAL_BEGIN_EX(irq_save);

    // cmd
    *((uint32_t *)(&spi_cmd)) = 0;
    spi_cmd.cmdBits = param->cmd_bits;
    spi_cmd.dataBytes = 0;
    spi_cmd.cmd = DRV_SFB_CMD_READ;
    spi_cmd.cs = cs;
    spi_cmd.keepCs = left_data_bytes ? 1 : 0;

    // send cmd
    drv_sfb_process_block(sf, &spi_cmd, param->cmd, NULL);

    // read data
    while(left_data_bytes)
    {
        cur_data_bytes = (left_data_bytes > DRV_SFB_NODMA_READ_DATA_LEN_MAX) ? DRV_SFB_NODMA_READ_DATA_LEN_MAX : left_data_bytes;
        left_data_bytes -= cur_data_bytes;

        // new cmd
        spi_cmd.cmdBits = cur_data_bytes << 3;
        spi_cmd.keepCs = left_data_bytes ? 1 : 0;

        // read data
        drv_sfb_process_block(sf, &spi_cmd, wdata, NULL);

        // copy result
        drv_sfb_raw_rdata(sf, rdata);

        // save
        cur_data_bytes_4align = cur_data_bytes >> 2;
        for (i=0,i4x=0; i4x<cur_data_bytes_4align; ++i4x,i+=4)
            drv_sfb_memcopy_reverse(&((uint8_t *)param->data)[index_data_bytes + i], &((uint8_t *)rdata)[i], 4);
        drv_sfb_memcopy_reverse(&((uint8_t *)param->data)[index_data_bytes + i], &((uint8_t *)rdata)[i], cur_data_bytes - i);

        index_data_bytes += cur_data_bytes;
    }

    if(drv_sfb_env.critical_obj == sf)
        OM_CRITICAL_END_EX(irq_save);
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief read with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] keep_cs  keep cs
 * @param[in] param  rw param
 **/
void drv_sfb_read_dma_ex(OM_SF_Type *sf, uint32_t cs, drv_sfb_keep_cs_t keep_cs, drv_sfb_rw_params_t *param)
{
    spi_cmd_t spi_cmd;
    int drv_sf_index = drv_sfb_index(sf);

    OM_ASSERT(cs < DRV_SFB_CS_NUM);

    OM_ASSERT(param->data_bytes <= DRV_SFB_DMA_DATA_LEN_MAX);

    // cmd
    *((uint32_t *)(&spi_cmd)) = 0;
    spi_cmd.cmdBits = (keep_cs==DRV_SFB_CS_BEGIN||keep_cs==DRV_SFB_CS_NOKEEP) ? param->cmd_bits : 0;
    spi_cmd.dataBytes = param->data_bytes;
    spi_cmd.cmd = DRV_SFB_CMD_READ;
    spi_cmd.cs = cs;
    spi_cmd.keepCs = (keep_cs==DRV_SFB_CS_END||keep_cs==DRV_SFB_CS_NOKEEP) ? 0 : 1;

    // op
    if(drv_sfb_env.callback[drv_sf_index])
        drv_sfb_process_nonblock(sf, &spi_cmd, param->cmd, (void *)param->data);
    else
        drv_sfb_process_block(sf, &spi_cmd, param->cmd, (void *)param->data);
}

/**
 * @brief read with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] param  rw param
 **/
void drv_sfb_read_dma(OM_SF_Type *sf, uint32_t cs, drv_sfb_rw_params_t *param)
{
    drv_sfb_read_dma_ex(sf, cs, DRV_SFB_CS_NOKEEP, param);
}

/**
 * @brief write with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] keep_cs  keep cs
 * @param[in] param  rw param
 **/
__RAM_CODE
void drv_sfb_write_dma_ex(OM_SF_Type *sf, uint32_t cs, drv_sfb_keep_cs_t keep_cs, drv_sfb_rw_params_t *param)
{
    spi_cmd_t spi_cmd;
    int drv_sf_index = drv_sfb_index(sf);

    OM_ASSERT(cs < DRV_SFB_CS_NUM);

    OM_ASSERT(param->data_bytes <= DRV_SFB_DMA_DATA_LEN_MAX);
//    OM_ASSERT(((uint32_t)param->data & 3) == 0);

    // cmd
    *((uint32_t *)(&spi_cmd)) = 0;
    spi_cmd.cmdBits = (keep_cs==DRV_SFB_CS_BEGIN||keep_cs==DRV_SFB_CS_NOKEEP) ? param->cmd_bits : 0;
    spi_cmd.dataBytes = param->data_bytes;
    spi_cmd.cmd = DRV_SFB_CMD_WRITE;
    spi_cmd.cs = cs;
    spi_cmd.keepCs = (keep_cs==DRV_SFB_CS_END||keep_cs==DRV_SFB_CS_NOKEEP) ? 0 : 1;
    spi_cmd.lcd2lane = drv_sfb_env.lcd2lane[drv_sf_index][cs];

    // op
    if(drv_sfb_env.callback[drv_sf_index])
        drv_sfb_process_nonblock(sf, &spi_cmd, param->cmd, (void *)param->data);
    else
        drv_sfb_process_block(sf, &spi_cmd, param->cmd, (void *)param->data);
}

/**
 * @brief write with dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] param  rw param
 **/
__RAM_CODE
void drv_sfb_write_dma(OM_SF_Type *sf, uint32_t cs, drv_sfb_rw_params_t *param)
{
    drv_sfb_write_dma_ex(sf, cs, DRV_SFB_CS_NOKEEP, param);
}

/**
 * @brief read without dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] param  rw param
 **/
__RAM_CODE
void drv_sfb_read_nodma(OM_SF_Type *sf, uint32_t cs, drv_sfb_rw_params_t *param)
{
    OM_ASSERT(cs < DRV_SFB_CS_NUM);

    if (param->cmd_bits == 8 && param->data_bytes <= 3)
        drv_sfb_read_nodma_1cmd_le3data(sf, cs, param); // more efficiency
    else
        drv_sfb_read_nodma_common(sf, cs, param);
}

/**
 * @brief write without dma
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] param  rw param
 **/
__RAM_CODE
void drv_sfb_write_nodma(OM_SF_Type *sf, uint32_t cs, drv_sfb_rw_params_t *param)
{
    spi_cmd_t spi_cmd;
    uint32_t wdata[2], i;
    uint32_t cur_data_bytes;
    uint32_t left_data_bytes = param->data_bytes;
    uint32_t index_data_bytes = 0;

    OM_ASSERT(cs < DRV_SFB_CS_NUM);

    uint32_t irq_save = 0;
    if(drv_sfb_env.critical_obj == sf)
        OM_CRITICAL_BEGIN_EX(irq_save);

    // cmd
    *((uint32_t *)(&spi_cmd)) = 0;
    spi_cmd.cmdBits = param->cmd_bits;
    spi_cmd.dataBytes = 0;
    spi_cmd.cmd = DRV_SFB_CMD_WRITE;
    spi_cmd.cs = cs;
    spi_cmd.keepCs = left_data_bytes ? 1 : 0;

    // send cmd
    drv_sfb_process_block(sf, &spi_cmd, param->cmd, NULL);

    // read data
    while(left_data_bytes)
    {
        cur_data_bytes = (left_data_bytes > DRV_SFB_NODMA_WRITE_DATA_LEN_MAX) ? DRV_SFB_NODMA_WRITE_DATA_LEN_MAX : left_data_bytes;
        left_data_bytes -= cur_data_bytes;

        // save
        for (i=0; i<cur_data_bytes; ++i)
            ((uint8_t *)wdata)[(i&~3)+3-(i%4)] = ((uint8_t *)param->data)[index_data_bytes + i];    //lint !e661
        index_data_bytes += cur_data_bytes;

        // new cmd
        spi_cmd.cmdBits = cur_data_bytes << 3;
        spi_cmd.keepCs = left_data_bytes ? 1 : 0;

        // read data
        drv_sfb_process_block(sf, &spi_cmd, wdata, NULL);
    }

    if(drv_sfb_env.critical_obj == sf)
        OM_CRITICAL_END_EX(irq_save);
}

/**
 * @brief open
 *
 * @param[in] sf  sf object
 **/
void drv_sfb_open(OM_SF_Type *sf)
{
    register_set(drv_sfb_cpm(sf), MASK_3REG(CPM_SF_SOFT_RESET,1, CPM_SF_DIV_SEL,0, CPM_SF_GATE_EN,0));
}

/**
 * @brief close
 *
 * @param[in] sf  sf object
 **/
void drv_sfb_close(OM_SF_Type *sf)
{
    register_set(drv_sfb_cpm(sf), MASK_3REG(CPM_SF_SOFT_RESET,0, CPM_SF_DIV_SEL,0, CPM_SF_GATE_EN,1));
}

/**
 * @brief config
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] config  config
 **/
__RAM_CODE
void drv_sfb_config(OM_SF_Type *sf, uint32_t cs, const drv_sfb_config_t *config)
{
    int drv_sf_index = drv_sfb_index(sf);
    uint32_t div = config->freq_hz<256 ? config->freq_hz : DIV_ROUND_UP(drv_sfb_clk(sf), config->freq_hz);

    OM_ASSERT(cs < DRV_SFB_CS_NUM);

    if (div > (DRV_SF_CTRL_CLOCK_DIV_MASK))
        div = DRV_SF_CTRL_CLOCK_DIV_MASK;
    div &= ~1; // must be even

    // Write REG
    REGWA(&sf->CONFIGURATION[cs].CTRL, MASK_7REG(
        DRV_SF_CTRL_LCD_RD_EN,      0,
        DRV_SF_CTRL_RGB_MODE,       0,
        DRV_SF_CTRL_LCD_SPI_CTRL,   0,
        DRV_SF_CTRL_WIDTH,          0,
        DRV_SF_CTRL_BP_CLOCK_DIV,   div<2 ? 1 : 0,
        DRV_SF_CTRL_MODE,           config->transmode,
        DRV_SF_CTRL_CLOCK_DIV,      div<2 ? 2 : div));

    // <32MHz=>2  >=32MHz=>5
    if (cs == 0)
        REGW(&sf->DELAY_CTRL, MASK_1REG(DRV_SF_DELAY_INNER, config->delay));
    else
        REGW(&sf->DELAY_CTRL, MASK_1REG(DRV_SF_DELAY_EXTERNAL, config->delay));

    // CS
    REGW(&sf->CONFIGURATION[cs].CS, MASK_1REG(DRV_SF_CS_POL, config->cs_pol));

    // Default disable all done irq
    sf->INTR_MASK = 0; /* disable all */
    sf->RAW_INTR_STATUS = DRV_SFB_DONE; /* clear */

    // default
    drv_sfb_env.lcd2lane[drv_sf_index][cs] = 0;
}

/**
 * @brief  sfb critical object set
 *
 * @param[in] sf  sf
 **/
void drv_sfb_critical_object_set(OM_SF_Type *sf)
{
    drv_sfb_env.critical_obj = sf;
}

/**
 * @brief  sfb critical object get
 *
 * @return obj
 **/
__RAM_CODE
OM_SF_Type *drv_sfb_critical_object_get(void)
{
    return drv_sfb_env.critical_obj;
}

/**
 * @brief  sfb critical object set
 *
 * @param[in] sf  sf
 **/
void drv_sfb_critical_cs_set(uint32_t cs)
{
    drv_sfb_env.critical_cs = cs;
}

/**
 * @brief  sfb critical object get
 *
 * @return obj
 **/
__RAM_CODE
uint32_t drv_sfb_critical_cs_get(void)
{
    return drv_sfb_env.critical_cs;
}

/**
 * @brief drv_sfb_enable()
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 **/
void drv_sfb_enable(OM_SF_Type *sf, uint32_t cs)
{
    if(!drv_sfb_is_opened(sf))
        drv_sfb_open(sf);
}

/**
 *******************************************************************************
 * @brief  drv sfb isr
 *
 * @param[in] sf  sf
 * @param[in] drv_sf_index  sf index
 *******************************************************************************
 */
void drv_sfb_isr(OM_SF_Type *sf, uint32_t drv_sf_index)
{
    // check
    if(sf->RAW_INTR_STATUS & DRV_SFB_DONE)
    {
        // clear
        sf->RAW_INTR_STATUS = DRV_SFB_DONE;

        // Default Disable IRQ
        sf->INTR_MASK = 0;

        // Allow sleep
        pm_sleep_allow((pm_id_t)(PM_ID_SF0 + drv_sf_index));

        // callback
        if(drv_sfb_env.callback[drv_sf_index])
            drv_sfb_env.callback[drv_sf_index](sf);
    }
}

/**
 * @brief set dma done event
 *
 * @param[in] sf  sf object
 * @param[in] callback  event callback
 **/
void drv_sfb_dma_done_event_register(OM_SF_Type *sf, drv_sfb_callback_t callback)
{
    IRQn_Type irqn = drv_sfb_irqn(sf);
    int drv_sf_index = drv_sfb_index(sf);

    drv_sfb_env.callback[drv_sf_index] = callback;

    if (callback)
    {
        NVIC_ClearPendingIRQ(irqn);
        NVIC_SetPriority(irqn, RTE_SF_BASE_IRQ_PRIORITY);
        NVIC_EnableIRQ(irqn);
    }
    else
    {
        NVIC_DisableIRQ(irqn);
    }
}

/**
 * @brief  sfb dma done event get
 *
 * @param[in] sf  sf
 *
 * @return callback
 **/
drv_sfb_callback_t drv_sfb_dma_done_event_get(OM_SF_Type *sf)
{
    return drv_sfb_env.callback[drv_sfb_index(sf)];
}

/**
 * @brief regs get
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 *
 * @return reg
 **/
uint32_t drv_sfb_regs_get(OM_SF_Type *sf, uint32_t cs)
{
    return sf->CONFIGURATION[cs].CTRL;
}

/**
 * @brief regs get
 *
 * @param[in] sf  sf object
 * @param[in] cs  cs (0 ~ DRV_SFB_CS_NUM-1)
 * @param[in] regs  reg
 **/
void drv_sfb_regs_set(OM_SF_Type *sf, uint32_t cs, uint32_t regs)
{
    sf->CONFIGURATION[cs].CTRL = regs;
}

/**
 * @brief  sfb lcd2lane enable
 *
 * @param[in] sf  sf
 * @param[in] cs  cs
 * @param[in] enable  enable
 **/
void drv_sfb_lcd2lane_enable(OM_SF_Type *sf, uint32_t cs, bool enable)
{
    int drv_sf_index = drv_sfb_index(sf);
    drv_sfb_env.lcd2lane[drv_sf_index][cs] = enable;
}

#endif  /* (RTE_SF_BASE) */
/** @} */

