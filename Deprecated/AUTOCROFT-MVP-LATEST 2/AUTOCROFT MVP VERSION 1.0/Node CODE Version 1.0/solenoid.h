#ifndef _SOLENOID_H
#define _SOLENOID_H

#define SOLENOID_ON				true
#define SOLENOID_OFF			false

extern void Solenoid_Init(void);
extern void Solenoid_Control(bool state);
extern bool Solenoid_IsOn(void);

#endif //_SOLENOID_H
