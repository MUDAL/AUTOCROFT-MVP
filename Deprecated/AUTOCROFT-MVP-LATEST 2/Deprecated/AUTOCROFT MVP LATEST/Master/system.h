#ifndef _SYSTEM_H
#define _SYSTEM_H

extern void System_Init(void);
extern void System_TimerDelayMs(uint32_t delayTime);
extern void System_TimerInit(sysTimer_t* pSysTimer, uint32_t timerRepTime);
extern uint32_t System_TimerGetTick(void);
extern bool System_TimerDoneCounting(sysTimer_t* pSysTimer);

#endif //_SYSTEM_H
