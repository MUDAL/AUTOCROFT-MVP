#ifndef _SOLENOID_H
#define _SOLENOID_H

#define SOLENOID_ON				true
#define SOLENOID_OFF			false

extern void Solenoid_Init(void);
extern void Solenoid_Switch(bool state);
extern bool Solenoid_IsRunning(void);

#endif //_SOLENOID_H
