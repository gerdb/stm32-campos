/**
 *  Project     Campos
 *  @file		lcd.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		LCD/TFT functionality
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
#include "lcd.h"
#include "track.h"
#include "logo.h"


/* local variables -----------------------------------------------------------*/
uint16_t color_table[256];

/**
 * @brief Initialize the LCD
 * - The TFT itself
 * - fill the backgrounds
 * - create the right window
 * @param  None
 * @retval None
 */
void LCD_Init(void) {
	int x, y;

	// Initialize the TFT display
	ili9325_Init();

	// Fill the color table
	for (x = 0; x < 256; x++) {
		// Fill the color table with 256 shades of grey
		//bw[x] = (x/8)<<11 | ((x/4)<< 5) | ((x/8));

		// Fill the color table with 256 shades of green
		color_table[x] = ((x / 4) << 5);
	}

	// Fill the screen black
	for (y = 0; y < 120; y++) {
		for (x = 0; x < 120; x++) {
			pixels.zoomed[y][x] = LCD_BLACK;
		}
	}
	LCD_Clr();
	LCD_DrawInfoWindow();
}

/**
 * Redraws the right info windo
 *
 * @param none
 * @retval None
 */
void LCD_DrawInfoWindow(void) {
	int i;
	// Draw the info window on the right side
	ili9325_SetDisplayWindow(240, 0, 80, 240);
	ili9325_SetCursor(240, 0);

	// Prepare to write to the LCD ram
	LCD_IO_WriteReg(LCD_REG_34);
	for (i = 80 * 240; i != 0; i--)
		LCD_IO_WriteData(LCD_BLACK);

	// Print the header texts
	LCD_Print(35, 0, "State:", LCD_OPAQUE);
	LCD_Print(35, 3, "Position:", LCD_OPAQUE);
	LCD_Print(35, 7, "Intensity:", LCD_OPAQUE);
	LCD_Print(35, 10, "Battery:", LCD_OPAQUE);
}


/**
 * Print a string
 *
 * @param x Horizontal position in 7 pixel steps
 * @param y Vertical position in 13 pixel steps
 * @param s String to print
 * @param transparent or opaque background
 * @retval None
 */
void LCD_Print(int x, int y, char * s, int transparent) {
	int cy, cx;
	int fontdata;
	int xx;
	int ci;

	cx = x;
	// Get the next character (max 10)
	for (ci = 0; (ci < 10) && (s[ci]!=0); ci++) {
		for (cy = 0; cy < 12; cy ++) {
			// Write one character
			ili9325_SetCursor( (cx + ci) * 7, 5 + y*13+cy);
			LCD_IO_WriteReg(LCD_REG_34);
			// Get the font
			fontdata = Font12_Table[(s[ci]-' ')*12+cy];
			for (xx = 0; xx<7; xx++) {
				// Write it with white pixels on black
				if (fontdata & 0x80)
					LCD_IO_WriteData(LCD_WHITE);
				else {
					if (transparent == LCD_OPAQUE)
						LCD_IO_WriteData(LCD_BLACK);
					else {
						ili9325_SetCursor( (cx + ci) * 7 +xx, 5 + y*13+cy);
						LCD_IO_WriteReg(LCD_REG_34);
					}
				}
				fontdata <<= 1;
			}
		}
	}
}

/**
 * @brief  Set the display window to the right status field
 * @param  None
 * @retval None
 */
void LCD_FocusStatusWindow(void) {
	ili9325_SetDisplayWindow(240, 0, 80, 240);
}

/**
 * @brief  Draw the mini window with position of the active
 * 		   selected window
 * @param  None
 * @retval None
 */
