#ifndef _IRRIGATION_H
#define _IRRIGATION_H

typedef enum 
{
	
	NO_WATERING = 0,
	LIGHT_WATERING,
	HEAVY_WATERING
	
}irrigMethod_t;

typedef struct
{
	
	uint32_t minValue;
	uint32_t maxValue;
	
}irrig_t;

extern irrigMethod_t Irrigation_Get_Method(sensorLevel_t moisture,
																		       sensorLevel_t humidity,
																		       sensorLevel_t temperature);

extern void Irrigation_Set_Time(uint32_t irrigTime);

#endif //_IRRIGATION_H

