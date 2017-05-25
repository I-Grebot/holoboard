/*
 * ir_sensors.c
 *
 *  Created on: May 25, 2017
 *      Author: ldoppea
 */


/* Inclusion */
#include "main.h"
#include "ir_sensors.h"

uint8_t sensor0detect, sensor1detect, sensor2detect;

/* Local, Private functions */
static void OS_IrSensorsTask(void *pvParameters);

BaseType_t ir_sensor_start(void)
{
	sensor0detect = 0;
	sensor1detect = 0;
	sensor2detect = 0;

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

		vTaskDelayUntil( &xNextWakeTime, LED_PWM_PERIOD_TICK);
    }
}

