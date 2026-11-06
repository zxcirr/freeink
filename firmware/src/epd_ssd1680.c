/*
 * SSD1680 128x250 BWR panel driver (ZK_EL_OM6626_2.13_V1.1).
 *
 * Sequence follows the panel driver in the stock firmware:
 *
 *   reset, 0x12 (SW reset, wait BUSY), 0x3C 0x05, 0x18 0x80,
 *   0x22 0xB1 + 0x20 (clock/start init cycle, wait BUSY),
 *   window setup (0x01/0x11 0x01/0x44/0x45/0x4E/0x4F/0x21),
 *   0x24 black plane, 0x26 red plane (inverted),
 *   0x22 0xC7 + 0x20, wait BUSY, 0x10 0x01 deep sleep.
 *
 * IMPORTANT: after any 0x20 activation the panel must be idle (BUSY
 * low) before further commands are accepted.  Fixed short delays are
 * NOT sufficient; we poll BUSY with a generous timeout.
 *
 * Plane convention on the wire / from the host:
 *   black plane: 1 = white, 0 = black   (native SSD1680 0x24 order)
 *   red plane:   1 = no red, 0 = red    (inverted before 0x26)
 */

#include "om_driver.h"
#include "board.h"
#include "epd.h"

static void epd_cmd(uint8_t cmd)
{
    board_epd_dc(0);
    board_epd_write(&cmd, 1);
}

static void epd_data(const uint8_t *data, uint16_t len)
{
    board_epd_dc(1);
    board_epd_write(data, len);
}

static void epd_data1(uint8_t value)
{
    epd_data(&value, 1);
}

static void epd_data_inverted(const uint8_t *data, uint16_t len)
{
    uint8_t buf[256];

    board_epd_dc(1);
    while (len) {
        uint16_t n = len > sizeof(buf) ? sizeof(buf) : len;
        for (uint16_t i = 0; i < n; i++)
            buf[i] = (uint8_t)~data[i];
        board_epd_write(buf, n);
        data += n;
        len -= n;
    }
}

static void epd_fill(uint8_t cmd, uint8_t value)
{
    uint8_t buf[256];
    uint32_t left = EPD_PLANE_BYTES;

    for (uint32_t i = 0; i < sizeof(buf); i++)
        buf[i] = value;

    epd_cmd(cmd);
    board_epd_dc(1);
    while (left) {
        uint16_t n = left > sizeof(buf) ? sizeof(buf) : (uint16_t)left;
        board_epd_write(buf, n);
        left -= n;
    }
}

/*
 * SSD1680: BUSY high while the panel is working.  Wait until BUSY
 * falls, polling every millisecond, up to timeout_ms.
 */
static uint32_t s_busy_to;      /* per-wait timeout history (bit shifts) */

static void epd_wait_busy(uint32_t timeout_ms)
{
    DRV_DELAY_MS(2);            /* let BUSY rise after a 0x20 command */
    while (board_epd_busy() && timeout_ms) {
        DRV_DELAY_MS(1);
        timeout_ms--;
    }
    s_busy_to = (s_busy_to << 1) | (board_epd_busy() ? 1 : 0);
}

/* Per-frame window + mode setup (runs only while the panel is idle). */
static void epd_config(void)
{
    epd_cmd(0x01);              /* driver output: 250 gate lines, 3 bytes */
    epd_data1(0xF9);
    epd_data1(0x00);
    epd_data1(0x00);

    epd_cmd(0x11);              /* data entry: X increment, Y decrement */
    epd_data1(0x01);

    epd_cmd(0x44);              /* RAM X window: 0..15 (128 px / 8) */
    epd_data1(0x00);
    epd_data1(0x0F);

    epd_cmd(0x45);              /* RAM Y window: start 249 -> end 0 */
    epd_data1(0xF9);
    epd_data1(0x00);
    epd_data1(0x00);
    epd_data1(0x00);

    epd_cmd(0x4E);              /* RAM X address counter: 0 */
    epd_data1(0x00);

    epd_cmd(0x4F);              /* RAM Y address counter: 249 */
    epd_data1(0xF9);
    epd_data1(0x00);

    epd_cmd(0x3C);              /* border waveform */
    epd_data1(0x05);

    epd_cmd(0x21);              /* display update control: ignore BW RAM */
    epd_data1(0x00);
    epd_data1(0x80);

    epd_cmd(0x18);              /* internal temperature sensor */
    epd_data1(0x80);
}

