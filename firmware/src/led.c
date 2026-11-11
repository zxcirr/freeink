/*
 * RGB status LED for boards with BOARD_HAS_RGB_LED = 1 (ZKC21V).
 * On boards without a fitted LED (ZKC42V) every function is a no-op.
 *
 * Pins from the InkTag board configs:
 *   red   GPIO4
 *   green GPIO18
 *   blue  GPIO17
 *   active low (0 = on)
 *
 * Patterns are non-blocking; call led_task() from the main loop.
 */

#include "om_driver.h"
#include "board.h"
#include "led.h"

#if !BOARD_HAS_RGB_LED

void led_init(void) {}
void led_set_pattern(led_pattern_t pattern) { (void)pattern; }
void led_set_connected(int connected) { (void)connected; }
void led_task(void) {}

#else

static const pin_config_t led_pin_config[] = {
    {PAD_LED_R, {PINMUX_PAD4_GPIO_MODE_CFG},  PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_LED_G, {PINMUX_PAD18_GPIO_MODE_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_LED_B, {PINMUX_PAD17_GPIO_MODE_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
};

static const gpio_config_t led_gpio_config[] = {
    /* Active low: high = off. */
    {OM_GPIO0, PAD_LED_R, GPIO_DIR_OUTPUT, GPIO_LEVEL_HIGH, GPIO_TRIG_NONE},
    {OM_GPIO0, PAD_LED_G, GPIO_DIR_OUTPUT, GPIO_LEVEL_HIGH, GPIO_TRIG_NONE},
    {OM_GPIO0, PAD_LED_B, GPIO_DIR_OUTPUT, GPIO_LEVEL_HIGH, GPIO_TRIG_NONE},
};

static led_pattern_t s_pattern = LED_OFF;
static uint32_t s_pattern_start_ms;
static uint8_t s_connected;
static uint8_t s_last_r, s_last_g, s_last_b;

static void led_apply(uint8_t r, uint8_t g, uint8_t b)
{
    if (r == s_last_r && g == s_last_g && b == s_last_b)
        return;
    s_last_r = r;
    s_last_g = g;
    s_last_b = b;

    drv_gpio_write(OM_GPIO0, GPIO_MASK(PAD_LED_R),
                   r ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
    drv_gpio_write(OM_GPIO0, GPIO_MASK(PAD_LED_G),
                   g ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
    drv_gpio_write(OM_GPIO0, GPIO_MASK(PAD_LED_B),
                   b ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
}

static uint32_t now_ms(void)
{
    return PMU_TIMER_TICK2MS(drv_pmu_timer_cnt_get());
}

void led_init(void)
{
    drv_pin_init(led_pin_config, sizeof(led_pin_config) / sizeof(led_pin_config[0]));
    drv_gpio_init(led_gpio_config, sizeof(led_gpio_config) / sizeof(led_gpio_config[0]));
    s_last_r = s_last_g = s_last_b = 0xFF;
    led_apply(0, 0, 0);
}

void led_set_pattern(led_pattern_t pattern)
{
    s_pattern = pattern;
    s_pattern_start_ms = now_ms();
}

void led_set_connected(int connected)
{
    s_connected = connected ? 1 : 0;
    led_set_pattern(s_connected ? LED_CONNECTED : LED_ADVERTISING);
}

void led_task(void)
{
    uint32_t t = now_ms() - s_pattern_start_ms;

    switch (s_pattern) {
    case LED_OFF:
        led_apply(0, 0, 0);
        break;

    case LED_ADVERTISING:
#if LED_IDLE_BLINK
        /* Blue, 500 ms on / 500 ms off. */
        led_apply(0, 0, ((t / 500) & 1) ? 0 : 1);
#else
        /* "lit only during work": dark while idle advertising. */
        led_apply(0, 0, 0);
#endif
        break;

    case LED_CONNECTED:
        led_apply(0, 1, 0);
        break;

    case LED_TRANSFER:
        /* Cyan, 100 ms on / 100 ms off. */
        if ((t / 100) & 1)
            led_apply(0, 0, 0);
        else
            led_apply(0, 1, 1);
        break;

    case LED_DISPLAY:
        led_apply(1, 0, 0);
        break;

    case LED_SUCCESS:
        if (t < 1000) {
            led_apply(0, 1, 0);
        } else {
            led_set_pattern(s_connected ? LED_CONNECTED : LED_ADVERTISING);
        }
        break;

    case LED_ERROR:
        if (t < 1200) {
            led_apply(((t / 100) & 1) ? 0 : 1, 0, 0);
        } else {
            led_set_pattern(s_connected ? LED_CONNECTED : LED_ADVERTISING);
        }
        break;

    default:
        led_set_pattern(LED_OFF);
        break;
    }
}

#endif /* BOARD_HAS_RGB_LED */
