#include "stm32f10x.h"                  // Device header
#include "iwdg.h"

//IWDG default settings: 409.6ms timeout

void IWDG_Start(void)
{
	/*
	Description:
	Activates the independent watchdog peripheral
	
	Parameters:
	None
	
	Return:
	None
	
	*/
	IWDG->KR |= 0xCCCC; 
}

void IWDG_Refresh(void)
{
	/*
	Description:
	Reloads the independent watchdog's timer in order to
	prevent a system reset
	
	Parameters:
	None
	
	Return:
	None
	
	*/
	IWDG->KR |= 0xAAAA;
}	
