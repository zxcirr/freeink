#ifndef __ZKC_BOARD_H__
#define __ZKC_BOARD_H__

#include "om_driver.h"

/*
 * ZKC21V / ZKC42V shared EPD connector (from InkTag board configs):
 *
 *   SCK   GPIO15  (SPI1_SCK)
 *   MOSI  GPIO21  (SPI1_DO)
 *   CS    GPIO22  (SPI1_CS)
 *   DC    GPIO20  (GPIO)
 *   RST   GPIO16  (GPIO)
 *   BUSY  GPIO3   (GPIO input)
 *   EN    GPIO7   (GPIO output, active high)
 *
 * The panel is write-only, so SPI1 is used in 3-wire mode.
 */

#define PAD_EPD_SCK     15
#define PAD_EPD_MOSI    21
#define PAD_EPD_CS      22
#define PAD_EPD_DC      20
#define PAD_EPD_RST     16
#define PAD_EPD_BUSY    3
#define PAD_EPD_EN      7

/* RGB status LED, active low (ZKC21V only; BOARD_HAS_RGB_LED gates it). */
#define PAD_LED_R       4
#define PAD_LED_G       18
#define PAD_LED_B       17

#if defined(BOARD_ZKC21V)
#define BLE_NAME        "ZKC21V"
#define EPD_WIDTH       128
#define EPD_HEIGHT      250
#define BOARD_HAS_RGB_LED 1
/* format 2: two 1bpp planes (black/white + red). */
#define EPD_DEFAULT_FORMAT 2
#elif defined(BOARD_ZKC42V)
#define BLE_NAME        "ZKC42V"
#define EPD_WIDTH       400
#define EPD_HEIGHT      300
/* No RGB LED fitted on the ZKC42V board (user-confirmed). */
#define BOARD_HAS_RGB_LED 0
/* Same wire format as the 2.1": two 1bpp planes (black/white + red). */
#define EPD_DEFAULT_FORMAT 2
#else
#error "Define BOARD_ZKC21V or BOARD_ZKC42V"
#endif

#ifndef BOARD_HAS_RGB_LED
#define BOARD_HAS_RGB_LED 0
#endif

#define NVDS_FLASH_BASE 0xF0000u

#define EPD_PLANE_BYTES (((EPD_WIDTH + 7) / 8) * EPD_HEIGHT)
#define EPD_IMAGE_BYTES (EPD_PLANE_BYTES * 2)

void board_init(void);
void board_epd_enable(int on);
void board_epd_reset(int high);
void board_epd_dc(int data);
int board_epd_busy(void);
void board_epd_write(const uint8_t *data, uint16_t len);

#endif
