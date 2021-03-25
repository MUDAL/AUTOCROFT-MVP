#ifndef _DMA_H
#define _DMA_H

#define DMA_CHANNEL3_MEMORY_INC_MODE			DMA_CCR3_MINC
#define DMA_CHANNEL3_CLEAR_TC_INT_FLAG		DMA_IFCR_CTCIF3

#define DMA_CHANNEL5_MEMORY_INC_MODE			DMA_CCR5_MINC
#define DMA_CHANNEL5_CLEAR_TC_INT_FLAG		DMA_IFCR_CTCIF5

#define DMA_CHANNEL6_MEMORY_INC_MODE			DMA_CCR6_MINC
#define DMA_CHANNEL6_CIRCULAR_BUFFER			DMA_CCR6_CIRC
#define DMA_CHANNEL6_TC_INT_ENABLE				DMA_CCR6_TCIE
#define DMA_CHANNEL6_ENABLE								DMA_CCR6_EN

//Initializes the DMA channel for a desired USART
extern void DMA_USART_Rx_Init(DMA_Channel_TypeDef* dmaChannel, 
															USART_TypeDef* uartPort, 
															char* uartRxBuffer, 
															uint32_t bufferSize, 
															uint32_t dmaConfig);

/*Enables Configured DMA channel to receive incoming data
to a peripheral. Returns 'true' if the DMA has started receiving data. 															
*/															
extern bool DMA_Rx_Is_Receiving(DMA_TypeDef* dmaPort, 
																DMA_Channel_TypeDef* dmaChannel,
																uint32_t dmaClearInterruptFlag,
																uint32_t numberOfBytesToReceive);

extern void DMA_Rx_Restart(DMA_TypeDef* dmaPort, 
													 DMA_Channel_TypeDef* dmaChannel,
													 uint32_t dmaClearInterruptFlag);

#endif //_DMA_H
