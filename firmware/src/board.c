#include "om.h"
#include "om_driver.h"
#include "board.h"

static const pin_config_t pin_config[] = {
    {PAD_EPD_SCK,  {PINMUX_PAD15_SPI1_SCK_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_EPD_MOSI, {PINMUX_PAD21_SPI1_DO_CFG},  PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_EPD_CS,   {PINMUX_PAD22_SPI1_CS_CFG},  PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_EPD_DC,   {PINMUX_PAD20_GPIO_MODE_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_EPD_RST,  {PINMUX_PAD16_GPIO_MODE_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_EPD_BUSY, {PINMUX_PAD3_GPIO_MODE_CFG},  PMU_PIN_MODE_PU, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_EPD_EN,   {PINMUX_PAD7_GPIO_MODE_CFG},  PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
};

static const gpio_config_t gpio_config[] = {
    {OM_GPIO0, PAD_EPD_DC,   GPIO_DIR_OUTPUT, GPIO_LEVEL_LOW,  GPIO_TRIG_NONE},
    {OM_GPIO0, PAD_EPD_RST,  GPIO_DIR_OUTPUT, GPIO_LEVEL_HIGH, GPIO_TRIG_NONE},
    /* EN asserted at init: the panel boost capacitors charge during the
       board power-on window, NOT during a runtime SPI transaction.  Toggling
       EN later caused rail dips that froze the SPI peripheral (dead wait in
       drv_spi_transfer TX polling). */
    {OM_GPIO0, PAD_EPD_EN,   GPIO_DIR_OUTPUT, GPIO_LEVEL_HIGH, GPIO_TRIG_NONE},
    {OM_GPIO0, PAD_EPD_BUSY, GPIO_DIR_INPUT,  GPIO_LEVEL_HIGH, GPIO_TRIG_NONE},
};

void board_init(void)
{
    drv_pmu_dcdc_enable(true);
    drv_pmu_select_32k(PMU_32K_SEL_RC);

    /* Keep unused pins in a defined state, as the vendor EVB does. */
    drv_pmu_pin_mode_set(PMU_PIN_ALL_MASK, PMU_PIN_MODE_PU);
    drv_pmu_pin_driven_current_set(PMU_PIN_ALL_MASK, PMU_PIN_DRIVER_CURRENT_NORMAL);

    drv_pin_init(pin_config, sizeof(pin_config) / sizeof(pin_config[0]));
    drv_gpio_init(gpio_config, sizeof(gpio_config) / sizeof(gpio_config[0]));

    spi_config_t cfg = {
        .freq      = 8 * 1000 * 1000,
        .mode      = SPI_MODE_0,
        .role      = SPI_ROLE_MASTER,
        .wire      = SPI_WIRE_3,
        .first_bit = SPI_MSB_FIRST,
        .cs_valid  = SPI_CS_LOW,
    };
    drv_spi_init(OM_SPI1, &cfg);
}

void board_epd_enable(int on)
{
    drv_gpio_write(OM_GPIO0, GPIO_MASK(PAD_EPD_EN),
                   on ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

void board_epd_reset(int high)
{
    drv_gpio_write(OM_GPIO0, GPIO_MASK(PAD_EPD_RST),
                   high ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

void board_epd_dc(int data)
{
    drv_gpio_write(OM_GPIO0, GPIO_MASK(PAD_EPD_DC),
                   data ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

int board_epd_busy(void)
{
    return drv_gpio_read(OM_GPIO0, GPIO_MASK(PAD_EPD_BUSY)) != 0;
}

#ifdef EPD_DEBUG
volatile uint32_t s_spi_timeout;
#endif

void board_epd_write(const uint8_t *data, uint16_t len)
{
    om_error_t err = drv_spi_transfer(OM_SPI1, data, len, NULL, 0, 1000);
#ifdef EPD_DEBUG
    if (err != OM_ERROR_OK)
        s_spi_timeout++;
#else
    (void)err;
#endif
}
