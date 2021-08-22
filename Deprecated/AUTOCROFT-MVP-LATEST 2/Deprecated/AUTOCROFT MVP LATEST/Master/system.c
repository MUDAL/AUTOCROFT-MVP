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
	GPIO_Reset();
}

void System_TimerDelayMs(uint32_t delayTime)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	SysTick_DelayMs(delayTime);
}

void System_TimerInit(sysTimer_t* pSysTimer, uint32_t timerRepTime)
{
	pSysTimer->start = 0;
	pSysTimer->isCounting = false;
	pSysTimer->ticksToWait = timerRepTime;
}

uint32_t System_TimerGetTick(void)
{
	return SysTick_GetTick();
}

bool System_TimerDoneCounting(sysTimer_t* pSysTimer)
{
	bool countingComplete = false;
	
	if (!pSysTimer->isCounting)
	{
		pSysTimer->start = SysTick_GetTick();
		pSysTimer->isCounting = true;
	}
	
	else
	{
		if ( SysTick_DoneCounting(pSysTimer) )
		{
			countingComplete = true;
			pSysTimer->start = 0;
			pSysTimer->isCounting = false;
		}
	}
	
	return countingComplete;
}
