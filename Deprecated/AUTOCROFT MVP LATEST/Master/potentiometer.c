#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "adc.h"
#include "potentiometer.h"

#define MAX_ADC		  				4095
#define MAX_PERCENT					100

void Potentiometer_Init(void)
{
	/*
	Description:
	Initializes interface for potentiometer which is used for setting
	parameters e.g. moisture thresholds.
	
	Parameters:
	None
	
	Return:
	None
	
	*/
	
	//Analog mode configuration for PB0
	GPIO_InputInit(GPIOB,
								 GPIO_PORT_REG_LOW,
								 GPIO_PIN0,
								 GPIO_ANALOG_MODE,
								 false);
	
	
	ADC_SingleConv_Init(ADC1,
											ADC_CHANNEL8,
											ADC_CH8_MAX_SAMPLE_TIME); 
											 
}

uint16_t Potentiometer_GetRawPosition(void)
{
	/*
	Description:
	Calculates position of potentiometer's knob.
	
	Parameters:
	None
	
	Return:
	1.) Position of knob i.e. raw ADC value (an 16-bit unsigned integer)
	*/
	
	return ADC_Read(ADC1);
}

uint8_t Potentiometer_GetPercentPosition(void)
{
	/*
	Description:
	Calculates position of potentiometer's knob in percentage
	format.
	
	Parameters:
	None
	
	Return:
	1.) percentage position of knob (an 8-bit unsigned integer)
	*/
	
	return ( ADC_Read(ADC1) * MAX_PERCENT ) / MAX_ADC;
	
}
