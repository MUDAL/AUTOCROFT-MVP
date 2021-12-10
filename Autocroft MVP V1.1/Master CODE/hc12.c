#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "dma.h"
#include "uart.h"
#include "hc12.h"

void HC12_Init(void)
{
	//GPIO configuration for USART1 Tx
	GPIO_OutputInit(GPIOA,
									GPIO_PORT_REG_HIGH,
									GPIO_PIN9_OUTPUT_MODE_2MHZ,
									GPIO_PIN9_ALT_FUNC_PUSH_PULL);
	//GPIO configuration for USART1 Rx
	GPIO_InputInit(GPIOA,
								 GPIO_PORT_REG_HIGH,
								 GPIO_PIN10,
								 GPIO_PIN10_INPUT_PULLUP_OR_PULLDOWN,
								 GPIO_PULLUP_ENABLE);
	//GPIO configuration for 'SET' pin of HC12
	GPIO_OutputInit(GPIOB,
									GPIO_PORT_REG_LOW,
									GPIO_PIN0_OUTPUT_MODE_2MHZ,
									GPIO_PIN0_GEN_PUR_OPEN_DRAIN);
	//Leave SET pin at a high state
	GPIO_OutputWrite(GPIOB,GPIO_PIN0,true);
	//Usart configuration
	USART_Init(USART1,
						 BAUD_9600,
						 RX_DMA_ENABLE,
						 (USART_TX_ENABLE | USART_RX_ENABLE));
}

void HC12_RxBufferInit(uint8_t* pBuffer, uint8_t bufferSize)
{
	//DMA1 channel 5 configuration for USART1 Rx
	DMA_USART_Rx_Init(DMA1_Channel5,
										USART1,
										pBuffer,
										bufferSize, 
										DMA_CHANNEL5_MEMORY_INC_MODE |
										DMA_CHANNEL5_CIRCULAR_BUFFER |
										DMA_CHANNEL_ENABLE);
}

void HC12_TransmitByte(uint8_t data)
{
	USART_TransmitByte(USART1,data);
}

void HC12_TransmitBytes(uint8_t* bytes, uint8_t len)
{
	USART_TransmitBytes(USART1,bytes,len);
}

bool HC12_RxBufferFull(void)
{
	return DMA_Rx_BufferFull(DMA1, DMA_CHANNEL5);
}

/**
@brief the SET pin either puts the HC12 module into command mode or  
releases it from command mode.  
*/
void HC12_SetPinControl(bool pinState)
{
	GPIO_OutputWrite(GPIOB,GPIO_PIN0,pinState);
}
