/**
 *  Project     Campos
 *  @file		main.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for lcd_io.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LCD_IO_H_
#define LCD_IO_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"
#include "core_cmInstr.h"
#include "ili9325.h"
#include "fonts.h"


/* Exported variables --------------------------------------------------------*/
extern const uint8_t Font12_Table[];

/* Macros --------------------------------------------------------------------*/

/* Chip Select macro definition */
#define LCD_CS_LOW()       HAL_GPIO_WritePin(LCD_NCS_GPIO_PORT, LCD_NCS_PIN, GPIO_PIN_RESET)
#define LCD_CS_HIGH()      HAL_GPIO_WritePin(LCD_NCS_GPIO_PORT, LCD_NCS_PIN, GPIO_PIN_SET)

/* Set WRX High to send data */
#define LCD_WRX_LOW()      HAL_GPIO_WritePin(LCD_WRX_GPIO_PORT, LCD_WRX_PIN, GPIO_PIN_RESET)
#define LCD_WRX_HIGH()     HAL_GPIO_WritePin(LCD_WRX_GPIO_PORT, LCD_WRX_PIN, GPIO_PIN_SET)

/* Set WRX High to send data */
#define LCD_RDX_LOW()      HAL_GPIO_WritePin(LCD_RDX_GPIO_PORT, LCD_RDX_PIN, GPIO_PIN_RESET)
#define LCD_RDX_HIGH()     HAL_GPIO_WritePin(LCD_RDX_GPIO_PORT, LCD_RDX_PIN, GPIO_PIN_SET)

/* Set CD High to send data */
#define LCD_CD_CMD()      HAL_GPIO_WritePin(LCD_CD_GPIO_PORT, LCD_CD_PIN, GPIO_PIN_RESET)
#define LCD_CD_DATA()     HAL_GPIO_WritePin(LCD_CD_GPIO_PORT, LCD_CD_PIN, GPIO_PIN_SET)

/* Set Reset low for Reset */
#define LCD_RESET_LOW()      HAL_GPIO_WritePin(LCD_RES_GPIO_PORT, LCD_RES_PIN, GPIO_PIN_RESET)
#define LCD_RESET_HIGH()     HAL_GPIO_WritePin(LCD_RES_GPIO_PORT, LCD_RES_PIN, GPIO_PIN_SET)

#define LCD_DATA(x)     LCD_DATA_GPIO_PORT->ODR = x

#define LCD_IO_WRITE_2xDATA(x) LCD_DATA(x);\
							 LCD_WRX_GPIO_PORT->BSRRH = LCD_WRX_PIN; \
							 LCD_WRX_GPIO_PORT->BSRRL = LCD_WRX_PIN; \
							 LCD_WRX_GPIO_PORT->BSRRH = LCD_WRX_PIN; \
							 LCD_WRX_GPIO_PORT->BSRRL = LCD_WRX_PIN

#define LCD_IO_WRITE_1xDATA(x) LCD_DATA(x);__NOP();__NOP();\
							 LCD_WRX_GPIO_PORT->BSRRH = LCD_WRX_PIN;__NOP();__NOP(); \
							 LCD_WRX_GPIO_PORT->BSRRL = LCD_WRX_PIN

// DB0 .. DB15			: PE0..PE15
// NCS (ChipSelect)		: PD7
// RES (Reset)			: PD8
// CD (Command/Data)	: PD9
// WRX (Write)			: PD10
// RDX (Read)			: PD11
/**
 * @brief  LCD Control pin
 */
#define LCD_NCS_PIN                GPIO_PIN_7
#define LCD_NCS_GPIO_PORT          GPIOD
#define LCD_NCS_GPIO_CLK_ENABLE()  __GPIOD_CLK_ENABLE()
#define LCD_NCS_GPIO_CLK_DISABLE() __GPIOD_CLK_DISABLE()

#define LCD_RES_PIN                GPIO_PIN_8
#define LCD_RES_GPIO_PORT          GPIOD
#define LCD_RES_GPIO_CLK_ENABLE()  __GPIOD_CLK_ENABLE()
#define LCD_RES_GPIO_CLK_DISABLE() __GPIOD_CLK_DISABLE()

#define LCD_CD_PIN                GPIO_PIN_9
#define LCD_CD_GPIO_PORT          GPIOD
#define LCD_CD_GPIO_CLK_ENABLE()  __GPIOD_CLK_ENABLE()
#define LCD_CD_GPIO_CLK_DISABLE() __GPIOD_CLK_DISABLE()

#define LCD_WRX_PIN                GPIO_PIN_10
#define LCD_WRX_GPIO_PORT          GPIOD
#define LCD_WRX_GPIO_CLK_ENABLE()  __GPIOD_CLK_ENABLE()
#define LCD_WRX_GPIO_CLK_DISABLE() __GPIOD_CLK_DISABLE()

#define LCD_RDX_PIN                GPIO_PIN_11
#define LCD_RDX_GPIO_PORT          GPIOD
#define LCD_RDX_GPIO_CLK_ENABLE()  __GPIOD_CLK_ENABLE()
#define LCD_RDX_GPIO_CLK_DISABLE() __GPIOD_CLK_DISABLE()

#define LCD_GPIO_PORT          	GPIOD

#define LCD_DATA_GPIO_PORT          GPIOE
#define LCD_DATA_GPIO_CLK_ENABLE()  __GPIOE_CLK_ENABLE()
#define LCD_DATA_GPIO_CLK_DISABLE() __GPIOE_CLK_DISABLE()

/* Function prototypes -------------------------------------------------------*/

void LCD_IO_Init(void);
void LCD_IO_WriteData(uint16_t RegValue);
void LCD_IO_WriteReg(uint8_t Reg);
uint16_t LCD_IO_ReadData(void);

#endif /* LCD_IO_H_ */
