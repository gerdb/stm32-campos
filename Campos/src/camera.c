/**
 *  Project     Campos
 *  @file		camera.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Camera interface specific funtionality
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

/**
 * This file is also based on the following file from the STM32Cube project
 *
 ******************************************************************************
 * @file    stm324x9i_eval_camera.c
 * @author  MCD Application Team
 * @version V2.0.1
 * @date    26-February-2014
 * @brief   This file includes the driver for Camera modules mounted on
 *          STM324x9I-EVAL evaluation board.
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

/* Includes ------------------------------------------------------------------*/
#include "camera.h"
#include "ov5647.h"

uint8_t pixels[864][120]; // Pixel field
uint8_t frame_flag = 0; // is set, if a new frame was received

DCMI_HandleTypeDef hdcmi_eval;

/* Prototypes of local functions ---------------------------------------------*/
static void DCMI_MspInit(void);
static uint32_t GetSize(void);

/**
 * @brief  Initializes the camera.
 * @param  Camera: Pointer to the camera configuration structure
 * @retval Camera status
 */
uint8_t BSP_CAMERA_Init() {
	DCMI_HandleTypeDef *phdcmi;

	uint8_t ret = CAMERA_ERROR;

	/* Configure IO functionalities for CAMERA detect pin */
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Enable the 2 Pins to switch on the camera */
	CAMERA_ON_CLK_ENABLE();
	CAMERA_LED_CLK_ENABLE();

	/* Configure the GPIO_LED pin */
	GPIO_InitStruct.Pin = CAMERA_LED_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(CAMERA_LED_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = CAMERA_ON_PIN;
	HAL_GPIO_Init(CAMERA_ON_PORT, &GPIO_InitStruct);

	HAL_GPIO_WritePin(CAMERA_LED_PORT, CAMERA_LED_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CAMERA_ON_PORT, CAMERA_ON_PIN, GPIO_PIN_SET);

	/* Get the DCMI handle structure */
	phdcmi = &hdcmi_eval;

	/*** Configures the DCMI to interface with the camera module ***/
	/* DCMI configuration */
	phdcmi->Init.CaptureRate = DCMI_CR_ALL_FRAME;
	phdcmi->Init.HSPolarity = DCMI_HSPOLARITY_LOW;
	phdcmi->Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
	phdcmi->Init.VSPolarity = DCMI_VSPOLARITY_HIGH;
	phdcmi->Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
	phdcmi->Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
	phdcmi->Instance = DCMI;

	// wait until camera is powered up
	CAMERA_Delay(100);

	/* DCMI Initialization */
	DCMI_MspInit();
	HAL_DCMI_Init(phdcmi);
	HAL_DCMI_EnableCROP(phdcmi);
	HAL_DCMI_ConfigCROP(phdcmi, 0, 0, 120 - 1, 120 - 1);

	if (ov5647_ReadID(CAMERA_I2C_ADDRESS) == OV5647_ID) {
		/* Camera Init */
		ov5647_Init(CAMERA_I2C_ADDRESS);

		/* Return CAMERA_OK status */
		ret = CAMERA_OK;
	}

	return ret;
}

/**
 * @brief  Starts the camera capture in continuous mode.
 * @param  buff: pointer to the camera output buffer
 * @retval None
 */
void BSP_CAMERA_ContinuousStart(uint8_t *buff) {
	/* Start the camera capture */
	HAL_DCMI_Start_DMA(&hdcmi_eval, DCMI_MODE_CONTINUOUS, (uint32_t) buff,
			GetSize());
}

/**
 * @brief  Starts the camera capture in snapshot mode.
 * @param  buff: pointer to the camera output buffer
 * @retval None
 */
void BSP_CAMERA_SnapshotStart(uint8_t *buff) {
	/* Start the camera capture */
	HAL_DCMI_Start_DMA(&hdcmi_eval, DCMI_MODE_SNAPSHOT, (uint32_t) buff,
			GetSize());
}

/**
 * @brief Suspend the CAMERA capture
 * @param  None
 * @retval None
 */
void BSP_CAMERA_Suspend(void) {
	/* Disable the DMA */
	__HAL_DMA_DISABLE(hdcmi_eval.DMA_Handle);
	/* Disable the DCMI */
	__HAL_DCMI_DISABLE(&hdcmi_eval);

}

/**
 * @brief Resume the CAMERA capture
 * @param  None
 * @retval None
 */
void BSP_CAMERA_Resume(void) {
	/* Enable the DCMI */
	__HAL_DCMI_ENABLE(&hdcmi_eval);
	/* Enable the DMA */
	__HAL_DMA_ENABLE(hdcmi_eval.DMA_Handle);
}

/**
 * @brief  Stop the CAMERA capture
 * @param  None
 * @retval Camera status
 */
uint8_t BSP_CAMERA_Stop(void) {
	DCMI_HandleTypeDef *phdcmi;

	uint8_t ret = CAMERA_ERROR;

	/* Get the DCMI handle structure */
	phdcmi = &hdcmi_eval;

	if (HAL_DCMI_Stop(phdcmi) == HAL_OK) {
		ret = CAMERA_OK;
	}

	return ret;
}

void BSP_CAMERA_DebugWrite(uint16_t Reg, uint8_t Value) {
	CAMERA_IO_Write(CAMERA_I2C_ADDRESS, Reg, Value);
}

uint8_t BSP_CAMERA_DebugRead(uint16_t Reg) {
	return CAMERA_IO_Read(CAMERA_I2C_ADDRESS, Reg);
}

/**
 * @brief  Handles DCMI interrupt request.
 * @param  None
 * @retval None
 */
void BSP_CAMERA_IRQHandler(void) {
	HAL_DCMI_IRQHandler(&hdcmi_eval);
}

/**
 * @brief  Handles DMA interrupt request.
 * @param  None
 * @retval None
 */
void BSP_CAMERA_DMA_IRQHandler(void) {
	HAL_DMA_IRQHandler(hdcmi_eval.DMA_Handle);
}

/**
 * @brief  Get the capture size.
 * @param  current_resolution: the current resolution.
 * @retval cpature size.
 */
static uint32_t GetSize(void) {

	// 120x120
	return 120 * 120 / 4;
}

/**
 * @brief  Initializes the DCMI MSP.
 * @param  None
 * @retval None
 */
static void DCMI_MspInit(void) {
	static DMA_HandleTypeDef hdma_eval;
	GPIO_InitTypeDef GPIO_Init_Structure;
	DCMI_HandleTypeDef *hdcmi = &hdcmi_eval;

	// Enable DCMI clock
	__DCMI_CLK_ENABLE();

	// Enable DMA2 clock
	__DMA2_CLK_ENABLE();

	// Enable GPIO clocks
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();

	/** Configure DCMI GPIO as alternate function
	 DCMI_D0 PC6
	 DCMI_D1 PC7
	 DCMI_D2 PC8
	 DCMI_D3 PC9
	 DCMI_D4 PC11
	 DCMI_D5 PB6
	 DCMI_D6 PB8
	 DCMI_D7 PB9
	 DCMI_VSYNC PB7
	 DCMI_HSYNC PA4
	 DCMI_PXCLK PA6
	 */

	GPIO_Init_Structure.Pin = GPIO_PIN_4 | GPIO_PIN_6;
	GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
	GPIO_Init_Structure.Pull = GPIO_PULLUP;
	GPIO_Init_Structure.Speed = GPIO_SPEED_HIGH;
	GPIO_Init_Structure.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(GPIOA, &GPIO_Init_Structure);

	GPIO_Init_Structure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
	GPIO_Init_Structure.Pull = GPIO_PULLUP;
	GPIO_Init_Structure.Speed = GPIO_SPEED_HIGH;
	GPIO_Init_Structure.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(GPIOB, &GPIO_Init_Structure);

	GPIO_Init_Structure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9
			| GPIO_PIN_11;
	GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
	GPIO_Init_Structure.Pull = GPIO_PULLUP;
	GPIO_Init_Structure.Speed = GPIO_SPEED_HIGH;
	GPIO_Init_Structure.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(GPIOC, &GPIO_Init_Structure);

	/*** Configure the DMA ***/
	/* Set the parameters to be configured */
	hdma_eval.Init.Channel = DMA_CHANNEL_1;
	hdma_eval.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_eval.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_eval.Init.MemInc = DMA_MINC_ENABLE;
	hdma_eval.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_eval.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_eval.Init.Mode = DMA_CIRCULAR;
	hdma_eval.Init.Priority = DMA_PRIORITY_HIGH;
	hdma_eval.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	hdma_eval.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	hdma_eval.Init.MemBurst = DMA_MBURST_SINGLE;
	hdma_eval.Init.PeriphBurst = DMA_PBURST_SINGLE;

	hdma_eval.Instance = DMA2_Stream1;

	/* Associate the initialized DMA handle to the DCMI handle */
	__HAL_LINKDMA(hdcmi, DMA_Handle, hdma_eval);

	/*** Configure the NVIC for DCMI and DMA ***/
	/* NVIC configuration for DCMI transfer complete interrupt */
	HAL_NVIC_SetPriority(DCMI_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DCMI_IRQn);

	/* NVIC configuration for DMA2D transfer complete interrupt */
	HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

	/* Configure the DMA stream */
	HAL_DMA_Init(hdcmi->DMA_Handle);
}

/**
 * @brief  Line event callback
 * @param  hdcmi: pointer to the DCMI handle
 * @retval None
 */
void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi) {
	BSP_CAMERA_LineEventCallback();
}

