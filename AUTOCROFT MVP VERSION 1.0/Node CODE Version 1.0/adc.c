#include "stm32f10x.h"                  // Device header
#include "systick.h"
#include "adc.h"

void ADC_Init(ADC_TypeDef* adcPort,
							uint8_t channel,
							uint32_t sampleTimeReg)
{
	//channel selected as 1st conversion in sequence
	adcPort->SQR3 = channel; 
	//configure sample time
	if (channel <= 9)
	{
		adcPort->SMPR2 = sampleTimeReg; 
	}
	else if (channel > 9 && channel <= 15)
	{
		adcPort->SMPR1 = sampleTimeReg;
	}
	else
	{
		//Invalid input
		return;
	}
}

uint16_t ADC_Read(ADC_TypeDef* adcPort)
{
	adcPort->CR2 &= ~ADC_CR2_CONT;
	adcPort->CR2 |= ADC_CR2_ADON; 
	//stabilization delay
	SysTick_DelayMs(1);
	//turn ADC on again
	adcPort->CR2 |= ADC_CR2_ADON;
	//enable ADC calibration
	adcPort->CR2 |= ADC_CR2_CAL;
	//wait for calibration to be complete
	while((adcPort->CR2 & ADC_CR2_CAL) == ADC_CR2_CAL);
	//wait till end of conversion
	while((adcPort->SR & ADC_SR_EOC) != ADC_SR_EOC);
	volatile uint16_t adcData = adcPort->DR;
	adcPort->CR2 &= ~ADC_CR2_ADON;
	return adcData;
}	
