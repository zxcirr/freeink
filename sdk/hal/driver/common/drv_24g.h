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
 * @file     drv_24g.h
 * @brief    OM24G Driver
 * @date     24. July 2021
 * @author   OnMicro SW Team
 *
 * @defgroup OM24G OM24G
 * @ingroup  DRIVER
 * @brief    Private 24G protocol
 * @details  Private 24G protocol
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __OM24G_H__
#define __OM24G_H__

/*********************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_OM24G)
#include <stdint.h>
#ifdef __cplusplus

extern "C"
{
#endif

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */

/// OM24G PACKET STRUCTURE
typedef enum {
    /// PACKET STRUCTURE A 
    OM24G_STRUCTURE_A = 0,
    /// PACKET STRUCTURE B
    OM24G_STRUCTURE_B = 1,
} om24g_structure_t;

/// OM24G SYNCWORD SEL
typedef enum {
    /// Select sync word 0 
    OM24G_SYNCWORD0 = 0,
    /// Select sync word 1
    OM24G_SYNCWORD1 = 1,
} om24g_syncword_t;

/// OM24G transfer mode
typedef enum {
    /// RX mode
    OM24G_ROLE_PRX,
    /// TX mode
    OM24G_ROLE_PTX,
} om24g_role_t;

/// OM24G DEVIATION
typedef enum {
    /// 62.5K TX frequency offset
    OM24G_DEVIATION_62P5K,
    /// 125K TX frequency offset
    OM24G_DEVIATION_125K,
    /// 250K TX frequency offset
    OM24G_DEVIATION_250K,
    /// 500K TX frequency offset
    OM24G_DEVIATION_500K,
} om24g_deviation_t;

/// OM24G RATE
typedef enum {
    /// 250K data rate
    OM24G_RATE_250K,
    /// 500K data rate
    OM24G_RATE_500K,
    /// 1M data rate
    OM24G_RATE_1M,
    /// 2M data rate
    OM24G_RATE_2M,
    /// 100K data rate
    OM24G_RATE_100K,
    /// 125K data rate
    OM24G_RATE_125K,
    /// 50K data rate
    OM24G_RATE_50K,
    /// 25K data rate
    OM24G_RATE_25K,
} om24g_rate_t;

/// OM24G CRC_BYTE
typedef enum {
    /// 1 byte CRC
    OM24G_CRC_1BYTE,
    /// 2 byte CRC
    OM24G_CRC_2BYTE,
    /// 3 byte CRC
    OM24G_CRC_3BYTE,
    /// 4 byte CRC
    OM24G_CRC_4BYTE,
} om24g_crc_byte_t;

/// OM24G MODULATION MODE
typedef enum {
    /// GFSK MODULATION MODE
    OM24G_MODULATION_GFSK = 0,
    /// FSK MODULATION MODE
    OM24G_MODULATION_FSK = 1,
} om24g_modulation_t;

/// OM24G ENDIAN
typedef enum {
    /// LSB BYTE FIRST
    OM24G_ENDIAN_LSB = 0,
    /// MSB BYTE FIRST
    OM24G_ENDIAN_MSB = 1,
} om24g_endian_t;

/// OM24G DEMODULATION MODE
typedef enum {
    /// Software demodulation 
    OM24G_SOFT_DETECTION,
    /// Hardware demodulation
    OM24G_HARD_DETECTION,
    /// Dpll demodulation
    OM24G_DPLL_DETECTION,
} om24g_detect_mode_t;

/// OM24G config
typedef struct {
    /// tx data pointer
    uint8_t                 *tx_data;
    /// rx data pointer
    uint8_t                 *rx_data;
    // select package structure A or B
    om24g_structure_t       packet_struct_sel;
    /// preamble content
    uint8_t                 preamble;
    /// preamble lenth
    uint8_t                 preamble_len;
    /// select sync word0 or word1 to transmit package
    om24g_syncword_t        sync_word_sel;
    /// sync word0
    uint32_t                sync_word0;
    /// sync word1
    uint32_t                sync_word1;
    /// tx pipe address
    uint8_t                 tx_addr;
    /// rx pipe0 address
    uint8_t                 rx_addr;
    /// If it is 1, check if rx pipe address matches, and if it is 0, do not check.
    uint8_t                 addr_chk;
    /// The number of bits in the header.
    uint8_t                 hdr_bits;
    /// In fixed length mode, it is the length of the receiving and sending. In dynamic length mode, it is the maximum packet length that can be sent.
    uint16_t                static_len;
    /// The number of bits in rx pipe address.
    uint8_t                 addr1_bits;
    /// The number of bits in lenth.
    uint8_t                 len_bits;
    /// The location of rx pipe address.
    uint8_t                 addr1_pos;
    /// The location of rx pipe address(after/in/before header).
    uint8_t                 addr1_loc;
    /// The location of lenth.
    uint8_t                 len_pos;
    /// MSB first or LSB first transmit.
    om24g_endian_t          endian;
    /// The channel for transmitting frequency points.
    uint16_t                freq;
    /// air date rate.
    om24g_rate_t            data_rate;
    /// enable ack mode.
    uint8_t                 ack_en;
    /// Enable dynamic length.
    uint8_t                 dpl_en;
    /// Enable whitening.
    uint8_t                 white_en;
    /// 1: Do not whiten the header.
    uint8_t                 white_skip_hdr;
    /// 1: Do not whiten the pipe address.
    uint8_t                 white_skip_addr;
    /// 1: Do not whiten CRC.
    uint8_t                 white_skip_crc;
    /// select whiten mode(0~7) to Adapt to different chips.
    uint8_t                 white_sel;
    /// white seed value.
    uint16_t                white_seed;
    /// white out bit.
    uint8_t                 white_obit;
    /// lenth of crc.
    om24g_crc_byte_t        crc_len;
    /// Enable crc.
    uint8_t                 crc_en;
    /// select crc mode.
    uint8_t                 crc_mode;
    /// CRC Polynomial
    uint32_t                crc_poly;
    /// CRC initital value.
    uint32_t                crc_init;
    /// 1: CRC does not verify synchronization words.
    uint8_t                 crc_skip_sync;
    /// 1: CRC does not verify lenth.
    uint8_t                 crc_skip_len;
    /// 1: CRC does not verify pipe address.
    uint8_t                 crc_skip_addr;
    /// Select GFSK or FSK.
    om24g_modulation_t      modulation_mode;
    /// Select modulation mode.
    om24g_detect_mode_t     detect_mode;
} om24g_config_t;

