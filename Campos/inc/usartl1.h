/**
 *  Project     Campos
 *  @file		usartl1.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for usartl1.c
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
#ifndef USARTL1_H_
#define USARTL1_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"
#include "usartl2.h"

/* Typedefs ------------------------------------------------------------------*/
typedef struct {
	uint16_t pause;
	uint16_t data;
} rxType;

/* Global variables  ---------------------------------------------------------*/
extern UART_HandleTypeDef UartHandle;

/* Defines -------------------------------------------------------------------*/
/* Definition for USARTx clock resources */
#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __USART2_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_2
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF7_USART2
#define USARTx_RX_PIN                    GPIO_PIN_3
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF7_USART2

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART2_IRQn
#define USARTx_IRQHandler                USART2_IRQHandler

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Exported functions ------------------------------------------------------- */

// Transmit buffer with read and write pointer
#define USARTL1_TX_SIZE 1024
#define USARTL1_TX_MASK (USARTL1_TX_SIZE-1)

// Receive buffer with read and write pointer
#define USARTL1_RX_SIZE 1024
#define USARTL1_RX_MASK (USARTL1_RX_SIZE-1)


/* Function Prototypes --------------------------------------------------------*/
void USARTL1_Init(void);
void USARTL1_RxBufferTask(void);
int USARTL1_RxBufferNotEmpty(void);
void USARTL1_PutByte(UART_HandleTypeDef *huart, uint8_t b);
void USARTL1_IRQHandler(UART_HandleTypeDef *huart);

#endif /* USART_H_ */
