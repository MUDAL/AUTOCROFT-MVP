#ifndef _SYSTIMER_STRUCT_H
#define _SYSTIMER_STRUCT_H

typedef struct
{
	uint32_t start; 
	uint32_t ticksToWait; 
	bool isCounting;
}sysTimer_t;


#endif //_SYSTIMER_STRUCT_H