/// OM24G PIPE
typedef enum {
    /// pipe 0
    OM24G_PIPE0,
    /// pipe 1
    OM24G_PIPE1,
    /// pipe 2
    OM24G_PIPE2,
    /// pipe 3
    OM24G_PIPE3,
    /// pipe 4
    OM24G_PIPE4,
    /// pipe 5
    OM24G_PIPE5,
    /// pipe 6
    OM24G_PIPE6,
    /// pipe 7
    OM24G_PIPE7,
    /// pipe tx
    OM24G_PIPE_TX,
} om24g_pipe_t;

/// om24g control
typedef enum {
    OM24G_CONTROL_CLK_DISABLE      = 0U,                 /**< Disable clock OM24G controller, argu is bool, return OM_ERROR_OK */
    OM24G_CONTROL_CLK_ENABLE       = 1U,                 /**< Enable clock OM24G controller, argu is bool, return OM_ERROR_OK */
    OM24G_CONTROL_RESET            = 2U,                 /**< Reseet OM24G controller, argu is bool, return OM_ERROR_OK */
    OM24G_CONTROL_SWITCH_ROLE      = 3U,                 /**< rx/tx mode switch, argu is om24g_role_t, return OM_ERROR_OK */
    OM24G_CONTROL_DUMP_RF_REGISTER = 4U,                 /**< Dump 2.4G rf register, argu is NULL, return OM_ERROR_OK */
} om24g_control_t;

/// om24g transfer case
typedef enum {
    OM24G_SENSITIVITY,
    OM24G_POLL_STRUCTURE_A,
    OM24G_INT_STRUCTURE_A,
    OM24G_INT_STRUCTURE_B,
    OM24G_ACK_MODE_STRUCTURE_A,
    OM24G_INT_ACK_MODE_STRUCTURE_A,
    OM24G_TX_RX_SWITCH_POLL,
    OM24G_TX_RX_SWITCH_INT,
    OM24G_RX_ADDR_CHECK,
} om24g_transfer_t;

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*********************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief set RX  check_address value.
 *
 * @param[in] *pipenum          pipe nummber.
 * @param[in] *addr             rx address.
 *******************************************************************************
 */
extern void om24g_set_addr(uint8_t pipenum, uint8_t addr);

/**
 *******************************************************************************
 * @brief Switch between transmitting and receiving devices..
 *
 * @param[in] role  OM24G_ROLE_PRX or OM24G_ROLE_PTX.
 *******************************************************************************
 */
extern void om24g_switch_role(om24g_role_t role);

/**
 *******************************************************************************
 * @brief Set the 2.4G data rate us. Please use function om24g_set_rf_parameters to set the rate, om24g_set_rate did not configure the frequency offset.
 *
 * @param[in] data_rate       data rate.
 * @param[in] en_arb          Enable low speed mode. if is true, rx_rate = data_rate / n_avr, tx_rate = data_rate / n_rep.
 * @param[in] n_avr           Receive rate division value.
 * @param[in] n_rep           Transmission rate division value.
 *******************************************************************************
 */
extern void om24g_set_rate(uint8_t data_rate, bool en_arb, uint8_t n_avr, uint8_t n_rep);

/**
 *******************************************************************************
 * @brief Set transmission rate, frequency point.
 * The speed only allows the use of speeds listed in enumeration om24g_rate_t in drv_24g.h. Please do not configure them arbitrarily. 
 * For special requirements, please contact the developer.
 * 
 * @param[in] data_rate         tx/rx data rate.
 * @param[in] frequency         Integer frequency point.
 * @param[in] fract_freq        Decimal frequency point.
 *******************************************************************************
 */
