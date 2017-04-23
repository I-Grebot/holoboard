/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot 2017
 * -----------------------------------------------------------------------------
 * @file       hb_system.c
 * @author     Pierrick
 * @date       Apr 23, 2017
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *   Contains top-level system functions implementation
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

#include "holoboard.h"

static uint16_t sysTimerCnt;

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 192000000
  *            HCLK(Hz)                       = 192000000
  *            USBCLK(Hz)                     = 48000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 4
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 16
  *            PLL_N                          = 384
  *            PLL_P                          = 2
  *            PLL_Q                          = 8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
void hb_system_clock_config(void)
{
  RCC_HSEConfig(RCC_HSE_ON);

  if ( RCC_WaitForHSEStartUp() != SUCCESS )
  {
    while(1);
  }

  RCC_PLLConfig(RCC_PLLSource_HSE, 16, 384, 2, 8);
  RCC_PLLCmd(ENABLE);

  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

  /* Activate the OverDrive to reach the 196 MHz Frequency */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  PWR_OverDriveCmd( ENABLE );

  while( PWR_GetFlagStatus( PWR_FLAG_ODRDY ) == RESET );

  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

  PWR_OverDriveSWCmd(ENABLE);

  while( PWR_GetFlagStatus( PWR_FLAG_ODSWRDY ) == RESET );

  FLASH_SetLatency(FLASH_Latency_7);
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div4);
  RCC_PCLK2Config(RCC_HCLK_Div4);

  SystemCoreClockUpdate();
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
void hb_sys_cpu_cache_enable(void)
{
  /* Enable branch prediction */
  SCB->CCR |= (1 <<18);
  __DSB();

  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

void hb_sys_timer_run_time_config(void)
{
    TIM_TimeBaseInitTypeDef TIM_BaseStruct;

    /* Enable timer clock */
    SYS_RUNSTATS_TIM_CLK_ENABLE();

    /* Setup timer */
    TIM_BaseStruct.TIM_ClockDivision        = SYS_RUNSTATS_CKDIV;
    TIM_BaseStruct.TIM_Prescaler            = SYS_RUNSTATS_PRESCALER;
    TIM_BaseStruct.TIM_Period               = 0xFFFF;
    TIM_BaseStruct.TIM_RepetitionCounter    = 0;
    TIM_TimeBaseInit(SYS_RUNSTATS_TIM, &TIM_BaseStruct);

    /* Configure interrupt */
    TIM_ITConfig(SYS_RUNSTATS_TIM, TIM_IT_Update, ENABLE);
    NVIC_SetPriority(SYS_RUNSTATS_IRQn, HB_PRIORITY_SYS_RUNSTATS);
    NVIC_EnableIRQ(SYS_RUNSTATS_IRQn);

    /* Clear and start timer */
    sysTimerCnt = 0;
    TIM_SetCounter(SYS_RUNSTATS_TIM, 0);
    TIM_Cmd(SYS_RUNSTATS_TIM, ENABLE);

}

uint32_t hb_sys_timer_get_run_time_ticks(void)
{
    uint32_t ret;
    ret = (((uint32_t) sysTimerCnt) << 16UL) + TIM_GetCounter(SYS_RUNSTATS_TIM);
    return ret;
}

/*
 * Run-Time Timer Interrupt Sub-routine
 * Required to implement a 32bits timer.
 * sysTimerCnt is used as the 16 MSB of the timer.
 */
void SYS_RUNSTATS_ISR (void)
{
    NVIC_ClearPendingIRQ(SYS_RUNSTATS_IRQn);
    TIM_ClearITPendingBit(SYS_RUNSTATS_TIM, TIM_IT_Update);
    sysTimerCnt++;
}

