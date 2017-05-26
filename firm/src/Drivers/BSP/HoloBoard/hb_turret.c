/*
 * hb_turret.c
 *
 *  Created on: May 25, 2017
 *      Author: ldoppea
 */

#include "holoboard.h"

void hb_turret_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStruct;

	/* Enable GPIOs Clock */
	TURRET_CSN_GPIO_CLK_ENABLE();
	TURRET_SCK_GPIO_CLK_ENABLE();
	TURRET_MOSI_GPIO_CLK_ENABLE();
	TURRET_MISO_GPIO_CLK_ENABLE();

	/* Enable Control Interface SPI clock */
	TURRET_CLK_ENABLE();

	/* Configure TURRET_CSN as alternate function */
	GPIO_InitStructure.GPIO_Pin = TURRET_CSN_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed; /* 10 MHz */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(TURRET_CSN_GPIO_PORT, &GPIO_InitStructure);

	/* Configure TURRET_SCK as alternate function */
	GPIO_InitStructure.GPIO_Pin = TURRET_SCK_PIN;
	GPIO_Init(TURRET_SCK_GPIO_PORT, &GPIO_InitStructure);

	/* Configure TURRET_MOSI as alternate function */
	GPIO_InitStructure.GPIO_Pin = TURRET_MOSI_PIN;
	GPIO_Init(TURRET_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/* Configure USART RTS as alternate function */
	GPIO_InitStructure.GPIO_Pin = TURRET_MISO_PIN;
	GPIO_Init(TURRET_MISO_GPIO_PORT, &GPIO_InitStructure);

	/* Connect Alternate functions */
	GPIO_PinAFConfig(TURRET_SCK_GPIO_PORT,   TURRET_SCK_PIN_SOURCE,   TURRET_SCK_AF);
	GPIO_PinAFConfig(TURRET_MOSI_GPIO_PORT,  TURRET_MOSI_PIN_SOURCE,  TURRET_MOSI_AF);
	GPIO_PinAFConfig(TURRET_MISO_GPIO_PORT,  TURRET_MISO_PIN_SOURCE,  TURRET_MISO_AF);

	/* Configure TURRET_CSN as Digital Output */
	GPIO_InitStructure.GPIO_Pin = TURRET_CSN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed; /* 10 MHz */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(TURRET_CSN_GPIO_PORT, &GPIO_InitStructure);

	/* Default SPI Configuration */
	SPI_StructInit(&SPI_InitStruct);

	/* Configure custom fields */
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; /* TBC */
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set; // Internal NSS mgmt and pull internal NSS high;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;

	/* Configure SPI module */
	SPI_Init(TURRET_COM, &SPI_InitStruct);
	SPI_SSOutputCmd(TURRET_COM, ENABLE);



	/* Enable SPI module */
	TURRET_CSN_WRITE(HB_TURRET_FRAME_IDLE);
	SPI_Cmd(TURRET_COM, ENABLE);
}
