/**
 ******************************************************************************
 * @file    led1642gw.h
 * @author  Gemini
 * @brief   Header file for the LED1642GW 16-Channel LED Driver.
 *
 * This driver provides basic functions to initialize the driver handle and
 * write ON/OFF data to one or more daisy-chained LED1642GW ICs using
 * STM32 HAL GPIO functions.
 *
 * @note
 * 1. This driver uses bit-banging. For high-speed updates, consider
 * using an SPI peripheral.
 * 2. GPIO pins (SDI, CLK, LE, OE) MUST be configured as
 * GPIO_OUTPUT_PP (Push-Pull) in your main.c (e.g., via CubeMX).
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
 * @note   This structure holds the GPIO port and pin definitions for
 * communicating with the IC.
 */
typedef struct
{
    /* Serial Data In (SDI) Pin */
    GPIO_TypeDef *sdi_port;
    uint16_t sdi_pin;

    /* Clock (CLK) Pin */
    GPIO_TypeDef *clk_port;
    uint16_t clk_pin;

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
 * @param  sdi_port: GPIO Port for SDI.
 * @param  sdi_pin:  GPIO Pin for SDI.
 * @param  clk_port: GPIO Port for CLK.
 * @param  clk_pin:  GPIO Pin for CLK.
 * @param  le_port:  GPIO Port for LE.
 * @param  le_pin:   GPIO Pin for LE.
 * @param  oe_port:  GPIO Port for OE.
 * @param  oe_pin:   GPIO Pin for OE.
 * @retval None
 */
void LED1642GW_Init(LED1642GW_Handle_t *dev,
                    GPIO_TypeDef *sdi_port, uint16_t sdi_pin,
                    GPIO_TypeDef *clk_port, uint16_t clk_pin,
                    GPIO_TypeDef *le_port, uint16_t le_pin,
                    GPIO_TypeDef *oe_port, uint16_t oe_pin);

/**
 * @brief  Shifts out 16 bits of data to the IC.
 * @note   This function is private and called by LED1642GW_WriteDaisy.
 * @param  dev: Pointer to the driver handle.
 * @param  data: 16-bit data (1 bit per channel, MSB is CH15, LSB is CH0).
 * @retval None
 */
static void LED1642GW_ShiftOut(LED1642GW_Handle_t *dev, uint16_t data);

/**
 * @brief  Writes data to a single IC or a daisy-chain of ICs.
 * @param  dev: Pointer to the driver handle.
 *Details
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
