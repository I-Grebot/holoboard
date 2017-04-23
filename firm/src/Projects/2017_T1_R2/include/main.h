/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot 2017
 * -----------------------------------------------------------------------------
 * @file    main.h
 * @author     Pierrick
 * @date       Apr 23, 2017
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *    This file is the main header of IgreBot's 2017 Team 1 Robot 2 firmware
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
********************************************************************************
**
**  Firmware identification
**
********************************************************************************
*/

#define YEAR_STR          "2017"
#define ROBOT_NAME_STR    "R2"
#define BUILD_VERSION_STR __DATE__" @ "__TIME__

/**
********************************************************************************
**
**  Inclusions
**
********************************************************************************
*/

/* Standard libraries */
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <string.h>

/* Board-Support Package Drivers */
#include "holoboard.h"

/* Middleware: FreeRTOS + FreeRTOS-Plus */
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "semphr.h"


/* Project files */
#include "hardware_const.h"

/**
********************************************************************************
**
**  Applicative & High-Level Configuration
**
********************************************************************************
*/

/*
 * OS Tasks Priorities.
 * Higher value means higher priority
 */
#define OS_TASK_PRIORITY_LED          ( tskIDLE_PRIORITY + 2)

/*
 * OS Tasks Stacks sizes, in bytes
 */
#define OS_TASK_STACK_LED               configMINIMAL_STACK_SIZE


/*
 * -----------------------------------------------------------------------------
 * Core & Middlewares
 * -----------------------------------------------------------------------------
 */

/* FreeRTOS prototypes for the standard FreeRTOS callback/hook functions */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

/* OS handlers */
void sys_get_run_time_stats(char *pcWriteBuffer);

/*
 * -----------------------------------------------------------------------------
 * Hardware Management
 * -----------------------------------------------------------------------------
 */

/* RGB LED */
BaseType_t led_start(void);
void led_set_mode(HB_LED_ModeTypeDef mode);
void led_set_color(HB_LED_ColorTypeDef color);



#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
