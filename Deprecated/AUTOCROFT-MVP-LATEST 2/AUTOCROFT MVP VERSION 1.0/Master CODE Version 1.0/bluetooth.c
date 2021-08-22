#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "dma.h"
#include "uart.h"
#include "system.h"
#include "bluetooth.h"

void Bluetooth_Init(void)
{
	//GPIO configuration for USART3 Tx
	GPIO_OutputInit(GPIOB,
									GPIO_PORT_REG_HIGH,
									GPIO_PIN10_OUTPUT_MODE_2MHZ,
									GPIO_PIN10_ALT_FUNC_PUSH_PULL);
	//GPIO configuration for USART3 Rx
	GPIO_InputInit(GPIOB,
								 GPIO_PORT_REG_HIGH,
								 GPIO_PIN11,
								 GPIO_PIN11_INPUT_PULLUP_OR_PULLDOWN,
								 GPIO_PULLUP_ENABLE);
	//USART3 configuration
	USART_Init(USART3,
						 BAUD_9600,
						 RX_DMA_ENABLE,
						 (USART_TX_ENABLE | USART_RX_ENABLE));
}

void Bluetooth_RxBufferInit(uint8_t* pBuffer, uint8_t bufferSize)
{
	//DMA1 channel 3 configuration for USART3 Rx
	DMA_USART_Rx_Init(DMA1_Channel3,
										USART3,
										pBuffer,
										bufferSize, 
										DMA_CHANNEL3_MEMORY_INC_MODE |
										DMA_CHANNEL_ENABLE);
}

uint8_t Bluetooth_RxStatus(void)
{
	uint8_t rxStatus = NO_DATA_RECEIVED;
	
	if(DMA_Rx_BufferFull(DMA1,DMA_CHANNEL3))
	{
		rxStatus = COMPLETE_RX_DATA;
	}
	if(USART_RxIdleLineDetected(USART3))
	{
		rxStatus = INCOMPLETE_RX_DATA;
	}
	return rxStatus;
}

uint8_t Bluetooth_NumberOfBytesReceived(void)
{
	return BLUETOOTH_RX_MAX_LEN - DMA_Rx_CNDTR(DMA1_Channel3);
}

