/*
 * hb_power.c
 *
 *  Created on: May 25, 2017
 *      Author: ldoppea
 */

/* Inclusions */
#include "holoboard.h"

/* Private variables */
static const GPIO_TypeDef* HB_PWR_PORT[PWRn] = {VP1_EN_GPIO_PORT,
                                                VP2_EN_GPIO_PORT,
                                                VP3_EN_GPIO_PORT};

static const uint16_t HB_PWR_PIN[PWRn] = {VP1_EN_PIN,
                                          VP2_EN_PIN,
                                          VP3_EN_PIN};

/**
  * @brief  Configure all VPn power GPIOs (push-pull, no pull-up)
  * @param  None
  * @retval None
  */
void hb_pwr_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    VP1_EN_GPIO_CLK_ENABLE();
    VP2_EN_GPIO_CLK_ENABLE();
    VP3_EN_GPIO_CLK_ENABLE();

    /* Common configuration for all VPn */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;

    GPIO_InitStructure.GPIO_Pin = VP1_EN_PIN;
    GPIO_Init(VP1_EN_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = VP2_EN_PIN;
    GPIO_Init(VP2_EN_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = VP3_EN_PIN;
    GPIO_Init(VP3_EN_GPIO_PORT, &GPIO_InitStructure);

    /* Default state: Modules OFF */
    hb_pwr_disable(HB_PWR_VP1);
    hb_pwr_disable(HB_PWR_VP2);
    hb_pwr_disable(HB_PWR_VP3);
}

/**
  * @brief  Enable a VPx power-supply
  * @param  power VPx channel to enable
  * @retval None
  */
void hb_pwr_enable(HB_PWR_TypeDef power)
{
    GPIO_WriteBit((GPIO_TypeDef*) HB_PWR_PORT[power], HB_PWR_PIN[power], PWRx_ON);
}

/**
  * @brief  Disable a VPx power-supply
  * @param  power VPx channel to disable
  * @retval None
  */
void hb_pwr_disable(HB_PWR_TypeDef power)
{
    GPIO_WriteBit((GPIO_TypeDef*) HB_PWR_PORT[power], HB_PWR_PIN[power], PWRx_OFF);
}
