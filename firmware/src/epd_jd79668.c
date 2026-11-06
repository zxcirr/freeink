/*
 * JD79668AA 400x300 BWR (3-colour black/white/red) driver, ZKC42V.
 *
 * The controller is JD79668AA (SSD1680-compatible command set), the fitted
 * panel is 3-colour BWR (user-confirmed).  Facts from the InkTag host tool
 * sources + OM6626 app disassembly (kind 0x23 branch, colours==2):
 *   - init:    0x12 (SW reset, wait BUSY), 0x01(h-1, 3 bytes), 0x3C 0x01,
 *              0x18 0x80, 0x11 0x03, window 0x44/0x45/0x4E/0x4F,
 *              0x22 0xB9 + 0x20, wait BUSY
 *   - refresh: planes, 0x21 0x00 0x40, 0x22 0xF7 + 0x20, wait BUSY (30 s)
 *
 * Plane convention (identical to the working ZKC21V driver):
 *   black plane: 1 = white, 0 = black   -> written to 0x24 as-is
 *   red plane:   1 = no red, 0 = red    -> inverted, then 0x26 (1 = red)
 */

#include "om_driver.h"
#include "board.h"
#include "epd.h"

#define X_BYTES   (EPD_WIDTH / 8)          /* 50 */

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
    uint32_t left = EPD_PLANE_BYTES;   /* one 1bpp plane */

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

/* BUSY is high while the panel works (SSD1680-compatible). */
static void epd_wait_busy(uint32_t timeout_ms)
{
    DRV_DELAY_MS(2);
    while (board_epd_busy() && timeout_ms) {
        DRV_DELAY_MS(1);
        timeout_ms--;
    }
}

void epd_init(void)
{
    board_epd_enable(1);
    DRV_DELAY_MS(10);

    board_epd_reset(0);
    DRV_DELAY_MS(20);
    board_epd_reset(1);
    DRV_DELAY_MS(10);

    epd_cmd(0x12);                  /* software reset */
    epd_wait_busy(5000);

    epd_cmd(0x01);                  /* driver output: EPD_HEIGHT-1 gates */
    epd_data1((EPD_HEIGHT - 1) & 0xFF);
    epd_data1((EPD_HEIGHT - 1) >> 8);
    epd_data1(0x00);

    epd_cmd(0x3C);                  /* border waveform (BWRY: 0x01) */
    epd_data1(0x01);

    epd_cmd(0x18);                  /* internal temperature */
    epd_data1(0x80);

    epd_cmd(0x11);                  /* data entry: X inc, Y inc */
    epd_data1(0x03);

    epd_cmd(0x44);                  /* RAM X window 0..49 */
    epd_data1(0x00);
    epd_data1(X_BYTES - 1);

    epd_cmd(0x45);                  /* RAM Y window 0..299 */
    epd_data1(0x00);
    epd_data1(0x00);
    epd_data1((EPD_HEIGHT - 1) & 0xFF);
    epd_data1((EPD_HEIGHT - 1) >> 8);

    epd_cmd(0x4E);                  /* X counter = 0 */
    epd_data1(0x00);

    epd_cmd(0x4F);                  /* Y counter = 0 */
    epd_data1(0x00);
    epd_data1(0x00);

    epd_cmd(0x22);                  /* clock/init cycle for BWRY: 0xB9 */
    epd_data1(0xB9);
    epd_cmd(0x20);
    epd_wait_busy(5000);
}

/* Unpack helpers were used by the BWRY experiment; the BWR panel uses the
 * plain dual-plane path above. */
static void epd_refresh(void)
{
    epd_cmd(0x21);                  /* display update control (JD79668/BWR) */
    epd_data1(0x00);
    epd_data1(0x40);

    epd_cmd(0x22);                  /* display update sequence 0xF7 */
    epd_data1(0xF7);
    epd_cmd(0x20);
    epd_wait_busy(30000);           /* InkTag allows 30 s for this panel */
}

void epd_show(const uint8_t *black_plane, const uint8_t *red_plane)
{
    epd_init();

    epd_cmd(0x24);                  /* black/white RAM, native polarity */
    epd_data(black_plane, EPD_PLANE_BYTES);

    epd_cmd(0x26);                  /* red RAM (inverted like the 2.1") */
    epd_data_inverted(red_plane, EPD_PLANE_BYTES);

    epd_refresh();

    epd_cmd(0x10);                  /* deep sleep */
    epd_data1(0x01);
}

/* 2bpp indexed display is not used on this BWR panel; keeps the API. */
void epd_show_indexed(const uint8_t *img)
{
    (void)img;
    epd_clear();
}

void epd_clear(void)
{
    epd_init();

    epd_fill(0x24, 0xFF);           /* all white */
    epd_fill(0x26, 0x00);           /* no red */

    epd_refresh();

    epd_cmd(0x10);                  /* deep sleep */
    epd_data1(0x01);
}
