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
 * @file     drv_gpio.h
 * @brief    Header file for gpio
 * @date     3 Feb 2023
 * @author   OnMicro SW Team
 *
 * @defgroup GPIO GPIO
 * @ingroup  DRIVER
 * @brief    GPIO Driver for om662x
 * @details  GPIO Driver for om662x
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 *
 * @example example_gpio.c
 * This is an example of how to use the gpio
 *
 */

#ifndef __DRV_GPIO_H
#define __DRV_GPIO_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_OM662x.h"
#if (RTE_GPIO0)
#include <stdint.h>
#include "om_driver.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
#define GPIO_MASK(gpio_idx)       (1U << (gpio_idx))


/*******************************************************************************
 * TYPEDEFS
 */
/// GPIO Mask: bit field for gpio, 1 indicates valid, 0 indicates invalid
typedef uint32_t gpio_mask_t;

/// GPIO Level
typedef enum {
    /// gpio level is low
    GPIO_LEVEL_LOW  = 0,
    /// gpio level is high
    GPIO_LEVEL_HIGH = 0xFFFFFFFF,
} gpio_level_t;

/// GPIO Direction
typedef enum {
    /// GPIO output direction
    GPIO_DIR_OUTPUT   = 0U,
    /// GPIO input direction
    GPIO_DIR_INPUT    = 1U,
} gpio_dir_t;

/// GPIO Interrupt Trigger Type
typedef enum {
    /// GPIO trigger type is falling edge
    GPIO_TRIG_FALLING_EDGE        = 0x0CU,
    /// GPIO trigger type is rising edge
    GPIO_TRIG_RISING_EDGE         = 0x03U,
    /// GPIO trigger type  is both edge
    GPIO_TRIG_RISING_FAILING_EDGE = 0x06U,
    /// GPIO trigger type is low level
    GPIO_TRIG_LOW_LEVEL           = 0x00U,
    /// GPIO trigger type is high level
    GPIO_TRIG_HIGH_LEVEL          = 0x0FU,
    /// GPIO trigger type is none
    GPIO_TRIG_NONE                = 0x10U,
} gpio_trig_type_t;

/// GPIO Configuration
typedef struct {
    /// Pointer to OM_GPIOx
    OM_GPIO_Type          *om_gpio;
    /// GPIO index in a GPIO port, range in [0, 25]
    uint8_t                gpio_idx;
    /// GPIO direction
    gpio_dir_t             dir;
    /// Used when dir config as output, range in [0, 1]
    gpio_level_t           out_val;
    /// Used when dir config as input
    gpio_trig_type_t       trig_type;
} gpio_config_t;

/// GPIO Control
typedef enum {
    GPIO_CONTROL_CLK_DISABLE       = 0U,    /*!< Disable GPIO clock, argu is NULL, return OM_ERROR_OK */
    GPIO_CONTROL_CLK_ENABLE        = 1U,    /*!< Enable GPIO clock, argu is NULL, return OM_ERROR_OK */
    GPIO_CONTROL_RESET             = 2U,    /*!< Reset GPIO, argu is NULL, return OM_ERROR_OK */
    GPIO_CONTROL_CLEAR_INT         = 3U,    /*!< Clear GPIO interrupt, argu is gpio mask, return OM_ERROR_OK */
} gpio_control_t;


/*******************************************************************************
 * EXTERN VARIABLES
 */


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Set GPIO direction
 *
 * @param[in] om_gpio        Pointer to GPIO
 * @param[in] gpio_mask      GPIO pin mask
 * @param[in] dir            GPIO direction
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_gpio_set_dir(OM_GPIO_Type *om_gpio, gpio_mask_t gpio_mask, gpio_dir_t dir)
{
    if (dir == GPIO_DIR_OUTPUT) {
        om_gpio->OUTENSET = gpio_mask;
    } else {
        om_gpio->OUTENCLR = gpio_mask;
    }
}

/**
 *******************************************************************************
 * @brief Get GPIO direction
 *
 * @param[in] om_gpio        Pointer to GPIO
 * @param[in] gpio_mask      GPIO pin mask
 *
 * @return GPIO direction
 *******************************************************************************
 */
__STATIC_FORCEINLINE gpio_dir_t drv_gpio_get_dir(OM_GPIO_Type *om_gpio, gpio_mask_t gpio_mask)
{
    if (om_gpio->OUTENSET & gpio_mask) {
        return GPIO_DIR_OUTPUT;
    } else {
        return GPIO_DIR_INPUT;
    }
}

/**
 *******************************************************************************
 * @brief Read GPIO pin input
 *
 * @param[in] om_gpio        Pointer to GPIO
 * @param[in] gpio_mask      GPIO pin mask
 *
 * @return GPIO pad value
 *******************************************************************************
 */
__STATIC_FORCEINLINE uint32_t drv_gpio_read(OM_GPIO_Type *om_gpio, gpio_mask_t gpio_mask)
{
    return (om_gpio->DATA & gpio_mask);
}

