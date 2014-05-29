/**
 *  Project     Campos
 *  @file		track.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for track.c
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
#ifndef TRACK_H_
#define TRACK_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Type defs -----------------------------------------------------------------*/
typedef enum {
	TRACK_INIT = 0,
	TRACK_SEARCHING = 1,
	TRACK_LIGHT_FOUND = 2,
	TRACK_CENTER_DETECTED = 3,
	TRACK_LOST = 4
} Track_StatusTypeDef;

/* global variables ---------------------------------------------------------*/
extern int position_x; 		// position in sensor pixels
extern int position_y; 		// position in sensor pixels
extern int position_intx; 	// position in sensor pixels in a 120x120pixel frame
extern int position_inty; 	// position in sensor pixels in a 120x120pixel frame
extern int position_subx; 	// sub pixel position from 0..999
extern int position_suby; 	// sub pixel position from 0..999
extern int intensity; 		// intensity (integral of all pixel values)
extern Track_StatusTypeDef track_status; // Status of tracking


/* Function prototypes -------------------------------------------------------*/
void TRACK_Init(void);
void TRACK_Search(void);


#endif /* TRACK_H_ */
