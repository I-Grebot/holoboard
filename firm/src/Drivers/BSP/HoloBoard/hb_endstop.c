/*
 * hb_endstop.c
 *
 *  Created on: May 25, 2017
 *      Author: ldoppea
 */

#include "holoboard.h"

void hb_endstop_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

    /* Enable Port Clock */
	ENDSTOP0_GPIO_CLK_ENABLE();

	/* Configure switches as input */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;

	GPIO_InitStructure.GPIO_Pin = ENDSTOP0_PIN;
	GPIO_Init(ENDSTOP0_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ENDSTOP1_PIN;
	GPIO_Init(ENDSTOP1_GPIO_PORT, &GPIO_InitStructure);
}
