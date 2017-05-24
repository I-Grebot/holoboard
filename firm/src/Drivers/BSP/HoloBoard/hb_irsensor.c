/*
 * hb_irsensor.c
 *
 *  Created on: May 24, 2017
 *      Author: ldoppea
 */

#include "holoboard.h"

void hb_irsensor_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

    /* Enable Port Clock */
	IRSENSOR0_GPIO_CLK_ENABLE();
	IRSENSOR1_GPIO_CLK_ENABLE();
	IRSENSOR2_GPIO_CLK_ENABLE();
	IRSENSOR3_GPIO_CLK_ENABLE();
	IRSENSOR4_GPIO_CLK_ENABLE();
	IRSENSOR5_GPIO_CLK_ENABLE();
	IRSENSOR6_GPIO_CLK_ENABLE();
	IRSENSOR7_GPIO_CLK_ENABLE();
	IRSENSOR8_GPIO_CLK_ENABLE();
	IRSENSOR9_GPIO_CLK_ENABLE();

	/* Configure switches as input */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;

	GPIO_InitStructure.GPIO_Pin = IRSENSOR0_PIN;
	GPIO_Init(IRSENSOR0_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IRSENSOR1_PIN;
	GPIO_Init(IRSENSOR1_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IRSENSOR2_PIN;
	GPIO_Init(IRSENSOR2_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IRSENSOR3_PIN;
	GPIO_Init(IRSENSOR3_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IRSENSOR4_PIN;
	GPIO_Init(IRSENSOR4_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IRSENSOR5_PIN;
	GPIO_Init(IRSENSOR5_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IRSENSOR6_PIN;
	GPIO_Init(IRSENSOR6_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IRSENSOR7_PIN;
	GPIO_Init(IRSENSOR7_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IRSENSOR8_PIN;
	GPIO_Init(IRSENSOR8_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IRSENSOR9_PIN;
	GPIO_Init(IRSENSOR9_GPIO_PORT, &GPIO_InitStructure);
}
