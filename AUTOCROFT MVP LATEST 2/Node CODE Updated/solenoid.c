#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "solenoid.h"

void Solenoid_Init(void)
{
	/*
	Description:
	Initializes the Solenoid valve interface
	
	Parameters:
	None
	
	Return:
	None
	
	*/
	
	//GPIO configuration for solenoid valve pin
	GPIO_OutputInit(GPIOA,
									GPIO_PORT_REG_LOW,
									GPIO_PIN7_OUTPUT_MODE_2MHZ,
									GPIO_GEN_PUR_OUTPUT_PUSH_PULL);
	
	//Ensure solenoid valve pin is 'low' before any...
	//application uses other Solenoid APIs.
	GPIO_OutputWrite(GPIOA, GPIO_PIN7, false);
}

void Solenoid_Control(bool state)
{
	/*
	Description:
	Switches solenoid valve on or off
	
	Parameters:
	1.) state: if this parameter is passed 'true' or SOLENOID_ON, the solenoid
	valve will be turned on else if it is passed 'false' or SOLENOID_OFF, 
	the solenoid valve will be turned off.
	
	Return:
	None
	
	*/
	
	GPIO_OutputWrite(GPIOA, GPIO_PIN7, state);
}

bool Solenoid_IsOn(void)
{
	/*
	Description:
	Parameters:
	Return:
	*/
	return GPIO_OutputRead(GPIOA, GPIO_PIN7);
}
