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
 * @file     drv_pinmux.h
 * @brief    PINMUX Driver for om662x
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup PINMUX PINMUX
 * @ingroup  DRIVER
 * @brief    PINMUX Driver for om662x
 * @details  PINMUX Driver for om662x

 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __DRV_PINMUX_H
#define __DRV_PINMUX_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_PIN)
#include <stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
/// Number of pin
#define DRV_PIN_NUM                 26

/// Set Pinmux
#define DRV_PIN_MUX_SET(pin, func)  do {pin_func_t __pin_func = {func}; drv_pin_mux_set(pin, __pin_func); } while(0)


/*******************************************************************************
 * TYPEDEFS
 */
/// Pinmux Configuration Defination
typedef enum {
    PINMUX_JTAG_MODE_CFG                = 0,
    PINMUX_DEBUG_MODE_CFG               = 1,
    PINMUX_I2C0_SCK_CFG                 = 2,
    PINMUX_I2C0_SDA_CFG                 = 3,
    PINMUX_UART0_TX_CFG                 = 4,
    PINMUX_UART0_RX_CFG                 = 5,
    PINMUX_SFLASH_SI_CFG                = 8,
    PINMUX_SFLASH_SO_CFG                = 9,
    PINMUX_SFLASH_HD_CFG                = 10,
    PINMUX_SFLASH_WP_CFG                = 11,
    PINMUX_SFLASH_CK_CFG                = 12,
    PINMUX_SFLASH_CS_CFG                = 13,
    PINMUX_INPUT_MODE_CFG               = 14,   // analog input for ADC
    PINMUX_UART1_RX_CFG                 = 15,
    PINMUX_UART1_TX_CFG                 = 16,
    PINMUX_UART1_CTS_CFG                = 17,
    PINMUX_UART1_RTS_CFG                = 18,
    PINMUX_SPI0_DI_CFG                  = 19,
    PINMUX_SPI0_DO_CFG                  = 20,
    PINMUX_SPI0_CS_CFG                  = 21,
    PINMUX_SPI0_SCK_CFG                 = 22,
    PINMUX_SPI1_DI_CFG                  = 23,
    PINMUX_SPI1_DO_CFG                  = 24,
    PINMUX_SPI1_CS_CFG                  = 25,
    PINMUX_SPI1_SCK_CFG                 = 26,
    PINMUX_TX_EXT_PD_CFG                = 27,
    PINMUX_GPIO_MODE_CFG                = 28,
    PINMUX_RX_EXT_PD_CFG                = 29,
    PINMUX_PTA_ACTIVE_IN_CFG            = 30,
    PINMUX_PTA_ACTIVE_OUT_CFG           = 31,
    PINMUX_PTA_PRIORITY_CFG             = 32,
    PINMUX_PTA_FREQ_CFG                 = 33,
    PINMUX_TIMER0_ETR_CFG               = 34,
    PINMUX_TIMER1_ETR_CFG               = 35,
    PINMUX_TIMER2_ETR_CFG               = 36,
    PINMUX_TIMER0_BKIN_CFG              = 37,
    PINMUX_TIMER1_BKIN_CFG              = 38,
    PINMUX_TIMER2_BKIN_CFG              = 39,
    PINMUX_TIMER0_IO1_CFG               = 40,
    PINMUX_TIMER0_IO2_CFG               = 41,
    PINMUX_TIMER0_IO3_CFG               = 42,
    PINMUX_TIMER0_IO4_CFG               = 43,
    PINMUX_TIMER0_OUTN1_CFG             = 44,
    PINMUX_TIMER0_OUTN2_CFG             = 45,
    PINMUX_TIMER0_OUTN3_CFG             = 46,
    PINMUX_TIMER1_OUT1_CFG              = 47,
    PINMUX_TIMER1_OUT2_CFG              = 48,
    PINMUX_TIMER1_OUT3_CFG              = 49,
    PINMUX_TIMER1_OUT4_CFG              = 50,
    PINMUX_TIMER1_OUTN1_CFG             = 51,
    PINMUX_TIMER1_OUTN2_CFG             = 52,
    PINMUX_TIMER1_OUTN3_CFG             = 53,
    PINMUX_TIMER2_OUT1_CFG              = 54,
    PINMUX_TIMER2_OUT2_CFG              = 55,
    PINMUX_TIMER2_OUT3_CFG              = 56,
    PINMUX_TIMER2_OUT4_CFG              = 57,
    PINMUX_TIMER2_OUTN1_CFG             = 58,
    PINMUX_TIMER2_OUTN2_CFG             = 59,
    PINMUX_TIMER2_OUTN3_CFG             = 60,

    PINMUX_COMMON_PERIPHERAL_CFG        = 100,

    PINMUX_LP_TIMER_OUT0_CFG            = 101,
    PINMUX_LP_TIMER_OUT1_CFG            = 102,
} pinmux_t;

