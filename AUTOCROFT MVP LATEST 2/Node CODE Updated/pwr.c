#include "stm32f10x.h"                  // Device header
#include "pwr.h"

/**
@brief Clears standby flag of PWR module. The flag must be cleared  
after waking the MCU from sleep.  
@param None    
@return None    
*/
void PWR_ClearStandbyFlag(void)
{
	if ((PWR->CSR & PWR_CSR_SBF) == PWR_CSR_SBF) //check standby flag
	{
		PWR->CR |= PWR_CR_CSBF; //clear standby flag
	}
}

/**
@brief Puts the MCU to standby mode (a low power state).  
@param None  
@return None  
*/
void PWR_GoToStandbyMode(void)
{
	PWR->CSR &= ~PWR_CSR_EWUP; //disable wakeup pin
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; 
	PWR->CR |= PWR_CR_PDDS; //enter standby mode when CPU enters deep sleep
	PWR->CR |= PWR_CR_CWUF; //clear wakeup flag
	PWR->CSR |= PWR_CSR_EWUP; //enable wakeup pin
	__WFI();
}
