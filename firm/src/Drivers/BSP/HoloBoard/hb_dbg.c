/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot 2017
 * -----------------------------------------------------------------------------
 * @file       hb_dbg.c
 * @author     Pierrick
 * @date       Apr 27, 2017
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *   This module implements the serial interface functions
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

#include "holoboard.h"



/**
  * @brief  Initialize the Debug UART
  * @param  None
  * @retval None
  */
void hb_dbg_init(USART_InitTypeDef * USART_InitStruct)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_ClockInitTypeDef USART_ClockInitStruct;

    /* Enable GPIOs Clock */
    DBG_TX_GPIO_CLK_ENABLE();
    DBG_RX_GPIO_CLK_ENABLE();


    /* Enable Debug USART clock */
    DBG_CLK_ENABLE();

    /* Configure USART TX as alternate function */
    GPIO_InitStructure.GPIO_Pin = DBG_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed; /* 2 MHz */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(DBG_TX_GPIO_PORT, &GPIO_InitStructure);

    /* Configure USART RX as alternate function */
    GPIO_InitStructure.GPIO_Pin = DBG_RX_PIN;
    GPIO_Init(DBG_RX_GPIO_PORT, &GPIO_InitStructure);

    /* Connect Alternate functions */
    GPIO_PinAFConfig(DBG_TX_GPIO_PORT,  DBG_TX_PIN_SOURCE,  DBG_TX_AF);
    GPIO_PinAFConfig(DBG_RX_GPIO_PORT,  DBG_RX_PIN_SOURCE,  DBG_RX_AF);

    /* USART configuration */
    USART_Init(DBG_COM, USART_InitStruct);

    /* The TX and RX pins functions are swapped */
    USART_SWAPPinCmd(DBG_COM,ENABLE);

    /* USART Clock default configuration */
    USART_ClockStructInit(&USART_ClockInitStruct);
    USART_ClockInit(DBG_COM, &USART_ClockInitStruct);
}


void hb_dbg_enable(uint32_t nvic_priority)
{
    /* Enable USART Interrupts */
    USART_ITConfig(DBG_COM, USART_IT_RXNE, ENABLE);
    NVIC_SetPriority(DBG_IRQn, nvic_priority);
    NVIC_EnableIRQ(DBG_IRQn);

    /* Enable USART */
    USART_Cmd(DBG_COM, ENABLE);
}

void hb_dbg_disable(void)
{
    /* Disable IRQs */
    USART_ITConfig(DBG_COM, USART_IT_RXNE, DISABLE);
    NVIC_DisableIRQ(DBG_IRQn);

    /* Stop UART */
    USART_Cmd(DBG_COM, DISABLE);
}

