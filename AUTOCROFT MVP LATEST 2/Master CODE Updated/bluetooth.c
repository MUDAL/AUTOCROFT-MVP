#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "dma.h"
#include "uart.h"
#include "system.h"
#include "bluetooth.h"

#define BLUETOOTH_RX_MAX_TIME_MS		100
static sysTimer_t bluetoothRxTimer;

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
	
	System_TimerInit(&bluetoothRxTimer,BLUETOOTH_RX_MAX_TIME_MS);
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

bool Bluetooth_DataIsReady(uint8_t* pBuffer)
{
	bool dataIsReady = false;
	
	if(pBuffer[0] != 0)
	{//this indicates first byte of bluetooth data has been received.
		if(System_TimerDoneCounting(&bluetoothRxTimer))
		{//all bytes must have been received after the programmed timeout.
			dataIsReady = true;
		}
	}
	return dataIsReady;
}

uint8_t Bluetooth_NumberOfBytesReceived(void)
{
	return BLUETOOTH_RX_MAX_LEN - DMA_Rx_CNDTR(DMA1_Channel3);
}

/**
@brief Prepares the bluetooth Rx buffer for new data      
reception from starting address of the buffer.  
@param
@param
@return None
*/
void Bluetooth_ClearRxBuffer(uint8_t* pBuffer)
{
	pBuffer[0] = 0;
	DMA_Rx_ReInit(DMA1_Channel3, BLUETOOTH_RX_MAX_LEN);
}

