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
	//enable clock access for GPIOA, GPIOB, USART1, ADC1, ADC2
	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN 	| 
									 RCC_APB2ENR_IOPBEN 	| 
									 RCC_APB2ENR_USART1EN | 
									 RCC_APB2ENR_ADC1EN   |
									 RCC_APB2ENR_ADC2EN);
	//enable clock for I2C1
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
}
