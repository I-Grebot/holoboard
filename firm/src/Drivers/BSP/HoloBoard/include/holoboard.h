/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot 2017
 * -----------------------------------------------------------------------------
 * @file       holoboard.h
 * @author     Pierrick
 * @date       Apr 23, 2017
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *    This file is the main header of the holoboard BSP
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

#ifndef __HOLOBOARD_H
#define __HOLOBOARD_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
********************************************************************************
**
**  Inclusions
**
********************************************************************************
*/

/* Main Configuration file for STM32F7xx devices
 * This will also include the stm32f7xx_hal_conf file which defines
 * the SPL module used. */
#include <stm32f7xx.h>

/* Defines the HW mapping of the board and some useful macros */
#include "hb_io_mapping.h"

/* External configuration of the BlueBoard */
#include "hb_config.h"


 /**
 ********************************************************************************
 **
 **  Static Peripherals Definitions
 **
 ********************************************************************************
 */

 /* Timer to be used for run-time statistics */
 #define SYS_RUNSTATS_TIM                    TIM6
 #define SYS_RUNSTATS_TIM_CLK_ENABLE()       RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE)
 #define SYS_RUNSTATS_TIM_CLK_DISABLE()      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, DISABLE)
 #define SYS_RUNSTATS_IRQn                   TIM6_DAC_IRQn
 #define SYS_RUNSTATS_ISR                    TIM6_DAC_IRQHandler

/**
********************************************************************************
**
**  Enumerations
**
********************************************************************************
*/

/* List of LED colors */
typedef enum {
    HB_LED_OFF      = 0, /* All OFF */
    HB_LED_RED      = 1,
    HB_LED_GREEN    = 2,
    HB_LED_BLUE     = 3,
    HB_LED_CYAN     = 4, /* Blue + Green */
    HB_LED_YELLOW   = 5, /* Red + Green */
    HB_LED_MAGENTA  = 7, /* Blue + Red */
    HB_LED_WHITE    = 8, /* Red + Green + Blue */
} HB_LED_ColorTypeDef;

/* List of LED modes */
typedef enum {
    HB_LED_STATIC       = 0,
    HB_LED_BLINK_SLOW   = 1,
    HB_LED_BLINK_FAST   = 2
} HB_LED_ModeTypeDef;


/**
********************************************************************************
**
**  Prototypes
**
********************************************************************************
*/

/* Hardware Top */
void hb_init(void);

/* System */
void hb_system_clock_config(void);
void hb_sys_cpu_cache_enable(void);
void hb_sys_timer_run_time_config();
uint32_t hb_sys_timer_get_run_time_ticks(void);

/* RGB LED */
void hb_led_init(void);
void hb_led_set_color(HB_LED_ColorTypeDef color);

/* Digital I/Os */
void hb_dio_init(void);


#ifdef __cplusplus
}
#endif

#endif /* __HOLOBOARD_H */
