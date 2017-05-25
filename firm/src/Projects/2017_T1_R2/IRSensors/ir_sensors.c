/*
 * ir_sensors.c
 *
 *  Created on: May 25, 2017
 *      Author: ldoppea
 */


/* Inclusion */
#include "main.h"
#include "ir_sensors.h"

//faces sensors
uint8_t sensor0detect, sensor1detect, sensor2detect;

//angle sensors
uint8_t sensor3detect, sensor4detect, sensor5detect;

/* Local, Private functions */
static void OS_IrSensorsTask(void *pvParameters);

BaseType_t ir_sensor_start(void)
{
	sensor0detect = 0;
	sensor1detect = 0;
	sensor2detect = 0;

	sensor3detect = 0;
	sensor4detect = 0;
	sensor5detect = 0;

    return xTaskCreate(OS_IrSensorsTask, "IrSensor", OS_TASK_STACK_IR_SENSORS, NULL, OS_TASK_PRIORITY_IR_SENSORS, NULL );
}


static void OS_IrSensorsTask( void *pvParameters )
{
    TickType_t xNextWakeTime;
    char str[60];

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    for( ;; )
    {
    	sensor0detect = (IRSENSOR0_VALUE) ? 0:1;
    	sensor1detect = (IRSENSOR1_VALUE) ? 0:1;
    	sensor2detect = (IRSENSOR2_VALUE) ? 0:1;

    	sensor3detect = (IRSENSOR3_VALUE) ? 0:1;
    	sensor4detect = (IRSENSOR4_VALUE) ? 0:1;
    	sensor5detect = (IRSENSOR5_VALUE) ? 0:1;

    	if(sensor0detect)
		{
    		sprintf(str,"Sensor 0 ON \n\r");
    		serial_puts(str);
		}

    	if(sensor1detect)
		{
    		sprintf(str,"Sensor 1 ON \n\r");
    		serial_puts(str);
		}

    	if(sensor2detect)
		{
    		sprintf(str,"Sensor 2 ON \n\r");
    		serial_puts(str);
		}

    	if(sensor3detect)
		{
    		sprintf(str,"Sensor 3 ON \n\r");
    		serial_puts(str);
		}

    	if(sensor4detect)
		{
    		sprintf(str,"Sensor 4 ON \n\r");
    		serial_puts(str);
		}

    	if(sensor5detect)
		{
    		sprintf(str,"Sensor 5 ON \n\r");
    		serial_puts(str);
		}

		vTaskDelayUntil( &xNextWakeTime, LED_PWM_PERIOD_TICK);
    }
}

