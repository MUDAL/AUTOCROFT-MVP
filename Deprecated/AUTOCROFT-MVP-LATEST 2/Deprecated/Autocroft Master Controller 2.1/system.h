#ifndef _SYSTEM_H
#define _SYSTEM_H

//Initialize system functions (clocks,system peripherals etc.)
extern void System_Init(void);
/*
Initialize software timer ('sysTimer_t' struct variable). All
software timers depend on the system timer which would have been
initialized using 'System_Init()'. To create a software timer,
do the following:

>>sysTimer_t mySoftwareTimer; //create software timer.

To initialize the software timer, do the following:

>>System_Timer_Init(&mySoftwareTimer,1000);

//The value '1000' above is the repTime (repetition time) 
in milliseconds i.e. mySoftwareTimer reloads every 1000ms 

*/
extern void System_Software_Timer_Init(sysTimer_t* pTimer, uint32_t repTime);
//Gets current count or tick of the system timer
extern uint32_t System_Timer_GetTick(void);
//Creates a delay in milliseconds using the system timer
extern void System_Timer_DelayMs(uint32_t delayTime);
/*
Returns 'true' if a software timer is done counting and
'false' if otherwise.
*/
extern bool System_Software_Timer_Done_Counting(sysTimer_t* pTimer);

#endif //_SYSTEM_H