/// pin 0 pinmux function type
typedef enum {
    PINMUX_PAD0_TIMER1_OUT4_CFG         = PINMUX_TIMER1_OUT4_CFG,
    PINMUX_PAD0_TIMER2_ETR_CFG          = PINMUX_TIMER2_ETR_CFG,
    PINMUX_PAD0_UART0_TX_CFG            = PINMUX_UART0_TX_CFG,
    PINMUX_PAD0_SPI0_DI_CFG             = PINMUX_SPI0_DI_CFG,
    PINMUX_PAD0_I2C0_SCK_CFG            = PINMUX_I2C0_SCK_CFG,
    PINMUX_PAD0_INPUT_MODE_CFG          = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD0_GPIO_MODE_CFG           = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD0_LP_TIMER_OUT0_CFG       = PINMUX_LP_TIMER_OUT0_CFG,
} pin_0_func_t;

/// pin 1 pinmux function type
typedef enum {
    PINMUX_PAD1_TIMER1_OUT3_CFG         = PINMUX_TIMER1_OUT3_CFG,
    PINMUX_PAD1_TIMER2_OUTN1_CFG        = PINMUX_TIMER2_OUTN1_CFG,
    PINMUX_PAD1_TIMER0_ETR_CFG          = PINMUX_TIMER0_ETR_CFG,
    PINMUX_PAD1_UART0_RX_CFG            = PINMUX_UART0_RX_CFG,
    PINMUX_PAD1_UART1_CTS_CFG           = PINMUX_UART1_CTS_CFG,
    PINMUX_PAD1_SPI0_DO_CFG             = PINMUX_SPI0_DO_CFG,
    PINMUX_PAD1_I2C0_SDA_CFG            = PINMUX_I2C0_SDA_CFG,
    PINMUX_PAD1_INPUT_MODE_CFG          = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD1_GPIO_MODE_CFG           = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD1_LP_TIMER_OUT1_CFG       = PINMUX_LP_TIMER_OUT1_CFG,
} pin_1_func_t;

/// pin 2 pinmux function type
typedef enum {
    PINMUX_PAD2_TIMER1_OUT2_CFG         = PINMUX_TIMER1_OUT2_CFG,
    PINMUX_PAD2_TIMER2_OUTN2_CFG        = PINMUX_TIMER2_OUTN2_CFG,
    PINMUX_PAD2_TIMER0_BKIN_CFG         = PINMUX_TIMER0_BKIN_CFG,
    PINMUX_PAD2_UART0_TX_CFG            = PINMUX_UART0_TX_CFG,
    PINMUX_PAD2_UART1_RTS_CFG           = PINMUX_UART1_RTS_CFG,
    PINMUX_PAD2_SPI0_CS_CFG             = PINMUX_SPI0_CS_CFG,
    PINMUX_PAD2_I2C0_SCK_CFG            = PINMUX_I2C0_SCK_CFG,
    PINMUX_PAD2_TX_EXT_PD_CFG           = PINMUX_TX_EXT_PD_CFG,
    PINMUX_PAD2_PTA_ACTIVE_IN_CFG       = PINMUX_PTA_ACTIVE_IN_CFG,
    PINMUX_PAD2_INPUT_MODE_CFG          = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD2_GPIO_MODE_CFG           = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD2_LP_TIMER_OUT0_CFG       = PINMUX_LP_TIMER_OUT0_CFG,
} pin_2_func_t;

/// pin 3 pinmux function type
typedef enum {
    PINMUX_PAD3_TIMER1_OUT1_CFG         = PINMUX_TIMER1_OUT1_CFG,
    PINMUX_PAD3_TIMER2_OUTN3_CFG        = PINMUX_TIMER2_OUTN3_CFG,
    PINMUX_PAD3_TIMER0_ETR_CFG          = PINMUX_TIMER0_ETR_CFG,
    PINMUX_PAD3_UART0_RX_CFG            = PINMUX_UART0_RX_CFG,
    PINMUX_PAD3_UART1_RX_CFG            = PINMUX_UART1_RX_CFG,
    PINMUX_PAD3_SPI0_SCK_CFG            = PINMUX_SPI0_SCK_CFG,
    PINMUX_PAD3_I2C0_SDA_CFG            = PINMUX_I2C0_SDA_CFG,
    PINMUX_PAD3_RX_EXT_PD_CFG           = PINMUX_RX_EXT_PD_CFG,
    PINMUX_PAD3_PTA_ACTIVE_OUT_CFG      = PINMUX_PTA_ACTIVE_OUT_CFG,
    PINMUX_PAD3_INPUT_MODE_CFG          = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD3_GPIO_MODE_CFG           = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD3_LP_TIMER_OUT1_CFG       = PINMUX_LP_TIMER_OUT1_CFG,
} pin_3_func_t;

/// pin 4 pinmux function type
typedef enum {
    PINMUX_PAD4_TIMER0_IO1_CFG          = PINMUX_TIMER0_IO1_CFG,
    PINMUX_PAD4_TIMER1_ETR_CFG          = PINMUX_TIMER1_ETR_CFG,
    PINMUX_PAD4_UART0_TX_CFG            = PINMUX_UART0_TX_CFG,
    PINMUX_PAD4_UART1_TX_CFG            = PINMUX_UART1_TX_CFG,
    PINMUX_PAD4_SPI1_DI_CFG             = PINMUX_SPI1_DI_CFG,
    PINMUX_PAD4_I2C0_SCK_CFG            = PINMUX_I2C0_SCK_CFG,
    PINMUX_PAD4_TX_EXT_PD_CFG           = PINMUX_TX_EXT_PD_CFG,
    PINMUX_PAD4_PTA_PRIORITY_CFG        = PINMUX_PTA_PRIORITY_CFG,
    PINMUX_PAD4_INPUT_MODE_CFG          = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD4_GPIO_MODE_CFG           = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD4_LP_TIMER_OUT0_CFG       = PINMUX_LP_TIMER_OUT0_CFG,
} pin_4_func_t;

