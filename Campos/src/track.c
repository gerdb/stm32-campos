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

/* global variables ---------------------------------------------------------*/
int position_x = 0; 	// position in sensor pixels
int position_y = 0; 	// position in sensor pixels
int position_subx = 0; 	// sub pixel position from 0..999
int position_suby = 0; 	// sub pixel position from 0..999
int intensity = 0; 		// intensity (integral of all pixel values)
Track_StatusTypeDef track_status = TRACK_SEARCHING; // Status of tracking

/* local variables ----------------------------------------------------------*/
uint16_t intensity_x[32];
uint16_t intensity_y[32];
int lost_cnt=0;


/**
 * @brief  Initialize the module
 * @param  None
 * @retval None
 */
void TRACK_Init(void) {

}

/**
 * @brief  Search for the light point
 * @param  None
 * @retval None
 */
void TRACK_Search(void) {
	int maxx,maxy,max;
	int integral;
	int integral_l,integral_m,integral_r;
	int x, y, v;

	if (track_status == TRACK_SEARCHING) {
		maxx = 0;
		maxy = 0;
		max = 0;
		for (y = 0; y < 120; y++) {
			for (x = 0; x < 120; x++) {
				if (pixels[y][x] > max) {
					maxx = x;
					maxy = y;
					max = pixels[y][x];
				}
			}
		}

		position_x = maxx;
		position_y = maxy;
		//intensity = max;
		track_status = TRACK_LIGHT_FOUND;
		lost_cnt = 0;
	} else {
		integral = 0;

		if (position_y>16 && position_y<(120-16)
				&& position_x>16 && position_x<(120-16)) {

			for (x = 0; x < 32; x++) {
				intensity_x[x] = 0;
				for (y = 0; y < 32; y++) {
					v = pixels[position_y+y-16][position_x+x-16];
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
				position_x += maxx;
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
					v = pixels[position_y+y-16][position_x+x-16];
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
				position_y += maxy;
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
			if (lost_cnt>10) {
				track_status = TRACK_SEARCHING;
				position_x = 0;
				position_subx = 0;
				position_y = 0;
				position_suby = 0;
			}
		} else {
			lost_cnt = 0;
		}

	}
}