/**
 * @brief  Line Event callback.
 * @param  None
 * @retval None
 */
__weak void BSP_CAMERA_LineEventCallback(void) {
	/* NOTE : This function Should not be modified, when the callback is needed,
	 the HAL_DCMI_LineEventCallback could be implemented in the user file
	 */
}

/**
 * @brief  VSYNC event callback
 * @param  hdcmi: pointer to the DCMI handle
 * @retval None
 */
void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi) {
	BSP_CAMERA_VsyncEventCallback();
}

/**
 * @brief  VSYNC Event callback.
 * @param  None
 * @retval None
 */
__weak void BSP_CAMERA_VsyncEventCallback(void) {
	/* NOTE : This function Should not be modified, when the callback is needed,
	 the HAL_DCMI_VsyncEventCallback could be implemented in the user file
	 */
}

/**
 * @brief  Frame event callback
 * @param  hdcmi: pointer to the DCMI handle
 * @retval None
 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi) {
	//BSP_CAMERA_FrameEventCallback();
	frame_flag = 1;
}

/**
 * @brief  Frame Event callback.
 * @param  None
 * @retval None
 */
__weak void BSP_CAMERA_FrameEventCallback(void) {
	/* NOTE : This function Should not be modified, when the callback is needed,
	 the HAL_DCMI_FrameEventCallback could be implemented in the user file
	 */
}

/**
 * @brief  Error callback
 * @param  hdcmi: pointer to the DCMI handle
 * @retval None
 */
void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi) {
	BSP_CAMERA_ErrorCallback();
}

/**
 * @brief  Error callback.
 * @param  None
 * @retval None
 */
__weak void BSP_CAMERA_ErrorCallback(void) {
	/* NOTE : This function Should not be modified, when the callback is needed,
	 the HAL_DCMI_ErrorCallback could be implemented in the user file
	 */
}