/// pin 5 pinmux function type
typedef enum {
    PINMUX_PAD5_TIMER0_IO2_CFG          = PINMUX_TIMER0_IO2_CFG,
    PINMUX_PAD5_TIMER1_OUTN1_CFG        = PINMUX_TIMER1_OUTN1_CFG,
    PINMUX_PAD5_TIMER2_BKIN_CFG         = PINMUX_TIMER2_BKIN_CFG,
    PINMUX_PAD5_UART0_TX_CFG            = PINMUX_UART0_TX_CFG,
    PINMUX_PAD5_UART1_TX_CFG            = PINMUX_UART1_TX_CFG,
    PINMUX_PAD5_SPI1_DO_CFG             = PINMUX_SPI1_DO_CFG,
    PINMUX_PAD5_I2C0_SDA_CFG            = PINMUX_I2C0_SDA_CFG,
    PINMUX_PAD5_RX_EXT_PD_CFG           = PINMUX_RX_EXT_PD_CFG,
    PINMUX_PAD5_PTA_FREQ_CFG            = PINMUX_PTA_FREQ_CFG,
    PINMUX_PAD5_INPUT_MODE_CFG          = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD5_GPIO_MODE_CFG           = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD5_LP_TIMER_OUT1_CFG       = PINMUX_LP_TIMER_OUT1_CFG,
} pin_5_func_t;

/// pin 6 pinmux function type
typedef enum {
    PINMUX_PAD6_TIMER0_IO3_CFG          = PINMUX_TIMER0_IO3_CFG,
    PINMUX_PAD6_TIMER1_OUTN2_CFG        = PINMUX_TIMER1_OUTN2_CFG,
    PINMUX_PAD6_TIMER2_ETR_CFG          = PINMUX_TIMER2_ETR_CFG,
    PINMUX_PAD6_UART0_RX_CFG            = PINMUX_UART0_RX_CFG,
    PINMUX_PAD6_UART1_RX_CFG            = PINMUX_UART1_RX_CFG,
    PINMUX_PAD6_SPI1_CS_CFG             = PINMUX_SPI1_CS_CFG,
    PINMUX_PAD6_I2C0_SCK_CFG            = PINMUX_I2C0_SCK_CFG,
    PINMUX_PAD6_TX_EXT_PD_CFG           = PINMUX_TX_EXT_PD_CFG,
    PINMUX_PAD6_PTA_ACTIVE_IN_CFG       = PINMUX_PTA_ACTIVE_IN_CFG,
    PINMUX_PAD6_INPUT_MODE_CFG          = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD6_GPIO_MODE_CFG           = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD6_LP_TIMER_OUT0_CFG       = PINMUX_LP_TIMER_OUT0_CFG,
} pin_6_func_t;

/// pin 7 pinmux function type
typedef enum {
    PINMUX_PAD7_TIMER0_IO4_CFG          = PINMUX_TIMER0_IO4_CFG,
    PINMUX_PAD7_TIMER1_OUTN3_CFG        = PINMUX_TIMER1_OUTN3_CFG,
    PINMUX_PAD7_TIMER2_BKIN_CFG         = PINMUX_TIMER2_BKIN_CFG,
    PINMUX_PAD7_UART0_RX_CFG            = PINMUX_UART0_RX_CFG,
    PINMUX_PAD7_UART1_CTS_CFG           = PINMUX_UART1_CTS_CFG,
    PINMUX_PAD7_SPI1_SCK_CFG            = PINMUX_SPI1_SCK_CFG,
    PINMUX_PAD7_I2C0_SDA_CFG            = PINMUX_I2C0_SDA_CFG,
    PINMUX_PAD7_SFLASH_SO_CFG           = PINMUX_SFLASH_SO_CFG,
    PINMUX_PAD7_RX_EXT_PD_CFG           = PINMUX_RX_EXT_PD_CFG,
    PINMUX_PAD7_PTA_ACTIVE_OUT_CFG      = PINMUX_PTA_ACTIVE_OUT_CFG,
    PINMUX_PAD7_INPUT_MODE_CFG          = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD7_GPIO_MODE_CFG           = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD7_LP_TIMER_OUT1_CFG       = PINMUX_LP_TIMER_OUT1_CFG,
} pin_7_func_t;

