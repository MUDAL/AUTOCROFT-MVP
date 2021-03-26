#ifndef _SOLENOID_H
#define _SOLENOID_H

enum SolenoidState
{
	SOLENOID_ON = true,
	SOLENOID_OFF = false
};

extern void Solenoid_Init(void);
extern void Solenoid_Switch(bool state);

#endif //_SOLENOID_H
