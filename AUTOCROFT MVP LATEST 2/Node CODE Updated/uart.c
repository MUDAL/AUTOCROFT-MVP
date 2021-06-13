#include "stm32f10x.h"                  // Device header
#include "uart.h"

/**
@brief Initializes USART peripheral.  
@param uartPort: pointer to USART_TypeDef struct which contains  
all registers for the desired USART peripheral e.g. if uartPort   
is passed an argument of USART1, the USART1 peripheral is configured and  
if passed an argument of USART2, the USART2 peripheral is configured etc.  

@param baud: configures the baud rate of the USART peripheral. If this parameter  
is passed BAUD_9600, the baud rate is set to 9600 baud.  

@param dmaMode: enables DMA mode for transmission or reception. If passed RX_DMA_ENABLE,  
the DMA is enabled for reception and if passed TX_DMA_ENABLE, the DMA is enabled for transmisson.  
If both transmitter and receiver DMA modes are required, RX_DMA_ENABLE and TX_DMA_ENABLE must be  
logically ORed.  

@param uartMode: enables USART for transmission or reception. If this parameter is passed   
USART_TX_ENABLE, the USART transmitter is enabled and if passed with USART_RX_ENABLE, the USART   
receiver is enabled. If both transmitter and receiver are required, the USART_TX_ENABLE and   
USART_RX_ENABLE must be logically ORed.  

@return None  
*/
void USART_Init(USART_TypeDef* uartPort,
								uint32_t baud,
								uint8_t dmaMode,
								uint8_t uartMode)
{
	uartPort->CR1 |= USART_CR1_UE;
	uartPort->CR1 &= ~uartMode;
	switch (dmaMode)
	{
		case TX_DMA_DISABLE:
			uartPort->CR3 &= ~USART_CR3_DMAT;
			break;
		case RX_DMA_DISABLE:
			uartPort->CR3 &= ~USART_CR3_DMAR;
			break;
		case TX_DMA_ENABLE:
			uartPort->CR3 |= USART_CR3_DMAT;
			break;
		case RX_DMA_ENABLE:
			uartPort->CR3 |= USART_CR3_DMAR;
			break;
	}
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

/**
@brief Transmits a byte of data through the Tx pin of the configured USART peripheral.  
@param uartPort: pointer to USART_TypeDef struct which contains all registers for the   
desired USART peripheral e.g. if uartPort is passed an argument of USART1, the USART1 peripheral  
is configured and if passed an argument of USART2, the USART2 peripheral is configured etc.  
@param byte: Data to be transmitted. The data to be transmitted
is a byte.
@return None
*/
void USART_TransmitByte(USART_TypeDef* uartPort, uint8_t byte)
{
	while ( (uartPort->SR & USART_SR_TXE) != USART_SR_TXE);
	uartPort->DR = byte;
}

/**
@brief Transmits multiple bytes of data through the Tx pin of the configured USART peripheral.  
@param uartPort: pointer to USART_TypeDef struct which contains all registers for the desired   
USART peripheral e.g. if uartPort is passed an argument of USART1, the USART1 peripheral  
is configured and if passed an argument of USART2, the USART2 peripheral is configured etc.  
@param bytes: pointer to data to be transmitted. The data to be transmitted is an array of bytes.    
@return None  
*/
void USART_TransmitBytes(USART_TypeDef* uartPort, uint8_t* bytes, uint8_t len)
{
	uint8_t i = 0;
	
	while (i < len)
	{
		while ( (uartPort->SR & USART_SR_TXE) != USART_SR_TXE);
		uartPort->DR = bytes[i];
		i++;
	}
}

/**
@brief Gets baudrate of a configured USART peripheral.    
@param uartPort: pointer to USART_TypeDef struct which contains all registers  
for the desired USART peripheral.  
@return 32-bit unsigned integer equal to the value of the baud rate.  
*/
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
