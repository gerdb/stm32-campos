/**
 *  Project     Campos
 *  @file		usartl2.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for usartl2.c
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
#ifndef USARTL2_H_
#define USARTL2_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"
#include "printf.h"
#include "usartl1.h"

/* Typedefs ------------------------------------------------------------------*/
typedef enum {
	DECODE_CMD, DECODE_ADDRESS, DECODE_DATA, DECODE_ENTER
} enDecodeState;



/* Function Prototypes --------------------------------------------------------*/
void USARTL2_Init(void);
void USARTL2_Decode(char c);
void USARTL2_FrameCallback(void);


#endif /* USART_H_ */
