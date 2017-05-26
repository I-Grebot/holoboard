/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file       digital_servo.c
 * @author     Pierrick
 * @date       Apr 27, 2016
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *   This module handles the digital servos
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

/* Inclusion */
#include "main.h"

/* UART Configuration handlers */
//USART_InitTypeDef dsv_itf1_uart;
//USART_InitTypeDef dsv_itf2_uart;

/* Dynamixel Interfaces configuration */
//dxl_interface_t dxl1;
//dxl_interface_t dxl2;

/* Digital servo channels configuration structures */
const uint8_t dsv_nb_channels = 1;
dsv_channel_t dsv_chan1;
dsv_channel_t dsv_chan2;


/* Definition */
#define MAX_DSV_IN_QUEUE	5

#define XL320_GRABBER_OPEN 		350
#define XL320_GRABBER_CLOSED 	840


/* Local structures */
// TODO: UNUSED
typedef struct {
	uint8_t id;
	uint16_t current_Position;
	uint16_t min_Position;
	uint16_t max_Position;
}DSV_ControlTypeDef;

/* Local Variable Mutex */
static xQueueHandle xDSVMsgQueue;
//static DSV_ControlTypeDef servo1, servo2; // TODO : define a comprehensive name


// Servos
static dxl_servo_t servo1;

/* Local, Private functions */
static void OS_DSVTask(void *pvParameters);
void DSV_Create(DSV_ControlTypeDef* DSV, uint8_t id, uint16_t min_Position, uint16_t max_Position);

/**
********************************************************************************
**
**  Initialization & Configuration
**
********************************************************************************
*/

/**
  * @brief  Initialize the DSV hardware module
  */
void dsv_init(void)
{
    /* Initialize the dynamixel interfaces */
    dxl_init_itf(&dsv_chan1.dxl, HB_DSV_CHANNEL1);

    /*
     * Configure the Digital Servo UART for channel 1 structure:
     *   8 bits length + 1 stop bit, no parity
     */
    dsv_chan1.uart.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    dsv_chan1.uart.USART_BaudRate            = 57600;
    dsv_chan1.uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    dsv_chan1.uart.USART_Parity              = USART_Parity_No;
    dsv_chan1.uart.USART_StopBits            = USART_StopBits_1;
    dsv_chan1.uart.USART_WordLength          = USART_WordLength_8b;

    /* Initialize DSV Hardware */
    hb_dsv_init(dsv_chan1.dxl.itf_idx, &dsv_chan1.uart);

    /* Configure XL-320 interface */
    dsv_chan1.dxl.protocol = DXL_V2;
    dsv_chan1.dxl.hw_send_byte = dsv_put;
    dsv_chan1.dxl.hw_receive_byte = dsv_get;
    dsv_chan1.dxl.hw_flush = dsv_flush;

    /* Create Serial Queues for channel 1 */
    dsv_chan1.rx_queue = xQueueCreate(DSV_RX_QUEUE_SIZE, sizeof(char));
    dsv_chan1.tx_queue = xQueueCreate(DSV_TX_QUEUE_SIZE, sizeof(char));

    /* Enable hardware */
    hb_dsv_enable(dsv_chan1.dxl.itf_idx, OS_ISR_PRIORITY_DSV);

    // TEMP
     /* Initialize XL-320 Library */
     xl_320_init(XL_320_TX_ONLY);
     xl_320_set_hw_send(dsv_put);
     //xl_320_set_hw_switch(dsv_switch);

    // Initialize test servos
    dxl_init_servo(&servo1, &dsv_chan1.dxl, "XL-320");

}

/**
  * @brief  Update the hardware config and restart the
  *         DSV module
  */
void dsv_update_config(void)
{
    /* Disable hardware */
    bb_dsv_disable(dsv_chan1.dxl.itf_idx);

    /* Cleanup queues */
    vQueueDelete(dsv_chan1.rx_queue);
    vQueueDelete(dsv_chan1.tx_queue);

    /* Restart the entire initialization */
    hb_dsv_init(dsv_chan1.dxl.itf_idx, &dsv_chan1.uart);
}

/**
********************************************************************************
**
**  Low-level routines
**
********************************************************************************
*/

/**
  * @brief  Add a byte to send through dsv channel
  *         Might be started right away or stored in the TX Queue.
  * @param  chan_idx: DSV channel
  * @param  tx_data: data to send
  * @retval pdPASS if the character was sent or added to the TX Queue
  *         pdFAIL if the TX Queue is full
  */
