/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot 2017
 * -----------------------------------------------------------------------------
 * @file       hb_io_mapping.h
 * @author     Pierrick
 * @date       Apr 23, 2017
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *   This header describes the I/O mapping of the HoloBoard BSP.
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

#ifndef __HB_IO_MAPPING_H
#define __HB_IO_MAPPING_H

/**
********************************************************************************
**
**  RGB LED
**    3x Digital Outputs
**
********************************************************************************
*/

/** @addtogroup HB_LOW_LEVEL_RGB_LED
 * @{
 */

/* Number of instances */
#define LEDn                            ((uint8_t) 3)

/* MCU I/O connected on the LED anodes */
#define LEDx_ON                         (Bit_RESET)
#define LEDx_OFF                        (Bit_SET)

/* LEDR Mapped on PA0 */
#define LEDR_GPIO_PORT                   GPIOA
#define LEDR_GPIO_CLK_ENABLE()           RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE)
#define LEDR_GPIO_CLK_DISABLE()          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, DISABLE)
#define LEDR_PIN                         GPIO_Pin_0
#define LEDR_WRITE(_v)                   GPIO_WriteBit(LEDR_GPIO_PORT, LEDR_PIN, (_v))
#define LEDR_TOGGLE()                    GPIO_ToggleBits(LEDR_GPIO_PORT, LEDR_PIN)

/* LEDG Mapped on PA1 */
#define LEDG_GPIO_PORT                   GPIOA
#define LEDG_GPIO_CLK_ENABLE()           RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE)
#define LEDG_GPIO_CLK_DISABLE()          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, DISABLE)
#define LEDG_PIN                         GPIO_Pin_1
#define LEDG_WRITE(_v)                   GPIO_WriteBit(LEDG_GPIO_PORT, LEDG_PIN, (_v))
#define LEDG_TOGGLE()                    GPIO_ToggleBits(LEDG_GPIO_PORT, LEDG_PIN)

/* LEDB Mapped on PA2 */
#define LEDB_GPIO_PORT                   GPIOA
#define LEDB_GPIO_CLK_ENABLE()           RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE)
#define LEDB_GPIO_CLK_DISABLE()          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, DISABLE)
#define LEDB_PIN                         GPIO_Pin_2
#define LEDB_WRITE(_v)                   GPIO_WriteBit(LEDB_GPIO_PORT, LEDB_PIN, (_v))
#define LEDB_TOGGLE()                    GPIO_ToggleBits(LEDB_GPIO_PORT, LEDB_PIN)

/**
 * @}
 */

#endif /* __HB_IO_MAPPING_H */
