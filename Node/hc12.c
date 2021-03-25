#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "uart.h"
#include "sysTimer_struct.h"
#include "systick.h"
#include "dma.h"
#include "hc12.h"

#define HC12_RX_TIMEOUT_MS				5

static sysTimer_t privHc12RxTimer;

void HC12_Init(char* hc12RxBuffer)
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
										hc12RxBuffer,
										HC12_RX_BUFFER_SIZE, 
										DMA_CHANNEL5_MEMORY_INC_MODE);
	
//Initialize alarm for HC12 module
	privHc12RxTimer.start = 0;
	privHc12RxTimer.ticksToWait = HC12_RX_TIMEOUT_MS;
	privHc12RxTimer.isCounting = false;
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

bool HC12_Rx_Done_Receiving(void)
{
	
	bool RxDoneReceiving = false;
	
	if (!privHc12RxTimer.isCounting)
	{
		if (DMA_Rx_Is_Receiving(DMA1,
													  DMA1_Channel5,
													  DMA_CHANNEL5_CLEAR_TC_INT_FLAG,
													  HC12_RX_BUFFER_SIZE))
		{
			privHc12RxTimer.start = SysTick_GetTick();
			privHc12RxTimer.isCounting = true;
		}
	}
	
	else
	{
		if ( SysTick_Timer_Done_Counting(&privHc12RxTimer) )
		{
			DMA_Rx_Restart(DMA1,
										 DMA1_Channel5,
										 DMA_CHANNEL5_CLEAR_TC_INT_FLAG);
			
			privHc12RxTimer.start = 0;
			privHc12RxTimer.isCounting = false;
			RxDoneReceiving = true;
		}
	}
	
	return RxDoneReceiving;
	
}

