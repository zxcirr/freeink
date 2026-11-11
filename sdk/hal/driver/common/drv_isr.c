#include "RTE_OM662x.h"
#if (RTE_ISR)
#include <stdint.h>
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * CONST & VARIABLES
 */


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */

__WEAK void NMI_Handler(void)
{
    while(1);
}

__WEAK void HardFault_Handler(void)
{
    while(1);
}

__WEAK void MemManage_Handler(void)
{
    while(1);
}

__WEAK void BusFault_Handler(void)
{
    while(1);
}

__WEAK void UsageFault_Handler(void)
{
    while(1);
}

__WEAK void SoftFault_Handler(void)
{
    __disable_irq();
    while(1);
}

__WEAK void SVC_Handler(void)
{
}

__WEAK void DebugMon_Handler(void)
{
}

__WEAK void PendSV_Handler(void)
{
}

__WEAK void SysTick_Handler(void)
{
}

__WEAK void BT_IRQHandler(void)
{
    #ifdef CONFIG_BLE_CONTROLLER
    void obc_isr(void);
    obc_isr();
    #endif
}

__WEAK void BT_WAKEUP_IRQHandler(void)
{
    while(1);
}

__WEAK void DMA_IRQHandler(void)
{
    #if (RTE_DMA)
    drv_dma_isr();
    #endif
}

__WEAK void PIN_WAKEUP_IRQHandler(void)
{
    #if (RTE_PMU)
    drv_pmu_pin_wakeup_isr();
    #endif
}

__WEAK void TIM_IRQHandler(void)
{
    while(1);
}

__WEAK void OM24G_RF_IRQHandler(void)
{
    #if (RTE_OM24G)
    drv_om24g_isr();
    #endif
}

__WEAK void PMU_POF_IRQHandler(void)
{
    while(1);
}

__WEAK void PMU_TIMER_IRQHandler(void)
{
    #if (RTE_PMU_TIMER)
    drv_pmu_timer_isr();
    #endif
}

__WEAK void WDT_IRQHandler(void)
{
    #if (RTE_WDT)
    drv_wdt_isr();
    #endif
}

__WEAK void UART1_IRQHandler(void)
{
    #if (RTE_USART1)
    drv_usart_isr(OM_USART1);
    #endif
}

__WEAK void EFUSE_IRQHandler(void)
{
    while(1);
}

__WEAK void GPIO_IRQHandler(void)
{
    #if (RTE_GPIO0)
    drv_gpio_isr(OM_GPIO0);
    #endif
}

__WEAK void ADC_IRQHandler(void)
{
    #if (RTE_GPADC)
    drv_adc_isr();
    #endif
}

__WEAK void I2C0_IRQHandler(void)
{
    #if (RTE_I2C0)
    drv_i2c_isr(OM_I2C0);
    #endif
}

__WEAK void SF_IRQHandler(void)
{
    #if (RTE_SF)
    drv_sfb_isr(OM_SF, 0);
    #endif
}

__WEAK void SOFT0_IRQHandler(void)
{
    while(1);
}

__WEAK void SOFT1_IRQHandler(void)
{
    while(1);
}

__WEAK void SOFT2_IRQHandler(void)
{
    while(1);
}

__WEAK void SOFT3_IRQHandler(void)
{
    while(1);
}

__WEAK void SOFT4_IRQHandler(void)
{
    while(1);
}

__WEAK void SOFT5_IRQHandler(void)
{
    while(1);
}

__WEAK void SOFT6_IRQHandler(void)
{
    while(1);
}

__WEAK void SOFT7_IRQHandler(void)
{
    while(1);
}

__WEAK void VTRACK_IRQHandler(void)
{
    while(1);
}

__WEAK void CRY32M_RDY_IRQHandler(void)
{
    while(1);
}

__WEAK void UART0_IRQHandler(void)
{
    #if (RTE_USART0)
    drv_usart_ex_isr(OM_USART0);
    #endif
}

__WEAK void SPI0_IRQHandler(void)
{
    #if (RTE_SPI0)
    drv_spi_isr(OM_SPI0);
    #endif
}

__WEAK void CC_INTR_IRQHandler(void)
{
    while(1);
}

__WEAK void TIM0_IRQHandler(void)
{
    #if (RTE_TIM0)
    drv_tim_isr(OM_TIM0);
    #endif
}

__WEAK void TIM1_IRQHandler(void)
{
    #if (RTE_TIM1)
    drv_tim_isr(OM_TIM1);
    #endif
}

__WEAK void TIM2_IRQHandler(void)
{
    #if (RTE_TIM2)
    drv_tim_isr(OM_TIM2);
    #endif
}

__WEAK void SPI1_IRQHandler(void)
{
    #if (RTE_SPI1)
    drv_spi_isr(OM_SPI1);
    #endif
}

__WEAK void RTC_1HZ_IRQHandler(void)
{
    #if (RTE_RTC)
    drv_rtc_second_isr(OM_RTC);
    #endif
}

__WEAK void RTC_AF_IRQHandler(void)
{
    #if (RTE_RTC)
    drv_rtc_alarm_isr(OM_RTC);
    #endif
}

__WEAK void LP_TIMER_IRQHandler(void)
{
    #if (RTE_LP_TIM)
    drv_lp_tim_isr(OM_LP_TIM);
    #endif
}

#endif /* (RTE_NVIC) */

/** @} */
