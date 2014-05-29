/**
 *  Project     Campos
 *  @file		irlink.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		infrared interface
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

#include "irlink.h"

/* local functions ----------------------------------------------------------*/

/* local variables ----------------------------------------------------------*/
TIM_HandleTypeDef htim3;
TIM_OC_InitTypeDef sConfigTim3;

/**
 * @brief  Initialize the module and configure PWM PB5 as PWM output with 36kHz
 * @param  None
 * @retval None
 */
void IRLINK_Init(void) {

	// Timer configuration
	htim3.Instance = TIM3;
	htim3.Init.Period = 1166-1; // = 36kHz = 42MHz / 1166
	htim3.Init.Prescaler = 1;
	htim3.Init.ClockDivision = 1;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_PWM_Init(&htim3);

	// Configure Timer 3 channel 2 as PWM output
	sConfigTim3.OCMode = TIM_OCMODE_PWM1;
	sConfigTim3.OCIdleState = TIM_OUTPUTSTATE_ENABLE;
	sConfigTim3.Pulse = 0;
	sConfigTim3.OCPolarity = TIM_OCPOLARITY_HIGH;

	// PWM Mode
	HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigTim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

}


/**
 * @brief  Outputs a 36kHz burst, or none
 * @param  value != 0 to output a burst
 * @retval None
 */
void IRLINK_Output(int value) {
	if (value != 0) {
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 1166/2);
	} else {
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 0);
	}

}

/**
 * @brief  Initializes the TIM PWM MSP.
 * @param  htim: TIM handle
 * @retval None
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// TIM3 clock enable
	__TIM3_CLK_ENABLE();

	// GPIOB clock enable
	__GPIOB_CLK_ENABLE();

	// GPIO Configuration: Pin B5 as output push-pull
	GPIO_InitStructure.Pin = GPIO_PIN_5;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}
