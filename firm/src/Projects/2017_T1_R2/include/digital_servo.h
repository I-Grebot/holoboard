/* -----------------------------------------------------------------------------
 * BlueBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file       digital_servo.h
 * @author     Bebop35
 * @date       Mar 20, 2017
 * -----------------------------------------------------------------------------
 * @brief
 *   Digital servo module definitions
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/blueboard.git
 * -----------------------------------------------------------------------------
 */

#ifndef __DIGITAL_SERVO_H_
#define __DIGITAL_SERVO_H_

#include "FreeRTOS.h"
#include "holoboard.h"


typedef struct {

    /* Uart configuration handler for the DSV channel */
    USART_InitTypeDef uart;

    /* Uart RX and TX queues */
    QueueHandle_t rx_queue;
    QueueHandle_t tx_queue;

    /* Dynamixel Interface */
    dxl_interface_t dxl;

    // TODO: add mutex (maybe?)

} dsv_channel_t;

#endif /* __DIGITAL_SERVO_H_ */

