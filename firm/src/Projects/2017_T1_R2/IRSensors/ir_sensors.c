/*
 * ir_sensors.c
 *
 *  Created on: May 25, 2017
 *      Author: ldoppea
 */


/* Inclusion */
#include "main.h"
#include "ir_sensors.h"

uint8_t sensor0, sensor1, sensor2;

/* Local, Private functions */
static void OS_IrSensorsTask(void *pvParameters);

BaseType_t ir_sensor_start(void)
{
	sensor0 = 0;
	sensor1 = 0;
	sensor2 = 0;

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
    	sensor0 = IRSENSOR0_VALUE == 0;
    	sensor1 = IRSENSOR1_VALUE == 0;
    	sensor2 = IRSENSOR2_VALUE == 0;

    	if(sensor0 == 1)
		{
    		sprintf(str,"Sensor 0 ON \n\r");
    		serial_puts(str);
		}

    	if(sensor1 == 1)
		{
    		sprintf(str,"Sensor 1 ON \n\r");
    		serial_puts(str);
		}

    	if(sensor2 == 1)
		{
    		sprintf(str,"Sensor 2 ON \n\r");
    		serial_puts(str);
		}

		vTaskDelayUntil( &xNextWakeTime, LED_PWM_PERIOD_TICK);
    }
}

