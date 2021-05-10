#ifndef _DMA_H
#define _DMA_H

#define DMA_CHANNEL1											1
#define DMA_CHANNEL2											2
#define DMA_CHANNEL3											3
#define DMA_CHANNEL4											4
#define DMA_CHANNEL5											5
#define DMA_CHANNEL6											6
#define DMA_CHANNEL7											7

#define DMA_CHANNEL3_MEMORY_INC_MODE			DMA_CCR3_MINC

#define DMA_CHANNEL5_MEMORY_INC_MODE			DMA_CCR5_MINC
#define DMA_CHANNEL5_CIRCULAR_BUFFER			DMA_CCR5_CIRC
#define DMA_CHANNEL5_ENABLE								DMA_CCR5_EN

#define DMA_CHANNEL6_MEMORY_INC_MODE			DMA_CCR6_MINC
#define DMA_CHANNEL6_CIRCULAR_BUFFER			DMA_CCR6_CIRC
#define DMA_CHANNEL6_ENABLE								DMA_CCR6_EN

//Initializes the DMA channel for a desired USART
extern void DMA_USART_Rx_Init(DMA_Channel_TypeDef* dmaChannel, 
															USART_TypeDef* uartPort, 
															char* uartRxBuffer, 
															uint32_t bufferSize, 
															uint32_t dmaConfig);

//Returns true if DMA Rx buffer is full 																														
extern bool DMA_Rx_BufferFull(DMA_TypeDef* dmaPort, uint8_t dmaChannel);



#endif //_DMA_H