/// pin 8 pinmux function type
typedef enum {
    PINMUX_PAD8_TIMER2_OUT1_CFG         = PINMUX_TIMER2_OUT1_CFG,
    PINMUX_PAD8_TIMER0_BKIN_CFG         = PINMUX_TIMER0_BKIN_CFG,
    PINMUX_PAD8_UART0_TX_CFG            = PINMUX_UART0_TX_CFG,
    PINMUX_PAD8_UART1_RTS_CFG           = PINMUX_UART1_RTS_CFG,
    PINMUX_PAD8_SPI0_DI_CFG             = PINMUX_SPI0_DI_CFG,
    PINMUX_PAD8_I2C0_SCK_CFG            = PINMUX_I2C0_SCK_CFG,
    PINMUX_PAD8_SFLASH_CS_CFG           = PINMUX_SFLASH_CS_CFG,
    PINMUX_PAD8_TX_EXT_PD_CFG           = PINMUX_TX_EXT_PD_CFG,
    PINMUX_PAD8_PTA_PRIORITY_CFG        = PINMUX_PTA_PRIORITY_CFG,
    PINMUX_PAD8_INPUT_MODE_CFG          = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD8_GPIO_MODE_CFG           = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD8_LP_TIMER_OUT0_CFG       = PINMUX_LP_TIMER_OUT0_CFG,
} pin_8_func_t;

/// pin 9 pinmux function type
typedef enum {
    PINMUX_PAD9_TIMER2_OUT2_CFG         = PINMUX_TIMER2_OUT2_CFG,
    PINMUX_PAD9_TIMER0_OUTN1_CFG        = PINMUX_TIMER0_OUTN1_CFG,
    PINMUX_PAD9_TIMER1_BKIN_CFG         = PINMUX_TIMER1_BKIN_CFG,
    PINMUX_PAD9_UART0_RX_CFG            = PINMUX_UART0_RX_CFG,
    PINMUX_PAD9_UART1_RX_CFG            = PINMUX_UART1_RX_CFG,
    PINMUX_PAD9_SPI0_DO_CFG             = PINMUX_SPI0_DO_CFG,
    PINMUX_PAD9_I2C0_SDA_CFG            = PINMUX_I2C0_SDA_CFG,
    PINMUX_PAD9_SFLASH_CK_CFG           = PINMUX_SFLASH_CK_CFG,
    PINMUX_PAD9_RX_EXT_PD_CFG           = PINMUX_RX_EXT_PD_CFG,
    PINMUX_PAD9_PTA_FREQ_CFG            = PINMUX_PTA_FREQ_CFG,
    PINMUX_PAD9_INPUT_MODE_CFG          = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD9_GPIO_MODE_CFG           = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD9_LP_TIMER_OUT1_CFG       = PINMUX_LP_TIMER_OUT1_CFG,
} pin_9_func_t;

/// pin 10 pinmux function type
typedef enum {
    PINMUX_PAD10_TIMER2_OUT3_CFG        = PINMUX_TIMER2_OUT3_CFG,
    PINMUX_PAD10_TIMER0_OUTN2_CFG       = PINMUX_TIMER0_OUTN2_CFG,
    PINMUX_PAD10_TIMER1_ETR_CFG         = PINMUX_TIMER1_ETR_CFG,
    PINMUX_PAD10_UART0_TX_CFG           = PINMUX_UART0_TX_CFG,
    PINMUX_PAD10_UART1_TX_CFG           = PINMUX_UART1_TX_CFG,
    PINMUX_PAD10_SPI0_CS_CFG            = PINMUX_SPI0_CS_CFG,
    PINMUX_PAD10_I2C0_SCK_CFG           = PINMUX_I2C0_SCK_CFG,
    PINMUX_PAD10_SFLASH_SI_CFG          = PINMUX_SFLASH_SI_CFG,
    PINMUX_PAD10_TX_EXT_PD_CFG          = PINMUX_TX_EXT_PD_CFG,
    PINMUX_PAD10_PTA_ACTIVE_IN_CFG      = PINMUX_PTA_ACTIVE_IN_CFG,
    PINMUX_PAD10_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD10_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD10_LP_TIMER_OUT0_CFG      = PINMUX_LP_TIMER_OUT0_CFG,
} pin_10_func_t;

/// pin 11 pinmux function type
typedef enum {
    PINMUX_PAD11_TIMER2_OUT4_CFG        = PINMUX_TIMER2_OUT4_CFG,
    PINMUX_PAD11_TIMER0_OUTN3_CFG       = PINMUX_TIMER0_OUTN3_CFG,
    PINMUX_PAD11_TIMER1_BKIN_CFG        = PINMUX_TIMER1_BKIN_CFG,
    PINMUX_PAD11_UART0_RX_CFG           = PINMUX_UART0_RX_CFG,
    PINMUX_PAD11_UART1_CTS_CFG          = PINMUX_UART1_CTS_CFG,
    PINMUX_PAD11_SPI0_SCK_CFG           = PINMUX_SPI0_SCK_CFG,
    PINMUX_PAD11_I2C0_SDA_CFG           = PINMUX_I2C0_SDA_CFG,
    PINMUX_PAD11_SFLASH_HD_CFG          = PINMUX_SFLASH_HD_CFG,
    PINMUX_PAD11_RX_EXT_PD_CFG          = PINMUX_RX_EXT_PD_CFG,
    PINMUX_PAD11_PTA_ACTIVE_OUT_CFG     = PINMUX_PTA_ACTIVE_OUT_CFG,
    PINMUX_PAD11_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD11_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD11_LP_TIMER_OUT1_CFG      = PINMUX_LP_TIMER_OUT1_CFG,
} pin_11_func_t;