/**
 *******************************************************************************
 * @brief Write GPIO pin output
 *
 * @param[in] om_gpio        Pointer to GPIO
 * @param[in] gpio_mask      GPIO pin mask
 * @param[in] val            GPIO pin value
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_gpio_write(OM_GPIO_Type *om_gpio, gpio_mask_t gpio_mask, gpio_level_t val)
{
    uint8_t *p = (uint8_t *)&gpio_mask;

    if (p[0]) {
        om_gpio->MASK_0_7[p[0]] = val;
    }
    if (p[1]) {
        om_gpio->MASK_8_15[p[1]] = val;
    }
    if (p[2]) {
        om_gpio->MASK_16_23[p[2]] = val;
    }
    if (p[3]) {
        om_gpio->MASK_24_31[p[3]] = val;
    }
}

/**
 *******************************************************************************
 * @brief Toggle GPIO pin output
 *
 * @param[in] om_gpio        Pointer to GPIO
 * @param[in] gpio_mask      GPIO pin mask
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_gpio_toggle(OM_GPIO_Type *om_gpio, gpio_mask_t gpio_mask)
{
    OM_CRITICAL_BEGIN();
    om_gpio->DATAOUT ^= gpio_mask;
    OM_CRITICAL_END();
}

/**
 *******************************************************************************
 * @brief Read GPIO port input
 *
 * @param[in] om_gpio        Pointer to GPIO
 *
 * @return GPIO port value
 *******************************************************************************
 */
__STATIC_FORCEINLINE uint32_t drv_gpio_port_read(OM_GPIO_Type *om_gpio)
{
    return om_gpio->DATA;
}

/**
 *******************************************************************************
 * @brief Write GPIO port output
 *
 * @param[in] om_gpio        Pointer to GPIO
 * @param[in] port_val       GPIO port value
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_gpio_port_write(OM_GPIO_Type *om_gpio, uint32_t port_val)
{
    om_gpio->DATAOUT = port_val;
}

/**
 *******************************************************************************
 * @brief GPIO initialization
 *
 * @param[in] gpio_config      Configuration for GPIO
 * @param[in] gpio_config_num  Configuration number for GPIO
 *******************************************************************************
 */
extern void drv_gpio_init(const gpio_config_t *gpio_config, uint32_t gpio_config_num);

#if (RTE_GPIO_REGISTER_CALLBACK)
/**
 *******************************************************************************
 * @brief Register event callback for GPIO interrupt
 *
 * @param[in] om_gpio        Pointer to GPIO
 * @param[in] event_cb       Pointer to callback
 *******************************************************************************
 */
extern void drv_gpio_register_isr_callback(OM_GPIO_Type              *om_gpio,
                                             drv_isr_callback_t        event_cb);
#endif

/**
 *******************************************************************************
 * @brief The interrupt callback for GPIO driver. It is a weak function. User should define
 *        their own callback in user file, other than modify it in the GPIO driver.
 *
 * @param om_gpio           The GPIO device address
 * @param event             The driver usart event
 *                           - DRV_EVENT_COMMON_GENERAL
 * @param int_status        The interrupt status
 * @param gpio_data         The gpio level data
 *******************************************************************************
 */
extern __WEAK void drv_gpio_isr_callback(OM_GPIO_Type *om_gpio, drv_event_t event, uint32_t int_status, uint32_t gpio_data);

/**
 *******************************************************************************
 * @brief Set GPIO pin trigger type
 *
 * @param[in] om_gpio        Pointer to GPIO
 * @param[in] gpio_mask      GPIO pin mask, if bit field is 1, then set trigger
 * @param[in] trig           trig type
 *
 * @note When both sleep and GPIO input interrupts are enabled, the GPIO input interrupt must be configured as `GPIO_TRIG_RISING_FAILING_EDGE`.
 *       Because the 2nd edge is used to trigger pm system to detect the sleep state.
 *
 *******************************************************************************
 */
extern void drv_gpio_set_trig(OM_GPIO_Type *om_gpio, gpio_mask_t gpio_mask, gpio_trig_type_t trig);

/**
 *******************************************************************************
 * @brief Get GPIO pin trigger type
 *
 * @param[in] om_gpio        Pointer to GPIO
 * @param[in] gpio_mask      GPIO pad mask, if bit field is 1, then set trigger
 *
 * @return trig type
 *******************************************************************************
 */
extern gpio_trig_type_t drv_gpio_get_trig(OM_GPIO_Type *om_gpio, gpio_mask_t gpio_mask);

/**
 *******************************************************************************
 * @brief Control GPIO interface.
 *
 * @param[in] om_gpio        Pointer to GPIO port, Donot GPIO_CONTROL_CLK_DISABLE if using sleep
 * @param[in] control        Operation
 * @param[in] argu           Not used, always set NULL
 *
 * @return                   Control status, always return OM_ERROR_OK
 *******************************************************************************
 */
extern void *drv_gpio_control(OM_GPIO_Type *om_gpio, gpio_control_t control, void *argu);

/**
 *******************************************************************************
 * @brief gpio interrupt service routine
 *
 * @param[in] om_gpio        Pointer to GPIO port, Donot GPIO_CONTROL_CLK_DISABLE if using sleep
 *
 *******************************************************************************
 */
extern void drv_gpio_isr(OM_GPIO_Type *om_gpio);

#ifdef __cplusplus
}
#endif

#endif  /* RTE_GPIO0 */

#endif  /* __DRV_GPIO_H */


/** @} */
