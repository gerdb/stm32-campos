/**
 *  Project     Campos
 *  @file		irlink.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for irlink.c
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
#ifndef __IRLINK_H
#define __IRLINK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "track.h"

/* Function Prototypes --------------------------------------------------------*/
void IRLINK_Init(void);
void IRLINK_Output(int value);
void IRLINK_StartHeader(void);
void IRLINK_500usTask(void);
void IRLINK_Send(Track_StatusTypeDef track_status ,
		int position_x, int position_subx,
		int position_y, int position_suby,
		int intensity);

#endif /* __IRLINK_H */
