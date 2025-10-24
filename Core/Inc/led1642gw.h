/**
 ******************************************************************************
 * @file    led1642gw.h
 * @author  Gemini
 * @brief   Header file for the LED1642GW 16-Channel LED Driver.
 *
 * This driver provides basic functions to initialize the driver handle and
 * write ON/OFF data to one or more daisy-chained LED1642GW ICs using
 * the STM32 HAL SPI peripheral.
 *
 * @note
 * 1. This driver uses an SPI peripheral. You MUST configure the SPI
 * (e.g., in CubeMX) with the following settings:
 * - Mode: Transmit-Only Master
 * - Data Size: 16 Bits
 * - First Bit: MSB First
 * - CPOL: Low
 * - CPHA: 1 Edge (Rising edge)
 * (This corresponds to SPI Mode 0)
 *
 * 2. GPIO pins for LE (Latch) and OE (Output Enable) MUST be configured as
 * GPIO_OUTPUT_PP (Push-Pull).
 *
 * 3. Brightness/dimming is controlled by applying a PWM signal to the
 * OE (Output Enable) pin. This must be configured separately using
 * a timer peripheral on the STM32. This driver only provides a
 * function to manually enable/disable the output.
 *
 ******************************************************************************
 */

#ifndef LED1642GW_H
#define LED1642GW_H

/* Includes ------------------------------------------------------------------*/
/* * Change this to your specific STM32 HAL header
 * (e.g., stm32f4xx_hal.h, stm32g0xx_hal.h, etc.)
 */
#include "stm32f4xx_hal.h"

/* Typedefs ------------------------------------------------------------------*/

/**
 * @brief  LED1642GW driver handle structure.
 * @note   This structure holds the SPI handle and GPIO definitions for
 * communicating with the IC.
 */
typedef struct
{
    /* SPI Handle */
    SPI_HandleTypeDef *hspi;

    /* Latch Enable (LE) Pin */
    GPIO_TypeDef *le_port;
    uint16_t le_pin;

    /* Output Enable (OE) Pin - Active LOW */
    GPIO_TypeDef *oe_port;
    uint16_t oe_pin;

} LED1642GW_Handle_t;

/* Function Prototypes -------------------------------------------------------*/

/**
 * @brief  Initializes the LED1642GW driver handle.
 * @param  dev: Pointer to the LED1642GW_Handle_t structure.
 * @param  hspi: Pointer to the SPI_HandleTypeDef for communication.
 * @param  le_port:  GPIO Port for LE.
 * @param  le_pin:   GPIO Pin for LE.
 * @param  oe_port:  GPIO Port for OE.
 * @param  oe_pin:   GPIO Pin for OE.
 * @retval None
 */
void LED1642GW_Init(LED1642GW_Handle_t *dev,
                    SPI_HandleTypeDef *hspi,
                    GPIO_TypeDef *le_port, uint16_t le_pin,
                    GPIO_TypeDef *oe_port, uint16_t oe_pin);

/**
 * @brief  Writes data to a single IC or a daisy-chain of ICs via SPI.
 * @param  dev: Pointer to the driver handle.
 * @param  channel_data_array: Array of 16-bit data values. The first element
 * (index 0) is for the IC closest to the MCU (first in chain).
 * @param  num_ics: The number of daisy-chained ICs.
 * @retval None
 */
void LED1642GW_WriteDaisy(LED1642GW_Handle_t *dev, uint16_t *channel_data_array, uint8_t num_ics);

/**
 * @brief  Pulses the Latch Enable (LE) pin to latch data.
 * @param  dev: Pointer to the driver handle.
 * @retval None
 */
void LED1642GW_Latch(LED1642GW_Handle_t *dev);

/**
 * @brief  Sets the Output Enable (OE) pin state.
 * @param  dev: Pointer to the driver handle.
 * @param  enable: 1 to enable outputs (OE=LOW), 0 to disable (OE=HIGH).
 * @retval None
 */
void LED1642GW_SetOutputEnable(LED1642GW_Handle_t *dev, uint8_t enable);

#endif /* LED1642GW_H */

