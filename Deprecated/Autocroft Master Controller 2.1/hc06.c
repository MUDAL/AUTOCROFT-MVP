#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "uart.h"
#include "sysTimer_struct.h"
#include "dma.h"
#include "hc06.h"

#define HC06_DUMMY_BUFFER_SIZE	10

void HC06_Init(char* hc06RxBuffer)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	//GPIO configuration for USART1 Rx
	GPIO_Input_Init(GPIOA,
									GPIO_PORT_REG_HIGH,
									GPIO_PIN10,
									GPIO_PIN10_INPUT_PULLUP_OR_PULLDOWN,
									GPIO_PULLUP_ENABLE);
	
	//GPIO configuration for USART1 Tx
	GPIO_Output_Init(GPIOA,
									 GPIO_PORT_REG_HIGH,
									 GPIO_PIN9_OUTPUT_MODE_2MHZ,
									 GPIO_PIN9_ALT_FUNC_PUSH_PULL);
	
	//USART1 configuration
	USART_Init(USART1,
						 BAUD_9600,
						 USART_DMA_RX_ENABLE,
						(USART_TX_ENABLE | USART_RX_ENABLE));
	
	//DMA1 channel 5 configuration for USART1 Rx
	DMA_USART_Rx_Init(DMA1_Channel5,
										USART1,
										hc06RxBuffer,
										HC06_DUMMY_BUFFER_SIZE, 
										DMA_CHANNEL5_MEMORY_INC_MODE);
}
	
void HC06_Transmit(char* pData)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	USART_Transmit_String(USART1,pData);
}

bool HC06_Receiver_Data_Ready(sysTimer_t* pTimer, uint32_t numberOfBytes)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	bool dataIsReady = false;
	
	if (DMA_Receive(DMA1,
									DMA1_Channel5,
									DMA_CHANNEL5_CLEAR_TC_INT_FLAG,
									numberOfBytes,
									pTimer))
	
	{
		dataIsReady = true;
	}		
	
	return dataIsReady;
}