/// pin 12 pinmux function type
typedef enum {
    PINMUX_PAD12_TIMER1_OUT1_CFG        = PINMUX_TIMER1_OUT1_CFG,
    PINMUX_PAD12_TIMER2_ETR_CFG         = PINMUX_TIMER2_ETR_CFG,
    PINMUX_PAD12_UART0_TX_CFG           = PINMUX_UART0_TX_CFG,
    PINMUX_PAD12_UART1_RTS_CFG          = PINMUX_UART1_RTS_CFG,
    PINMUX_PAD12_SPI1_DI_CFG            = PINMUX_SPI1_DI_CFG,
    PINMUX_PAD12_I2C0_SCK_CFG           = PINMUX_I2C0_SCK_CFG,
    PINMUX_PAD12_SFLASH_WP_CFG          = PINMUX_SFLASH_WP_CFG,
    PINMUX_PAD12_TX_EXT_PD_CFG          = PINMUX_TX_EXT_PD_CFG,
    PINMUX_PAD12_PTA_PRIORITY_CFG       = PINMUX_PTA_PRIORITY_CFG,
    PINMUX_PAD12_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD12_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD12_LP_TIMER_OUT0_CFG      = PINMUX_LP_TIMER_OUT0_CFG,
} pin_12_func_t;

/// pin 13 pinmux function type
typedef enum {
    PINMUX_PAD13_TIMER1_OUT2_CFG        = PINMUX_TIMER1_OUT2_CFG,
    PINMUX_PAD13_TIMER2_OUTN1_CFG       = PINMUX_TIMER2_OUTN1_CFG,
    PINMUX_PAD13_TIMER0_ETR_CFG         = PINMUX_TIMER0_ETR_CFG,
    PINMUX_PAD13_UART0_RX_CFG           = PINMUX_UART0_RX_CFG,
    PINMUX_PAD13_UART1_RX_CFG           = PINMUX_UART1_RX_CFG,
    PINMUX_PAD13_SPI1_DO_CFG            = PINMUX_SPI1_DO_CFG,
    PINMUX_PAD13_I2C0_SDA_CFG           = PINMUX_I2C0_SDA_CFG,
    PINMUX_PAD13_RX_EXT_PD_CFG          = PINMUX_RX_EXT_PD_CFG,
    PINMUX_PAD13_PTA_FREQ_CFG           = PINMUX_PTA_FREQ_CFG,
    PINMUX_PAD13_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD13_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD13_LP_TIMER_OUT1_CFG      = PINMUX_LP_TIMER_OUT1_CFG,
} pin_13_func_t;

/// pin 14 pinmux function type
typedef enum {
    PINMUX_PAD14_TIMER1_OUT3_CFG        = PINMUX_TIMER1_OUT3_CFG,
    PINMUX_PAD14_TIMER2_OUTN2_CFG       = PINMUX_TIMER2_OUTN2_CFG,
    PINMUX_PAD14_TIMER0_BKIN_CFG        = PINMUX_TIMER0_BKIN_CFG,
    PINMUX_PAD14_UART0_TX_CFG           = PINMUX_UART0_TX_CFG,
    PINMUX_PAD14_UART1_TX_CFG           = PINMUX_UART1_TX_CFG,
    PINMUX_PAD14_SPI1_CS_CFG            = PINMUX_SPI1_CS_CFG,
    PINMUX_PAD14_I2C0_SCK_CFG           = PINMUX_I2C0_SCK_CFG,
    PINMUX_PAD14_TX_EXT_PD_CFG          = PINMUX_TX_EXT_PD_CFG,
    PINMUX_PAD14_PTA_ACTIVE_IN_CFG      = PINMUX_PTA_ACTIVE_IN_CFG,
    PINMUX_PAD14_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD14_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD14_LP_TIMER_OUT0_CFG      = PINMUX_LP_TIMER_OUT0_CFG,
} pin_14_func_t;

/// pin 15 pinmux function type
typedef enum {
    PINMUX_PAD15_TIMER1_OUT4_CFG        = PINMUX_TIMER1_OUT4_CFG,
    PINMUX_PAD15_TIMER2_OUTN3_CFG       = PINMUX_TIMER2_OUTN3_CFG,
    PINMUX_PAD15_TIMER0_ETR_CFG         = PINMUX_TIMER0_ETR_CFG,
    PINMUX_PAD15_UART0_RX_CFG           = PINMUX_UART0_RX_CFG,
    PINMUX_PAD15_UART1_CTS_CFG          = PINMUX_UART1_CTS_CFG,
    PINMUX_PAD15_SPI1_SCK_CFG           = PINMUX_SPI1_SCK_CFG,
    PINMUX_PAD15_I2C0_SDA_CFG           = PINMUX_I2C0_SDA_CFG,
    PINMUX_PAD15_RX_EXT_PD_CFG          = PINMUX_RX_EXT_PD_CFG,
    PINMUX_PAD15_PTA_ACTIVE_OUT_CFG     = PINMUX_PTA_ACTIVE_OUT_CFG,
    PINMUX_PAD15_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD15_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD15_LP_TIMER_OUT1_CFG      = PINMUX_LP_TIMER_OUT1_CFG,
} pin_15_func_t;