void LCD_MiniWindow(Camera_SizeTypeDef cameraSize) {
	int x, y;
	int mini_x1,mini_x2,mini_y1,mini_y2;
	ili9325_SetDisplayWindow(245, 180, 72, 54);
	ili9325_SetCursor(245, 180);
	/* Prepare to write GRAM */
	LCD_IO_WriteReg(LCD_REG_34);

	mini_x1 = offset_x/36 ;
	mini_y1 = offset_y/36 ;

	if (cameraSize == CAMERA_TOTAL) {
		mini_x2 = mini_x1 + 23;
		mini_y2 = mini_y1 + 2;
	} else {
		mini_x2 = mini_x1 + 3;
		mini_y2 = mini_y1 + 3;
	}

	for (y = 0; y < 54; y++) {
		for (x = 0; x < 72; x++) {
			if (x>= mini_x1 && x<= mini_x2 && y>= mini_y1 && y<= mini_y2)
				LCD_IO_WriteData(0xFFFF);
			else
				LCD_IO_WriteData(0x03E0);
		}
	}
}


/**
 * @brief Draw the content of an pixel array to the LCD
 * Draw one logical pixel of the array as 4 (2x2) pixels
 * Show also a red cursor
 *
 * @param pixelp pointer to the pixel array
 */
void LCD_Image_Zoomed(uint8_t* pixelp) {

	int x, y;
	int v;

	int cursor_x = 120 - position_intx;
	int cursor_y = 2 * position_inty;

	//Show or hide the cursor
	if (track_status != TRACK_CENTER_DETECTED) {
		cursor_x = -1;
		cursor_y = -1;
	}

	// Define the region to draw
	ili9325_SetDisplayWindow(0, 0, 240, 240);
	ili9325_SetCursor(0, 0);

	// Prepare to write to the LCD ram
	LCD_IO_WriteReg(LCD_REG_34);
	LCD_CD_DATA();
	for (y = 0; y < 240; y++) {
		if (y == cursor_y) {

			// Draw the vertical cursor line
			for (x = 240; x != 0; x--) {

				// 2 pixels width
				LCD_IO_WRITE_1xDATA(LCD_RED);
				LCD_IO_WRITE_1xDATA(LCD_RED);
			}
			pixelp += 120;
			y++;

		} else {

			for (x = 120; x != 0; x--) {
				// Draw the camera image
				v = color_table[*pixelp];
				// Draw the horizontal cursor line
				if (x == cursor_x)
					v = LCD_RED;
				// 2 pixels width
				LCD_IO_WRITE_1xDATA(v);
				LCD_IO_WRITE_1xDATA(v);
				pixelp++;
			}
		}
		// 2 pixels height
		if (y % 2 == 0) {
			pixelp -= 120;
		}
	}
}

void LCD_Image_Total(uint8_t* pixelp) {

	int x, y;
	int v;

	// Define the region to draw
	ili9325_SetDisplayWindow(window_x*72+12, window_y*9+39, 72, 9);
	ili9325_SetCursor(window_x*72+12, window_y*9+39);

	// Prepare to write to the LCD ram
	LCD_IO_WriteReg(LCD_REG_34);
	LCD_CD_DATA();
	for (y = 0; y < 9; y++) {

		for (x = 72; x != 0; x--) {
			// Draw the camera image
			v = color_table[*pixelp];
			// 2 pixels width
			LCD_IO_WRITE_1xDATA(v);
			pixelp+=4*3;
		}
		pixelp += 864*3*3;

	}
}

/**
 * @brief Clear the whole 240x240 pixel video area and fill it with black pixels
 *
 * @param  None
 * @retval None
 */
void LCD_Clr(void) {
	int x, y;

	// Define the region to draw
	ili9325_SetDisplayWindow(0, 0, 240, 240);
	ili9325_SetCursor(0, 0);

	// Prepare to write to the LCD ram
	LCD_IO_WriteReg(LCD_REG_34);
	LCD_CD_DATA();
	for (y = 0; y < 240; y++) {
		for (x = 240; x != 0; x--) {
			LCD_IO_WRITE_1xDATA(LCD_BLACK);
		}

	}
}



/**
 * @brief Draw the startup logo
 *
 * @param  None
 * @retval None
 */
void LCD_Logo(void) {
	int i;

	// Define the region to draw
	ili9325_SetDisplayWindow(0, 0, 320, 240);
	ili9325_SetCursor(0, 0);

	// Prepare to write to the LCD ram
	LCD_IO_WriteReg(LCD_REG_34);
	LCD_CD_DATA();
	for (i = 0; i < (240*320); i++) {
		LCD_IO_WRITE_1xDATA(logo[i]);
	}
}

