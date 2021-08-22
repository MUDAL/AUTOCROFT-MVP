#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include <string.h>
#include "gpio.h"
#include "sysTimer_struct.h"
#include "dma.h"
#include "uart.h"
#include "esp8266.h"

#define ESP8266_RX_BUFFER_SIZE		10 

static char esp8266RxBuffer[ ESP8266_RX_BUFFER_SIZE ];

void ESP8266_Init(void)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	//GPIO configuration for USART2 Rx
	GPIO_Input_Init(GPIOA,
									GPIO_PORT_REG_LOW,
									GPIO_PIN3,
									GPIO_PIN3_INPUT_PULLUP_OR_PULLDOWN,
									GPIO_PULLUP_ENABLE);
	
	//GPIO configuration for USART2 Tx
	GPIO_Output_Init(GPIOA,
									 GPIO_PORT_REG_LOW,
									 GPIO_PIN2_OUTPUT_MODE_2MHZ,
									 GPIO_PIN2_ALT_FUNC_PUSH_PULL);
	
	//USART2 configuration
	USART_Init(USART2,
						 BAUD_9600,
						 USART_DMA_RX_ENABLE,
						(USART_TX_ENABLE | USART_RX_ENABLE));
	
	//DMA1 channel 6 configuration for USART2 Rx
	DMA_USART_Rx_Init(DMA1_Channel6,
										USART2,
										esp8266RxBuffer,
										ESP8266_RX_BUFFER_SIZE - 1, 
									 (DMA_CHANNEL6_MEMORY_INC_MODE | 
										DMA_CHANNEL6_CIRCULAR_BUFFER | 
										DMA_CHANNEL6_TC_INT_ENABLE   | 
										DMA_CHANNEL6_ENABLE));
	
	//DMA1 channel 6 interrupt enable
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);
	
}

void ESP8266_Transmit(char* pData)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	USART_Transmit_String_With_Null(USART2,pData);
}

uint8_t ESP8266_Receive_Status(void)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	uint8_t state;
	
	if (!strcmp(esp8266RxBuffer,"SSID->OK ")){
		memset(esp8266RxBuffer,'\0',ESP8266_RX_BUFFER_SIZE - 1);
		state = SSID_RECEIVED;
	}
	
	else if (!strcmp(esp8266RxBuffer,"PSWD->OK ")){
		memset(esp8266RxBuffer,'\0',ESP8266_RX_BUFFER_SIZE - 1);
		state = PASSWORD_RECEIVED;
	}
	
	else if (!strcmp(esp8266RxBuffer,"WIFI->OK ")){
		memset(esp8266RxBuffer,'\0',ESP8266_RX_BUFFER_SIZE - 1);
		state = WIFI_CONNECTED;
	}
	
	else if (!strcmp(esp8266RxBuffer,"DSCN->OK ")){
		memset(esp8266RxBuffer,'\0',ESP8266_RX_BUFFER_SIZE - 1);
		state = WIFI_DISCONNECTED;
	}
	
	else
	{
		state = WIFI_IDLE;
	}
	
	return state;
}
