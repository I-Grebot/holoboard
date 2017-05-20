/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot 2017
 * -----------------------------------------------------------------------------
 * @file       hb_lcmxo2.c
 * @author     Pierrick
 * @date       Apr 24, 2017
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *   This module handles the communication with the LCMXO2 (motor+encoder)
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */


#include "holoboard.h"

void hb_lcmxo2_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStruct;

    /* Enable GPIOs Clock */
    SPI_CLK_GPIO_CLK_ENABLE();
    SPI_MISO_GPIO_CLK_ENABLE();
    SPI_MOSI_GPIO_CLK_ENABLE();
    LCMXO2_SS_GPIO_CLK_ENABLE();
    LCMXO2_RESET_GPIO_CLK_ENABLE();

    /* Enable Control Interface SPI1 clock */
    SPI_CLK_ENABLE();

    /* Configure SPI_CLK as alternate function */
    GPIO_InitStructure.GPIO_Pin = SPI_CLK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed; /* 100 MHz */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(SPI_CLK_GPIO_PORT, &GPIO_InitStructure);

    /* Configure SPI_MOSI as alternate function */
    GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;
    GPIO_Init(SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

    /* Configure SPI_MISO as alternate function */
    GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;
    GPIO_Init(SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

    /* Connect Alternate functions */
    GPIO_PinAFConfig(SPI_CLK_GPIO_PORT,   SPI_CLK_PIN_SOURCE,   SPI_CLK_AF);
    GPIO_PinAFConfig(SPI_MOSI_GPIO_PORT,  SPI_MOSI_PIN_SOURCE,  SPI_MOSI_AF);
    GPIO_PinAFConfig(SPI_MISO_GPIO_PORT,  SPI_MISO_PIN_SOURCE,  SPI_MISO_AF);

    /* Configure LCMXO2_SS as Digital Output */
    GPIO_InitStructure.GPIO_Pin = LCMXO2_SS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed; /* 2 MHz */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(LCMXO2_SS_GPIO_PORT, &GPIO_InitStructure);

    /* Configure LCMXO2_RESET as Digital Output */
    GPIO_InitStructure.GPIO_Pin = LCMXO2_RESET_PIN;
    GPIO_Init(LCMXO2_RESET_GPIO_PORT, &GPIO_InitStructure);

    /* Default SPI Configuration */
    SPI_StructInit(&SPI_InitStruct);

    /* Configure custom fields */
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set; // Internal NSS mgmt and pull internal NSS high;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;

    /* Configure SPI module */
    SPI_Init(SPI_COM, &SPI_InitStruct);

    /* Enable SPI module */
    SPI_Cmd(SPI_COM, ENABLE);
    LCMXO2_RESET_WRITE(LCMXO2_RESET_ON);
    LCMXO2_SS_WRITE(LCMXO2_SS_OFF);
}

uint16_t hb_lcmxo2_tx_rx(uint16_t value)
{
	uint8_t timer;

	/* Send a new 16 bits word */
	LCMXO2_SS_WRITE(LCMXO2_SS_ON);

    SPI_I2S_SendData16(SPI_COM, value);

    /* Wait until transmit complete */
    while(SPI_I2S_GetFlagStatus(SPI_COM, SPI_I2S_FLAG_TXE) == RESET);

    /* Wait until receive complete */
    while(SPI_I2S_GetFlagStatus(SPI_COM, SPI_I2S_FLAG_RXNE) == RESET);

    /* Wait until SPI is not busy anymore */
    while(SPI_I2S_GetFlagStatus(SPI_COM, SPI_I2S_FLAG_BSY) == SET);

    for(timer=0;timer<50;timer++);

    LCMXO2_SS_WRITE(LCMXO2_SS_OFF);
    /* Return 16 bit received word */

    return SPI_I2S_ReceiveData16(SPI_COM);
}

void hb_lcmxo2_set_pwm(uint16_t motor, int16_t value)
{
	uint16_t speed;

	if(value < 0)
		speed = (uint16_t)(((-value)&0x07FF)|0x0800);
	else
		speed = (uint16_t)value&0x07FF;
	hb_lcmxo2_tx_rx(motor);
	hb_lcmxo2_tx_rx(speed);
}

int16_t hb_lcmxo2_get_qei(uint16_t encoder)
{
	hb_lcmxo2_tx_rx(encoder&0x000F);
	return hb_lcmxo2_tx_rx(0x0000)&0x0FFF;
}
