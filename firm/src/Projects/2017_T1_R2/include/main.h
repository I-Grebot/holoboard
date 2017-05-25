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
 **  The welcome message display at shell's startup
 **
 ********************************************************************************
 */
#define AT_COMMAND "AT+BAUD8"
#define WELCOME_MESSAGE "\f"\
   "-----------------------------------------------------------\n\r"\
   "  IgreBot "YEAR_STR" ~ Command Shell\n\r"\
   "-----------------------------------------------------------\n\r"\
   "  Robot : "ROBOT_NAME_STR"\n\r"\
   "  Build : "BUILD_VERSION_STR"\n\r"\
   "-----------------------------------------------------------\n\r"\
   " Type 'help' for the list of available commands\n\r"\
   "-----------------------------------------------------------\n\r\n\r"\
   "> "

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
#define OS_TASK_PRIORITY_LED          ( tskIDLE_PRIORITY + 2 )
#define OS_TASK_PRIORITY_MOTION_CS    ( tskIDLE_PRIORITY + 4 )
#define OS_TASK_PRIORITY_IR_SENSORS   ( tskIDLE_PRIORITY + 4 )
/*
 * OS Tasks Stacks sizes, in bytes
 */
#define OS_TASK_STACK_LED               configMINIMAL_STACK_SIZE
#define OS_TASK_STACK_MOTION_CS         500
#define OS_TASK_STACK_IR_SENSORS        500

 /* NVIC Priorities. Lower value means higher priority.
  * Beware to use priorities smaller than configLIBRARY_LOWEST_INTERRUPT_PRIORITY
  * and higher than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY when using
  * ISR Save FreeRTOS API Routines!
  */
#define OS_ISR_PRIORITY_SER             ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 )

 /*
  * Events periodicity
  */
#define MOTION_CONTROL_PERIOD_MS      20

/**
********************************************************************************
**
**  Prototypes
**
********************************************************************************
*/

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

/* Serial Interface */
BaseType_t serial_init(void);
BaseType_t serial_put(char ch);
BaseType_t serial_puts(const char* str);
BaseType_t serial_get(const char* str);
int serial_printf(const char * restrict format, ... );

/*
 * -----------------------------------------------------------------------------
 * Sub-Systems
 * -----------------------------------------------------------------------------
 */

/* IR Sensors System */
BaseType_t ir_sensor_start(void);

/* Motion Control System */
BaseType_t motion_cs_start(void);
void motor1_set_speed(int speed);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
