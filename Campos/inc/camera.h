/**
 *  Project     Campos
 *  @file		camera.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for camera.h
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
 * This file is also based on the following file from the STM32Cube project
 *
 ******************************************************************************
 * @file    stm324x9i_eval_camera.h
 * @author  MCD Application Team
 * @version V2.0.1
 * @date    26-February-2014
 * @brief   This file contains the common defines and functions prototypes for
 *          the stm324x9i_eval_camera.c driver.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CAMERA_H
#define CAMERA_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"


/* Exported types -----------------------------------------------------------*/
typedef enum {
	CAMERA_OK = 0x00, CAMERA_ERROR = 0x01, CAMERA_TIMEOUT = 0x02

} Camera_StatusTypeDef;


typedef union  {
	uint8_t firstByte;
	uint8_t total[108][864];  // Pixel field as 864x108
	uint8_t zoomed[120][120]; // Pixel field as 120x120
} Union_PixelsType;

typedef enum {
	CAMERA_NONE, CAMERA_ZOOMED, CAMERA_TOTAL

} Camera_SizeTypeDef;

/* Exported variables -------------------------------------------------------*/
extern Union_PixelsType pixels; // Pixel field
extern uint8_t frame_flag; // is set, if a new frame was received
extern int window_x, window_y;
extern int offset_window_x, offset_window_y; // Offset of the captured window
extern int offset_x, offset_y;

/* Defines ------------------------------------------------------------------*/

#define CAMERA_ON_PIN					GPIO_PIN_0
#define CAMERA_ON_PORT					GPIOB
#define CAMERA_ON_CLK_ENABLE()			__GPIOB_CLK_ENABLE()
#define CAMERA_ON_CLK_DISABLE()			__GPIOB_CLK_DISABLE()

#define CAMERA_LED_PIN					GPIO_PIN_1
#define CAMERA_LED_PORT					GPIOB
#define CAMERA_LED_CLK_ENABLE()			__GPIOB_CLK_ENABLE()
#define CAMERA_LED_CLK_DISABLE()		__GPIOB_CLK_DISABLE()

#define CAMERA_I2C_ADDRESS               0x6C

/* Function prototypes -------------------------------------------------------*/

uint8_t BSP_CAMERA_Init();
void BSP_CAMERA_ContinuousStart(void);
void BSP_CAMERA_Suspend(void);
void BSP_CAMERA_Resume(void);
uint8_t BSP_CAMERA_Stop(void);
void BSP_CAMERA_LineEventCallback(void);
void BSP_CAMERA_VsyncEventCallback(void);
void BSP_CAMERA_FrameEventCallback(void);
void BSP_CAMERA_ErrorCallback(void);
void BSP_CAMERA_IRQHandler(void);
void BSP_CAMERA_DMA_IRQHandler(void);
void BSP_CAMERA_DebugWrite(uint16_t Reg, uint8_t Value);
uint8_t BSP_CAMERA_DebugRead(uint16_t Reg);
void BSP_CAMERA_SetOffset(int o_x, int o_y);
void BSP_CAMERA_SetSize(Camera_SizeTypeDef size);
Camera_SizeTypeDef BSP_CAMERA_GetSize(void);
#endif /* CAMERA_H */