/// pin 16 pinmux function type
typedef enum {
    PINMUX_PAD16_TIMER0_IO1_CFG         = PINMUX_TIMER0_IO1_CFG,
    PINMUX_PAD16_TIMER1_ETR_CFG         = PINMUX_TIMER1_ETR_CFG,
    PINMUX_PAD16_UART0_TX_CFG           = PINMUX_UART0_TX_CFG,
    PINMUX_PAD16_UART1_RTS_CFG          = PINMUX_UART1_RTS_CFG,
    PINMUX_PAD16_SPI0_DI_CFG            = PINMUX_SPI0_DI_CFG,
    PINMUX_PAD16_I2C0_SCK_CFG           = PINMUX_I2C0_SCK_CFG,
    PINMUX_PAD16_TX_EXT_PD_CFG          = PINMUX_TX_EXT_PD_CFG,
    PINMUX_PAD16_PTA_PRIORITY_CFG       = PINMUX_PTA_PRIORITY_CFG,
    PINMUX_PAD16_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD16_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD16_LP_TIMER_OUT0_CFG      = PINMUX_LP_TIMER_OUT0_CFG,
} pin_16_func_t;

/// pin 17 pinmux function type
typedef enum {
    PINMUX_PAD17_TIMER0_IO2_CFG         = PINMUX_TIMER0_IO2_CFG,
    PINMUX_PAD17_TIMER1_OUTN1_CFG       = PINMUX_TIMER1_OUTN1_CFG,
    PINMUX_PAD17_TIMER2_BKIN_CFG        = PINMUX_TIMER2_BKIN_CFG,
    PINMUX_PAD17_UART0_RX_CFG           = PINMUX_UART0_RX_CFG,
    PINMUX_PAD17_UART1_RX_CFG           = PINMUX_UART1_RX_CFG,
    PINMUX_PAD17_SPI0_DO_CFG            = PINMUX_SPI0_DO_CFG,
    PINMUX_PAD17_I2C0_SDA_CFG           = PINMUX_I2C0_SDA_CFG,
    PINMUX_PAD17_RX_EXT_PD_CFG          = PINMUX_RX_EXT_PD_CFG,
    PINMUX_PAD17_PTA_FREQ_CFG           = PINMUX_PTA_FREQ_CFG,
    PINMUX_PAD17_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD17_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD17_LP_TIMER_OUT1_CFG      = PINMUX_LP_TIMER_OUT1_CFG,
} pin_17_func_t;

/// pin 18 pinmux function type
typedef enum {
    PINMUX_PAD18_TIMER0_IO3_CFG         = PINMUX_TIMER0_IO3_CFG,
    PINMUX_PAD18_TIMER1_OUTN2_CFG       = PINMUX_TIMER1_OUTN2_CFG,
    PINMUX_PAD18_TIMER2_ETR_CFG         = PINMUX_TIMER2_ETR_CFG,
    PINMUX_PAD18_UART0_TX_CFG           = PINMUX_UART0_TX_CFG,
    PINMUX_PAD18_UART1_TX_CFG           = PINMUX_UART1_TX_CFG,
    PINMUX_PAD18_SPI0_CS_CFG            = PINMUX_SPI0_CS_CFG,
    PINMUX_PAD18_I2C0_SCK_CFG           = PINMUX_I2C0_SCK_CFG,
    PINMUX_PAD18_TX_EXT_PD_CFG          = PINMUX_TX_EXT_PD_CFG,
    PINMUX_PAD18_PTA_ACTIVE_IN_CFG      = PINMUX_PTA_ACTIVE_IN_CFG,
    PINMUX_PAD18_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD18_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD18_LP_TIMER_OUT0_CFG      = PINMUX_LP_TIMER_OUT0_CFG,
} pin_18_func_t;

/// pin 19 pinmux function type
typedef enum {
    PINMUX_PAD19_TIMER0_IO4_CFG         = PINMUX_TIMER0_IO4_CFG,
    PINMUX_PAD19_TIMER1_OUTN3_CFG       = PINMUX_TIMER1_OUTN3_CFG,
    PINMUX_PAD19_TIMER2_BKIN_CFG        = PINMUX_TIMER2_BKIN_CFG,
    PINMUX_PAD19_UART0_RX_CFG           = PINMUX_UART0_RX_CFG,
    PINMUX_PAD19_UART1_CTS_CFG          = PINMUX_UART1_CTS_CFG,
    PINMUX_PAD19_SPI0_SCK_CFG           = PINMUX_SPI0_SCK_CFG,
    PINMUX_PAD19_I2C0_SDA_CFG           = PINMUX_I2C0_SDA_CFG,
    PINMUX_PAD19_RX_EXT_PD_CFG          = PINMUX_RX_EXT_PD_CFG,
    PINMUX_PAD19_PTA_ACTIVE_OUT_CFG     = PINMUX_PTA_ACTIVE_OUT_CFG,
    PINMUX_PAD19_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD19_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD19_LP_TIMER_OUT1_CFG      = PINMUX_LP_TIMER_OUT1_CFG,
} pin_19_func_t;

