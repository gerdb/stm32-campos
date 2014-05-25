/**
 *  Project     Campos
 *  @file		usartl1.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		communication layer 1
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
#include "usartl1.h"
#include "main.h"

/* local functions ----------------------------------------------------------*/
static void USARTL1_Transmit(UART_HandleTypeDef *huart);
static void USARTL1_Receive(UART_HandleTypeDef *huart);

/* local variables ----------------------------------------------------------*/
UART_HandleTypeDef UartHandle;

// Transmit buffer with read and write pointer
uint8_t USARTL1_tx_buffer[USARTL1_TX_SIZE];
uint8_t USARTL1_txen = 0;
uint16_t USARTL1_tx_wr_pointer = 0;
uint16_t USARTL1_tx_rd_pointer = 0;

// Receive buffer with read and write pointer
uint8_t USARTL1_rx_buffer[USARTL1_RX_SIZE];
int USARTL1_rx_wr_pointer = 0;
int USARTL1_rx_rd_pointer = 0;

/**
 * @brief  Initialize the USART
 * @param  None
 * @retval None
 */
void USARTL1_Init(void) {

	USARTL1_rx_wr_pointer = 0;
	USARTL1_rx_rd_pointer = 0;
	USARTL1_txen = 0;

	UartHandle.Instance = USARTx;
	UartHandle.Init.BaudRate = 115200;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits = UART_STOPBITS_1;
	UartHandle.Init.Parity = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&UartHandle);
	HAL_UART_Receive_IT(&UartHandle, (uint8_t *) USARTL1_rx_buffer,
			USARTL1_RX_SIZE);
}

/**
 * @brief  Check, if there is something in the receive buffer to decode
 * @param  None
 * @retval None
 */
void USARTL1_RxBufferTask(void) {

	char c;
	//Increment the read pointer of the RX buffer
	if (USARTL1_RxBufferNotEmpty()) {
		USARTL1_rx_rd_pointer++;
		USARTL1_rx_rd_pointer &= USARTL1_RX_MASK;
		c = USARTL1_rx_buffer[USARTL1_rx_rd_pointer];
		// echo
		if (c != 27)
			USARTL1_PutByte(&UartHandle, c);
		//Decode the received byte
		USARTL2_Decode(c);
	}
}

/**
 * @brief  Returns, whether the receive buffer is empty or not
 *
 * @param  None
 * @return =0: there was no byte in the receive buffer
 *         >0: there was at least one byte in the receive buffer.
 */
int USARTL1_RxBufferNotEmpty(void) {

	return USARTL1_rx_wr_pointer != USARTL1_rx_rd_pointer;
}

/**
 * @brief  Write a new character into the transmit buffer
 *         This function is called by the printf function
 *
 * @param  huart handle to uart driver
 * @param  b character to send
 */
void USARTL1_PutByte(UART_HandleTypeDef *huart, uint8_t b) {

	// Send the byte
	if (!USARTL1_txen) {
		// Write the first byte directly into the USART
		USARTL1_txen = 1;

		huart->Instance->DR = b;
		__HAL_UART_ENABLE_IT(huart, UART_IT_TXE);
	} else {
		// Write the next character into the buffer
		while (((USARTL1_tx_wr_pointer + 1) & USARTL1_TX_MASK)
				== USARTL1_tx_rd_pointer)
			;
		USARTL1_tx_wr_pointer++;
		USARTL1_tx_wr_pointer &= USARTL1_TX_MASK;
		if (USARTL1_tx_wr_pointer == USARTL1_tx_rd_pointer) {
			USARTL1_tx_wr_pointer--;
			USARTL1_tx_wr_pointer &= USARTL1_TX_MASK;
		}

		USARTL1_tx_buffer[USARTL1_tx_wr_pointer] = b;
	}
}

/**
 * @brief  This function handles UART interrupt request.
 * @param  huart: UART handle
 * @retval None
 */
