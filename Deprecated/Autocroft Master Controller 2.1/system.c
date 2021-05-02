#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "clocks.h"
#include "gpio.h"
#include "sysTimer_struct.h"
#include "systick.h"
#include "system.h"

void System_Init(void)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	Clocks_Init();
	SysTick_Init();
	GPIO_Reset(GPIOA);
	GPIO_Reset(GPIOB);
}

void System_Software_Timer_Init(sysTimer_t* pTimer, uint32_t repTime)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	pTimer->start = 0;
	pTimer->isCounting = false;
	pTimer->ticksToWait = repTime;
}

uint32_t System_Timer_GetTick(void)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	return SysTick_GetTick();
}

void System_Timer_DelayMs(uint32_t delayTime)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	SysTick_DelayMs(delayTime);
}

bool System_Software_Timer_Done_Counting(sysTimer_t* pTimer)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	bool countingComplete = false;
	
	if (pTimer->start == 0)
	{
		pTimer->start = SysTick_GetTick();
		pTimer->isCounting = true;
	}
	
	if (pTimer->isCounting)
	{
		if ( SysTick_Timer_Done_Counting(pTimer) )
		{
			countingComplete = true;
			pTimer->start = 0;
			pTimer->isCounting = false;
		}
	}
	
	return countingComplete;
}
