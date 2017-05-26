/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot 2017
 * -----------------------------------------------------------------------------
 * @file       holoboard.c
 * @author     Pierrick
 * @date       Apr 23, 2017
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *   This is the top-level module of the Board Support Package (BSP) functions.
 *
 *   Hardware informations:
 *      o External Oscillator Clock :   8 MHz
 *      o Internal System Clock     : 192 MHz
 *
 *   Hardware peripherals resources usage:
 *      o TIM1                  for [MOT] Main Motors DIR and PWM channels 1 to 4
 *      o TIM3 / TIM4           for [QUA] Quadrature Encoders channels A (1) and B (2)
 *      o TIM5 / TIM8           for [ASV] Analog Servos PWM channels 1 to 8
 *      o TIM6                  for [SYS] Run-Time statistics
 *      o SPI4                  for [HMI] Human Machine Interface
 *      o CAN1                  for [CAN] CAN bus Interface
 *      o USART1                for [DBG] Debug USART
 *      o USART2                for [RS4] RS485 bus Interface
 *      o USART3                for [DSV] Digital Servo bus Interface
 *      o ADC1 / TIM2 / DMA2    for [MON] Analog monitoring (Scan-mode with DMA, auto)
 *      o USB_OTG               for [USB] USB OTG High-Speed 2.0 Interface
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

/* Inclusions */
#include "holoboard.h"

/**
  * @brief  Main initializations for the holoboard modules.
  * @param  None
  * @retval None
  */
void hb_init(void)
{
    /* System Config */
    hb_sys_cpu_cache_enable();
    hb_system_clock_config();

    /* Modules without custom-configuration */
    hb_pwr_init();
    hb_led_init();
    hb_lcmxo2_init();
    hb_irsensor_init();
    hb_endstop_init();
    hb_turret_init();

    hb_pwr_enable(HB_PWR_VP1);
    hb_pwr_enable(HB_PWR_VP2);
    hb_pwr_enable(HB_PWR_VP3);

    /* Set Interrupt group priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

/**
  * @brief  Apply the power-up sequence
  * @param  None
  * @retval None
  */
void hb_power_up(void)
{
    /* Enable powers */
    hb_pwr_enable(HB_PWR_VP1);
    hb_pwr_enable(HB_PWR_VP2);
    hb_pwr_enable(HB_PWR_VP3);
}

/**
  * @brief  Apply the power-down sequence
  * @param  None
  * @retval None
  */
void hb_power_down(void)
{
    /* Disable Power */
    hb_pwr_disable(HB_PWR_VP1);
    hb_pwr_disable(HB_PWR_VP2);
    hb_pwr_disable(HB_PWR_VP3);
}