/// pin 20 pinmux function type
typedef enum {
    PINMUX_PAD20_TIMER2_OUT1_CFG        = PINMUX_TIMER2_OUT1_CFG,
    PINMUX_PAD20_TIMER0_BKIN_CFG        = PINMUX_TIMER0_BKIN_CFG,
    PINMUX_PAD20_UART0_TX_CFG           = PINMUX_UART0_TX_CFG,
    PINMUX_PAD20_UART1_RTS_CFG          = PINMUX_UART1_RTS_CFG,
    PINMUX_PAD20_SPI1_DI_CFG            = PINMUX_SPI1_DI_CFG,
    PINMUX_PAD20_I2C0_SCK_CFG           = PINMUX_I2C0_SCK_CFG,
    PINMUX_PAD20_TX_EXT_PD_CFG          = PINMUX_TX_EXT_PD_CFG,
    PINMUX_PAD20_PTA_PRIORITY_CFG       = PINMUX_PTA_PRIORITY_CFG,
    PINMUX_PAD20_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD20_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD20_LP_TIMER_OUT0_CFG      = PINMUX_LP_TIMER_OUT0_CFG,
} pin_20_func_t;

/// pin 21 pinmux function type
typedef enum {
    PINMUX_PAD21_TIMER2_OUT2_CFG        = PINMUX_TIMER2_OUT2_CFG,
    PINMUX_PAD21_TIMER0_OUTN1_CFG       = PINMUX_TIMER0_OUTN1_CFG,
    PINMUX_PAD21_TIMER1_BKIN_CFG        = PINMUX_TIMER1_BKIN_CFG,
    PINMUX_PAD21_UART0_RX_CFG           = PINMUX_UART0_RX_CFG,
    PINMUX_PAD21_UART1_RX_CFG           = PINMUX_UART1_RX_CFG,
    PINMUX_PAD21_SPI1_DO_CFG            = PINMUX_SPI1_DO_CFG,
    PINMUX_PAD21_I2C0_SDA_CFG           = PINMUX_I2C0_SDA_CFG,
    PINMUX_PAD21_RX_EXT_PD_CFG          = PINMUX_RX_EXT_PD_CFG,
    PINMUX_PAD21_PTA_FREQ_CFG           = PINMUX_PTA_FREQ_CFG,
    PINMUX_PAD21_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD21_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD21_LP_TIMER_OUT1_CFG      = PINMUX_LP_TIMER_OUT1_CFG,
} pin_21_func_t;

/// pin 22 pinmux function type
typedef enum {
    PINMUX_PAD22_TIMER2_OUT3_CFG        = PINMUX_TIMER2_OUT3_CFG,
    PINMUX_PAD22_TIMER0_OUTN2_CFG       = PINMUX_TIMER0_OUTN2_CFG,
    PINMUX_PAD22_TIMER1_ETR_CFG         = PINMUX_TIMER1_ETR_CFG,
    PINMUX_PAD22_UART0_TX_CFG           = PINMUX_UART0_TX_CFG,
    PINMUX_PAD22_UART1_TX_CFG           = PINMUX_UART1_TX_CFG,
    PINMUX_PAD22_SPI1_CS_CFG            = PINMUX_SPI1_CS_CFG,
    PINMUX_PAD22_I2C0_SCK_CFG           = PINMUX_I2C0_SCK_CFG,
    PINMUX_PAD22_TX_EXT_PD_CFG          = PINMUX_TX_EXT_PD_CFG,
    PINMUX_PAD22_PTA_ACTIVE_IN_CFG      = PINMUX_PTA_ACTIVE_IN_CFG,
    PINMUX_PAD22_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD22_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD22_LP_TIMER_OUT0_CFG      = PINMUX_LP_TIMER_OUT0_CFG,
} pin_22_func_t;

/// pin 23 pinmux function type
typedef enum {
    PINMUX_PAD23_TIMER2_OUT4_CFG        = PINMUX_TIMER2_OUT4_CFG,
    PINMUX_PAD23_TIMER0_OUTN3_CFG       = PINMUX_TIMER0_OUTN3_CFG,
    PINMUX_PAD23_TIMER1_BKIN_CFG        = PINMUX_TIMER1_BKIN_CFG,
    PINMUX_PAD23_UART0_RX_CFG           = PINMUX_UART0_RX_CFG,
    PINMUX_PAD23_SPI1_SCK_CFG           = PINMUX_SPI1_SCK_CFG,
    PINMUX_PAD23_I2C0_SDA_CFG           = PINMUX_I2C0_SDA_CFG,
    PINMUX_PAD23_RX_EXT_PD_CFG          = PINMUX_RX_EXT_PD_CFG,
    PINMUX_PAD23_PTA_ACTIVE_OUT_CFG     = PINMUX_PTA_ACTIVE_OUT_CFG,
    PINMUX_PAD23_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD23_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD23_LP_TIMER_OUT1_CFG      = PINMUX_LP_TIMER_OUT1_CFG,
} pin_23_func_t;

