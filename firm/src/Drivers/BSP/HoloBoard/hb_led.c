/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot 2017
 * -----------------------------------------------------------------------------
 * @file       hb_led.c
 * @author     Pierrick
 * @date       Apr 23, 2017
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *   This module handles the RGB LED
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

#include "holoboard.h"

/**
  * @brief  Configure LEDs GPIOs (push-pull, no pull-up)
  * @param  None
  * @retval None
  */
void hb_led_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    LEDR_GPIO_CLK_ENABLE();
    LEDG_GPIO_CLK_ENABLE();
    LEDB_GPIO_CLK_ENABLE();

    /* Common configuration for all LEDs */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;

    GPIO_InitStructure.GPIO_Pin = LEDR_PIN;
    GPIO_Init(LEDR_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LEDG_PIN;
    GPIO_Init(LEDG_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LEDB_PIN;
    GPIO_Init(LEDB_GPIO_PORT, &GPIO_InitStructure);

    /* Default state: LEDs OFF */
    LEDR_WRITE(LEDx_OFF);
    LEDG_WRITE(LEDx_OFF);
    LEDB_WRITE(LEDx_OFF);
}

/**
  * @brief  Setup a new RGB Led color
  * @param  color: Color to setup
  * @retval None
  */
void hb_led_set_color(HB_LED_ColorTypeDef color)
{
    switch(color)
    {
    case HB_LED_RED:
        LEDR_WRITE(LEDx_ON);
        LEDG_WRITE(LEDx_OFF);
        LEDB_WRITE(LEDx_OFF);
        break;
    case HB_LED_GREEN:
        LEDR_WRITE(LEDx_OFF);
        LEDG_WRITE(LEDx_ON);
        LEDB_WRITE(LEDx_OFF);
        break;
    case HB_LED_BLUE:
        LEDR_WRITE(LEDx_OFF);
        LEDG_WRITE(LEDx_OFF);
        LEDB_WRITE(LEDx_ON);
        break;
    case HB_LED_CYAN:
        LEDR_WRITE(LEDx_OFF);
        LEDG_WRITE(LEDx_ON);
        LEDB_WRITE(LEDx_ON);
        break;
    case HB_LED_YELLOW:
        LEDR_WRITE(LEDx_ON);
        LEDG_WRITE(LEDx_ON);
        LEDB_WRITE(LEDx_OFF);
        break;
    case HB_LED_MAGENTA:
        LEDR_WRITE(LEDx_ON);
        LEDG_WRITE(LEDx_OFF);
        LEDB_WRITE(LEDx_ON);
        break;
    case HB_LED_WHITE:
        LEDR_WRITE(LEDx_ON);
        LEDG_WRITE(LEDx_ON);
        LEDB_WRITE(LEDx_ON);
        break;

    default:
    case HB_LED_OFF:
        LEDR_WRITE(LEDx_OFF);
        LEDG_WRITE(LEDx_OFF);
        LEDB_WRITE(LEDx_OFF);
        break;

    }

}
