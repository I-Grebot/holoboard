/* -----------------------------------------------------------------------------
 * BlueBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file       serial.c
 * @author     Pierrick
 * @date       Apr 24, 2017
 * -----------------------------------------------------------------------------
 * @brief
 *   This module implements the serial interface functions
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

#include "main.h"
static const char * const welcomeMessage  = WELCOME_MESSAGE;
/* Local private hardware configuration handlers */
static USART_InitTypeDef Serial_Config;
static QueueHandle_t Serial_RxQueue;
static QueueHandle_t Serial_TxQueue;

BaseType_t serial_init(void)
{
    /*
     * Configure the Debug UART init structure:
     *   8 bits length + 1 stop bit, no parity
     */
    Serial_Config.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    Serial_Config.USART_BaudRate            = SERIAL_BAUDRATE;
    Serial_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    Serial_Config.USART_Parity              = USART_Parity_No;//USART_Parity_Even;
    Serial_Config.USART_StopBits            = USART_StopBits_1;
    Serial_Config.USART_WordLength          = USART_WordLength_8b;//USART_WordLength_9b;

    /* Initialize and configure Hardware */
    hb_dbg_init(&Serial_Config);

    /* Create Serial Queues */
    Serial_RxQueue = xQueueCreate(SERIAL_RX_QUEUE_LEN, sizeof(char));
    Serial_TxQueue = xQueueCreate(SERIAL_TX_QUEUE_LEN, sizeof(char));

    /* Enable hardware */
    hb_dbg_enable(OS_ISR_PRIORITY_SER);

    return pdPASS;
}

/**
  * @brief  Add a byte to send through Serial Interface.
  *         Might be started right away or stored in the TX Queue.
  * @param  ch: character to send
  * @retval pdPASS if the character was sent or added to the TX Queue
  *         pdFAIL if the TX Queue is full
  */

BaseType_t serial_put(char ch)
{

    /* If the queue is empty and no message is being transmitted,
     * the line is idle so we can initiate directly the transmission.
     */
    if((uxQueueMessagesWaiting(Serial_TxQueue) == 0) &&
       (USART_GetFlagStatus(SERIAL_COM, USART_FLAG_TXE) == SET)) {

        USART_ITConfig(SERIAL_COM, USART_IT_TC, ENABLE);
        USART_SendData(SERIAL_COM, (uint16_t) ch);

        return pdPASS;

    /* Otherwise, at least 1 transmission is pending,
     * so we add it the the transmit queue.
     */
    } else {

        if(xQueueSend(Serial_TxQueue, &ch, SERIAL_TX_TIMEOUT) == pdPASS)
        {
            USART_ITConfig(SERIAL_COM, USART_IT_TC, ENABLE);
            return pdPASS;
        } else {
            return pdFAIL;
        }
    }

}

/**
  * @brief  Send a string through Serial Interface
  * @param  str: string to send
  * @retval Pass/Fail status
  */
BaseType_t serial_puts(const char* str)
{
    while (*str)
    {
        if((serial_put(*str)) == pdPASS) {
            str++;
        } else {
            return pdFAIL;
        }
    }

    return pdPASS;
}

/**
  * @brief  Receive a byte from debug UART
  * @param  Const pointer to read value
  * @retval Pass/Fail status
  */
BaseType_t serial_get(const char* str)
{
    return(xQueueReceive(Serial_RxQueue, str, SERIAL_RX_TIMEOUT));
}


/*
 * Serial Interface ISR
 */
void SERIAL_ISR (void)
{
    char rxChar;
    char txChar;

    // We have not woken a task at the start of the ISR.
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Hand RX Interrupt
    if(USART_GetITStatus(SERIAL_COM, USART_IT_RXNE) != RESET)
    {
        // RXNE Flag cleared with the USAR_ReceiveData() call
        rxChar = USART_ReceiveData(SERIAL_COM);

        // Put the received char into the Rx Queue
        xQueueSendFromISR(Serial_RxQueue, &rxChar, &xHigherPriorityTaskWoken);

    }

    // Handle TX Interrupt
    else if(USART_GetITStatus(SERIAL_COM, USART_IT_TC) != RESET)
    {
        // Clear IT
        USART_ClearITPendingBit(SERIAL_COM, USART_IT_TC);

        /* Check to see if there is more data to send.
         * The TC interrupt will be re-generated at the end of the transmission.
         * Therefore, this will auto-reload until the queue is empty.
         */
        if((xQueueReceiveFromISR(Serial_TxQueue, &txChar, &xHigherPriorityTaskWoken)) == pdPASS)
        {
            USART_SendData(SERIAL_COM, (uint16_t) txChar);
        }
        else
        {
            // Nothing to send! Disable back the ISR
            USART_ITConfig(SERIAL_COM, USART_IT_TC, DISABLE);
        }

    }
}

