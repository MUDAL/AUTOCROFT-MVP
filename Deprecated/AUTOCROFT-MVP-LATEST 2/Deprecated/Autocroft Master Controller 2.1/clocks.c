#include "stm32f10x.h"                  // Device header
#include "clocks.h"

void Clocks_Init(void)
{
	/*
	Description:
	Initializes all required peripheral clocks.
	
	Parameters:
	None
	
	Return:
	None
	
	*/
	
	while( (RCC->CR & RCC_CR_HSIRDY) != RCC_CR_HSIRDY ); //wait for internal oscillator to be stable
	RCC->CFGR &= ~RCC_CFGR_SW; //use internal oscillator as system clock
	RCC->CFGR &= ~RCC_CFGR_PPRE1; //No APB1 prescaler, run it at 8MHz
	RCC->CR &= ~RCC_CR_PLLON;	//disable PLL
	RCC->CR &= ~RCC_CR_HSEON; //disable external oscillator
	//enable clock for DMA1
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	//enable clock access for GPIOA, GPIOB and USART1
	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_USART1EN);
	//enable clock for USART2, USART3
	RCC->APB1ENR |= (RCC_APB1ENR_USART2EN  | RCC_APB1ENR_USART3EN);
}