void USARTL1_IRQHandler(UART_HandleTypeDef *huart) {
	uint32_t tmp1 = 0, tmp2 = 0;

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);
	/* UART parity error interrupt occurred ------------------------------------*/
	if ((tmp1 != RESET) && (tmp2 != RESET)) {
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_PE);

		huart->ErrorCode |= HAL_UART_ERROR_PE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART frame error interrupt occurred -------------------------------------*/
	if ((tmp1 != RESET) && (tmp2 != RESET)) {
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_FE);

		huart->ErrorCode |= HAL_UART_ERROR_FE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART noise error interrupt occurred -------------------------------------*/
	if ((tmp1 != RESET) && (tmp2 != RESET)) {
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_NE);

		huart->ErrorCode |= HAL_UART_ERROR_NE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART Over-Run interrupt occurred ----------------------------------------*/
	if ((tmp1 != RESET) && (tmp2 != RESET)) {
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);

		huart->ErrorCode |= HAL_UART_ERROR_ORE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
	/* UART in mode Receiver ---------------------------------------------------*/
	if ((tmp1 != RESET) && (tmp2 != RESET)) {
		USARTL1_Receive(huart);
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_RXNE);
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_TXE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TXE);
	/* UART in mode Transmitter ------------------------------------------------*/
	if ((tmp1 != RESET) && (tmp2 != RESET)) {
		USARTL1_Transmit(huart);
		__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_TXE);
	}

	if (huart->ErrorCode != HAL_UART_ERROR_NONE) {
		/* Set the UART state ready to be able to start again the process */
		huart->State = HAL_UART_STATE_READY;

		HAL_UART_ErrorCallback(huart);
	}
}

/**
 * @brief  Transmit an amount of data in non blocking mode
 * @param  huart: UART handle
 * @retval None
 */
static void USARTL1_Transmit(UART_HandleTypeDef *huart) {
	//Increment the read pointer of the TX buffer
	if (USARTL1_tx_wr_pointer != USARTL1_tx_rd_pointer) {
		USARTL1_tx_rd_pointer++;
		USARTL1_tx_rd_pointer &= USARTL1_TX_MASK;

		//send the next byte
		huart->Instance->DR = USARTL1_tx_buffer[USARTL1_tx_rd_pointer];
	} else {
		// This was the last byte to send, disable the transmission.
		USARTL1_txen = 0;
		/* Disable the UART Transmit Complete Interrupt */
		__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
	}
}

/**
 * @brief  Receives an amount of data in non blocking mode
 * @param  huart: UART handle
 * @retval HAL status
 */
static void USARTL1_Receive(UART_HandleTypeDef *huart) {
	uint8_t c;

	// Increment the buffer pointer, if it's possible
	USARTL1_rx_wr_pointer++;
	USARTL1_rx_wr_pointer &= USARTL1_RX_MASK;
	if (USARTL1_rx_wr_pointer == USARTL1_rx_rd_pointer) {
		USARTL1_rx_wr_pointer--;
		USARTL1_rx_wr_pointer &= USARTL1_RX_MASK;
	}

	// Write the received byte
	c = (uint8_t) (huart->Instance->DR & (uint8_t) 0x00FF);
	USARTL1_rx_buffer[USARTL1_rx_wr_pointer] = c;
}

/**
 * @brief  UART error callbacks
 * @param  UartHandle: UART handle
 * @note   This example shows a simple way to report transfer error, and you can
 *         add your own implementation.
 * @retval None
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle) {
	/* Turn LED3 on: Transfer error in reception/transmission process */
	BSP_LED_On(LED3);
}

/**
 * @brief  Tx Transfer completed callback
 * @param  UartHandle: UART handle.
 * @note   This example shows a simple way to report end of IT Tx transfer, and
 *         you can add your own implementation.
 * @retval None
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle) {
	/* Set transmission flag: trasfer complete*/

	/* Turn LED6 on: Transfer in transmission process is correct */
	BSP_LED_On(LED6);
}
