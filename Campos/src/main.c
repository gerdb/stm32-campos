/**
 *  Project     Campos
 *  @file		main.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Projects main file
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
#include "main.h"
#include "usartl1.h"
#include "camera.h"
#include "lcd.h"
#include "track.h"


/* function prototypes ------------------------------------------------------*/
void SystemClock_Config(void);


char txt[20]; // Temporary memory for strings

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void) {

	// Configure the system clock to 168 Mhz
	SystemClock_Config();

	// Initialize the hardware layer module
	HAL_Init();

	// Enable systick and configure 1ms tick
	HAL_SYSTICK_Config(168000000/ 1000);

	// Initialize the LCD display
	LCD_Init();

	// Configure LEDs
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
	BSP_LED_On(LED4);

	// Initialize the debug port
	USARTL2_Init();

	// Initialize the camera and start video mode
	BSP_CAMERA_Init();
	BSP_CAMERA_ContinuousStart((uint8_t *) pixels);

	// Main loop
	while (1) {

		// Debug ports
		USARTL1_RxBufferTask();

		// Update the status window on the right side of the TFT
		LCD_FocusStatusWindow();
		switch (track_status) {
		case TRACK_SEARCHING:
			LCD_Print(0, LCD_Y_TRACK_STATUS, "searching");
			break;
		case TRACK_LIGHT_FOUND:
			LCD_Print(0, LCD_Y_TRACK_STATUS, "Light");
			break;
		case TRACK_CENTER_DETECTED:
			LCD_Print(0, LCD_Y_TRACK_STATUS, "Center detected");
			break;
		}
		sprintf(txt, "%04d.%03d", position_x, position_subx);
		LCD_Print(0, LCD_Y_POSX, txt);

		sprintf(txt, "%04d.%03d", position_y, position_suby);
		LCD_Print(0, LCD_Y_POSY, txt);

		sprintf(txt, "%05d", intensity);
		LCD_Print(0, LCD_Y_INTENSITY, txt);

		// Mini window that shows the position of the actual window
		LCD_MiniWindow();

		// Search for the light
		if (frame_flag != 0) {
			frame_flag = 0;
			TRACK_Search();
		}

		// Update the LCD
		LCD_Image(&pixels[0][0]);
	}

}



