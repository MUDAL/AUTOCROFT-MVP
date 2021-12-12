#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "clocks.h"
#include "gpio.h"
#include "i2c.h"
#include "afio.h"
#include "exti.h"
#include "pwr.h"
#include "systick.h"
#include "system.h"

void System_Init(void)
{
	Clock_HSI_8MHz_Init();
	SysTick_Init();
	GPIO_Reset();
	GPIO_OutputInit(GPIOB,
									GPIO_PORT_REG_LOW,
								 (GPIO_PIN6_OUTPUT_MODE_2MHZ | GPIO_PIN7_OUTPUT_MODE_2MHZ),
								 (GPIO_PIN6_ALT_FUNC_OPEN_DRAIN | GPIO_PIN7_ALT_FUNC_OPEN_DRAIN));
	
	I2C_Init(I2C1,
					 I2C_PERIPH_FREQ_8MHZ,
					 I2C_STANDARD_MODE_8MHZ_CCR, 
					 I2C_STANDARD_MODE_8MHZ_TRISE);		
	//GPIOA EXTI configuration for exiting stop mode
	GPIO_InputInit(GPIOA,
								 GPIO_PORT_REG_LOW,
								 GPIO_PIN0,
								 GPIO_PIN0_INPUT_PULLUP_OR_PULLDOWN,
								 true);
	AFIO_Init(EXTI_LINE0,AFIO_EXTI_SELECT_PA0);
	EXTI_Init(EXTI_LINE0,EXTI_FALLING_EDGE);
	NVIC_EnableIRQ(EXTI0_IRQn);
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

void System_GoToStopMode(void)
{
	PWR_GoToStopMode();
}
