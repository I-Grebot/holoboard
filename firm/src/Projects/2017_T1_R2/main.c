/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot 2017
 * -----------------------------------------------------------------------------
 * @file       main.c
 * @author     Pierrick
 * @date       Apr 23, 2017
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *   Main module of IgreBot's 2017 Robot #2 Firmware
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */
/* General Header */
#include "main.h"

/**
********************************************************************************
**
**  Main Program
**
********************************************************************************
*/

int main( void )
{

  /* HoloBoard Initializations */
  hb_init();

  led_start();


  /* Start FreeRTOS Scheduler */
  vTaskStartScheduler();

  /* Infinite loop */
  for( ;; );

  /* Should never reach this line! */
  return 0;
}
