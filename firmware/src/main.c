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

    __enable_irq();

    while (1) {
        evt_schedule();
        ble_epd_task();
        led_task();
    }
}
