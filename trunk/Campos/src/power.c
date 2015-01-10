/**
 *  Project     Campos
 *  @file		power.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		switch on /off the system and measures the battery voltage
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

#include "power.h"

/* local variables ----------------------------------------------------------*/
// The result of the analog to digital conversion
__IO uint16_t ADCConvertedValue[1] = { 0 };
int battery = 0;
int batteryFilt = 0;
int batteryFiltL = 0;

// Debounce the power key
int power_key_cnt = 2;
int power_key = 1;
int power_key_released = 0;

// Low voltage switch off
int low_voltage_cnt = 0;

// ADC handler declaration
ADC_HandleTypeDef AdcHandle;

/**
 * @brief  Initialize the module
 * @param  None
 * @retval None
 */
void POWER_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable the GPIO_LED Clock */
	POWER_GPIO_CLK_ENABLE();

	// Configure the power hold pin
	GPIO_InitStruct.Pin = POWER_HOLD_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(POWER_GPIO_PORT, &GPIO_InitStruct);

	HAL_GPIO_WritePin(POWER_GPIO_PORT, POWER_HOLD_PIN, GPIO_PIN_SET);

	// Configure the power key pin
	GPIO_InitStruct.Pin = POWER_KEY_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

	HAL_GPIO_Init(POWER_GPIO_PORT, &GPIO_InitStruct);

	ADC_ChannelConfTypeDef sConfig;

	// Configure the ADC peripheral
	AdcHandle.Instance = ADCx;

	AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
	AdcHandle.Init.Resolution = ADC_RESOLUTION12b;
	AdcHandle.Init.ScanConvMode = DISABLE;
	AdcHandle.Init.ContinuousConvMode = ENABLE;
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;
	AdcHandle.Init.NbrOfDiscConversion = 0;
	AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	AdcHandle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
	AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.NbrOfConversion = 1;
	AdcHandle.Init.DMAContinuousRequests = ENABLE;
	AdcHandle.Init.EOCSelection = DISABLE;

	HAL_ADC_Init(&AdcHandle);

	// Configure ADC regular channel
	sConfig.Channel = ADCx_CHANNEL;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	sConfig.Offset = 0;

	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);

	// Start the conversion process and enable interrupt
	HAL_ADC_Start_DMA(&AdcHandle, (uint32_t*) ADCConvertedValue, 1);

}

/**
 * @brief  The cyclic task
 * @param  None
 * @retval None
 */
void POWER_Task(void) {
	// Scale the ADC result to mVs
	// Resistor divider is 3:1
	// Reference voltage: 3.0V = 3000mV
	// ADC resolution 12bit = 0..4095
	// Voltage = ADC / 4096 * 3000mV * 3
	// Voltage = ADC * 1125 / 512
	battery = ADCConvertedValue[0] * 1125 / 512;


	// Debounce the key
	if (HAL_GPIO_ReadPin(POWER_GPIO_PORT, POWER_KEY_PIN)) {
		if (power_key_cnt < 2) {
			power_key_cnt ++;
		}
		else {
			power_key = 1;
		}
	} else {
		if (power_key_cnt > 0) {
			power_key_cnt --;
		}
		else {
			power_key = 0;
		}
	}

	// Key was released
	if (power_key==0) {
		power_key_released = 1;
	}

	// Low voltage under 5.5Volt
	if (battery < 5500) {
		low_voltage_cnt ++;
	}
	else {
		low_voltage_cnt = 0;
	}


	// Switch off by key
	if (power_key_released && (power_key == 1)) {
		HAL_GPIO_WritePin(POWER_GPIO_PORT, POWER_HOLD_PIN, GPIO_PIN_RESET);
	}

	// Switch off by low voltage
	if (low_voltage_cnt > 100) {
		HAL_GPIO_WritePin(POWER_GPIO_PORT, POWER_HOLD_PIN, GPIO_PIN_RESET);
	}

	// Init the filter
	if (batteryFilt == 0) {
		batteryFilt = battery;
		batteryFiltL = batteryFilt * 256;
	}

	// Filter the battery voltage
	batteryFiltL += battery - batteryFilt;
	batteryFilt = batteryFiltL / 256;
}

/**
 * @brief ADC MSP Initialization
 *        This function configures the hardware resources used in this example:
 *           - Peripheral's clock enable
 *           - Peripheral's GPIO Configuration
 * @param huart: UART handle pointer
 * @retval None
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
	GPIO_InitTypeDef GPIO_InitStruct;
	static DMA_HandleTypeDef hdma_adc;

	// Enable peripherals and GPIO Clocks
	// Enable GPIO clock
	ADCx_CHANNEL_GPIO_CLK_ENABLE();
	// ADC1 Periph clock enable
	ADCx_CLK_ENABLE();
	// Enable DMA2 clock
	DMAx_CLK_ENABLE();

	// Configure peripheral GPIO
	// ADC1 Channel1 GPIO pin configuration
	GPIO_InitStruct.Pin = ADCx_CHANNEL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);

	// Configure the DMA streams
	// Set the parameters to be configured
	hdma_adc.Instance = ADCx_DMA_STREAM;

	hdma_adc.Init.Channel = ADCx_DMA_CHANNEL;
	hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
	hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_adc.Init.Mode = DMA_CIRCULAR;
	hdma_adc.Init.Priority = DMA_PRIORITY_HIGH;
	hdma_adc.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	hdma_adc.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
	hdma_adc.Init.MemBurst = DMA_MBURST_SINGLE;
	hdma_adc.Init.PeriphBurst = DMA_PBURST_SINGLE;

	HAL_DMA_Init(&hdma_adc);

	// Associate the initialized DMA handle to the the ADC handle
	__HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);

	// Configure the NVIC for DMA
	// NVIC configuration for DMA transfer complete interrupt
	//HAL_NVIC_SetPriority(ADCx_DMA_IRQn, 0, 0);
	//HAL_NVIC_EnableIRQ(ADCx_DMA_IRQn);
}

/**
 * @brief ADC MSP De-Initialization
 *        This function frees the hardware resources used in this example:
 *          - Disable the Peripheral's clock
 *          - Revert GPIO to their default state
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc) {
	static DMA_HandleTypeDef hdma_adc;

	// Reset peripherals
	ADCx_FORCE_RESET();
	ADCx_RELEASE_RESET();

	// Disable peripherals and GPIO Clocks
	// De-initialize the ADC1 Channel1 GPIO pin
	HAL_GPIO_DeInit(ADCx_CHANNEL_GPIO_PORT, ADCx_CHANNEL_PIN);

	// Disable the DMA Streams
	// De-Initialize the DMA Stream associate to transmission process
	HAL_DMA_DeInit(&hdma_adc);

	// Disable the NVIC for DMA
	HAL_NVIC_DisableIRQ(ADCx_DMA_IRQn);
}

