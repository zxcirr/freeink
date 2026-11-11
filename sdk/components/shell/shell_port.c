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
 * @file     shell_task.c
 * @brief    shell task
 * @date     03. April 2020
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


/*******************************************************************************
 * INCLUDES
 */
#include "om_driver.h"
#include "shell.h"
#include "evt.h"
#include "om_kfifo.h"

#ifdef CONFIG_SHELL

/*******************************************************************************
 * TYPEDEFS
 */

typedef struct {
    om_kfifo_t          fifo;
    uint8_t             fifo_buffer[64];
    char                shell_line[SHELL_MAX_ARGUMENTS];
    uint32_t            shell_line_index;
    const shell_cmd_t   *shell_line_cmd;
} shell_env_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
static shell_env_t shell_env;


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
/* 1. shell retarget to USARTx ---------------------------------------------- */
#if (CONFIG_SHELL_USART0 || CONFIG_SHELL_USART1)

#if (CONFIG_SHELL_USART0)
#define SHELL_USART          OM_USART0
#elif (CONFIG_SHELL_USART1)
#define SHELL_USART          OM_USART1
#endif

static void shell_usart_cb(void *om_usart, drv_event_t event, void *rx_buf, void *rx_cnt)
{
    if (event & DRV_EVENT_COMMON_READ_COMPLETED) {
        om_kfifo_in(&shell_env.fifo, rx_buf, (uint32_t)rx_cnt);
        evt_set(EVT_TYPE_SHELL);
    }
}

static void shell_usart_init(void)
{
    #if ((CONFIG_SHELL_USART0) && (RTE_USART0))
    usart_ex_config_t usart_cfg = {
        .baudrate       = CONFIG_SHELL_USART_BAUDRATE,
    };
    drv_usart_ex_init(SHELL_USART, &usart_cfg);
    drv_usart_ex_register_isr_callback(SHELL_USART, shell_usart_cb);
    #elif ((CONFIG_SHELL_USART1) && (RTE_USART1))
    usart_config_t usart_cfg = {
        .baudrate       = CONFIG_SHELL_USART_BAUDRATE,
        .flow_control   = USART_FLOW_CONTROL_NONE,
        .data_bit       = USART_DATA_BIT_8,
        .stop_bit       = USART_STOP_BIT_1,
        .parity         = USART_PARITY_NONE,
    };

    drv_usart_init(SHELL_USART, &usart_cfg);
    drv_usart_register_isr_callback(SHELL_USART, shell_usart_cb);
    drv_usart_read_int(SHELL_USART, NULL, 0);
    #endif
}

static void shell_usart_out(char c)
{
    OM_CRITICAL_BEGIN();
    #if ((CONFIG_SHELL_USART0) && (RTE_USART0))
    drv_usart_ex_write(SHELL_USART, (uint8_t *)&c, 1, DRV_MAX_DELAY);
    #elif ((CONFIG_SHELL_USART1) && (RTE_USART1))
    drv_usart_write(SHELL_USART, (uint8_t *)&c, 1, DRV_MAX_DELAY);
    #endif
    OM_CRITICAL_END();
}

#if (RTE_PM)
static void shell_pm_sleep_store_restore_handler(pm_sleep_state_t sleep_state, pm_status_t power_status)
{
    if (sleep_state == PM_SLEEP_RESTORE_HSI) {
        shell_usart_init();
    }
}
#endif

#endif


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */

static void shell_evt_handler(void)
{
    uint8_t rx_ch;
    uint16_t rx_len;

    evt_clear(EVT_TYPE_SHELL);

    #if (RTE_PM)
    pm_sleep_prevent(PM_ID_SHELL);
    #endif  /* (RTE_PM) */

    // process rx character
    while (1) {
        rx_len = om_kfifo_out_1byte(&shell_env.fifo, &rx_ch);
        if (rx_len) {
            if (shell_get_line(rx_ch, shell_env.shell_line, sizeof(shell_env.shell_line), (unsigned *)(&shell_env.shell_line_index)) == true) {
                if (shell_env.shell_line_index != 0) {
                    shell_main(shell_env.shell_line, shell_env.shell_line_cmd);
                    memset(shell_env.shell_line, 0x00, sizeof(shell_env.shell_line));
                    shell_env.shell_line_index = 0x00;
                }
                shell_printf("> ");
            }
        } else {
            break;
        }
    }

    #if (RTE_PM)
    if (shell_env.shell_line_index == 0) {
        pm_sleep_allow(PM_ID_SHELL);
    }
    #endif  /* (RTE_PM) */
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
void shell_init(const shell_cmd_t *cmd)
{
    memset(shell_env.shell_line, 0x00, sizeof(shell_env.shell_line));
    shell_env.shell_line_cmd = cmd;

    evt_callback_set(EVT_TYPE_SHELL, shell_evt_handler);
    om_kfifo_init(&shell_env.fifo, shell_env.fifo_buffer, sizeof(shell_env.fifo_buffer));

    #if (CONFIG_SHELL_USART0 || CONFIG_SHELL_USART1)
    shell_usart_init();
    #endif

    shell_printf("\nOMBLE2\n");
    shell_printf("> \n");

    #if (RTE_PM)
    pm_sleep_prevent(PM_ID_SHELL);
    pm_sleep_store_restore_callback_register(shell_pm_sleep_store_restore_handler);
    #endif  /* (RTE_PM) */
}

void shell_out(char c)
{
    shell_usart_out(c);
}

#endif

/** @} */
