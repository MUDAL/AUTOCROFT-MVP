#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "clocks.h"
#include "gpio.h"
#include "pwr.h"
#include "systick.h"
#include "system.h"

void System_Init(void)
{
	Clocks_Init();
	SysTick_Init();
	GPIO_Reset();
}

void System_Reset(void)
{
	NVIC_SystemReset();
}

void System_TimerDelayMs(uint32_t delayTime)
{
	SysTick_DelayMs(delayTime);
}

void System_TimerInit(sysTimer_t* pSysTimer, uint32_t timerRepTime)
{
	pSysTimer->start = 0;
	pSysTimer->isCounting = false;
	pSysTimer->ticksToWait = timerRepTime;
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
		if ((SysTick_GetTick() - pSysTimer->start) >= pSysTimer->ticksToWait)
		{
			countingComplete = true;
			pSysTimer->start = 0;
			pSysTimer->isCounting = false;
		}
	}
	return countingComplete;
}

void System_ClearStandbyFlag(void)
{
	PWR_ClearStandbyFlag();
}

void System_GoToStandbyMode(void)
{
	PWR_GoToStandbyMode();
}