/*
 * Full panel init: hardware reset + SW reset + the 0x22/0xB1 clock
 * and display init cycle.  Must end with the panel idle so that the
 * RAM writes that follow are actually accepted.
 */
#ifdef EPD_DEBUG
uint16_t s_dbg_init_to;
uint16_t s_dbg_refresh_to;
uint32_t s_dbg_init_ms;
uint32_t s_dbg_refresh_ms;
#endif

void epd_init(void)
{
#ifdef EPD_DEBUG
    uint32_t t0 = PMU_TIMER_TICK2MS(drv_pmu_timer_cnt_get());
    s_busy_to = 0;
#endif
    board_epd_enable(1);
    DRV_DELAY_MS(20);            /* let the panel boost circuit charge */

    board_epd_reset(0);
    DRV_DELAY_MS(10);
    board_epd_reset(1);
    DRV_DELAY_MS(10);

    epd_cmd(0x12);              /* software reset */
    epd_wait_busy(5000);

    epd_cmd(0x3C);              /* border waveform */
    epd_data1(0x05);

    epd_cmd(0x18);              /* internal temperature sensor */
    epd_data1(0x80);

    /* Clock + display init cycle; the stock firmware waits up to 5 s
       for BUSY here. */
    epd_cmd(0x22);
    epd_data1(0xB1);
    epd_cmd(0x20);
    epd_wait_busy(5000);
#ifdef EPD_DEBUG
    s_dbg_init_to = (uint16_t)s_busy_to;
    s_dbg_init_ms = PMU_TIMER_TICK2MS(drv_pmu_timer_cnt_get()) - t0;
#endif
}

void epd_show(const uint8_t *black_plane, const uint8_t *red_plane)
{
    epd_init();
    epd_config();

    epd_cmd(0x24);              /* black/white RAM: native polarity */
    epd_data(black_plane, EPD_PLANE_BYTES);

    epd_cmd(0x26);              /* red RAM: invert (host: 0 = red) */
    epd_data_inverted(red_plane, EPD_PLANE_BYTES);

#ifdef EPD_DEBUG
    s_busy_to = 0;
    uint32_t t1 = PMU_TIMER_TICK2MS(drv_pmu_timer_cnt_get());
#endif
    epd_cmd(0x22);              /* display update sequence 0xC7 (OTP LUT) */
    epd_data1(0xC7);
    epd_cmd(0x20);              /* master activation */
    epd_wait_busy(20000);       /* stock firmware allows 15 s for refresh */
#ifdef EPD_DEBUG
    s_dbg_refresh_to = (uint16_t)s_busy_to;
    s_dbg_refresh_ms = PMU_TIMER_TICK2MS(drv_pmu_timer_cnt_get()) - t1;
#endif

    epd_cmd(0x10);              /* deep sleep */
    epd_data1(0x01);
#if !EPD_EN_ALWAYS_ON
    DRV_DELAY_MS(2);
    board_epd_enable(0);         /* cut panel power while idle */
#endif
}

void epd_clear(void)
{
    epd_init();
    epd_config();

    epd_fill(0x24, 0xFF);       /* all white */
    epd_fill(0x26, 0x00);       /* no red */

    epd_cmd(0x22);
    epd_data1(0xC7);
    epd_cmd(0x20);
    epd_wait_busy(20000);

    epd_cmd(0x10);              /* deep sleep */
    epd_data1(0x01);
#if !EPD_EN_ALWAYS_ON
    DRV_DELAY_MS(2);
    board_epd_enable(0);         /* cut panel power while idle */
#endif
}

/*
 * 2bpp indexed display is not used by this board (INFO reports format 2);
 * provided so the common BLE code links.  Falls back to a plain clear.
 */
void epd_show_indexed(const uint8_t *img)
{
    (void)img;
    epd_clear();
}
