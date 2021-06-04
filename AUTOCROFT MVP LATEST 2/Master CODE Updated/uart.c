#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "uart.h"

void USART_Init(USART_TypeDef* uartPort, uint32_t baud, uint8_t uartMode)
{
	/*
	Description:
	Initializes USART peripheral
	
	Parameters:
	1.) uartPort: pointer to USART_TypeDef struct which contains
	all registers for the desired USART peripheral
	e.g. if uartPort is passed an argument of USART1, the USART1 peripheral
	is configured and if passed an argument of USART2, the USART2 peripheral
	is configured etc.
	
	2.) baud: configures the baud rate of the USART peripheral. If this parameter
	is passed BAUD_9600, the baud rate is set to 9600 baud. 
	
	3.) uartMode: enables USART for transmission or reception. If this
	parameter is passed USART_TX_ENABLE, the USART transmitter is
	enabled and if passed with USART_RX_ENABLE, the USART receiver is 
	enabled. If both transmitter and receiver are required, the USART_TX_ENABLE
	and USART_RX_ENABLE must be logically ORed.
	
	Return:
	None
	
	*/
	uartPort->CR1 |= USART_CR1_UE;
	uartPort->CR1 &= ~uartMode;
	switch (baud)
	{
		case BAUD_2400:
			uartPort->BRR = BRR_2400;
			break;
		case BAUD_9600:
			uartPort->BRR = BRR_9600;
			break;
		case BAUD_115200:
			uartPort->BRR = BRR_115200;
			break;
	}
	uartPort->CR1 |= uartMode;
}

void USART_TransmitByte(USART_TypeDef* uartPort, uint8_t byte)
{
	/*
	Description:
	Transmits a byte of data through the Tx pin of
	the configured USART peripheral
	
	Parameters:
	1.) uartPort:  pointer to USART_TypeDef struct which contains
	all registers for the desired USART peripheral
	e.g. if uartPort is passed an argument of USART1, the USART1 peripheral
	is configured and if passed an argument of USART2, the USART2 peripheral
	is configured etc.
	
	2.) byte: Data to be transmitted. The data to be transmitted
	is a character or byte.
	
	Return:
	None
	
	*/
	
	while ((uartPort->SR & USART_SR_TXE) != USART_SR_TXE);
	uartPort->DR = byte;
	
}

void USART_TransmitBytes(USART_TypeDef* uartPort, uint8_t* bytes, uint8_t len)
{
	/*
	Description:
	Transmits multiple bytes of data through the Tx pin of
	the configured USART peripheral
	
	Parameters:
	1.) uartPort:  pointer to USART_TypeDef struct which contains
	all registers for the desired USART peripheral
	e.g. if uartPort is passed an argument of USART1, the USART1 peripheral
	is configured and if passed an argument of USART2, the USART2 peripheral
	is configured etc.
	
	2.) bytes: pointer to data to be transmitted. The data to be transmitted
	is an array of characters excluding the 'NULL' character.
	
	Return:
	None
	
	*/
	uint8_t i = 0;
	
	while (i < len)
	{
		while ((uartPort->SR & USART_SR_TXE) != USART_SR_TXE);
		uartPort->DR = bytes[i];
		i++;
	}
}

uint32_t USART_GetBaudRate(USART_TypeDef* uartPort)
{
	volatile uint16_t brr = uartPort->BRR;
	uint32_t baudRate = 0;
	
	switch(brr)
	{
		case BRR_2400:
			baudRate = 2400;
			break;
		case BRR_9600:
			baudRate = 9600;
			break;
		case BRR_115200:
			baudRate = 115200;
			break;
	}
	return baudRate;
}

bool USART_RxBufferFull(USART_TypeDef* uartPort)
{
	if ((uartPort->SR & USART_SR_RXNE) == USART_SR_RXNE)
	{
		return true;
	}
	return false;
}

uint8_t USART_ReadByte(USART_TypeDef* uartPort)
{
	return uartPort->DR;
}