extern void om24g_set_rf_parameters(om24g_rate_t data_rate, uint16_t frequency, float fract_freq);

/**
 *******************************************************************************
 * @brief Set demodulation mode.
 *
 * @param[in] detc_mode   OM24G_SOFT_DETECTION，OM24G_HARD_DETECTION or OM24G_DPLL_DETECTION.
 *******************************************************************************
 */
extern void om24g_detection_mode(uint8_t detc_mode);

/**
 *******************************************************************************
 * @brief Obtain the rssi value..
 *
 * @return rssi_value
 *******************************************************************************
 */
extern int8_t om24g_get_rssi(void);

/**
 *******************************************************************************
 * @brief init the 2.4g transerive when the event begin.
 * If the deviation is dynamically switched, it is prohibited to repeatedly call om24g_init.
 * 
 * @param[in] cfg          2.4G register configuration.
 *******************************************************************************
 */
extern void om24g_init(const om24g_config_t *cfg);

/**
 *******************************************************************************
 * @brief Registering 2.4G Callbacks In interrupt mode.
 *
 * @param[in] event_cb     call back
 *******************************************************************************
 */
extern void om24g_register_event_callback(drv_isr_callback_t event_cb);

/**
 *******************************************************************************
 * @brief Set 2.4G frequency point.
 *
 * @param[in]  freq_channel      Integer frequency point
 * @param[in]  fract_freq         Decimal frequency point, If set freq to 2402.123456MHZ, freq_channel = 2402, fractFreq = 0.123456
 *******************************************************************************
 */
extern void om24g_set_freq(uint16_t freq_channel, float fract_freq);

/**
 *******************************************************************************
 * @brief sending data to transmit FIFO by polling mode.
 *
 * @param[in]  tx_payload      Data to be written to transmit FIFO
 * @param[in]  tx_num      Length of payload
 *
 * @return true:  send success
 *         false: send fali, Trigger maximum retransmission interrupt
 *******************************************************************************
 */
extern bool om24g_write(const uint8_t *tx_payload, uint16_t tx_num);

/**
 *******************************************************************************
 * @brief sending data to transmit FIFO by interrupt mode.
 *
 * @param[in]  tx_payload       Data to be written to transmit FIFO
 * @param[in]  tx_num      Number of bytes to write
 *******************************************************************************
 */
extern void om24g_write_int(const uint8_t *tx_payload, uint16_t tx_num);

/**
 *******************************************************************************
 * @brief Receive data from receive FIFO by polling mode.
 *
 * @param[in] rx_payload      Data read from receive buffer
 * @param[in] timeout_ms      rx timeout value
 *
 * @return Length of data received
 *******************************************************************************
 */
extern uint16_t om24g_read(uint8_t *rx_payload, uint32_t timeout_ms);

/**
 *******************************************************************************
 * @brief Receive data from receive FIFO by interrupt mode. The received data and length can be obtained from the callback function.
  
 * @param[in] rx_payload      Data read from receive buffer
 * @param[in] max_rx_num      The maximum number of receives for this application.

 *******************************************************************************
 */
extern void om24g_read_int(uint8_t *rx_payload, uint16_t max_rx_num);

/**
 *******************************************************************************
 * @brief Receive ack data from receive FIFO.
 *
 *  @return Length of data received.
 *******************************************************************************
 */
extern uint16_t om24g_read_ack(void);

/**
 *******************************************************************************
 * @brief write ack data to transmit FIFO.
 *
 * @param[in]  tx_num       Number of bytes to write
 *******************************************************************************
 */
__STATIC_FORCEINLINE void om24g_write_ack(uint8_t tx_num)
{
    OM_24G->DMA_TX_LEN = tx_num;
}

/**
 *******************************************************************************
 * @brief om24g interrupt service routine
 *******************************************************************************
 */
extern void drv_om24g_isr(void);

/**
 *******************************************************************************
 * @brief Control the OM24G
 *  Before starting RF work, please turn on the clock in the following order:drv_pmu_ana_enable(true, PMU_ANA_RF_24G); om24g_control(OM24G_CONTROL_CLK_ENABLE, NULL);
 *  After completing RF work, please turn off the clock in the following order:om24g_control(OM24G_CONTROL_CLK_DISABLE, NULL); drv_pmu_ana_enable(false, PMU_ANA_RF_24G); 
 * 
 * @param[in] control        Control options
 * @param[in] argu           argument for control options, Not used, always set NULL
 *
 * @return                   status:
 *                           execution_status
 *******************************************************************************
 */
extern void *om24g_control(om24g_control_t control, void *argu);

/**
 *******************************************************************************
 * @brief Check if the TX is idle
 *
 * @return   TRUE: IDLE  FALSE: BUSY
 *******************************************************************************
 */
extern bool om24g_tx_idle(void);

/**
 *******************************************************************************
 * @brief Check if the RX is idle
 *
 * @return   TRUE: IDLE  FALSE: BUSY
 *******************************************************************************
 */
extern bool om24g_rx_idle(void);
#ifdef __cplusplus
}
#endif

#endif  /*RTE_OM24G*/
#endif /*__OM24G_H*/

/** @} */