uint8_t dsv_put(uint8_t chan_idx, uint8_t tx_data)
{
    QueueHandle_t* tx_queue;
    USART_TypeDef* usart_if;

    if(chan_idx == HB_DSV_CHANNEL1) {
        tx_queue = &dsv_chan1.tx_queue;
        usart_if = DSV_COM;
    }


    /* If the queue is empty and no message is being transmitted,
     * the line is idle so we can initiate directly the transmission.
     */
    if((uxQueueMessagesWaiting(*tx_queue) == 0) &&
       (USART_GetFlagStatus(usart_if, USART_FLAG_TXE) == SET)) {

        hb_dsv_switch(chan_idx, DXL_MODE_TX);
        USART_ITConfig(usart_if, USART_IT_TC, ENABLE);
        USART_SendData(usart_if, (uint16_t) tx_data);

        return pdPASS;

    /* Otherwise, at least 1 transmission is pending,
     * so we add it the the transmit queue.
     */
    } else {

        if(xQueueSend(*tx_queue, &tx_data, DSV_TX_TIMEOUT) == pdPASS)
        {
            USART_ITConfig(usart_if, USART_IT_TC, ENABLE);
            return pdPASS;
        } else {
            return pdFAIL;
        }
    }

}

/**
  * @brief  Receive a byte from dsv channel
  *         The timeout is given from the return delay of the
  *         dynamixel interface (+ 1 ms of safety)
  * @param  chan_idx: DSV channel
  * @param  Const pointer to read value
  * @retval Pass/Fail status
  */
uint8_t dsv_get(uint8_t chan_idx, const uint8_t* data)
{
    if(chan_idx == HB_DSV_CHANNEL1) {
        if(xQueueReceive(dsv_chan1.rx_queue, data, DSV_RX_TIMEOUT) == pdPASS) {
            return DXL_PASS;
        }
    }

    return DXL_FAIL;
}

/**
  * @brief  Flush a receiver interface (empty queue)
  * @param  chan_idx: DSV channel
  * @retval Pass/Fail status
  */
uint8_t dsv_flush(uint8_t chan_idx)
{

    if(chan_idx == HB_DSV_CHANNEL1) {
        xQueueGenericReset(dsv_chan1.rx_queue, pdFALSE);

    }

    return pdPASS;
}


/*
 * Digital Servo (channel 1) ISR
 */
void DSV_ISR (void)
{
    char rxChar;
    char txChar;

    // We have not woken a task at the start of the ISR.
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Hand RX Interrupt
    if(USART_GetITStatus(DSV_COM, USART_IT_RXNE) != RESET)
    {
        // RXNE Flag cleared with the USAR_ReceiveData() call
        rxChar = USART_ReceiveData(DSV_COM);

        // Put the received char into the Rx Queue
        xQueueSendFromISR(dsv_chan1.rx_queue, &rxChar, &xHigherPriorityTaskWoken);

    }

    // Handle TX Interrupt
    else if(USART_GetITStatus(DSV_COM, USART_IT_TC) != RESET)
    {
        // Clear IT
        USART_ClearITPendingBit(DSV_COM, USART_IT_TC);

        /* Check to see if there is more data to send.
         * The TC interrupt will be re-generated at the end of the transmission.
         * Therefore, this will auto-reload until the queue is empty.
         */
        if((xQueueReceiveFromISR(dsv_chan1.tx_queue, &txChar, &xHigherPriorityTaskWoken)) == pdPASS)
        {
            USART_SendData(DSV_COM, (uint16_t) txChar);
        }
        else
        {
            // Nothing to send! Disable back the ISR and switch back to RX
            USART_ITConfig(DSV_COM, USART_IT_TC, DISABLE);
            hb_dsv_switch(dsv_chan1.dxl.itf_idx, DXL_MODE_RX);
        }

    }
}

/*
 * Digital Servo (channel 2) ISR
 */

#ifdef HB_USE_RS485_DSV_CHAN2
void RS485_ISR (void)
{
    char rxChar;
    char txChar;

    // We have not woken a task at the start of the ISR.
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Hand RX Interrupt
    if(USART_GetITStatus(RS485_COM, USART_IT_RXNE) != RESET)
    {

        // RXNE Flag cleared with the USAR_ReceiveData() call
        rxChar = USART_ReceiveData(RS485_COM);

        // Put the received char into the Rx Queue
        xQueueSendFromISR(dsv_chan2.rx_queue, &rxChar, &xHigherPriorityTaskWoken);

    }

    // Handle TX Interrupt
    else if(USART_GetITStatus(RS485_COM, USART_IT_TC) != RESET)
    {
        // Clear IT
        USART_ClearITPendingBit(RS485_COM, USART_IT_TC);

        /* Check to see if there is more data to send.
         * The TC interrupt will be re-generated at the end of the transmission.
         * Therefore, this will auto-reload until the queue is empty.
         */
        if((xQueueReceiveFromISR(dsv_chan2.tx_queue, &txChar, &xHigherPriorityTaskWoken)) == pdPASS)
        {
            USART_SendData(RS485_COM, (uint16_t) txChar);
        }
        else
        {
            // Nothing to send! Disable back the ISR and switch back to RX
            USART_ITConfig(RS485_COM, USART_IT_TC, DISABLE);
            bb_dsv_switch(dsv_chan2.dxl.itf_idx, DXL_MODE_RX);
        }

    }
}
#endif