/// pin 24 pinmux function type
typedef enum {
    PINMUX_PAD24_TIMER0_IO1_CFG         = PINMUX_TIMER0_IO1_CFG,
    PINMUX_PAD24_TIMER0_OUTN2_CFG       = PINMUX_TIMER0_OUTN2_CFG,
    PINMUX_PAD24_TIMER0_ETR_CFG         = PINMUX_TIMER0_ETR_CFG,
    PINMUX_PAD24_UART0_TX_CFG           = PINMUX_UART0_TX_CFG,
    PINMUX_PAD24_UART1_RX_CFG           = PINMUX_UART1_RX_CFG,
    PINMUX_PAD24_SPI0_DI_CFG            = PINMUX_SPI0_DI_CFG,
    PINMUX_PAD24_I2C0_SCK_CFG           = PINMUX_I2C0_SCK_CFG,
    PINMUX_PAD24_TX_EXT_PD_CFG          = PINMUX_TX_EXT_PD_CFG,
    PINMUX_PAD24_PTA_PRIORITY_CFG       = PINMUX_PTA_PRIORITY_CFG,
    PINMUX_PAD24_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD24_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD24_LP_TIMER_OUT0_CFG      = PINMUX_LP_TIMER_OUT0_CFG,
} pin_24_func_t;

/// pin 25 pinmux function type
typedef enum {
    PINMUX_PAD25_TIMER0_IO2_CFG         = PINMUX_TIMER0_IO2_CFG,
    PINMUX_PAD25_TIMER0_OUTN1_CFG       = PINMUX_TIMER0_OUTN1_CFG,
    PINMUX_PAD25_TIMER0_BKIN_CFG        = PINMUX_TIMER0_BKIN_CFG,
    PINMUX_PAD25_UART0_RX_CFG           = PINMUX_UART0_RX_CFG,
    PINMUX_PAD25_UART1_TX_CFG           = PINMUX_UART1_TX_CFG,
    PINMUX_PAD25_SPI0_DO_CFG            = PINMUX_SPI0_DO_CFG,
    PINMUX_PAD25_I2C0_SDA_CFG           = PINMUX_I2C0_SDA_CFG,
    PINMUX_PAD25_RX_EXT_PD_CFG          = PINMUX_RX_EXT_PD_CFG,
    PINMUX_PAD25_PTA_FREQ_CFG           = PINMUX_PTA_FREQ_CFG,
    PINMUX_PAD25_INPUT_MODE_CFG         = PINMUX_INPUT_MODE_CFG,
    PINMUX_PAD25_GPIO_MODE_CFG          = PINMUX_GPIO_MODE_CFG,
    PINMUX_PAD25_LP_TIMER_OUT1_CFG      = PINMUX_LP_TIMER_OUT1_CFG,
} pin_25_func_t;

/// Pinmux Function
typedef union {
    uint8_t         pin_func;
    pin_0_func_t    pin0_func;
    pin_1_func_t    pin1_func;
    pin_2_func_t    pin2_func;
    pin_3_func_t    pin3_func;
    pin_4_func_t    pin4_func;
    pin_5_func_t    pin5_func;
    pin_6_func_t    pin6_func;
    pin_7_func_t    pin7_func;
    pin_8_func_t    pin8_func;
    pin_9_func_t    pin9_func;
    pin_10_func_t   pin10_func;
    pin_11_func_t   pin11_func;
    pin_12_func_t   pin12_func;
    pin_13_func_t   pin13_func;
    pin_14_func_t   pin14_func;
    pin_15_func_t   pin15_func;
    pin_16_func_t   pin16_func;
    pin_17_func_t   pin17_func;
    pin_18_func_t   pin18_func;
    pin_19_func_t   pin19_func;
    pin_20_func_t   pin20_func;
    pin_21_func_t   pin21_func;
    pin_22_func_t   pin22_func;
    pin_23_func_t   pin23_func;
    pin_24_func_t   pin24_func;
    pin_25_func_t   pin25_func;
} pin_func_t;

/// Pin Configuration
typedef struct {
    /// pin index
    uint8_t                      dig_pad;
    /// function
    pin_func_t                   func;
    /// mode
    pmu_pin_mode_t               mode;
    /// current
    pmu_pin_driver_current_t     drv;
} pin_config_t;


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Initialize pin function
 *
 * @param[in] pin      pin
 * @param[in] func     function
 *
 *******************************************************************************
 */
extern void drv_pin_mux_set(uint8_t pin, pin_func_t func);

/**
 *******************************************************************************
 * @brief Initialize pin function and electrical characteristics
 *
 * @param[in] pin_cfg           Configuration for pinmux
 * @param[in] pin_cfg_num       Configuration number for pinmux
 *
 *******************************************************************************
 */
extern void drv_pin_init(const pin_config_t *pin_cfg, uint32_t pin_cfg_num);


#ifdef __cplusplus
}
#endif

#endif  /* RTE_PIN */
#endif  /* __DRV_PINMUX_H */


/** @} */

