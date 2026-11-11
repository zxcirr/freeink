#ifndef __ZKC_EPD_H__
#define __ZKC_EPD_H__

#include <stdint.h>
#include "board.h"

void epd_init(void);
void epd_show(const uint8_t *black_plane, const uint8_t *red_plane);
void epd_show_indexed(const uint8_t *img);   /* 2bpp: 0 blk 1 wht 2 yel 3 red */
void epd_clear(void);

#endif
