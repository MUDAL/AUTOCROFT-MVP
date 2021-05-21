#ifndef _SYSTICK_H
#define _SYSTICK_H

extern void SysTick_Init(void);
extern uint32_t SysTick_GetTick(void);
extern void SysTick_DelayMs(uint32_t delayTime);
extern bool SysTick_DoneCounting(sysTimer_t* pTimer);

#endif //_SYSTICK_H
