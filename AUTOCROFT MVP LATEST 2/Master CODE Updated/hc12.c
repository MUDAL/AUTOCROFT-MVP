#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
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
	//USART1 configuration
	USART_Init(USART1,
						 BAUD_9600,
						 TX_RX_DMA_DISABLE,
						 (USART_TX_ENABLE | USART_RX_ENABLE));
}
	
void HC12_TransmitBytes(uint8_t* bytes, uint8_t len)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	USART_TransmitBytes(USART1,bytes,len);
}

bool HC12_Rx_BufferFull(void)
{
	return USART_RxBufferFull(USART1);
}

uint8_t HC12_ReadByte(void)
{
	return USART_ReadByte(USART1);
}

