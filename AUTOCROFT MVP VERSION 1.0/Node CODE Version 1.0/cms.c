#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "adc.h"
#include "cms.h"

#define MIN_PERCENT_MOISTURE				0
#define MAX_PERCENT_MOISTURE				100
#define ADC_VALUE_AIR_3V3						2780
#define ADC_VALUE_WATER_3V3					1000

void CMS_Init(void)
{
	/*
	Description:
	Initializes interface for capacitive moisture
	sensor.
	
	Parameters:
	None
	
	Return:
	None
	
	*/
	ADC_SingleConv_Init(ADC1,
											ADC_CHANNEL5,
											ADC_CH5_MAX_SAMPLE_TIME); 
}

uint8_t CMS_GetMoisture(void)
{
	/*
	Description:
	Obtains percentage moisture measurement of the
	capacitive moisture sensor
	
	Parameters:
	None
	
	Return:
	1.) percentage moisture (an 8-bit unsigned integer)
	*/
	
	uint8_t moisture;
	uint16_t adcVal = ADC_Read(ADC1);
	
	if (adcVal >= ADC_VALUE_AIR_3V3)
	{
		moisture = MIN_PERCENT_MOISTURE;
	}
	
	else if (adcVal <= ADC_VALUE_WATER_3V3)
	{
		moisture = MAX_PERCENT_MOISTURE;
	}
	
	else
	{
		moisture = ( (adcVal - ADC_VALUE_AIR_3V3) * MAX_PERCENT_MOISTURE ) / 
							 ( ADC_VALUE_WATER_3V3 - ADC_VALUE_AIR_3V3 );
	}
	
	return moisture;
}
