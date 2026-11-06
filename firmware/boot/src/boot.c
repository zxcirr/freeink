/*
 * FreeInk bootloader v0 (slot selector) — runs at 0x404000.
 *
 * Flash layout (Route A):
 *   0x404000  boot          64 KB
 *   0x414000  app slot A   192 KB
 *   0x444000  app slot B   192 KB
 *   0x480000  MBR (SDK-managed, do not touch)
 *   0x4F0000  NVDS
 *
 * v0 behaviour: validate slot A, then slot B (vector-table sanity), jump
 * to the first one that looks alive; if neither does, blink red fast as a
 * rescue signal.  BLE DFU and the swap record arrive in v1 — v0 exists to
 * prove the relocation chain (boot -> moved app) end-to-end.
 */

#include "om.h"
#include "om_driver.h"

#define PAD_LED_R 4   /* RGB LED red, active low (ZKC21V) */

#define SLOT_A_ADDR     0x414000u
#define SLOT_A_SIZE     0x30000u      /* 192K */
#define SLOT_B_ADDR     0x444000u
#define SLOT_B_SIZE     0x30000u

#define RAM_LOW         0x20000000u
#define RAM_HIGH        0x20001400u   /* conservative: 80 KB SRAM top */

static int slot_valid(uint32_t base, uint32_t size)
{
    const volatile uint32_t *v = (const volatile uint32_t *)base;
    uint32_t sp  = v[0];
    uint32_t rst = v[1];

    if (sp <= RAM_LOW || sp > RAM_HIGH)      return 0;
    if (rst < base + 4 || rst >= base + size) return 0;
    if ((rst & 1) == 0)                       return 0;  /* Thumb bit */

    /* Vector table must start with at least a few non-erased entries. */
    if (v[2] == 0xFFFFFFFF && v[3] == 0xFFFFFFFF && v[4] == 0xFFFFFFFF)
        return 0;
    return 1;
}

static void jump_to(uint32_t base)
{
    const uint32_t *v = (const uint32_t *)base;
    uint32_t sp  = v[0];
    uint32_t rst = v[1] & ~1u;

    SCB->VTOR = base;
    __set_MSP(sp);
    ((void (*)(void))rst)();
}

static void rescue_blink(void)
{
    pin_config_t pc = { PAD_LED_R, {PINMUX_PAD4_GPIO_MODE_CFG},
                        PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL };
    gpio_config_t gc = { OM_GPIO0, PAD_LED_R, GPIO_DIR_OUTPUT,
                         GPIO_LEVEL_HIGH, GPIO_TRIG_NONE };
    drv_pin_init(&pc, 1);
    drv_gpio_init(&gc, 1);

    while (1) {
        drv_gpio_write(OM_GPIO0, GPIO_MASK(PAD_LED_R), GPIO_LEVEL_LOW);
        DRV_DELAY_MS(80);
        drv_gpio_write(OM_GPIO0, GPIO_MASK(PAD_LED_R), GPIO_LEVEL_HIGH);
        DRV_DELAY_MS(80);
    }
}

int main(void)
{
    /* Boot runs with whatever clock the OEM boot set up (like the app did
       when it lived at 0x404000).  Keep the boot out of anything that may
       have side effects; we only read flash and jump. */

    if (slot_valid(SLOT_A_ADDR, SLOT_A_SIZE)) {
        jump_to(SLOT_A_ADDR);
    } else if (slot_valid(SLOT_B_ADDR, SLOT_B_SIZE)) {
        jump_to(SLOT_B_ADDR);
    }

    rescue_blink();
    return 0;
}
