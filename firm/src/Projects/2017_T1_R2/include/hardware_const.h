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


/**
********************************************************************************
**
**  Digital Servos
**
********************************************************************************
*/

// Queues size for each channel, in Bytes
#define DSV_RX_QUEUE_SIZE   64
#define DSV_TX_QUEUE_SIZE   64

// Timeouts
#define DSV_RX_TIMEOUT      pdMS_TO_TICKS( 10 ) // Must be at least longest TX + Return-time delay
#define DSV_TX_TIMEOUT      pdMS_TO_TICKS(  5 )

// Servos IDs
#define DSV_GRABBER_LEFT_ID     10  // TBC
#define DSV_GRABBER_RIGHT_ID    20
#define DSV_LANDER_RIGHT_ID     30

// Positions
#define DSV_GRABBER_LEFT_POS_OPENED    100 // TBC
#define DSV_GRABBER_LEFT_POS_CLOSED    800
#define DSV_GRABBER_RIGHT_POS_OPENED   900
#define DSV_GRABBER_RIGHT_POS_CLOSED   200
#define DSV_LANDER_POS_UP              400
#define DSV_LANDER_POS_DOWN            600

// Speeds
#define DSV_GRABBERS_SPEED             500 // TBC
#define DSV_LANDER_SPEED_UP            800
#define DSV_LANDER_SPEED_DOWN          300

// Torques
#define DSV_GRABBERS_TORQUE            800 // TBC


/**
********************************************************************************
**
**  Serial Interface
**
********************************************************************************
*/

#define SERIAL_BAUDRATE         115200

#define SERIAL_ISR              DBG_ISR
#define SERIAL_COM              DBG_COM

#define XL_ISR              	DSV_ISR

#define SERIAL_RX_QUEUE_LEN     64
#define SERIAL_TX_QUEUE_LEN    256 // That's only because we like to transmit

#define SERIAL_RX_TIMEOUT      pdMS_TO_TICKS( 10 )
#define SERIAL_TX_TIMEOUT      pdMS_TO_TICKS( 10 )

#endif /* __HARDWARE_CONST_H */
