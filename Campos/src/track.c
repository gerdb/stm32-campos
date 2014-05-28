/**
 *  Project     Campos
 *  @file		track.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		tracking of light point
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

/* Includes -----------------------------------------------------------------*/
#include "track.h"
#include "camera.h"



extern int size_x, size_y;
extern DCMI_HandleTypeDef hdcmi_eval;

/* global variables ---------------------------------------------------------*/
int position_x = 0; 	// position in sensor pixels
int position_y = 0; 	// position in sensor pixels
int position_intx = 0; 	// position in sensor pixels in a 120x120pixel frame
int position_inty = 0; 	// position in sensor pixels in a 120x120pixel frame
int position_subx = 0; 	// sub pixel position from 0..999
int position_suby = 0; 	// sub pixel position from 0..999
int intensity = 0; 		// intensity (integral of all pixel values)
Track_StatusTypeDef track_status = TRACK_INIT; // Status of tracking

/* local variables ----------------------------------------------------------*/
uint16_t intensity_x[32];
uint16_t intensity_y[32];
int lost_cnt=0;

int maxx,maxy,max;

/**
 * @brief  Initialize the module
 * @param  None
 * @retval None
 */
void TRACK_Init(void) {
	track_status = TRACK_INIT;
}

/**
 * @brief  Search for the light point
 * @param  None
 * @retval None
 */
void TRACK_Search(void) {

	int integral;
	int integral_l,integral_m,integral_r;
	int x, y, v;

	if (track_status == TRACK_INIT) {

		// Reset position to 0
		maxx = 0;
		maxy = 0;
		max  = 0;
		position_x = 0;
		position_y = 0;
		position_subx = 0;
		position_suby = 0;
		intensity = 0;
		lost_cnt = 0;

		// Zoom out to search the whole camera area
		BSP_CAMERA_SetSize(CAMERA_TOTAL);

		// Start searching for some light
		track_status = TRACK_SEARCHING;

	} else if (track_status == TRACK_SEARCHING) {
		for (y = 0; y < 108; y++) {
			for (x = 0; x < 864; x++) {
				if (pixels[y][x] > max) {
					maxx = x + window_x*864;
					maxy = y + window_y*108;;
					max = pixels[y][x];
				}
			}
		}

		// A light point was found when the complete camera field was scanned
		if ((window_x == 2) && (window_y == 17)) {
			if (max > 128) {
				position_x = maxx;
				position_y = maxy;
				intensity  = max;

				// Zoom in to search the whole camera area
				BSP_CAMERA_SetSize(CAMERA_ZOOMED);
				x = position_x / 60 * 60 - 30;
				if (x<0)
					x = 0;
				if (x > (2592-120))
					x = (2592-120);
				y = position_y / 60 * 60 - 30;
				if (y<0)
					y = 0;
				if (y > (1944-120))
					y = (1944-120);

				BSP_CAMERA_SetOffset(x,y);
//				offset_x = x;
//				offset_y = y;
//				HAL_DCMI_ConfigCROP(&hdcmi_eval, offset_x, offset_y, size_x - 1, size_y - 1);
				track_status = TRACK_LIGHT_FOUND;
			}
		}


	} else {
		integral = 0;

		// Calculate the relative position
		position_intx =  offset_window_x - position_x;
		position_inty =  offset_window_y - position_y;

		if (position_inty>16 && position_inty<(120-16)
				&& position_intx>16 && position_intx<(120-16)) {

			for (x = 0; x < 32; x++) {
				intensity_x[x] = 0;
				for (y = 0; y < 32; y++) {
					v = pixels[position_inty+y-16][position_intx+x-16];
					if (v > 40) {
						intensity_x[x] += v;
						integral += v;
					}
				}
			}
			integral_l = 0;
			maxx = 16;
			for (x = 0; x < 32; x++) {
				if (integral_l < (integral/2)) {
					integral_l += intensity_x[x];
					maxx = x;
				}
			}
			maxx-=16;
			if ((maxx >-8) && (maxx <8)) {
				position_intx += maxx;
				integral_m = intensity_x[maxx+16];
				integral_r = integral - integral_l;
				integral_l = integral_l - integral_m;

				if (integral_m>0) {
					position_subx = (1000*(integral_m - (integral_l-integral_r))/2)/integral_m;
				}
				else
					position_subx = 0;
			}





			integral = 0;
			for (y = 0; y < 32; y++) {
				intensity_y[y] = 0;
				for (x = 0; x < 32; x++) {
					v = pixels[position_inty+y-16][position_intx+x-16];
					if (v > 40) {
						intensity_y[y] += v;
						integral += v;
					}
				}
			}
			integral_l = 0;
			maxy = 16;
			for (y = 0; y < 32; y++) {
				if ((integral_l*2) < integral) {
					integral_l += intensity_y[y];
					maxy = y;
				}
			}

			maxy-=16;
			if ((maxy >-8) && (maxy <8)) {
				position_inty += maxy;
				integral_m = intensity_y[maxy+16];
				integral_r = integral - integral_l;
				integral_l = integral_l - integral_m;

				if (integral_m>0) {
					position_suby = (1000*(integral_m - (integral_l-integral_r))/2)/integral_m;
				}
				else
					position_suby = 0;
			}

		}
		intensity = integral;
		if (intensity < 100 ) {
			lost_cnt ++;
			if (lost_cnt>100) {
				track_status = TRACK_INIT;
			}
		} else {
			lost_cnt = 0;
		}

	}
}

