/**
 *  Project     Campos
 *  @file		lcd_io.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		IO port to the LCD/TFT
 *
 *  @copyright	GPL3
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/* Includes ------------------------------------------------------------------*/
#include "lcd_io.h"

/**
 * @brief  Configures the LCD parallel interface.
 * @param  None
 * @retval None
 */
void LCD_IO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure NCS in Output Push-Pull mode */
	LCD_NCS_GPIO_CLK_ENABLE();
	GPIO_InitStructure.Pin = LCD_NCS_PIN | LCD_RES_PIN | LCD_CD_PIN
			| LCD_WRX_PIN | LCD_RDX_PIN;

	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(LCD_GPIO_PORT, &GPIO_InitStructure);

	LCD_DATA_GPIO_CLK_ENABLE();
	GPIO_InitStructure.Pin = GPIO_PIN_All;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(LCD_DATA_GPIO_PORT, &GPIO_InitStructure);

	LCD_CS_HIGH();
	LCD_WRX_HIGH();
	LCD_RDX_HIGH();

	// Generate a reset
	LCD_RESET_HIGH();
	HAL_Delay(10);
	LCD_RESET_LOW();
	HAL_Delay(10);
	LCD_RESET_HIGH();
	HAL_Delay(10);

	LCD_CS_HIGH();
	LCD_WRX_HIGH();
	LCD_RDX_HIGH();
	HAL_Delay(10);

	/* activate the controller by WRX or RDX */
	LCD_CS_LOW();

}

/**
 * @brief  Write a data value.
 * @param  value to write
 * @retval None
 */
void LCD_IO_WriteData(uint16_t value) {

	// Set CD to send data
	LCD_CD_DATA();

	// Write value
	LCD_DATA(value);

	// Short WRX pulse
	LCD_WRX_LOW();
	LCD_WRX_HIGH();
}

/**
 * @brief  Write a register value..
 * @param  value to write
 * @retval None
 */
void LCD_IO_WriteReg(uint8_t value) {

	// Reset CD to send commands
	LCD_CD_CMD();

	// Write value
	LCD_DATA(value);

	// Short WRX pulse
	LCD_WRX_LOW();
	LCD_WRX_HIGH();
}

/**
 * @brief  Read data from the LCD
 * @param  None
 * @retval 16 bit data from the LCD
 */
uint16_t LCD_IO_ReadData(void) {
	uint16_t data;
	GPIO_InitTypeDef GPIO_InitStructure;

	// Switch all LCD data ports to input
	GPIO_InitStructure.Pin = GPIO_PIN_All;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(LCD_DATA_GPIO_PORT, &GPIO_InitStructure);

	// Set CD to read data
	LCD_CD_DATA();

	// Short RDX pulse
	LCD_RDX_LOW();
	// Read data now
	data = LCD_DATA_GPIO_PORT->IDR;
	LCD_RDX_HIGH();

	// Switch back to output
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(LCD_DATA_GPIO_PORT, &GPIO_InitStructure);

	return data;
}



