/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot 2017
 * -----------------------------------------------------------------------------
 * @file       hb_config.h
 * @author     Pierrick
 * @date       Apr 23, 2017
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *    External configuration-file for the holoboard BSP
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

#ifndef __HB_CONFIG_H
#define __HB_CONFIG_H


/**
 ********************************************************************************
 **
 ** System Run-Stats
 **
 ********************************************************************************
 */

 /* Clock-divider and prescaler config to adjust the run-time timer frequency
  * APB1 Timers Running at   96000 kHz
  * System ticks running at      1 kHz
  */
#define SYS_RUNSTATS_CKDIV          TIM_CKD_DIV4
#define SYS_RUNSTATS_PRESCALER      (1199) // 20 kHz

/* NVIC priority of the system runstats timer */
#define HB_PRIORITY_SYS_RUNSTATS    (15) // configLIBRARY_LOWEST_INTERRUPT_PRIORITY



#endif /* __HB_CONFIG_H */
