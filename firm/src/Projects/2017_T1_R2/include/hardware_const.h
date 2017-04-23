/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot 2017
 * -----------------------------------------------------------------------------
 * @file       hardware_const.h
 * @author     Pierrick
 * @date       Apr 23, 2017
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *   This file contains some hardware constant definitions
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

#ifndef __HARDWARE_CONST_H
#define __HARDWARE_CONST_H



/**
********************************************************************************
**
**  LED
**
********************************************************************************
*/

/* LED PWM Period and Brightness value (global), ticks unit*/
#define LED_PWM_PERIOD_TICK      20
#define LED_PWM_DUTY_TICK        1

/* Slow blinking periods (ticks x PWM_PERIOD unit) */
#define LED_BLINK_SLOW          100
#define LED_BLINK_FAST           10

#endif /* __HARDWARE_CONST_H */
