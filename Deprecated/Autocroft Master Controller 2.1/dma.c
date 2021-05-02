#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "sysTimer_struct.h"
#include "systick.h"
#include "dma.h"

//Generic DMA Channel enable
#define  DMA_CCR_EN		((uint16_t)0x0001)

void DMA_USART_Rx_Init(DMA_Channel_TypeDef* dmaChannel,
											 USART_TypeDef* uartPort,
											 char* uartRxBuffer, 
											 uint32_t bufferSize,
											 uint32_t dmaConfig)
{
	/*
	Description:
	Initializes a USART Rx DMA channel
	
	Parameters:
	1.) dmaChannel: pointer to DMA_Channel_TypeDef struct which contains
	all registers for the desired DMA channel
	e.g. if dmaChannel is passed an argument of DMA1_Channel5, DMA1 Channel 5
	is configured and if passed an argument of DMA2_Channel1, DMA2 Channel 1
	is configured.
	
	2.) uartPort: pointer to USART_TypeDef struct which contains
	all registers for the desired USART peripheral
	e.g. if uartPort is passed an argument of USART1, the USART1 peripheral
	is configured and if passed an argument of USART2, the USART2 peripheral
	is configured etc.
	
	3.) uartRxBuffer: pointer to first element of character array (or buffer) which stores
	data from the configured USART's receiver via the configured DMA channel. e.g. 
	if dmaChannel = DMA1_Channel5 and uartPort = USART1, USART1 receives 
	multiple byte data which will be directly transferred to memory(i.e. uartRxBuffer) 
	via DMA1 channel 5.
	
	4.) bufferSize: size of uartRxBuffer (i.e. number of bytes or characters to receive from USART Rx
	via DMA transfer). e.g. if this parameter is passed with an argument of 25, it means 25 bytes
	of data will be directly transferred from the USART Rx to memory (i.e. uartRxBuffer).
	
	5.) dmaConfig: this parameter enables different types of configurations for a DMA channel.
	The possible arguments that can be passed to this parameter are defined in the dma.h header
	and they can be logically ORed depending on the desired configuration. Some of these 
	arguments (in generic form) include:
	
	- DMA_CHANNELX_MEMORY_INC_MODE: Enables memory increment mode for channel 'x' of
	a DMA peripheral
	
	- DMA_CHANNELX_CLEAR_TC_INT_FLAG: Clears transfer conplete interrupt flag for channel 'x' of
	a DMA peripheral
	
	- DMA_CHANNELX_CIRCULAR_BUFFER: Enables circular buffer mode for channel 'x' of a DMA peripheral
	
	- DMA_CHANNELX_TC_INT_ENABLE: Enables transfer complete interrupt for channel 'x' of a DMA
	peripheral
	
	- DMA_CHANNELX_ENABLE	: Enables channel 'x' of a DMA peripheral
	
	Return:
	None
	
	*/ 
	volatile uint32_t* pUart_DR = (uint32_t*)&uartPort->DR;
	
	dmaChannel->CPAR = (uint32_t)pUart_DR;
	dmaChannel->CMAR = (uint32_t)uartRxBuffer;
	dmaChannel->CNDTR = bufferSize;
	dmaChannel->CCR |= dmaConfig;
}

bool DMA_Receive(DMA_TypeDef* dmaPort, 
								 DMA_Channel_TypeDef* dmaChannel,
								 uint32_t dmaClearInterruptFlag,
								 uint32_t numberOfBytesToReceive, 
								 sysTimer_t* pDmaTimer )

{
	/*
	Description:
	Transfers multiple bytes of data from peripheral directly to
	memory (or SRAM)
	
	Parameters:
	1.) dmaPort: pointer to DMA_TypeDef struct which contains
	all registers for the desired DMA peripheral
	e.g. if dmaPort is passed an argument of DMA1, the DMA1 peripheral
	is configured and if passed an argument of DMA2, the DMA2 peripheral
	is configured.
	
	2.) dmaChannel: pointer to DMA_Channel_TypeDef struct which contains
	all registers for the desired DMA channel
	e.g. if dmaChannel is passed an argument of DMA1_Channel5, DMA1 Channel 5
	is configured and if passed an argument of DMA2_Channel1, DMA2 Channel 1
	is configured.
	
	3.) dmaClearInterruptFlag: clears the transfer complete interrupt flag of a
	DMA channel. e.g. if passed with DMA_CHANNELX_CLEAR_TC_INT_FLAG, the transfer
	complete interrupt flag for channel 'x' of a DMA peripheral will be cleared.
	
	4.) numberOfBytesToReceive: number of bytes to be transferred from peripheral
	directly to memory. The DMA works in such a way that the number of bytes to
	be received from the peripheral have to be known beforehand or else there might 
	be issues e.g. if the number of bytes programmed into the DMA channel register is
	10 bytes and the DMA channel only transfers 4 bytes, the channel will be stuck in
	an infinite loop (if polling is used) waiting for the transfer of the remaining 6
	bytes. The 5th parameter of this function address this case whereby the number of
	bytes to be transferred isn't known and ends up being less than the value programmed
	into the DMA channel register.
	
	5.) pDmaTimer: pointer to a software timer of type 'sysTimer_t' used to handle timing
	operations for the configured DMA peripheral. The software timer allocates time for  
	all data to be received by memory from peripheral. Once this time elapses, the DMA channel
	is disabled whether the CNDTR register reaches 0 or not.
	For instance, if CNDTR = 30 i.e. we're assuming 30 bytes of data will be received, if only 
	15 bytes of data are received, the DMA will be stuck in an infinite loop as it would keep waiting
	for reception of the remaining 15 bytes.
	To prevent the waiting for data reception from blocking other operations, the software timer allocates
	time for which all data is expected to be received. Once the time elapses, a 'dataReceived' signal is set
	and the data received via DMA before the timeout can be used by the main applicatiom.
	
	Return:
	1.) true: if all data expected to be received have been received and are ready for use by other
	parts of the program.
	2.) false: if all expected data haven't been read or the software timer isn't done counting hence
	received data aren't ready to be used by other parts of the program.
	
	*/
	bool dataReceived = false;

	if (pDmaTimer->start == 0)
	{
		dmaPort->IFCR |= dmaClearInterruptFlag;
		dmaChannel->CNDTR = numberOfBytesToReceive;
		dmaChannel->CCR |= DMA_CCR_EN; 
		
		if (dmaChannel->CNDTR != numberOfBytesToReceive)
		{ //if DMA receives data, start timer
			pDmaTimer->start = SysTick_GetTick();
			pDmaTimer->isCounting = true;
		}
		
	}
	
	if (pDmaTimer->isCounting)
	{
		if (SysTick_Timer_Done_Counting(pDmaTimer))
		{
			dataReceived = true;
			dmaChannel->CCR &= ~DMA_CCR_EN;
			dmaPort->IFCR |= dmaClearInterruptFlag;
			dmaChannel->CNDTR = 0;
			pDmaTimer->start = 0;
			pDmaTimer->isCounting = false;
		}
	}
	
	return dataReceived;
}

void DMA1_Channel6_IRQHandler(void)
{
	/*
	Description:
	DMA1 channel 6 interrupt service routine.
	
	Parameters:
	None
	
	Return:
	None
	
	*/
	
	DMA1->IFCR |= DMA_IFCR_CTCIF6;
}
