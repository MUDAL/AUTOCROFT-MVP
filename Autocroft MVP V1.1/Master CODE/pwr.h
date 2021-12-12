#ifndef _PWR_H
#define _PWR_H

extern void PWR_ClearStandbyFlag(void);
/*Requires PA0 wakeup pin which isn't supported by current Autocroft circuit*/
extern void PWR_GoToStandbyMode(void);
extern void PWR_GoToStopMode(void);

#endif //_PWR_H
