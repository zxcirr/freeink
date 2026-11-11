#include "om.h"
#include "om_driver.h"
#include "evt.h"
#include "pm.h"
#include "board.h"
#include "nvds.h"
#include "ob_config.h"
#include "omble.h"
#include "epd.h"
#include "ble_epd.h"
#include "led.h"

static void epd_power_idle(void)
{
#if !EPD_EN_ALWAYS_ON
    /* Panel truly off between refreshes: EN low.  epd_init() re-enables it. */
    board_epd_enable(0);
#endif
}

int main(void)
{
    struct ob_stack_param param = {
        .max_connection = OB_LE_HOST_CONNECTION_NB,
        .max_ext_adv_set = OB_LE_HOST_ADV_SET_NUM,
        .max_att_mtu = OB_LE_HOST_ATT_MTU,
        .max_gatt_serv_num = OB_LE_HOST_MAX_GATT_SERV_NUM,
        .max_gatt_write_cache = OB_LE_HOST_ATT_WRITE_CACHE_SIZE,
        .smp_sc_support = OB_LE_HOST_SC_PAIRING,
    };

    drv_wdt_init(0);
    board_init();
    led_init();
    drv_rf_init();
    evt_init();
    evt_timer_init();
    nvds_init(NVDS_FLASH_BASE);
    omble_init(&param);

    ble_epd_init();
    epd_power_idle();

    /* Power management (sleep between radio events), vendor ble_app_simple
       pattern.  DISABLED BY DEFAULT: entering PM sleep broke ongoing SPI
       transactions (drv_spi_transfer froze spinning on the TX FIFO while
       COMMIT was flashing the panel; see docs/POWER.md).  Re-enable with
       `make PM=1` once pm_sleep_block() guards refresh in ble_epd_task. */
#ifdef PM_SLEEP_ENABLE
    pm_sleep_enable(true);
    pm_sleep_allow(PM_ID_SHELL);
#endif

    __enable_irq();

    while (1) {
        evt_schedule();
        ble_epd_task();
        led_task();

#ifdef PM_SLEEP_ENABLE
        OM_CRITICAL_BEGIN();
        if (evt_get_all() == 0) {
            pm_power_manage();
        }
        OM_CRITICAL_END();
#endif
    }
}
