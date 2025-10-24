/**
 ******************************************************************************
 * @file    led1642gw.c
 * @author  Gemini
 * @brief   Source file for the LED1642GW 16-Channel LED Driver.
 ******************************************************************************
 */

#include "led1642gw.h"

/* Private Function Implementations ----------------------------------------*/

/**
 * @brief  Shifts out 16 bits of data to the IC.
 * @note   This function is private and called by LED1642GW_WriteDaisy.
 * @param  dev: Pointer to the driver handle.
 * @param  data: 16-bit data (1 bit per channel, MSB is CH15, LSB is CH0).
 * @retval None
 */
void LED1642GW_ShiftOut(LED1642GW_Handle_t *dev, uint16_t data, LE_Setting_t setting)
{
	int clockpulse;

	switch (setting) {
	case WRITE_SWITCH:
		clockpulse = 0;
		break;
	case DATA_LATCH:
		clockpulse = 2;
		break;
	case GLOBAL_LATCH:
		clockpulse = 4;
		break;
	default:
		return;
	}

    /* Send 16 bits, MSB first */
    for (int bit = 15; bit >= 0; bit--)
    {
        if (bit == clockpulse) {
			HAL_GPIO_WritePin(dev->le_port, dev->le_pin, GPIO_PIN_SET);
		}

        /* Set SDI pin based on the MSB of data */
        if (data & 0x8000)
        {
            HAL_GPIO_WritePin(dev->sdi_port, dev->sdi_pin, GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(dev->sdi_port, dev->sdi_pin, GPIO_PIN_RESET);
        }

        /* Set CLK high */
        HAL_GPIO_WritePin(dev->clk_port, dev->clk_pin, GPIO_PIN_SET);
        /* Set CLK low */
        HAL_GPIO_WritePin(dev->clk_port, dev->clk_pin, GPIO_PIN_RESET);


        /* Shift data left to get the next bit */
        data <<= 1;
    }
    HAL_GPIO_WritePin(dev->sdi_port, dev->sdi_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->le_port, dev->le_pin, GPIO_PIN_RESET);

}

/* Public Function Implementations -----------------------------------------*/

/**
 * @brief  Initializes the LED1642GW driver handle.
 * @param  dev: Pointer to the LED1642GW_Handle_t structure.
 * @param  sdi_port: GPIO Port for SDI.
 * ... (all other params) ...
 * @retval None
 */
void LED1642GW_Init(LED1642GW_Handle_t *dev,
                    GPIO_TypeDef *sdi_port, uint16_t sdi_pin,
                    GPIO_TypeDef *clk_port, uint16_t clk_pin,
                    GPIO_TypeDef *le_port, uint16_t le_pin,
                    GPIO_TypeDef *oe_port, uint16_t oe_pin)
{
    /* Store GPIO configuration in the handle */
    dev->sdi_port = sdi_port;
    dev->sdi_pin = sdi_pin;
    dev->clk_port = clk_port;
    dev->clk_pin = clk_pin;
    dev->le_port = le_port;
    dev->le_pin = le_pin;
    dev->oe_port = oe_port;
    dev->oe_pin = oe_pin;

    /* * Set initial pin states:
     * CLK = LOW
     * LE = LOW
     * OE = HIGH (disabled)
     */
    HAL_GPIO_WritePin(dev->clk_port, dev->clk_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->le_port, dev->le_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(dev->oe_port, dev->oe_pin, GPIO_PIN_SET); // OE is active-low

    for (int i=0; i < 15; i++) {
    	LED1642GW_ShiftOut(dev, 0x000F, DATA_LATCH);
    	HAL_Delay(1);
    }
    	LED1642GW_ShiftOut(dev, 0x000F, GLOBAL_LATCH);

}

/**
 * @brief  Writes data to a single IC or a daisy-chain of ICs.
 * @param  dev: Pointer to the driver handle.
 * @param  channel_data_array: Array of 16-bit data values. The first element
 * (index 0) is for the IC closest to the MCU (first in chain).
 * @param  num_ics: The number of daisy-chained ICs.
 * @retval None
 */
void LED1642GW_WriteDaisy(LED1642GW_Handle_t *dev, uint16_t *channel_data_array, uint8_t num_ics)
{
    /* * Data is shifted to the last IC in the chain first.
     * So we send data from the end of the array (last IC)
     * to the start of the array (first IC).
     */
    for (int8_t i = num_ics - 1; i >= 0; i--)
    {
        LED1642GW_ShiftOut(dev, channel_data_array[i], WRITE_SWITCH);
    }
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
