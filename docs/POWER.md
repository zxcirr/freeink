# FreeInk power notes

Measured on ZKC21V bench supply 3.3 V (2026-11-06):

| state | current |
|---|---|
| idle, advertising (100 ms interval, blue LED 500 ms blink) | 3.0 mA |
| BLE image transfer | 3.0 mA |
| panel refresh window | 9.0 mA avg (0.3 A peak) |

Interpretation after review: the 4.2 board (no LED) also idles at 3 mA, so the LED was not
the main cost. Root causes fixed in 2026-11-06 build:

1. **CPU never slept.** The app main loop lacked `pm_sleep_enable(true)` +
   `pm_power_manage()` (the pattern the vendor ble_app_simple uses). Added.
2. **Panel EN stayed high between refreshes.** EN is now low at boot and after every
   refresh; epd_init re-enables with a 20 ms boost-circuit settling delay.

Expected idle after the fix: well under 1 mA (BLE 100 ms legacy advertising only).
LED blink on 2.13 still adds ~1-2 mA average while advertising — consider slower/short blink.

Battery estimates (18650 3000 mAh, 85 % usable, LDO ≈ 1:1 in current):

| firmware state | avg current | runtime |
|---|---|---|
| as-is | ~3 mA | ~1 month |
| LED off + adv 3-5 s | ~0.5-1 mA | ~4-8 months |
| deep-sleep duty cycle (v1 boot phase) | ~0.1 mA | >1 year (self-discharge bound) |

Refresh energy is negligible: 9 mA x 20 s = 0.05 mAh per image.

TODO: measure idle without LED, measure ZKC42V idle, logging of ADC battery voltage once host reads it.


## Incident 2026-11-06: PM sleep froze SPI during panel refresh

Symptom: COMMIT/CLEAR acked over BLE but DONE never arrived; panel dead.
J-Link halt while stuck showed PC parked in `drv_spi_transfer` TX-FIFO
polling (`drv_spi_transfer+0x104`), i.e. the SPI peripheral had stopped
while the CPU was still looping. A/B vs the previous golden build isolated
the change "enable pm sleep in main loop": BLE-driven light/deep sleep cuts
the high-speed clock under an in-flight synchronous SPI transaction.

Decisions:
- PM is now opt-in (`make PM=1`, macro PM_SLEEP_ENABLE); default builds keep
  the verified behaviour (no system sleep). Idle current stays ~3 mA.
- Proper fix before re-enabling: guard refresh with pm_sleep_block()/
  pm_sleep_allow() around epd_show()/epd_clear() in ble_epd_task, then
  re-benchmark idle current. (TODO)
- Related lesson recorded: EPD EN must be asserted from board init (panel
  boost-capacitor inrush at runtime can dip the rail as well).

Debug tail (`make EPDDBG=1`, macro EPD_DEBUG): DONE gains 12 bytes:
init/refresh busy-timeout bitmaps, elapsed (u8), BUSY level, SPI error
count (u32). Kept in-tree for future field failures.