/**
********************************************************************************
**
**  Service & Test routines
**
********************************************************************************
*/

/**** THIS IS ALL TEMPORARY ****/
/*
void dsv_test_pos(uint8_t id, uint16_t pos)
{

//    servo1.id = DXL_ID_BROADCAST;
    servo1.id = id;

    dxl_set_position(&servo1, pos);
    dxl_set_torque(&servo1, 1);

}*/

/*void dsv_test_led(uint8_t id, uint8_t led)
{

    servo1.id = DXL_ID_BROADCAST;

    dxl_set_led(&servo1, led);
}*/

void dsv_ping(uint8_t id)
{
    servo1.id = id;
    dxl_ping(&servo1);
}

void dsv_reset(uint8_t id)
{

    servo1.id = DXL_ID_BROADCAST;

    dxl_reset(&servo1);
}

uint8_t dsv_read(uint8_t itf_idx, uint8_t id, uint8_t address) {

    uint8_t data;

    servo1.id = id;
    dxl_read(&servo1, address, &data, 1); // size temp

    return data;

}




/**
********************************************************************************
**
**  High-level management / Main task
**
********************************************************************************
*/

BaseType_t dsv_start(void)
{

	xDSVMsgQueue = xQueueCreate( MAX_DSV_IN_QUEUE, sizeof(DSV_ControlTypeDef));
    if(xDSVMsgQueue==NULL)
    {
    	serial_puts("Error: Insufficient heap RAM available for DSVMsgQueue");
    	return pdFAIL;
    }

    //DSV_Create(&servo1, 23, 0, 1023);
    //DSV_Create(&servo2, 42, 0, 1023);

    return xTaskCreate(OS_DSVTask, "DIGITAL SERVO", 350, NULL, OS_TASK_PRIORITY_DSV, NULL );
}

static void OS_DSVTask( void *pvParameters )
{
	DSV_ControlTypeDef DSV_To_Manage;
    TickType_t xNextWakeTime;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

//    for( ;; )
//    {
//    	/* Block on the queue to wait for data to arrive */
//    	xQueueReceive(xDSVMsgQueue, &DSV_To_Manage, portMAX_DELAY);
//    	if (DSV_To_Manage.current_Position > DSV_To_Manage.max_Position)
//    		DSV_To_Manage.current_Position = DSV_To_Manage.max_Position;
//    	if (DSV_To_Manage.current_Position < DSV_To_Manage.min_Position)
//    		DSV_To_Manage.current_Position = DSV_To_Manage.min_Position;
//
//    	xl_320_set_position(DSV_To_Manage.id, DSV_To_Manage.current_Position);
//    	vTaskDelay(5/portTICK_RATE_MS);
//    	xl_320_set_led(DSV_To_Manage.id, XL_320_LED_BLUE);
//    }
//    vTaskDelayUntil( &xNextWakeTime, 500);
    servo1.id=80;
	vTaskDelayUntil( &xNextWakeTime, 500);
	dxl_set_led(&servo1,DXL_LED_BLUE);
	vTaskDelayUntil( &xNextWakeTime, 500);
//	dxl_set_position(&servo1,XL320_GRABBER_OPEN);
//	vTaskDelayUntil( &xNextWakeTime, 2000);
//	dxl_set_position(&servo1,XL320_GRABBER_CLOSED);
//	vTaskDelayUntil( &xNextWakeTime, 2000);
//	dxl_set_position(&servo1,XL320_GRABBER_OPEN);
    for (;;)
    {

    }

}

void dsv_open_grabber() {
	dxl_set_position(&servo1,XL320_GRABBER_OPEN);
	dxl_set_led(&servo1,DXL_LED_BLUE);
}

void dsv_close_grabber() {
	dxl_set_position(&servo1,XL320_GRABBER_CLOSED);
	dxl_set_led(&servo1,DXL_LED_YELLOW);
}
/*
void DSV_Create(DSV_ControlTypeDef* DSV, uint8_t id, uint16_t min_Position, uint16_t max_Position)
{
	DSV->id = id;
	DSV->max_Position = max_Position;
	DSV->min_Position = min_Position;
	xl_320_set_control_mode(DSV->id, XL_320_JOIN_MODE);
}*/

/* EXAMPLE
void DSV_SetServo1(uint16_t position)
{
	servo1.current_Position = position;
	xQueueSend( xDSVMsgQueue, &servo1, (TickType_t)0 );
}*/
