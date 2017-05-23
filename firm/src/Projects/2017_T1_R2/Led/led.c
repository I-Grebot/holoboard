/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot 2017
 * -----------------------------------------------------------------------------
 * @file       led.c
 * @author     Pierrick
 * @date       Apr 23, 2017
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *   This module handles the RGB Led
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

/* Inclusion */
#include "main.h"

static const char * const welcomeMessage  = WELCOME_MESSAGE;


/* Local Variable Mutex */
static xSemaphoreHandle xLedColorMutex;
static xSemaphoreHandle xLedModeMutex;
static HB_LED_ColorTypeDef LedColor=HB_LED_BLUE;
static HB_LED_ModeTypeDef LedMode=HB_LED_BLINK_SLOW;

/* Local, Private functions */
static void OS_LedTask(void *pvParameters);


BaseType_t led_start(void)
{
	xLedColorMutex = xSemaphoreCreateMutex();
	xLedModeMutex = xSemaphoreCreateMutex();

    return xTaskCreate(OS_LedTask, "LED", OS_TASK_STACK_LED, NULL, OS_TASK_PRIORITY_LED, NULL );
}

static void OS_LedTask( void *pvParameters )
{
    TickType_t xNextWakeTime;

    uint32_t blinkCounter = 0;
    uint32_t blinkPeriod = 0;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    /* Send the welcome message. */
    //serial_puts(welcomeMessage);

    for( ;; )
    {
        switch(LedMode)
        {
            case HB_LED_BLINK_SLOW:
                blinkPeriod = LED_BLINK_SLOW;
                break;

            case HB_LED_BLINK_FAST:
                blinkPeriod = LED_BLINK_FAST;
                break;

            default:
            case HB_LED_STATIC:
                        break;
        }

        /* Handles blinking counter */
        if(blinkCounter++ > blinkPeriod)
        {
        	blinkCounter = 0;
        }

        if((blinkCounter > blinkPeriod / 2) || LedMode == HB_LED_STATIC)
        {
            /* Duration ON */
            hb_led_set_color(LedColor);
            vTaskDelayUntil( &xNextWakeTime, LED_PWM_DUTY_TICK);

            /* Duration OFF */
            hb_led_set_color(HB_LED_OFF);
            vTaskDelayUntil( &xNextWakeTime, LED_PWM_PERIOD_TICK-LED_PWM_DUTY_TICK);
        }
        else
        {
            vTaskDelayUntil( &xNextWakeTime, LED_PWM_PERIOD_TICK);
        }
    }
}

void led_set_color(HB_LED_ColorTypeDef color)
{
	xSemaphoreTake(xLedColorMutex, 10);
	LedColor = color;
	xSemaphoreGive(xLedColorMutex);
}

void led_set_mode(HB_LED_ModeTypeDef mode)
{
	xSemaphoreTake(xLedModeMutex, 10);
	LedMode = mode;
	xSemaphoreGive(xLedModeMutex);
}
