#ifndef __ZKC_LED_H__
#define __ZKC_LED_H__

#include <stdint.h>

typedef enum {
    LED_OFF = 0,
    LED_ADVERTISING,   /* blue slow blink */
    LED_CONNECTED,     /* green solid */
    LED_TRANSFER,      /* cyan fast blink */
    LED_DISPLAY,       /* red solid */
    LED_SUCCESS,       /* green solid, then back to state */
    LED_ERROR          /* red fast blink, then back to state */
} led_pattern_t;

void led_init(void);
void led_set_pattern(led_pattern_t pattern);
void led_set_connected(int connected);
void led_task(void);

#endif
