#ifndef _UART_H
#define _UART_H

#define USART_TX_ENABLE						USART_CR1_TE
#define USART_RX_ENABLE						USART_CR1_RE

enum Baud
{
	BAUD_2400 = 0xD05,
	BAUD_9600 = 0x341,
	BAUD_115200 = 0x45
};

enum UartDma
{
	TX_DMA_DISABLE = 0,
	RX_DMA_DISABLE,
	TX_DMA_ENABLE,
	RX_DMA_ENABLE
};

//Initializes the USART of choice
extern void USART_Init(USART_TypeDef* uartPort, uint16_t baud, uint8_t dmaMode, uint8_t uartMode);
//Transmits single byte serial data
extern void USART_TransmitByte(USART_TypeDef* uartPort, char byte);
//Transmits multibyte serial data (excluding NULL) through a configured USART peripheral 
extern void USART_TransmitString(USART_TypeDef* uartPort, char* pString);

#endif //_UART_H
