#ifndef _SYSTEM_H
#define _SYSTEM_H

extern void System_Init(void);
extern void System_TimerDelayMs(uint32_t delayTime);
extern void System_TimerInit(sysTimer_t* pSysTimer, uint32_t timerRepTime);
extern bool System_TimerDoneCounting(sysTimer_t* pSysTimer);
extern void System_ClearStandbyFlag(void);
extern void System_GoToStandbyMode(void);

#endif //_SYSTEM_H
