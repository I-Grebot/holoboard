/* -----------------------------------------------------------------------------
 * BlueBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file       system.c
 * @author     Paul
 * @date       Jan 30, 2017
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *   Module implementing some OS handlers / overloads
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/firm_blueboard.git
 * -----------------------------------------------------------------------------
 */

#include "main.h"
/* End-Of-Line characters that should be used */
#define SHELL_EOL                       "\n\r"
#define SHELL_SYS_PFX           "[SYS] "    // For returns of Sys command

/* This example demonstrates how a human readable table of run time stats
information is generated from raw data provided by uxTaskGetSystemState().
The human readable table is written to pcWriteBuffer.  (see the vTaskList()
API function which actually does just this). */
void sys_get_run_time_stats(char *pcWriteBuffer)
{
    const char * const pcHeader =
                    SHELL_SYS_PFX"Task                  Abs. Time    % Time"SHELL_EOL;
    const char * const pcLineSeparator =
                    SHELL_SYS_PFX"-----------------------------------------"SHELL_EOL;

    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    uint32_t ulTotalRunTime;
    uint32_t ulStatsAsPercentage;

   /* Make sure the write buffer does not contain a string. */
   *pcWriteBuffer = 0x00;

   /* Take a snapshot of the number of tasks in case it changes while this
   function is executing. */
   uxArraySize = uxTaskGetNumberOfTasks();

   /* Allocate a TaskStatus_t structure for each task.  An array could be
   allocated statically at compile time. */
   pxTaskStatusArray = pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );

   if( pxTaskStatusArray != NULL )
   {
      /* Generate raw status information about each task. */
      uxArraySize = uxTaskGetSystemState( pxTaskStatusArray,
                                 uxArraySize,
                                 &ulTotalRunTime );

      /* For percentage calculations. */
      ulTotalRunTime /= 100UL;

      /* Avoid divide by zero errors. */
      if( ulTotalRunTime > 0 )
      {
          /* Header */
          sprintf( pcWriteBuffer, pcHeader );
          pcWriteBuffer += strlen( ( char * ) pcWriteBuffer );
          sprintf( pcWriteBuffer, pcLineSeparator );
          pcWriteBuffer += strlen( ( char * ) pcWriteBuffer );

          /* For each populated position in the pxTaskStatusArray array,
          format the raw data as human readable ASCII data. */
         for( x = 0; x < uxArraySize; x++ )
         {
            /* What percentage of the total run time has the task used?
            This will always be rounded down to the nearest integer.
            ulTotalRunTimeDiv100 has already been divided by 100. */
            ulStatsAsPercentage =
                  pxTaskStatusArray[ x ].ulRunTimeCounter / ulTotalRunTime;

            if( ulStatsAsPercentage > 0UL )
            {
               sprintf( pcWriteBuffer, SHELL_SYS_PFX"%-20s %10lu   %3lu%%"SHELL_EOL,
                                 pxTaskStatusArray[ x ].pcTaskName,
                                 pxTaskStatusArray[ x ].ulRunTimeCounter,
                                 ulStatsAsPercentage );
            }
            else
            {
               /* If the percentage is zero here then the task has
               consumed less than 1% of the total run time. */
               sprintf( pcWriteBuffer, SHELL_SYS_PFX"%-20s %10lu    <1%%"SHELL_EOL,
                                 pxTaskStatusArray[ x ].pcTaskName,
                                 pxTaskStatusArray[ x ].ulRunTimeCounter );
            }

            pcWriteBuffer += strlen( ( char * ) pcWriteBuffer );
         }

         /* Footer */
         sprintf( pcWriteBuffer, pcLineSeparator );
         pcWriteBuffer += strlen( ( char * ) pcWriteBuffer );
         sprintf( pcWriteBuffer,SHELL_SYS_PFX"%-20s %10lu   %3lu%%"SHELL_EOL, "TOTAL", 100*ulTotalRunTime, 100UL);
         pcWriteBuffer += strlen( ( char * ) pcWriteBuffer );
      }

      /* The array is no longer needed, free the memory it consumes. */
      vPortFree( pxTaskStatusArray );
   }
}
