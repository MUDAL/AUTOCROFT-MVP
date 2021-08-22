#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "uart.h"
#include "sysTimer_struct.h"
#include "dma.h"
#include "sim800l.h"

/*
The SIM800L module receives data in a specific format. To get the SMS
from the data received, the SMS has to be parsed with certain considerations
in mind.

for example, the data below was received from a SIM800L module.

>> "\r\n+CMT: \"+2348144086706\",\"\",\"21/03/08,19:21:49+04\"\r\nHello Hey Bye Good friends\n\r\n".

The actual SMS is 'Hello Hey Bye Good friends\n'. Assuming the sender has no name, the SMS will
start at index 50. The starting index of the sender's name (considering the quotation marks)
is 25. There is a need for at least 23 bytes to be added to the sender's starting index to
put the current index a few bytes from the SMS's starting index i.e. at least 2 indexes before the
SMS's starting index.

*/

#define INDEX_OF_SENDER_NAME      25
#define MIN_BYTES_TO_ADD    			23
#define STARTING_INDEX  					INDEX_OF_SENDER_NAME + MIN_BYTES_TO_ADD

#define SIM800L_DUMMY_BUFFER_SIZE	10

void SIM800L_Init(char* sim800lRxBuffer)
{	
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	//GPIO onfiguration (PA0) for SIM800L Ring pin input detection
	GPIO_Input_Init(GPIOA,
									GPIO_PORT_REG_LOW,
									GPIO_PIN0,
									GPIO_PIN0_INPUT_FLOATING,
									false);
	
	//GPIO configuration for USART3 Rx
	GPIO_Input_Init(GPIOB,
									GPIO_PORT_REG_HIGH,
									GPIO_PIN11,
									GPIO_PIN11_INPUT_PULLUP_OR_PULLDOWN,
									GPIO_PULLUP_ENABLE);
	
	//GPIO configuration for USART3 Tx
	GPIO_Output_Init(GPIOB,
									 GPIO_PORT_REG_HIGH,
									 GPIO_PIN10_OUTPUT_MODE_2MHZ,
									 GPIO_PIN10_ALT_FUNC_PUSH_PULL);
	
	//USART3 configuration
	USART_Init(USART3,
						 BAUD_9600,
						 USART_DMA_RX_ENABLE,
						(USART_TX_ENABLE | USART_RX_ENABLE));
	
	//DMA1 channel 3 configuration for USART3 Rx
	DMA_USART_Rx_Init(DMA1_Channel3,
										USART3,
										sim800lRxBuffer,
										SIM800L_DUMMY_BUFFER_SIZE, 
										DMA_CHANNEL3_MEMORY_INC_MODE);
}

void SIM800L_Transmit_Byte(char data)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	USART_Transmit_Byte(USART3, data);
}

void SIM800L_Transmit_String(char* pData)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	USART_Transmit_String(USART3, pData);
}

bool SIM800L_Receiver_Data_Ready(sysTimer_t* pTimer, uint32_t numberOfBytes)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	bool dataIsReady = false;
	
	if ( DMA_Receive(DMA1,
									 DMA1_Channel3,
							     DMA_CHANNEL3_CLEAR_TC_INT_FLAG,
							     numberOfBytes,
							     pTimer) )
	{
		dataIsReady = true;
	}
	
	return dataIsReady;
	
}

bool SIM800L_Is_Ringing(void)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	return !( GPIO_Input_Read(GPIOA,GPIO_PIN0) );
}

void SIM800L_Get_SMS(char* sim800lRxBuffer,
										 uint32_t sim800lRxBufferSize,
										 char* smsBuffer)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	uint32_t smsStartingIndex = 0;
	
	//Get starting index of SMS from the SIM800L data
	for (int i = STARTING_INDEX; i < sim800lRxBufferSize; i++)
	{
			if (sim800lRxBuffer[i] == '\n')
			{
				/*if a new line is encountered, store the index after (i.e.
				the starting index of the SMS).
				*/
				smsStartingIndex = i + 1;
				break;
			}
	}

	//Get SMS
	for (int j = smsStartingIndex; sim800lRxBuffer[j] != '\r'; j++)
	{
			smsBuffer[j - smsStartingIndex] = sim800lRxBuffer[j];
	}
	
}

