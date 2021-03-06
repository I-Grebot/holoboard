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

/**
 * @}
 */

/**
********************************************************************************
**
**    Main Motors Full H-Bridges (DRV8872) and QEI through LCMXO2-256HC (SPI)
**    1x Digital Output for LCMXO2 Reset
**    1x Digital Output for SPI_MISO
**    1x Digital Output for SPI_CLK
**    1x Digital Output for SPI_SS (software managed)
**    1x Digital Input for SPI_MOSI
**	  4x Digital Inputs for fault feedback (MOT0_FAULT/MOT1_FAULT/MOT2_FAULT/MOT3_FAULT)
**
********************************************************************************
*/

/** @addtogroup HB_LOW_LEVEL_LCMXO2_SPI
 * @{
 */

/* General definitions */
#define SPI_COM                             SPI1
#define SPI_CLK_ENABLE()                    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE)
#define SPI_CLK_DISABLE()                   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE)
#define SPI_IRQn                            SPI1_IRQn

/* Sleep, Reset and Fault are active low */
#define LCMXO2_RESET_ON                       (Bit_SET)
#define LCMXO2_RESET_OFF                      (Bit_RESET)
#define LCMXO2_SS_ON                       	  (Bit_RESET)
#define LCMXO2_SS_OFF                         (Bit_SET)
#define MOT_STATUS_FAULT                      (Bit_RESET)
#define MOT_STATUS_NORMAL                     (Bit_SET)

/* SPI_CLK Mapped on PA5 */
#define SPI_CLK_GPIO_PORT					GPIOA
#define SPI_CLK_PIN                        	GPIO_Pin_5
#define SPI_CLK_GPIO_CLK_ENABLE()          	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE)
#define SPI_CLK_GPIO_CLK_DISABLE()         	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, DISABLE)
#define SPI_CLK_AF							GPIO_AF5_SPI1
#define SPI_CLK_PIN_SOURCE                 	GPIO_PinSource5

/* SPI_MISO Mapped on PA6 */
#define SPI_MISO_GPIO_PORT					GPIOA
#define SPI_MISO_PIN                        GPIO_Pin_6
#define SPI_MISO_GPIO_CLK_ENABLE()          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE)
#define SPI_MISO_GPIO_CLK_DISABLE()         RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, DISABLE)
#define SPI_MISO_AF							GPIO_AF5_SPI1
#define SPI_MISO_PIN_SOURCE                 GPIO_PinSource6

/* SPI_MOSI Mapped on PA7 */
#define SPI_MOSI_GPIO_PORT					GPIOA
#define SPI_MOSI_PIN                        GPIO_Pin_7
#define SPI_MOSI_GPIO_CLK_ENABLE()          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE)
#define SPI_MOSI_GPIO_CLK_DISABLE()         RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, DISABLE)
#define SPI_MOSI_AF							GPIO_AF5_SPI1
#define SPI_MOSI_PIN_SOURCE                 GPIO_PinSource7

/* LCMXO2_SS Mapped on PC5 */
#define LCMXO2_SS_GPIO_PORT                 GPIOC
#define LCMXO2_SS_GPIO_CLK_ENABLE()         RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)
#define LCMXO2_SS_GPIO_CLK_DISABLE()        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, DISABLE)
#define LCMXO2_SS_PIN                       GPIO_Pin_5
#define LCMXO2_SS_WRITE(_v)                 GPIO_WriteBit(LCMXO2_SS_GPIO_PORT, LCMXO2_SS_PIN, (_v))

/* LCMXO2_RESET Mapped on PC4 */
#define LCMXO2_RESET_GPIO_PORT              GPIOC
#define LCMXO2_RESET_GPIO_CLK_ENABLE()      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)
#define LCMXO2_RESET_GPIO_CLK_DISABLE()     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, DISABLE)
#define LCMXO2_RESET_PIN                    GPIO_Pin_4
#define LCMXO2_RESET_WRITE(_v)              GPIO_WriteBit(LCMXO2_RESET_GPIO_PORT, LCMXO2_RESET_PIN, (_v))

/* MOT0_FAULT Mapped on PE9 */
#define MOT0_FAULT_GPIO_PORT                GPIOE
#define MOT0_FAULT_GPIO_CLK_ENABLE()        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE)
#define MOT0_FAULT_GPIO_CLK_DISABLE()       RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, DISABLE)
#define MOT0_FAULT_PIN                      GPIO_Pin_9

/* MOT1_FAULT Mapped on PE8 */
#define MOT1_FAULT_GPIO_PORT                GPIOE
#define MOT1_FAULT_GPIO_CLK_ENABLE()        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE)
#define MOT1_FAULT_GPIO_CLK_DISABLE()       RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, DISABLE)
#define MOT1_FAULT_PIN                      GPIO_Pin_8

/* MOT2_FAULT Mapped on PE7 */
#define MOT2_FAULT_GPIO_PORT                GPIOE
#define MOT2_FAULT_GPIO_CLK_ENABLE()        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE)
#define MOT2_FAULT_GPIO_CLK_DISABLE()       RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, DISABLE)
#define MOT2_FAULT_PIN                      GPIO_Pin_7

/* MOT3_FAULT Mapped on PB2 */
#define MOT3_FAULT_GPIO_PORT                GPIOB
#define MOT3_FAULT_GPIO_CLK_ENABLE()        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)
#define MOT3_FAULT_GPIO_CLK_DISABLE()       RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, DISABLE)
#define MOT3_FAULT_PIN                      GPIO_Pin_2
/**
 * @}
 */

/**
********************************************************************************
**
**  Debug UART Interface [USART1]
**    2x Digital Communication Signals (UART_RX / UART_TX)
**
********************************************************************************
*/

/** @addtogroup BB_LOW_LEVEL_DEBUG_UART
 * @{
 */

/* Definitions for Debug USART connected on USART1 */
#define DBG_COM                         USART1
#define DBG_CLK_ENABLE()                RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
#define DBG_CLK_DISABLE()               RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE)
#define DBG_IRQn                        USART1_IRQn
#define DBG_ISR                         USART1_IRQHandler

/* UART_RX Mapped on PA10 */
#define DBG_RX_GPIO_PORT                GPIOA
#define DBG_RX_PIN                      GPIO_Pin_10
#define DBG_RX_GPIO_CLK_ENABLE()        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE)
#define DBG_RX_GPIO_CLK_DISABLE()       RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, DISABLE)
#define DBG_RX_AF                       GPIO_AF7_USART1
#define DBG_RX_PIN_SOURCE               GPIO_PinSource10

/* UART_TX Mapped on PA9 */
#define DBG_TX_GPIO_PORT                GPIOA
#define DBG_TX_PIN                      GPIO_Pin_9
#define DBG_TX_GPIO_CLK_ENABLE()        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE)
#define DBG_TX_GPIO_CLK_DISABLE()       RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, DISABLE)
#define DBG_TX_AF                       GPIO_AF7_USART1
#define DBG_TX_PIN_SOURCE               GPIO_PinSource9

/**
 * @}
 */

#endif /* __HB_IO_MAPPING_H */
