/**
 *  Project     Campos
 *  @file		ov5647.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for ov5647.h
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
#ifndef __OV5647_H
#define __OV5647_H

/* Includes ------------------------------------------------------------------*/
#include "camera.h"
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"

/* Exported types -----------------------------------------------------------*/
struct regval_list {
	uint16_t addr;
	uint8_t data;
};

/* Defines & Macros -----------------------------------------------------------*/
#define REG_DLY  0xffff

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

#define  OV5647_ID    0x5647
#define OV5647_SC_CMMN_CHIP_ID_H 	0x300A
#define OV5647_SC_CMMN_CHIP_ID_L 	0x300B

/* Function prototypes -------------------------------------------------------*/

void ov5647_Init(uint16_t DeviceAddr);
uint16_t ov5647_ReadID(uint16_t DeviceAddr);
void CAMERA_IO_Init(void);
void CAMERA_IO_Write(uint8_t addr, uint16_t reg, uint8_t value);
uint8_t CAMERA_IO_Read(uint8_t addr, uint16_t reg);
void CAMERA_Delay(uint32_t delay);

#endif // __OV5647_H
