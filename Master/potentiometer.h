#ifndef _POTENTIOMETER_H
#define _POTENTIOMETER_H

extern void Potentiometer_Init(void);
extern uint16_t Potentiometer_GetRawPosition(void);
extern uint8_t Potentiometer_GetPercentPosition(void);

#endif //_POTENTIOMETER_H
