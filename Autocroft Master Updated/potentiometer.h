#ifndef _POTENTIOMETER_H
#define _POTENTIOMETER_H

extern void Potentiometer_Init(void);
extern uint16_t Potentiometer_Get_RawPosition(void);
extern uint8_t Potentiometer_Get_PercentPosition(void);

#endif //_POTENTIOMETER_H
