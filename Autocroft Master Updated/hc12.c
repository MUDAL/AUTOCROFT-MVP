#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "uart.h"
#include "dma.h"
#include "hc12.h"

void HC12_TxInit(void)
{
	
	//GPIO configuration for USART1 Tx
	GPIO_Output_Init(GPIOA,
									 GPIO_PORT_REG_HIGH,
									 GPIO_PIN9_OUTPUT_MODE_2MHZ,
									 GPIO_PIN9_ALT_FUNC_PUSH_PULL);
	
	//USART1 configuration
	USART_Init(USART1,
						 BAUD_9600,
						 USART_TX_DMA_DISABLE,
						 USART_TX_ENABLE);
	
}

void HC12_Rx_BufferInit(char* pBuffer, uint32_t bufferSize)
{
	//GPIO configuration for USART1 Rx
	GPIO_Input_Init(GPIOA,
									GPIO_PORT_REG_HIGH,
									GPIO_PIN10,
									GPIO_PIN10_INPUT_PULLUP_OR_PULLDOWN,
									GPIO_PULLUP_ENABLE);
	
	//USART1 configuration
	USART_Init(USART1,
						 BAUD_9600,
						 USART_DMA_RX_ENABLE,
						 USART_RX_ENABLE);
	
	//DMA1 channel 5 configuration for USART1 Rx
	DMA_USART_Rx_Init(DMA1_Channel5,
										USART1,
										pBuffer,
										bufferSize - 1, 
										DMA_CHANNEL5_MEMORY_INC_MODE |
										DMA_CHANNEL5_CIRCULAR_BUFFER |
										DMA_CHANNEL5_ENABLE);
}
	
void HC12_Transmit(char* pData)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	USART_Transmit_String(USART1,pData);
}

bool HC12_Rx_BufferFull(void)
{
	return DMA_Rx_BufferFull(DMA1, DMA_CHANNEL5);
}

