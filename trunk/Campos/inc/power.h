/**
 *  Project     Campos
 *  @file		power.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for power.c
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
#ifndef __POWER_H
#define __POWER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"


/* Exported variables -------------------------------------------------------*/
extern int battery;
extern int batteryFilt;

/* Defines -------------------------------------------------------*/
#define POWER_GPIO_CLK_ENABLE()  __GPIOD_CLK_ENABLE()
#define POWER_HOLD_PIN           GPIO_PIN_2
#define POWER_KEY_PIN            GPIO_PIN_4
#define POWER_GPIO_PORT          GPIOD

/* User can use this section to tailor ADCx instance used and associated
   resources */
/* Definition for ADCx clock resources */
#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __ADC1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()               __DMA2_CLK_ENABLE()
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __GPIOA_CLK_ENABLE()

#define ADCx_FORCE_RESET()              __ADC_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __ADC_RELEASE_RESET()

/* Definition for ADCx Channel Pin */
#define ADCx_CHANNEL_PIN                GPIO_PIN_1
#define ADCx_CHANNEL_GPIO_PORT          GPIOA

/* Definition for ADCx's Channel */
#define ADCx_CHANNEL                    ADC_CHANNEL_1

/* Definition for ADCx's DMA */
#define ADCx_DMA_CHANNEL                DMA_CHANNEL_0
#define ADCx_DMA_STREAM                 DMA2_Stream0

/* Definition for ADCx's NVIC */
#define ADCx_DMA_IRQn                   DMA2_Stream0_IRQn
#define ADCx_DMA_IRQHandler             DMA2_Stream0_IRQHandler



/* Function Prototypes --------------------------------------------------------*/
void POWER_Init(void);
void POWER_Task(void);

#endif /* __POWER_H */
