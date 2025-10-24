/**
 ******************************************************************************
 * @file    led1642gw.c
 * @author  Gemini
 * @brief   Source file for the LED1642GW 16-Channel LED Driver.
 * (SPI Implementation)
 ******************************************************************************
 */

#include "led1642gw.h"

/* Public Function Implementations -----------------------------------------*/

/**
 * @brief  Initializes the LED1642GW driver handle.
 * @param  dev: Pointer to the LED1642GW_Handle_t structure.
 * @param  hspi: Pointer to the SPI_HandleTypeDef for communication.
 * @param  le_port:  GPIO Port for LE.
 ... (all other params) ...
 * @retval None
 */
void LED1642GW_Init(LED1642GW_Handle_t *dev,
                    SPI_HandleTypeDef *hspi,
                    GPIO_TypeDef *le_port, uint16_t le_pin,
                    GPIO_TypeDef *oe_port, uint16_t oe_pin)
{
    /* Store SPI handle and GPIO configuration in the handle */
    dev->hspi = hspi;
    dev->le_port = le_port;
    dev->le_pin = le_pin;
    dev->oe_port = oe_port;
    dev->oe_pin = oe_pin;

    /*
     * Set initial pin states:
     * LE = LOW (ready to receive data)
     * OE = HIGH (outputs disabled)
     * SPI peripheral (CLK/SDI) is managed by the HAL.
     */
    HAL_GPIO_WritePin(dev->le_port, dev->le_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->oe_port, dev->oe_pin, GPIO_PIN_SET); // OE is active-low
}

/**
 * @brief  Writes data to a single IC or a daisy-chain of ICs via SPI.
 * @param  dev: Pointer to the driver handle.
 * @param  channel_data_array: Array of 16-bit data values. The first element
 * (index 0) is for the IC closest to the MCU (first in chain).
 * @param  num_ics: The number of daisy-chained ICs.
 * @retval None
 */
void LED1642GW_WriteDaisy(LED1642GW_Handle_t *dev, uint16_t *channel_data_array, uint8_t num_ics)
{
    /* Ensure LE is low before shifting data */
    HAL_GPIO_WritePin(dev->le_port, dev->le_pin, GPIO_PIN_RESET);

    /*
     * Data is shifted to the last IC in the chain first.
     * So we send data from the end of the array (last IC)
     * to the start of the array (first IC).
     *
     * We now assume SPI is configured for 8-BIT DATA.
     */
    for (int8_t i = num_ics - 1; i >= 0; i--)
    {
        uint16_t data = channel_data_array[i];
        uint8_t high_byte = (uint8_t)(data >> 8);
        uint8_t low_byte = (uint8_t)(data & 0xFF);

        /* Send the high byte */
        HAL_SPI_Transmit(dev->hspi, &high_byte, 1, HAL_MAX_DELAY);

        /*
         * If this is the low byte of the *last* IC (the one closest
         * to the MCU, index 0), we set LE high just before sending it.
         * This ensures LE is high for the final 8 clock pulses
         * of the *entire* transmission.
         */
        if (i == 0)
        {
            HAL_GPIO_WritePin(dev->le_port, dev->le_pin, GPIO_PIN_SET);
        }

        /* Send the low byte */
        HAL_SPI_Transmit(dev->hspi, &low_byte, 1, HAL_MAX_DELAY);
    }

    /*
     * Wait for SPI transmission to be complete (Busy flag = 0)
     * before pulling the latch low.
     */
    while (__HAL_SPI_GET_FLAG(dev->hspi, SPI_FLAG_BSY))
    {
        /* Wait */
    }

    /* Latch is complete, set LE low. */
    HAL_GPIO_WritePin(dev->le_port, dev->le_pin, GPIO_PIN_RESET);
}

/**
 * @brief  Sets the Output Enable (OE) pin state.
 * @param  dev: Pointer to the driver handle.
 * @param  enable: 1 to enable outputs (OE=LOW), 0 to disable (OE=HIGH).
 * @retval None
 */
void LED1642GW_SetOutputEnable(LED1642GW_Handle_t *dev, uint8_t enable)
{
    if (enable)
    {
        /* Enable outputs (Active LOW) */
        HAL_GPIO_WritePin(dev->oe_port, dev->oe_pin, GPIO_PIN_RESET);
    }
    else
    {
        /* Disable outputs */
        HAL_GPIO_WritePin(dev->oe_port, dev->oe_pin, GPIO_PIN_SET);
    }
}


