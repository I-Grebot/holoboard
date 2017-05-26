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

/* Components libraries */
#include "dynamixel.h"


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

/* List of available power enables */
typedef enum {
    HB_PWR_VP1 = 0,
    HB_PWR_VP2 = 1,
    HB_PWR_VP3 = 2
} HB_PWR_TypeDef;


/* List of TURRET frame states */
/* SPI CSn is Active Low when actively transmitting a frame */
typedef enum {
    HB_TURRET_FRAME_IDLE   = (Bit_SET),
    HB_TURRET_FRAME_ACTIVE = (Bit_RESET)
} HB_HMI_FrameTypeDef;

/* List of available Digital Servo channels */
typedef enum {
    HB_DSV_CHANNEL1 = 1,
	HB_DSV_CHANNEL2 = 2
} HB_DSV_ChannelTypeDef;

/**
********************************************************************************
**
**  Prototypes
**
********************************************************************************
*/

/* Hardware Top */
void hb_init(void);
void hb_power_up(void);
void hb_power_down(void);

/* System */
void hb_system_clock_config(void);
void hb_sys_cpu_cache_enable(void);
void hb_sys_timer_run_time_config();
uint32_t hb_sys_timer_get_run_time_ticks(void);

/* Power modules */
void hb_pwr_init(void);
void hb_pwr_enable(HB_PWR_TypeDef power);
void hb_pwr_disable(HB_PWR_TypeDef power);

/* RGB LED */
void hb_led_init(void);
void hb_led_set_color(HB_LED_ColorTypeDef color);

/* FPGA LCMXO2 */
void hb_lcmxo2_init(void);
uint16_t hb_lcmxo2_tx_rx(uint16_t value);
void hb_lcmxo2_set_pwm(uint16_t motor, int16_t value);
int16_t hb_lcmxo2_get_qei(uint16_t encoder);

/* Turret */
void hb_turret_init(void);

/* Debug Interface */
void hb_dbg_init(USART_InitTypeDef * USART_InitStruct);
void hb_dbg_enable(uint32_t nvic_priority);
void hb_dbg_disable(void);

/* IR Sensors */
void hb_irsensor_init(void);

/* Endstop */
void hb_endstop_init(void);

/* Digital Servo */
void hb_dsv_init(uint8_t dsv_chan, USART_InitTypeDef * USART_InitStruct);
void hb_dsv_enable(uint8_t dsv_chan, uint32_t nvic_priority);
void hb_dsv_disable(uint8_t dsv_chan);
void hb_dsv_switch(uint8_t dsv_chan, dxl_switch_mode_e mode);
void hb_dsv_put(uint8_t dsv_chan, uint8_t ch);

#ifdef __cplusplus
}
#endif

#endif /* __HOLOBOARD_H */
